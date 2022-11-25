/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef CORRECT_TONE_FILTER_H_
#define CORRECT_TONE_FILTER_H_

#include "ParserFactory.h"
#include "ScoreFilter.h"
#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

class CorrectToneFilter : public ScoreFilter {
public:
    CorrectToneFilter(const String& file_name, Filter& filter);
    CorrectToneFilter(const String& file_name, bool auto_start, Filter& filter);

    ~CorrectToneFilter();

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;
    bool isAvailable(int param_id) override;
    bool begin() override;
    void update() override;

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;

private:
    // ----------------- timing variable -----------------
    int now_tempo_;                //< txt:曲の拍子, mid:基礎Tickの実時間
    unsigned long duration_;       //< txt:ノート再生時間, mid:デルタタイムの実時間
    unsigned long schedule_time_;  //再生開始時間

    unsigned long total_delta_time_;

    // ----------------- playing status -----------------
    int play_state_;
    int default_state_;
    bool is_waiting_;
    bool is_end_track_;
    bool is_note_playing_;  //< ノート再生状態
    bool is_music_start_;

    ScoreParser::MidiMessage midi_message_;
    ScoreParser::MidiMessage registered_midi_msg_;
    ScoreParser::MidiMessage playing_midi_msg_;

    bool selectScore(int id);
    bool registerNote();
    void playNote();
    void stopNote(uint8_t velocity, uint8_t channel);
    void changePlayingNote();

    bool executeMetaEvent(const ScoreParser::MidiMessage& midi_message);
    // bool executeSysExEvent(ScoreParser::MidiMessage& midi_message); //現状は対応させない
    bool executeMIDIEvent(const ScoreParser::MidiMessage& midi_message);
};

#endif  // CORRECT_TONE_FILTER_H_
