/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef OCTAVE_SHIFT_H_
#define OCTAVE_SHIFT_H_

#include "YuruInstrumentFilter.h"

/// Octave shift filter
class OctaveShift : public BaseFilter {
public:
    enum ParamId {  // MAGIC CHAR = 'O'
        PARAMID_OCTAVE_SHIFT = ('O' << 8)
    };

    OctaveShift(Filter& filter);

    void setShift(int new_shift);

    int getShift();

    bool isAvailable(int param_id) override;

    intptr_t getParam(int param_id) override;

    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

private:
    int shift_;
};

#endif  // OCTAVE_SHIFT_H_
