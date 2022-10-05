/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include <SDHCI.h>

#include "waveParser/WavReader.h"

TEST(Wave, test) {
    SDClass sd;
    if (!sd.begin()) {
        printf("SD: begin error.\n");
    } else {
        printf("SD: open success.\n");
    }
    File file = sd.open("testdata/WAVE/D2.wav");

    WavReader* wr = new WavReader();
    WavReader::WaveData wd = wr->load(file);

    printf("music data start:%d Byte\nmusic data length:%d\n", wd.pos, wd.size);
}
