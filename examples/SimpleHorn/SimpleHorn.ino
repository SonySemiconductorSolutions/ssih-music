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
#include <YuruhornSrc.h>

// this file names are deifned middle C (60) as C3
const SDSink::Item table[26] = {
    {48, "AUDIO/C2.wav"}, {49, "AUDIO/C#2.wav"},  //< C2, C#2
    {50, "AUDIO/D2.wav"}, {51, "AUDIO/D#2.wav"},  //< D2, D#2
    {52, "AUDIO/E2.wav"},                         //< E2
    {53, "AUDIO/F2.wav"}, {54, "AUDIO/F#2.wav"},  //< F2, F#2
    {55, "AUDIO/G2.wav"}, {56, "AUDIO/G#2.wav"},  //< G2, G#2
    {57, "AUDIO/A2.wav"}, {58, "AUDIO/A#2.wav"},  //< A2, A#2
    {59, "AUDIO/B2.wav"},                         //< B2
    {60, "AUDIO/C3.wav"}, {61, "AUDIO/C#3.wav"},  //< C3, C#3
    {62, "AUDIO/D3.wav"}, {63, "AUDIO/D#3.wav"},  //< D3, D#3
    {64, "AUDIO/E3.wav"},                         //< E3
    {65, "AUDIO/F3.wav"}, {66, "AUDIO/F#3.wav"},  //< F3, F#3
    {67, "AUDIO/G3.wav"}, {68, "AUDIO/G#3.wav"},  //< G3, G#3
    {69, "AUDIO/A3.wav"}, {70, "AUDIO/A#3.wav"},  //< A3, A#3
    {71, "AUDIO/B3.wav"},                         //< B3
    {72, "AUDIO/C4.wav"}, {73, "AUDIO/C#4.wav"}   //< C4, C#4
};

SDSink sink(table, 26);
YuruhornSrc inst(sink);

void setup() {
    // init built-in I/O
    Serial.begin(115200);
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    // initialize memory pool
    initMemoryPools();
    createStaticPools(MEM_LAYOUT_RECORDINGPLAYER);

    // setup instrument
    inst.setParam(SDSink::PARAMID_LOOP, true);  // enable loop
    if (!inst.begin()) {
        Serial.println("ERROR: init error.");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("Ready to play SimpleHorn");
}

void loop() {
    // run instrument
    inst.update();
}
