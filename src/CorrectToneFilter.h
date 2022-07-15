/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef CORRECT_TONE_FILTER_H_
#define CORRECT_TONE_FILTER_H_

#include <time.h>

#include "ScoreReader.h"
#include "YuruInstrumentFilter.h"

class CorrectToneFilter : public BaseFilter {
public:
    CorrectToneFilter(const String& file_name, Filter& filter);
    CorrectToneFilter(Filter& filter);

    ~CorrectToneFilter();

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;
    bool isAvailable(int param_id) override;
    bool begin() override;
    void update() override;

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;

private:
    ScoreReader* sr_;
    std::vector<ScoreReader::ScoreData> scores_;

    bool is_note_registered_;  //< ノート登録状態
    bool is_note_playing_;     //< ノート再生状態

    unsigned long play_start_;  //< 再生開始時間
    unsigned long play_end_;    //< 再生終了時間
    bool is_music_start_;

    int score_num_;  //< 選択中の楽譜

    int note_registered_;  //< 登録されているノート番号
    int note_playing_;     //< 再生中のノート番号

    uint8_t velocity_;  //< 音量
    uint8_t channel_;   //< チャンネル

    bool registerNoteTxt();
    void playNote();
    void stopNote(uint8_t velocity, uint8_t channel);
    void changePlayingNote();

    bool isMIDINum(int note);

    void printScore();
    // others getter setter
    void setScore(unsigned int new_score_num);
    int getScoreNum();

    bool executeCommand(ScoreReader::Note note);

    void selectScore(int id);

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
    ScoreReader::Note note_;
    bool is_waiting_;
    bool registerNoteMidi();
};

#endif  // CORRECT_TONE_FILTER_H_
