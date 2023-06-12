/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef ARDUINO_ARCH_SPRESENSE
#error "Board selection is wrong!!"
#endif
#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <ChannelFilter.h>
#include <ScoreSrc.h>
#include <SFZSink.h>

SFZSink sink("SawLpf.sfz");
ChannelFilter filter(sink);
ScoreSrc inst("SCORE", filter);

void setup() {
    // init built-in I/O
    Serial.begin(115200);
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    // setup instrument
    inst.setParam(ScoreSrc::PARAMID_CONTINUOUS_PLAYBACK, 1);
    inst.setParam(ChannelFilter::PARAMID_CHANNEL_MASK, 0xFFFF);
    if (!inst.begin()) {
        Serial.println("ERROR: init error.");
        while (true) {
            delay(1000);
        }
    }
    inst.setParam(ScoreSrc::PARAMID_STATUS, ScoreSrc::PLAY);

    Serial.println("Ready to play MusicBox");
}

void loop() {
    // run instrument
    inst.update();
}
