/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef YURUINSTRUMENTCONSOLE_H_
#define YURUINSTRUMENTCONSOLE_H_

#include "YuruInstrumentFilter.h"

class YuruInstrumentConsole {
public:
    YuruInstrumentConsole(Filter *filter);
    YuruInstrumentConsole(Filter &filter);
    ~YuruInstrumentConsole();

    Filter *getFilter();

    void begin();
    void poll();

    void execute(char *line, int length);

private:
    Filter *filter_;
    size_t line_length_;
    char *line_buffer_;
    size_t line_index_;
};

#endif  // YURUINSTRUMENTCONSOLE_H_
