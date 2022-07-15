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
        uint32_t data_len;          //データ長
        uint16_t format;            //音声データのフォーマット
        uint16_t ch;                //チャンネル数
        uint32_t samples_per_sec;   //サンプリング周波数
        uint32_t avg_byte_per_sec;  // 1秒あたりのバイト数
        uint16_t block_align;       //ブロックサイズ
        uint16_t bits_per_sample;   //ビットレート
        uint16_t cb_size;           //拡張パラメータ
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