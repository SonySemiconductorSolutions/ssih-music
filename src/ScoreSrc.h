/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SCORE_SRC_H_
#define SCORE_SRC_H_

#include <time.h>

#include "ScoreParser.h"
#include "SmfParser.h"
#include "YuruInstrumentFilter.h"

class ScoreSrc : public BaseFilter {
public:
    enum ParamId {                              // MAGIC CHAR = 'M'
        PARAMID_NUMBER_OF_SCORES = ('G' << 8),  //<
        PARAMID_SCORE,                          //<
        PARAMID_SCORE_NAME,                     //<
        PARAMID_STATUS                          //<
    };

    enum Status { PAUSE = 0, PLAYING, END_SCORE };

    ScoreSrc(const String& file_name, Filter& filter);
    ScoreSrc(const String& file_name, bool auto_start, Filter& filter);

    virtual ~ScoreSrc();

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;
    bool isAvailable(int param_id) override;
    bool begin() override;
    void update() override;

private:
    ScoreParser* parser_;

    String directory_name_;
    int score_num_;

    // others getter setter
    void setScore(int score_id);

    void selectScore(int id);

    int getScoreNum();

    //時間関連の関数は後に移植予定
    // ----------------- timing variable -----------------
    uint16_t root_tick_;      //< txt:曲のBPM, mid:曲の基礎Tick
    int now_tempo_;           //< txt:曲の拍子, mid:基礎Tickの実時間
    unsigned long duration_;  //< txt:ノート再生時間, mid:デルタタイムの実時間

    // ----------------- timing variable -----------------
    // 再生状態
    int play_state_;
    int default_state_;
    unsigned long schedule_time_;  //再生開始時間

    ScoreParser::MidiMessage midi_message_;
    ScoreParser::MidiMessage playing_midi_message_;
    bool is_waiting_;
    bool is_music_start_;

    bool readDirectoryScores(const String& dir_name);
    bool readScore(File& file);

    bool executeMetaEvent(const ScoreParser::MidiMessage& midi_message);
    // bool executeSysExEvent(ScoreParser::MidiMessage& midi_message); //現状は対応させない
    bool executeMIDIEvent(const ScoreParser::MidiMessage& midi_message);
};

#endif  // SCORE_SRC_H_
