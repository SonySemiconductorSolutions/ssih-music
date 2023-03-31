/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file OctaveShift.h
 */
#ifndef OCTAVE_SHIFT_H_
#define OCTAVE_SHIFT_H_

#include "YuruInstrumentFilter.h"

/**
 * @brief @~japanese 演奏の音程をオクターブ単位でシフトさせる(上下させる)楽器部品です。
 * @details @~japanese YuruhornSrc を入力元とした楽器で、高い声の人と低い声の人で同じ曲を合奏するときに使用します。
 * このケースでは低い声の人の演奏を1オクターブ高くすると同じ音程で演奏できるようになります。
 */
class OctaveShift : public BaseFilter {
public:
    enum ParamId {  // MAGIC CHAR = 'O'
        /**
         * @brief [get,set] @~japanese 音程のシフト量です。1を指定すると1オクターブ高い音、-1を指定すると1オクターブ低い音になります。
         */
        PARAMID_OCTAVE_SHIFT = ('O' << 8)
    };

    /**
     * @brief @~japanese OctaveShift オブジェクトを生成します。
     * @details @~japanese 初期状態のシフト量は0(シフトしない)です。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    OctaveShift(Filter& filter);

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

    /**
     * @brief @~japanese 音程のシフト量を取得します。
     * @details @~japanese 第1引数に OctaveShift::PARAMID_OCTAVE_SHIFT を指定した Filter::getParam と同じです。
     * @return @~japanese 音程のシフト量
     */
    int getShift();

    /**
     * @brief @~japanese 音程のシフト量を更新します。
     * @details @~japanese 第1引数に OctaveShift::PARAMID_OCTAVE_SHIFT を指定した Filter::setParam と同じです。
     * @param[in] shift シフト量
     */
    void setShift(int shift);

private:
    int shift_;
};

#endif  // OCTAVE_SHIFT_H_
