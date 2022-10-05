/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef YURUINSTRUMENTCONSOLE_H_
#define YURUINSTRUMENTCONSOLE_H_

#include "YuruInstrumentConfig.h"

class YuruInstrumentConsole : public YuruInstrumentConfig {
public:
    YuruInstrumentConsole(Filter &filter);
    ~YuruInstrumentConsole();

    void begin();
    void poll();

    void execute(char *line, int length);

private:
    size_t line_length_;
    char *line_buffer_;
    size_t line_index_;
};

#endif  // YURUINSTRUMENTCONSOLE_H_
