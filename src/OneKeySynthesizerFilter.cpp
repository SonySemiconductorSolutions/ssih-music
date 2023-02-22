/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "OneKeySynthesizerFilter.h"

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

static const char kClassName[] = "OneKeySynthesizerFilter";

static const int kDefaultTempo = (int)60000000 / 120;

static OneKeySynthesizerFilter::Note kInvalidNote = {INVALID_NOTE_NUMBER, 0};

OneKeySynthesizerFilter::OneKeySynthesizerFilter(const String& file_name, Filter& filter)
    : ScoreFilter(file_name, filter),
      play_state_(ScoreFilter::PLAY),
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

OneKeySynthesizerFilter::~OneKeySynthesizerFilter() {
}

bool OneKeySynthesizerFilter::begin() {
    if (ScoreFilter::begin()) {
        return setScoreIndex(0);
    }
    return false;
}

void OneKeySynthesizerFilter::update() {
    if (!isParserAvailable()) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
    }

    ScoreFilter::update();
}

bool OneKeySynthesizerFilter::isAvailable(int param_id) {
    return ScoreFilter::isAvailable(param_id);
}

intptr_t OneKeySynthesizerFilter::getParam(int param_id) {
    return ScoreFilter::getParam(param_id);
}

bool OneKeySynthesizerFilter::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreFilter::PARAMID_SCORE) {
        return setScoreIndex((int)value);
    } else {
        return ScoreFilter::setParam(param_id, value);
    }
}

bool OneKeySynthesizerFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
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

bool OneKeySynthesizerFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (ASSIGNABLE_SIZE <= note && note != NOTE_ALL) {
        error_printf("[%s::%s]: note:%d is out of number.\n", kClassName, __func__, note);
        return false;
    }
    if (channel < 1 || 16 < channel) {
        error_printf("[%s::%s]: channel:%d is out of channel num.\n", kClassName, __func__, channel);
        return false;
    }

    // assign valid notes to a specified channel
    if (getNumberOfScores() > 0) {
        while (play_state_ != ScoreFilter::END) {
            if ((updateNotes() & (1U << (channel - 1))) == 0) {
                continue;
            }
            bool has_allocated_note = false;
            for (const auto& e : assigned_notes_[channel - 1]) {
                if (e.note != INVALID_NOTE_NUMBER) {
                    has_allocated_note = true;
                    break;
                }
            }
            if (!has_allocated_note) {
                continue;
            }
            break;
        }
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

bool OneKeySynthesizerFilter::setScoreIndex(int id) {
    if (ScoreFilter::setScoreIndex(id)) {
        play_state_ = ScoreFilter::PLAY;
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

uint16_t OneKeySynthesizerFilter::updateNotes() {
    uint16_t change_flags = 0x0000;
    trace_printf("[%s::%s]: play_state=%d\n", kClassName, __func__, play_state_);
    if (!isParserAvailable()) {
        return change_flags;
    }
    while (play_state_ != ScoreFilter::END) {
        if (midi_message_.delta_time > 0) {
            // process current MIDI event next time this function called
            midi_message_.delta_time = 0;
            break;
        }

        // process MIDI event while delta_time is 0
        uint8_t status_byte = midi_message_.status_byte;
        if ((status_byte & 0xF0) == MIDI_MSG_NOTE_ON && midi_message_.data_byte2 == 0) {
            status_byte = MIDI_MSG_NOTE_OFF | (status_byte & 0x0F);
        }
        if ((status_byte & 0xF0) == MIDI_MSG_NOTE_OFF) {
            // unassign note
            for (auto& e : assigned_notes_[status_byte & 0x0F]) {
                if (e.note == midi_message_.data_byte1) {
                    change_flags = change_flags | (1U << (status_byte & 0x0F));
                    e = kInvalidNote;
                    break;
                }
            }
        } else if ((status_byte & 0xF0) == MIDI_MSG_NOTE_ON) {
            // assign note
            for (auto& e : assigned_notes_[status_byte & 0x0F]) {
                if (e.note == INVALID_NOTE_NUMBER) {
                    change_flags = change_flags | (1U << (status_byte & 0x0F));
                    e.note = midi_message_.data_byte1;
                    e.velocity = midi_message_.data_byte2;
                    break;
                }
            }
        } else if (status_byte == MIDI_MSG_META_EVENT && midi_message_.event_code == MIDI_META_END_OF_TRACK) {
            play_state_ = ScoreFilter::END;
            ScoreFilter::sendStop();
            break;
        }
        getMidiMessage(&midi_message_);
    }
    return change_flags;
}

#endif  // ARDUINO_ARCH_SPRESENSE
