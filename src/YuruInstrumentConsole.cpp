/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "YuruInstrumentConsole.h"

#include <Arduino.h>

#include <File.h>
#include <SDHCI.h>

#include "YuruInstrumentConfig.h"
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

static const char kClassName[] = "YuruInstrumentConsole";

const int kMaxLineLength = 64;
const int kBaudrate = 115200;  //< 115200bps

struct CommandSpec {
    const char *name;
    int (*func)(YuruInstrumentConsole *console, int argc, char *argv[]);
};

static void PrintLsItem(File &file) {
#ifdef EMMC_USE
    const char root[] = "/mnt/emmc/";
#else
    const char root[] = "/mnt/sd0/";
#endif
    const size_t root_length = strlen(root);
    if (file) {
        // drop prefix if starts with "/mnt/sd0/"
        const char *path = file.name();
        if (strncmp(path, root, root_length) == 0) {
            path = path + root_length;
        }
        if (file.isDirectory()) {
            printf("%s/\n", path);
        } else {
            printf("%s\n", path);
        }
    }
}

static int PrintLs(const char *path) {
    SDClass sdcard = SDClass();
    if (!sdcard.begin()) {
        return -YuruInstrumentConfig::kErrNoDev;
    }
    File dir = sdcard.open(path);
    if (dir) {
        if (dir.isDirectory()) {
            File file;
            while ((file = dir.openNextFile())) {
                PrintLsItem(file);
                file.close();
            }
        } else {
            PrintLsItem(dir);
        }
        dir.close();
        return YuruInstrumentConfig::kNoError;
    } else {
        error_printf("[%s::%s] error: '%s': No such file or directory\n", kClassName, __func__, path);
        return -YuruInstrumentConfig::kErrNoEnt;
    }
}

static int CommandLs(YuruInstrumentConsole * /*console*/, int argc, char *argv[]) {
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            int err = PrintLs(argv[i]);
            if (err) {
                return err;
            }
        }
        return YuruInstrumentConfig::kNoError;
    } else {
        return PrintLs("");
    }
}

static int CommandMkdir(YuruInstrumentConsole * /*console*/, int argc, char *argv[]) {
    SDClass sdcard = SDClass();
    sdcard.begin();
    for (int i = 1; i < argc; i++) {
        if (!sdcard.mkdir(argv[i])) {
            return -YuruInstrumentConfig::kErrInval;
        }
    }
    return YuruInstrumentConfig::kNoError;
}

static int CommandRm(YuruInstrumentConsole * /*console*/, int argc, char *argv[]) {
    SDClass sdcard = SDClass();
    sdcard.begin();
    for (int i = 1; i < argc; i++) {
        if (sdcard.exists(argv[i])) {
            if (!sdcard.remove(argv[i])) {
                return -YuruInstrumentConfig::kErrPerm;
            }
        } else {
            error_printf("[%s::%s] error: '%s': No such file or directory\n", kClassName, __func__, argv[i]);
            return -YuruInstrumentConfig::kErrNoEnt;
        }
    }
    return YuruInstrumentConfig::kNoError;
}

static int CommandRmdir(YuruInstrumentConsole * /*console*/, int argc, char *argv[]) {
    SDClass sdcard = SDClass();
    sdcard.begin();
    for (int i = 1; i < argc; i++) {
        if (sdcard.exists(argv[i])) {
            if (!sdcard.rmdir(argv[i])) {
                return -YuruInstrumentConfig::kErrInval;
            }
        } else {
            error_printf("[%s::%s] error: '%s': No such file or directory\n", kClassName, __func__, argv[i]);
            return -YuruInstrumentConfig::kErrNoEnt;
        }
    }
    return YuruInstrumentConfig::kNoError;
}

static int CommandCat(YuruInstrumentConsole * /*console*/, int argc, char *argv[]) {
    SDClass sdcard = SDClass();
    sdcard.begin();
    for (int i = 1; i < argc; i++) {
        File file = sdcard.open(argv[i]);
        if (file) {
            int ch = -1;
            while ((ch = file.read()) >= 0) {
                printf("%c", ch);
            }
            printf("\n");
            file.close();
        } else {
            error_printf("[%s::%s] error: '%s': No such file or directory\n", kClassName, __func__, argv[i]);
            return -YuruInstrumentConfig::kErrNoEnt;
        }
    }
    return YuruInstrumentConfig::kNoError;
}

static int CommandOd(YuruInstrumentConsole * /*console*/, int argc, char *argv[]) {
    SDClass sdcard = SDClass();
    sdcard.begin();
    for (int i = 1; i < argc; i++) {
        int cursor = 0;
        File file = sdcard.open(argv[i]);
        if (file) {
            int ch = -1;
            while ((ch = file.read()) >= 0) {
                if (cursor && ((cursor & 0x0F) == 0)) {
                    printf("\n");
                }
                printf("%02x ", ch);
                cursor++;
            }
            printf("\n");
            file.close();
        } else {
            error_printf("[%s::%s] error: '%s': No such file or directory\n", kClassName, __func__, argv[i]);
            return -YuruInstrumentConfig::kErrNoEnt;
        }
    }
    return YuruInstrumentConfig::kNoError;
}

static int CommandAppend(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    if (argc > 1) {
        SDClass sdcard = SDClass();
        if (sdcard.begin()) {
            File file = sdcard.open(argv[1], FILE_WRITE);
            if (file) {
                if (argc > 2) {
                    file.seek(file.size());
                    for (int i = 2; i < argc; i++) {
                        if (i > 2) {
                            file.write(' ');
                        }
                        file.write((const uint8_t *)argv[2], strlen(argv[2]));
                    }
                    file.write('\n');
                }
                file.close();
            }
            return YuruInstrumentConfig::kNoError;
        }
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandLoadConfig(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    if (argc > 1) {
        Filter *filter = console->getFilter();
        if (filter) {
            return console->loadFromFile(argv[1]);
        }
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandSaveConfig(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    if (argc > 1) {
        Filter *filter = console->getFilter();
        if (filter) {
            return console->saveToFile(argv[1]);
        }
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandListParam(YuruInstrumentConsole *console, int /*argc*/, char * /*argv*/[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    Filter *filter = console->getFilter();
    if (filter) {
        return console->printParamList();
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandIsAvailable(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    if (argc > 1) {
        const char *param_id = argv[1];
        Filter *filter = console->getFilter();
        if (filter) {
            return console->printParam(param_id);
        }
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandGetParam(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    if (argc > 1) {
        const char *param_id = argv[1];
        Filter *filter = console->getFilter();
        if (filter) {
            return console->printParam(param_id);
        }
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandSetParam(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    if (argc > 2) {
        const char *param_id = argv[1];
        const char *value_str = argv[2];
        Filter *filter = console->getFilter();
        if (filter) {
            return console->setParam(param_id, value_str);
        }
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandSendNoteOff(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    Filter *filter = console->getFilter();
    if (argc > 3) {
        char *endp = nullptr;
        uint8_t note = 0;
        uint8_t velocity = 0;
        uint8_t channel = 0;
        note = strtol(argv[1], &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, argv[1]);
            return -YuruInstrumentConfig::kErrInval;
        }
        velocity = strtol(argv[2], &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, argv[2]);
            return -YuruInstrumentConfig::kErrInval;
        }
        channel = strtol(argv[3], &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, argv[3]);
            return -YuruInstrumentConfig::kErrInval;
        }
        if (filter) {
            filter->sendNoteOff(note, velocity, channel);
            return YuruInstrumentConfig::kNoError;
        }
        return YuruInstrumentConfig::kNoError;
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandSendNoteOn(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    Filter *filter = console->getFilter();
    if (argc > 3) {
        char *endp = nullptr;
        uint8_t note = 0;
        uint8_t velocity = 0;
        uint8_t channel = 0;
        note = strtol(argv[1], &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, argv[1]);
            return -YuruInstrumentConfig::kErrInval;
        }
        velocity = strtol(argv[2], &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, argv[2]);
            return -YuruInstrumentConfig::kErrInval;
        }
        channel = strtol(argv[3], &endp, 0);
        if (endp == nullptr || *endp != '\0') {
            error_printf("[%s::%s] parse error: parse '%s' as integer\n", kClassName, __func__, argv[3]);
            return -YuruInstrumentConfig::kErrInval;
        }
        if (filter) {
            filter->sendNoteOn(note, velocity, channel);
            return YuruInstrumentConfig::kNoError;
        }
        return YuruInstrumentConfig::kNoError;
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandSendMidiMessage(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (console == nullptr) {
        return -YuruInstrumentConfig::kErrInval;
    }
    Filter *filter = console->getFilter();
    uint8_t msg[16];
    size_t msg_length = sizeof(msg);
    memset(msg, 0x00, sizeof(msg));
    if ((argc - 1) < (int)msg_length) {
        msg_length = argc - 1;
    }
    for (size_t i = 0; i < msg_length; i++) {
        msg[i] = strtoul(argv[i + 1], nullptr, 0);
    }
    if (filter) {
        filter->sendMidiMessage(msg, msg_length);
        return YuruInstrumentConfig::kNoError;
    }
    return -YuruInstrumentConfig::kErrInval;
}

static int CommandHelp(YuruInstrumentConsole *console, int argc, char *argv[]);

// clang-format off
static const CommandSpec g_command_spec[] = {
    {"ls",                 CommandLs                },
    {"mkdir",              CommandMkdir             },
    {"rm",                 CommandRm                },
    {"rmdir",              CommandRmdir             },
    {"cat",                CommandCat               },
    {"od",                 CommandOd                },
    {"append",             CommandAppend            },
    {"load",               CommandLoadConfig        },
    {"save",               CommandSaveConfig        },
    {"list",               CommandListParam         },
    {"isAvailable",        CommandIsAvailable       },
    {"get",                CommandGetParam          },
    {"set",                CommandSetParam          },
    {"noteOff",            CommandSendNoteOff       },
    {"noteOn",             CommandSendNoteOn        },
    {"send",               CommandSendMidiMessage   },
    {"help",               CommandHelp              },
    {"?",                  CommandHelp              }};
// clang-format on

static int CommandHelp(YuruInstrumentConsole * /*console*/, int /*argc*/, char * /*argv*/[]) {
    const int kCommandSpecNum = sizeof(g_command_spec) / sizeof(g_command_spec[0]);
    for (int i = 0; i < kCommandSpecNum; i++) {
        printf("%s\n", g_command_spec[i].name);
    }
    return YuruInstrumentConfig::kNoError;
}

YuruInstrumentConsole::YuruInstrumentConsole(Filter &filter)
    : YuruInstrumentConfig(filter), line_length_(kMaxLineLength), line_buffer_(new char[line_length_]), line_index_(0) {
}

YuruInstrumentConsole::~YuruInstrumentConsole() {
    if (line_buffer_) {
        delete[] line_buffer_;
        line_buffer_ = nullptr;
    }
    line_length_ = 0;
    line_index_ = 0;
}

void YuruInstrumentConsole::begin() {
    Serial.begin(kBaudrate);
}

void YuruInstrumentConsole::poll() {
    while (Serial.available()) {
        int ch = Serial.read();
        if (ch == '\r' || ch == '\n') {
            line_buffer_[line_index_] = '\0';
            execute(line_buffer_, line_index_);
            line_index_ = 0;
        } else if (line_index_ < line_length_ - 1) {
            line_buffer_[line_index_++] = ch;
        }
    }
}

static int ExecuteCommand(YuruInstrumentConsole *console, int argc, char *argv[]) {
    if (argc >= 0) {
        printf(">");
        for (int i = 0; i < argc; i++) {
            printf(" %s", argv[i]);
        }
        printf("\n");
    }
    const int kCommandSpecNum = sizeof(g_command_spec) / sizeof(g_command_spec[0]);
    for (int i = 0; i < kCommandSpecNum; i++) {
        if (strcmp(g_command_spec[i].name, argv[0]) == 0) {
            int err = g_command_spec[i].func(console, argc, argv);
            if (err != 0) {
                error_printf("[%s::%s] error code: %d\n", kClassName, __func__, err);
            }
        }
    }
    return YuruInstrumentConfig::kNoError;
}

void YuruInstrumentConsole::execute(char *line, int length) {
    const int kMaxTokenNum = 20;
    struct TokenRange {
        int begin;
        int end;
    };
    TokenRange token[kMaxTokenNum] = {0};  // token[n][0]=start, token[n][1]=end
    int n = 0;
    bool in_token = false;
    for (int i = 0; i <= length; i++) {  // loop include '\0'
        if (in_token && (isSpace(line[i]) || line[i] == '\0')) {
            token[n].end = i - 1;
            in_token = false;
            n++;
        }
        if (!in_token && !isSpace(line[i])) {
            token[n].begin = token[n].end = i;
            in_token = true;
        }

        if (line[i] == '\0') {
            break;
        }
        if (n >= kMaxTokenNum) {
            break;
        }
    }

    char *args[kMaxTokenNum] = {0};
    for (int i = 0; i < n; i++) {
        args[i] = &line[token[i].begin];
        line[token[i].end + 1] = '\0';
    }
    ExecuteCommand(this, n, args);
}

#endif  // ARDUINO_ARCH_SPRESENSE
