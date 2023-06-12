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

TEST(YuruhornSrc, isAvailable) {
    NullFilter dummy = NullFilter();
    YuruhornSrc inst = YuruhornSrc(dummy);
    EXPECT_EQ(true, inst.isAvailable(YuruhornSrc::PARAMID_ACTIVE_LEVEL));
}

TEST(YuruhornSrc, setParam) {
    NullFilter dummy = NullFilter();
    YuruhornSrc inst = YuruhornSrc(dummy);
    EXPECT_EQ(true, inst.setParam(YuruhornSrc::PARAMID_ACTIVE_LEVEL, 100));
}

TEST(YuruhornSrc, getParam) {
    NullFilter dummy = NullFilter();
    YuruhornSrc inst = YuruhornSrc(dummy);
    inst.setParam(YuruhornSrc::PARAMID_ACTIVE_LEVEL, 200);
    EXPECT_EQ(200, inst.getParam(YuruhornSrc::PARAMID_ACTIVE_LEVEL));
}
