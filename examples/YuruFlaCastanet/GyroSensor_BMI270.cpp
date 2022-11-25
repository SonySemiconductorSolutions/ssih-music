/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include "config.h"

#ifdef GYRO_TYPE_BMI270

#include "GyroSensor.h"

#include <Arduino.h>
#include <Wire.h>

#include <bmi270.h>

static const uint16_t kReadWriteLen = 30;  //< The Wire library implementation uses a 32 byte buffer

static struct bmi2_dev g_bmi2;

// bmi2_dev.read
static int8_t bmi270_read(uint8_t dev_id, uint8_t reg_addr, uint8_t* buf, uint16_t len) {
    if (buf == nullptr || len == 0) {
        return -1;
    }

    Wire.beginTransmission(dev_id);
    Wire.write(reg_addr);
    if (Wire.endTransmission() == 0) {
        uint16_t nbytes = Wire.requestFrom((int)dev_id, (int)len);
        if (nbytes > len) {
            nbytes = len;
        }
        for (uint16_t i = 0; i < nbytes; i++) {
            buf[i] = Wire.read();
        }
    } else {
        return -1;
    }
    return BMI2_INTF_RET_SUCCESS;
}

// bmi2_dev.write
static int8_t bmi270_write(uint8_t dev_id, uint8_t reg_addr, const uint8_t* data, uint16_t len) {
    if (data == nullptr || len == 0) {
        return -1;
    }
    Wire.beginTransmission(dev_id);
    Wire.write(reg_addr);
    for (uint16_t i = 0; i < len; i++) {
        Wire.write(data[i]);
    }
    if (Wire.endTransmission() != 0) {
        return -1;
    }
    return BMI2_INTF_RET_SUCCESS;
}

// bmi2_dev.delay_us
static void bmi270_delay_us(uint32_t period, void* intf_ptr) {
    delayMicroseconds(period);
}

static int8_t configureInterrupt(struct bmi2_dev* dev) {
    struct bmi2_int_pin_config int_pin_config;
    int_pin_config.pin_type = BMI2_INT1;
    int_pin_config.int_latch = BMI2_INT_NON_LATCH;
    int_pin_config.pin_cfg[0].lvl = BMI2_INT_ACTIVE_HIGH;
    int_pin_config.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
    int_pin_config.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
    int_pin_config.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;

    int8_t ret = 0;

    ret = bmi2_set_int_pin_config(&int_pin_config, dev);
    if (ret != 0) {
        Serial.print("error: bmi2_set_int_pin_config() => ");
        Serial.println(ret);
        return ret;
    }

    ret = bmi2_map_data_int((uint8_t)BMI2_DRDY_INT, (bmi2_hw_int_pin)int_pin_config.pin_type, dev);
    if (ret != 0) {
        Serial.print("error: bmi2_map_data_int() => ");
        Serial.println(ret);
        return ret;
    }

    return ret;
}

static int8_t configureAccAndGyro(struct bmi2_dev* dev, int gyro_range) {
    struct bmi2_sens_config sens_config[2];
    sens_config[0].type = BMI2_ACCEL;
    sens_config[0].cfg.acc.bwp = BMI2_ACC_OSR2_AVG2;
    sens_config[0].cfg.acc.odr = BMI2_ACC_ODR_100HZ;
    sens_config[0].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;
    sens_config[0].cfg.acc.range = BMI2_ACC_RANGE_4G;
    sens_config[1].type = BMI2_GYRO;
    sens_config[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
    sens_config[1].cfg.gyr.bwp = BMI2_GYR_OSR2_MODE;
    sens_config[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
    if (gyro_range <= 250) {
        sens_config[1].cfg.gyr.range = BMI2_GYR_RANGE_250;
    } else if (gyro_range <= 500) {
        sens_config[1].cfg.gyr.range = BMI2_GYR_RANGE_500;
    } else if (gyro_range <= 1000) {
        sens_config[1].cfg.gyr.range = BMI2_GYR_RANGE_1000;
    } else if (gyro_range <= 2000) {
        sens_config[1].cfg.gyr.range = BMI2_GYR_RANGE_2000;
    }
    sens_config[1].cfg.gyr.ois_range = BMI2_GYR_OIS_2000;

    uint8_t sens_list[2] = {BMI2_ACCEL, BMI2_GYRO};

    int8_t ret = 0;

    ret = bmi2_set_sensor_config(sens_config, sizeof(sens_config) / sizeof(sens_config[0]), dev);
    if (ret != 0) {
        Serial.print("error: bmi2_set_sensor_config() => ");
        Serial.println(ret);
        return ret;
    }

    ret = bmi2_sensor_enable(sens_list, sizeof(sens_list) / sizeof(sens_list[0]), dev);
    if (ret != 0) {
        Serial.print("error: bmi2_sensor_enable() => ");
        Serial.println(ret);
        return ret;
    }

    return ret;
}

static int convertDegreePerSecond(int raw, int range) {
    return raw * range / 32768;
}

GyroSensor::GyroSensor(int range) : gyro_range_(range) {
}

bool GyroSensor::begin() {
    // configure I2C
    Wire.begin();
    g_bmi2.dev_id = BMI2_I2C_SEC_ADDR;
    g_bmi2.read = (bmi2_read_fptr_t)bmi270_read;
    g_bmi2.write = (bmi2_write_fptr_t)bmi270_write;
    g_bmi2.delay_us = (bmi2_delay_fptr_t)bmi270_delay_us;
    g_bmi2.intf = BMI2_I2C_INTF;
    g_bmi2.read_write_len = kReadWriteLen;
    g_bmi2.config_file_ptr = nullptr;

    // initialize BMI270
    int8_t ret = 0;
    ret = bmi270_init(&g_bmi2);
    if (ret != 0) {
        Serial.print("error: bmi270_init() => ");
        Serial.println(ret);
        return false;
    }
    ret = configureInterrupt(&g_bmi2);
    if (ret != 0) {
        return false;
    }
    ret = configureAccAndGyro(&g_bmi2, gyro_range_);
    if (ret != 0) {
        return false;
    }

    return true;
}

bool GyroSensor::get(int* x, int* y, int* z) {
    struct bmi2_sens_data sensor_data;

    int8_t ret = bmi2_get_sensor_data(&sensor_data, &g_bmi2);
    if (ret != 0) {
        Serial.print("error: bmi2_get_sensor_data() => ");
        Serial.println(ret);
        return false;
    }

    *x = convertDegreePerSecond(sensor_data.gyr.x, gyro_range_);
    *y = convertDegreePerSecond(sensor_data.gyr.y, gyro_range_);
    *z = convertDegreePerSecond(sensor_data.gyr.z, gyro_range_);

    return true;
}

#endif  // GYRO_TYPE_BMI270
