/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef PCMRENDERER_H_
#define PCMRENDERER_H_

#include <OutputMixer.h>

class PcmRenderer {
public:
    enum State { kStateReady, kStateActive, kStatePause };
    static const int kMaxChannelNum = 4;

    PcmRenderer(int sample_rate, int bit_depth, int channels, int samples_per_frame, size_t cache_capacity, int mix_channels);
    ~PcmRenderer();
    State getState();
    void setState(State state);
    void begin();
    void setVolume(int master, int player0, int player1);
    bool render();

    // event handlers
#if 0
    void onError(const ErrorAttentionParam *attparam);
#endif
    void onActivated(MsgQueId requester_dtq, MsgType msgtype, AsOutputMixDoneParam *param);
    void onSendData(int32_t identifier, bool is_end);

    // cache buffer
    size_t getCapacity();
    size_t getWritableSize(int ch);
    size_t getReadableSize(int ch);
    void clear(int ch);
    size_t write(int ch, void *src, size_t request_size);
    size_t read(int ch, void *dst, size_t request_size);

private:
    State state_;

    // data description
    int sample_rate_;
    int bit_depth_;
    int channels_;
    int samples_per_frame_;

    // cache buffer
    unsigned int frames_;
    size_t capacity_;
    int mix_channels_;
    uint8_t *cache_[kMaxChannelNum];
    size_t wp_[kMaxChannelNum];
    size_t rp_[kMaxChannelNum];
};

#endif  // PCMRENDERER_H_
