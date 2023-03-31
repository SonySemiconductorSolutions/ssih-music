/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file YuruhornSrc.h
 */
#ifndef YURUHORN_SRC_H_
#define YURUHORN_SRC_H_

#include <vector>

#include "YuruInstrumentFilter.h"
#include "VoiceCapture.h"

/**
 * @brief @~japanese マイクに入力された音と同じ高さのノート・オンを送信する楽器部品です。
 * @see VoiceCapture
 */
class YuruhornSrc : public VoiceCapture {
public:
    enum ParamId {  // MAGIC CHAR = 'Y'
        /**
         * @brief [get,set] @~japanese 音声入力が有効であると判定する音量のしきい値です。
         */
        PARAMID_ACTIVE_LEVEL = ('Y' << 8),  //<
        /**
         * @brief [set] @~japanese 演奏ボタンを有効フラグです。
         */
        PARAMID_PLAY_BUTTON_ENABLE,
        /**
         * @brief [get,set] @~japanese 再生する調です。
         * YuruhornSrc::SCALE_CHROMATIC を設定すると12音、
         * YuruhornSrc::SCALE_C_MAJOR を設定すると7音(Cメジャースケール)が演奏できます。
         */
        PARAMID_SCALE,
        /**
         * @brief [get,set] @~japanese 再生するノート番号の下限値です。
         */
        PARAMID_MIN_NOTE,
        /**
         * @brief [get,set] @~japanese 再生するノート番号の上限値です。
         */
        PARAMID_MAX_NOTE,
        /**
         * @brief [get,set] @~japanese 周波数値が得られなかった時に、演奏中の音を継続するフレーム数です。
         */
        PARAMID_CORRECT_FRAMES,
        /**
         * @brief [get,set] @~japanese ノイズの誤検出を防ぐために、音の出始めでも音を出さないフレーム数です。
         */
        PARAMID_SUPPRESS_FRAMES,
        /**
         * @brief [get,set] @~japanese 音が途切れてから音を出し続けるフレーム数です。
         */
        PARAMID_KEEP_FRAMES,
        /**
         * @brief [get] @~japanese 入力音量です。
         */
        PARAMID_VOLUME_METER,
        /**
         * @brief [get,set] @~japanese デバッグ用のモニタリング機能です。有効化するとシリアル・プロッタで波形が確認できます。
         */
        PARAMID_MONITOR_ENABLE
    };

    enum Scale {
        // clang-format off
        //                  B A G FE D C
        SCALE_CHROMATIC = 0b111111111111,   //< chromatic scale (4095)
        SCALE_C_MAJOR   = 0b101010110101,   //< C-major scale (2741)
        SCALE_Db_MAJOR  = 0b010101101011,   //< Db-major scale (1387)
        SCALE_D_MAJOR   = 0b101011010110,   //< D-major scale (2774)
        SCALE_Eb_MAJOR  = 0b010110101101,   //< Eb-major scale (1453)
        SCALE_E_MAJOR   = 0b101101011010,   //< E-major scale (2906)
        SCALE_F_MAJOR   = 0b011010110101,   //< F-major scale (1717)
        SCALE_Gb_MAJOR  = 0b110101101010,   //< Gb-major scale (3434)
        SCALE_G_MAJOR   = 0b101011010101,   //< G-major scale (2773)
        SCALE_Ab_MAJOR  = 0b010110101011,   //< Ab-major scale (1451)
        SCALE_A_MAJOR   = 0b101101010110,   //< A-major scale (2902)
        SCALE_Bb_MAJOR  = 0b011010101101,   //< Bb-major scale (1709)
        SCALE_B_MAJOR   = 0b110101011010,   //< B-major scale (3418)
        SCALE_ALL       = SCALE_CHROMATIC
        // clang-format on
    };

    /**
     * @brief @~japanese YuruhornSrc オブジェクトを生成します。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    YuruhornSrc(Filter& filter);
    YuruhornSrc(uint8_t perform_pin, uint8_t volume_pin, Filter& filter);

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    /**
     * @brief @~japanese 音声入力が有効であると判定する音量のしきい値を取得します。
     * @return Threshold
     */
    int getActiveLevel();

    /**
     * @brief @~japanese 音声乳ryクが有効であると判定する音量のしきい値を設定します。
     * @param[in] new_active_thres Threshold
     */
    void setActiveLevel(int active_level);

protected:
    void onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) override;

private:
    int active_level_;
    bool is_button_enable_;
    uint8_t perform_note_;
    int extend_frames_;
    int suppress_frames_;
    int keep_frames_;
    int invalid_counter_;
    int active_counter_;
    int silent_counter_;
    uint16_t scale_mask_;
    uint8_t max_note_;
    uint8_t min_note_;
    int level_meter_;
    bool is_monitor_enable_;
    int monitor_volume_;
    uint8_t perform_pin_;
    uint8_t volume_pin_;

    uint8_t lookupNote(unsigned int freq);
    uint8_t lingerNote(uint8_t note, unsigned int volume);
};

#endif  // YURUHORN_SRC_H_
