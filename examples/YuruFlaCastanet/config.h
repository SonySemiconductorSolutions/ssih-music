/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

// #define GYRO_TYPE_BMI160
// #define GYRO_TYPE_BMI270

// configure by GYRO_TYPE envvar
#if defined(GYRO_TYPE)
#if GYRO_TYPE == BMI160
#if !defined(GYRO_TYPE_BMI160)
#define GYRO_TYPE_BMI160
#endif
#elif GYRO_TYPE == BMI270
#if !defined(GYRO_TYPE_BMI270)
#define GYRO_TYPE_BMI270
#endif
#endif
#endif  // defined(GYRO_TYPE)

// default configuration
#if !defined(GYRO_TYPE_BMI160) && !defined(GYRO_TYPE_BMI270)
#define GYRO_TYPE_BMI270
#endif  // !defined(GYRO_TYPE_BMI160) && !defined(GYRO_TYPE_BMI270)
