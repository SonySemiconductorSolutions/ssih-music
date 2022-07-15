/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SDSINK_H_
#define SDSINK_H_

#include <stdint.h>

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "PcmRenderer.h"
#include "WavReader.h"
#include "YuruInstrumentFilter.h"

class SDSink : public NullFilter {
public:
    enum ParamId {                    // MAGIC CHAR = 'S'
        PARAMID_OFFSET = ('S' << 8),  //<
        PARAMID_LOOP
    };
    struct Item {
        uint8_t note;
        String path;
    };

    struct PcmData {
        PcmData();
        uint8_t note;
        String path;
        uint32_t size;
        uint32_t offset;
    };

    enum PcmFileType {
        kPcmFileTypeRaw = 0,
        kPcmFileTypeWave,
        kPcmFileTypeOthers,
    };

    // Constructor
    SDSink(const Item *table, size_t table_length);
    ~SDSink();

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool begin() override;
    void update() override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

private:
    File file_;
    uint32_t offset_;
    int volume_;
    PcmRenderer renderer_;

    std::vector<PcmData> pcm_table_;
    int table_index_;
    size_t remain_pcm_size_;
    bool loop_;
};

#endif  // SDSINK_H_
