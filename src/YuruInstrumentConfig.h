/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef YURUINSTRUMENTCONFIG_H_
#define YURUINSTRUMENTCONFIG_H_

#include "YuruInstrumentFilter.h"

class YuruInstrumentConfig {
public:
    enum ErrorCode { kNoError = 0, kErrPerm, kErrNoEnt, kErrAccess, kErrExist, kErrNoDev, kErrNotDir, kErrIsDir, kErrInval };

    YuruInstrumentConfig(Filter *filter);
    YuruInstrumentConfig(Filter &filter);
    ~YuruInstrumentConfig();

    Filter *getFilter();

    int printParamList();
    int printAvailable(const char *param_str);
    int printParam(const char *param_str);
    int setParam(const char *param_str, const char *value_str);
    int loadFromFile(const char *path);
    int saveToFile(const char *path);

private:
    Filter *filter_;
};

#endif  // YURUINSTRUMENTCONFIG_H_
