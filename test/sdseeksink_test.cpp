/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include "SDSeekSink.h"

TEST(SDSeekSink, test) {
    SDSeekSink sink = SDSeekSink("testdata/SDSeekSink/soundsrc.tar");
    sink.begin();
    sink.sendNoteOn(60, 0, 0);
    sink.update();
    sink.sendNoteOn(50, 0, 0);
    sink.update();
    sink.sendNoteOn(40, 0, 0);
    sink.update();
    sink.sendNoteOn(60, 0, 0);
    sink.update();
    sink.sendNoteOff(60, 0, 0);
    sink.update();
}
