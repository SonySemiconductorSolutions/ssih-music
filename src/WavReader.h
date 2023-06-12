/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file WavReader.h
 */
#ifndef WAV_READER_H_
#define WAV_READER_H_
#include <Arduino.h>
#include <SDHCI.h>

/**
 * @brief @~japanese WAVファイルまたはPCMファイルから、音声データ領域の情報を読み出します。
 */
class WavReader {
public:
    /**
     * @brief @~japanese WavReader オブジェクトを生成します。
     * @param[in] file @~japanese 入力ファイル
     */
    WavReader(File& file);

    /**
     * @brief @~japanese 音声データ領域のファイルオフセットを取得します。
     * @return @~japanese 音声データ領域のファイルオフセット
     */
    uint32_t getPcmOffset();

    /**
     * @brief @~japanese 音声データ領域のデータサイズを取得します。
     * @return @~japanese 音声データ領域のデータサイズ
     */
    uint32_t getPcmSize();

    /**
     * @brief @~japanese 入力ファイルがWAVファイルか否かを取得します。
     * @retval true WAV file
     * @retval false Not a WAV file
     */
    bool isWaveFile();

private:
    static const unsigned int kWaveChunkSize = 4;

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
