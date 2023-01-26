/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "CorrectToneFilter.h"

#include "midi_util.h"
#include "ScoreFilter.h"
#include "ScoreParser.h"

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

static const char kClassName[] = "CorrectToneFilter";

static const int kDefaultTempo = (int)60000000 / 120;

static CorrectToneFilter::Note kInvalidNote = {INVALID_NOTE_NUMBER, 0};

CorrectToneFilter::CorrectToneFilter(const String& file_name, Filter& filter) : CorrectToneFilter(file_name, false, filter) {
}

CorrectToneFilter::CorrectToneFilter(const String& file_name, bool auto_start, Filter& filter)
    : ScoreFilter(file_name, filter),
      current_tempo_(kDefaultTempo),
      duration_(0),
      schedule_time_(0),
      total_delta_time_(0),
      default_state_(auto_start ? ScoreFilter::PLAY : ScoreFilter::PAUSE),
      play_state_(default_state_),
      next_state_(default_state_),
      is_event_available(false),
      is_music_start_(true),
      midi_message_(),
      assigned_notes_(),
      playing_notes_() {
    memset(&midi_message_, 0x00, sizeof(midi_message_));
    for (auto& notes : assigned_notes_) {
        for (auto& e : notes) {
            e = kInvalidNote;
        }
    }
    for (auto& notes : playing_notes_) {
        for (auto& e : notes) {
            e = kInvalidNote;
        }
    }
}

CorrectToneFilter::~CorrectToneFilter() {
}

bool CorrectToneFilter::begin() {
    if (ScoreFilter::begin()) {
        return setScoreIndex(0);
    }
    return false;
}

void CorrectToneFilter::update() {
    if (!isParserAvailable()) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
    } else if (getNumberOfScores() > 0) {
        updateNotes();
    }

    ScoreFilter::update();
}

bool CorrectToneFilter::isAvailable(int param_id) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        return true;
    } else {
        return ScoreFilter::isAvailable(param_id);
    }
}

intptr_t CorrectToneFilter::getParam(int param_id) {
    if (param_id == ScoreFilter::PARAMID_STATUS) {
        return play_state_;
    } else {
        return ScoreFilter::getParam(param_id);
    }
}

bool CorrectToneFilter::setParam(int param_id, intptr_t value) {
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

bool CorrectToneFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (ASSIGNABLE_SIZE <= note && note != NOTE_ALL) {
        error_printf("[%s::%s]: note:%d is out of number.\n", kClassName, __func__, note);
        return false;
    }
    if (channel < 1 || 16 < channel) {
        error_printf("[%s::%s]: channel:%d is out of channel num.\n", kClassName, __func__, channel);
        return false;
    }

    uint8_t play_note_mask = 0x00;
    if (note != NOTE_ALL) {
        play_note_mask = 1U << note;
    } else {
        play_note_mask = 0xFF;
    }

    for (int i = 0; i < ASSIGNABLE_SIZE; i++) {
        if (play_note_mask & (1U << i)) {
            if (playing_notes_[channel - 1][i].note != INVALID_NOTE_NUMBER) {
                ScoreFilter::sendNoteOff(playing_notes_[channel - 1][i].note, playing_notes_[channel - 1][i].velocity, channel);
                playing_notes_[channel - 1][i] = kInvalidNote;
            }
        }
    }

    return true;
}

bool CorrectToneFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (ASSIGNABLE_SIZE <= note && note != NOTE_ALL) {
        error_printf("[%s::%s]: note:%d is out of number.\n", kClassName, __func__, note);
        return false;
    }
    if (channel < 1 || 16 < channel) {
        error_printf("[%s::%s]: channel:%d is out of channel num.\n", kClassName, __func__, channel);
        return false;
    }

    uint8_t play_note_mask = 0x00;
    if (note != NOTE_ALL) {
        play_note_mask = 1U << note;
    } else {
        play_note_mask = 0xFF;
    }

    debug_printf("[%s::%s]: note:%d, channel:%d\n", kClassName, __func__, note, channel);
    for (int i = 0; i < ASSIGNABLE_SIZE; i++) {
        if (play_note_mask & (1U << i)) {
            if (assigned_notes_[channel - 1][i].note != INVALID_NOTE_NUMBER) {
                ScoreFilter::sendNoteOn(assigned_notes_[channel - 1][i].note, assigned_notes_[channel - 1][i].velocity, channel);
                playing_notes_[channel - 1][i] = assigned_notes_[channel - 1][i];
            }
        }
    }

    return true;
}

bool CorrectToneFilter::setScoreIndex(int id) {
    if (ScoreFilter::setScoreIndex(id)) {
        current_tempo_ = kDefaultTempo;
        duration_ = 0;
        schedule_time_ = 0;
        total_delta_time_ = 0;
        play_state_ = default_state_;
        is_event_available = false;
        is_music_start_ = true;
        memset(&midi_message_, 0x00, sizeof(midi_message_));
        for (uint8_t channel = 1; channel <= 16; channel++) {
            for (auto& e : assigned_notes_[channel - 1]) {
                e = kInvalidNote;
            }
            for (auto& e : playing_notes_[channel - 1]) {
                if (e.note != INVALID_NOTE_NUMBER) {
                    sendNoteOff(e.note, e.velocity, channel);
                }
                e = kInvalidNote;
            }
        }
        return true;
    }

    return false;
}

bool CorrectToneFilter::updateNotes() {
    if (!isParserAvailable()) {
        return false;
    }
    if (play_state_ == ScoreFilter::END) {
        return false;
    }
    if (is_music_start_) {
        schedule_time_ = millis();
        is_music_start_ = false;
    }

    if (!is_event_available) {
        unsigned long prev_total_delta_time = total_delta_time_;
        getMidiMessage(&midi_message_);
        total_delta_time_ += midi_message_.delta_time;
        debug_printf("[%s::%s] delta_time:%d, ", kClassName, __func__, midi_message_.delta_time);
        debug_printf("status_byte:%02x, data_byte1:%02x, data_byte2:%02x, ", midi_message_.status_byte, midi_message_.data_byte1, midi_message_.data_byte2);
        debug_printf("event_code:%02x, event_length:%02x\n", midi_message_.event_code, midi_message_.event_length);

        uint16_t root_tick = getRootTick();
        if (root_tick != 0) {
            schedule_time_ += (unsigned long)(((current_tempo_ / root_tick) * total_delta_time_) / 1000) -
                              (unsigned long)(((current_tempo_ / root_tick) * prev_total_delta_time) / 1000);
        }
        is_event_available = true;
    }
    if (play_state_ != next_state_) {
        trace_printf("[%s::%s] old stat:%d, now stat:%d\n", kClassName, __func__, play_state_, next_state_);
        if (next_state_ == ScoreFilter::PAUSE) {
            // hold remaining time
            duration_ = schedule_time_ - millis();
        } else if (next_state_ == ScoreFilter::PLAY) {
            // re-calculate schedule time
            schedule_time_ = millis() + duration_;
        }
        play_state_ = next_state_;
    } else if (schedule_time_ <= millis() && play_state_ == PLAY) {
        uint8_t status_byte = midi_message_.status_byte;
        if ((status_byte & 0xF0) == MIDI_MSG_NOTE_ON && midi_message_.data_byte2 == 0) {
            status_byte = MIDI_MSG_NOTE_OFF | (status_byte & 0x0F);
        }
        if ((status_byte & 0xF0) == MIDI_MSG_NOTE_OFF) {
            // unassign note
            for (auto& e : assigned_notes_[status_byte & 0x0F]) {
                if (e.note == midi_message_.data_byte1) {
                    e = kInvalidNote;
                    break;
                }
            }
        } else if ((status_byte & 0xF0) == MIDI_MSG_NOTE_ON) {
            // assign note
            for (auto& e : assigned_notes_[status_byte & 0x0F]) {
                if (e.note == INVALID_NOTE_NUMBER) {
                    e.note = midi_message_.data_byte1;
                    e.velocity = midi_message_.data_byte2;
                    break;
                }
            }
        } else if (status_byte == MIDI_MSG_META_EVENT) {
            if (midi_message_.event_code == MIDI_META_SET_TEMPO) {
                uint32_t tempo = 0;
                for (uint32_t i = 0; i < midi_message_.event_length; i++) {
                    tempo = (tempo << 8) | midi_message_.sysex_array[i];
                }
                current_tempo_ = tempo & 0x00FFFFFF;
                debug_printf("[%s::%s] current_tempo_:%d\n", kClassName, __func__, current_tempo_);
            } else if (midi_message_.event_code == MIDI_META_END_OF_TRACK) {
                play_state_ = ScoreFilter::END;
            } else {
                debug_printf("[%s::%s] %02x is unused Meta event.\n", kClassName, __func__, midi_message_.event_code);
            }
        } else {
            debug_printf("[%s::%s] This is unused Midi event.\n", kClassName, __func__);
        }
        memset(&midi_message_, 0x00, sizeof(midi_message_));
        is_event_available = false;
    }
    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
