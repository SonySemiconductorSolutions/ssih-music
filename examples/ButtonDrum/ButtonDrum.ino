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

#include <SDSink.h>

// this file names are deifned middle C (60) as C3
const SDSink::Item table[12] = {
    {60, "AUDIO/C3.wav"},   // C3
    {61, "AUDIO/C#3.wav"},  // C#3
    {62, "AUDIO/D3.wav"},   // D3
    {63, "AUDIO/D#3.wav"},  // D#3
    {64, "AUDIO/E3.wav"},   // E3
    {65, "AUDIO/F3.wav"},   // F3
    {66, "AUDIO/F#3.wav"},  // F#3
    {67, "AUDIO/G3.wav"},   // G3
    {68, "AUDIO/G#3.wav"},  // G#3
    {69, "AUDIO/A3.wav"},   // A3
    {70, "AUDIO/A#3.wav"},  // A#3
    {71, "AUDIO/B3.wav"}    // B3
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
