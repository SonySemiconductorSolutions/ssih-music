/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file SFZParser.h
 */
#ifndef SFZ_PARSER_H_
#define SFZ_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "path_util.h"

/**
 * @brief @~japanese SFZParser のハンドラーです。
 * @code {.cpp}
 * #include <SFZParser.h>
 *
 * class SFZDump : public SFZHandler {
 * public:
 *     void startSfz() override {
 *     }
 *     void endSfz() override {
 *     }
 *     void startHeader(const String& header) override {
 *         Serial.print("<");
 *         Serial.print(header);
 *         Serial.println(">");
 *     }
 *     void endHeader(const String& header) override {
 *     }
 *     void opcode(const String& opcode, const String& value) override {
 *         Serial.print(opcode);
 *         Serial.print("=");
 *         Serial.println(value);
 *     }
 * };
 *
 * void setup() {
 *     File sfz = File("input.sfz");
 *     SFZParser parser;
 *     SFZDump dump;
 *     parser.parse(sfz, "input.sfz", &dump);
 * }
 *
 * void loop() {
 * }
 * @endcode
 *
 */
class SFZHandler {
public:
    /**
     * @brief @~japanese ドキュメントの開始通知を受けます。
     */
    virtual void startSfz() = 0;
    /**
     * @brief @~japanese ドキュメントの終了通知を受けます。
     */
    virtual void endSfz() = 0;
    /**
     * @brief @~japanese ヘッダーの開始通知を受けます。
     */
    virtual void startHeader(const String& header) = 0;
    /**
     * @brief @~japanese ヘッダーの終了通知を受けます。
     */
    virtual void endHeader(const String& header) = 0;
    /**
     * @brief @~japanese Opcode代入文の通知を受けます。
     */
    virtual void opcode(const String& opcode, const String& value) = 0;
};

/**
 * @brief @~japanese SFZファイルのパーサーです。
 */
class SFZParser {
public:
    /**
     * @brief @~japanese SFZParser オブジェクトを生成します。
     */
    SFZParser();

    ~SFZParser();

    /**
     * @brief @~japanese SFZファイルを解析し、ハンドラーのコールバック関数を呼び出します。
     * @param[in] sfz_file @~japanese SFZファイル
     * @param[in] sfz_file_path @~japanese SFZファイル名
     * @param[in] handler @~japanese ハンドラー
     */
    void parse(File sfz_file, const String& sfz_file_path, SFZHandler* handler);

private:
    int state_;
    bool parsing_root_;
    String header_name_;
    std::vector<String> define_names_;
    std::vector<String> define_values_;
};

#endif  // SFZ_PARSER_H_
