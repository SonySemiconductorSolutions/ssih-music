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

#include <SDSink.h>

#include "Button.h"

// this file names are deifned middle C (60) as C4
const SDSink::Item table[12] = {
    {60, "SawLpf/60_C4.wav"},   // C4
    {61, "SawLpf/61_C#4.wav"},  // C#4
    {62, "SawLpf/62_D4.wav"},   // D4
    {63, "SawLpf/63_D#4.wav"},  // D#4
    {64, "SawLpf/64_E4.wav"},   // E4
    {65, "SawLpf/65_F4.wav"},   // F4
    {66, "SawLpf/66_F#4.wav"},  // F#4
    {67, "SawLpf/67_G4.wav"},   // G4
    {68, "SawLpf/68_G#4.wav"},  // G#4
    {69, "SawLpf/69_A4.wav"},   // A4
    {70, "SawLpf/70_A#4.wav"},  // A#4
    {71, "SawLpf/71_B4.wav"}    // B4
};
SDSink inst(table, 12);

Button button4(PIN_D04);
Button button5(PIN_D05);
Button button6(PIN_D06);

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

    Serial.println("Ready to play ButtonDrum");
}

int note1 = INVALID_NOTE_NUMBER;
int note2 = INVALID_NOTE_NUMBER;

int selector = 0;

void loop() {
    if (button4.hasChanged()) {
        if (button4.isPressed()) {
            note1 = 60 + (selector * 2);
            inst.sendNoteOn(note1, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        } else {
            inst.sendNoteOff(note1, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
    }

    if (button5.hasChanged()) {
        if (button5.isPressed()) {
            note2 = 61 + (selector * 2);
            inst.sendNoteOn(note2, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        } else {
            inst.sendNoteOff(note2, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
    }

    if (button6.hasChanged() && button6.isPressed()) {
        selector = (selector + 1) % 6;
    }

    // run instrument
    inst.update();
}
