/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ScoreSrc.h"

#include "ScoreParser.h"

//#define DEBUG (1)

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#if defined(DEBUG)
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // if defined(DEBUG)

static const char kClassName[] = "ScoreSrc";

const int kDefaultTempo = (int)60000000 / 120;

ScoreSrc::ScoreSrc(const String& file_name, Filter& filter) : ScoreSrc(file_name, false, filter) {
}

ScoreSrc::ScoreSrc(const String& file_name, bool auto_start, Filter& filter)
    : ScoreFilter(file_name, filter),
      now_tempo_(kDefaultTempo),
      duration_(0),
      schedule_time_(0),
      play_state_(auto_start ? ScoreSrc::PLAYING : ScoreSrc::PAUSE),
      default_state_(auto_start ? ScoreSrc::PLAYING : ScoreSrc::PAUSE),
      is_auto_playing_(auto_start),
      is_waiting_(false),
      is_music_start_(false),
      midi_message_(),
      playing_midi_message_() {
}

ScoreSrc::~ScoreSrc() {
}

bool ScoreSrc::selectScore(int id) {
    if (ScoreFilter::setScoreIndex(id)) {
        play_state_ = default_state_;  //楽譜再生終了状態からの復帰用
        is_music_start_ = true;
        memset(&midi_message_, 0x00, sizeof(midi_message_));
        memset(&playing_midi_message_, 0x00, sizeof(playing_midi_message_));
        schedule_time_ = 0;
        duration_ = 0;
        is_waiting_ = false;
        return true;
    }

    return false;
}

bool ScoreSrc::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        if (!(play_state_ == ScoreFilter::END_SCORE)) {
            play_state_ = value;
        }
        return true;
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        selectScore((int)value);
        return true;
    } else {
        return ScoreFilter::setParam(param_id, value);
    }
}

intptr_t ScoreSrc::getParam(int param_id) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        return play_state_;
    } else {
        return ScoreFilter::getParam(param_id);
    }
}

bool ScoreSrc::isAvailable(int param_id) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        return true;
    } else {
        return ScoreFilter::isAvailable(param_id);
    }
}

bool ScoreSrc::begin() {
    if (ScoreFilter::begin()) {
        selectScore(0);
        return true;
    }
    return false;
}

void ScoreSrc::update() {
    if (play_state_ == ScoreFilter::END_SCORE) {
        debug_printf("[%s::%s] END SCORE\n", kClassName, __func__);
        if (is_auto_playing_) {
            int next_index = getScoreIndex() + 1;
            if (next_index >= getNumberOfScores()) {
                next_index = 0;
            }
            if (selectScore(next_index)) {
                debug_printf("[%s::%s] NEXT SCORE\n", kClassName, __func__);
            }
        }
        return;
    }
    if (is_music_start_) {
        schedule_time_ = millis();
        is_music_start_ = false;
    }
    if (isScoreParserReady()) {
        if (!is_waiting_) {
            getMidiMessage(&midi_message_);
            debug_printf("[%s::%s] delta_time:%d,", kClassName, __func__, midi_message_.delta_time);
            debug_printf(" status_byte:%02x, data_byte1:%02x, data_byte2:%02x,", midi_message_.status_byte, midi_message_.data_byte1, midi_message_.data_byte2);
            debug_printf(" event_code:%02x, event_length:%02x\n", midi_message_.event_code, midi_message_.event_length);
            uint16_t root_tick = getRootTick();
            if (root_tick != 0) {
                duration_ = (unsigned long)(((now_tempo_ / root_tick) * midi_message_.delta_time) / 1000);
            }
            schedule_time_ += duration_;
            is_waiting_ = true;
        }

        if (default_state_ != play_state_) {
            trace_printf("[%s::%s] old stat:%d, now stat:%d\n", kClassName, __func__, default_state_, play_state_);
            if (play_state_ == ScoreFilter::PAUSE) {
                duration_ = schedule_time_ - millis();  //残時間の取得
                pauseAllNotes();
            } else if (play_state_ == ScoreFilter::PLAYING) {
                schedule_time_ = millis() + duration_;  //残時間の再生開始
                resumeAllNotes();
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
    ScoreFilter::update();
}

bool ScoreSrc::executeMetaEvent(const ScoreParser::MidiMessage& midi_message) {
    int data = 0;                                             //他のメタイベント対応する場合にも使いまわしたい
    if (midi_message.event_code == ScoreParser::kSetTempo) {  //テンポ変更
        for (unsigned int i = 0; i < midi_message.event_length; i++) {
            data = data << 8;
            data += midi_message.sysex_array[i];
        }
        now_tempo_ = data;
        debug_printf("[%s::%s] %d\n", kClassName, __func__, data);
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

bool ScoreSrc::executeMIDIEvent(const ScoreParser::MidiMessage& midi_message) {
    uint8_t status = midi_message.status_byte & 0xf0;
    uint8_t ch = midi_message.status_byte & 0x0f;
    if (status == ScoreParser::kNoteOff) {  // Note off
        ScoreFilter::sendNoteOff(midi_message.data_byte1, midi_message.data_byte2, ch);
        playing_midi_message_ = midi_message_;
    } else if (status == ScoreParser::kNoteOn) {  // Note on
        if (midi_message.data_byte2 == 0) {       // velocity = 0
            ScoreFilter::sendNoteOff(midi_message.data_byte1, midi_message.data_byte2, ch);
        } else {
            ScoreFilter::sendNoteOn(midi_message.data_byte1, midi_message.data_byte2, ch);
        }
        playing_midi_message_ = midi_message_;
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

#endif  // ARDUINO_ARCH_SPRESENSE
