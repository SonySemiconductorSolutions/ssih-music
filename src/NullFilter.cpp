/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)
#include <MemoryUtil.h>
#endif  // ARDUINO_ARCH_SPRESENSE

#include "YuruInstrumentFilter.h"

NullFilter::NullFilter() {
}

NullFilter::~NullFilter() {
}

bool NullFilter::begin() {
#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)
    // initialize memory pools and message libs
    initMemoryPools();
    createStaticPools(MEM_LAYOUT_RECORDINGPLAYER);
#endif  // ARDUINO_ARCH_SPRESENSE
    return true;
}

void NullFilter::update() {
}

bool NullFilter::isAvailable(int param_id) {
    return false;
}

intptr_t NullFilter::getParam(int param_id) {
    return 0;
}

bool NullFilter::setParam(int param_id, intptr_t value) {
    return false;
}

bool NullFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    return true;
}

bool NullFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    return true;
}
