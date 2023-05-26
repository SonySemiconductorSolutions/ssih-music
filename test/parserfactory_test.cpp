/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

#include <memory>

#include <gtest/gtest.h>

#include "midi_util.h"
#include "ParserFactory.h"
#include "PlaylistParser.h"
#include "SmfParser.h"
#include "TextScoreParser.h"

static void create_file(const String& file_path, const String& text) {
    registerDummyFile(file_path, (uint8_t*)text.c_str(), text.length());
}

static void create_file(const String& file_path, const uint8_t* content, int size) {
    registerDummyFile(file_path, content, size);
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
// 1. Text Score
TEST(ParserFactory, parserFactoryTest1) {
    create_file("testdata/SCORE/textscore1.txt",
                "#MUSIC_TITLE:test score1\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#MUSIC_END\n"
                "");
    ParserFactory factory;
    std::unique_ptr<ScoreParser> parser(factory.getScoreParser("testdata/SCORE/textscore1.txt"));

    parser->loadScore(0);
    EXPECT_EQ(1, parser->getNumberOfScores());
    EXPECT_EQ(96, parser->getRootTick());
    EXPECT_STREQ("textscore1.txt", parser->getFileName().c_str());
    EXPECT_STREQ("test score1", parser->getTitle(0).c_str());

    ScoreParser::MidiMessage msg;
    parser->getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser->getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser->getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser->getMidiMessage(&msg);
    EXPECT_EQ((96 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

TEST(ParserFactory, parserFactoryTest2) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,                    // "MThd"
                          0x00, 0x00, 0x00, 0x06,                    // length = 6
                          0x00, 0x00,                                // format = 0
                          0x00, 0x01,                                // tracks = 1
                          0x01, 0xE0,                                // division = 480
                          0x4D, 0x54, 0x72, 0x6B,                    // "MTrk" (Track 1)
                          0x00, 0x00, 0x00, 0x14,                    // length = 20
                          0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20,  // [     0] Set Tempo (500,000[us])
                          0x00, 0x90, 0x3C, 0x03,                    // [     0] Note On (n=1, k=60, v=3)
                          0x83, 0x60, 0x80, 0x3C, 0x00,              // [   864] Note Off (n=1, k=60, v=0)
                          0x00, 0xFF, 0x2F, 0x00};                   // [     0] End of Track
    create_file("testdata/SCORE/smfscore1.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    ParserFactory factory;
    std::unique_ptr<ScoreParser> parser(factory.getScoreParser("testdata/SCORE/smfscore1.mid"));

    EXPECT_EQ(1, parser->getNumberOfScores());
    EXPECT_EQ(480, parser->getRootTick());
    EXPECT_STREQ("smfscore1.mid", parser->getFileName().c_str());
    EXPECT_STREQ("smfscore1.mid", parser->getTitle(0).c_str());
    parser->loadScore(0);

    ScoreParser::MidiMessage msg;
    parser->getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser->getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(3, msg.data_byte2);

    parser->getMidiMessage(&msg);
    EXPECT_EQ(480, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser->getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

TEST(ParserFactory, parserFactoryTest3) {
    create_file("testdata/SCORE/textscore1.txt",
                "#MUSIC_TITLE:test score1\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#MUSIC_END\n"
                "");
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,                    // "MThd"
                          0x00, 0x00, 0x00, 0x06,                    // length = 6
                          0x00, 0x00,                                // format = 0
                          0x00, 0x01,                                // tracks = 1
                          0x01, 0xE0,                                // division = 480
                          0x4D, 0x54, 0x72, 0x6B,                    // "MTrk" (Track 1)
                          0x00, 0x00, 0x00, 0x14,                    // length = 20
                          0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20,  // [     0] Set Tempo (500,000[us])
                          0x00, 0x90, 0x3C, 0x03,                    // [     0] Note On (n=1, k=60, v=3)
                          0x83, 0x60, 0x80, 0x3C, 0x00,              // [   864] Note Off (n=1, k=60, v=0)
                          0x00, 0xFF, 0x2F, 0x00};                   // [     0] End of Track
    create_file("testdata/SCORE/smfscore1.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    create_file("testdata/SCORE/playlist1.m3u",
                "textscore1.txt\n"
                "\n"
                "#Smf(MIDI)\n"
                "smfscore1.mid\n"
                "");
    ParserFactory factory;
    std::unique_ptr<ScoreParser> parser(factory.getScoreParser("testdata/SCORE/playlist1.m3u"));

    parser->loadScore(0);
    EXPECT_EQ(2, parser->getNumberOfScores());
    EXPECT_EQ(96, parser->getRootTick());
    EXPECT_STREQ("textscore1.txt", parser->getFileName().c_str());
    EXPECT_STREQ("test score1", parser->getTitle(0).c_str());

    ScoreParser::MidiMessage msg;
    parser->getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser->getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser->getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    parser->getMidiMessage(&msg);
    EXPECT_EQ((96 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);

    parser->loadScore(1);
    EXPECT_EQ(480, parser->getRootTick());
    EXPECT_STREQ("smfscore1.mid", parser->getFileName().c_str());
    EXPECT_STREQ("smfscore1.mid", parser->getTitle(1).c_str());

    parser->getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser->getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(3, msg.data_byte2);

    parser->getMidiMessage(&msg);
    EXPECT_EQ(480, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser->getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}
