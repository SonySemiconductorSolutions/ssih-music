/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "SmfParser.h"

//#define DEBUG (1)
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

SmfParser::SmfParser(const String& path)
    : conductor_index_(0),
      conductor_delta_time_(0),
      score_midi_message_(),
      root_tick_(0),
      track_data_(),
      is_smf_score_end_(false),
      running_status_(0),
      file_(path.c_str()),
      reader_(file_) {
    parseSMF();

    debug_printf("[%s::%s] SMF File name:%s\n", kClassName, __func__, getFileName().c_str());
    debug_printf("[%s::%s] SMF root tick:%d\n", kClassName, __func__, getRootTick());
    for (size_t i = 0; i < track_data_.size(); i++) {
        debug_printf("[%s::%s] track id:%d ", kClassName, __func__, track_data_[i].track_id);
        if (track_data_[i].is_conductor) {
            debug_printf("(conductor track)\n");
        } else {
            debug_printf("\n");
        }
        debug_printf("[%s::%s]     start byte :%d\n", kClassName, __func__, track_data_[i].start_byte);
        debug_printf("[%s::%s]     data length:%d\n", kClassName, __func__, track_data_[i].data_length);
    }
    debug_printf("[%s::%s] playable track num:%d\n", kClassName, __func__, getNumberOfScores());
}

SmfParser::~SmfParser() {
}

uint32_t SmfParser::parseVariableLength() {
    uint32_t length = 0;
    unsigned char c;
    do {
        if (!reader_.available()) {
            error_printf("[%s::%s] %s() Error:End Of file.\n", kClassName, __func__, __func__);
            break;
        }
        c = (unsigned char)reader_.read();
        // 7ビットシフト
        length = length << 7;
        //取得バイトの下位7ビット取得
        //下位7ビットの結合
        length += (c & 0x7F);
    } while (c & 0x80);

    return length;
}

bool SmfParser::getMidiMessage(MidiMessage* midi_message) {
    if (is_smf_score_end_) {
        midi_message->status_byte = ScoreParser::kMetaEvent;
        midi_message->event_code = ScoreParser::kEndOfTrack;
        midi_message->event_length = 0;
        return false;
    } else {
        if (score_midi_message_.status_byte == 0) {
            parseMTrkEvent(&score_midi_message_);
        }
        if (smf_music_conductor_[conductor_index_].event_code == ScoreParser::kEndOfTrack) {
            memcpy(midi_message, &score_midi_message_, sizeof(*midi_message));
            memset(&score_midi_message_, 0x00, sizeof(score_midi_message_));
        } else {
            debug_printf("[%s::%s] conductor_delta_time_:%d, score_midi_message_.delta_time:%d\n", kClassName, __func__, conductor_delta_time_,
                         score_midi_message_.delta_time);
            if (conductor_delta_time_ <= score_midi_message_.delta_time) {
                memcpy(midi_message, &smf_music_conductor_[conductor_index_], sizeof(*midi_message));
                midi_message->delta_time = conductor_delta_time_;
                score_midi_message_.delta_time -= conductor_delta_time_;
                conductor_index_++;
                conductor_delta_time_ = smf_music_conductor_[conductor_index_].delta_time;
            } else {
                memcpy(midi_message, &score_midi_message_, sizeof(*midi_message));
                conductor_delta_time_ -= score_midi_message_.delta_time;
                memset(&score_midi_message_, 0x00, sizeof(score_midi_message_));
            }
        }
    }

    return true;
}

const unsigned int kSmfChunkSize = 4;

struct SmfHeader {
    char chunk[kSmfChunkSize];
    uint32_t header_len;
    uint16_t format;     // 2Byte
    uint16_t track_num;  // 2Byte
    uint16_t time;       // 2Byte
};

static bool parseSMFHeader(BufferedFileReader& reader, SmfHeader* header) {
    bool ret = true;
    // reader.read(header, sizeof(*header));
    char chunk[kSmfChunkSize + 1];
    for (unsigned int i = 0; i < kSmfChunkSize; i++) {
        chunk[i] = reader.read();
    }
    chunk[kSmfChunkSize] = '\0';
    strncpy(header->chunk, chunk, kSmfChunkSize);
    if (strncmp(header->chunk, "MThd", kSmfChunkSize) != 0) {
        ret = false;
    } else {
        for (unsigned int i = 0; i < sizeof(header->header_len); i++) {
            header->header_len = header->header_len << 8;
            header->header_len += reader.read();
        }
        for (unsigned int i = 0; i < sizeof(header->format); i++) {
            header->format = header->format << 8;
            header->format += reader.read();
        }
        for (unsigned int i = 0; i < sizeof(header->track_num); i++) {
            header->track_num = header->track_num << 8;
            header->track_num += reader.read();
        }
        for (unsigned int i = 0; i < sizeof(header->time); i++) {
            header->time = header->time << 8;
            header->time += reader.read();
        }
        if ((header->time >> 8) & 0x80) {
            error_printf("[%s::%s] This MIDI file is not supported.\n", kClassName, __func__);
            ret = false;
        }
    }

    trace_printf("[%s::%s] chunk:%s, length:%d, format:%d, truck:%d, time:%d\n", kClassName, __func__, chunk, header->header_len, header->format,
                 header->track_num, header->time);

    return ret;
}

bool SmfParser::parseSMF() {
    struct SmfHeader h;
    if (!parseSMFHeader(reader_, &h)) {
        return false;
    }
    root_tick_ = h.time;

    int first_track_byte = reader_.position();

    for (int i = 0; i < h.track_num; i++) {
        parseSMFTrack();
    }

    if (1 < h.track_num) {  //トラック1はコンダクタートラックとして扱う
        reader_.seek(first_track_byte);
        parseConductorTrack();
    }

    return true;
}

bool SmfParser::parseSMFTrack() {
    SmfParser::TrackData track = TrackData();
    //トラックヘッダの取得
    char chunk[kSmfChunkSize + 1];
    for (unsigned int i = 0; i < kSmfChunkSize; i++) {
        chunk[i] = reader_.read();
    }
    chunk[kSmfChunkSize] = '\0';
    if (strncmp(chunk, "MTrk", kSmfChunkSize) != 0) {
        return false;
    }

    //トラックデータ長の取得
    uint32_t track_size = 0;
    for (unsigned int i = 0; i < sizeof(track_size); i++) {
        track_size = track_size << 8;
        track_size += reader_.read();
    }
    String track_name = reader_.name();
    if (track_name.indexOf('/') >= 0) {
        track_name = track_name.substring(track_name.lastIndexOf('/') + 1);
    }
    track.track_id = (int)track_data_.size();
    track.start_byte = reader_.position();
    track.data_length = track_size;
    track.is_conductor = false;
    track.name = track_name;  // TODO: get name from meta-event (Sequence/Track Name: FF 03 Len text)

    track_data_.push_back(track);

    reader_.seek(reader_.position() + track_size);

    return true;
}

bool SmfParser::parseConductorTrack() {
    ScoreParser::MidiMessage event;
    memset(&event, 0x00, sizeof(event));

    //コンダクタートラックのヘッダー確認
    char chunk[kSmfChunkSize + 1];
    for (unsigned int i = 0; i < kSmfChunkSize; i++) {
        chunk[i] = reader_.read();
    }
    chunk[kSmfChunkSize] = '\0';
    if (strncmp(chunk, "MTrk", kSmfChunkSize) != 0) {
        return false;
    }

    uint32_t track_len = 0;
    for (unsigned int i = 0; i < sizeof(track_len); i++) {
        track_len = track_len << 8;
        track_len += reader_.read();
    }
    int first_position = reader_.position();

    uint32_t others_delta_time = 0;
    track_data_[0].is_conductor = true;
    while (true) {
        if (reader_.position() - first_position > track_len) {
            break;
        }
        memset(&event, 0x00, sizeof(event));

        parseMTrkEvent(&event);
        if (event.status_byte == ScoreParser::kMetaEvent) {
            event.delta_time += others_delta_time;
            smf_music_conductor_.push_back(event);
            others_delta_time = 0;
            if (event.event_code == ScoreParser::kEndOfTrack) {
                break;
            }
        } else {
            others_delta_time += event.delta_time;
            track_data_[0].is_conductor = false;
        }
    }
    for (size_t i = 0; i < smf_music_conductor_.size(); i++) {
        debug_printf("[%s::%s] delta time:%d, status_byte:%02x, event_code:%02x, event_length:%d (data:", kClassName, __func__,
                     smf_music_conductor_[i].delta_time, smf_music_conductor_[i].status_byte, smf_music_conductor_[i].event_code,
                     smf_music_conductor_[i].event_length);
        for (size_t j = 0; j < smf_music_conductor_[i].event_length; j++) {
            debug_printf("%02x ", smf_music_conductor_[i].sysex_array[j]);
        }
        debug_printf(")\n");
    }

    return true;
}

bool SmfParser::parseMTrkEvent(ScoreParser::MidiMessage* midi_message) {
    midi_message->delta_time = parseVariableLength();
    trace_printf("[%s::%s] delta time:%d\n", kClassName, __func__, midi_message->delta_time);
    //ステータスバイトの取得
    uint8_t status_byte = (uint8_t)(reader_.read());
    trace_printf("[%s::%s] status byte:%02x\n", kClassName, __func__, status_byte);

    uint32_t deta_len = 0;
    if (status_byte == 0xFF) {  // Meta Event
        midi_message->status_byte = status_byte;
        parseMetaEvent(midi_message);
    } else if (status_byte == 0xF0 || status_byte == 0xF7) {  // SysEx Event
        deta_len = parseVariableLength();
        reader_.seek(reader_.position() + deta_len);
    } else {  // MIDI Event
        parseSMFEvent(midi_message, status_byte);
    }

    return true;
}

bool SmfParser::parseMetaEvent(ScoreParser::MidiMessage* midi_message) {
    bool ret = true;
    uint32_t deta_len = 0;

    // metaイベントコードバイトの取得
    unsigned char event_code = (unsigned char)reader_.read();
    midi_message->event_code = (uint8_t)event_code;

    deta_len = parseVariableLength();
    midi_message->event_length = deta_len;

    trace_printf("[%s::%s] (Meta Event) Dt:%d event_code:%02x deta_len:%u ", kClassName, __func__, midi_message->delta_time, event_code,
                 (unsigned int)deta_len);
    if (event_code == ScoreParser::kSetTempo && deta_len == 3) {
        for (unsigned int i = 0; i < deta_len; i++) {
            unsigned char data = reader_.read();
            midi_message->sysex_array[i] = (uint8_t)data;
        }
    } else if (event_code == ScoreParser::kEndOfTrack) {
        trace_printf("(Score end)");
    } else if (event_code < 0x80) {
        for (unsigned int i = 0; i < deta_len; i++) {
            if (i < kSysExMaxSize) {
                midi_message->sysex_array[i] = (uint8_t)reader_.read();
            }
        }
        // reader_.seek(reader_.position() + deta_len);
    } else {
        ret = false;
    }
    trace_printf("\n");
    return ret;
}

static const uint8_t kInvalidParamValue = 0xff;
bool SmfParser::parseSMFEvent(ScoreParser::MidiMessage* midi_message, uint8_t status_byte) {
    if (0x80 <= status_byte) {
        debug_printf("[%s::%s] %02x (MIDI)\n", kClassName, __func__, status_byte);
        uint8_t stat = status_byte;
        midi_message->status_byte = stat;
        parseSMFEventParam(midi_message, stat, kInvalidParamValue);
        running_status_ = stat;
    } else {
        debug_printf("[%s::%s] %02x (MIDI(rs(%02x)))\n", kClassName, __func__, status_byte, running_status_);
        uint8_t stat = status_byte;
        midi_message->status_byte = running_status_;
        parseSMFEventParam(midi_message, running_status_, stat);
    }
    return true;
}

bool SmfParser::parseSMFEventParam(ScoreParser::MidiMessage* midi_message, uint8_t stat, uint8_t param) {
    if ((0x90 <= stat && stat <= 0x9f)) {  // Note On event
        if (param == kInvalidParamValue) {
            midi_message->data_byte1 = reader_.read();
            midi_message->data_byte2 = reader_.read();
        } else {
            midi_message->data_byte1 = param;
            midi_message->data_byte2 = reader_.read();
        }
    } else if ((0x80 <= stat && stat <= 0x8f)) {  // Note Off event.
        if (param == kInvalidParamValue) {
            midi_message->data_byte1 = reader_.read();
            midi_message->data_byte2 = reader_.read();
        } else {
            midi_message->data_byte1 = param;
            midi_message->data_byte2 = reader_.read();
        }
    } else if ((0xc0 <= stat && stat <= 0xcf) ||  // Program Change
               (0xd0 <= stat && stat <= 0xdf)) {  // Channel Pressure
        if (param == kInvalidParamValue) {
            midi_message->data_byte1 = reader_.read();
            midi_message->data_byte2 = 0;
        } else {
            midi_message->data_byte1 = param;
            midi_message->data_byte2 = 0;
        }
    } else {  // Control Change & Polyphonic Key Pressure & Pitch Bend Change
        if (param == kInvalidParamValue) {
            midi_message->data_byte1 = reader_.read();
            midi_message->data_byte2 = reader_.read();
        } else {
            midi_message->data_byte1 = param;
            midi_message->data_byte2 = reader_.read();
        }
    }

    return true;
}

uint16_t SmfParser::getRootTick() {
    return root_tick_;
}

int SmfParser::getNumberOfScores() {
    if (track_data_.size() > 0) {
        return 1;
    } else {
        return 0;
    }
}

String SmfParser::getFileName() {
    return reader_.name();
}

bool SmfParser::loadScore(int id) {
    debug_printf("[%s::%s] %d/%d\n", kClassName, __func__, id, (int)track_data_.size());

    int first_playable_track = 0;
    if (track_data_.size() >= 2) {
        if (track_data_[0].is_conductor) {
            first_playable_track++;
        }
    }
    reader_.seek(track_data_[first_playable_track].start_byte);
    if (smf_music_conductor_.size() > 0) {
        conductor_delta_time_ = smf_music_conductor_[0].delta_time;
    } else {
        conductor_delta_time_ = 0;
    }
    conductor_index_ = 0;
    return true;
}

String SmfParser::getTitle(int id) {
    if (id < 0 || track_data_.size() <= (size_t)id) {
        return String();
    }
    return track_data_[id].name;
};

#endif  // ARDUINO_ARCH_SPRESENSE
