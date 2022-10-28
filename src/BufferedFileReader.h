/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef BUFFERED_FILE_READER_H_
#define BUFFERED_FILE_READER_H_

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

class BufferedFileReader {
public:
    BufferedFileReader(File& file);
    BufferedFileReader(File& file, size_t buf_size);
    ~BufferedFileReader();

    BufferedFileReader(const BufferedFileReader& rhs);
    BufferedFileReader& operator=(const BufferedFileReader& rhs);

    // void seek(size_t position);
    int read(void* dst, size_t request_size);
    int read(void);
    boolean seek(uint32_t pos);
    int available(void);
    uint32_t position(void);
    uint32_t size(void);

    char* name(void);  //返り値は基のArduinoのそれに合わせる
    String readStringUntil(char terminator);

private:
    uint8_t* buf_;     //バッファ
    size_t position_;  // ファイルのアドレス(バッファの先頭部分)
    size_t capacity_;

    size_t offset_;  // バッファ内のアドレス
    size_t size_;    //バッファから読み出せるデータサイズ
    File file_;      //ファイル
};

// class BufferedFileReader
#endif  // BUFFERED_FILE_READER_H_
