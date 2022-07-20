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

#include <OneKeySynthesizerFilter.h>
#include <SFZSink.h>

SFZSink sink("SawLpf.sfz");
OneKeySynthesizerFilter inst("SCORE", sink);

int button4 = HIGH;
int button5 = HIGH;

void setup() {
    // init built-in I/O
    Serial.begin(115200);
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    // init buttons
    pinMode(PIN_D04, INPUT_PULLUP);
    pinMode(PIN_D05, INPUT_PULLUP);

    // initialize memory pool
    initMemoryPools();
    createStaticPools(MEM_LAYOUT_RECORDINGPLAYER);

    // setup instrument
    if (!inst.begin()) {
        Serial.println("ERROR: init error.");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("Ready to play OneKeySynth");
}

void loop() {
    int button4_input = digitalRead(PIN_D04);
    if (button4_input != button4) {
        if (button4_input == LOW) {
            inst.sendNoteOn(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        } else {
            inst.sendNoteOff(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
        button4 = button4_input;
    }

    int button5_input = digitalRead(PIN_D05);
    if (button5_input != button5) {
        if (button5_input == LOW) {
            inst.sendNoteOn(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        } else {
            inst.sendNoteOff(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
        button5 = button5_input;
    }

    // run instrument
    inst.update();
}
