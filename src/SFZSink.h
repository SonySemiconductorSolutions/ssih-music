/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SFZ_SINK_H_
#define SFZ_SINK_H_

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "YuruInstrumentFilter.h"
#include "SFZParser.h"
#include "PcmRenderer.h"
#include "WavReader.h"
#include "path_util.h"

class SFZSink : public NullFilter, public SFZHandler {
public:
    enum SfzHeader { kHUnSppported, kGlobal, kGroup, kControl, kRegion };

    enum LoopMode { kNoSettingLoopMode, kNoLoop, kOneShot, kLoopContinuous, kLoopSustain };

    enum Opcode {
        kOpcodeSample,
        kOpcodeLokey,
        kOpcodeHikey,
        kOpcodeSwLokey,
        kOpcodeSwHikey,
        kOpcodeSwLast,
        kOpcodeOffset,
        kOpcodeEnd,
        kOpcodeCount,
        kOpcodeLoopMode,
        kOpcodeLoopStart,
        kOpcodeLoopEnd,
        kOpcodeSwDefault,
        kOpcodeMax
    };

    struct Region {
        String sample;
        uint8_t lokey;
        uint8_t hikey;
        uint8_t sw_last;
        uint32_t offset;
        uint32_t end;
        uint32_t count;
        LoopMode loop_mode;
        uint32_t loop_start;
        uint32_t loop_end;
        size_t pcm_offset;
        size_t pcm_size;
        bool silence;
    };

    struct OpcodeContainer {
        bool is_valid;
        uint32_t group_id;
        String sample;
        bool silence;
        unsigned long specified;
        uint32_t opcode[kOpcodeMax];
    };

    friend class SfzTest;

    SFZSink(const String& sfz_filename);

    ~SFZSink();

    bool begin() override;

    void update() override;

    bool isAvailable(int param_id) override;

    intptr_t getParam(int param_id) override;

    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

    void startSfz() override;

    void endSfz() override;

    void header(const String& header) override;

    void opcode(const String& opcode, const String& value) override;

private:
    // for parse
    String sfz_folder_path_;
    std::vector<Region> regions_;
    OpcodeContainer global_;
    OpcodeContainer group_;
    OpcodeContainer region_;
    SfzHeader current_header_;
    uint32_t current_group_;
    int regions_in_group_;
    uint8_t sw_lokey_;
    uint8_t sw_hikey_;
    uint8_t sw_last_;

    // for play
    const Region* playing_region_;
    File file_;
    uint32_t loop_count_;

    int volume_;
    PcmRenderer renderer_;

    void startSound();

    void loadSound(int frame);

    void stopSound();
};

#endif  // SFZ_SINK_H_
