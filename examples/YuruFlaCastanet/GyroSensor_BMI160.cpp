/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include "config.h"

#ifdef GYRO_TYPE_BMI160

#include "GyroSensor.h"

#include <BMI160Gen.h>

static int convertDegreePerSecond(int raw, int range) {
    return raw * range / 32768;
}

GyroSensor::GyroSensor(int range) : gyro_range_(range) {
}

bool GyroSensor::begin() {
    // initialize gyro sensor
    BMI160.begin(BMI160GenClass::I2C_MODE);
    uint8_t dev_id = BMI160.getDeviceID();
    Serial.print("DEVICE ID:");
    Serial.println(dev_id);
    BMI160.setGyroRange(gyro_range_);
    Serial.println("end");
    return true;
}

bool GyroSensor::get(int* x, int* y, int* z) {
    int raw_x = 0, raw_y = 0, raw_z = 0;
    BMI160.readGyro(raw_x, raw_y, raw_z);

    *x = convertDegreePerSecond(raw_y, gyro_range_);
    *y = convertDegreePerSecond(raw_x, gyro_range_);
    *z = convertDegreePerSecond(raw_z, gyro_range_);
    return true;
}

#endif  // GYRO_TYPE_BMI160
