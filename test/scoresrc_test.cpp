/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

//#include <gtest/gtest.h>

#include "BufferedFileReader.h"
#include "SmfParser.h"
#include "TextScoreParser.h"

//#define DEBUG (1)

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

enum ScoreFileType {
    kScoreFileTypeTxt = 0,
    kScoreFileTypeMidi,
    kScoreFileTypeOthers,
};

int getFileType(String& file_type) {
    file_type.toLowerCase();
    if (file_type == ".txt") {
        return kScoreFileTypeTxt;
    } else if (file_type == ".mid" || file_type == ".midi") {
        return kScoreFileTypeMidi;
    } else {
        return kScoreFileTypeOthers;
    }
}

void midiparse() {
    String file_name = "testdata/BeethovenFurElise.mid";
    String file_type = file_name;
    int dot_position = file_type.lastIndexOf(".");
    if (dot_position < 0) {
        error_printf("ScoreReader: SD: file:%s This file is not supported.\n", file_name.c_str());
        return;
    } else {
        file_type = file_type.substring(dot_position);
    }
    debug_printf("File_name:%s\n", file_name.c_str());
    debug_printf("ScoreReader: SD: file type:%s\n", file_type.c_str());

    SmfParser parser(file_name);

    // MIDIヘッダー解析
    if (getFileType(file_type) == kScoreFileTypeMidi) {
        trace_printf("Midi\n");
        for (int i = 0; i < parser.getNumberOfScores(); i++) {
            debug_printf("ScoreReader: File Name    :%s\n", parser.getFileName().c_str());
            debug_printf("ScoreReader: Track        :%d\n", i);
            debug_printf("ScoreReader: Title        :%s\n", parser.getTitle(i).c_str());
            debug_printf("ScoreReader: Tick         :%d\n", parser.getRootTick());
        }
    } else {
        error_printf("ScoreReader: SD: file:%s This file is not supported.\n", file_name.c_str());
    }

    parser.loadScore(1);
    for (int i = 0; i < 10; i++) {
        ScoreParser::MidiMessage msg;
        parser.getMidiMessage(&msg);
        if (msg.status_byte == ScoreParser::kMetaEvent) {
            if (msg.event_code == ScoreParser::kSetTempo) {
                unsigned long tick = 0;
                for (int i = 0; i < (int)msg.event_length; i++) {
                    tick = (tick << 8) | msg.sysex_array[i];
                }
                printf("%d: delta_time:%d SetTempo tick:%lu\n", i, msg.delta_time, tick);
            } else if (msg.event_code == ScoreParser::kEndOfTrack) {
                printf("%d: delta_time:%d EndOfTrack\n", i, msg.delta_time);
            } else {
                printf("%d: delta_time:%d 0x%02X\n", i, msg.delta_time, msg.event_code);
            }
        } else if ((msg.status_byte & 0xF0) == ScoreParser::kNoteOff) {
            printf("%d: delta_time:%d NoteOff note_num:%d\n", i, msg.delta_time, msg.data_byte1);
        } else if ((msg.status_byte & 0xF0) == ScoreParser::kNoteOn) {
            printf("%d: delta_time:%d NoteOn note_num:%d\n", i, msg.delta_time, msg.data_byte1);
        } else {
            printf("%d: delta_time:%d 0x%02X\n", i, msg.delta_time, msg.status_byte);
        }
    }
}

void textparse() {
    String file_name = "testdata/Yukai_na_Makiba.txt";
    String file_type = file_name;
    int dot_position = file_type.lastIndexOf(".");
    if (dot_position < 0) {
        error_printf("ScoreReader: SD: file:%s This file is not supported.\n", file_name.c_str());
        return;
    } else {
        file_type = file_type.substring(dot_position);
    }
    debug_printf("File_name:%s\n", file_name.c_str());
    debug_printf("ScoreReader: SD: file type:%s\n", file_type.c_str());

    TextScoreParser parser(file_name);

    // MIDIヘッダー解析
    if (getFileType(file_type) == kScoreFileTypeTxt) {
        trace_printf("Midi\n");
        for (int i = 0; i < parser.getNumberOfScores(); i++) {
            debug_printf("ScoreReader: File Name    :%s\n", parser.getFileName().c_str());
            debug_printf("ScoreReader: Track        :%d\n", i);
            debug_printf("ScoreReader: Title        :%s\n", parser.getTitle(i).c_str());
            debug_printf("ScoreReader: Tick         :%d\n", parser.getRootTick());
        }
    } else {
        error_printf("ScoreReader: SD: file:%s This file is not supported.\n", file_name.c_str());
    }

    parser.loadScore(0);
    for (int i = 0; i < 10; i++) {
        ScoreParser::MidiMessage msg;
        parser.getMidiMessage(&msg);
        if (msg.status_byte == ScoreParser::kMetaEvent) {
            if (msg.event_code == ScoreParser::kSetTempo) {
                unsigned long tick = 0;
                for (int i = 0; i < (int)msg.event_length; i++) {
                    tick = (tick << 8) | msg.sysex_array[i];
                }
                printf("%d: delta_time:%d SetTempo tick:%lu\n", i, msg.delta_time, tick);
            } else if (msg.event_code == ScoreParser::kEndOfTrack) {
                printf("%d: delta_time:%d EndOfTrack\n", i, msg.delta_time);
            } else {
                printf("%d: delta_time:%d 0x%02X\n", i, msg.delta_time, msg.event_code);
            }
        } else if ((msg.status_byte & 0xF0) == ScoreParser::kNoteOff) {
            printf("%d: delta_time:%d NoteOff note_num:%d\n", i, msg.delta_time, msg.data_byte1);
        } else if ((msg.status_byte & 0xF0) == ScoreParser::kNoteOn) {
            printf("%d: delta_time:%d NoteOn note_num:%d\n", i, msg.delta_time, msg.data_byte1);
        } else {
            printf("%d: delta_time:%d 0x%02X\n", i, msg.delta_time, msg.status_byte);
        }
    }
}

int main(void) {
    midiparse();
    textparse();
    return 0;
}
