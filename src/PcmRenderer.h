/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file PcmRenderer.h
 */
#ifndef PCM_RENDERER_H_
#define PCM_RENDERER_H_

#include <OutputMixer.h>

/**
 * @brief @~japanese 音声出力のラッパークラスです。簡易的なミキサー機能を備えます。
 * @details @~japanese 音声を出力するには、まず PcmRenderer::begin() で音声出力処理を開始します。
 * 次に PcmRenderer::allocateChannel() で音声出力チャンネルを割り当てて、チャンネル番号を得ます。
 * 出力したい音声データを PcmRenderer::write() で書き込んで、 PcmRenderer::render() を実行すると音声が出力されます。
 * 音声出力が終わったら PcmRenderer::deallocateChannel() でチャンネルを解放します。
 * 複数の音を同時に出したい場合は PcmRenderer::allocateChannel() を必要な回数実行して、
 * それぞれの音声出力チャンネルにそれぞれの音声データを PcmRenderer::write() で書き込みます。
 * PcmRenderer::render() を実行すると、有効な音声出力チャンネルの音声データをミックスした音声が出力されます。
 * @code {.cpp}
 * #include <PcmRenderer.h>
 *
 * const int SAMPLES_PER_FRAME = 240;
 * PcmRenderer renderer = PcmRenderer(48000, 16, 2, SAMPLES_PER_FRAME, SAMPLES_PER_FRAME * 10, 4);
 *
 * void setup() {
 *     renderer.begin();
 *     randomSeed(analogRead(A0));
 * }
 *
 * void make_noise(uint16_t *frame, int frame_size) {
 *     for (int i = 0; i < frame_size; i++) {
 *         frame[i] = (uint16_t)random(65536);
 *     }
 * }
 *
 * void loop() {
 *     // output noise for 10 seconds
 *     int ch = renderer.allocateChannel();
 *     unsigned long start = millis();
 *     while (start + 10000 <= millis()) {
 *         uint16_t frame[SAMPLES_PER_FRAME];
 *         if (renderer.getWritableSize(ch) >= sizeof(frame)) {
 *             make_noise(frame, SAMPLES_PER_FRAME);
 *             renderer.write(ch, frame, sizeof(frame));
 *         }
 *     }
 *     renderer.deallocateChannel(ch);
 * }
 * @endcode
 */
class PcmRenderer {
public:
    /**
     * @brief @~japanese 音声チャンネルの状態。
     * @startuml
     * [*] -> Unallocated
     * Unallocated --> Allocating: allocateChannel()
     * Allocating --> Allocated: render()
     * Allocated --> Allocated: render()
     * Allocated --> Deallocating: deallocateChannel()
     * Deallocating --> Unallocated: render()
     * @enduml
     */
    enum State {
        kStateUnallocated,
        kStateAllocating,
        kStateAllocated,
        kStateDeallocating,
        kStateDeallocated,
        kStateReady = kStateUnallocated,
        kStateActive = kStateAllocated,
        kStatePause = kStateDeallocated
    };
    /**
     * @brief @~japanese 最大音声チャンネル数を指定します。この値が4のとき最大4和音の出力ができます。
     */
    static const int kMaxChannel = 4;

    /**
     * @brief @~japanese PcmRenderer オブジェクトを生成します。
     * @param[in] sample_rate Sampling rate [Hz]
     * @param[in] bit_depth Bit depth [bit]
     * @param[in] channels Audio channel number
     * @param[in] samples_per_frame Audio samples per frame
     * @param[in] cache_capacity Cache capacity
     * @param[in] mix_channels Mixing channel number (PcmRenderer::kMaxChannel or less)
     * @see PcmRenderer::kMaxChannel
     */
    PcmRenderer(int sample_rate, int bit_depth, int channels, int samples_per_frame, size_t cache_capacity, int mix_channels);

    ~PcmRenderer();

    /**
     * @brief @~japanese 音声出力を開始します。
     */
    void begin();

    /**
     * @brief @~japanese 出力音量を設定します。
     *
     * @param[in] master Master volume -1020(-102.0dB) to 120(+12.0dB)
     * @param[in] player0
     * @param[in] player1
     * @see OutputMixer::setVolume()
     */
    void setVolume(int master, int player0, int player1);

    /**
     * @brief @~japanese PcmRenderer::write() で書き込まれた音声データをミックスして出力します。
     * @retval true Success
     * @retval false Fail
     */
    bool render();

#if 0
    void onError(const ErrorAttentionParam *attparam);
#endif
    /**
     * @brief This function is for internal processing. Do not run.
     */
    void onActivated(MsgQueId requester_dtq, MsgType msgtype, AsOutputMixDoneParam *param);

    /**
     * @brief This function is for internal processing. Do not run.
     */
    void onSendData(int32_t identifier, bool is_end);

    size_t getCapacity();

    /**
     * @brief @~japanese 音声出力チャンネルを有効化してユーザーに割り当てます。
     * @retval <0 Fail
     * @retval >=0 allocated channel number
     */
    int allocateChannel();

    /**
     * @brief @~japanese 音声出力チャンネルを解放します。
     * @param[in] ch Channel number
     */
    void deallocateChannel(int ch);

    /**
     * @brief @~japanese 音声出力チャンネルに書き込めるデータサイズを取得します。
     * @param[in] ch Channel number
     * @return Writable size
     */
    size_t getWritableSize(int ch);

    /**
     * @brief @~japanese 音声出力チャンネルから読み出せるデータサイズを取得します。
     * @param[in] ch Channel number
     * @return Readable size
     */
    size_t getReadableSize(int ch);

    /**
     * @brief @~japanese 音声出力チャンネルをクリアします。
     * @param[in] ch Channel number
     */
    void clear(int ch);

    /**
     * @brief @~japanese 音声出力チャンネルに音声データを書き込みます。
     * @param[in] ch Channel number
     * @param[in] src PCM data address
     * @param[in] request_size PCM data size
     * @return Written size
     */
    size_t write(int ch, void *src, size_t request_size);

    /**
     * @brief @~japanese 音声出力チャンネルから音声データを読み出します。
     * @param[in] ch Channel number
     * @param[out] dst PCM data address
     * @param[in] request_size PCM data size
     * @return Read size
     */
    size_t read(int ch, void *dst, size_t request_size);

private:
    // data description
    int sample_rate_;
    int bit_depth_;
    int channels_;
    int samples_per_frame_;

    // flow control
    unsigned long request_count_;
    unsigned long response_count_;

    // cache buffer
    unsigned int frames_;
    size_t capacity_;
    int mix_channels_;
    State states_[kMaxChannel];
    uint8_t *cache_[kMaxChannel];
    size_t wp_[kMaxChannel];
    size_t rp_[kMaxChannel];
};

#endif  // PCM_RENDERER_H_
