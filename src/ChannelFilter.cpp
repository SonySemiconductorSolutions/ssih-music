/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ChannelFilter.h"

//#define DEBUG (1)

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#if defined(DEBUG)
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // if defined(DEBUG)

static const char kClassName[] = "ChannelFilter";

const uint16_t kAllChannelPlay = ~0;

ChannelFilter::ChannelFilter(Filter& filter) : ChannelFilter(kAllChannelPlay, filter) {
}

ChannelFilter::ChannelFilter(uint16_t channel, Filter& filter) : BaseFilter(filter), channel_flags_(channel) {
}
ChannelFilter::~ChannelFilter() {
}
bool ChannelFilter::setParam(int param_id, intptr_t value) {
    if (param_id == ChannelFilter::PARAMID_ENABLE_CHANNEL) {
        if (0 <= value && value < 16) {
            uint16_t ch = 1U << value;
            channel_flags_ |= ch;
        } else {
            error_printf("[%s::%s] error: cannot find channel\n", kClassName, __func__);
            return false;
        }
        debug_printf("[%s::%s] channels_flag:%04x\n", kClassName, __func__, channel_flags_);
        return true;
    } else if (param_id == ChannelFilter::PARAMID_DISABLE_CHANNEL) {
        if (0 <= value && value < 16) {
            uint16_t ch = 1U << value;
            ch = ~ch;
            channel_flags_ &= ch;
        } else {
            error_printf("[%s::%s] error: cannot find channel\n", kClassName, __func__);
            return false;
        }
        debug_printf("[%s::%s] channels_flag:%04x\n", kClassName, __func__, channel_flags_);
        return true;
    } else if (param_id == ChannelFilter::PARAMID_CHANNEL_MASK) {
        channel_flags_ = (uint16_t)value;
        return true;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

intptr_t ChannelFilter::getParam(int param_id) {
    if (param_id == ChannelFilter::PARAMID_ENABLE_CHANNEL) {
        return (intptr_t)channel_flags_;
    } else if (param_id == ChannelFilter::PARAMID_DISABLE_CHANNEL) {
        return (intptr_t)channel_flags_;
    } else if (param_id == ChannelFilter::PARAMID_CHANNEL_MASK) {
        return (intptr_t)channel_flags_;
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool ChannelFilter::isAvailable(int param_id) {
    if (param_id == ChannelFilter::PARAMID_ENABLE_CHANNEL) {
        return true;
    } else if (param_id == ChannelFilter::PARAMID_DISABLE_CHANNEL) {
        return true;
    } else if (param_id == ChannelFilter::PARAMID_CHANNEL_MASK) {
        return true;
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

bool ChannelFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    // debug_printf("[%s::%s] SendNoteOn  note:%d, velocity:%d, channel:%d\n", kClassName, __func__, note, velocity, channel);
    if (channel_flags_ & (1U << channel)) {
        return BaseFilter::sendNoteOn(note, velocity, channel);
    }
}

bool ChannelFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (channel_flags_ & (1U << channel)) {
        return BaseFilter::sendNoteOff(note, velocity, channel);
    }
}

#endif  // ARDUINO_ARCH_SPRESENSE