/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "SmfParser.h"

#include <string.h>

#include "path_util.h"
#include "midi_util.h"

// #define DEBUG (1)
// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#ifdef DEBUG
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else  // DEBUG
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // DEBUG

static const char kClassName[] = "SmfParser";

static const int kBufSize = 256;
static const int kMaxTrack = 32;
static const int kChunkSize = 4;

struct MThdChunk {
    char chunk_type[kChunkSize];  // 4Bytes
    uint32_t length;              // 4Bytes
    uint16_t format;              // 2Bytes
    uint16_t ntrks;               // 2Bytes
    uint16_t division;            // 2Bytes
};

struct MTrkChunk {
    char chunk_type[kChunkSize];  // 4Bytes
    uint32_t length;              // 4Bytes
};

static bool parseMThd(File& file, MThdChunk* mthd) {
    char chunk[kChunkSize + 1];
    for (int i = 0; i < kChunkSize; i++) {
        chunk[i] = file.read();
    }
    chunk[kChunkSize] = '\0';
    memcpy(mthd->chunk_type, chunk, kChunkSize);
    if (memcmp(mthd->chunk_type, "MThd", kChunkSize) != 0) {
        error_printf("[%s::%s] unexpected MThd chunk: %s\n", kClassName, __func__, chunk);
        return false;
    }

    mthd->length = 0;
    for (size_t i = 0; i < sizeof(mthd->length); i++) {
        mthd->length = (mthd->length << 8) | (file.read() & 0xFF);
    }
    mthd->format = 0;
    for (size_t i = 0; i < sizeof(mthd->format); i++) {
        mthd->format = (mthd->format << 8) | (file.read() & 0xFF);
    }
    mthd->ntrks = 0;
    for (size_t i = 0; i < sizeof(mthd->ntrks); i++) {
        mthd->ntrks = (mthd->ntrks << 8) | (file.read() & 0xFF);
    }
    mthd->division = 0;
    for (size_t i = 0; i < sizeof(mthd->division); i++) {
        mthd->division = (mthd->division << 8) | (file.read() & 0xFF);
    }

    trace_printf("[%s::%s] chunk:%s length:%d format:%d truck:%d division:%d\n", kClassName, __func__, chunk, mthd->length, mthd->format, mthd->ntrks,
                 mthd->division);
    if (mthd->division & 0x8000) {
        error_printf("[%s::%s] This MIDI file is not supported.\n", kClassName, __func__);
        return false;
    }

    return true;
}

static bool parseMTrk(File& file, MTrkChunk* mtrk) {
    char chunk[kChunkSize + 1];
    for (int i = 0; i < kChunkSize; i++) {
        chunk[i] = file.read();
    }
    chunk[kChunkSize] = '\0';
    memcpy(mtrk->chunk_type, chunk, kChunkSize);
    if (memcmp(mtrk->chunk_type, "MTrk", kChunkSize) != 0) {
        error_printf("[%s::%s] unexpected MTrk chunk: %s\n", kClassName, __func__, chunk);
        return false;
    }

    mtrk->length = 0;
    for (size_t i = 0; i < sizeof(mtrk->length); i++) {
        mtrk->length = (mtrk->length << 8) | (file.read() & 0xFF);
    }

    trace_printf("[%s::%s] chunk:%s length:%d\n", kClassName, __func__, chunk, mtrk->length);

    return true;
}

static uint32_t parseVariableLength(SmfParser::TrackReader& reader) {
    uint32_t length = 0;
    int ch = 0x00;
    do {
        if (!reader.available()) {
            error_printf("[%s::%s] Error:End Of file.\n", kClassName, __func__);
            break;
        }
        ch = reader.read();
        length = (length << 7) | (ch & 0x7F);
    } while (ch & 0x80);
    return length;
}

SmfParser::TrackReader::TrackReader() : file_(nullptr), file_pos_(0), track_offset_(0), track_size_(0), buf_(nullptr), buf_size_(0), buf_pos_(0) {
}

SmfParser::TrackReader::TrackReader(File* file, uint32_t offset, uint32_t size)
    : file_(file), file_pos_(offset), track_offset_(offset), track_size_(size), buf_(new uint8_t[kBufSize]), buf_size_(0), buf_pos_(0) {
}

SmfParser::TrackReader::TrackReader(const TrackReader& rhs)
    : file_(rhs.file_),
      file_pos_(rhs.file_pos_),
      track_offset_(rhs.track_offset_),
      track_size_(rhs.track_size_),
      buf_(new uint8_t[kBufSize]),
      buf_size_(0),
      buf_pos_(0) {
}

SmfParser::TrackReader::~TrackReader() {
    if (buf_) {
        delete[] buf_;
        buf_ = nullptr;
    }
}

size_t SmfParser::TrackReader::available() {
    size_t seg_pos = file_pos_ - track_offset_ + buf_pos_;
    return track_size_ - seg_pos;
}

boolean SmfParser::TrackReader::reset() {
    file_pos_ = track_offset_;
    buf_size_ = 0;
    buf_pos_ = 0;
    return true;
}

int SmfParser::TrackReader::read(void) {
    size_t seg_pos = file_pos_ - track_offset_ + buf_pos_;
    if (file_ == nullptr) {
        return -1;
    }
    if (track_size_ <= seg_pos) {
        return -1;
    }
    if (buf_ == nullptr) {
        return -1;
    }

    if (buf_size_ <= buf_pos_) {
        // read next segment
        file_pos_ += buf_size_;
        file_->seek(file_pos_);
        size_t readable_size = (file_->available() < kBufSize) ? file_->available() : kBufSize;
        int read_size = file_->read(buf_, readable_size);
        if (read_size < 0) {
            return -1;
        }
        buf_size_ = read_size;
        buf_pos_ = 0;
    }

    return buf_[buf_pos_++];
}

SmfParser::TrackParser::TrackParser() : msg(), reader_(), is_registered_(false), running_status_(0x00), at_eot_(true) {
}

SmfParser::TrackParser::TrackParser(File* file, size_t offset, size_t size)
    : msg(), reader_(file, offset, size), is_registered_(false), running_status_(0x00), at_eot_(false) {
}

SmfParser::TrackParser::TrackParser(const TrackParser& rhs) : msg(), reader_(rhs.reader_), is_registered_(false), running_status_(0x00), at_eot_(rhs.at_eot_) {
}

bool SmfParser::TrackParser::available() {
    return is_registered_;
}

bool SmfParser::TrackParser::discard() {
    is_registered_ = false;
    return true;
}

bool SmfParser::TrackParser::eot() {
    return at_eot_;
}

bool SmfParser::TrackParser::parse() {
    if (!reader_.available()) {
        return false;
    }

    msg.delta_time = parseVariableLength(reader_);
    msg.status_byte = (uint8_t)(reader_.read());
    trace_printf("[%s::%s] delta time:%d status byte:%02x\n", kClassName, __func__, msg.delta_time, msg.status_byte);

    if (msg.status_byte == MIDI_MSG_META_EVENT) {
        parseMetaEvent();
    } else if (msg.status_byte == MIDI_MSG_SYS_EX_EVENT || msg.status_byte == MIDI_MSG_END_OF_EXCLUSIVE) {
        uint32_t deta_len = parseVariableLength(reader_);
        for (uint32_t i = 0; i < deta_len; i++) {
            reader_.read();
        }
    } else {
        parseMIDIEvent();
    }
    is_registered_ = true;
    return true;
}

bool SmfParser::TrackParser::parseMIDIEvent() {
    uint8_t bytes[3] = {running_status_, 0, 0};
    int write_offset = 0;
    int param_num = 0;
    if (msg.status_byte & 0x80) {
        bytes[0] = msg.status_byte;
        write_offset = 1;
    } else {
        bytes[1] = msg.status_byte;
        write_offset = 2;
    }
    if ((bytes[0] & 0xF0) == MIDI_MSG_PROGRAM_CHANGE ||    // Program Change
        (bytes[0] & 0xF0) == MIDI_MSG_CHANNEL_PRESSURE) {  // Channel Pressure
        param_num = 1;
    } else {
        param_num = 2;
    }
    for (int i = write_offset; i <= param_num; i++) {
        bytes[i] = (uint8_t)reader_.read();
    }
    msg.status_byte = bytes[0];
    msg.data_byte1 = bytes[1];
    msg.data_byte2 = bytes[2];
    running_status_ = bytes[0];
    trace_printf("[%s::%s] sb:%02x, db1:%d, db2:%d\n", kClassName, __func__, bytes[0], bytes[1], bytes[2]);
    return true;
}

bool SmfParser::TrackParser::parseMetaEvent() {
    msg.event_code = (uint8_t)reader_.read();
    msg.event_length = parseVariableLength(reader_);

    trace_printf("[%s::%s] (Meta Event) Dt:%d event_code:%02x len:%u\n", kClassName, __func__, msg.delta_time, msg.event_code, (unsigned int)msg.event_length);
    if (msg.event_code == MIDI_META_SET_TEMPO && msg.event_length == MIDI_METALEN_SET_TEMPO) {
        for (uint32_t i = 0; i < msg.event_length; i++) {
            msg.sysex_array[i] = (uint8_t)reader_.read();
        }
    } else if (msg.event_code == MIDI_META_END_OF_TRACK) {
        trace_printf("[%s::%s] (Score end)\n", kClassName, __func__);
        at_eot_ = true;
    } else if (msg.event_code < 0x80) {
        for (uint32_t i = 0; i < msg.event_length; i++) {
            if (i < kSysExMaxSize) {
                msg.sysex_array[i] = (uint8_t)reader_.read();
            }
        }
    } else {
        return false;
    }
    return true;
}

SmfParser::SmfParser(const String& path) : file_(path.c_str()), root_tick_(0), tracks_(), is_end_(false), parsers_() {
    parse();
    debug_printf("[%s::%s] SMF File name:%s\n", kClassName, __func__, getFileName().c_str());
    debug_printf("[%s::%s] SMF root tick:%d\n", kClassName, __func__, getRootTick());
    for (size_t i = 0; i < tracks_.size(); i++) {
        debug_printf("[%s::%s] track id:%d offset:%d length:%d\n", kClassName, __func__, tracks_[i].track_id, tracks_[i].offset, tracks_[i].size);
    }
    debug_printf("[%s::%s] scores num:%d\n", kClassName, __func__, getNumberOfScores());
}

SmfParser::~SmfParser() {
    parsers_.clear();
    if (file_) {
        file_.close();
    }
}

uint16_t SmfParser::getRootTick() {
    return root_tick_;
}

String SmfParser::getFileName() {
    String s = file_.name();
    return s.substring(s.lastIndexOf("/") + 1);
}

int SmfParser::getNumberOfScores() {
    if (tracks_.size() > 0) {
        return 1;
    } else {
        return 0;
    }
}

bool SmfParser::loadScore(int id) {
    debug_printf("[%s::%s] file=%s id=%d mtrks=%d mask=%08X\n", kClassName, __func__, file_.name(), id, (int)tracks_.size(), getPlayTrack());
    parsers_.clear();
    for (size_t i = 0; i < tracks_.size(); i++) {
        parsers_.push_back(TrackParser(&file_, tracks_[i].offset, tracks_[i].size));
    }
    is_end_ = false;
    return true;
}

String SmfParser::getTitle(int id) {
    if (id < 0 || tracks_.size() <= (size_t)id) {
        return String();
    }
    return tracks_[id].name;
}

bool SmfParser::getMidiMessage(MidiMessage* midi_message) {
    if (is_end_) {
        midi_message->delta_time = 0;
        midi_message->status_byte = MIDI_MSG_META_EVENT;
        midi_message->event_code = MIDI_META_END_OF_TRACK;
        midi_message->event_length = 0;
        return false;
    }

    while (true) {
        // find earliest MIDI message
        TrackParser* earliest = nullptr;
        size_t earliest_index = parsers_.size();
        for (size_t i = 0; i < parsers_.size(); i++) {
            TrackParser& e = parsers_[i];
            if (e.eot()) {
                continue;
            }
            if (!e.available()) {
                if (e.parse() == false) {
                    continue;
                }
                if (e.msg.status_byte == MIDI_MSG_META_EVENT && e.msg.event_code == MIDI_META_END_OF_TRACK) {
                    e.discard();
                    continue;
                }
            }
            if (earliest == nullptr) {
                earliest = &e;
                earliest_index = i;
            } else if (e.msg.delta_time < earliest->msg.delta_time) {
                earliest = &e;
                earliest_index = i;
            }
        }
        if (earliest == nullptr) {
            debug_printf("[%s::%s] all tracks at end\n", kClassName, __func__);
            is_end_ = true;
            midi_message->delta_time = 0;
            midi_message->status_byte = MIDI_MSG_META_EVENT;
            midi_message->event_code = MIDI_META_END_OF_TRACK;
            midi_message->event_length = 0;
            return true;
        }

        if ((getPlayTrack() & (1U << earliest_index)) == 0) {
            earliest->discard();
            continue;
        } else {
            // pop earliest MIDI message
            memcpy(midi_message, &earliest->msg, sizeof(*midi_message));
            debug_printf("[%s::%s] delta_time:%ld, status_byte:%02x, event_code:%02x\n", kClassName, __func__, earliest->msg.delta_time,
                         earliest->msg.status_byte, earliest->msg.event_code);
            // substract delta_time
            for (auto& e : parsers_) {
                if (e.available()) {
                    e.msg.delta_time -= midi_message->delta_time;
                }
            }
            earliest->discard();
            break;
        }
    }

    return true;
}

bool SmfParser::parse() {
    file_.seek(0);

    struct MThdChunk mthd;
    if (!parseMThd(file_, &mthd)) {
        return false;
    }
    root_tick_ = mthd.division;

    tracks_.clear();
    for (int i = 0; i < mthd.ntrks && i < kMaxTrack; i++) {
        debug_printf("[%s::%s] %d/%d\n", kClassName, __func__, i, mthd.ntrks);
        SmfParser::Track track;
        track.track_id = (int)tracks_.size();
        track.name = getBaseName(file_.name());  // TODO: get name from meta-event (Sequence/Track Name: FF 03 Len text)
        MTrkChunk mtrk;
        if (!parseMTrk(file_, &mtrk)) {
            return false;
        }
        track.size = mtrk.length;
        track.offset = file_.position();
        debug_printf("[%s::%s] track_id:%d offset=%d size=%d\n", kClassName, __func__, track.track_id, track.offset, track.size);
        tracks_.push_back(track);
        file_.seek(track.offset + track.size);
    }

    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
