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

#include <OneKeySynthesizerFilter.h>
#include <SFZSink.h>

#include "Button.h"

SFZSink sink("SawLpf.sfz");
OneKeySynthesizerFilter inst("SCORE", sink);

Button button4(PIN_D04);

int PLAY_CHANNEL = 1;

void setup() {
    // init built-in I/O
    Serial.begin(115200);
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

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
    if (button4.hasChanged()) {
        if (button4.isPressed()) {
            inst.sendNoteOn(OneKeySynthesizerFilter::NOTE_ALL, DEFAULT_VELOCITY, PLAY_CHANNEL);
        } else {
            inst.sendNoteOff(OneKeySynthesizerFilter::NOTE_ALL, DEFAULT_VELOCITY, PLAY_CHANNEL);
        }
    }

    // run instrument
    inst.update();
}
