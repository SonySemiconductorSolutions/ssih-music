/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include "midi_util.h"
#include "ParserFactory.h"
#include "PlaylistParser.h"
#include "SmfParser.h"
#include "TextScoreParser.h"

static void create_file(const String& file_path, const String& text) {
    registerDummyFile(file_path, (uint8_t*)text.c_str(), text.length());
}

static uint32_t getTempo(const ScoreParser::MidiMessage& msg) {
    uint32_t tempo = 0;
    for (uint32_t i = 0; i < msg.event_length; i++) {
        tempo = (tempo << 8) | msg.sysex_array[i];
    }
    // printf("tempo:%d\n", tempo);
    return tempo;
}

// Positive Test
// 1. undefined music status
TEST(TextScoreParser, textParserTest1) {
    create_file("testdata/SCORE/textscore1.txt",
                "#MUSIC_TITLE:test score1\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#MUSIC_END\n"
                "");
    TextScoreParser parser("testdata/SCORE/textscore1.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore1.txt", parser.getFileName().c_str());
    EXPECT_STREQ("test score1", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((96 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// 2. defined music status
TEST(TextScoreParser, textParserTest2) {
    create_file("testdata/SCORE/textscore2.txt",
                "#MUSIC_TITLE:test score2\n"
                "#MUSIC_RHYTHM:1\n"
                "#MUSIC_BPM:240\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#MUSIC_END\n"
                "");
    TextScoreParser parser("testdata/SCORE/textscore2.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore2.txt", parser.getFileName().c_str());
    EXPECT_STREQ("test score2", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(250000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((48 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// 3. change music status
TEST(TextScoreParser, textParserTest3) {
    create_file("testdata/SCORE_LIST/textscore3.txt",
                "#MUSIC_TITLE:test score3\n"
                "#MUSIC_RHYTHM:1\n"
                "#MUSIC_BPM:240\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#BPMCHANGE 120\n"
                "62,-1,,;\n"
                "#RHYTHMCHANGE 6\n"
                "64,-1,,;\n"
                "#MUSIC_END\n"
                "");
    TextScoreParser parser("testdata/SCORE_LIST/textscore3.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore3.txt", parser.getFileName().c_str());
    EXPECT_STREQ("test score3", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(250000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((48 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(62, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(62, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((48 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(64, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(16, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(64, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((16 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// Negative Test
// 1. Note Invalid
TEST(TextScoreParser, textParserTest4) {
    create_file("testdata/SCORE/textscore4.txt",
                "#MUSIC_TITLE:\n"
                "#MUSIC_START\n"
                "-1,,,;\n"
                "#MUSIC_END\n"
                "");
    TextScoreParser parser("testdata/SCORE/textscore4.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore4.txt", parser.getFileName().c_str());
    EXPECT_STREQ("", parser.getTitle(0).c_str());  // Non title
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ((96 * 4), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// 2. score parameter failed
TEST(TextScoreParser, textParserTest5) {
    create_file("testdata/SCORE/textscore5.txt",
                "#MUSIC_TITLE:test score5\n"
                "#MUSIC_RHYTHM:d\n"
                "#MUSIC_BPM:k\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#MUSIC_END\n"
                "");
    TextScoreParser parser("testdata/SCORE/textscore5.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore5.txt", parser.getFileName().c_str());
    EXPECT_STREQ("test score5", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((96 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// 3. change music status failed
TEST(TextScoreParser, textParserTest6) {
    create_file("testdata/SCORE/textscore6.txt",
                "#MUSIC_TITLE:test score6\n"
                "#MUSIC_RHYTHM:1\n"
                "#MUSIC_BPM:240\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#BPMCHANGE f4\n"
                "62,-1,,;\n"
                "#RHYTHMCHANGE o\n"
                "64,-1,,;\n"
                "#MUSIC_END\n"
                "");
    TextScoreParser parser("testdata/SCORE/textscore6.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore6.txt", parser.getFileName().c_str());
    EXPECT_STREQ("test score6", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(250000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((48 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(250000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(62, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(62, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((48 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(64, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(64, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((96 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// others test
TEST(TextScoreParser, textParserTest7) {
    create_file("testdata/SCORE/textscore7.txt",
                "#MUSIC_TITLE:test score7\n"
                "#MUSIC_BPM:240\n"
                "#MUSIC_START\n"
                "60,-1;\n"
                "#RHYTHMCHANGE 1\n"
                "60,-1;\n"
                "#RHYTHMCHANGE 2\n"
                "60,-1;\n"
                "#RHYTHMCHANGE 3\n"
                "60,-1;\n"
                "#RHYTHMCHANGE 4\n"
                "60,-1;\n"
                "#RHYTHMCHANGE 5\n"
                "60,-1;\n"
                "#RHYTHMCHANGE 6\n"
                "60,-1;\n"
                "#RHYTHMCHANGE 0\n"
                "60,-1;\n"
                "#MUSIC_END\n"
                "");
    TextScoreParser parser("testdata/SCORE/textscore7.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore7.txt", parser.getFileName().c_str());
    EXPECT_STREQ("test score7", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(250000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(128, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(128, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(64, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(64, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(32, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(32, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(16, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(16, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// 8. note straddle bar & change tone
TEST(TextScoreParser, textParserTest8) {
    create_file("testdata/SCORE/textscore8.txt",
                "#MUSIC_TITLE:test score8\n"
                "#MUSIC_TONE:2\n"
                "#MUSIC_START\n"
                "60,-1,60,;\n"
                "// straddle bar\n"
                "#TONECHANGE 1\n"
                "#DELAY 1000\n"
                ",-1,,60;\n"
                "#MUSIC_END\n"
                "");
    TextScoreParser parser("testdata/SCORE/textscore8.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore8.txt", parser.getFileName().c_str());
    EXPECT_STREQ("test score8", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((96 * 3) + (96 * 2), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((96 * 2), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// 9. undefined MUSIC_END
TEST(TextScoreParser, textParserTest9) {
    create_file("testdata/SCORE/textscore9.txt",
                "#MUSIC_TITLE:test score9\n"
                "#MUSIC_TONE:2\n"
                "#MUSIC_START\n"
                "60,-1,60,;\n"
                "// straddle bar\n"
                "#TONECHANGE 1\n"
                ",-1,,60;\n"
                "");
    TextScoreParser parser("testdata/SCORE/textscore9.txt");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("textscore9.txt", parser.getFileName().c_str());
    EXPECT_STREQ("test score9", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((96 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ((96 * 2), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}
