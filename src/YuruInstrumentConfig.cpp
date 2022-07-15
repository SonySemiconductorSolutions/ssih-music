/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "YuruInstrumentConfig.h"

#include <Arduino.h>

#include <SDHCI.h>

#include "OctaveShift.h"
#include "ScoreSrc.h"
#include "ToneFilter.h"
#include "YuruhornSrc.h"
#include "YuruInstrumentFilter.h"

//#define DEBUG

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

enum ErrorCode { kNoError = 0, kErrPerm, kErrNoEnt, kErrAccess, kErrExist, kErrNoDev, kErrNotDir, kErrIsDir, kErrInval };

const int kMaxLineLength = 64;
const int kBaudrate = 115200;  //< 115200bps
const char kSectionName[] = "yurugakki";

enum ParamType { kIntegerValue, kIntegerValueReadOnly, kStringValue, kStringValueReadOnly };

struct ParamSpec {
    const char *name;
    int id;
    ParamType type;
};

struct CommandSpec {
    const char *name;
    int (*func)(YuruInstrumentConfig *config, int argc, char *argv[]);
};

// clang-format off
static const ParamSpec g_param_spec[] = {
    {"mic_gain",       VoiceCapture::PARAMID_MIC_GAIN,          kIntegerValue        },
    {"input_level",    VoiceCapture::PARAMID_INPUT_LEVEL,       kIntegerValue        },
    {"rec",            VoiceCapture::PARAMID_RECORDING,         kIntegerValueReadOnly},
    {"active_level",   YuruhornSrc::PARAMID_ACTIVE_LEVEL,       kIntegerValue        },
    {"play_button",    YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE, kIntegerValue        },
    {"play_scale",     YuruhornSrc::PARAMID_SCALE,              kIntegerValue        },
    {"min_note",       YuruhornSrc::PARAMID_MIN_NOTE,           kIntegerValue        },
    {"max_note",       YuruhornSrc::PARAMID_MAX_NOTE,           kIntegerValue        },
    {"correct_frame",  YuruhornSrc::PARAMID_CORRECT_FRAMES,     kIntegerValue        },
    {"suppress_frame", YuruhornSrc::PARAMID_SUPPRESS_FRAMES,    kIntegerValue        },
    {"keep_frame",     YuruhornSrc::PARAMID_KEEP_FRAMES,        kIntegerValue        },
    {"volume_meter",   YuruhornSrc::PARAMID_VOLUME_METER,       kIntegerValueReadOnly},
    {"monitoring",     YuruhornSrc::PARAMID_MONITOR_ENABLE,     kIntegerValue        },
    {"score_num",      ScoreSrc::PARAMID_NUMBER_OF_SCORES,      kIntegerValueReadOnly},
    {"playing_score",  ScoreSrc::PARAMID_PLAYING_SCORE,         kIntegerValueReadOnly},
    {"score_name",     ScoreSrc::PARAMID_PLAYING_SCORE_NAME,    kStringValueReadOnly },
    {"octave_shift",   OctaveShift::PARAMID_OCTAVE_SHIFT,       kIntegerValue        },
    {"tone",           ToneFilter::PARAMID_TONE,                kIntegerValue        },
    {"volume",         Filter::PARAMID_OUTPUT_LEVEL,            kIntegerValue        }
};
// clang-format on

static const ParamSpec *FindParamSpecByName(const char *param_name) {
    const int kParamSpecNum = sizeof(g_param_spec) / sizeof(g_param_spec[0]);
    for (int i = 0; i < kParamSpecNum; i++) {
        if (strcmp(g_param_spec[i].name, param_name) == 0) {
            return &g_param_spec[i];
        }
    }
    return nullptr;
}

static const ParamSpec *FindParamSpecById(int param_id) {
    const int kParamSpecNum = sizeof(g_param_spec) / sizeof(g_param_spec[0]);
    for (int i = 0; i < kParamSpecNum; i++) {
        if (param_id == g_param_spec[i].id) {
            return &g_param_spec[i];
        }
    }
    return nullptr;
}

static int ApplyParam(YuruInstrumentConfig *config, const String &section, const String &name, const String &value) {
    if (config == nullptr) {
        return -kErrInval;
    }
    Filter *filter = config->getFilter();
    if (filter == nullptr) {
        return -kErrInval;
    }

    if (section == kSectionName) {
        char *endp = nullptr;

        const ParamSpec *param_spec = FindParamSpecByName(name.c_str());
        if (param_spec == nullptr) {
            int param_id = strtol(name.c_str(), &endp, 0);
            if (endp == nullptr || *endp != '\0') {
                return -kErrInval;
            }
            param_spec = FindParamSpecById(param_id);
        }
        if (param_spec == nullptr) {
            return -kErrInval;
        }

        intptr_t param_value = strtol(value.c_str(), &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("parse error: parse '%s' as integer\n", value.c_str());
            return -kErrInval;
        }

        bool is_available = filter->isAvailable(param_spec->id);
        if (is_available) {
            filter->setParam(param_spec->id, param_value);
            printf("setParam[%d] = %d\n", param_spec->id, (int)param_value);
            return 0;
        } else {
            printf("isAvailable[%d] = %d\n", param_spec->id, is_available);
        }
    }
    return -kErrInval;
}

YuruInstrumentConfig::YuruInstrumentConfig(Filter *filter) : filter_(filter) {
}

YuruInstrumentConfig::YuruInstrumentConfig(Filter &filter) : filter_(&filter) {
}

YuruInstrumentConfig::~YuruInstrumentConfig() {
}

Filter *YuruInstrumentConfig::getFilter() {
    return filter_;
}

int YuruInstrumentConfig::printParamList() {
    const int kParamSpecNum = sizeof(g_param_spec) / sizeof(g_param_spec[0]);
    for (int i = 0; i < kParamSpecNum; i++) {
        printf("%d: %s\n", g_param_spec[i].id, g_param_spec[i].name);
    }
    return 0;
}

int YuruInstrumentConfig::printAvailable(const char *param_str) {
    if (filter_) {
        char *endp = nullptr;
        int param_id = -1;
        const ParamSpec *param_spec = FindParamSpecByName(param_str);
        if (param_spec) {
            param_id = param_spec->id;
        } else {
            param_id = strtol(param_str, &endp, 0);
            if (endp == nullptr || *endp != '\0') {
                error_printf("parse error: parse '%s' as integer\n", param_str);
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
        const ParamSpec *param_spec = FindParamSpecByName(param_str);
        if (param_spec) {
            param_id = param_spec->id;
        } else {
            param_id = strtol(param_str, &endp, 0);
            if (endp == nullptr || *endp != '\0') {
                error_printf("parse error: parse '%s' as integer\n", param_str);
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

int YuruInstrumentConfig::setParam(const char *param_str, const char *value_str) {
    if (filter_) {
        char *endp = nullptr;
        int param_id = -1;
        const ParamSpec *param_spec = FindParamSpecByName(param_str);
        if (param_spec) {
            param_id = param_spec->id;
        } else {
            param_id = strtol(param_str, &endp, 0);
            if (endp == nullptr || *endp != '\0') {
                error_printf("parse error: parse '%s' as integer\n", param_str);
                return -kErrInval;
            }
        }
        intptr_t value = strtol(value_str, &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("parse error: parse '%s' as integer\n", value_str);
            return -kErrInval;
        }
        bool is_available = filter_->isAvailable(param_id);
        if (is_available) {
            filter_->setParam(param_id, value);
            printf("setParam[%d] = %d\n", param_id, (int)value);
            return 0;
        } else {
            printf("isAvailable[%d] = %d\n", param_id, is_available);
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
            ApplyParam(this, section, name, value);
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
    const int kParamSpecNum = sizeof(g_param_spec) / sizeof(g_param_spec[0]);
    for (int i = 0; i < kParamSpecNum; i++) {
        const ParamSpec &param = g_param_spec[i];
        if (filter_ && filter_->isAvailable(param.id)) {
            if (param.type == kIntegerValue) {
                file.print(param.name);
                file.print('=');
                file.println((long)filter_->getParam(param.id));
            } else if (param.type == kStringValue) {
                file.print(param.name);
                file.print('=');
                file.println((const char *)filter_->getParam(param.id));
            }
        }
    }
    file.close();
    return 0;
}

#endif  // ARDUINO_ARCH_SPRESENSE
