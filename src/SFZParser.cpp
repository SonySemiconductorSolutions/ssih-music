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

#include <MemoryUtil.h>
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

void SFZParser::parse(File sfz_file, const String& sfz_file_path, SFZHandler* handler) {
    enum State {
        kReady,
        kSlash,
        kSkip,
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
    String macro;
    String include_path;
    String define_name;
    String define_value;
    String header_name;
    String opcode;
    String value;

    bool stash = parsing_root_;
    parsing_root_ = false;
    if (stash) {
        handler->startSfz();
    }
    while (true) {
        int ch = sfz_file.read();
        if (state == kReady) {
            if (ch == '/') {
                state = kSlash;
            } else if (ch == '#') {
                macro = "";
                state = kExpectMacro;
            } else if (ch == '<') {
                header_name = "";
                state = kExpectHeaderName;
            } else if (isGraph(ch)) {
                opcode = String((char)ch);
                state = kOpcode;
            }
        } else if (state == kSlash) {
            if (ch == '/') {
                state = kSkip;
            }
        } else if (state == kSkip) {
            if (ch == '\r' || ch == '\n') {
                state = kReady;
            }
        } else if (state == kExpectMacro) {
            if (isPrintable(ch)) {
                macro = String((char)ch);
                state = kMacro;
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
                    state = kSkip;
                }
            } else {
                macro += (char)ch;
            }
        } else if (state == kExpectIncludePath) {
            if (ch == '"') {
                include_path = "";
                state = kIncludePath;
            }
        } else if (state == kIncludePath) {
            if (ch == '"') {
                SDClass sdcard;
                if (sdcard.begin()) {
                    String path = getFolderPath(sfz_file_path) + include_path;
                    File include_file = sdcard.open(path.c_str());
                    if (include_file) {
                        debug_printf("parsing '%s'\n", path.c_str());
                        parse(include_file, sfz_file_path, handler);
                        include_file.close();
                    } else {
                        error_printf("error: cannot open '%s'\n", path.c_str());
                    }
                }
                state = kSkip;
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
        } else if (state == kExpectDefineName) {
            if (ch == '$') {
                define_name = String((char)ch);
                state = kDefineName;
            }
        } else if (state == kDefineName) {
            if (isGraph(ch)) {
                define_name += (char)ch;
            } else {
                define_value = "";
                state = kExpectDefineValue;
            }
        } else if (state == kExpectDefineValue) {
            if (isGraph(ch)) {
                define_value = String((char)ch);
                state = kDefineValue;
            }
        } else if (state == kDefineValue) {
            if (isGraph(ch)) {
                define_value += (char)ch;
            } else {
                String n = replaceString(define_name, define_names_, define_values_);
                String v = replaceString(define_value, define_names_, define_values_);
                define_names_.push_back(n);
                define_values_.push_back(v);
                state = kSkip;
            }
        } else if (state == kExpectHeaderName) {
            if (isPrintable(ch)) {
                header_name = String((char)ch);
                state = kHeaderName;
            }
        } else if (state == kHeaderName) {
            if (ch == '>') {
                handler->header(header_name);
                state = kReady;
            } else if (isGraph(ch)) {
                header_name += (char)ch;
            }
        } else if (state == kOpcode) {
            if (isWhitespace(ch)) {
                state = kWaitEqual;
            } else if (ch == '=') {
                state = kWaitValue;
            } else if (isGraph(ch)) {
                opcode += (char)ch;
            }
        } else if (state == kWaitEqual) {
            if (ch == '=') {
                state = kWaitValue;
            }
        } else if (state == kWaitValue) {
            if (!isWhitespace(ch)) {
                value = String((char)ch);
                state = kValue;
            }
        } else if (state == kValue) {
            if (ch == '=') {
                value.trim();
                int space_pos = -1;
                for (size_t i = value.length(); i > 0; i--) {
                    if (isWhitespace(value[i - 1])) {
                        space_pos = i - 1;
                        break;
                    }
                }
                if (space_pos >= 0) {
                    String next_opcode = value.substring(space_pos + 1);
                    value = value.substring(0, space_pos);
                    value.trim();
                    String o = replaceString(opcode, define_names_, define_values_);
                    String v = replaceString(value, define_names_, define_values_);
                    handler->opcode(o, v);
                    opcode = next_opcode;
                    state = kWaitValue;
                } else {
                    value.trim();
                    String o = replaceString(opcode, define_names_, define_values_);
                    String v = replaceString(value, define_names_, define_values_);
                    handler->opcode(o, v);
                    opcode = value;
                    state = kWaitValue;
                }
            } else if (ch == '<') {
                value.trim();
                String o = replaceString(opcode, define_names_, define_values_);
                String v = replaceString(value, define_names_, define_values_);
                handler->opcode(o, v);
                header_name = "";
                state = kExpectHeaderName;
            } else if (ch < 0 || ch == '\r' || ch == '\n') {
                String o = replaceString(opcode, define_names_, define_values_);
                String v = replaceString(value, define_names_, define_values_);
                handler->opcode(o, v);
                state = kReady;
            } else {
                value += (char)ch;
            }
        }

        if (ch < 0) {
            break;
        } else if (ch == '\r' || ch == '\n') {
            state = kReady;
        }
    }

    parsing_root_ = stash;
    if (parsing_root_) {
        handler->endSfz();
    }
}
#endif  // ARDUINO_ARCH_SPRESENSE
