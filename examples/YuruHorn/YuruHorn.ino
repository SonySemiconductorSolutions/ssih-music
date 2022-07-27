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

#include <MemoryUtil.h>

#include <OctaveShift.h>
#include <SFZSink.h>
#include <YuruhornSrc.h>
#include <YuruInstrumentConsole.h>

SFZSink sink("yuruhorn.sfz");
OctaveShift filter(sink);
YuruhornSrc inst(filter);

YuruInstrumentConsole console(inst);

void setup() {
    // init built-in I/O
    Serial.begin(115200);
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    // initialize memory pools and message libs
    initMemoryPools();
    createStaticPools(MEM_LAYOUT_RECORDINGPLAYER);

    // setup instrument
    console.begin();
    if (!inst.begin()) {
        Serial.println("ERROR: init error.");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("Ready to play YuruHorn");
}

void loop() {
    inst.update();
    console.poll();
}
