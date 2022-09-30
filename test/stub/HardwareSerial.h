/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_HARDWARE_SERIAL_H_
#define DUMMY_HARDWARE_SERIAL_H_

#include <stdio.h>

#include "Arduino.h"
#include "Stream.h"

class HardwareSerial : public Stream {
public:
    HardwareSerial(uint8_t channel = 0);
    operator bool(void) const;
    virtual int available(void);
    int availableForWrite(void);
    void begin(unsigned long baud);
    void begin(unsigned long baud, uint16_t config);
    void end(void);
    virtual void flush(void);
    virtual int peek(void);
    virtual int read(void);
    virtual size_t write(uint8_t val);
    virtual size_t write(const char* str);
    virtual size_t write(const uint8_t* buf, size_t len);
};

#endif  // DUMMY_HARDWARE_SERIAL_H_
