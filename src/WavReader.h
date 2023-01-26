/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef WAV_READER_H_
#define WAV_READER_H_
#include <Arduino.h>
#include <SDHCI.h>

const unsigned int kWaveChunkSize = 4;

class WavReader {
public:
    struct RIFFHeader {
        RIFFHeader();
        char chunk[kWaveChunkSize];
        uint32_t data_len;
        char riff_format[kWaveChunkSize];
    };

    struct WaveHeader {
        WaveHeader();
        char chunk[kWaveChunkSize];
        uint32_t data_len;          // Data length
        uint16_t format;            // Audio Data Format
        uint16_t ch;                // Number of channels
        uint32_t samples_per_sec;   // Sampling frequency
        uint32_t avg_byte_per_sec;  // Bytes per second
        uint16_t block_align;       // Block size
        uint16_t bits_per_sample;   // bit rate
        uint16_t cb_size;           // Extended parameters
    };

    WavReader(File& file);

    uint32_t getPcmOffset();
    uint32_t getPcmSize();
    bool isWaveFile();

private:
    uint32_t pos_;
    uint32_t size_;
    WaveHeader wave_header_;
    bool is_wave_file_;

    bool load(File& file);
    bool parseHeader(File& file);
    bool parseRIFFHeader(File& file, RIFFHeader* riffh);
    bool parseWaveHeader(File& file, WaveHeader* waveh);
};

#endif  // WAV_READER_H_
