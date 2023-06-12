/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include "CorrectToneFilter.h"
#include "midi_util.h"
#include "OneKeySynthesizerFilter.h"
#include "ScoreSrc.h"
#include "TimeKeeper.h"
#include "YuruhornSrc.h"

static void create_file(const String& file_path, const String& text) {
    registerDummyFile(file_path, (uint8_t*)text.c_str(), text.length());
}

static void create_file(const String& file_path, const uint8_t* content, int size) {
    registerDummyFile(file_path, content, size);
}

class TestScoreFilter : public ScoreFilter {
public:
    TestScoreFilter(const String& file_name, Filter& filter) : ScoreFilter(file_name, filter) {
    }

    bool getScoreMidiMessage(ScoreParser::MidiMessage* midi_message) {
        return getMidiMessage(midi_message);
    }
};

class DummyFilter : public NullFilter {
public:
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t ch) override {
        return true;
    }

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t ch) override {
        return true;
    }

    bool sendControlChange(uint8_t, uint8_t, uint8_t) {
        return true;
    }

    bool sendProgramChange(uint8_t, uint8_t) {
        return true;
    }
};

// 1. text
TEST(ScoreSrc, ScoreSrc1) {
    DummyFilter f;
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
    ScoreSrc inst("testdata/SCORE_LIST/textscore3.txt", f);

    setTime(0);

    EXPECT_TRUE(inst.begin());

    EXPECT_TRUE(inst.isAvailable(ScoreSrc::PARAMID_STATUS));
    EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreSrc::PLAY));

    int count = 0;
    while (count < 1000) {
        inst.update();
        count++;
        if (inst.getParam(ScoreSrc::PARAMID_STATUS) == ScoreFilter::END) {
            break;
        }
        if (count % 100 == 0 && count != 0) {
            if (inst.getParam(ScoreSrc::PARAMID_STATUS) == ScoreFilter::PLAY) {
                EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreFilter::PAUSE));
            } else if (inst.getParam(ScoreSrc::PARAMID_STATUS) == ScoreFilter::PAUSE) {
                EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreFilter::PLAY));
            }
        }
    }
    EXPECT_LT(1600, getTime());
    EXPECT_GT(5000, getTime());
}

// 2. MIDI
TEST(ScoreSrc, ScoreSrc2) {
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
    DummyFilter f;
    ScoreSrc inst("testdata/SCORE/smfscore4.mid", f);

    setTime(0);

    EXPECT_TRUE(inst.begin());

    EXPECT_TRUE(inst.isAvailable(ScoreSrc::PARAMID_STATUS));
    EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreSrc::PLAY));

    int count = 0;
    while (count < 1000) {
        inst.update();
        count++;
        if (inst.getParam(ScoreSrc::PARAMID_STATUS) == ScoreFilter::END) {
            break;
        }
    }
    EXPECT_GT(1500, getTime());
}

// 3. playlist & continuous playback
TEST(ScoreSrc, ScoreSrc3) {
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
    DummyFilter f;
    ScoreSrc inst("testdata/SCORE/playlist1.m3u", f);

    setTime(0);

    EXPECT_TRUE(inst.begin());

    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_SCORE));
    EXPECT_EQ(0, inst.getParam(ScoreFilter::PARAMID_SCORE));

    EXPECT_TRUE(inst.setParam(ScoreFilter::PARAMID_SCORE, 1));
    EXPECT_EQ(1, inst.getParam(ScoreFilter::PARAMID_SCORE));

    EXPECT_TRUE(inst.setParam(ScoreFilter::PARAMID_SCORE, 0));
    EXPECT_EQ(0, inst.getParam(ScoreFilter::PARAMID_SCORE));

    EXPECT_TRUE(inst.isAvailable(ScoreSrc::PARAMID_CONTINUOUS_PLAYBACK));
    EXPECT_FALSE(inst.getParam(ScoreSrc::PARAMID_CONTINUOUS_PLAYBACK));
    EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_CONTINUOUS_PLAYBACK, true));
    EXPECT_TRUE(inst.getParam(ScoreSrc::PARAMID_CONTINUOUS_PLAYBACK));

    EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreSrc::PLAY));

    int count = 0;

    bool end_flag = false;
    while (count < 5000) {
        inst.update();
        count++;
        if (inst.getParam(ScoreFilter::PARAMID_SCORE) == 1) {
            end_flag = true;
        } else if (inst.getParam(ScoreFilter::PARAMID_SCORE) == 0 && end_flag) {
            break;
        }
    }
    EXPECT_GT(25000, getTime());
}

// 4. use MIDI Message
TEST(ScoreSrc, ScoreSrc4) {
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
    DummyFilter f;
    ScoreSrc inst("testdata/SCORE/smfscore4.mid", f);

    setTime(0);

    EXPECT_TRUE(inst.begin());

    EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreSrc::PLAY));

    uint8_t stop_msg[] = {MIDI_MSG_STOP};
    EXPECT_TRUE(inst.sendMidiMessage(stop_msg, MIDI_MSGLEN_STOP));

    uint8_t con_msg[] = {MIDI_MSG_CONTINUE};
    EXPECT_TRUE(inst.sendMidiMessage(con_msg, MIDI_MSGLEN_CONTINUE));

    int count = 0;
    while (count < 50) {
        inst.update();
        count++;
    }

    uint8_t spp_msg[] = {MIDI_MSG_SONG_POSITION_POINTER, 0, 8};
    EXPECT_TRUE(inst.sendMidiMessage(spp_msg, MIDI_MSGLEN_SONG_POSITION_POINTER));

    count = 0;
    while (count < 50) {
        inst.update();
        count++;
    }

    spp_msg[1] = 127;
    spp_msg[2] = 127;
    EXPECT_TRUE(inst.sendMidiMessage(spp_msg, MIDI_MSGLEN_SONG_POSITION_POINTER));

    EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreSrc::PAUSE));
    EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_SCORE, 0));
    EXPECT_TRUE(inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreSrc::PLAY));

    uint8_t mtc_msg[] = {MIDI_MSG_SYS_EX_EVENT, 0x7F, 0x7F, 0x01, 0x01, 0x00, 0x00, 0x02, 0x00, 0xF7};
    EXPECT_TRUE(inst.sendMidiMessage(mtc_msg, 10));

    mtc_msg[7] = 0x00;
    mtc_msg[8] = 0x10;
    EXPECT_TRUE(inst.sendMidiMessage(mtc_msg, 10));

    mtc_msg[6] = 0x01;
    mtc_msg[8] = 0x00;
    EXPECT_TRUE(inst.sendMidiMessage(mtc_msg, 10));
}
