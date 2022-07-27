/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_STORAGE_H_
#define DUMMY_STORAGE_H_

#include "File.h"

class StorageClass {
public:
    StorageClass(void);
    StorageClass(const char *str);
    File open(const char *filename, uint8_t mode = FILE_READ);
    File open(const String &filename, uint8_t mode = FILE_READ);
    boolean exists(const char *filepath);
    boolean exists(const String &filepath);
    boolean mkdir(const char *filepath);
    boolean mkdir(const String &filepath);
    boolean remove(const char *filepath);
    boolean remove(const String &filepath);
    boolean rmdir(const char *filepath);
    boolean rmdir(const String &filepath);
};

extern StorageClass Storage;

#endif  // DUMMY_STORAGE_H_
