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

bool NullFilter::isAvailable(int) {
    return false;
}

intptr_t NullFilter::getParam(int) {
    return 0;
}

bool NullFilter::setParam(int, intptr_t) {
    return false;
}

bool NullFilter::sendNoteOff(uint8_t, uint8_t, uint8_t) {
    return false;
}

bool NullFilter::sendNoteOn(uint8_t, uint8_t, uint8_t) {
    return false;
}

bool NullFilter::sendControlChange(uint8_t, uint8_t, uint8_t) {
    return false;
}

bool NullFilter::sendProgramChange(uint8_t, uint8_t) {
    return false;
}

bool NullFilter::sendSongPositionPointer(uint16_t /*beats*/) {
    return true;
}

bool NullFilter::sendSongSelect(uint8_t /*song*/) {
    return true;
}

bool NullFilter::sendStart() {
    bool ret = true;
    ret = sendSongPositionPointer(0);
    if (ret) {
        ret = sendContinue();
    }
    return ret;
}

bool NullFilter::sendContinue() {
    return true;
}

bool NullFilter::sendStop() {
    return true;
}

bool NullFilter::sendMidiMessage(uint8_t* msg, size_t length) {
    if (msg == nullptr || length == 0) {
        return false;
    }
    if ((msg[0] & 0xF0) == MIDI_MSG_CONTROL_CHANGE && length >= MIDI_MSGLEN_CONTROL_CHANGE) {
        return sendControlChange(msg[1], msg[2], (msg[0] & 0x0F) + 1);
    } else if ((msg[0] & 0xF0) == MIDI_MSG_PROGRAM_CHANGE && length >= MIDI_MSGLEN_PROGRAM_CHANGE) {
        return sendProgramChange(msg[1], (msg[0] & 0x0F) + 1);
    } else if (msg[0] == MIDI_MSG_SONG_POSITION_POINTER && length >= MIDI_MSGLEN_SONG_POSITION_POINTER) {
        return sendSongPositionPointer(((msg[2] & 0x7F) << 7) | ((msg[1] & 0x7F) << 0));
    } else if (msg[0] == MIDI_MSG_SONG_SELECT && length >= MIDI_MSGLEN_SONG_SELECT) {
        return sendSongSelect(msg[1] & 0x7F);
    } else if (msg[0] == MIDI_MSG_START && length >= MIDI_MSGLEN_START) {
        return sendStart();
    } else if (msg[0] == MIDI_MSG_CONTINUE && length >= MIDI_MSGLEN_CONTINUE) {
        return sendContinue();
    } else if (msg[0] == MIDI_MSG_STOP && length >= MIDI_MSGLEN_STOP) {
        return sendStop();
    }
    return false;
}
