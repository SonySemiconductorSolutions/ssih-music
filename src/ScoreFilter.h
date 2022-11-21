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
        PARAMID_SCORE,                          //<
        PARAMID_SCORE_NAME,                     //<
        PARAMID_STATUS                          //<
    };

    enum Status { PAUSE = 0, PLAYING, END_SCORE };

    enum NoteState {
        kNoteStatePlaying,
        kNoteStatePause,
        kNoteStateEnd
    };

    struct Note {
        uint8_t note;
        uint8_t velocity;
        uint8_t channel;
        NoteState stat;
    };

    ScoreFilter(const String& file_name, Filter& filter);

    virtual ~ScoreFilter();

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;
    bool isAvailable(int param_id) override;
    bool begin() override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;

protected:
    bool setScoreIndex(int index);
    bool getMidiMessage(ScoreParser::MidiMessage* midi_message);
    int getScoreIndex();
    bool isScoreParserReady();
    void setStatus(int stat);
    uint16_t getRootTick();
    int getNumberOfScores();
    void pauseAllNotes();
    void resumeAllNotes();

private:
    ScoreParser* parser_;

    String file_name_;
    int score_index_;

    uint16_t root_tick_;  //< txt:曲のBPM, mid:曲の基礎Tick

    std::vector<Note> playing_notes_;
};

#endif  // SCORE_FILTER_H_