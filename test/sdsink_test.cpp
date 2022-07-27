/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include <Arduino.h>

#include "SDSink.h"

static const SDSink::Item table[] = {
    {59, "testdata/common/B2.raw"},  //<
    {60, "testdata/common/C3.raw"},  //<
    {62, "testdata/common/D3.raw"}   //<
};

TEST(SDSink, test) {
    SDSink sink = SDSink(table, sizeof(table) / sizeof(table[0]));
    sink.begin();
    sink.sendNoteOn(60, 0, 0);
    sink.update();
    sink.update();
    sink.update();
}
