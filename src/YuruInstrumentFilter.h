/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file YuruInstrumentFilter.h
 */
#ifndef YURU_INSTRUMENT_FILTER_H_
#define YURU_INSTRUMENT_FILTER_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @brief @~japanese 無効なノートナンバーを表します。
 */
static const uint8_t INVALID_NOTE_NUMBER = 0xFF;

/**
 * @brief @~japanese 最小のノート番号です。値は0です。
 */
static const uint8_t NOTE_NUMBER_MIN = 0x00;

/**
 * @brief @~japanese 最大のノート番号です。値は127です。
 */
static const uint8_t NOTE_NUMBER_MAX = 0x7F;

/**
 * @brief @~japanese ライブラリで扱う標準的なベロシティ値です。
 */
static const uint8_t DEFAULT_VELOCITY = 64;

/**
 * @brief @~japanese ライブラリで扱う標準的なチャンネル番号です。
 */
static const uint8_t DEFAULT_CHANNEL = 1;

/**
 * @brief @~japanese 1オクターブ当たりの音の数を表します。
 */
static const int PITCH_NUM = 12;

/**
 * @brief @~japanese 楽器を構成する部品のインターフェースです。
 * @details @~japanese Arduinoスケッチから呼び出す関数、設定値を設定する関数、MIDIメッセージを扱う関数を定義します。
 * Arduinoスケッチのsetup関数からは Filter::begin() 関数、loop関数からは Filter::update() 関数を呼び出すことで、楽器を演奏します。
 * @code {.cpp}
 * #include <YuruInstrumentFilter.h>
 *
 * class MyFilter: public Filter {
 *    // implement MyFilter
 * };
 *
 * MyFilter inst();
 *
 * void setup() {
 *     inst.begin();
 * }
 *
 * void loop() {
 *     inst.update();
 * }
 * @endcode
 */
class Filter {
public:
    enum ParamId {
        /**
         * @brief [get, set] @~japanese 音声出力レベルを設定します。
         */
        PARAMID_OUTPUT_LEVEL
    };

    /**
     * @brief @~japanese 楽器の構成部品を初期化して使える状態にします。
     *
     * @retval true Success
     * @retval false Fail
     */
    virtual bool begin() = 0;
    /**
     * @brief @~japanese 楽器の定常処理を実行します。
     */
    virtual void update() = 0;

    /**
     * @brief @~japanese 楽器の設定値を取得または変更できるかどうかを取得します。
     *
     * @param[in] param_id @~japanese 設定値を示すIDです。
     * @retval true @~japanese 設定・変更できる
     * @retval false  @~japanese 設定・取得できない
     */
    virtual bool isAvailable(int param_id) = 0;
    /**
     * @brief @~japanese 楽器の設定値を取得します。
     *
     * @param[in] param_id @~japanese 取得する設定値のIDを指定します。
     * @return @~japanese 設定値を返します。
     */
    virtual intptr_t getParam(int param_id) = 0;
    /**
     * @brief @~japanese 楽器の設定値を変更します。
     *
     * @param[in] param_id @~japanese 設定する設定値のIDを指定します。
     * @param[in] value @~japanese 新しい設定値を指定します。
     * @retval true Success
     * @retval false Fail
     */
    virtual bool setParam(int param_id, intptr_t value) = 0;

    /**
     * @brief Send "Note Off" event
     * @param[in] note note number (0 to 127)
     * @param[in] velocity velocity (0 to 127)
     * @param[in] channel channel number (1 to 16)
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) = 0;

    /**
     * @brief Send "Note On" event
     * @param[in] note note number (0 to 127)
     * @param[in] velocity velocity (0 to 127)
     * @param[in] channel channel number (1 to 16)
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) = 0;

    /**
     * @brief Send "Control Change" event
     * @param[in] ctrl_num Control number (0 to 127)
     * @param[in] value value
     * @param[in] channel channel number (1 to 16)
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendControlChange(uint8_t ctrl_num, uint8_t value, uint8_t channel) = 0;

    /**
     * @brief Send "Program Change" event
     * @param[in] prog_num Program number (0 to 127)
     * @param[in] channel channel number (1 to 16)
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendProgramChange(uint8_t prog_num, uint8_t channel) = 0;

    /**
     * @brief Send "Song Position Pointer"
     * @param[in] beats MIDI beats (1 MIDI beat = 6 MIDI clocks)
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendSongPositionPointer(uint16_t beats) = 0;

    /**
     * @brief Send "Song Select"
     * @param[in] song sequence or song is to be played
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendSongSelect(uint8_t song) = 0;

    /**
     * @brief Send "Start"
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendStart() = 0;

    /**
     * @brief Send "Continue"
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendContinue() = 0;

    /**
     * @brief Send "Stop"
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendStop() = 0;

    /**
     * @brief Send raw MIDI message
     *
     * @param[in] msg MIDI message
     * @param[in] length Length of message
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendMidiMessage(uint8_t* msg, size_t length) = 0;
};

/**
 * @brief @~japanese Filter の標準的な実装クラスです。
 *
 * @details @~japanese Filter インターフェースをすべて空関数で実装したクラスです。
 * 終端の部品を開発するときは NullFilter 、そうでない場合は BaseFilter を基底クラスとして実装してください。
 * @see BaseFilter
 */
class NullFilter : public Filter {
public:
    /**
     * @brief @~japanese NullFilter オブジェクトを生成します。
     */
    NullFilter();
    ~NullFilter();

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
     * @brief send Song Position Pointer
     *
     * @details @~japanese MIDIビートを用いてソングポジションポインターを設定する
     *
     * @param[in] beats MIDI beats
     * @retval true success
     * @retval false failed
     */
    bool sendSongPositionPointer(uint16_t beats) override;
    bool sendSongSelect(uint8_t song) override;
    bool sendStart() override;
    bool sendContinue() override;
    bool sendStop() override;

    bool sendMidiMessage(uint8_t* msg, size_t length) override;
};

/**
 * @brief @~japanese Filter の標準的な実装クラスです。他の Filter オブジェクトを後ろにつなげることで、機能を拡張することができます。
 *
 * @details @~japanese Filter インターフェースを実装したクラスです。
 * BaseFilter オブジェクトの関数を呼び出すと、後続の Filter オブジェクトの同関数が連鎖的に呼び出されます。
 * 終端の部品を開発するときは NullFilter 、そうでない場合は BaseFilter を基底クラスとして実装してください。
 * @code {.cpp}
 * #include <YuruInstrumentFilter.h>
 *
 * class FirstFilter: public BaseFilter {
 * public:
 *     FirstFilter(Filter& filter): BaseFilter(filter) {
 *     }
 *
 *     void update() override {
 *         if (something_happend) {
 *             BaseFilter::sendNoteOn(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
 *         }
 *         return BaseFilter::update();
 *     }
 * };
 *
 * class MiddleFilter: public BaseFilter {
 * public:
 *     MiddleFilter(Filter& filter): BaseFilter(filter) {
 *     }
 *
 *     bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override {
 *         // make higher sound
 *         return BaseFilter::sendNoteOff(note + 1, velocity, channel);
 *     }
 *
 *     bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override {
 *         // make higher sound
 *         return BaseFilter::sendNoteOn(note + 1, velocity, channel);
 *     }
 * };
 *
 * class LastFilter: public NullFilter {
 * public:
 *     LastFilter(): NullFilter() {
 *     }
 *
 *     bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override {
 *         // do something
 *         return NullFilter::sendNoteOn(note, velocity, channel);
 *     }
 * };
 *
 * LastFilter last;
 * MiddleFilter middle(last);
 * FirstFilter inst(middle);
 *
 * void setup() {
 *     inst.begin();
 * }
 *
 * void loop() {
 *     inst.update();
 * }
 * @endcode
 * @see NullFilter
 */
class BaseFilter : public NullFilter {
protected:
    Filter* next_filter_;

public:
    /**
     * @brief @~japanese BaseFilter オブジェクトを生成します。
     *
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    BaseFilter(Filter& filter);
    ~BaseFilter();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendControlChange(uint8_t ctrl_num, uint8_t value, uint8_t channel) override;
    bool sendProgramChange(uint8_t prog_num, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendSongPositionPointer(uint16_t beats) override;
    bool sendSongSelect(uint8_t song) override;
    bool sendStart() override;
    bool sendContinue() override;
    bool sendStop() override;

    bool sendMidiMessage(uint8_t* msg, size_t length) override;
};

#endif  // YURU_INSTRUMENT_FILTER_H_
