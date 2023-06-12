/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef PATH_UTIL_H_
#define PATH_UTIL_H_

#include <Arduino.h>

/**
 * @brief @~japanese 引数に与えられた path のフォルダパスを返します。
 * @details @~japanese "/path/to.file" が与えられたとき "/path/" を返します。
 * @param[in] path file path
 * @return folder path
 */
String getFolderPath(const String& path);

/**
 * @brief @~japanese 引数に与えられた path のファイル名部分を返します。
 * @details @~japanese "/path/to.file" が与えられたとき "to.file" を返します。
 * @param path file path
 * @return file name
 */
String getBaseName(const String& path);

/**
 * @brief @~japanese 引数に与えられた path の拡張子部分を返します。
 * @details @~japanese "/path/to.file" が与えられたとき ".file" を返します。
 *
 * @param path file path
 * @return extension
 */
String getExtension(const String& path);

/**
 * @brief @~japanese 引数に与えられた path が隠しファイルかどうかを判定します。
 * @param path file path
 * @retval true @~japanese 隠しファイル
 * @retval false @~japanese 通常ファイル
 */
bool isHidden(const String& path);

/**
 * @brief @~japanese 引数に与えられた path から "./" や "../" を削除します。
 * @param path file path
 * @return normalized path
 */
String normalizePath(const String& path);

/**
 * @brief @~japanese 引数に与えられたパスを連結します。
 * @param dir filder path
 * @param path file path
 * @return joined path
 */
String joinPath(const String& dir, const String& path);

#endif  // PATH_UTIL_H_
