/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "SmfParser.h"

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
static const int kSmfChunkSize = 4;

struct SmfHeader {
    char chunk_type[kSmfChunkSize];  // 4Bytes
    uint32_t length;                 // 4Bytes
    uint16_t format;                 // 2Bytes
    uint16_t ntrks;                  // 2Bytes
    uint16_t division;               // 2Bytes
};

static bool parseMThd(File& file, SmfHeader* header) {
    bool ret = true;
    char chunk[kSmfChunkSize + 1];
    for (int i = 0; i < kSmfChunkSize; i++) {
        chunk[i] = file.read();
    }
    chunk[kSmfChunkSize] = '\0';
    strncpy(header->chunk_type, chunk, kSmfChunkSize);
    if (strncmp(header->chunk_type, "MThd", kSmfChunkSize) != 0) {
        ret = false;
    } else {
        for (size_t i = 0; i < sizeof(header->length); i++) {
            header->length = (header->length << 8) | (file.read() & 0xFF);
        }
        for (size_t i = 0; i < sizeof(header->format); i++) {
            header->format = (header->format << 8) | (file.read() & 0xFF);
        }
        for (size_t i = 0; i < sizeof(header->ntrks); i++) {
            header->ntrks = (header->ntrks << 8) | (file.read() & 0xFF);
        }
        for (size_t i = 0; i < sizeof(header->division); i++) {
            header->division = (header->division << 8) | (file.read() & 0xFF);
        }
        if (header->division & 0x8000) {
            error_printf("[%s::%s] This MIDI file is not supported.\n", kClassName, __func__);
            ret = false;
        }
    }

    trace_printf("[%s::%s] chunk:%s, length:%d, format:%d, truck:%d, division:%d\n", kClassName, __func__, chunk, header->length, header->format, header->ntrks,
                 header->division);

    return ret;
}

static uint32_t parseVariableLength(SmfParser::SegmentReader& reader) {
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

SmfParser::SegmentReader::SegmentReader(File* file, uint32_t offset, uint32_t size)
    : file_(file), file_pos_(offset), seg_offset_(offset), seg_size_(size), buf_(new uint8_t[kBufSize]), buf_size_(0), buf_pos_(0) {
}

SmfParser::SegmentReader::~SegmentReader() {
    if (buf_) {
        delete[] buf_;
        buf_ = nullptr;
    }
}

size_t SmfParser::SegmentReader::available() {
    size_t seg_pos = file_pos_ - seg_offset_ + buf_pos_;
    return seg_size_ - seg_pos;
}

boolean SmfParser::SegmentReader::reset() {
    file_pos_ = seg_offset_;
    buf_size_ = 0;
    buf_pos_ = 0;
    return true;
}

int SmfParser::SegmentReader::read(void) {
    size_t seg_pos = file_pos_ - seg_offset_ + buf_pos_;
    if (file_ == nullptr) {
        return -1;
    }
    if (seg_size_ <= seg_pos) {
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

SmfParser::SmfParser(const String& path) : file_(path.c_str()), root_tick_(0), mtrks_(), is_end_(false), readers_() {
    file_.seek(0);

    parseSMF();

    debug_printf("[%s::%s] SMF File name:%s\n", kClassName, __func__, getFileName().c_str());
    debug_printf("[%s::%s] SMF root tick:%d\n", kClassName, __func__, getRootTick());
    for (size_t i = 0; i < mtrks_.size(); i++) {
        debug_printf("[%s::%s] track id:%d start byte:%d data length:%d\n", kClassName, __func__, mtrks_[i].track_id, mtrks_[i].offset, mtrks_[i].size);
    }
    debug_printf("[%s::%s] playable track num:%d\n", kClassName, __func__, getNumberOfScores());
}

SmfParser::~SmfParser() {
    if (file_) {
        file_.close();
    }
    for (auto& e : readers_) {
        if (e.reader) {
            delete e.reader;
        }
    }
}

uint16_t SmfParser::getRootTick() {
    return root_tick_;
}

String SmfParser::getFileName() {
    return file_.name();
}

int SmfParser::getNumberOfScores() {
    if (mtrks_.size() > 0) {
        return 1;
    } else {
        return 0;
    }
}

bool SmfParser::loadScore(int id) {
    debug_printf("[%s::%s] file=%s id=%d mtrks=%d mask=%08X\n", kClassName, __func__, file_.name(), id, (int)mtrks_.size(), getPlayTrack());

    for (auto& e : readers_) {
        if (e.reader) {
            delete e.reader;
        }
    }
    readers_.clear();

    for (size_t i = 0; i < mtrks_.size(); i++) {
        TrackReader track;
        track.reader = new SegmentReader(&file_, mtrks_[i].offset, mtrks_[i].size);
        if (getPlayTrack() & (1U << i)) {
            track.at_eot = false;
        } else {
            track.at_eot = true;
        }
        track.is_registered = false;
        readers_.push_back(track);
    }

    is_end_ = false;
    return true;
}

String SmfParser::getTitle(int id) {
    if (id < 0 || mtrks_.size() <= (size_t)id) {
        return String();
    }
    return mtrks_[id].name;
};

bool SmfParser::getMidiMessage(MidiMessage* midi_message) {
    if (is_end_) {
        midi_message->status_byte = MIDI_MSG_META_EVENT;
        midi_message->event_code = MIDI_META_END_OF_TRACK;
        midi_message->event_length = 0;
        return false;
    }

    // find earliest MIDI message
    TrackReader* earliest = nullptr;
    for (auto& e : readers_) {
        if (e.at_eot) {
            continue;
        }
        if (!e.is_registered) {
            if (parseMTrkEvent(e, &e.msg) == false) {
                e.at_eot = true;
                continue;
            } else {
                e.is_registered = true;
                if (e.msg.status_byte == MIDI_MSG_META_EVENT && e.msg.event_code == MIDI_META_END_OF_TRACK) {
                    e.at_eot = true;
                    continue;
                }
            }
        }
        if (earliest == nullptr) {
            earliest = &e;
        } else if (e.msg.delta_time < earliest->msg.delta_time) {
            earliest = &e;
        }
    }
    if (earliest == nullptr) {
        debug_printf("[%s::%s] all tracks at end\n", kClassName, __func__);
        is_end_ = true;
        midi_message->status_byte = MIDI_MSG_META_EVENT;
        midi_message->event_code = MIDI_META_END_OF_TRACK;
        midi_message->event_length = 0;
        return true;
    }

    // pop earliest MIDI message
    memcpy(midi_message, &earliest->msg, sizeof(*midi_message));
    debug_printf("[%s::%s] delta_time:%ld, status_byte:%02x, event_code:%02x\n", kClassName, __func__, (unsigned long)earliest->msg.delta_time,
                 earliest->msg.status_byte, earliest->msg.event_code);
    earliest->is_registered = false;

    // substract delta_time
    for (auto& e : readers_) {
        if (e.is_registered) {
            e.msg.delta_time -= earliest->msg.delta_time;
        }
    }

    return true;
}

bool SmfParser::parseSMF() {
    struct SmfHeader mthd;
    if (!parseMThd(file_, &mthd)) {
        return false;
    }
    root_tick_ = mthd.division;

    for (int i = 0; i < mthd.ntrks && i < kMaxTrack; i++) {
        debug_printf("[%s::%s] %d/%d\n", kClassName, __func__, i, mthd.ntrks);
        parseMTrk();
    }

    return true;
}

bool SmfParser::parseMTrk() {
    // Get Track Headers
    char chunk[kSmfChunkSize + 1];
    for (int i = 0; i < kSmfChunkSize; i++) {
        chunk[i] = file_.read();
    }
    chunk[kSmfChunkSize] = '\0';
    if (strncmp(chunk, "MTrk", kSmfChunkSize) != 0) {
        return false;
    }

    // Get track data length
    uint32_t track_size = 0;
    for (size_t i = 0; i < sizeof(track_size); i++) {
        track_size = track_size << 8;
        track_size += file_.read();
    }

    SmfParser::MTrkSegment mtrk;
    mtrk.track_id = (int)mtrks_.size();
    mtrk.offset = file_.position();
    mtrk.size = track_size;
    mtrk.name = getBaseName(file_.name());  // TODO: get name from meta-event (Sequence/Track Name: FF 03 Len text)
    debug_printf("[%s::%s] track_id:%d offset=%d size=%d\n", kClassName, __func__, mtrk.track_id, mtrk.offset, mtrk.size);
    mtrks_.push_back(mtrk);

    file_.seek(mtrk.offset + mtrk.size);

    return true;
}

bool SmfParser::parseMTrkEvent(TrackReader& track, ScoreParser::MidiMessage* midi_message) {
    if (!track.reader->available()) {
        return false;
    }

    midi_message->delta_time = parseVariableLength(*track.reader);
    uint8_t status_byte = (uint8_t)(track.reader->read());
    trace_printf("[%s::%s] delta time:%d status byte:%02x\n", kClassName, __func__, midi_message->delta_time, status_byte);

    if (status_byte == MIDI_MSG_META_EVENT) {
        midi_message->status_byte = status_byte;
        return parseMetaEvent(track, midi_message);
    } else if (status_byte == MIDI_MSG_SYS_EX_EVENT || status_byte == MIDI_MSG_END_OF_EXCLUSIVE) {
        uint32_t deta_len = parseVariableLength(*track.reader);
        for (uint32_t i = 0; i < deta_len; i++) {
            track.reader->read();
        }
        return true;
    } else {
        return parseMIDIEvent(track, midi_message, status_byte);
    }
}

bool SmfParser::parseMIDIEvent(TrackReader& track, ScoreParser::MidiMessage* midi_message, uint8_t status_byte) {
    uint8_t bytes[3] = {track.running_status, 0, 0};
    int write_offset = 0;
    int param_num = 0;
    if (status_byte & 0x80) {
        bytes[0] = status_byte;
        write_offset = 1;
    } else {
        bytes[1] = status_byte;
        write_offset = 2;
    }
    if ((bytes[0] & 0xF0) == MIDI_MSG_PROGRAM_CHANGE ||    // Program Change
        (bytes[0] & 0xF0) == MIDI_MSG_CHANNEL_PRESSURE) {  // Channel Pressure
        param_num = 1;
    } else {
        param_num = 2;
    }
    for (int i = write_offset; i <= param_num; i++) {
        bytes[i] = (uint8_t)track.reader->read();
    }
    midi_message->status_byte = bytes[0];
    midi_message->data_byte1 = bytes[1];
    midi_message->data_byte2 = bytes[2];
    track.running_status = bytes[0];
    return true;
}

bool SmfParser::parseMetaEvent(TrackReader& track, ScoreParser::MidiMessage* midi_message) {
    uint8_t event_code = (uint8_t)track.reader->read();
    midi_message->event_code = (uint8_t)event_code;

    uint32_t len = parseVariableLength(*track.reader);
    midi_message->event_length = len;

    trace_printf("[%s::%s] (Meta Event) Dt:%d event_code:%02x len:%u\n", kClassName, __func__, midi_message->delta_time, event_code, (unsigned int)len);
    if (event_code == MIDI_META_SET_TEMPO && len == MIDI_METALEN_SET_TEMPO) {
        for (uint32_t i = 0; i < len; i++) {
            midi_message->sysex_array[i] = (uint8_t)track.reader->read();
        }
    } else if (event_code == MIDI_META_END_OF_TRACK) {
        trace_printf("[%s::%s] (Score end)\n", kClassName, __func__);
    } else if (event_code < 0x80) {
        for (uint32_t i = 0; i < len; i++) {
            if (i < kSysExMaxSize) {
                midi_message->sysex_array[i] = (uint8_t)track.reader->read();
            }
        }
    } else {
        return false;
    }
    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
