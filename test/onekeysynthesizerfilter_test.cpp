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

TEST(OneKeySynthesizerFilter, ParameterAvailability) {
    DummyFilter f;
    OneKeySynthesizerFilter inst("NOEXIST", f);

    EXPECT_FALSE(inst.begin());

    // unused param
    EXPECT_FALSE(inst.isAvailable(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE));
    EXPECT_FALSE(inst.setParam(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE, 1));
    EXPECT_EQ(0, inst.getParam(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE));

    inst.update();
}

// 1. OneKeySynth Positive Test
TEST(OneKeySynthesizerFilter, OneKeySynthTest1) {
    create_file("testdata/SCORE/textscore1.txt",
                "#MUSIC_TITLE:test score1\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#MUSIC_END\n"
                "");
    DummyFilter f;
    OneKeySynthesizerFilter inst("testdata/SCORE/textscore1.txt", f);

    EXPECT_TRUE(inst.begin());

    EXPECT_TRUE(inst.sendNoteOn(0, 0, 1));
    EXPECT_TRUE(inst.sendNoteOff(0, 0, 1));
    inst.update();
    EXPECT_TRUE(inst.sendNoteOn(0, 0, 1));
    EXPECT_TRUE(inst.sendNoteOff(0, 0, 1));
    inst.update();
}

// 2. OneKeySynth Error Test
TEST(OneKeySynthesizerFilter, OneKeySynthTest2) {
    create_file("testdata/SCORE/textscore1.txt",
                "#MUSIC_TITLE:test score1\n"
                "#MUSIC_START\n"
                "60,-1,,;\n"
                "#MUSIC_END\n"
                "");
    DummyFilter f;
    OneKeySynthesizerFilter inst("testdata/SCORE/textscore1.txt", f);

    EXPECT_TRUE(inst.begin());

    EXPECT_FALSE(inst.sendNoteOn(8, 0, 1));
    EXPECT_FALSE(inst.sendNoteOff(8, 0, 1));
    EXPECT_FALSE(inst.sendNoteOn(0, 0, 0));
    EXPECT_FALSE(inst.sendNoteOff(0, 0, 0));
    EXPECT_FALSE(inst.sendNoteOn(0, 0, 17));
    EXPECT_FALSE(inst.sendNoteOff(0, 0, 17));
}
