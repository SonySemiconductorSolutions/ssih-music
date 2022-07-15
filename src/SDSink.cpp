/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "SDSink.h"

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
const int kPbSamoleCount = 240;
const int kPbBlockSize = kPbSamoleCount * (kPbBitDepth / 8) * kPbChannelCount;
const int kPbCacheSize = (24 * 1024);

const int kPbBytePerSec = kPbSampleFrq * (kPbBitDepth / 8) * kPbChannelCount;

// const int kDefaultOffset = 200;
const int kDefaultOffset = 0;

const int kVolumeMin = -1020;
const int kVolumeMax = 120;

// cache parameter
const int kPreloadFrameNum = 3;
const int kLoadFrameNum = 10;

SDSink::PcmData::PcmData() : note(INVALID_NOTE_NUMBER), path(""), size(0), offset(0) {
}

static uint32_t msToSa(int ms) {
    return kPbSampleFrq * ms / 1000 * (kPbBitDepth / 8) * kPbChannelCount;
}

static uint32_t msToByte(int ms) {
    return kPbBytePerSec * ms / 1000;
}

static void writeToRenderer(PcmRenderer &renderer, int ch, File &file, size_t offset, size_t pcm_offset, size_t pcm_size, size_t *remain_pcm_size, bool loop) {
    // trace_printf("%s: writable=%d\n", __func__, renderer.getWritableSize());
    if (renderer.getWritableSize(ch) >= kPbBlockSize) {
        // WAVEファイル対応(Size)
        size_t read_size = 0;
        // trace_printf("(%s) (pcm_size:%d) remain_pcm_size:%d\n", __func__, pcm_size, (*remain_pcm_size));
        if (kPbBlockSize <= (*remain_pcm_size)) {
            read_size = kPbBlockSize;
        } else {
            read_size = (*remain_pcm_size);
        }
        uint8_t buffer[read_size];
        read_size = file.read(buffer, read_size);
        renderer.write(ch, buffer, read_size);
        if (read_size < kPbBlockSize || (*remain_pcm_size) == 0) {
            if (loop) {
                file.seek(pcm_offset + offset);
                (*remain_pcm_size) = pcm_size;
                debug_printf("file finish\n");
            } else {
                (*remain_pcm_size) = 0;
            }
        } else {
            (*remain_pcm_size) -= kPbBlockSize;
        }
    }
}

SDSink::SDSink(const SDSink::Item *table, size_t table_length)
    : NullFilter(),
      offset_(kDefaultOffset),
      volume_(0),
      renderer_(kPbSampleFrq, kPbBitDepth, kPbChannelCount, kPbSamoleCount, kPbCacheSize, 1),
      table_index_(-1),
      remain_pcm_size_(0),
      loop_(false) {
    for (size_t i = 0; i < table_length; i++) {
        PcmData pcm_data = PcmData();
        pcm_data.note = table[i].note;
        pcm_data.path = table[i].path;
        // wave
        SDClass sd;
        File file = sd.open(pcm_data.path.c_str());
        WavReader wav_reader = WavReader(file);
        pcm_data.size = wav_reader.getPcmSize();
        pcm_data.offset = wav_reader.getPcmOffset();
        file.close();

        pcm_table_.push_back(pcm_data);
    }

    for (size_t i = 0; i < pcm_table_.size(); i++) {
        debug_printf("pcm_note:%3d, pcm_path:%s, pcm_size:%06d, pcm_offset:%6d\n", pcm_table_[i].note, pcm_table_[i].path.c_str(), pcm_table_[i].size,
                     pcm_table_[i].offset);
    }
}

SDSink::~SDSink() {
}

bool SDSink::begin() {
    bool ok = true;

    // setup renderer
    renderer_.begin();

    debug_printf("start playback\n");

    // preload sound
    for (int i = 0; i < kPreloadFrameNum; i++) {
        if (!renderer_.render()) {
            break;
        }
    }

    return ok;
}

void SDSink::update() {
    switch (renderer_.getState()) {
        case PcmRenderer::kStateReady:
            break;
        case PcmRenderer::kStateActive:
            for (int i = 0; i < kLoadFrameNum; i++) {
                if (0 <= table_index_) {
                    writeToRenderer(renderer_, 0, file_, msToSa(offset_), pcm_table_[table_index_].offset, pcm_table_[table_index_].size, &remain_pcm_size_,
                                    loop_);
                }
            }
            break;
        case PcmRenderer::kStatePause:
            break;
        default:
            break;
    }
}

bool SDSink::isAvailable(int param_id) {
    if (param_id == PARAMID_OFFSET) {
        return true;
    } else if (param_id == PARAMID_LOOP) {
        return true;
    } else if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        return true;
    }
    return NullFilter::isAvailable(param_id);
}

intptr_t SDSink::getParam(int param_id) {
    if (param_id == PARAMID_OFFSET) {
        return offset_;
    } else if (param_id == PARAMID_LOOP) {
        return loop_;
    } else if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        return volume_;
    }
    return NullFilter::getParam(param_id);
}

bool SDSink::setParam(int param_id, intptr_t value) {
    if (param_id == PARAMID_OFFSET) {
        offset_ = (value < 0) ? 0 : value;
        return true;
    } else if (param_id == PARAMID_LOOP) {
        loop_ = (value != 0);
        return true;
    } else if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        volume_ = constrain(value, kVolumeMin, kVolumeMax);
        renderer_.setVolume(volume_, 0, 0);
        return true;
    }
    return NullFilter::setParam(param_id, value);
}

bool SDSink::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (NOTE_NUMBER_MIN <= note && note <= NOTE_NUMBER_MAX) {
        // find file
        table_index_ = -1;
        for (size_t i = 0; i < pcm_table_.size(); i++) {
            if (note == pcm_table_[i].note) {
                table_index_ = i;
                break;
            }
        }
        file_.close();
        if (table_index_ < 0) {
            error_printf("error: note is nothing(note=%d)\n", note);
            return false;
        }

        SDClass sd;
        file_ = sd.open(pcm_table_[table_index_].path.c_str());
        if (!file_) {
            error_printf("error: file open error(%s)\n", pcm_table_[table_index_].path.c_str());
            return false;
        }

        remain_pcm_size_ = pcm_table_[table_index_].size;

        uint32_t offset = msToByte(offset_);
        file_.seek(pcm_table_[table_index_].offset + offset);
        renderer_.clear(channel);
        for (int i = 0; i < kPreloadFrameNum; i++) {
            writeToRenderer(renderer_, channel, file_, offset, pcm_table_[table_index_].offset, pcm_table_[table_index_].size, &remain_pcm_size_, loop_);
        }
        renderer_.setState(PcmRenderer::kStateActive);
    }
    return true;
}

bool SDSink::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    renderer_.clear(channel);
    renderer_.setState(PcmRenderer::kStatePause);
    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
