/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef PATH_UTIL_H_
#define PATH_UTIL_H_

#include <Arduino.h>

String getFolderPath(const String& file_path);

String normalizePath(const String& input_path);

#endif  // PATH_UTIL_H_
