/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file SDSink.h
 */
#ifndef SD_SINK_H_
#define SD_SINK_H_

#include <stdint.h>

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "PcmRenderer.h"
#include "YuruInstrumentConfig.h"

/**
 * @brief @~japanese ユーザーが定義した音源テーブルにしたがって、音源を再生する楽器部品です。
 */
class SDSink : public NullFilter {
public:
    enum ParamId {  // MAGIC CHAR = 'S'
        /**
         * @brief @~japanese 音源ファイルの先頭のスキップ量を指定します。
         */
        PARAMID_OFFSET = ('S' << 8),
        /**
         * @brief @~japanese ループ再生の有効・無効を指定します。
         */
        PARAMID_LOOP
    };

    struct Item {
        uint8_t note;
        String path;
    };

    struct PlaybackUnit {
        String path;
        uint32_t offset;
        uint32_t end;
        File file;
        int render_ch;
    };

    /**
     * @brief @~japanese SDSink オブジェクトを生成します。
     * @param[in] table @~japanese 音源テーブル
     * @param[in] table_length @~japanese 音源テーブルの要素数
     */
    SDSink(const Item *table, size_t table_length);

    ~SDSink();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendControlChange(uint8_t ctrl_num, uint8_t value, uint8_t channel) override;

private:
    PlaybackUnit units_[128];
    PcmRenderer renderer_;
    uint32_t offset_;
    bool loop_;
    int volume_;
};

#endif  // SD_SINK_H_
