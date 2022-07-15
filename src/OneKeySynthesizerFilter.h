/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef ONE_KEY_SYNTHESIZER_FILTER_H_
#define ONE_KEY_SYNTHESIZER_FILTER_H_

#include <time.h>

#include "ScoreReader.h"
#include "YuruInstrumentFilter.h"

class OneKeySynthesizerFilter : public BaseFilter {
public:
    OneKeySynthesizerFilter(const String& file_name, Filter& filter);
    OneKeySynthesizerFilter(Filter& filter);

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;

    bool isAvailable(int param_id) override;
    bool begin() override;
    void update() override;

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;

    ~OneKeySynthesizerFilter();

private:
    ScoreReader* sr_;
    std::vector<ScoreReader::ScoreData> scores_;

    unsigned int score_num_;  //< 選択中の楽譜
    int prev_note_;

    int note_registered_;      //< 登録されているノート番号
    bool is_note_registered_;  //< ノート登録状態
    bool is_note_playing_;     //< ノート再生状態

    void registerNote();

    // others getter setter
    void setScore(unsigned int new_score_num);
    int getScoreNum();
    void selectScore(int id);
};

#endif  // ONE_KEY_SYNTHESIZER_FILTER_H_
