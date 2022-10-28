/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef TEXT_SCORE_PARSER_H_
#define TEXT_SCORE_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "BufferedFileReader.h"
#include "ScoreParser.h"

class TextScoreParser : public ScoreParser {
public:
    enum Rhythm {
        kRhythm4Note = 0,
        kRhythm8Note,
        kRhythm16Note,
        kRhythm1NoteTriplets,
        kRhythm2NoteTriplets,
        kRhythm4NoteTriplets,
        kRhythm8NoteTriplets,
    };

    struct Music {
        size_t offset;
        int tick;
        String title;
        int tempo;
        int tone;
        int rhythm;
    };

    TextScoreParser(const String& path);
    virtual ~TextScoreParser();

    uint16_t getRootTick() override;
    String getFileName() override;
    int getNumberOfScores() override;
    bool loadScore(int id) override;
    String getTitle(int id) override;

    bool getMidiMessage(ScoreParser::MidiMessage* msg) override;

private:
    std::vector<Music> musics_;
    File file_;
    BufferedFileReader reader_;
    bool is_end_of_music_;
    int tempo_;
    int tone_;
    int rhythm_;

    int note_;
    int duration_;
};

#endif  // TEXT_SCORE_PARSER_H_
