/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "SDSeekSink.h"

#include <SDHCI.h>

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

// playback parameters
const int kPbSampleFrq = 48000;
const int kPbBitDepth = 16;
const int kPbChannelCount = 2;
const int kPbSampleCount = 240;
const int kPbBlockSize = kPbSampleCount * (kPbBitDepth / 8) * kPbChannelCount;
const int kPbCacheSize = (24 * 1024);

// cache parameter
const int kPreloadFrameNum = 3;
const int kLoadFrameNum = 10;

static void writeToRenderer(PcmRenderer &renderer, int ch, TarReader::Stream &stream) {
    // trace_printf("%s: writable=%d\n", __func__, renderer.getWritableSize());
    if (renderer.getWritableSize(ch) >= kPbBlockSize) {
        uint8_t buffer[kPbBlockSize];
        size_t read_size = stream.read(buffer, sizeof(buffer));
        renderer.write(ch, buffer, read_size);
        if (read_size < sizeof(buffer)) {
            stream.seek(0);
        }
    }
}

SDSeekSink::SDSeekSink(const String &file_path)
    : NullFilter(), reader_(file_path.c_str()), volume_(0), renderer_(kPbSampleFrq, kPbBitDepth, kPbChannelCount, kPbSampleCount, kPbCacheSize, 1) {
    trace_printf("%s(%s)\n", __func__, file_path.c_str());
}

SDSeekSink::~SDSeekSink() {
    trace_printf("%s()\n", __func__);
}

bool SDSeekSink::begin() {
    trace_printf("%s()\n", __func__);
    bool ok = true;

    // parse tar file
    SDClass sdcard;
    ok = sdcard.begin();
    if (!ok) {
        error_printf("error: cannot access sdcard\n");
    }
    std::vector<TarReader::Entry> tar = reader_.getEntries();
    table_.clear();
    table_.resize(tar.size());
    for (size_t i = 0; i < table_.size(); i++) {
        table_[i] = INVALID_NOTE_NUMBER;
    }
    // build sound index
    TarReader::Stream csv = reader_.open("soundsrc.txt");
    while (csv.available()) {
        String line = csv.readStringUntil('\n');
        int sep = line.indexOf(',');
        if (sep >= 0) {
            int note = line.substring(0, sep).toInt();
            String name = line.substring(sep + 1);
            for (size_t i = 0; i < tar.size(); i++) {
                if (name == tar[i].name) {
                    debug_printf("%s: note=%d, name=%s, index=%d\n", __func__, note, tar[i].name.c_str(), i);
                    if (NOTE_NUMBER_MIN <= note && note <= NOTE_NUMBER_MAX) {
                        table_[i] = note;
                    }
                }
            }
        }
    }

    // setup renderer
    renderer_.begin();

    // preload sound
    for (int i = 0; i < kPreloadFrameNum; i++) {
        if (!renderer_.render()) {
            break;
        }
    }

    return true;
}

void SDSeekSink::update() {
    trace_printf("%s()\n", __func__);
    switch (renderer_.getState()) {
        case PcmRenderer::kStateReady:
            break;
        case PcmRenderer::kStateActive:
            for (int i = 0; i < kLoadFrameNum; i++) {
                writeToRenderer(renderer_, 0, stream_);
            }
            break;
        case PcmRenderer::kStatePause:
            break;
        default:
            break;
    }
}

bool SDSeekSink::isAvailable(int param_id) {
    trace_printf("%s(%d)\n", __func__, param_id);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        return true;
    }
    return NullFilter::isAvailable(param_id);
}

intptr_t SDSeekSink::getParam(int param_id) {
    trace_printf("%s(%d)\n", __func__, param_id);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        return volume_;
    }
    return NullFilter::getParam(param_id);
}

bool SDSeekSink::setParam(int param_id, intptr_t value) {
    trace_printf("%s(%d, %d)\n", __func__, param_id, value);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        volume_ = constrain(value, -1020, 120);
        renderer_.setVolume(volume_, 0, 0);
        return true;
    }
    return NullFilter::setParam(param_id, value);
}

bool SDSeekSink::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    debug_printf("%s(%d,%d,%d)\n", __func__, note, velocity, channel);
    if (NOTE_NUMBER_MIN <= note && note <= NOTE_NUMBER_MAX) {
        // find sound index
        int index = -1;
        for (size_t i = 0; i < table_.size(); i++) {
            if (table_[i] == note) {
                index = i;
                break;
            }
        }
        // write sound
        debug_printf("%s: index=%d\n", __func__, index);
        if (index >= 0) {
            stream_ = reader_.open(index);
            renderer_.clear(channel);
            for (int i = 0; i < kPreloadFrameNum; i++) {
                writeToRenderer(renderer_, channel, stream_);
            }
            renderer_.setState(PcmRenderer::kStateActive);
        }
    }
    return true;
}

bool SDSeekSink::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    debug_printf("%s(%d,%d,%d)\n", __func__, note, velocity, channel);
    renderer_.clear(channel);
    renderer_.setState(PcmRenderer::kStatePause);
    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
