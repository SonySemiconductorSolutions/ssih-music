/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "SFZParser.h"

#include <stdint.h>

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

// #define DEBUG (1)

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

static const char kClassName[] = "SFZParser";

enum State {
    kReady,
    kExpectMacro,
    kMacro,
    kExpectDefineName,
    kDefineName,
    kExpectDefineValue,
    kDefineValue,
    kExpectIncludePath,
    kIncludePath,
    kIncludeEsc,
    kHeader,
    kExpectHeaderName,
    kHeaderName,
    kOpcode,
    kWaitEqual,
    kWaitValue,
    kValue
};

static String replaceString(const String& str, std::vector<String>& names, std::vector<String>& values) {
    String ret = str;
    int pos = 0;
    while ((pos = ret.indexOf('$', pos)) >= 0) {
        int end = pos;
        for (end = pos + 1; end < (int)ret.length(); end++) {
            if (!isAlphaNumeric(ret[end]) && ret[end] != '_') {
                break;
            }
        }
        String name = ret.substring(pos, end);
        for (size_t i = 0; i < names.size(); i++) {
            if (names[i] == name) {
                ret = ret.substring(0, pos) + values[i] + ret.substring(end);
                break;
            }
        }
        pos++;
    }
    return ret;
}

SFZParser::SFZParser() : state_(kReady), parsing_root_(true), header_name_(), define_names_(), define_values_() {
}

SFZParser::~SFZParser() {
}

static int isGraphX(int c) {
    return (c >= 0 && (isGraph(c) || (c & 0x80)));
}

void SFZParser::parse(File sfz_file, const String& sfz_file_path, SFZHandler* handler) {
    bool skip_line = false;
    String macro_name;

    String include_path;
    String define_name;
    String define_value;
    String opcode_name;
    String value_str;
    int value_tokens = 0;
    int value_last_token_pos = -1;

    bool parsing_root = parsing_root_;
    parsing_root_ = false;
    if (parsing_root) {
        state_ = kReady;
        handler->startSfz();
    }
    int prev_ch = -1;
    int next_ch = -1;
    while (true) {
        int ch = -1;
        if (next_ch >= 0) {
            ch = next_ch;
            next_ch = -1;
        } else {
            ch = sfz_file.read();
        }
        if (ch >= 0) {
            if (skip_line) {
                if (ch == '\r' || ch == '\n') {
                    skip_line = false;
                } else {
                    continue;
                }
            }
            if (ch == '/') {
                next_ch = sfz_file.read();
                if (next_ch == '/') {
                    skip_line = true;
                    next_ch = -1;
                    continue;
                }
            }
        }
        trace_printf("[%s::%s] state=%d ch=0x%02X(%c)\n", kClassName, __func__, state_, ch, (ch < 0 || ch == '\r' || ch == '\n') ? ' ' : ch);
        if (state_ == kReady) {
            if (ch == '#') {
                state_ = kExpectMacro;
                macro_name = "";
            } else if (ch == '<') {
                if (header_name_ != "") {
                    handler->endHeader(header_name_);
                    header_name_ = "";
                }
                state_ = kExpectHeaderName;
            } else if (ch == '=') {
                state_ = kWaitValue;
            } else if (isGraphX(ch)) {
                state_ = kOpcode;
                opcode_name = String((char)ch);
            }
        } else if (state_ == kExpectMacro) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state_ = kReady;
            } else if (isGraphX(ch)) {
                state_ = kMacro;
                macro_name = String((char)ch);
            }
        } else if (state_ == kMacro) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state_ = kReady;
            } else if (isWhitespace(ch)) {
                if (macro_name == "define") {
                    state_ = kExpectDefineName;
                    define_name = "";
                } else if (macro_name == "include") {
                    state_ = kExpectIncludePath;
                    include_path = "";
                } else {
                    state_ = kReady;
                }
            } else {
                macro_name += (char)ch;
            }
        } else if (state_ == kExpectDefineName) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state_ = kReady;
            } else if (ch == '$') {
                state_ = kDefineName;
                define_name = String((char)ch);
            }
        } else if (state_ == kDefineName) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state_ = kReady;
            } else if (isGraphX(ch)) {
                define_name += (char)ch;
            } else {
                state_ = kExpectDefineValue;
                define_value = "";
            }
        } else if (state_ == kExpectDefineValue) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state_ = kReady;
            } else if (isGraphX(ch)) {
                state_ = kDefineValue;
                define_value = String((char)ch);
            }
        } else if (state_ == kDefineValue) {
            if (ch < 0 || ch == '\r' || ch == '\n' || isWhitespace(ch)) {
                state_ = kReady;
                String n = replaceString(define_name, define_names_, define_values_);
                String v = replaceString(define_value, define_names_, define_values_);
                define_names_.push_back(n);
                define_values_.push_back(v);
            } else if (isGraphX(ch)) {
                define_value += (char)ch;
            }
        } else if (state_ == kExpectIncludePath) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state_ = kReady;
            } else if (ch == '"') {
                state_ = kIncludePath;
                include_path = "";
            }
        } else if (state_ == kIncludePath) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state_ = kReady;
            } else if (ch == '"') {
                state_ = kReady;
                String path = joinPath(getFolderPath(sfz_file_path), include_path);
                File include_file = File(path.c_str());
                if (include_file) {
                    debug_printf("[%s::%s] parsing '%s'\n", kClassName, __func__, path.c_str());
                    parse(include_file, sfz_file_path, handler);
                    include_file.close();
                } else {
                    error_printf("[%s::%s] error: cannot open '%s'\n", kClassName, __func__, path.c_str());
                }
            } else if (ch == '\\') {
                state_ = kIncludeEsc;
            } else {
                include_path += (char)ch;
            }
        } else if (state_ == kIncludeEsc) {
            if (ch >= 0) {
                include_path += (char)ch;
            }
            state_ = kIncludePath;
        } else if (state_ == kExpectHeaderName) {
            if (ch < 0) {
                state_ = kReady;
            } else if (isGraphX(ch)) {
                state_ = kHeaderName;
                header_name_ = String((char)ch);
            }
        } else if (state_ == kHeaderName) {
            if (ch < 0) {
                state_ = kReady;
            } else if (ch == '<') {
                state_ = kExpectHeaderName;
            } else if (ch == '>') {
                header_name_.trim();
                handler->startHeader(header_name_);
                state_ = kReady;
            } else if (isGraphX(ch)) {
                header_name_ += (char)ch;
            }
        } else if (state_ == kOpcode) {
            if (ch < 0) {
                state_ = kReady;
            } else if (ch == '<') {
                state_ = kExpectHeaderName;
            } else if (ch == '=') {
                state_ = kWaitValue;
            } else if (isWhitespace(ch)) {
                state_ = kWaitEqual;
            } else if (isGraphX(ch)) {
                opcode_name += (char)ch;
            }
        } else if (state_ == kWaitEqual) {
            if (ch < 0) {
                state_ = kReady;
            } else if (ch == '<') {
                state_ = kExpectHeaderName;
            } else if (ch == '=') {
                state_ = kWaitValue;
            } else if (isGraphX(ch)) {
                state_ = kReady;
                next_ch = ch;
            }
        } else if (state_ == kWaitValue) {
            if (ch < 0) {
                state_ = kReady;
            } else if (ch == '<') {
                state_ = kExpectHeaderName;
            } else if (isGraphX(ch)) {
                state_ = kValue;
                value_str = String((char)ch);
                value_tokens = 1;
                value_last_token_pos = 0;
            }
        } else if (state_ == kValue) {
            if (ch < 0 || ch == '\r' || ch == '\n' || ch == '<') {
                value_str.trim();
                String o = replaceString(opcode_name, define_names_, define_values_);
                String v = replaceString(value_str, define_names_, define_values_);
                if (o != "" && v != "") {
                    handler->opcode(o, v);
                }
                state_ = kReady;
                next_ch = ch;
            } else if (ch == '#') {
                if (isSpace(prev_ch)) {
                    value_str.trim();
                    String o = replaceString(opcode_name, define_names_, define_values_);
                    String v = replaceString(value_str, define_names_, define_values_);
                    if (o != "" && v != "") {
                        handler->opcode(o, v);
                    }
                    state_ = kReady;
                    next_ch = ch;
                } else {
                    value_str += (char)ch;
                    if (prev_ch == ' ' && ch != ' ') {
                        value_tokens++;
                        value_last_token_pos = value_str.length() - 1;
                    }
                }
            } else if (ch == '=') {
                if (value_tokens >= 2) {
                    String next_opcode = value_str.substring(value_last_token_pos);
                    next_opcode.trim();
                    value_str = value_str.substring(0, value_last_token_pos);
                    value_str.trim();
                    String o = replaceString(opcode_name, define_names_, define_values_);
                    String v = replaceString(value_str, define_names_, define_values_);
                    handler->opcode(o, v);
                    opcode_name = next_opcode;
                    state_ = kWaitValue;
                } else {
                    value_str += (char)ch;
                }
            } else if (ch == ' ' || isGraphX(ch)) {
                value_str += (char)ch;
                if (prev_ch == ' ' && ch != ' ') {
                    value_tokens++;
                    value_last_token_pos = value_str.length() - 1;
                }
            }
        }
        prev_ch = ch;
        if (ch < 0) {
            break;
        }
    }

    if (parsing_root) {
        if (header_name_ != "") {
            handler->endHeader(header_name_);
        }
        handler->endSfz();
    }
    parsing_root_ = parsing_root;
}

#endif  // ARDUINO_ARCH_SPRESENSE
