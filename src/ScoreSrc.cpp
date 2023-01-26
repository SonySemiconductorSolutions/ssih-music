/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ScoreSrc.h"

#include "midi_util.h"
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
      current_tempo_(kDefaultTempo),
      duration_(0),
      schedule_time_(0),
      total_delta_time_(0),
      default_state_(auto_start ? ScoreFilter::PLAY : ScoreFilter::PAUSE),
      play_state_(default_state_),
      next_state_(default_state_),
      is_auto_playing_(auto_start),
      is_event_available_(false),
      is_music_start_(false),
      midi_message_() {
    memset(&midi_message_, 0x00, sizeof(midi_message_));
}

ScoreSrc::~ScoreSrc() {
}

bool ScoreSrc::begin() {
    if (ScoreFilter::begin()) {
        return setScoreIndex(0);
    }
    return false;
}

void ScoreSrc::update() {
    if (play_state_ == ScoreFilter::END) {
        // debug_printf("[%s::%s] END SCORE\n", kClassName, __func__);
        if (is_auto_playing_) {
            int next_index = getScoreIndex() + 1;
            if (next_index >= getNumberOfScores()) {
                next_index = 0;
            }
            if (setScoreIndex(next_index)) {
                debug_printf("[%s::%s] NEXT SCORE\n", kClassName, __func__);
            }
        }
        return;
    }
    if (is_music_start_) {
        schedule_time_ = millis();
        is_music_start_ = false;
    }
    if (isParserAvailable()) {
        if (!is_event_available_) {
            unsigned long prev_total_delta_time = total_delta_time_;
            getMidiMessage(&midi_message_);
            total_delta_time_ += midi_message_.delta_time;
            debug_printf("[%s::%s] delta_time:%d,", kClassName, __func__, midi_message_.delta_time);
            debug_printf(" status_byte:%02x, data_byte1:%02x, data_byte2:%02x,", midi_message_.status_byte, midi_message_.data_byte1, midi_message_.data_byte2);
            debug_printf(" event_code:%02x, event_length:%02x\n", midi_message_.event_code, midi_message_.event_length);

            uint16_t root_tick = getRootTick();
            if (root_tick != 0) {
                schedule_time_ += (unsigned long)(((current_tempo_ / root_tick) * total_delta_time_) / 1000) -
                                  (unsigned long)(((current_tempo_ / root_tick) * prev_total_delta_time) / 1000);
            }
            is_event_available_ = true;
        }

        if (play_state_ != next_state_) {
            trace_printf("[%s::%s] old stat:%d, now stat:%d\n", kClassName, __func__, play_state_, next_state_);
            if (next_state_ == ScoreFilter::PAUSE) {
                // hold remaining time
                duration_ = schedule_time_ - millis();
                ScoreFilter::sendStop();
            } else if (next_state_ == ScoreFilter::PLAY) {
                // re-calculate schedule time
                schedule_time_ = millis() + duration_;
                ScoreFilter::sendContinue();
            }
            play_state_ = next_state_;
        } else if (schedule_time_ <= millis() && play_state_ == PLAY) {
            executeMidiEvent(midi_message_);
            memset(&midi_message_, 0x00, sizeof(midi_message_));
            is_event_available_ = false;
        }
    }
    ScoreFilter::update();
}

bool ScoreSrc::isAvailable(int param_id) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        return true;
    } else {
        return ScoreFilter::isAvailable(param_id);
    }
}

intptr_t ScoreSrc::getParam(int param_id) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        return play_state_;
    } else {
        return ScoreFilter::getParam(param_id);
    }
}

bool ScoreSrc::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        if (play_state_ != ScoreFilter::END) {
            next_state_ = value;
        }
        return true;
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        return setScoreIndex((int)value);
    } else {
        return ScoreFilter::setParam(param_id, value);
    }
}

bool ScoreSrc::sendSongPositionPointer(uint16_t beats) {
    return true;
}

bool ScoreSrc::sendSongSelect(uint8_t song) {
    return setScoreIndex(song);
}

bool ScoreSrc::sendContinue() {
    debug_printf("[%s::%s] Continue\n", kClassName, __func__);
    next_state_ = ScoreFilter::PLAY;
    return true;
}

bool ScoreSrc::sendStop() {
    debug_printf("[%s::%s] Stop\n", kClassName, __func__);
    next_state_ = ScoreFilter::PAUSE;
    return true;
}

bool ScoreSrc::setScoreIndex(int id) {
    if (ScoreFilter::setScoreIndex(id)) {
        duration_ = 0;
        schedule_time_ = 0;
        play_state_ = default_state_;
        is_event_available_ = false;
        is_music_start_ = true;
        memset(&midi_message_, 0x00, sizeof(midi_message_));
        return true;
    }

    return false;
}

bool ScoreSrc::executeMidiEvent(const ScoreParser::MidiMessage& msg) {
    uint8_t status_byte = msg.status_byte;
    if ((status_byte & 0xF0) == MIDI_MSG_NOTE_ON && msg.data_byte2 == 0) {
        status_byte = MIDI_MSG_NOTE_OFF | (status_byte & 0x0F);
    }
    if ((status_byte & 0xF0) == MIDI_MSG_NOTE_OFF) {
        return sendNoteOff(msg.data_byte1, msg.data_byte2, (status_byte & 0x0F) + 1);
    } else if ((status_byte & 0xF0) == MIDI_MSG_NOTE_ON) {
        return sendNoteOn(msg.data_byte1, msg.data_byte2, (status_byte & 0x0F) + 1);
    } else if (status_byte == MIDI_MSG_META_EVENT) {
        if (msg.event_code == MIDI_META_SET_TEMPO) {
            uint32_t tempo = 0;
            for (uint32_t i = 0; i < msg.event_length; i++) {
                tempo = (tempo << 8) | msg.sysex_array[i];
            }
            current_tempo_ = tempo & 0x00FFFFFF;
            debug_printf("[%s::%s] set tempo to %d\n", kClassName, __func__, current_tempo_);
            total_delta_time_ = 0;
        } else if (msg.event_code == MIDI_META_END_OF_TRACK) {
            debug_printf("[%s::%s] end of track\n", kClassName, __func__);
            play_state_ = ScoreFilter::END;
        } else if (msg.event_code < 0x80) {
            debug_printf("[%s::%s] event_code %02X is not supported.\n", kClassName, __func__, msg.event_code);
            return false;
        }
    } else {
        debug_printf("[%s::%s] status_byte %02X is not supported.\n", kClassName, __func__, status_byte);
        return false;
    }
    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
