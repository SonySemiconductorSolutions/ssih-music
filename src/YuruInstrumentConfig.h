/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef YURU_INSTRUMENT_CONFIG_H_
#define YURU_INSTRUMENT_CONFIG_H_

#include <vector>

#include <Arduino.h>

#include "YuruInstrumentFilter.h"

class YuruInstrumentConfig {
public:
    enum ErrorCode { kNoError = 0, kErrPerm, kErrNoEnt, kErrAccess, kErrExist, kErrNoDev, kErrNotDir, kErrIsDir, kErrInval };
    enum ParamType { kIntegerValue, kIntegerValueReadOnly, kStringValue, kStringValueReadOnly };
    struct ParamSpec {
        String name;
        int id;
        ParamType type;
    };

    YuruInstrumentConfig(Filter &filter);
    ~YuruInstrumentConfig();

    Filter *getFilter();

    int printParamList();
    int printAvailable(const char *param_str);
    int printParam(const char *param_str);
    int registerParam(const char *name, int id, bool readonly = false);
    int registerStringParam(const char *name, int id, bool readonly = false);
    int setParam(const char *param_str, const char *value_str);
    int loadFromFile(const char *path);
    int saveToFile(const char *path);

private:
    Filter *filter_;
    std::vector<ParamSpec> param_specs_;
};

#endif  // YURU_INSTRUMENT_CONFIG_H_
