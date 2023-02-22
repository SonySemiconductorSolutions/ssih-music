/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ScoreSrc.h"

#include "midi_util.h"
#include "ScoreParser.h"

// #define DEBUG (1)

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

static const int kDefaultTempo = (int)60000000 / 120;  // 120BPM = 500,000us

ScoreSrc::ScoreSrc(const String& file_name, Filter& filter)
    : ScoreFilter(file_name, filter),
      time_keeper_(),
      default_state_(ScoreFilter::PAUSE),
      play_state_(default_state_),
      next_state_(default_state_),
      is_continuous_playback_(false),
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
    time_keeper_.setCurrentTime();
    if (play_state_ == ScoreFilter::END) {
        return;
    }
    if (is_music_start_) {
        time_keeper_.startSmfTimer();
        is_music_start_ = false;
    }
    if (isParserAvailable() && play_state_ != ScoreFilter::END) {
        if (!is_event_available_) {
            getMidiMessage(&midi_message_);
            is_event_available_ = true;
            debug_printf("[%s::%s] MIDI message %d, (%02X, %02X, %02X), (%02X, %02X)\n",                 //
                         kClassName, __func__, midi_message_.delta_time,                                 //
                         midi_message_.status_byte, midi_message_.data_byte1, midi_message_.data_byte2,  //
                         midi_message_.event_code, midi_message_.event_length);                          //
            time_keeper_.setScheduleTime(midi_message_.delta_time);
        }

        if (play_state_ != next_state_) {
            trace_printf("[%s::%s] change state %d => %d\n", kClassName, __func__, play_state_, next_state_);
            if (next_state_ == ScoreFilter::PAUSE) {
                // hold remaining time
                time_keeper_.stopSmfTimer();
                ScoreFilter::sendStop();
            } else if (next_state_ == ScoreFilter::PLAY) {
                // re-calculate schedule time
                time_keeper_.continueSmfTimer();
                ScoreFilter::sendContinue();
            }
            play_state_ = next_state_;
        } else if (time_keeper_.isScheduledTime() && play_state_ == ScoreFilter::PLAY) {
            time_keeper_.forward(midi_message_.delta_time);
            executeMidiEvent(midi_message_);
            is_event_available_ = false;
        }
    }
    ScoreFilter::update();
}

bool ScoreSrc::isAvailable(int param_id) {
    if (param_id == ScoreSrc::PARAMID_CONTINUOUS_PLAYBACK) {
        return true;
    } else if (param_id == ScoreFilter::PARAMID_STATUS) {
        return true;
    } else {
        return ScoreFilter::isAvailable(param_id);
    }
}

intptr_t ScoreSrc::getParam(int param_id) {
    if (param_id == ScoreSrc::PARAMID_CONTINUOUS_PLAYBACK) {
        return is_continuous_playback_;
    } else if (param_id == ScoreFilter::PARAMID_STATUS) {
        return play_state_;
    } else {
        return ScoreFilter::getParam(param_id);
    }
}

bool ScoreSrc::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreSrc::PARAMID_CONTINUOUS_PLAYBACK) {
        is_continuous_playback_ = value ? true : false;
        return true;
    } else if (param_id == ScoreFilter::PARAMID_STATUS) {
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
    debug_printf("[%s::%s] SongPositionPointer(%d)\n", kClassName, __func__, beats);
    if (isParserAvailable() && play_state_ != ScoreFilter::END) {
        uint32_t ticks = time_keeper_.midiBeatToTick(beats);
        if (ticks < time_keeper_.getTotalTick()) {
            setScoreIndex(getScoreIndex());
        }
        while (time_keeper_.getTotalTick() < ticks && play_state_ != ScoreFilter::END) {
            if (!is_event_available_) {
                getMidiMessage(&midi_message_);
                is_event_available_ = true;
            }
            if (time_keeper_.getTotalTick() + midi_message_.delta_time <= ticks) {
                time_keeper_.forward(midi_message_.delta_time);
                if (midi_message_.status_byte == MIDI_MSG_META_EVENT) {
                    executeMidiEvent(midi_message_);  // for SetTempo and EndOfTrack
                }
                is_event_available_ = false;
            } else {
                time_keeper_.setSmfDuration(time_keeper_.calculateDurationMs(midi_message_.delta_time - ticks));
                debug_printf("[%s::%s] remain_ticks=%d, tempo=%d\n", kClassName, __func__, (int)ticks, time_keeper_.getTempo());
                break;
            }
        }
    }
    return true;
}

bool ScoreSrc::sendSongSelect(uint8_t song) {
    debug_printf("[%s::%s] SongSelect(%d)\n", kClassName, __func__, song);
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

bool ScoreSrc::sendMtcFullMessage(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr) {
    debug_printf("[%s::%s] FullMessage(%d,%02d:%02d:%02d:%02d)\n", kClassName, __func__,  //
                 (hr >> 5) & 0x03, hr & 0x1F, mn & 0x3F, sc & 0x3F, fr & 0x1F);

    time_keeper_.setCurrentTime();

    if (isParserAvailable() && play_state_ != ScoreFilter::END) {
        unsigned long target_ms = time_keeper_.mtcToMs(hr, mn, sc, fr);
        debug_printf("[%s::%s] current_ms=%d, target_ms=%d\n", kClassName, __func__,  //
                     (int)time_keeper_.calculateCurrentMs(0), (int)target_ms);

        while (play_state_ != ScoreFilter::END) {
            if (time_keeper_.isBeforeScheduledMs(target_ms)) {
                time_keeper_.rescheduleTime(target_ms);
                break;
            } else {
                time_keeper_.forward(midi_message_.delta_time);
                executeMidiEvent(midi_message_);
                is_event_available_ = false;
            }

            if (!is_event_available_) {
                getMidiMessage(&midi_message_);
                is_event_available_ = true;
            }
        }
    }

    return true;
}

bool ScoreSrc::setScoreIndex(int id) {
    if (ScoreFilter::setScoreIndex(id)) {
        time_keeper_.reset(getRootTick(), kDefaultTempo);
        play_state_ = next_state_ = default_state_;
        is_event_available_ = false;
        is_music_start_ = true;
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
            time_keeper_.setTempo(tempo & 0x00FFFFFF);
            debug_printf("[%s::%s] set tempo to %d\n", kClassName, __func__, time_keeper_.getTempo());
        } else if (msg.event_code == MIDI_META_END_OF_TRACK) {
            debug_printf("[%s::%s] end of track\n", kClassName, __func__);
            if (is_continuous_playback_) {
                int current_state = play_state_;
                int next_index = getScoreIndex() + 1;
                if (next_index >= getNumberOfScores()) {
                    next_index = 0;
                }
                if (setScoreIndex(next_index)) {
                    next_state_ = current_state;
                } else {
                    play_state_ = ScoreFilter::END;
                }
            } else {
                play_state_ = ScoreFilter::END;
            }
            return true;
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
