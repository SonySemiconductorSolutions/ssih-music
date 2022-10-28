/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SCORE_SRC_H_
#define SCORE_SRC_H_

#include <time.h>

#include "ParserFactory.h"
#include "ScoreFilter.h"
#include "ScoreParser.h"
#include "SmfParser.h"
#include "YuruInstrumentFilter.h"

class ScoreSrc : public ScoreFilter {
public:
    ScoreSrc(const String& file_name, Filter& filter);
    ScoreSrc(const String& file_name, bool auto_start, Filter& filter);

    virtual ~ScoreSrc();

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;
    bool isAvailable(int param_id) override;
    bool begin() override;
    void update() override;

private:
    // ----------------- timing variable -----------------
    int now_tempo_;           //< txt:曲の拍子, mid:基礎Tickの実時間
    unsigned long duration_;  //< txt:ノート再生時間, mid:デルタタイムの実時間
    unsigned long schedule_time_;

    // ----------------- playing status -----------------
    int play_state_;
    int default_state_;
    bool is_auto_playing_;
    bool is_waiting_;
    bool is_music_start_;

    ScoreParser::MidiMessage midi_message_;
    ScoreParser::MidiMessage playing_midi_message_;

    bool selectScore(int id);
    bool executeMetaEvent(const ScoreParser::MidiMessage& midi_message);
    // bool executeSysExEvent(ScoreParser::MidiMessage& midi_message); //現状は対応させない
    bool executeMIDIEvent(const ScoreParser::MidiMessage& midi_message);
};

#endif  // SCORE_SRC_H_
