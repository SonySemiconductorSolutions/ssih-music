/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include "YuruInstrumentFilter.h"

BaseFilter::BaseFilter(Filter& filter) : NullFilter(), next_filter_(&filter) {
}

BaseFilter::~BaseFilter() {
}

bool BaseFilter::begin() {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->begin();
}

void BaseFilter::update() {
    if (next_filter_ == nullptr) {
        return;
    }
    next_filter_->update();
}

bool BaseFilter::isAvailable(int param_id) {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->isAvailable(param_id);
}

intptr_t BaseFilter::getParam(int param_id) {
    if (next_filter_ == nullptr) {
        return 0;
    }
    return next_filter_->getParam(param_id);
}

bool BaseFilter::setParam(int param_id, intptr_t value) {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->setParam(param_id, value);
}

bool BaseFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->sendNoteOff(note, velocity, channel);
}

bool BaseFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->sendNoteOn(note, velocity, channel);
}

bool BaseFilter::sendSongPositionPointer(uint16_t beats) {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->sendSongPositionPointer(beats);
}

bool BaseFilter::sendSongSelect(uint8_t song) {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->sendSongSelect(song);
}

bool BaseFilter::sendStart() {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->sendStart();
}

bool BaseFilter::sendContinue() {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->sendContinue();
}

bool BaseFilter::sendStop() {
    if (next_filter_ == nullptr) {
        return false;
    }
    return next_filter_->sendStop();
}

bool BaseFilter::sendMidiMessage(uint8_t* msg, size_t length) {
    if (msg == nullptr || length == 0) {
        return false;
    }

    bool ret = NullFilter::sendMidiMessage(msg, length);
    if (ret == false) {
        if (next_filter_) {
            ret = next_filter_->sendMidiMessage(msg, length);
        }
    }
    return ret;
}
