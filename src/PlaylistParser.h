/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef PLAYLIST_PARSER_H_
#define PLAYLIST_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

class PlaylistParser : public ScoreParser {
public:
    struct PlaylistData {
        int index;
        String file_name;

        int time;
        String title;
    };

    PlaylistParser(const String& path);
    ~PlaylistParser();

    uint16_t getRootTick() override;
    String getFileName() override;
    int getNumberOfScores() override;
    bool loadScore(int index) override;
    String getTitle(int index) override;

    bool getMidiMessage(ScoreParser::MidiMessage* midi_message) override;

private:
    ScoreParser* parser_;
    std::vector<PlaylistData> playlist_data_;
};

#endif  // PLAYLIST_PARSER_H_
