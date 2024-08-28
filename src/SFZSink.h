/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file SFZSink.h
 */
#ifndef SFZ_SINK_H_
#define SFZ_SINK_H_

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "SFZParser.h"
#include "PcmRenderer.h"
#include "YuruInstrumentConfig.h"

/**
 * @brief @~japanese 音源定義ファイル(SFZファイル)にしたがって、音源を再生する楽器部品です。
 */
class SFZSink : public NullFilter, public SFZHandler {
public:
    enum ParamId {  // MAGIC CHAR = 'Z'
        PARAMID_SW_LAST = ('Z' << 8),
        PARAMID_SW_LOKEY,
        PARAMID_SW_HIKEY
    };

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
        kOpcodeLoProg,
        kOpcodeHiProg,
        kOpcodeLoCC0,
        kOpcodeHiCC0,
        kOpcodeLoCC32,
        kOpcodeHiCC32,
        kOpcodeMax
    };
    enum LoopMode { kInvalidLoopMode, kNoLoop, kOneShot, kLoopContinuous, kLoopSustain };

    /**
     * @brief @~japanese SFZファイルから読み出したregionデータを格納する構造体です。
     */
    struct Region {
        String sample;
        uint8_t lochan;
        uint8_t hichan;
        uint8_t lokey;
        uint8_t hikey;
        uint8_t lovel;
        uint8_t hivel;
        uint8_t loprog;
        uint8_t hiprog;
        uint8_t locc0;
        uint8_t hicc0;
        uint8_t locc32;
        uint8_t hicc32;
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

    /**
     * @brief @~japanese パース中のデータを格納する中間形式の構造体です。パース処理が完了すると SFZSink::Region に変換されます。
     */
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

    struct CCParamStore {
        uint8_t msb;
        uint8_t lsb;
    };

    /**
     * @brief @~japanese SFZSink オブジェクトを生成します。
     * @param[in] sfz_path @~japanese SFZファイルパス
     */
    SFZSink(const String& sfz_path);

    ~SFZSink();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendControlChange(uint8_t ctrl_num, uint8_t value, uint8_t channel) override;
    bool sendProgramChange(uint8_t prog_num, uint8_t channel) override;

    /**
     * @brief @~japanese region数を取得します。
     * @return number of regions
     */
    size_t getNumberOfRegions();

    /**
     * @brief @~japanese regionを取得します。
     * @param[in] id region index
     * @return region object
     */
    const Region* getRegion(size_t id);

    /**
     * @brief @~japanese sw_lokey値を取得します。
     * @return sw_lokey value
     */
    uint8_t getSwLoKey();

    /**
     * @brief @~japanese sw_hikey値を取得します。
     * @return sw_hikey value
     */
    uint8_t getSwHiKey();

    /**
     * @brief @~japanese sw_last値を取得します。
     * @return sw_last value
     */
    uint8_t getSwLast();

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
    CCParamStore bank_;
    int volume_;

    uint8_t prog_num_;

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
