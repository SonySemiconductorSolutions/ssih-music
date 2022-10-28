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

#include <BMI160Gen.h>
#include <SFZSink.h>

// sensor cycle
const unsigned long INTERVAL = 250;  // [ms]
unsigned long next = 0;

// gyro sensor parameter
const int GYRO_RANGE = 250;

// instrument
SFZSink inst = SFZSink("FlamencoCastanet.sfz");

// convert gyro sensor raw value to deg/sec
int convertDegreePerSecond(int raw) {
    return raw * GYRO_RANGE / 32768;
}

void setup() {
    // initialize Serial communication
    Serial.begin(115200);
    while (!Serial) {
        ;  // wait for the serial port to open
    }

    // initialize gyro sensor
    BMI160.begin(BMI160GenClass::I2C_MODE);
    uint8_t dev_id = BMI160.getDeviceID();
    Serial.print("DEVICE ID: ");
    Serial.println(dev_id, HEX);
    BMI160.setGyroRange(GYRO_RANGE);

    // initialize instrument
    inst.begin();
    Serial.println("Ready to play Castanet");
}

void loop() {
    unsigned long now = millis();
    if (next == 0) {
        next = now;
    }
    if (next <= now) {
        int raw_x = 0, raw_y = 0, raw_z = 0;
        BMI160.readGyro(raw_x, raw_y, raw_z);

        int x = 0, y = 0, z = 0;  // [deg/sec]
        x = convertDegreePerSecond(raw_y);
        y = convertDegreePerSecond(raw_x);
        z = convertDegreePerSecond(raw_z);

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
            inst.sendNoteOn(note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
        next += INTERVAL;
    }
    inst.update();
}
