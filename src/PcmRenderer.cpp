/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "PcmRenderer.h"

#include <stdlib.h>
#include <pthread.h>

#include <Arduino.h>

#include <Audio.h>
#include <OutputMixer.h>
#include <MemoryUtil.h>
#include <arch/board/cxd56_audio.h>

#define ARM_MATH_CM4
#define __FPU_PRESENT 1U
#include <cmsis/arm_math.h>

// #define DEBUG (1)

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#if defined(DEBUG)
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // if defined(DEBUG)

static const char kClassName[] = "PcmRenderer";

static const int kPreLoadFrames = 3;  //< Mixer starts when data is sent 3 frames or more
static const uint8_t kUnderflowThreshold = 2;

/**
 * @brief singleton of OutputMixer
 *
 */
static OutputMixer *g_mixer = OutputMixer::getInstance();

/**
 * @brief active PcmRenderer
 *
 */
static PcmRenderer *g_renderer = nullptr;

#if 0
/**
 * @brief callback function for OutputMixer error
 *
 * @param attparam
 */
static void outputMixerErrorCallback(const ErrorAttentionParam *attparam) {
    trace_printf("[%s::%s] ()\n", kClassName, __func__);
    if (g_renderer) {
        g_renderer->onError(attparam);
    }
}
#endif

/**
 * @brief callback function of OutputMixer::activate
 *
 * @param requester_dtq
 * @param msgtype
 * @param param
 */
static void outputMixerCallback(MsgQueId requester_dtq, MsgType msgtype, AsOutputMixDoneParam *param) {
    trace_printf("[%s::%s] ()\n", kClassName, __func__);
    if (g_renderer) {
        g_renderer->onActivated(requester_dtq, msgtype, param);
    }
}

/**
 * @brief callback function of OutputMixer::sendData
 *
 * @param identifier
 * @param is_end
 */
static void pcmProcDoneCallback(int32_t identifier, bool is_end) {
    // trace_printf("[%s::%s] ()\n", kClassName, __func__);
    if (g_renderer) {
        g_renderer->onSendData(identifier, is_end);
    }
}

PcmRenderer::PcmRenderer(int sample_rate, int bit_depth, int channels, int samples_per_frame, size_t cache_capacity, int mix_channels)
    : sample_rate_(sample_rate),
      bit_depth_(bit_depth),
      channels_(channels),
      samples_per_frame_(samples_per_frame),
      request_count_(0),
      response_count_(0),
      frames_(0),
      capacity_(cache_capacity),
      mix_channels_((mix_channels < PcmRenderer::kMaxChannel) ? mix_channels : PcmRenderer::kMaxChannel),
      states_(),
      cache_(),
      wp_(),
      rp_() {
    trace_printf("[%s::%s] (%d, %d, %d, %d, %d, %d)\n", kClassName, __func__, sample_rate, bit_depth, channels, samples_per_frame, (int)cache_capacity,
                 mix_channels);
    for (int i = 0; i < mix_channels_; i++) {
        states_[i] = kStateUnallocated;
        cache_[i] = new uint8_t[capacity_];
        wp_[i] = 0;
        rp_[i] = 0;
    }
}

PcmRenderer::~PcmRenderer() {
    trace_printf("[%s::%s] ()\n", kClassName, __func__);
    if (g_renderer == this) {
        g_renderer = nullptr;
    }
    for (int i = 0; i < mix_channels_; i++) {
        delete[] cache_[i];
        cache_[i] = nullptr;
    }
}

PcmRenderer::State PcmRenderer::getState() {
    debug_printf("[%s::%s] deprecated\n", kClassName, __func__);
    return states_[0];
}

void PcmRenderer::setState(PcmRenderer::State state) {
    debug_printf("[%s::%s] deprecated\n", kClassName, __func__);
    states_[0] = state;
}

void PcmRenderer::begin() {
    trace_printf("[%s::%s] ()\n", kClassName, __func__);
    err_t err = AUDIOLIB_ECODE_OK;
    bool ok = true;

    g_renderer = this;
    err = g_mixer->activateBaseband();
    if (err != OUTPUTMIXER_ECODE_OK) {
        error_printf("[%s::%s] error: failed OutputMixer::activateBaseband => %d\n", kClassName, __func__, err);
    }
    err = g_mixer->create();
    if (err != AUDIOLIB_ECODE_OK) {
        error_printf("[%s::%s] error: failed OutputMixer::create => %d\n", kClassName, __func__, err);
    }
    ok = g_mixer->setRenderingClkMode(OUTPUTMIXER_RNDCLK_NORMAL);
    if (!ok) {
        error_printf("[%s::%s] error: failed OutputMixer::setRenderingClkMode => %d\n", kClassName, __func__, ok);
    }
    err = g_mixer->activate(OutputMixer0, HPOutputDevice, outputMixerCallback);
    if (err != AUDIOLIB_ECODE_OK) {
        error_printf("[%s::%s] error: failed OutputMixer::activate => %d\n", kClassName, __func__, err);
    }
    err = g_mixer->setVolume(0, 0, 0);
    board_external_amp_mute_control(false);

    // send dummy frames
    const int bytes_per_sample = (bit_depth_ / 8) * channels_;
    const size_t frame_size = bytes_per_sample * samples_per_frame_;
    for (int i = 0; i < kPreLoadFrames; i++) {
        AsPcmDataParam pcm;
        err_t err = ERR_OK;
        err = pcm.mh.allocSeg(S0_REND_PCM_BUF_POOL, frame_size);
        if (err != ERR_OK) {
            error_printf("[%s::%s] error: failed MemoryHandler::allocSeg => %d\n", kClassName, __func__, err);
            continue;
        }

        pcm.identifier = 0;
        pcm.callback = nullptr;
        pcm.sample = frame_size / bytes_per_sample;
        pcm.size = frame_size;
        pcm.is_end = (pcm.size < frame_size);
        pcm.is_valid = (pcm.size > 0);
        pcm.bit_length = bit_depth_;

        uint8_t *raw = reinterpret_cast<uint8_t *>(pcm.mh.getPa());
        memset(raw, 0x00, frame_size);
        err = g_mixer->sendData(OutputMixer0, pcmProcDoneCallback, pcm);
        if (err != OUTPUTMIXER_ECODE_OK) {
            error_printf("[%s::%s] error: failed OutputMixer::sendData => %d\n", kClassName, __func__, err);
            continue;
        }
    }
}

void PcmRenderer::setVolume(int master, int player0, int player1) {
    trace_printf("[%s::%s] (%d, %d, %d)\n", kClassName, __func__, master, player0, player1);
    g_mixer->setVolume(master, player0, player1);
}

bool PcmRenderer::render() {
    // trace_printf("[%s::%s] ()\n", kClassName, __func__);
    const int bytes_per_sample = (bit_depth_ / 8) * channels_;
    const size_t frame_size = bytes_per_sample * samples_per_frame_;

    size_t read_size = frame_size;
    for (int i = 0; i < mix_channels_; i++) {
        if (states_[i] != kStateUnallocated) {
            read_size = (read_size < getReadableSize(i) ? read_size : getReadableSize(i));
        }
    }
    if (read_size < frame_size) {
        // if underflow is near, continue to sendData
        if (request_count_ - response_count_ > kUnderflowThreshold) {
            // retry after
            return false;
        }
    }

    err_t err = ERR_OK;

    AsPcmDataParam pcm;
    err = pcm.mh.allocSeg(S0_REND_PCM_BUF_POOL, frame_size);
    if (err != ERR_OK) {
        // error_printf("[%s::%s] error: failed MemoryHandler::allocSeg => %d\n", kClassName, __func__, err);
        return false;
    }

    pcm.identifier = (int32_t)frames_++;
    pcm.callback = nullptr;
    pcm.sample = frame_size / bytes_per_sample;
    pcm.size = frame_size;
    pcm.is_end = (pcm.size < frame_size);
    pcm.is_valid = (pcm.size > 0);
    pcm.bit_length = bit_depth_;

    uint8_t *raw = reinterpret_cast<uint8_t *>(pcm.mh.getPa());
    // trace_printf("[%s::%s] readable=%d, framesize=%d, valid\n", kClassName, __func__, (int)read_size, (int)frame_size);
    memset(raw, 0x00, frame_size);
    for (int i = 0; i < mix_channels_; i++) {
        if (states_[i] == kStateUnallocated) {
            continue;
        }
        read_size = (read_size < getReadableSize(i) ? read_size : getReadableSize(i));
        if (bit_depth_ == 16) {
            size_t frame_sample_size = frame_size / 2;
            int16_t *dst = reinterpret_cast<int16_t *>(raw);
            int16_t src[frame_sample_size];
            read(i, src, read_size);
            if (states_[i] == kStateAllocated) {
                for (size_t j = 0; j < frame_sample_size; j += 2) {
                    *((uint32_t *)&dst[j]) = __QADD16(*((uint32_t *)&dst[j]), *((uint32_t *)&src[j]));
                }
            } else if(states_[i] == kStateDeallocating) {
                // fade-out
                trace_printf("[%d]:Deallocate\n", i);
                for (size_t j = 0; j < frame_sample_size; j ++) {
                    src[j] = (int16_t)(src[j] * (float)(frame_sample_size - j) / (frame_sample_size));
                }

                for (size_t j = 0; j < frame_sample_size; j += 2) {
                    *((uint32_t *)&dst[j]) = __QADD16(*((uint32_t *)&dst[j]), *((uint32_t *)&src[j]));
                }
                states_[i] = kStateDeallocated;
            }
        }
        if (states_[i] == kStateDeallocating) {
            states_[i] = kStateDeallocated;
        }
        if (states_[i] == kStateDeallocated && getReadableSize(i) == 0) {
            states_[i] = kStateUnallocated;
        }
    }

    err = g_mixer->sendData(OutputMixer0, pcmProcDoneCallback, pcm);
    if (err != OUTPUTMIXER_ECODE_OK) {
        error_printf("[%s::%s] error: failed OutputMixer::sendData => %d\n", kClassName, __func__, err);
        return false;
    }

    request_count_++;
    return true;
}

#if 0
void PcmRenderer::onError(const ErrorAttentionParam *attparam) {
    error_printf("[%s::%s] error: OutputMixer(error_code=%d,%d)\n", kClassName, __func__, attparam->error_code, AS_ATTENTION_CODE_WARNING);
}
#endif

void PcmRenderer::onActivated(MsgQueId requester_dtq, MsgType msgtype, AsOutputMixDoneParam *param) {
    debug_printf("[%s::%s] info: activated OutputMixer\n", kClassName, __func__);
    render();
}

void PcmRenderer::onSendData(int32_t identifier, bool is_end) {
    // trace_printf("[%s::%s] (%d, %d)\n", kClassName, __func__, identifier, is_end);
    response_count_++;
    while (true) {
        if (render()) {
            continue;
        }
        break;
    }
    if (is_end) {
        setState(kStateUnallocated);
    }
}

size_t PcmRenderer::getCapacity() {
    // trace_printf("[%s::%s] ()\n", kClassName, __func__);
    return capacity_;
}

int PcmRenderer::allocateChannel() {
    trace_printf("[%s::%s] ()\n", kClassName, __func__);
    for (int i = 0; i < mix_channels_; i++) {
        if (states_[i] == kStateUnallocated) {
            wp_[i] = rp_[i] = 0;
            states_[i] = kStateAllocating;
            debug_printf("[%s::%s] allocated %d\n", kClassName, __func__, i);
            return i;
        }
    }
    return -1;
}

void PcmRenderer::deallocateChannel(int ch) {
    trace_printf("[%s::%s] (%d)\n", kClassName, __func__, ch);
    if (0 <= ch && ch < mix_channels_) {
        states_[ch] = kStateDeallocating;
        trace_printf("[%d]:Deallocating\n", ch);
    }
}

size_t PcmRenderer::getWritableSize(int ch) {
    // trace_printf("[%s::%s] ()\n", kClassName, __func__);
    if (ch < 0 || mix_channels_ <= ch) {
        return 0;
    }
    if (rp_[ch] <= wp_[ch]) {
        return ((capacity_ - wp_[ch]) + rp_[ch]) - 1;
    } else {
        return (rp_[ch] - wp_[ch]) - 1;
    }
}

size_t PcmRenderer::getReadableSize(int ch) {
    // trace_printf("[%s::%s] ()\n", kClassName, __func__);
    if (ch < 0 || mix_channels_ <= ch) {
        return 0;
    }
    if (rp_[ch] <= wp_[ch]) {
        return wp_[ch] - rp_[ch];
    } else {
        return (capacity_ - rp_[ch]) + wp_[ch];
    }
}

void PcmRenderer::clear(int ch) {
    debug_printf("[%s::%s] deprecated\n", kClassName, __func__);
    wp_[ch] = rp_[ch] = 0;
}

size_t PcmRenderer::write(int ch, void *src, size_t request_size) {
    // trace_printf("[%s::%s] (%p, %d)\n", kClassName, __func__, src, request_size);
    // debug_printf("[%s::%s] request_size=%d, readable_size=%d, writable_size=%d\n", kClassName, __func__, request_size, getReadableSize(), getWritableSize());
    if (ch < 0 || mix_channels_ <= ch) {
        return 0;
    }
    size_t writable_size = getWritableSize(ch);
    if (writable_size < request_size) {
        return 0;
    }

    if (bit_depth_ == 16) {
        size_t frame_sample_size = request_size / 2;

        int16_t dst[frame_sample_size];
        memcpy(dst, src, request_size);
        if (states_[ch] == kStateAllocating) {
            for (size_t i = 0; i < frame_sample_size; i++) {
                dst[i] = (int16_t)(dst[i] * ((float)i / (frame_sample_size)));
            }
           states_[ch] = kStateAllocated;
        }
        trace_printf("[%d]:Allocate\n", ch);

        uint8_t *p = reinterpret_cast<uint8_t *>(dst);
        if (wp_[ch] + request_size >= capacity_) {
            size_t s = capacity_ - wp_[ch];
            memcpy(&cache_[ch][wp_[ch]], &p[0], s);
            memcpy(&cache_[ch][0], &p[s], request_size - s);
            wp_[ch] = request_size - s;
        } else {
            memcpy(&cache_[ch][wp_[ch]], p, request_size);
            wp_[ch] += request_size;
        }
    }

    return request_size;
}

size_t PcmRenderer::read(int ch, void *dst, size_t request_size) {
    // trace_printf("[%s::%s] (%p, %d)\n", kClassName, __func__, dst, request_size);
    // debug_printf("[%s::%s] request_size=%d, readable_size=%d, writable_size=%d\n", kClassName, __func__, request_size, getReadableSize(), getWritableSize());
    if (ch < 0 || mix_channels_ <= ch) {
        return 0;
    }
    size_t readable_size = getReadableSize(ch);
    if (readable_size < request_size) {
        return 0;
    }
    uint8_t *p = reinterpret_cast<uint8_t *>(dst);
    if (rp_[ch] + request_size >= capacity_) {
        size_t s = capacity_ - rp_[ch];
        memcpy(&p[0], &cache_[ch][rp_[ch]], s);
        memcpy(&p[s], &cache_[ch][0], request_size - s);
        rp_[ch] = request_size - s;
    } else {
        memcpy(p, &cache_[ch][rp_[ch]], request_size);
        rp_[ch] += request_size;
    }
    return request_size;
}

#endif  // ARDUINO_ARCH_SPRESENSE
