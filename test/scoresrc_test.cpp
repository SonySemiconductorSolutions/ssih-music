/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include "ScoreReader.h"

TEST(ScoreReader, test) {
    ScoreReader sr("testdata/ScoreSrc/BeethovenFurElise.mid");
    std::vector<ScoreReader::ScoreData> scores = sr.getScoresData();
    sr.load(scores[0]);
    int i = 0;
    while(true) {
        ScoreReader::Note note = sr.getNote();
        printf("get Note(%d) ==> ", i);
        if(note.note_num == ScoreReader::kCommandScoreEnd) {
            printf("Score End\n");
            break;
        } else {
            printf("dt:%d nn:%d ve:%02x ch:%d\n", note.delta_time, note.note_num, note.velocity, note.ch);
        }
        i++;
    }
    ScoreReader::Note note = sr.getNote();
    if(note.note_num == ScoreReader::kCommandScoreEnd) {
        printf("Score End\n");
    }
    //sr.begin();
}
