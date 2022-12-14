/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef GYRO_SENSOR_H_
#define GYRO_SENSOR_H_

class GyroSensor {
public:
    GyroSensor(int range);
    bool begin();
    bool get(int* x, int* y, int* z);

private:
    int gyro_range_;
};

#endif  // GYRO_SENSOR_H_
