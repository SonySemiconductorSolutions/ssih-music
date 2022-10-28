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

#include "SFZParser.h"
#include "PcmRenderer.h"
#include "YuruInstrumentFilter.h"

class SFZSink : public NullFilter, public SFZHandler {
public:
    enum Header { kInvalidHeader, kGlobal, kGroup, kControl, kRegion };
    enum Opcode {
        kOpcodeSample,
        kOpcodeLochan,
        kOpcodeHichan,
        kOpcodeLokey,
        kOpcodeHikey,
        kOpcodeLovel,
        kOpcodeHivel,
        kOpcodeLorand,
        kOpcodeHirand,
        kOpcodeSeqLength,
        kOpcodeSeqPosition,
        kOpcodeSwLokey,
        kOpcodeSwHikey,
        kOpcodeSwLast,
        kOpcodeGroup,
        kOpcodeOffBy,
        kOpcodeOffset,
        kOpcodeEnd,
        kOpcodeCount,
        kOpcodeLoopMode,
        kOpcodeLoopStart,
        kOpcodeLoopEnd,
        kOpcodeDefaultPath,
        kOpcodeSwDefault,
        kOpcodeMax
    };
    enum LoopMode { kInvalidLoopMode, kNoLoop, kOneShot, kLoopContinuous, kLoopSustain };

    struct Region {
        String sample;
        uint8_t lochan;
        uint8_t hichan;
        uint8_t lokey;
        uint8_t hikey;
        uint8_t lovel;
        uint8_t hivel;
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

    struct PlaybackUnit {
        uint8_t note;
        uint8_t channel;
        int render_ch;
        Region* region;
        File file;
        uint32_t loop;
    };

    friend class SfzTest;

    SFZSink(const String& sfz_path);
    ~SFZSink();

    // Filter, NullFilter
    bool begin() override;
    void update() override;
    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

    // SFZHandler
    void startSfz() override;
    void endSfz() override;
    void startHeader(const String& header) override;
    void endHeader(const String& header) override;
    void opcode(const String& opcode, const String& value) override;

private:
    // for play
    String sfz_path_;
    std::vector<Region> regions_;
    std::vector<PlaybackUnit> playback_units_;
    PcmRenderer renderer_;
    int volume_;

    // for parse
    OpcodeContainer global_;
    OpcodeContainer group_;
    OpcodeContainer region_;
    Header header_;
    uint32_t group_id_;
    int regions_in_group_;
    String default_path_;
    uint8_t sw_lokey_;
    uint8_t sw_hikey_;
    uint8_t sw_last_;

    PlaybackUnit* startPlayback(uint8_t note, uint8_t velocity, uint8_t channel, Region* region);
    void continuePlayback(PlaybackUnit* unit, int frames);
    void stopPlayback(PlaybackUnit* unit);
};

#endif  // SFZ_SINK_H_
