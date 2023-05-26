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
static char* parseText(const ScoreParser::MidiMessage& msg) {
    char* c = new char[msg.event_length + 1];
    memset(c, 0x00, msg.event_length + 1);
    memcpy(c, msg.sysex_array, msg.event_length);
    return c;
}

// 1. smf(simple)
TEST(SmfParser, smfParserTest1) {
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
    SmfParser parser("testdata/SCORE/smfscore1.mid");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(480, parser.getRootTick());
    EXPECT_STREQ("smfscore1.mid", parser.getFileName().c_str());
    EXPECT_STREQ("smfscore1.mid", parser.getTitle(0).c_str());
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
    EXPECT_EQ(3, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(480, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// SetTempo & All Channel Message& running status
TEST(SmfParser, smfParserTest2) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,                    // "MThd"
                          0x00, 0x00, 0x00, 0x06,                    // length = 6
                          0x00, 0x00,                                // format = 0
                          0x00, 0x01,                                // tracks = 1
                          0x00, 0x60,                                // division = 96
                          0x4D, 0x54, 0x72, 0x6B,                    // "MTrk" (Track 1)
                          0x00, 0x00, 0x00, 0x25,                    // length =37
                          0x00, 0xFF, 0x51, 0x03, 0x01, 0xE2, 0x40,  // [     0] Set Tempo (123,456[us])
                          0x00, 0x90, 0x3C, 0x03,                    // [     0] NoteOn (n=1, k=60, v=3)
                          0x83, 0x60, 0x3C, 0x00,                    // [   864] NoteOn (n=1, k=60, v=0)
                          0x00, 0xa0, 0x00, 0x01,                    // [     0] Polyphonic Key Pressure (n=1, k=0, v=1)
                          0x00, 0xb0, 0x02, 0x03,                    // [     0] Control Change (n=1, c=2, v=3)
                          0x00, 0xc0, 0x04,                          // [     0] Program Change (n=1, p=4)
                          0x00, 0xd0, 0x05,                          // [     0] Channel Pressure (n=1, v=5)
                          0x00, 0xe0, 0x06, 0x07,                    // [     0] Pitch Bend Change (n=1, l=6, m=7)
                          0x00, 0xFF, 0x2F, 0x00};                   // [     0] End of Track
    create_file("testdata/SCORE_LIST/smfscore2.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    SmfParser parser("testdata/SCORE_LIST/smfscore2.mid");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("smfscore2.mid", parser.getFileName().c_str());
    EXPECT_STREQ("smfscore2.mid", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(123456, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(3, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(480, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_POLYPHONIC_KEY_PRESSURE, msg.status_byte);
    EXPECT_EQ(0, msg.data_byte1);
    EXPECT_EQ(1, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_CONTROL_CHANGE, msg.status_byte);
    EXPECT_EQ(2, msg.data_byte1);
    EXPECT_EQ(3, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_PROGRAM_CHANGE, msg.status_byte);
    EXPECT_EQ(4, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_CHANNEL_PRESSURE, msg.status_byte);
    EXPECT_EQ(5, msg.data_byte1);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_PITCH_BEND_CHANGE, msg.status_byte);
    EXPECT_EQ(6, msg.data_byte1);
    EXPECT_EQ(7, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// Meta Event & SysExEvent
TEST(SmfParser, smfParserTest3) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,                                                        // "MThd"
                          0x00, 0x00, 0x00, 0x06,                                                        // length = 6
                          0x00, 0x00,                                                                    // format = 0
                          0x00, 0x01,                                                                    // tracks = 1
                          0x00, 0x60,                                                                    // division = 96
                          0x4D, 0x54, 0x72, 0x6B,                                                        // "MTrk" (Track 1)
                          0x00, 0x00, 0x00, 0x31,                                                        // length = 49
                          0x00, 0xFF, 0x51, 0x03, 0x01, 0xE2, 0x40,                                      // [     0] Set Tempo (123,456[us])
                          0x00, 0xFF, 0x01, 0x07, 0x50, 0x65, 0x67, 0x61, 0x73, 0x75, 0x73,              // [     0] Text Event ("Pegasus")
                          0x00, 0xF0, 0x0A, 0x52, 0x65, 0x76, 0x6F, 0x6C, 0x75, 0x74, 0x69, 0x6F, 0x6E,  // [     0] SysEx Event ("Revolution")
                          0x01, 0xF7, 0x0A, 0x53, 0x74, 0x61, 0x79, 0x20, 0x47, 0x6F, 0x6C, 0x64, 0xF7,  // [     1] SysEx Event ("Stay Gold") EOX
                          0x00, 0xFF, 0x2F, 0x00};                                                       // [     0] End of Track
    create_file("testdata/SCORE/smfscore3.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    SmfParser parser("testdata/SCORE/smfscore3.mid");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("smfscore3.mid", parser.getFileName().c_str());
    EXPECT_STREQ("smfscore3.mid", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(123456, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_TEXT_EVENT, msg.event_code);
    EXPECT_EQ(7, msg.event_length);
    EXPECT_STREQ("Pegasus", parseText(msg));

    memset(&msg, 0x00, sizeof(msg));
    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_SYS_EX_EVENT, msg.status_byte);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(1, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_END_OF_EXCLUSIVE, msg.status_byte);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// Any Track
TEST(SmfParser, smfParserTest4) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,                                                              // "MThd"
                          0x00, 0x00, 0x00, 0x06,                                                              // length = 6
                          0x00, 0x01,                                                                          // format = 1
                          0x00, 0x02,                                                                          // tracks = 2
                          0x00, 0x60,                                                                          // division = 96
                          0x4D, 0x54, 0x72, 0x6B,                                                              // "MTrk" (Track 1)
                          0x00, 0x00, 0x00, 0x28,                                                              // length = 40
                          0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20,                                            // [     0] Set Tempo (500,000[us])
                          0x00, 0xFF, 0x01, 0x0A, 0x52, 0x65, 0x76, 0x6F, 0x6C, 0x75, 0x74, 0x69, 0x6F, 0x6E,  // [     0] Text Event ("Revolution")
                          0x60, 0xFF, 0x51, 0x03, 0x06, 0x1A, 0x80,                                            // [    96] Set Tempo (400,000[us])
                          0x81, 0x38, 0xFF, 0x51, 0x03, 0x05, 0x16, 0x15,                                      // [   312] Set Tempo (333,333[us])
                          0x00, 0xFF, 0x2F, 0x00,                                                              // [     0] End of Track
                          0x4D, 0x54, 0x72, 0x6B,                                                              // "MTrk" (Track 2)
                          0x00, 0x00, 0x00, 0x17,                                                              // length = 24
                          0x18, 0x91, 0x3C, 0x64,                                                              // [    24] Note On (n=2, k=60, v=100)
                          0x18, 0x3C, 0x00,                                                                    // [    24] Note On (n=2, k=60, v=0)
                          0x60, 0x48, 0x64,                                                                    // [    96] Note On (n=2, k=72, v=100)
                          0x18, 0x48, 0x00,                                                                    // [    24] Note On (n=2, k=72, v=0)
                          0x70, 0x3C, 0x3C,                                                                    // [   112] Note On (n=2, k=60, v=60)
                          0x18, 0x3C, 0x00,                                                                    // [    24] Note On (n=2, k=60, v=0)
                          0x00, 0xFF, 0x2F, 0x00};                                                             // [     0] End of Track
    create_file("testdata/SCORE/smfscore4.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    SmfParser parser("testdata/SCORE/smfscore4.mid");

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("smfscore4.mid", parser.getFileName().c_str());
    EXPECT_STREQ("smfscore4.mid", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_TEXT_EVENT, msg.event_code);
    EXPECT_EQ(10, msg.event_length);
    EXPECT_STREQ("Revolution", parseText(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(100, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(400000, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(48, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(72, msg.data_byte1);
    EXPECT_EQ(100, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(72, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(112, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(333333, getTempo(msg));

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(60, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// select track
TEST(SmfParser, smfParserTest5) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,                                                              // "MThd"
                          0x00, 0x00, 0x00, 0x06,                                                              // length = 6
                          0x00, 0x01,                                                                          // format = 1
                          0x00, 0x02,                                                                          // tracks = 2
                          0x00, 0x60,                                                                          // division = 96
                          0x4D, 0x54, 0x72, 0x6B,                                                              // "MTrk" (Track 1)
                          0x00, 0x00, 0x00, 0x28,                                                              // length = 40
                          0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20,                                            // [     0] Set Tempo (500,000[us])
                          0x00, 0xFF, 0x01, 0x0A, 0x52, 0x65, 0x76, 0x6F, 0x6C, 0x75, 0x74, 0x69, 0x6F, 0x6E,  // [     0] Text Event ("Revolution")
                          0x60, 0xFF, 0x51, 0x03, 0x06, 0x1A, 0x80,                                            // [    96] Set Tempo (400,000[us])
                          0x81, 0x38, 0xFF, 0x51, 0x03, 0x05, 0x16, 0x15,                                      // [   312] Set Tempo (333,333[us])
                          0x00, 0xFF, 0x2F, 0x00,                                                              // [     0] End of Track
                          0x4D, 0x54, 0x72, 0x6B,                                                              // "MTrk" (Track 2)
                          0x00, 0x00, 0x00, 0x17,                                                              // length = 24
                          0x18, 0x91, 0x3C, 0x64,                                                              // [    24] Note On (n=2, k=60, v=100)
                          0x18, 0x3C, 0x00,                                                                    // [    24] Note On (n=2, k=60, v=0)
                          0x60, 0x48, 0x64,                                                                    // [    96] Note On (n=2, k=72, v=100)
                          0x18, 0x48, 0x00,                                                                    // [    24] Note On (n=2, k=72, v=0)
                          0x70, 0x3C, 0x3C,                                                                    // [   112] Note On (n=2, k=60, v=60)
                          0x18, 0x3C, 0x00,                                                                    // [    24] Note On (n=2, k=60, v=0)
                          0x00, 0xFF, 0x2F, 0x00};                                                             // [     0] End of Track
    create_file("testdata/SCORE/smfscore4.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    SmfParser parser("testdata/SCORE/smfscore4.mid");  // use smfscore4.mid

    EXPECT_EQ(1, parser.getNumberOfScores());
    EXPECT_EQ(96, parser.getRootTick());
    EXPECT_STREQ("smfscore4.mid", parser.getFileName().c_str());
    EXPECT_STREQ("smfscore4.mid", parser.getTitle(0).c_str());
    parser.setDisableTrack(10);
    EXPECT_EQ(4294966271, parser.getPlayTrack());
    parser.setPlayTrack(0);
    parser.setEnableTrack(1);
    EXPECT_EQ(2, parser.getPlayTrack());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(100, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(72, msg.data_byte1);
    EXPECT_EQ(100, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(72, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(112, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(60, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(24, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON + 1, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);
    EXPECT_EQ(0, msg.data_byte2);

    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// Negative Test
// 6. smf(simple)
TEST(SmfParser, smfParserTest6) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x67, 0x64,                    // "MTgd" !!! wrong chunk ID !!!
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
    create_file("testdata/SCORE/smfscore6.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    SmfParser parser("testdata/SCORE/smfscore6.mid");

    EXPECT_EQ(0, parser.getNumberOfScores());
    EXPECT_EQ(0, parser.getRootTick());
    EXPECT_STREQ("smfscore6.mid", parser.getFileName().c_str());
    EXPECT_STREQ("", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// 7. smf(simple)
TEST(SmfParser, smfParserTest7) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,                    // "MThd"
                          0x00, 0x00, 0x00, 0x06,                    // length = 6
                          0x00, 0x00,                                // format = 0
                          0x00, 0x01,                                // tracks = 1
                          0x01, 0xE0,                                // division = 480
                          0x4D, 0x54, 0x71, 0x6B,                    // "MTsk" !!! wrong chunk ID !!!
                          0x00, 0x00, 0x00, 0x14,                    // length = 20
                          0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20,  // [     0] Set Tempo (500,000[us])
                          0x00, 0x90, 0x3C, 0x03,                    // [     0] Note On (n=1, k=60, v=3)
                          0x83, 0x60, 0x80, 0x3C, 0x00,              // [   864] Note Off (n=1, k=60, v=0)
                          0x00, 0xFF, 0x2F, 0x00};                   // [     0] End of Track
    create_file("testdata/SCORE/smfscore7.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    SmfParser parser("testdata/SCORE/smfscore7.mid");

    EXPECT_EQ(0, parser.getNumberOfScores());
    EXPECT_EQ(480, parser.getRootTick());
    EXPECT_STREQ("smfscore7.mid", parser.getFileName().c_str());
    EXPECT_STREQ("", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// 8. smf(simple)
TEST(SmfParser, smfParserTest8) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,                    // "MThd"
                          0x00, 0x00, 0x00, 0x06,                    // length = 6
                          0x00, 0x00,                                // format = 0
                          0x00, 0x01,                                // tracks = 1
                          0x81, 0xE0,                                // division = SMPTE
                          0x4D, 0x54, 0x72, 0x6B,                    // "MTrk" (Track 1)
                          0x00, 0x00, 0x00, 0x14,                    // length = 20
                          0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20,  // [     0] Set Tempo (500,000[us])
                          0x00, 0x90, 0x3C, 0x03,                    // [     0] Note On (n=1, k=60, v=3)
                          0x83, 0x60, 0x80, 0x3C, 0x00,              // [   864] Note Off (n=1, k=60, v=0)
                          0x00, 0xFF, 0x2F, 0x00};                   // [     0] End of Track
    create_file("testdata/SCORE/smfscore8.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    SmfParser parser("testdata/SCORE/smfscore8.mid");
    EXPECT_EQ(0, parser.getRootTick());
}

// 9. smf(undefined track)
TEST(SmfParser, smfParserTest9) {
    uint8_t smf_data[] = {0x4D, 0x54, 0x68, 0x64,  // "MThd"
                          0x00, 0x00, 0x00, 0x06,  // length = 6
                          0x00, 0x00,              // format = 0
                          0x00, 0x01,              // tracks = 1
                          0x01, 0xE0};             // division = 480
    create_file("testdata/SCORE/smfscore9.mid", smf_data, sizeof(smf_data) / sizeof(smf_data[0]));
    SmfParser parser("testdata/SCORE/smfscore9.mid");

    EXPECT_EQ(0, parser.getNumberOfScores());
    EXPECT_EQ(480, parser.getRootTick());
    EXPECT_STREQ("smfscore9.mid", parser.getFileName().c_str());
    EXPECT_STREQ("", parser.getTitle(0).c_str());
    parser.loadScore(0);

    ScoreParser::MidiMessage msg;
    parser.getMidiMessage(&msg);
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}
