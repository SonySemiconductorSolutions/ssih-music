/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SCORE_PARSER_H_
#define SCORE_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

#include "YuruInstrumentFilter.h"

class ScoreParser {
public:
    static const unsigned kSysExMaxSize = 128;
    struct MidiMessage {
        uint32_t delta_time;
        uint8_t status_byte;
        uint8_t data_byte1;
        uint8_t data_byte2;
        // for sysex event and meta-event
        uint8_t event_code;
        uint32_t event_length;
        uint8_t sysex_array[kSysExMaxSize];
    };

    ScoreParser();
    virtual ~ScoreParser();

    virtual uint16_t getRootTick() = 0;
    virtual String getFileName() = 0;
    virtual int getNumberOfScores() = 0;
    virtual bool loadScore(int id) = 0;
    virtual String getTitle(int id) = 0;
    bool setEnableTrack(uint16_t value);
    bool setDisableTrack(uint16_t value);
    bool setPlayTrack(uint32_t mask);
    uint32_t getPlayTrack();

    virtual bool getMidiMessage(MidiMessage* midi_message) = 0;

private:
    uint32_t play_track_flags_;
};

#endif  // SCORE_PARSER_H_
