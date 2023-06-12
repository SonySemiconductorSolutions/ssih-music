/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file YuruInstrumentConsole.h
 */
#ifndef YURU_INSTRUMENT_CONSOLE_H_
#define YURU_INSTRUMENT_CONSOLE_H_

#include "YuruInstrumentConfig.h"

/**
 * @brief @~japanese Filter のパラメータ設定とファイル操作機能を持つコンソール機能を実現するクラスです。
 */
class YuruInstrumentConsole : public YuruInstrumentConfig {
public:
    /**
     * @brief @~japanese YuruInstrumentConsole オブジェクトを生成します。
     * @param[in] filter Filter object
     */
    YuruInstrumentConsole(Filter &filter);

    /**
     * @brief @~japanese YuruInstrumentConsole オブジェクトを破棄します。
     */
    ~YuruInstrumentConsole();

    /**
     * @brief @~japanese コンソール機能を使用可能な状態にします。
     */
    void begin();

    /**
     * @brief @~japanese コンソール入力を受け付けます。
     */
    void poll();

    /**
     * @brief @~japanese コンソール入力されたコマンドを実行します。
     * @param[in] line command line
     * @param[in] length command character length
     */
    void execute(char *line, int length);

private:
    size_t line_length_;
    char *line_buffer_;
    size_t line_index_;
};

#endif  // YURU_INSTRUMENT_CONSOLE_H_
