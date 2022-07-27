/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include "gtest/gtest.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <Arduino.h>

#include "YuruhornSrc.h"

class DummyFilter : public NullFilter {
private:
public:
    DummyFilter() : NullFilter() {
    }
    ~DummyFilter() {
    }
    bool setParam(int param_id, intptr_t value) override {
        return true;
    }
    intptr_t getParam(int param_id) override {
        return param_id;
    }
};

DummyFilter dummy = DummyFilter();
YuruhornSrc yurusrc = YuruhornSrc(dummy);

TEST(YuruhornSrc, isAvailable) {
    EXPECT_EQ(1, yurusrc.isAvailable(YuruhornSrc::PARAMID_ACTIVE_LEVEL));
}

TEST(YuruhornSrc, sendNoteOn) {
    EXPECT_EQ(1, yurusrc.sendNoteOn(60, 100, 1));
}

TEST(YuruhornSrc, setParam) {
    EXPECT_EQ(1, yurusrc.setParam(2, 100));
}

TEST(YuruhornSrc, getParam) {
    EXPECT_EQ(77, yurusrc.getParam(77));
}
