/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SFZ_PARSER_H_
#define SFZ_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "path_util.h"

class SFZHandler {
public:
    virtual void startSfz() = 0;
    virtual void endSfz() = 0;
    virtual void startHeader(const String& header) = 0;
    virtual void endHeader(const String& header) = 0;
    virtual void opcode(const String& opcode, const String& value) = 0;
};

class SFZParser {
public:
    SFZParser();
    ~SFZParser();
    void parse(File sfz_file, const String& sfz_file_path, SFZHandler* handler);

private:
    bool parsing_root_;
    std::vector<String> define_names_;
    std::vector<String> define_values_;
};

#endif  // SFZ_PARSER_H_
