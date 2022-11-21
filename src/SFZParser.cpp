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
        kExpectIncludePath,
        kIncludePath,
        kIncludeEsc,
        kExpectDefineName,
        kDefineName,
        kExpectDefineValue,
        kDefineValue,
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
    String macro;
    String include_path;
    String define_name;
    String define_value;
    String header_name;
    String opcode;
    String value;
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
        if (skip_line) {
            if (ch == '\r' || ch == '\n') {
                skip_line = false;
            }
            continue;
        }
        if (ch == '/') {
            next_ch = sfz_file.read();
            if (next_ch == '/') {
                skip_line = true;
                next_ch = -1;
                continue;
            }
        }
        trace_printf("[%s::%s] state=%d ch=0x%02X(%c)\n", kClassName, __func__, state, ch, (ch < 0 || ch == '\r' || ch == '\n') ? ' ' : ch);
        if (state == kReady) {
            if (ch == '#') {
                macro = "";
                state = kExpectMacro;
            } else if (ch == '<') {
                if (header_name != "") {
                    handler->endHeader(header_name);
                }
                header_name = "";
                state = kExpectHeaderName;
            } else if (isGraphX(ch)) {
                opcode = String((char)ch);
                state = kOpcode;
            }
        } else if (state == kExpectMacro) {
            if (isGraphX(ch)) {
                macro = String((char)ch);
                state = kMacro;
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                next_ch = ch;
                skip_line = true;
                state = kReady;
            }
        } else if (state == kMacro) {
            if (isWhitespace(ch)) {
                if (macro == "include") {
                    include_path = "";
                    state = kExpectIncludePath;
                } else if (macro == "define") {
                    define_name = "";
                    state = kExpectDefineName;
                } else {
                    skip_line = true;
                    state = kReady;
                }
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                next_ch = ch;
                skip_line = true;
                state = kReady;
            } else {
                macro += (char)ch;
            }
        } else if (state == kExpectIncludePath) {
            if (ch == '"') {
                include_path = "";
                state = kIncludePath;
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                next_ch = ch;
                skip_line = true;
                state = kReady;
            }
        } else if (state == kIncludePath) {
            if (ch == '"') {
                String path = getFolderPath(sfz_file_path) + include_path;
                File include_file = File(path.c_str());
                if (include_file) {
                    debug_printf("[%s::%s] parsing '%s'\n", kClassName, __func__, path.c_str());
                    parse(include_file, sfz_file_path, handler);
                    include_file.close();
                } else {
                    error_printf("[%s::%s] error: cannot open '%s'\n", kClassName, __func__, path.c_str());
                }
                skip_line = true;
                state = kReady;
            } else if (ch == '\\') {
                state = kIncludeEsc;
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                next_ch = ch;
                skip_line = true;
                state = kReady;
            } else {
                include_path += (char)ch;
            }
        } else if (state == kIncludeEsc) {
            if (ch >= 0) {
                include_path += (char)ch;
            }
            state = kIncludePath;
        } else if (state == kExpectDefineName) {
            if (ch == '$') {
                define_name = String((char)ch);
                state = kDefineName;
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                next_ch = ch;
                skip_line = true;
                state = kReady;
            }
        } else if (state == kDefineName) {
            if (isGraphX(ch)) {
                define_name += (char)ch;
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                next_ch = ch;
                skip_line = true;
                state = kReady;
            } else {
                define_value = "";
                state = kExpectDefineValue;
            }
        } else if (state == kExpectDefineValue) {
            if (isGraphX(ch)) {
                define_value = String((char)ch);
                state = kDefineValue;
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                next_ch = ch;
                skip_line = true;
                state = kReady;
            }
        } else if (state == kDefineValue) {
            if (isGraphX(ch)) {
                define_value += (char)ch;
            } else {
                String n = replaceString(define_name, define_names_, define_values_);
                String v = replaceString(define_value, define_names_, define_values_);
                define_names_.push_back(n);
                define_values_.push_back(v);
                next_ch = ch;
                skip_line = true;
                state = kReady;
            }
        } else if (state == kExpectHeaderName) {
            if (isGraphX(ch)) {
                header_name = String((char)ch);
                state = kHeaderName;
            }
        } else if (state == kHeaderName) {
            if (ch == '>') {
                handler->startHeader(header_name);
                state = kReady;
            } else if (ch == '<') {
                header_name = "";
                state = kExpectHeaderName;
            } else if (isGraphX(ch)) {
                header_name += (char)ch;
            }
        } else if (state == kOpcode) {
            if (isWhitespace(ch)) {
                state = kWaitEqual;
            } else if (ch == '=') {
                state = kWaitValue;
            } else if (isGraphX(ch)) {
                opcode += (char)ch;
            }
        } else if (state == kWaitEqual) {
            if (isGraphX(ch)) {
                if (ch == '=') {
                    state = kWaitValue;
                } else {
                    next_ch = ch;
                    state = kReady;
                }
            }
        } else if (state == kWaitValue) {
            if (isGraphX(ch)) {
                value = String((char)ch);
                value_tokens = 1;
                value_last_token_pos = 0;
                state = kValue;
            }
        } else if (state == kValue) {
            if (ch == '=') {
                if (value_tokens >= 2) {
                    String next_opcode = value.substring(value_last_token_pos);
                    next_opcode.trim();
                    value = value.substring(0, value_last_token_pos - 1);
                    value.trim();
                    String o = replaceString(opcode, define_names_, define_values_);
                    String v = replaceString(value, define_names_, define_values_);
                    handler->opcode(o, v);
                    opcode = next_opcode;
                    state = kWaitValue;
                } else {
                    value += (char)ch;
                }
            } else if (ch == '<') {
                value.trim();
                String o = replaceString(opcode, define_names_, define_values_);
                String v = replaceString(value, define_names_, define_values_);
                handler->opcode(o, v);
                if (header_name != "") {
                    handler->endHeader(header_name);
                }
                header_name = "";
                state = kExpectHeaderName;
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                value.trim();
                String o = replaceString(opcode, define_names_, define_values_);
                String v = replaceString(value, define_names_, define_values_);
                handler->opcode(o, v);
                state = kReady;
            } else if (isGraphX(ch)) {
                value += (char)ch;
                if (prev_ch == ' ' && ch != ' ') {
                    value_tokens++;
                    value_last_token_pos = value.length() - 1;
                }
            } else if (ch == ' ') {
                value += (char)ch;
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
