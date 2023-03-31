/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file VoiceCapture.h
 */
#ifndef VOICE_CAPTURE_H_
#define VOICE_CAPTURE_H_

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include <pthread.h>

#include <FrontEnd.h>
#include <File.h>

#include "YuruInstrumentFilter.h"

#endif  // ARDUINO_ARCH_SPRESENSE

#define SUB_CORE_ID (1)

#define MSGID_INIT (110)
#define MSGID_INIT_DONE (113)
#define MSGID_SEND_CAPTURE (114)
#define MSGID_SEND_RESULT (115)

// MIC input format
#define MIC_SAMPLING_FREQ (48000)
#define MIC_BYTE_WIDTH (2)
#define MIC_CHANNELS (1)
#define MIC_FRAME_LENGTH (256 * 3)
#define MIC_BLOCK_SIZE (MIC_FRAME_LENGTH * MIC_BYTE_WIDTH * MIC_CHANNELS)

// FrontEnd output format
#if !defined(CAP_SAMPLING_FREQ)
#define CAP_SAMPLING_FREQ (48000)
#endif
#define CAP_BYTE_WIDTH (2)
#define CAP_CHANNELS (1)
#define CAP_FRAME_LENGTH (MIC_FRAME_LENGTH * CAP_SAMPLING_FREQ / MIC_SAMPLING_FREQ)
#define CAP_BLOCK_SIZE (CAP_FRAME_LENGTH * CAP_BYTE_WIDTH * CAP_CHANNELS)

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)
/**
 * @brief @~japanese 音声入力を扱う楽器部品です。
 * @details @~japanese マイクから入力された音声信号の周波数と音量を解析します。
 * VoiceCaptureを継承したクラスは VoiceCapture::onCapture() 関数をオーバーライドすることで、簡単に音声パラメータを扱うことができます。
 * VoiceCaptureを使用するためには、SpresenseのSubCore1にYuruHorn_SubCore1.ino(音声を解析するスケッチ)をあらかじめ書き込んでおく必要があります。
 * @code {.cpp}
 * #include <VoiceCapture.h>
 *
 * class Buzzer : public VoiceCapture {
 * public:
 *     Buzzer(Filter& filter): VoiceCapture(filter), prev_vol_(0) {
 *     }
 *
 *     void onCapture(unsigned int freq_number, unsigned int freq_denom, unsigned int volume) override {
 *         if (prev_vol_ < 400 && volume >= 400) {
 *             // start voice => start sound
 *             VoiceCapture::sendNoteOn(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
 *         }
 *         if (prev_vol_ >= 400 && volume < 400) {
 *             // stop voice => stop sound
 *             VoiceCapture::sendNoteOff(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
 *         }
 *         prev_vol_ = volume;
 *     }
 *
 * private:
 *     unsigned int prev_vol_;
 * };
 *
 * NullFilter sink;
 * Buzzer inst(sink);
 *
 * void setup() {
 *     inst.begin();
 * }
 *
 * void loop() {
 *     inst.update();
 * }
 * @endcode
 * @see YuruHorn_SubCore1.ino
 */
class VoiceCapture : public BaseFilter {
#else
class VoiceCapture {
#endif  // ARDUINO_ARCH_SPRESENSE
public:
    /**
     * @brief @~japanese キャプチャした音声データを格納する構造体です。MainCoreから送信し、SubCoreで受信します。
     */
    struct Capture {
        unsigned int id;
        void* data;
        size_t size;
        int sample_size;
        unsigned int channel;

        unsigned int fs;

        unsigned long capture_time;
        unsigned char reserved;
    };

    /**
     * @brief @~japanese 音声データの解析結果を格納する構造体です。
     */
    struct Result {
        unsigned int id;
        unsigned int freq_numer;
        unsigned int freq_denom;
        unsigned int volume;
        unsigned long capture_time;
        unsigned long result_time;
    };

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)
    enum ParamId {  // MAGIC CHAR = 'V'
        /**
         * @brief [get,set] @~japanese マイクゲインです。値範囲は0(0.0dB)から210(+21.0dB)です。
         */
        PARAMID_MIC_GAIN = ('V' << 8),
        /**
         * @brief [get,set] @~japanese マイクの入力音声にかける係数(百分率)です。
         */
        PARAMID_INPUT_LEVEL,
        /**
         * @brief [get,set] @~japanese 録音時間(秒数)です。
         */
        PARAMID_RECORDING_TIME,
        /**
         * @brief [get,set] @~japanese  録音状態です。
         */
        PARAMID_RECORDING
    };

    class Recorder {
    public:
        enum State { RECORDER_STATE_INIT, RECORDER_STATE_RECORDING, RECORDER_STATE_FULL };
        Recorder();
        ~Recorder();

        bool start();
        bool end();
        void writeFrame(void* buffer);
        void flush();

        bool isRecording();
        int getRecordingTime();
        bool setRecordingTime(int value);

    private:
        int state_;
        File file_;
        uint8_t* buffer_;
        int frame_index_;
        int recording_time_;
        int seconds_;
    };

    /**
     * @brief @~japanese VoiceCapture オブジェクトを生成します。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    VoiceCapture(Filter& filter);

    ~VoiceCapture();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

#if 0
    bool onMicFrontend(AsMicFrontendEvent evtype, uint32_t result, uint32_t sub_result);
#endif
    /**
     * @brief This function is for internal processing. Do not run.
     * @param[in] param
     */
    void onFrontendDone(AsPcmDataParam param);

protected:
    /**
     * @brief @~japanese SubCoreによる解析結果の通知を受ける関数です。
     * VoiceCaptureを継承したクラスは VoiceCapture::onCapture() 関数をオーバーライドしてください。
     * @param[in] freq_numer @~japanese 周波数の分子
     * @param[in] freq_denom @~japanese 周波数の分母
     * @param[in] volume @~japanese 音量
     */
    virtual void onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) = 0;

private:
    int gain_;
    int input_level_;
    Result* result_;

    size_t capture_frames_;
    size_t send_frames_;
    size_t receive_frames_;

    Recorder recorder_;
#endif  // ARDUINO_ARCH_SPRESENSE
};

#endif  // VOICE_CAPTURE_H_
