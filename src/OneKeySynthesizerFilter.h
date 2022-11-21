/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef ONE_KEY_SYNTHESIZER_FILTER_H_
#define ONE_KEY_SYNTHESIZER_FILTER_H_

#include "ParserFactory.h"
#include "ScoreFilter.h"
#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

class OneKeySynthesizerFilter : public ScoreFilter {
public:
    OneKeySynthesizerFilter(const String& file_name, Filter& filter);

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;

    bool isAvailable(int param_id) override;
    bool begin() override;

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;

    ~OneKeySynthesizerFilter();

private:
    ScoreParser::MidiMessage registered_midi_msg_;

    bool is_note_registered_;  //< ノート登録状態
    bool is_note_playing_;     //< ノート再生状態

    bool is_end_track_;

    void registerNote();

    bool selectScore(int id);
};

#endif  // ONE_KEY_SYNTHESIZER_FILTER_H_
