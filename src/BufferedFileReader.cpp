/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "BufferedFileReader.h"

#include <Arduino.h>
#include "YuruInstrumentFilter.h"

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

const int kBufLen = 10 * 1024;
BufferedFileReader::BufferedFileReader(File& file) : BufferedFileReader(file, kBufLen) {
}

BufferedFileReader::BufferedFileReader(File& file, size_t buf_size) : position_(0), capacity_(buf_size), offset_(0), size_(0), file_() {
    buf_ = new uint8_t[capacity_];
    if (file.isDirectory()) {
        error_printf("BufferedFileReader Error:this file is directory.\n");
        return;
    }
    memset(buf_, 0x00, capacity_);

    file_ = file;
}

BufferedFileReader::BufferedFileReader(const BufferedFileReader& rhs) : buf_(nullptr), position_(0), capacity_(rhs.capacity_), offset_(0), size_(0), file_() {
    *this = rhs;
}

BufferedFileReader& BufferedFileReader::operator=(const BufferedFileReader& rhs) {
    if (buf_) {
        delete[] buf_;
    }
    buf_ = new uint8_t[rhs.capacity_];
    position_ = rhs.position_;
    capacity_ = rhs.capacity_;
    offset_ = 0;
    size_ = 0;
    file_ = rhs.file_;
    return *this;
}

BufferedFileReader::~BufferedFileReader() {
    if (buf_) {
        delete[] buf_;
        buf_ = nullptr;
    }
}

int BufferedFileReader::read(void) {
    uint8_t data = 0;
    if (read(&data, 1) == 1) {
        return (int)data;
    } else {
        return -1;
    }
}

int BufferedFileReader::read(void* dst, size_t request_size) {
    uint8_t* p = reinterpret_cast<uint8_t*>(dst);
    if (p == nullptr) {
        return -1;
    }

    if (available() == 0) {
        return -1;
    }

    //ファイル全体の読み出し量とリクエストサイズを比較してOut-of-boundsを出さないようにする
    size_t remain_size = ((size_t)available() < request_size) ? (size_t)available() : request_size;
    size_t read_size = 0;

    while (0 < remain_size) {
        trace_printf("size_:%u, offset_:%u, remain_size:%u\n", (unsigned int)size_, (unsigned int)offset_, (unsigned int)remain_size);
        if (size_ <= offset_ + remain_size) {
            //バッファ内に残っているデータすべてをコピー
            size_t readable_size = size_ - offset_;
            memcpy(&p[read_size], &buf_[offset_], readable_size);
            offset_ += readable_size;
            read_size += readable_size;
            remain_size -= readable_size;
            //新たにデータをキャッシュする
            size_ = (file_.available() < (int)capacity_) ? file_.available() : (int)capacity_;
            trace_printf("capacity_:%u, file_.available():%u\n", (unsigned int)capacity_, (unsigned int)file_.available());
            if (size_ > 0) {
                position_ = file_.position();
                int ret = file_.read(buf_, size_);
                if (ret >= 0) {
                    size_ = (size_t)ret;
                } else {
                    return (int)read_size;
                }
                offset_ = 0;
            } else {
                return (int)read_size;
            }
        } else {
            memcpy(&p[read_size], &buf_[offset_], remain_size);
            offset_ += remain_size;
            read_size += remain_size;
            remain_size -= remain_size;
        }
    }
    return (int)read_size;
}

boolean BufferedFileReader::seek(uint32_t pos) {
    if (position_ <= pos && pos < position_ + size_) {  // キャッシュしている範囲内にseekする→読み出し位置だけを調整する
        offset_ = pos - position_;
        return true;
    } else {  // キャッシュしている範囲外にseekする→ファイルをシークしてキャッシュを無効化
        size_ = 0;
        offset_ = 0;
        position_ = pos;
        return file_.seek(pos);
    }
}

int BufferedFileReader::available(void) {
    return size() - position();
}
uint32_t BufferedFileReader::position(void) {
    return position_ + offset_;
}
uint32_t BufferedFileReader::size(void) {
    return file_.size();
}

char* BufferedFileReader::name(void) {
    return file_.name();
}

String BufferedFileReader::readStringUntil(char terminator) {
    String ret;
    int c = read();
    while (c >= 0 && c != terminator) {
        ret += (char)c;
        c = read();
    }
    return ret;
}

#endif  // ARDUINO_ARCH_SPRESENSE
