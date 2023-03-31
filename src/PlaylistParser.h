/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file PlaylistParser.h
 */
#ifndef PLAYLIST_PARSER_H_
#define PLAYLIST_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

/**
 * @brief @~japanese プレイリストファイルを入力とする ScoreParser です。
 */
class PlaylistParser : public ScoreParser {
public:
    /**
     * @brief @~japanese PlaylistParser オブジェクトを生成します。
     * @param[in] path @~japanese プレイリストファイル(.m3u)のパスを指定します。
     */
    PlaylistParser(const String& path);

    ~PlaylistParser();

    uint16_t getRootTick() override;
    String getFileName() override;
    int getNumberOfScores() override;
    bool loadScore(int index) override;
    String getTitle(int index) override;
    bool getMidiMessage(ScoreParser::MidiMessage* midi_message) override;

private:
    struct PlaylistData {
        int index;
        String file_name;
        String title;
    };

    ScoreParser* parser_;
    std::vector<PlaylistData> playlist_data_;
};

#endif  // PLAYLIST_PARSER_H_
