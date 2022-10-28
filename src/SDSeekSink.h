/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SD_SEEK_SINK_H_
#define SD_SEEK_SINK_H_

#include <stdint.h>

#include <Arduino.h>

#include "YuruInstrumentFilter.h"
#include "PcmRenderer.h"
#include "TarReader.h"

class SDSeekSink : public NullFilter {
public:
    SDSeekSink(const String &file_path);
    ~SDSeekSink();

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool begin() override;
    void update() override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

private:
    std::vector<uint8_t> table_;
    TarReader reader_;
    TarReader::Stream stream_;
    int volume_ = 0;
    PcmRenderer renderer_;
};

#endif  // SD_SEEK_SINK_H_
