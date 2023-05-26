/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_FILE_H_
#define DUMMY_FILE_H_

#include <stdio.h>

#include "Arduino.h"
#include "Stream.h"

#define FILE_READ (1)
#define FILE_WRITE (2)

#define MAXFILELEN (128)

class File : public Stream {
private:
    char *name_;
    uint8_t mode_;
    FILE *fp_;
    unsigned long size_;
    unsigned long curpos_;
    uint8_t *dummy_file_content_;
    bool is_directory_;
    int dummy_files_index_;

public:
    File(const char *name, uint8_t mode = FILE_READ);
    File(void);
    File(const File &lhs);
    ~File(void);

    File &operator=(const File &lhs);

    virtual size_t write(uint8_t val);
    virtual size_t write(const uint8_t *buf, size_t len);
    virtual int read(void);
    virtual int peek(void);
    virtual int available(void);
    virtual void flush(void);
    virtual int read(void *buf, size_t len);
    boolean seek(uint32_t pos);
    uint32_t position(void);
    uint32_t size(void);
    void close(void);
    operator bool(void);
    char *name(void);
    boolean isDirectory(void);
    File openNextFile(uint8_t mode = FILE_READ);
    void rewindDirectory(void);
};

void registerDummyFile(const String &path, const uint8_t *content, int size);

#endif  // DUMMY_FILE_H_
