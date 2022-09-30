/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef PATH_UTIL_H_
#define PATH_UTIL_H_

#include <Arduino.h>

String getFolderPath(const String& path);

String getBaseName(const String& path);

String getExtension(const String& path);

bool isHidden(const String& path);

String normalizePath(const String& path);

#endif  // PATH_UTIL_H_
