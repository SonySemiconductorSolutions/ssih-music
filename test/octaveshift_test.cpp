/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include "OctaveShift.h"
#include "YuruhornSrc.h"

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

TEST(OctaveShift, ParameterAvailability) {
    DummyFilter f;
    OctaveShift inst(f);

    // supported parameters
    EXPECT_TRUE(inst.isAvailable(OctaveShift::PARAMID_OCTAVE_SHIFT));
    EXPECT_EQ(0, inst.getParam(OctaveShift::PARAMID_OCTAVE_SHIFT));
    EXPECT_TRUE(inst.setParam(OctaveShift::PARAMID_OCTAVE_SHIFT, -1));
    EXPECT_EQ(-1, inst.getParam(OctaveShift::PARAMID_OCTAVE_SHIFT));

    // unsupported parameters
    EXPECT_FALSE(inst.isAvailable(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE));
    EXPECT_FALSE(inst.setParam(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE, 1));
    EXPECT_EQ(0, inst.getParam(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE));
}

TEST(OctaveShift, OctaveShiftTest1) {
    DummyFilter f;
    OctaveShift inst(f);

    // handle MIDI messages
    EXPECT_TRUE(inst.sendNoteOn(60, 1, 0));
    EXPECT_TRUE(inst.sendNoteOff(60, 0, 0));
}
