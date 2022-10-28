/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "OneKeySynthesizerFilter.h"

#include "ScoreFilter.h"
#include "ScoreParser.h"

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
//#define DEBUG (1)
#ifdef DEBUG
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else  // DEBUG
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // DEBUG

static const char kClassName[] = "OneKeySynthesizerFilter";

OneKeySynthesizerFilter::OneKeySynthesizerFilter(const String& file_name, Filter& filter)
    : ScoreFilter(file_name, filter), is_note_registered_(false), is_note_playing_(false), is_end_track_(false) {
}

OneKeySynthesizerFilter::~OneKeySynthesizerFilter() {
}

bool OneKeySynthesizerFilter::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreFilter::PARAMID_SCORE) {
        return selectScore((int)value);
    } else {
        return ScoreFilter::setParam(param_id, value);
    }
}

intptr_t OneKeySynthesizerFilter::getParam(int param_id) {
    return ScoreFilter::getParam(param_id);
}

bool OneKeySynthesizerFilter::isAvailable(int param_id) {
    return ScoreFilter::isAvailable(param_id);
}

bool OneKeySynthesizerFilter::begin() {
    if (ScoreFilter::begin()) {
        return selectScore(0);
    }
    return false;
}

bool OneKeySynthesizerFilter::selectScore(int id) {
    if (ScoreFilter::setScoreIndex(id)) {
        is_end_track_ = false;
        is_note_registered_ = false;
        is_note_playing_ = false;
        memset(&registered_midi_msg_, 0x00, sizeof(registered_midi_msg_));
        return true;
    }

    return false;
}

bool OneKeySynthesizerFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (!isScoreParserReady()) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return false;
    }
    if (is_end_track_) {
        debug_printf("[%s::%s] end of track.\n", kClassName, __func__);
        return true;
    }
    if (getNumberOfScores() > 0) {
        registerNote();
    }
    if (is_note_registered_) {    //ノート登録状態
        if (!is_note_playing_) {  //ノート停止状態
            uint8_t ch = registered_midi_msg_.status_byte & 0x0f;
            ScoreFilter::sendNoteOn(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, ch);
            is_note_playing_ = true;  //ノート再生状態
        }
    }
    return true;
}

bool OneKeySynthesizerFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (is_note_playing_) {  //ノート再生状態
        uint8_t ch = registered_midi_msg_.status_byte & 0x0f;
        ScoreFilter::sendNoteOff(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, ch);
        is_note_registered_ = false;  //ノート未登録状態
        is_note_playing_ = false;     //ノート停止状態
    }
    return true;
}

void OneKeySynthesizerFilter::registerNote() {
    if (!isScoreParserReady()) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return;
    }
    if (!is_note_registered_) {  //ノート未登録状態
        while (true) {
            ScoreParser::MidiMessage midi_message;
            memset(&midi_message, 0x00, sizeof(midi_message));
            getMidiMessage(&midi_message);
            debug_printf("[%s::%s] delta_time:%d,", kClassName, __func__, midi_message.delta_time);
            debug_printf(" status_byte:%02x, data_byte1:%02x, data_byte2:%02x, ", midi_message.status_byte, midi_message.data_byte1, midi_message.data_byte2);
            debug_printf(" event_code:%02x, event_length:%02x\n", midi_message.event_code, midi_message.event_length);

            uint8_t status = midi_message.status_byte & 0xf0;
            if (midi_message.status_byte == ScoreParser::kMetaEvent && midi_message.event_code == ScoreParser::kEndOfTrack) {  //楽譜終了
                is_end_track_ = true;
                break;
            } else if (status == ScoreParser::kNoteOn) {
                if (midi_message.data_byte2 > 0) {  // velocity = 0 (= sendNoteOff)
                    is_note_registered_ = true;     //ノート登録状態
                    registered_midi_msg_ = midi_message;
                    break;
                }
            }
        }
    }
}

#endif  // ARDUINO_ARCH_SPRESENSE
