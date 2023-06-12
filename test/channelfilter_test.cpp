/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include "ChannelFilter.h"
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

TEST(ChannelFilter, ParameterAvailability) {
    DummyFilter f;
    ChannelFilter inst(f);

    EXPECT_TRUE(inst.isAvailable(ChannelFilter::PARAMID_ENABLE_CHANNEL));
    EXPECT_TRUE(inst.isAvailable(ChannelFilter::PARAMID_DISABLE_CHANNEL));
    EXPECT_TRUE(inst.isAvailable(ChannelFilter::PARAMID_CHANNEL_MASK));

    EXPECT_EQ(65535, inst.getParam(ChannelFilter::PARAMID_ENABLE_CHANNEL));
    EXPECT_EQ(65535, inst.getParam(ChannelFilter::PARAMID_DISABLE_CHANNEL));
    EXPECT_EQ(65535, inst.getParam(ChannelFilter::PARAMID_CHANNEL_MASK));

    EXPECT_FALSE(inst.isAvailable(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE));
    EXPECT_FALSE(inst.setParam(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE, 1));
    EXPECT_EQ(0, inst.getParam(YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE));
}

TEST(ChannelFilter, Masking) {
    DummyFilter f;
    ChannelFilter inst(f);

    EXPECT_TRUE(inst.setParam(ChannelFilter::PARAMID_DISABLE_CHANNEL, 3));
    EXPECT_EQ(65531, inst.getParam(ChannelFilter::PARAMID_CHANNEL_MASK));
    EXPECT_TRUE(inst.setParam(ChannelFilter::PARAMID_CHANNEL_MASK, 0));
    EXPECT_EQ(0, inst.getParam(ChannelFilter::PARAMID_CHANNEL_MASK));
    EXPECT_TRUE(inst.setParam(ChannelFilter::PARAMID_ENABLE_CHANNEL, 2));
    EXPECT_EQ(2, inst.getParam(ChannelFilter::PARAMID_CHANNEL_MASK));

    EXPECT_FALSE(inst.setParam(ChannelFilter::PARAMID_DISABLE_CHANNEL, 0));
    EXPECT_FALSE(inst.setParam(ChannelFilter::PARAMID_ENABLE_CHANNEL, 0));
    EXPECT_FALSE(inst.setParam(ChannelFilter::PARAMID_DISABLE_CHANNEL, 17));
    EXPECT_FALSE(inst.setParam(ChannelFilter::PARAMID_ENABLE_CHANNEL, 17));
}

TEST(ChannelFilter, ChannelFiltering) {
    DummyFilter f;
    ChannelFilter inst(f);

    EXPECT_TRUE(inst.setParam(ChannelFilter::PARAMID_CHANNEL_MASK, 0));
    EXPECT_TRUE(inst.setParam(ChannelFilter::PARAMID_ENABLE_CHANNEL, 2));

    EXPECT_TRUE(inst.sendNoteOn(60, 1, 2));
    EXPECT_FALSE(inst.sendNoteOn(60, 1, 5));

    EXPECT_TRUE(inst.sendNoteOff(60, 0, 2));
    EXPECT_FALSE(inst.sendNoteOff(60, 0, 5));
}

TEST(ChannelFilter, ChannelRange) {
    DummyFilter f;
    ChannelFilter inst(f);

    EXPECT_FALSE(inst.sendNoteOn(60, 1, 0));
    EXPECT_FALSE(inst.sendNoteOff(60, 0, 0));
    EXPECT_FALSE(inst.sendNoteOn(60, 1, 17));
    EXPECT_FALSE(inst.sendNoteOff(60, 0, 17));
}
