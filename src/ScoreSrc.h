/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SCORESRC_H_
#define SCORESRC_H_

#include <time.h>

#include "ScoreReader.h"
#include "YuruInstrumentFilter.h"

class ScoreSrc : public BaseFilter {
public:
    enum ParamId {                              // MAGIC CHAR = 'M'
        PARAMID_NUMBER_OF_SCORES = ('M' << 8),  //<
        PARAMID_PLAYING_SCORE,                  //<
        PARAMID_PLAYING_SCORE_NAME,             //<
        PARAMID_SCORE_NAME                      //<
    };

    ScoreSrc(const String& file_name, Filter& filter);
    ScoreSrc(Filter& filter);

    ~ScoreSrc();

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;
    bool isAvailable(int param_id) override;
    bool begin() override;
    void update() override;

private:
    ScoreReader* sr_;
    std::vector<ScoreReader::ScoreData> scores_;

    unsigned int score_num_;
    int prev_note_;

    void printScore();

    // others getter setter
    void setScore(unsigned int new_score_num);

    void selectScore(int id);

    int getScoreNum();

    //時間関連の関数は後に移植予定
    // ----------------- timing variable -----------------
    int bpm_;               //< txt:曲のBPM, mid:曲の基礎Tick
    int now_rhythm_;        //< txt:曲の拍子, mid:基礎Tickの実時間
    int beat_ms_;           //< txt:16分音符の実時間
    int triplets_beat_ms_;  //< txt:24分音符の実時間
    int note_interval_;     //< txt:ノート再生時間, mid:デルタタイムの実時間

    // timing function
    bool updateNoteInterval();
    void setRhythm(int new_rhythm);
    void calcNoteInterval();
    void setBpm(int new_bpm);
    // ----------------- timing variable -----------------
    // 再生状態
    int play_state_;
    unsigned long play_start_;  //再生開始時間
    unsigned long play_end_;    //再生開始時間
    bool is_music_start_;

    ScoreReader::Note note_;
    bool is_waiting_;
    bool playNoteMidi();
    bool playNoteTxt();
    bool executeCommand(ScoreReader::Note note);
};

#endif  // SCORESRC_H_
