/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_ARDUINO_H_
#define DUMMY_ARDUINO_H_

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#include <pthread.h>

#include "HardwareSerial.h"
#include "WString.h"

typedef bool boolean;
typedef unsigned int err_t;

#define ERR_OK (0x00000000)
#define ERR_NG (0x80000000)

#define ledOn(x) digitalWrite(x, HIGH)
#define ledOff(x) digitalWrite(x, LOW)

#define abs(x) ((x) > 0 ? (x) : (-x))
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

#define LOW (0)
#define HIGH (1)

#define LED0 (0)
#define LED1 (1)
#define LED2 (2)
#define LED3 (3)
#define PIN_D14 (14)

// Digital I/O

int digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, uint8_t value);

// Characters

int isAlpha(int ch);
int isAlphaNumeric(int ch);
int isAscii(int ch);
int isControl(int ch);
int isDigit(int ch);
int isGraph(int ch);
int isHexadecimalDigit(int ch);
int isLowerCase(int ch);
int isPrintable(int ch);
int isPunct(int ch);
int isSpace(int ch);
int isUpperCase(int ch);
int isWhitespace(int ch);

// Time

uint64_t millis(void);

// Characters

int isDigit(int ch);
int isSpace(int ch);

// Communication

extern HardwareSerial Serial;

#endif  // DUMMY_ARDUINO_H_
