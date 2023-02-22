/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef PCM_RENDERER_H_
#define PCM_RENDERER_H_

#include <OutputMixer.h>

class PcmRenderer {
public:
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
    static const int kMaxChannel = 4;

    PcmRenderer(int sample_rate, int bit_depth, int channels, int samples_per_frame, size_t cache_capacity, int mix_channels);
    ~PcmRenderer();
    State getState();            //< deprecated
    void setState(State state);  //< deprecated
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
    int allocateChannel();
    void deallocateChannel(int ch);
    size_t getWritableSize(int ch);
    size_t getReadableSize(int ch);
    void clear(int ch);  //<deprecated
    size_t write(int ch, void *src, size_t request_size);
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
