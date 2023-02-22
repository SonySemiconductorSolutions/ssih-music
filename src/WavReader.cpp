/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "WavReader.h"

#include <string.h>

//#define DEBUG (1)

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#ifdef DEBUG
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else  // DEBUG
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // DEBUG

static const char kClassName[] = "WavReader";

const int kSampleFrq = 48000;
const int kBitDepth = 16;
const int kChannelCount = 2;

static uint32_t swap32(uint32_t v) {
    return (((v >> 0) & 0xFF) << 24) | (((v >> 8) & 0xFF) << 16) | (((v >> 16) & 0xFF) << 8) | (((v >> 24) & 0xFF) << 0);
}

static uint16_t swap16(uint16_t v) {
    return (((v >> 0) & 0xFF) << 8) | (((v >> 8) & 0xFF) << 0);
}

static uint32_t getByte32LE(File& file) {
    uint32_t ret = 0;
    for (unsigned int i = 0; i < sizeof(ret); i++) {
        ret = ret << 8;
        ret += file.read();
    }
    ret = swap32(ret);
    return ret;
}

static uint16_t getByte16LE(File& file) {
    uint16_t ret = 0;
    for (unsigned int i = 0; i < sizeof(ret); i++) {
        ret = ret << 8;
        ret += file.read();
    }
    ret = swap16(ret);
    return ret;
}

WavReader::RIFFHeader::RIFFHeader() : data_len(0) {
}

WavReader::WaveHeader::WaveHeader() : data_len(0), format(0), ch(0), samples_per_sec(0), avg_byte_per_sec(0), block_align(0), bits_per_sample(0), cb_size(0) {
}

WavReader::WavReader(File& file) : pos_(0), size_(0), wave_header_(WaveHeader()), is_wave_file_(false) {
    is_wave_file_ = load(file);
}

uint32_t WavReader::getPcmOffset() {
    return pos_;
}
uint32_t WavReader::getPcmSize() {
    return size_;
}
bool WavReader::isWaveFile() {
    return is_wave_file_;
}

bool WavReader::load(File& file) {
    if (!file) {
        error_printf("[%s::%s] error: %s invalid\n", kClassName, __func__, file.name());
        return false;
    }
    trace_printf("[%s::%s] %s valid\n", kClassName, __func__, file.name());

    if (!parseHeader(file)) {
        pos_ = 0;
        size_ = file.size();
        return false;
    }

    uint32_t data_len = 0;
    while (true) {
        // Music data check
        char riff_chunk[kWaveChunkSize + 1];
        for (unsigned int i = 0; i < kWaveChunkSize; i++) {
            riff_chunk[i] = file.read();
        }
        riff_chunk[kWaveChunkSize] = '\0';
        trace_printf("[%s::%s] chunk:%s\n", kClassName, __func__, riff_chunk);
        data_len = getByte32LE(file);
        if (memcmp(riff_chunk, "data", kWaveChunkSize) == 0) {
            pos_ = file.position();
            size_ = data_len;
            break;
        }
        file.seek(file.position() + data_len);
        if (!file.available()) {
            return false;
        }
    }

    trace_printf("[%s::%s] music_start(data Length:%d)\n", kClassName, __func__, size_);
    if (size_ == 0) {
        return false;
    }

    return true;
}

bool WavReader::parseHeader(File& file) {
    RIFFHeader riffh = RIFFHeader();
    debug_printf("[%s::%s] %s parse start.\n", kClassName, __func__, file.name());

    if (!parseRIFFHeader(file, &riffh)) {
        debug_printf("[%s::%s] %s is not WAVE.\n", kClassName, __func__, file.name());
        return false;
    }
    if (memcmp(riffh.riff_format, "WAVE", sizeof(riffh.riff_format)) != 0) {
        debug_printf("[%s::%s] %s is not WAVE Format File.\n", kClassName, __func__, file.name());
        return false;
    }

    trace_printf("[%s::%s] this file is WAVE Format File.\n", kClassName, __func__);

    if (!parseWaveHeader(file, &wave_header_)) {
        debug_printf("[%s::%s] %s is not supported.\n", kClassName, __func__, file.name());
        return false;
    }

    if (!(wave_header_.samples_per_sec == kSampleFrq && wave_header_.ch == kChannelCount && wave_header_.bits_per_sample == kBitDepth)) {
        return false;
    }

    return true;
}

bool WavReader::parseRIFFHeader(File& file, RIFFHeader* riffh) {
    char riff_chunk[kWaveChunkSize + 1];
    for (unsigned int i = 0; i < kWaveChunkSize; i++) {
        riff_chunk[i] = file.read();
    }
    riff_chunk[kWaveChunkSize] = '\0';
    memcpy(riffh->chunk, riff_chunk, kWaveChunkSize);
    if (memcmp(riffh->chunk, "RIFF", kWaveChunkSize) != 0) {
        return false;
    }

    trace_printf("[%s::%s] this file is RIFF Format File.\n", kClassName, __func__);
    riffh->data_len = getByte32LE(file);
    trace_printf("[%s::%s] file len = %d\n", kClassName, __func__, riffh->data_len);

    for (unsigned int i = 0; i < sizeof(riffh->riff_format); i++) {
        riffh->riff_format[i] = file.read();
    }
    return true;
}

bool WavReader::parseWaveHeader(File& file, WaveHeader* waveh) {
    trace_printf("[%s::%s] Sub chunk prase\n", kClassName, __func__);
    while (true) {
        char wave_chunk[kWaveChunkSize + 1];
        for (unsigned int i = 0; i < kWaveChunkSize; i++) {
            wave_chunk[i] = file.read();
        }
        wave_chunk[kWaveChunkSize] = '\0';
        memcpy(waveh->chunk, wave_chunk, kWaveChunkSize);
        if (memcmp(waveh->chunk, "fmt ", kWaveChunkSize) == 0) {
            break;
        }

        waveh->data_len = getByte32LE(file);

        file.seek(file.position() + waveh->data_len);
        if (!file.available()) {
            return false;
        }
    }

    waveh->data_len = getByte32LE(file);
    trace_printf("[%s::%s]   Sub chunk len    = %d\n", kClassName, __func__, waveh->data_len);

    waveh->format = getByte16LE(file);
    trace_printf("[%s::%s]   Format           = %d\n", kClassName, __func__, waveh->format);

    waveh->ch = getByte16LE(file);
    debug_printf("[%s::%s] @ Channel          = %d\n", kClassName, __func__, waveh->ch);

    waveh->samples_per_sec = getByte32LE(file);
    debug_printf("[%s::%s] @ Samples per sec  = %d\n", kClassName, __func__, waveh->samples_per_sec);

    waveh->avg_byte_per_sec = getByte32LE(file);
    trace_printf("[%s::%s]   Avg byte per sec = %d\n", kClassName, __func__, waveh->avg_byte_per_sec);

    waveh->block_align = getByte16LE(file);
    trace_printf("[%s::%s]   Block align      = %d\n", kClassName, __func__, waveh->block_align);

    waveh->bits_per_sample = getByte16LE(file);
    debug_printf("[%s::%s] @ Bits per sample  = %d\n\n", kClassName, __func__, waveh->bits_per_sample);

    // Eliminate other data in the subchunk because it is not needed
    if (16 < waveh->data_len) {
        file.seek(file.position() + (waveh->data_len - 16));
    }

    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
