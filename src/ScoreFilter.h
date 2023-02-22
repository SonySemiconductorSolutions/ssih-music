/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SCORE_FILTER_H_
#define SCORE_FILTER_H_

#include <vector>

#include "ParserFactory.h"
#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

class ScoreFilter : public BaseFilter {
public:
    enum ParamId {                              // MAGIC CHAR = 'M'
        PARAMID_NUMBER_OF_SCORES = ('G' << 8),  //<
        PARAMID_ENABLE_TRACK,                   //<
        PARAMID_DISABLE_TRACK,                  //<
        PARAMID_TRACK_MASK,                     //<
        PARAMID_SCORE,                          //<
        PARAMID_SCORE_NAME,                     //<
        PARAMID_STATUS                          //<
    };

    enum Status { PAUSE = 0, PLAY, END };

    struct Note {
        uint8_t note;
        uint8_t velocity;
        uint8_t channel;
        int stat;
    };

    ScoreFilter(const String& file_name, Filter& filter);
    virtual ~ScoreFilter();

    bool begin() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendSongPositionPointer(uint16_t beats) override;
    bool sendSongSelect(uint8_t song) override;
    bool sendContinue() override;
    bool sendStop() override;
    virtual bool sendMtcFullMessage(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr);
    bool sendMidiMessage(uint8_t* msg, size_t length) override;

    bool isParserAvailable();
    int getNumberOfScores();
    int getScoreIndex();
    bool setScoreIndex(int index);
    uint16_t getRootTick();

protected:
    bool getMidiMessage(ScoreParser::MidiMessage* midi_message);

private:
    String file_name_;
    String score_name_;
    ScoreParser* parser_;
    int score_index_;
    uint16_t root_tick_;
    std::vector<Note> playing_notes_;

    uint32_t play_track_flags_;
};

#endif  // SCORE_FILTER_H_
