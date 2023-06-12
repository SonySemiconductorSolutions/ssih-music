/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "YuruInstrumentConfig.h"

#include <Arduino.h>

#include <File.h>
#include <SDHCI.h>

#include "YuruInstrumentFilter.h"

// #define DEBUG (1)

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#ifdef DEBUG
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else  // DEBUG
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // DEBUG

static const char kClassName[] = "YuruInstrumentConfig";

const int kMaxLineLength = 64;
const int kBaudrate = 115200;  //< 115200bps
const char kSectionName[] = "yurugakki";

YuruInstrumentConfig::YuruInstrumentConfig(Filter &filter) : filter_(&filter) {
}

YuruInstrumentConfig::~YuruInstrumentConfig() {
}

Filter *YuruInstrumentConfig::getFilter() {
    return filter_;
}

int YuruInstrumentConfig::printParamList() {
    for (const auto &e : param_specs_) {
        printf("%d: %s\n", e.id, e.name.c_str());
    }
    return 0;
}

int YuruInstrumentConfig::printAvailable(const char *param_str) {
    if (filter_) {
        char *endp = nullptr;
        int param_id = -1;
        const YuruInstrumentConfig::ParamSpec *param_spec = nullptr;
        for (const auto &e : param_specs_) {
            if (e.name == param_str) {
                param_spec = &e;
                break;
            }
        }
        if (param_spec) {
            param_id = param_spec->id;
        } else {
            param_id = strtol(param_str, &endp, 0);
            if (endp == nullptr || *endp != '\0') {
                error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, param_str);
                return -kErrInval;
            }
        }
        printf("isAvailable[%d] => %d\n", param_id, filter_->isAvailable(param_id));
        return 0;
    }
    return 0;
}

int YuruInstrumentConfig::printParam(const char *param_str) {
    if (filter_) {
        char *endp = nullptr;
        int param_id = -1;
        const YuruInstrumentConfig::ParamSpec *param_spec = nullptr;
        for (const auto &e : param_specs_) {
            if (e.name == param_str) {
                param_spec = &e;
                break;
            }
        }
        if (param_spec) {
            param_id = param_spec->id;
        } else {
            param_id = strtol(param_str, &endp, 0);
            if (endp == nullptr || *endp != '\0') {
                error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, param_str);
                return -kErrInval;
            }
        }
        bool is_available = filter_->isAvailable(param_id);
        if (is_available) {
            intptr_t value = filter_->getParam(param_id);
            printf("getParam[%d] => %d\n", param_id, (int)value);
            return 0;
        } else {
            printf("isAvailable[%d] = %d\n", param_id, is_available);
        }
    }
    return -kErrInval;
}

int YuruInstrumentConfig::registerParam(const char *name, int id, bool readonly) {
    YuruInstrumentConfig::ParamSpec spec;
    spec.name = name;
    spec.id = id;
    spec.type = readonly ? YuruInstrumentConfig::kIntegerValueReadOnly : YuruInstrumentConfig::kIntegerValue;
    param_specs_.push_back(spec);
    return 0;
}

int YuruInstrumentConfig::registerStringParam(const char *name, int id, bool readonly) {
    YuruInstrumentConfig::ParamSpec spec;
    spec.name = name;
    spec.id = id;
    spec.type = readonly ? YuruInstrumentConfig::kStringValueReadOnly : YuruInstrumentConfig::kStringValue;
    param_specs_.push_back(spec);
    return 0;
}

int YuruInstrumentConfig::setParam(const char *param_str, const char *value_str) {
    if (filter_) {
        char *endp = nullptr;
        int param_id = -1;
        const YuruInstrumentConfig::ParamSpec *param_spec = nullptr;
        for (const auto &e : param_specs_) {
            if (e.name == param_str) {
                param_spec = &e;
                break;
            }
        }
        if (param_spec) {
            param_id = param_spec->id;
        } else {
            param_id = strtol(param_str, &endp, 0);
            if (endp == nullptr || *endp != '\0') {
                error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, param_str);
                return -kErrInval;
            }
        }
        intptr_t value = strtol(value_str, &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, value_str);
            return -kErrInval;
        }
        bool is_available = filter_->isAvailable(param_id);
        if (is_available) {
            filter_->setParam(param_id, value);
            debug_printf("setParam[%d] = %d\n", param_id, (int)value);
            return 0;
        } else {
            error_printf("isAvailable[%d] = %d\n", param_id, is_available);
        }
    }
    return -kErrInval;
}

int YuruInstrumentConfig::loadFromFile(const char *path) {
    SDClass sdcard = SDClass();
    if (!sdcard.begin()) {
        return -kErrNoDev;
    }
    File file = sdcard.open(path, FILE_READ);
    if (!file) {
        return -kErrNoEnt;
    }
    file.setTimeout(0);

    String section = "";
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        unsigned int length = line.length();
        if (length == 0) {
            continue;
        }
        int pos = -1;
        if (line[0] == '[') {
            int rblacket_pos = line.lastIndexOf(']');
            if (rblacket_pos >= 0) {
                // ex. `[section]`
                section = line.substring(1, rblacket_pos);
            } else {
                section = "";
            }
        } else if (line[0] == ';' || line[0] == '#') {
            // ex. `; comment`, `# comment`
            continue;
        } else if ((pos = line.indexOf('=')) >= 0) {
            // ex. `name=value`
            String name = line.substring(0, pos);
            String value = line.substring(pos + 1);
            name.trim();
            value.trim();
            if (section != kSectionName) {
                continue;
            }

            const YuruInstrumentConfig::ParamSpec *param_spec = nullptr;
            for (const auto &e : param_specs_) {
                if (e.name == name) {
                    param_spec = &e;
                    break;
                }
            }
            if (param_spec == nullptr) {
                char *endp = nullptr;
                int param_id = strtol(name.c_str(), &endp, 0);
                if (endp == nullptr || *endp != '\0') {
                    continue;
                }
                for (const auto &e : param_specs_) {
                    if (e.id == param_id) {
                        param_spec = &e;
                        break;
                    }
                }
            }
            if (param_spec) {
                char *endp = nullptr;
                intptr_t param_value = strtol(value.c_str(), &endp, 0);
                if (endp == nullptr || *endp != '\0') {
                    continue;
                }

                bool is_available = filter_->isAvailable(param_spec->id);
                if (is_available) {
                    filter_->setParam(param_spec->id, param_value);
                    printf("setParam[%d] = %d\n", param_spec->id, (int)param_value);
                } else {
                    printf("isAvailable[%d] = %d\n", param_spec->id, is_available);
                }
            }
        }
    }
    return 0;
}

int YuruInstrumentConfig::saveToFile(const char *path) {
    SDClass sdcard;
    if (!sdcard.begin()) {
        return -kErrNoDev;
    }
    File file = sdcard.open(path, FILE_WRITE);
    if (!file) {
        return -kErrNoEnt;
    }

    file.seek(0);
    file.print("[");
    file.print(kSectionName);
    file.println("]");
    for (const auto &e : param_specs_) {
        if (filter_ && filter_->isAvailable(e.id)) {
            if (e.type == kIntegerValue) {
                file.print(e.name);
                file.print('=');
                file.println((long)filter_->getParam(e.id));
            } else if (e.type == kStringValue) {
                file.print(e.name);
                file.print('=');
                file.println((const char *)filter_->getParam(e.id));
            }
        }
    }
    file.close();
    return 0;
}

#endif  // ARDUINO_ARCH_SPRESENSE
