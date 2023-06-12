/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <Arduino.h>

class Button {
public:
    Button(int pin) : pin_(pin), prev_stat_(HIGH), now_stat_(HIGH) {
        // init buttons
        pinMode(pin, INPUT_PULLUP);
    }

    ~Button() {
    }

    int read() {
        return digitalRead(pin_);
    }

    bool hasChanged() {
        int button = read();
        if (prev_stat_ != button) {
            delayMicroseconds(10);
            if (button == read()) {
                prev_stat_ = button;
                now_stat_ = prev_stat_;
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    bool isPressed() {
        if (now_stat_ == LOW) {
            return true;
        }
        return false;
    }

private:
    int pin_;
    int prev_stat_;
    int now_stat_;
};

#endif  // BUTTON_H_
