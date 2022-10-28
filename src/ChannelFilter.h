/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef CHANNEL_FILTER_H_
#define CHANNEL_FILTER_H_

#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

class ChannelFilter : public BaseFilter {
public:
    enum ParamId {                            // MAGIC CHAR = 'C'
        PARAMID_ENABLE_CHANNEL = ('C' << 8),  //<
        PARAMID_DISABLE_CHANNEL,
        PARAMID_CHANNEL_MASK
    };

    ChannelFilter(Filter& filter);
    ChannelFilter(uint16_t channel, Filter& filter);
    ~ChannelFilter();

    bool setParam(int param_id, intptr_t value) override;
    intptr_t getParam(int param_id) override;
    bool isAvailable(int param_id) override;

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;

private:
    uint16_t channel_flags_;
};

#endif  // CHANNEL_FILTER_H_