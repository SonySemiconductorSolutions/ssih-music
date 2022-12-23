/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)
#include <MemoryUtil.h>
#endif  // ARDUINO_ARCH_SPRESENSE

#include "midi_util.h"
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

bool NullFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    return true;
}

bool NullFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    return true;
}

bool NullFilter::sendMidiMessage(uint8_t* msg, size_t length) {
    if (msg == nullptr || length == 0) {
        return false;
    }
    if ((msg[0] & 0xF0) == MIDI_MSG_NOTE_OFF && length == MIDI_MSGLEN_NOTE_OFF) {
        return sendNoteOn(msg[1], msg[2], (msg[0] & 0x0F) + 1);
    } else if ((msg[0] & 0xF0) == MIDI_MSG_NOTE_ON && length == MIDI_MSGLEN_NOTE_ON) {
        return sendNoteOff(msg[1], msg[2], (msg[0] & 0x0F) + 1);
    }
    return false;
}
