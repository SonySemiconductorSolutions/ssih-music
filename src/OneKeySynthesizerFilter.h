/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef ONE_KEY_SYNTHESIZER_FILTER_H_
#define ONE_KEY_SYNTHESIZER_FILTER_H_

#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

class OneKeySynthesizerFilter : public BaseFilter {
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
    ScoreParser* parser_;

    String directory_name_;
    ScoreParser::MidiMessage registered_midi_msg_;

    int score_num_;  //< 選択中の楽譜

    bool is_note_registered_;  //< ノート登録状態
    bool is_note_playing_;     //< ノート再生状態

    bool is_end_track_;

    void registerNote();

    // others getter setter
    void setScore(int score_id);
    int getScoreNum();

    bool readDirectoryScores(const String& dir_name);

    bool readScore(File& file);

    void selectScore(int id);
};

#endif  // ONE_KEY_SYNTHESIZER_FILTER_H_
