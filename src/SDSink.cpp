/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "SDSink.h"

#ifdef EMMC_USE
#include <eMMC.h>
#define EMMC_POWER_PIN 26
#define EMMC_EN 31 // High=eMMC / Low=SD
#else
#include <SDHCI.h>
#endif

#include "WavReader.h"

// #define DEBUG (1)
// #define LOW_DELAY /*Must change to special package.*/

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

static const char kClassName[] = "SDSink";

// playback parameters
// NOTICE: PcmRenderer only supports 48kHz/16bit/2ch. Otherwise, it will not work.
const int kPbSampleFrq = 48000;
const int kPbBitDepth = 16;
const int kPbChannelCount = 2;
#if LOW_DELAY
const int kPbSampleCount = 60;
const int kPbCacheSize = (8 * 1024);
#else
const int kPbSampleCount = 240;
const int kPbCacheSize = (24 * 1024);
#endif /* LOW_DELAY */
const int kPbBlockSize = kPbSampleCount * (kPbBitDepth / 8) * kPbChannelCount;

const int kDefaultOffset = 0;
const int kVolumeMin = -1020;
const int kVolumeMax = 120;
const int kDefaultVolume = 0;

// cache parameter
const int kLoadFrames = 10;

const static int kUnallocatedChannel = -1;
const static int kDeallocatedChannel = -2;

static uint32_t msToByte(int ms) {
    const int kPbBytePerSec = kPbSampleFrq * (kPbBitDepth / 8) * kPbChannelCount;
    return kPbBytePerSec * ms / 1000;
}

SDSink::SDSink(const SDSink::Item* table, size_t table_length)
    : NullFilter(),
      units_(),
      renderer_(kPbSampleFrq, kPbBitDepth, kPbChannelCount, kPbSampleCount, kPbCacheSize, 4),
      offset_(kDefaultOffset),
      loop_(false),
      volume_(kDefaultVolume) {
    for (size_t i = 0; i < sizeof(units_) / sizeof(units_[0]); i++) {
        units_[i].offset = 0;
        units_[i].end = 0;
        units_[i].render_ch = kDeallocatedChannel;
    }
    for (size_t i = 0; i < table_length; i++) {
        if (table[i].note < sizeof(units_) / sizeof(units_[0])) {
            units_[table[i].note].path = table[i].path;
        }
    }
}

SDSink::~SDSink() {
    for (size_t i = 0; i < sizeof(units_) / sizeof(units_[0]); i++) {
        if (units_[i].render_ch >= 0) {
            renderer_.deallocateChannel(units_[i].render_ch);
            units_[i].render_ch = kDeallocatedChannel;
        }
        if (units_[i].file) {
            units_[i].file.close();
        }
    }
}

bool SDSink::begin() {
    NullFilter::begin();

#ifdef EMMC_USE
    pinMode(EMMC_EN, OUTPUT);
    digitalWrite(EMMC_EN, HIGH); //eMMC Select

    pinMode(EMMC_POWER_PIN, OUTPUT);
    digitalWrite(EMMC_POWER_PIN, HIGH);
    delay(50);
    if (!eMMC.begin())
    {
        Serial.println("eMMC can't begin");
        exit(1);
    }
#else
    SDClass sdcard;
    if (!sdcard.begin()) {
        error_printf("[%s::%s] error: cannot access sdcard\n", kClassName, __func__);
        exit(1);
     }
#endif
    for (size_t i = 0; i < sizeof(units_) / sizeof(units_[0]); i++) {
        if (units_[i].path.length() == 0) {
            continue;
        }
        File file;
        if (units_[i].path[0] == '/') {
            file = File(units_[i].path.c_str());
        } else {
#ifdef EMMC_USE
            file = eMMC.open(units_[i].path.c_str());
#else
            file = sdcard.open(units_[i].path.c_str());
#endif
        }
        if (!file) {
            error_printf("[%s::%s] error: cannot open \"%s\"\n", kClassName, __func__, units_[i].path.c_str());
            continue;
        }
        if (file.isDirectory()) {
            error_printf("[%s::%s] error: \"%s\" is directory\n", kClassName, __func__, units_[i].path.c_str());
            file.close();
            continue;
        }
        WavReader wav_reader = WavReader(file);
        units_[i].offset = wav_reader.getPcmOffset();
        units_[i].end = units_[i].offset + wav_reader.getPcmSize();
        file.close();
    }

    renderer_.begin();
    return true;
}

void SDSink::update() {
    for (size_t i = 0; i < sizeof(units_) / sizeof(units_[0]); i++) {
        if (units_[i].render_ch < 0) {
            continue;
        }
        for (int j = 0; j < kLoadFrames; j++) {
            // end of file
            if (units_[i].file.position() >= units_[i].file.size()) {
                if (loop_) {
                    units_[i].file.seek(units_[i].offset + msToByte(offset_));
                } else {
                    renderer_.deallocateChannel(units_[i].render_ch);
                    units_[i].render_ch = kDeallocatedChannel;
                    units_[i].file.close();
                    break;
                }
            }

            // output
            if (renderer_.getWritableSize(units_[i].render_ch) < kPbBlockSize) {
                break;
            }
            size_t read_size = units_[i].end - units_[i].file.position();
            read_size = (read_size < kPbBlockSize) ? read_size : kPbBlockSize;
            uint8_t buffer[read_size];
            units_[i].file.read(buffer, read_size);
            renderer_.write(units_[i].render_ch, buffer, read_size);
        }
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

bool SDSink::sendNoteOff(uint8_t note, uint8_t /*velocity*/, uint8_t /*channel*/) {
    if (sizeof(units_) / sizeof(units_[0]) <= note) {
        error_printf("[%s::%s] out of note number\n", kClassName, __func__);
        return false;
    }

    if (units_[note].path.length() == 0) {
        error_printf("[%s::%s] undefined note number\n", kClassName, __func__);
        return false;
    }

    if (units_[note].render_ch >= 0) {
        renderer_.deallocateChannel(units_[note].render_ch);
        units_[note].render_ch = kDeallocatedChannel;
        units_[note].file.close();
    } else if (units_[note].render_ch == kUnallocatedChannel) {
        if (units_[note].file) {
            units_[note].render_ch = kDeallocatedChannel;
            units_[note].file.close();
        }
    }
    return true;
}

bool SDSink::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    debug_printf("[%s::%s] note:%d\n", kClassName, __func__, note);
    if (sizeof(units_) / sizeof(units_[0]) <= note) {
        error_printf("[%s::%s] out of note number\n", kClassName, __func__);
        return false;
    }

    if (units_[note].path.length() == 0) {
        error_printf("[%s::%s] undefined note number\n", kClassName, __func__);
        return false;
    }

    if (velocity == 0) {
        return sendNoteOff(note, velocity, channel);
    }

    if (units_[note].render_ch >= 0) {
        renderer_.deallocateChannel(units_[note].render_ch);
        units_[note].render_ch = kDeallocatedChannel;
        units_[note].file.close();
    } else if (units_[note].render_ch == kUnallocatedChannel) {
        units_[note].render_ch = kDeallocatedChannel;
        units_[note].file.close();
    }

    units_[note].file = File(units_[note].path.c_str());
    if (units_[note].file) {
        units_[note].file.seek(units_[note].offset + msToByte(offset_));
        int render_channel = renderer_.allocateChannel();
        units_[note].render_ch = (render_channel < 0) ? kUnallocatedChannel : render_channel;
        if (units_[note].render_ch == kUnallocatedChannel) {
            error_printf("[%s::%s] cannot allocate channel\n", kClassName, __func__);
        } else {
            update();
        }
    }
    update();
    return true;
}

bool SDSink::sendControlChange(uint8_t ctrl_num, uint8_t /*value*/, uint8_t /*channel*/) {
    if (0x7B <= ctrl_num && ctrl_num <= 0x7F) {
        debug_printf("[%s::%s] All Note Off\n", kClassName, __func__);
        for (auto& e : units_) {
            if (e.path.length() == 0) {
                continue;
            }
            renderer_.deallocateChannel(e.render_ch);
            e.render_ch = kDeallocatedChannel;
            e.file.close();
        }
    }

    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
