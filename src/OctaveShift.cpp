/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "OctaveShift.h"

#include <Arduino.h>

#ifdef DEBUG
#define debugPrintf printf
#else   // DEBUG
// clang-format off
#define debugPrintf(...) do {} while (0)
// clang-format on
#endif  // DEBUG

OctaveShift::OctaveShift(Filter& filter) : BaseFilter(filter), shift_(0) {
}

void OctaveShift::setShift(int new_shift) {
    shift_ = new_shift;
}

int OctaveShift::getShift() {
    return shift_;
}

bool OctaveShift::setParam(int param_id, intptr_t value) {
    if (param_id == PARAMID_OCTAVE_SHIFT) {
        setShift(value);
        return true;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

intptr_t OctaveShift::getParam(int param_id) {
    if (param_id == PARAMID_OCTAVE_SHIFT) {
        return getShift();
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool OctaveShift::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    return BaseFilter::sendNoteOn(constrain(note + (PITCH_NUM * shift_), NOTE_NUMBER_MIN, NOTE_NUMBER_MAX), velocity, channel);
}

bool OctaveShift::isAvailable(int param_id) {
    if (param_id == PARAMID_OCTAVE_SHIFT) {
        return true;
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

#endif  // ARDUINO_ARCH_SPRESENSE