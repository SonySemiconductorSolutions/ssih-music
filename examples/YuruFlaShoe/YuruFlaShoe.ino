/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include <Wire.h>

#include <KX122.h>
#include <SFZSink.h>

// sensor cycle
const unsigned long INTERVAL = 20;  // [ms]
unsigned long next = 0;

// accelerometer
KX122 kx122(KX122_DEVICE_ADDRESS_1F);
const int BUF_LENGTH = 4;
int ax[BUF_LENGTH];
int ay[BUF_LENGTH];
int az[BUF_LENGTH];
int last_index = 0;
int base_ax = 0;
int base_ay = 0;
int base_az = 0;

// instrument
SFZSink inst = SFZSink("FlamencoShoe.sfz");

void sense() {
    float raw[3];  // [m/s2]
    byte ret = kx122.get_val(raw);
    if (ret == 0) {
        last_index = (last_index + 1) % BUF_LENGTH;
        ax[last_index] = (int)(raw[0] * 1000) - base_ax;  // [mm/s2]
        ay[last_index] = (int)(raw[1] * 1000) - base_ay;  // [mm/s2]
        az[last_index] = (int)(raw[2] * 1000) - base_az;  // [mm/s2]
    }
}

void setup() {
    // initialize Serial communication
    Serial.begin(115200);
    while (!Serial) {
        ;  // wait for the serial port to open
    }

    // initialize accelerometer
    Wire.begin();
    byte ret = 0;
    ret = kx122.init();
    if (ret != 0) {
        Serial.println("KX122.init() failed");
    }
    for (int i = 0; i < BUF_LENGTH; i++) {
        sense();
        delay(INTERVAL);
    }
    while (true) {
        int min_ax = ax[0], max_ax = ax[0];
        int min_ay = ay[0], max_ay = ay[0];
        int min_az = az[0], max_az = az[0];
        for (int i = 0; i < BUF_LENGTH; i++) {
            min_ax = (min_ax < ax[i]) ? min_ax : ax[i];
            max_ax = (max_ax > ax[i]) ? max_ax : ax[i];
            min_ay = (min_ay < ay[i]) ? min_ay : ay[i];
            max_ay = (max_ay > ay[i]) ? max_ay : ay[i];
            min_az = (min_az < az[i]) ? min_az : az[i];
            max_az = (max_az > az[i]) ? max_az : az[i];
        }
        if (max_ax - min_ax < 3 && max_ay - min_ay < 3 && max_az - min_az < 3) {
            base_ax = min_ax;
            base_ay = min_ay;
            base_az = min_az;
            break;
        }
        sense();
        delay(INTERVAL);
    }

    // initialize instrument
    inst.begin();
    Serial.println("Ready to play Shoe");
}

void loop() {
    const int TACON_THRESHOLD_X = 200;   // [ms/s2]
    const int TACON_THRESHOLD_Z = -200;  // [ms/s2]
    const int PLANTER_THRESHOLD = 800;   // [ms/s2]

    unsigned long now = millis();
    if (next == 0) {
        next = now;
    }
    if (next <= now) {
        int min_ax = ax[0], max_ax = ax[0];
        int min_ay = ay[0], max_ay = ay[0];
        int min_az = az[0], max_az = az[0];
        bool planter_ready = true, tacon_ready = true;
        for (int i = 0; i < BUF_LENGTH; i++) {
            min_ax = (min_ax < ax[i]) ? min_ax : ax[i];
            max_ax = (max_ax > ax[i]) ? max_ax : ax[i];
            min_ay = (min_ay < ay[i]) ? min_ay : ay[i];
            max_ay = (max_ay > ay[i]) ? max_ay : ay[i];
            min_az = (min_az < az[i]) ? min_az : az[i];
            max_az = (max_az > az[i]) ? max_az : az[i];
            planter_ready = planter_ready && (ax[i] <= PLANTER_THRESHOLD);
            tacon_ready = tacon_ready && ((ax[i] <= TACON_THRESHOLD_X) && (az[i] >= TACON_THRESHOLD_Z));
        }

        sense();
        int prev_index = (last_index + BUF_LENGTH - 1) % BUF_LENGTH;
        if (planter_ready && ax[last_index] > PLANTER_THRESHOLD) {
            Serial.println("planter(toe)");
            inst.sendNoteOn(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }

        if (tacon_ready && ((ax[last_index] - ax[prev_index]) > TACON_THRESHOLD_X && (az[last_index] - az[prev_index]) < TACON_THRESHOLD_Z)) {
            Serial.println("tacon(heel)");
            inst.sendNoteOn(62, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
        Serial.print(ax[last_index] - ax[prev_index]);
        Serial.print(",");
        Serial.print(az[last_index] - az[prev_index]);
        Serial.println();

        next += INTERVAL;
    }
    inst.update();
}
