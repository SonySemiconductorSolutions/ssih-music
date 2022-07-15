/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef TARREADER_H_
#define TARREADER_H_

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include <Arduino.h>
#include <File.h>

class TarReader {
public:
    struct Entry {
        String name;
        size_t offset;
        size_t size;
    };

    class Stream : public ::Stream {
    public:
        Stream();
        Stream(File file, Entry entry);
        ~Stream(void);
        virtual size_t write(uint8_t data);
        virtual int read(void);
        virtual int peek(void);
        virtual int available(void);
        virtual void flush(void);
        virtual int read(void *buf, size_t nbyte);
        boolean seek(uint32_t pos);
        uint32_t position(void);
        uint32_t size(void);
        void close(void);

    private:
        File file_;
        Entry entry_;
    };

    TarReader(const char *path);
    ~TarReader();
    Stream open(const char *name);
    Stream open(int index);
    const std::vector<Entry> getEntries();

private:
    File file_;
    std::vector<Entry> entries_;
};

#endif  // TARREADER_H_
