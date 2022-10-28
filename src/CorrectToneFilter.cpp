/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "CorrectToneFilter.h"

#include "ScoreFilter.h"
#include "ScoreParser.h"

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

static const char kClassName[] = "CorrectToneFilter";

const int kDefaultTempo = (int)60000000 / 120;

static bool isMIDINum(int note) {
    return (NOTE_NUMBER_MIN <= note && note <= NOTE_NUMBER_MAX);
}

CorrectToneFilter::CorrectToneFilter(const String& file_name, Filter& filter) : CorrectToneFilter(file_name, false, filter) {
}

CorrectToneFilter::CorrectToneFilter(const String& file_name, bool auto_start, Filter& filter)
    : ScoreFilter(file_name, filter),
      now_tempo_(kDefaultTempo),
      duration_(0),
      schedule_time_(0),
      play_state_(auto_start ? ScoreFilter::PLAYING : ScoreFilter::PAUSE),
      default_state_(auto_start ? ScoreFilter::PLAYING : ScoreFilter::PAUSE),
      is_waiting_(false),
      is_end_track_(false),
      is_note_playing_(false),
      is_music_start_(true),
      midi_message_(),
      registered_midi_msg_(),
      playing_midi_msg_() {
}

CorrectToneFilter::~CorrectToneFilter() {
}

bool CorrectToneFilter::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        if (!(play_state_ == ScoreFilter::END_SCORE)) {
            play_state_ = value;
        }
        return true;
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        return selectScore((int)value);
    } else {
        return ScoreFilter::setParam(param_id, value);
    }
}

intptr_t CorrectToneFilter::getParam(int param_id) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        return play_state_;
    } else {
        return ScoreFilter::getParam(param_id);
    }
}

bool CorrectToneFilter::isAvailable(int param_id) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        return true;
    } else {
        return ScoreFilter::isAvailable(param_id);
    }
}

bool CorrectToneFilter::begin() {
    if (ScoreFilter::begin()) {
        return selectScore(0);
    }
    return false;
}

void CorrectToneFilter::update() {
    if (!isScoreParserReady()) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
    } else if (getNumberOfScores() > 0) {
        registerNote();
    }
    if (is_note_playing_) {
        changePlayingNote();
    }
    ScoreFilter::update();
}

bool CorrectToneFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    playNote();
    return true;
}

bool CorrectToneFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    stopNote(velocity, channel);
    return true;
}

bool CorrectToneFilter::selectScore(int id) {
    if (ScoreFilter::setScoreIndex(id)) {
        play_state_ = default_state_;  //楽譜再生終了状態からの復帰用
        is_music_start_ = true;
        is_end_track_ = false;
        memset(&midi_message_, 0x00, sizeof(midi_message_));
        memset(&playing_midi_msg_, 0x00, sizeof(playing_midi_msg_));
        memset(&registered_midi_msg_, 0x00, sizeof(registered_midi_msg_));
        schedule_time_ = 0;
        duration_ = 0;
        is_waiting_ = false;
        return true;
    }

    return false;
}
bool CorrectToneFilter::registerNote() {
    if (play_state_ == ScoreFilter::END_SCORE) {
        return false;
    }
    if (is_music_start_) {
        schedule_time_ = millis();
        is_music_start_ = false;
    }
    if (isScoreParserReady()) {
        if (!is_waiting_) {
            getMidiMessage(&midi_message_);
            debug_printf("[%s::%s] delta_time:%d, ", kClassName, __func__, midi_message_.delta_time);
            debug_printf("status_byte:%02x, data_byte1:%02x, data_byte2:%02x, ", midi_message_.status_byte, midi_message_.data_byte1, midi_message_.data_byte2);
            debug_printf("event_code:%02x, event_length:%02x\n", midi_message_.event_code, midi_message_.event_length);
            uint16_t root_tick = getRootTick();
            if (root_tick != 0) {
                duration_ = (unsigned long)(((now_tempo_ / root_tick) * midi_message_.delta_time) / 1000);
            }
            schedule_time_ += duration_;
            debug_printf("[%s::%s] duration_:%d = dt:%d * tempo_:%d / tick_:%d / 1000\n", kClassName, __func__, (int)duration_, midi_message_.delta_time,
                         now_tempo_, getRootTick());
            is_waiting_ = true;
        }
        if (default_state_ != play_state_) {
            trace_printf("[%s::%s] old stat:%d, now stat:%d\n", kClassName, __func__, default_state_, play_state_);
            if (play_state_ == CorrectToneFilter::PAUSE) {
                duration_ = schedule_time_ - millis();  //残時間の取得
                uint8_t ch = playing_midi_msg_.status_byte & 0x0f;
                ScoreFilter::sendNoteOff(playing_midi_msg_.data_byte1, playing_midi_msg_.data_byte2, ch);
            } else if (play_state_ == CorrectToneFilter::PLAYING) {
                schedule_time_ = millis() + duration_;  //残時間の再生開始
            }
            default_state_ = play_state_;
        } else if (schedule_time_ <= millis() && play_state_ == PLAYING) {
            if (midi_message_.status_byte == ScoreParser::kMetaEvent) {  // メタイベントの処理
                executeMetaEvent(midi_message_);
            } else if (midi_message_.status_byte == ScoreParser::kSysExEvent) {  // SysExイベントの処理
                // executeSysExEvent();
            } else {  // MIDIイベントの処理
                executeMIDIEvent(midi_message_);
            }
            memset(&midi_message_, 0x00, sizeof(midi_message_));
            is_waiting_ = false;
        }
    }
    return true;
}

bool CorrectToneFilter::executeMetaEvent(const ScoreParser::MidiMessage& midi_message) {
    int data = 0;                                             //他のメタイベント対応する場合にも使いまわしたい
    if (midi_message.event_code == ScoreParser::kSetTempo) {  //テンポ変更
        for (unsigned int i = 0; i < midi_message.event_length; i++) {
            data = data << 8;
            data += midi_message.sysex_array[i];
        }
        now_tempo_ = data;
        debug_printf("[%s::%s] now_tempo_:%d\n", kClassName, __func__, now_tempo_);
    } else if (midi_message.event_code == ScoreParser::kEndOfTrack) {  //楽譜終了
        play_state_ = ScoreFilter::END_SCORE;
    } else if (midi_message.event_code < 0x80) {
        debug_printf("[%s::%s] %02x is unused Meta event.\n", kClassName, __func__, midi_message.event_code);
    } else {
        error_printf("[%s::%s] error: This Meta event is not supported.\n", kClassName, __func__);
        return false;
    }
    return true;
}

bool CorrectToneFilter::executeMIDIEvent(const ScoreParser::MidiMessage& midi_message) {
    uint8_t status = midi_message.status_byte & 0xf0;
    if (status == ScoreParser::kNoteOff) {  // Note off
        registered_midi_msg_ = midi_message;
    } else if (status == ScoreParser::kNoteOn) {  // Note on
        registered_midi_msg_ = midi_message;
    } else if (status == ScoreParser::kPolyphonicKeyPressure) {  // Polyphonic Key Pressure
        debug_printf("[%s::%s] Polyphonic Key Pressure is unused Meta event.\n", kClassName, __func__);
    } else if (status == ScoreParser::kControlChange) {  // Control Change
        debug_printf("[%s::%s] Control Change is unused Meta event.\n", kClassName, __func__);
    } else if (status == ScoreParser::kProgramChange) {  // Program Change
        debug_printf("[%s::%s] Program Change is unused Meta event.\n", kClassName, __func__);
    } else if (status == ScoreParser::kChannelPressure) {  // Channel Pressure
        debug_printf("[%s::%s] Channel Pressure is unused Meta event.\n", kClassName, __func__);
    } else if (status == ScoreParser::kPitchBendChange) {  // Pitch Bend Change
        debug_printf("[%s::%s] Pitch Bend Change is unused Meta event.\n", kClassName, __func__);
    }
    return true;
}

void CorrectToneFilter::playNote() {
    if (!is_note_playing_) {  //ノート停止状態
        uint8_t status = registered_midi_msg_.status_byte & 0xf0;
        if (status == ScoreParser::kNoteOn) {  //有効ノート
            uint8_t ch = registered_midi_msg_.status_byte & 0x0f;
            ScoreFilter::sendNoteOn(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, ch);
        }
        playing_midi_msg_ = registered_midi_msg_;  //再生中のノートを保持
        is_note_playing_ = true;                   //ノート再生状態
    } else {                                       //ノート再生状態
        changePlayingNote();
    }
}

void CorrectToneFilter::stopNote(uint8_t velocity, uint8_t channel) {
    if (is_note_playing_) {  //ノート再生状態
        if (isMIDINum(playing_midi_msg_.data_byte1)) {
            uint8_t ch = playing_midi_msg_.status_byte & 0x0f;
            ScoreFilter::sendNoteOff(playing_midi_msg_.data_byte1, playing_midi_msg_.data_byte2, ch);
        }
        is_note_playing_ = false;  //ノート停止状態
    }
}

void CorrectToneFilter::changePlayingNote() {
    if (playing_midi_msg_.status_byte != registered_midi_msg_.status_byte || playing_midi_msg_.data_byte1 != registered_midi_msg_.data_byte1) {
        uint8_t playing_status = registered_midi_msg_.status_byte & 0xf0;
        if (isMIDINum(playing_midi_msg_.data_byte1) && playing_status == ScoreParser::kNoteOn) {
            uint8_t ch = playing_midi_msg_.status_byte & 0x0f;
            ScoreFilter::sendNoteOff(playing_midi_msg_.data_byte1, playing_midi_msg_.data_byte2, ch);
        }
        if (isMIDINum(registered_midi_msg_.data_byte1)) {  //有効ノート
            int8_t regist_ch = registered_midi_msg_.status_byte & 0x0f;
            uint8_t regist_status = registered_midi_msg_.status_byte & 0xf0;
            if (regist_status == ScoreParser::kNoteOn && play_state_ == PLAYING) {
                ScoreFilter::sendNoteOn(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, regist_ch);
            } else if (regist_status == ScoreParser::kNoteOff) {
                ScoreFilter::sendNoteOff(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, regist_ch);
            }
        }
        playing_midi_msg_ = registered_midi_msg_;  //再生中のノートを保持
    }
}

#endif  // ARDUINO_ARCH_SPRESENSE
