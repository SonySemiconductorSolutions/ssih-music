/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_PRINT_H_
#define DUMMY_PRINT_H_

#include <stdio.h>

#include "Arduino.h"
#include "WString.h"

#define DEC (10)
#define HEX (16)
#define OCT (8)

class Print {
public:
    size_t print(const char *val);
    size_t print(const String &val);
    size_t print(char val);
    size_t print(unsigned char val, int base = DEC);
    size_t print(int val, int base = DEC);
    size_t print(unsigned int val, int base = DEC);
    size_t print(long val, int base = DEC);
    size_t print(unsigned long val, int base = DEC);
    size_t print(double val, int decimalPlaces = 2);

    size_t println(const char *val);
    size_t println(const String &val);
    size_t println(char val);
    size_t println(unsigned char val, int base = DEC);
    size_t println(int val, int base = DEC);
    size_t println(unsigned int val, int base = DEC);
    size_t println(long val, int base = DEC);
    size_t println(unsigned long val, int base = DEC);
    size_t println(double val, int decimalPlaces = 2);

    size_t println(void);

    virtual size_t write(uint8_t val) = 0;
    size_t write(const char *str);
    virtual size_t write(const uint8_t *buf, size_t len);
    size_t write(const char *buf, size_t len);
};

#endif  // DUMMY_PRINT_H_
