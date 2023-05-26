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

static uint32_t getTempo(const ScoreParser::MidiMessage& msg) {
    uint32_t tempo = 0;
    for (uint32_t i = 0; i < msg.event_length; i++) {
        tempo = (tempo << 8) | msg.sysex_array[i];
    }
    // printf("tempo:%d\n", tempo);
    return tempo;
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

// 1. Check usable parameter
TEST(ScoreFilter, ScoreFilterTest1) {
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
    TestScoreFilter inst("testdata/SCORE/playlist1.m3u", f);

    EXPECT_TRUE(inst.begin());

    EXPECT_EQ(960, inst.getRootTick());
    EXPECT_EQ(0, inst.getScoreIndex());
    EXPECT_TRUE(inst.isParserAvailable());

    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_NUMBER_OF_SCORES));
    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_ENABLE_TRACK));
    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_DISABLE_TRACK));
    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_TRACK_MASK));
    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_SCORE));
    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_SCORE_NAME));

    EXPECT_EQ(2, inst.getParam(ScoreFilter::PARAMID_NUMBER_OF_SCORES));
    EXPECT_EQ(4294967295, inst.getParam(ScoreFilter::PARAMID_ENABLE_TRACK));
    EXPECT_EQ(4294967295, inst.getParam(ScoreFilter::PARAMID_DISABLE_TRACK));
    EXPECT_EQ(4294967295, inst.getParam(ScoreFilter::PARAMID_TRACK_MASK));
    EXPECT_TRUE(inst.setParam(ScoreFilter::PARAMID_SCORE, 0));
    EXPECT_EQ(0, inst.getParam(ScoreFilter::PARAMID_SCORE));
    EXPECT_STREQ("textscore1.txt", (char*)(inst.getParam(ScoreFilter::PARAMID_SCORE_NAME)));

    EXPECT_FALSE(inst.setParam(ScoreFilter::PARAMID_NUMBER_OF_SCORES, 1));
    EXPECT_TRUE(inst.setParam(ScoreFilter::PARAMID_DISABLE_TRACK, 1));
    EXPECT_EQ(4294967293, inst.getParam(ScoreFilter::PARAMID_TRACK_MASK));
    EXPECT_TRUE(inst.setParam(ScoreFilter::PARAMID_ENABLE_TRACK, 1));
    EXPECT_EQ(4294967295, inst.getParam(ScoreFilter::PARAMID_TRACK_MASK));
    EXPECT_TRUE(inst.setParam(ScoreFilter::PARAMID_TRACK_MASK, 3));
    EXPECT_EQ(3, inst.getParam(ScoreFilter::PARAMID_TRACK_MASK));

    EXPECT_TRUE(inst.setParam(ScoreFilter::PARAMID_SCORE, 1));
    EXPECT_EQ(1, inst.getParam(ScoreFilter::PARAMID_SCORE));
    EXPECT_STREQ("smfscore1.mid", (char*)(inst.getParam(ScoreFilter::PARAMID_SCORE_NAME)));

    EXPECT_EQ(480, inst.getRootTick());
    EXPECT_EQ(2, inst.getNumberOfScores());
    EXPECT_EQ(1, inst.getScoreIndex());
    EXPECT_TRUE(inst.isParserAvailable());

    EXPECT_FALSE(inst.setParam(ScoreFilter::PARAMID_SCORE, 10));

    EXPECT_TRUE(inst.sendNoteOn(60, 1, 1));
    EXPECT_TRUE(inst.sendNoteOn(60, 0, 1));

    EXPECT_TRUE(inst.sendNoteOn(60, 1, 1));
    EXPECT_TRUE(inst.sendNoteOn(64, 1, 1));
    EXPECT_TRUE(inst.sendNoteOn(67, 1, 1));

    uint8_t stop_msg[] = {MIDI_MSG_STOP};
    EXPECT_TRUE(inst.sendMidiMessage(stop_msg, 1));
    uint8_t continue_msg[] = {MIDI_MSG_CONTINUE};
    EXPECT_TRUE(inst.sendMidiMessage(continue_msg, 1));
    EXPECT_TRUE(inst.sendNoteOff(60, 0, 1));

    uint8_t spp_msg[] = {MIDI_MSG_SONG_POSITION_POINTER, 0, 1};
    EXPECT_TRUE(inst.sendMidiMessage(spp_msg, 3));

    uint8_t mtc_msg[] = {MIDI_MSG_SYS_EX_EVENT, 0x7F, 0x7F, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xF7};
    EXPECT_TRUE(inst.sendMidiMessage(mtc_msg, 10));

    EXPECT_TRUE(inst.sendNoteOff(64, 0, 1));
    EXPECT_TRUE(inst.sendNoteOff(67, 0, 1));

    uint8_t song_select_msg[] = {MIDI_MSG_SONG_SELECT, 0};
    EXPECT_TRUE(inst.sendMidiMessage(song_select_msg, 2));

    ScoreParser::MidiMessage msg;
    EXPECT_TRUE(inst.getScoreMidiMessage(&msg));
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_SET_TEMPO, msg.event_code);
    EXPECT_EQ(500000, getTempo(msg));

    EXPECT_TRUE(inst.getScoreMidiMessage(&msg));
    EXPECT_EQ(0, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_ON, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    EXPECT_TRUE(inst.getScoreMidiMessage(&msg));
    EXPECT_EQ(96, msg.delta_time);
    EXPECT_EQ(MIDI_MSG_NOTE_OFF, msg.status_byte);
    EXPECT_EQ(60, msg.data_byte1);

    EXPECT_TRUE(inst.getScoreMidiMessage(&msg));
    EXPECT_EQ((96 * 3), msg.delta_time);
    EXPECT_EQ(MIDI_MSG_META_EVENT, msg.status_byte);
    EXPECT_EQ(MIDI_META_END_OF_TRACK, msg.event_code);
}

// Negative Test
// 2. parser is null
TEST(ScoreFilter, ScoreFilterTest2) {
    DummyFilter f;
    ScoreFilter inst("NOEXIST", f);

    EXPECT_FALSE(inst.begin());

    EXPECT_EQ(960, inst.getRootTick());
    EXPECT_EQ(0, inst.getNumberOfScores());
    EXPECT_EQ(0, inst.getScoreIndex());
    EXPECT_FALSE(inst.isParserAvailable());

    // unused param
    EXPECT_FALSE(inst.isAvailable(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE));
    EXPECT_FALSE(inst.setParam(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE, 1));
    EXPECT_EQ(0, inst.getParam(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE));

    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_NUMBER_OF_SCORES));
    EXPECT_EQ(0, inst.getParam(ScoreFilter::PARAMID_NUMBER_OF_SCORES));
    EXPECT_FALSE(inst.setParam(ScoreFilter::PARAMID_NUMBER_OF_SCORES, 1));

    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_ENABLE_TRACK));
    EXPECT_FALSE(inst.setParam(ScoreFilter::PARAMID_ENABLE_TRACK, 33));
    EXPECT_EQ(4294967295, inst.getParam(ScoreFilter::PARAMID_ENABLE_TRACK));

    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_DISABLE_TRACK));
    EXPECT_FALSE(inst.setParam(ScoreFilter::PARAMID_DISABLE_TRACK, 4294967295));
    EXPECT_EQ(4294967295, inst.getParam(ScoreFilter::PARAMID_DISABLE_TRACK));

    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_TRACK_MASK));
    EXPECT_TRUE(inst.setParam(ScoreFilter::PARAMID_TRACK_MASK, 3333333333));
    EXPECT_EQ(3333333333, inst.getParam(ScoreFilter::PARAMID_TRACK_MASK));

    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_SCORE));
    EXPECT_EQ(0, inst.getParam(ScoreFilter::PARAMID_SCORE));
    EXPECT_FALSE(inst.setParam(ScoreFilter::PARAMID_SCORE, 1));

    EXPECT_TRUE(inst.isAvailable(ScoreFilter::PARAMID_SCORE_NAME));
    EXPECT_EQ((intptr_t) nullptr, inst.getParam(ScoreFilter::PARAMID_SCORE_NAME));
    String str = std::move("NOEXIST");
    EXPECT_FALSE(inst.setParam(ScoreFilter::PARAMID_SCORE_NAME, (intptr_t)str.c_str()));
    EXPECT_FALSE(inst.setScoreIndex(0));
}

TEST(ScoreFilter, ScoreFilterTest3) {
    create_file("testdata/SCORE/text.txt",
                "test text texus\n"
                "");
    DummyFilter f;
    ScoreFilter inst("testdata/SCORE/text.txt", f);

    EXPECT_FALSE(inst.begin());

    EXPECT_EQ(960, inst.getRootTick());
    EXPECT_EQ(0, inst.getNumberOfScores());
    EXPECT_EQ(0, inst.getScoreIndex());
    EXPECT_TRUE(inst.isParserAvailable());
    EXPECT_FALSE(inst.setScoreIndex(0));
}
