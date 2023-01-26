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

SFZParser::SFZParser() : parsing_root_(true), define_names_(), define_values_() {
}

SFZParser::~SFZParser() {
}

static int isGraphX(int c) {
    return (c >= 0 && (isGraph(c) || (c & 0x80)));
}

void SFZParser::parse(File sfz_file, const String& sfz_file_path, SFZHandler* handler) {
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

    State state = kReady;
    bool skip_line = false;
    String macro_name;
    String header_name;

    String include_path;
    String define_name;
    String define_value;
    String opcode_name;
    String value_str;
    int value_tokens = 0;
    int value_last_token_pos = -1;

    bool stash = parsing_root_;
    parsing_root_ = false;
    if (stash) {
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
        trace_printf("[%s::%s] state=%d ch=0x%02X(%c)\n", kClassName, __func__, state, ch, (ch < 0 || ch == '\r' || ch == '\n') ? ' ' : ch);
        if (state == kReady) {
            if (ch == '#') {
                state = kExpectMacro;
                macro_name = "";
            } else if (ch == '<') {
                if (header_name != "") {
                    handler->endHeader(header_name);
                }
                state = kExpectHeaderName;
                header_name = "";
            } else if (ch == '=') {
                state = kWaitValue;
            } else if (isGraphX(ch)) {
                state = kOpcode;
                opcode_name = String((char)ch);
            }
        } else if (state == kExpectMacro) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state = kReady;
            } else if (isGraphX(ch)) {
                state = kMacro;
                macro_name = String((char)ch);
            }
        } else if (state == kMacro) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state = kReady;
            } else if (isWhitespace(ch)) {
                if (macro_name == "define") {
                    state = kExpectDefineName;
                    define_name = "";
                } else if (macro_name == "include") {
                    state = kExpectIncludePath;
                    include_path = "";
                } else {
                    state = kReady;
                }
            } else {
                macro_name += (char)ch;
            }
        } else if (state == kExpectDefineName) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state = kReady;
            } else if (ch == '$') {
                state = kDefineName;
                define_name = String((char)ch);
            }
        } else if (state == kDefineName) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state = kReady;
            } else if (isGraphX(ch)) {
                define_name += (char)ch;
            } else {
                state = kExpectDefineValue;
                define_value = "";
            }
        } else if (state == kExpectDefineValue) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state = kReady;
            } else if (isGraphX(ch)) {
                state = kDefineValue;
                define_value = String((char)ch);
            }
        } else if (state == kDefineValue) {
            if (ch < 0 || ch == '\r' || ch == '\n' || isWhitespace(ch)) {
                state = kReady;
                String n = replaceString(define_name, define_names_, define_values_);
                String v = replaceString(define_value, define_names_, define_values_);
                define_names_.push_back(n);
                define_values_.push_back(v);
            } else if (isGraphX(ch)) {
                define_value += (char)ch;
            }
        } else if (state == kExpectIncludePath) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state = kReady;
            } else if (ch == '"') {
                state = kIncludePath;
                include_path = "";
            }
        } else if (state == kIncludePath) {
            if (ch < 0 || ch == '\r' || ch == '\n') {
                state = kReady;
            } else if (ch == '"') {
                String path = joinPath(getFolderPath(sfz_file_path), include_path);
                File include_file = File(path.c_str());
                if (include_file) {
                    debug_printf("[%s::%s] parsing '%s'\n", kClassName, __func__, path.c_str());
                    parse(include_file, sfz_file_path, handler);
                    include_file.close();
                } else {
                    error_printf("[%s::%s] error: cannot open '%s'\n", kClassName, __func__, path.c_str());
                }
                state = kReady;
            } else if (ch == '\\') {
                state = kIncludeEsc;
            } else {
                include_path += (char)ch;
            }
        } else if (state == kIncludeEsc) {
            if (ch >= 0) {
                include_path += (char)ch;
            }
            state = kIncludePath;
        } else if (state == kExpectHeaderName) {
            if (ch < 0) {
                state = kReady;
            } else if (isGraphX(ch)) {
                state = kHeaderName;
                header_name = String((char)ch);
            }
        } else if (state == kHeaderName) {
            if (ch < 0) {
                state = kReady;
            } else if (ch == '<') {
                state = kExpectHeaderName;
            } else if (ch == '>') {
                header_name.trim();
                handler->startHeader(header_name);
                state = kReady;
            } else if (isGraphX(ch)) {
                header_name += (char)ch;
            }
        } else if (state == kOpcode) {
            if (ch < 0) {
                state = kReady;
            } else if (ch == '<') {
                state = kExpectHeaderName;
            } else if (ch == '=') {
                state = kWaitValue;
            } else if (isWhitespace(ch)) {
                state = kWaitEqual;
            } else if (isGraphX(ch)) {
                opcode_name += (char)ch;
            }
        } else if (state == kWaitEqual) {
            if (ch < 0) {
                state = kReady;
            } else if (ch == '<') {
                state = kExpectHeaderName;
            } else if (ch == '=') {
                state = kWaitValue;
            } else if (isGraphX(ch)) {
                state = kReady;
                next_ch = ch;
            }
        } else if (state == kWaitValue) {
            if (ch < 0) {
                state = kReady;
            } else if (ch == '<') {
                state = kExpectHeaderName;
            } else if (isGraphX(ch)) {
                state = kValue;
                value_str = String((char)ch);
                value_tokens = 1;
                value_last_token_pos = 0;
            }
        } else if (state == kValue) {
            if (ch < 0 || ch == '\r' || ch == '\n' || ch == '<') {
                value_str.trim();
                String o = replaceString(opcode_name, define_names_, define_values_);
                String v = replaceString(value_str, define_names_, define_values_);
                if (o != "" && v != "") {
                    handler->opcode(o, v);
                }
                state = kReady;
                next_ch = ch;
            } else if (ch == '#') {
                if (isSpace(prev_ch)) {
                    value_str.trim();
                    String o = replaceString(opcode_name, define_names_, define_values_);
                    String v = replaceString(value_str, define_names_, define_values_);
                    if (o != "" && v != "") {
                        handler->opcode(o, v);
                    }
                    state = kReady;
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
                    state = kWaitValue;
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

    if (header_name != "") {
        handler->endHeader(header_name);
    }
    parsing_root_ = stash;
    if (parsing_root_) {
        handler->endSfz();
    }
}

#endif  // ARDUINO_ARCH_SPRESENSE
