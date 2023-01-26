/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SCORE_SRC_H_
#define SCORE_SRC_H_

#include "ScoreFilter.h"
#include "YuruInstrumentFilter.h"

class ScoreSrc : public ScoreFilter {
public:
    ScoreSrc(const String& file_name, Filter& filter);
    ScoreSrc(const String& file_name, bool auto_start, Filter& filter);
    virtual ~ScoreSrc();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendSongPositionPointer(uint16_t beats) override;
    bool sendSongSelect(uint8_t song) override;
    bool sendContinue() override;
    bool sendStop() override;

    bool setScoreIndex(int index);

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
    bool is_auto_playing_;
    bool is_event_available_;
    bool is_music_start_;

    ScoreParser::MidiMessage midi_message_;

    bool executeMidiEvent(const ScoreParser::MidiMessage& msg);
};

#endif  // SCORE_SRC_H_
