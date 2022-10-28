/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "TarReader.h"

#include <SDHCI.h>

//#define DEBUG (1)

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

// tar specification
const char kRegType = '0';
const size_t kBlockSize = 512;
const size_t kNameOffset = 0, kSizeOffset = 124, kTypeFlagOffset = 156;
const size_t kNameLength = 100, kSizeLength = 12;

TarReader::Stream::Stream() : file_(File()), entry_({"", 0, 0}) {
    trace_printf("%s()\n", __func__);
}

TarReader::Stream::Stream(File file, Entry entry) : file_(file), entry_(entry) {
    trace_printf("%s({%s}, {%s, %X, %d})\n", __func__, file_.name(), entry_.name.c_str(), entry_.offset, entry_.size);
}

TarReader::Stream::~Stream(void) {
    trace_printf("%s()\n", __func__);
}

size_t TarReader::Stream::write(uint8_t data) {
    // trace_printf("%s(%d)\n", __func__, data);
    return -1;
}

int TarReader::Stream::read(void) {
    // trace_printf("%s()\n", __func__);
    if (available() > 0) {
        return file_.read();
    } else {
        return -1;
    }
}

int TarReader::Stream::peek(void) {
    // trace_printf("%s()\n", __func__);
    if (available() > 0) {
        return file_.peek();
    } else {
        return -1;
    }
}

int TarReader::Stream::available(void) {
    // trace_printf("%s()\n", __func__);
    return size() - position();
}

void TarReader::Stream::flush(void) {
    // trace_printf("%s()\n", __func__);
}

int TarReader::Stream::read(void *buf, size_t nbyte) {
    // trace_printf("%s(%p, %d)\n", __func__, buf, nbyte);
    if (available() < (int)nbyte) {
        debug_printf("file end\n");
    }
    return file_.read(buf, ((int)nbyte < available()) ? nbyte : available());
}

boolean TarReader::Stream::seek(uint32_t pos) {
    // trace_printf("%s(%d)\n", __func__, pos);
    if (pos < entry_.size) {
        return file_.seek(entry_.offset + pos);
    }
    return false;
}

uint32_t TarReader::Stream::position(void) {
    // trace_printf("%s()\n", __func__);
    return file_.position() - entry_.offset;
}

uint32_t TarReader::Stream::size(void) {
    // trace_printf("%s()\n", __func__);
    return entry_.size;
}

void TarReader::Stream::close(void) {
    trace_printf("%s()\n", __func__);
}

TarReader::TarReader(const char *path) : file_(), entries_() {
    trace_printf("%s(%s)\n", __func__, path);
    if (path == nullptr) {
        return;
    }
    bool ok = true;
    SDClass sdcard;

    ok = sdcard.begin();
    if (!ok) {
        error_printf("error: cannot access sdcard\n");
        return;
    }
    file_ = sdcard.open(path);
    entries_.clear();

    // create table
    size_t block_pos = 0;
    while (block_pos < file_.size()) {
        file_.seek(block_pos + kTypeFlagOffset);
        if (file_.read() == kRegType) {
            char filename[kNameLength + 1];
            file_.seek(block_pos + kNameOffset);
            file_.read(filename, kNameLength);
            filename[kNameLength] = '\0';
            char filesize_str[kSizeLength + 1];
            file_.seek(block_pos + kSizeOffset);
            file_.read(filesize_str, kSizeLength);
            filesize_str[kSizeLength] = '\0';
            size_t filesize = (size_t)strtoul(filesize_str, nullptr, 8);

            TarReader::Entry entry;
            entry.name = String(filename);
            entry.offset = block_pos + kBlockSize;
            entry.size = filesize;
            entries_.push_back(entry);

            block_pos = (block_pos + kBlockSize + filesize) & ~(kBlockSize - 1);
            continue;
        }
        block_pos += kBlockSize;
    }

#if defined(DEBUG)
    for (const auto &e : entries_) {
        debug_printf("%s:%d: %s,%X,%d\n", __FILE__, __LINE__, e.name.c_str(), e.offset, e.size);
    }
#endif  // if defined(DEBUG)
}

TarReader::~TarReader() {
    trace_printf("%s()\n", __func__);
    entries_.clear();
    if (file_) {
        file_.close();
    }
}

TarReader::Stream TarReader::open(const char *name) {
    trace_printf("%s(%s)\n", __func__, name);
    for (const auto &e : entries_) {
        if (e.name == name) {
            debug_printf("%s: name=%s, offset=%d, size=%d\n", __func__, e.name.c_str(), e.offset, e.size);
            file_.seek(e.offset);
            TarReader::Stream stream = TarReader::Stream(file_, e);
            return stream;
        }
    }
    return TarReader::Stream();
}

TarReader::Stream TarReader::open(int index) {
    trace_printf("%s(%d)\n", __func__, index);
    if (0 <= index && (size_t)index < entries_.size()) {
        const auto &e = entries_[index];
        debug_printf("%s: name=%s, offset=%d, size=%d\n", __func__, e.name.c_str(), e.offset, e.size);
        file_.seek(e.offset);
        TarReader::Stream stream = TarReader::Stream(file_, e);
        return stream;
    }
    return TarReader::Stream();
}

const std::vector<TarReader::Entry> TarReader::getEntries() {
    trace_printf("%s()\n", __func__);
    return entries_;
}

#endif  // ARDUINO_ARCH_SPRESENSE
