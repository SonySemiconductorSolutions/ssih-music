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

int selector = 0;

int button4 = HIGH;
int button5 = HIGH;
int button6 = HIGH;

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
    pinMode(PIN_D06, INPUT_PULLUP);

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

void loop() {
    int button4_input = digitalRead(PIN_D04);
    if (button4_input != button4) {
        if (button4_input == LOW) {
            note1 = 60 + (selector * 2);
            inst.sendNoteOn(note1, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        } else {
            inst.sendNoteOff(note1, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
        button4 = button4_input;
    }

    int button5_input = digitalRead(PIN_D05);
    if (button5_input != button5) {
        if (button5_input == LOW) {
            note2 = 61 + (selector * 2);
            inst.sendNoteOn(note2, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        } else {
            inst.sendNoteOff(note2, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
        button5 = button5_input;
    }

    int button6_input = digitalRead(PIN_D06);
    if (button6_input != button6) {
        if (button6_input == LOW) {
            selector = (selector + 1) % 6;
        }
        button6 = button6_input;
    }

    // run instrument
    inst.update();
}
