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

// this file names are deifned middle C (60) as C4
const SDSink::Item table[25] = {
    {48, "SawLpf/48_C3.wav"}, {49, "SawLpf/49_C#3.wav"},  //< C3, C#3
    {50, "SawLpf/50_D3.wav"}, {51, "SawLpf/51_D#3.wav"},  //< D3, D#3
    {52, "SawLpf/52_E3.wav"},                             //< E3
    {53, "SawLpf/53_F3.wav"}, {54, "SawLpf/54_F#3.wav"},  //< F3, F#3
    {55, "SawLpf/55_G3.wav"}, {56, "SawLpf/56_G#3.wav"},  //< G3, G#3
    {57, "SawLpf/57_A3.wav"}, {58, "SawLpf/58_A#3.wav"},  //< A3, A#3
    {59, "SawLpf/59_B3.wav"},                             //< B3
    {60, "SawLpf/60_C4.wav"}, {61, "SawLpf/61_C#4.wav"},  //< C4, C#3
    {62, "SawLpf/62_D4.wav"}, {63, "SawLpf/63_D#4.wav"},  //< D4, D#4
    {64, "SawLpf/64_E4.wav"},                             //< E4
    {65, "SawLpf/65_F4.wav"}, {66, "SawLpf/66_F#4.wav"},  //< F4, F#4
    {67, "SawLpf/67_G4.wav"}, {68, "SawLpf/68_G#4.wav"},  //< G4, G#4
    {69, "SawLpf/69_A4.wav"}, {70, "SawLpf/70_A#4.wav"},  //< A4, A#4
    {71, "SawLpf/71_B4.wav"},                             //< B4
    {72, "SawLpf/72_C5.wav"}                              //< C5
};

SDSink sink(table, 25);
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
