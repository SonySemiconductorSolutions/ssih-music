/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef ONE_KEY_SYNTHESIZER_FILTER_H_
#define ONE_KEY_SYNTHESIZER_FILTER_H_

#include "ScoreFilter.h"
#include "YuruInstrumentFilter.h"

class OneKeySynthesizerFilter : public ScoreFilter {
public:
    static const int ASSIGNABLE_SIZE = 7;
    static const int NOTE_ALL = 127;
    struct Note {
        uint8_t note;
        uint8_t velocity;
    };

    OneKeySynthesizerFilter(const String& file_name, Filter& filter);
    ~OneKeySynthesizerFilter();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

    bool setScoreIndex(int id);

private:
    int play_state_;

    ScoreParser::MidiMessage midi_message_;

    Note assigned_notes_[16][ASSIGNABLE_SIZE];
    Note playing_notes_[16][ASSIGNABLE_SIZE];

    uint16_t updateNotes();
};

#endif  // ONE_KEY_SYNTHESIZER_FILTER_H_
