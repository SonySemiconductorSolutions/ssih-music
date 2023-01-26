/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef CORRECT_TONE_FILTER_H_
#define CORRECT_TONE_FILTER_H_

#include "ScoreFilter.h"
#include "YuruInstrumentFilter.h"

class CorrectToneFilter : public ScoreFilter {
public:
    static const int ASSIGNABLE_SIZE = 7;
    static const int NOTE_ALL = 127;
    struct Note {
        uint8_t note;
        uint8_t velocity;
    };

    CorrectToneFilter(const String& file_name, Filter& filter);
    CorrectToneFilter(const String& file_name, bool auto_start, Filter& filter);
    ~CorrectToneFilter();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

    bool setScoreIndex(int id);

private:
    // schedule
    int current_tempo_;
    unsigned long duration_;
    unsigned long schedule_time_;
    unsigned long total_delta_time_;

    // playback
    int default_state_;
    int play_state_;
    int next_state_;
    bool is_event_available;
    bool is_music_start_;

    ScoreParser::MidiMessage midi_message_;

    Note assigned_notes_[16][ASSIGNABLE_SIZE];
    Note playing_notes_[16][ASSIGNABLE_SIZE];

    bool updateNotes();
};

#endif  // CORRECT_TONE_FILTER_H_
