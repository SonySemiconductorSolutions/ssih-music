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

#include "GyroSensor.h"

#include <SFZSink.h>

// sensor cycle
const unsigned long INTERVAL = 250;  // [ms]
unsigned long next = 0;

// gyro sensor parameter
const int GYRO_RANGE = 250;

// instrument
SFZSink inst = SFZSink("FlamencoCastanet.sfz");
GyroSensor sensor(GYRO_RANGE);

void setup() {
    // initialize Serial communication
    Serial.begin(115200);
    while (!Serial) {
        ;  // wait for the serial port to open
    }

    sensor.begin();

    // initialize instrument
    inst.begin();
    Serial.println("Ready to play Castanet");
}

int playing_note = INVALID_NOTE_NUMBER;

void loop() {
    unsigned long now = millis();
    if (next == 0) {
        next = now;
    }
    if (next <= now) {
        int x = 0, y = 0, z = 0;  // [deg/sec]
        sensor.get(&x, &y, &z);
        int x_thresh = 150, y_thresh = 150, z_thresh = 150;  // [deg/sec]
        int note = INVALID_NOTE_NUMBER;
        if (x >= x_thresh) {
            Serial.println("PITCH(CW) => roll");
            note = 70;
        } else if (x <= -x_thresh) {
            Serial.println("PITCH(CCW) => triplet");
            note = 73;
        } else if (y >= y_thresh) {
            Serial.println("ROLL(CW) => rhythm");
            note = 71;
        } else if (y <= -y_thresh) {
            Serial.println("ROLL(CCW) => triplet");
            note = 73;
        } else if (z >= z_thresh) {
            Serial.println("YAW(CW) => triplet");
            note = 73;
        } else if (z <= -z_thresh) {
            Serial.println("YAW(CCW) => triplet");
            note = 73;
        }
        if (note != INVALID_NOTE_NUMBER) {
            if (playing_note != INVALID_NOTE_NUMBER) {
                inst.sendNoteOff(playing_note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
            }
            inst.sendNoteOn(note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
            playing_note = note;
        }
        next += INTERVAL;
    }
    inst.update();
}
