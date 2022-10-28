/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "TextScoreParser.h"

#include <Arduino.h>

#include <File.h>
#include <SDHCI.h>

#include "BufferedFileReader.h"
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

static const char kClassName[] = "TextScoreParser";

static const int kDefaultTick = 96;
static const int kDefaultTempo = 120;

static int getHeaderValue(const String& line, char delim) {
    String value = line.substring(line.indexOf(delim) + 1);
    value.trim();
    return value.toInt();
}

static unsigned long calculateTick(int rhythm) {
    unsigned long tick = kDefaultTick;
    if (rhythm == TextScoreParser::kRhythm4Note) {
        tick = kDefaultTick;
    } else if (rhythm == TextScoreParser::kRhythm8Note) {
        tick = kDefaultTick / 2;
    } else if (rhythm == TextScoreParser::kRhythm16Note) {
        tick = kDefaultTick / 4;
    } else if (rhythm == TextScoreParser::kRhythm1NoteTriplets) {
        tick = kDefaultTick * 4 / 3;
    } else if (rhythm == TextScoreParser::kRhythm2NoteTriplets) {
        tick = kDefaultTick * 2 / 3;
    } else if (rhythm == TextScoreParser::kRhythm4NoteTriplets) {
        tick = kDefaultTick / 3;
    } else if (rhythm == TextScoreParser::kRhythm8NoteTriplets) {
        tick = kDefaultTick / 6;
    }
    return tick;
}

TextScoreParser::TextScoreParser(const String& path)
    : ScoreParser(),
      musics_(),
      file_(path.c_str()),
      reader_(file_),
      is_end_of_music_(true),
      tempo_(kDefaultTempo),
      tone_(0),
      rhythm_(TextScoreParser::kRhythm4Note),
      note_(INVALID_NOTE_NUMBER),
      duration_(0) {
    String title = "";
    int tempo = kDefaultTempo;
    int tone = 0;
    int rhythm = TextScoreParser::kRhythm4Note;
    reader_.seek(0);
    while (reader_.available()) {
        size_t pos = reader_.position();
        String line = reader_.readStringUntil('\n');
        line.trim();
        if (line.startsWith("#MUSIC_TITLE:")) {
            String value = line.substring(line.indexOf(":") + 1);
            value.trim();
            title = value;
        } else if (line.startsWith("#MUSIC_BPM:")) {
            int value = getHeaderValue(line, ':');
            if (value > 0) {
                tempo = value;
            }
        } else if (line.startsWith("#MUSIC_TONE:")) {
            int value = getHeaderValue(line, ':');
            if (value > 0) {
                tone = value;
            }
        } else if (line.startsWith("#MUSIC_RHYTHM:")) {
            int value = getHeaderValue(line, ':');
            if (value > 0) {
                rhythm = value;
            }
        } else if (line.equals("#MUSIC_START")) {
            TextScoreParser::Music music;
            music.offset = pos;
            music.tick = kDefaultTick;
            music.title = title;
            music.tempo = tempo;
            music.tone = tone;
            music.rhythm = rhythm;
            musics_.push_back(music);
        } else if (line.equals("#MUSIC_END")) {
            title = "";
            tempo = kDefaultTempo;
            tone = 0;
            rhythm = TextScoreParser::kRhythm4Note;
        }
    }
}

TextScoreParser::~TextScoreParser() {
    if (file_) {
        file_.close();
    }
}

static void createSetTempo(ScoreParser::MidiMessage* msg, int tempo) {
    if (msg == nullptr) {
        error_printf("[%s::%s] error: internal error, msg is NULL\n", kClassName, __func__);
        return;
    }
    uint32_t usec = 60000000 / tempo;
    msg->delta_time = 0;
    msg->status_byte = ScoreParser::kMetaEvent;
    msg->event_code = ScoreParser::kSetTempo;
    msg->event_length = 3;
    msg->sysex_array[0] = (usec >> 16) & 0xFF;
    msg->sysex_array[1] = (usec >> 8) & 0xFF;
    msg->sysex_array[2] = (usec >> 0) & 0xFF;
    debug_printf("[%s::%s] -> %4d %02X %02X %02X %02X %02X %02X\n", kClassName, __func__, msg->delta_time, msg->status_byte, msg->event_code, msg->event_length,
                 msg->sysex_array[0], msg->sysex_array[1], msg->sysex_array[2]);
}

static void createEndOfTrack(ScoreParser::MidiMessage* msg) {
    if (msg == nullptr) {
        error_printf("[%s::%s] error: internal error, msg is NULL\n", kClassName, __func__);
        return;
    }
    msg->delta_time = 0;
    msg->status_byte = ScoreParser::kMetaEvent;
    msg->event_code = ScoreParser::kEndOfTrack;
    msg->event_length = 0;
    debug_printf("[%s::%s] -> %4d %02X %02X %02X\n", kClassName, __func__, msg->delta_time, msg->status_byte, msg->event_code, msg->event_length);
}

static void createNoteOff(ScoreParser::MidiMessage* msg, uint32_t delta_time, uint8_t note) {
    if (msg == nullptr) {
        error_printf("[%s::%s] error: internal error, msg is NULL\n", kClassName, __func__);
        return;
    }
    msg->delta_time = delta_time;
    msg->status_byte = TextScoreParser::kNoteOff | DEFAULT_CHANNEL;
    msg->data_byte1 = note & 0x7F;
    msg->data_byte2 = DEFAULT_VELOCITY;
    debug_printf("[%s::%s] -> %4d %02X %02X %02X\n", kClassName, __func__, msg->delta_time, msg->status_byte, msg->data_byte1, msg->data_byte2);
}

static void createNoteOn(ScoreParser::MidiMessage* msg, uint32_t delta_time, uint8_t note) {
    if (msg == nullptr) {
        error_printf("[%s::%s] error: internal error, msg is NULL\n", kClassName, __func__);
        return;
    }
    msg->delta_time = delta_time;
    msg->status_byte = TextScoreParser::kNoteOn | DEFAULT_CHANNEL;
    msg->data_byte1 = note & 0x7F;
    msg->data_byte2 = DEFAULT_VELOCITY;
    debug_printf("[%s::%s] -> %4d %02X %02X %02X\n", kClassName, __func__, msg->delta_time, msg->status_byte, msg->data_byte1, msg->data_byte2);
}

bool TextScoreParser::getMidiMessage(ScoreParser::MidiMessage* msg) {
    int ch = -1;
    String note_str = "";
    while (!is_end_of_music_) {
        size_t pos = reader_.position();
        if ((ch = reader_.read()) < 0) {
            if (note_ != INVALID_NOTE_NUMBER) {
                createNoteOff(msg, duration_, note_);
                reader_.seek(pos);
                note_ = INVALID_NOTE_NUMBER;
                duration_ = 0;
                return true;
            } else {
                createEndOfTrack(msg);
                is_end_of_music_ = true;
                return true;
            }
        }
        if (isWhitespace(ch)) {
        } else if (ch == '/') {
            reader_.readStringUntil('\n');
        } else if (ch == '#') {
            String line = reader_.readStringUntil('\n');
            line.trim();
            debug_printf("[%s::%s] \"#%s\"\n", kClassName, __func__, line.c_str());
            if (line.startsWith("BPMCHANGE ")) {
                int tempo = getHeaderValue(line, ' ');
                if (tempo > 0) {
                    tempo_ = tempo;
                }
                createSetTempo(msg, tempo_);
                return true;
            } else if (line.startsWith("TONECHANGE ")) {
                tone_ = getHeaderValue(line, ' ');
            } else if (line.startsWith("RHYTHMCHANGE ")) {
                rhythm_ = getHeaderValue(line, ' ');
            } else if (line.startsWith("DELAY ")) {
                int delay_msec = getHeaderValue(line, ' ');
                // [tick] = [msec] * [tick/beat] * [beat/minute] * [minute/msec]
                duration_ += delay_msec * kDefaultTick * tempo_ / 60000;
            } else if (line.equals("MUSIC_START")) {
                createSetTempo(msg, tempo_);
                return true;
            } else if (line.equals("MUSIC_END")) {
                if (note_ != INVALID_NOTE_NUMBER) {
                    createNoteOff(msg, duration_, note_);
                    reader_.seek(pos);
                    note_ = INVALID_NOTE_NUMBER;
                    duration_ = 0;
                    return true;
                } else {
                    createEndOfTrack(msg);
                    is_end_of_music_ = true;
                    return true;
                }
            }
        } else if (ch == '-' || isDigit(ch)) {
            if (note_ != INVALID_NOTE_NUMBER) {
                createNoteOff(msg, duration_, note_);
                reader_.seek(pos);
                note_ = INVALID_NOTE_NUMBER;
                duration_ = 0;
                return true;
            } else {
                note_str += (char)ch;
            }
        } else if (ch == ',' || ch == ';') {
            if (note_str.length() == 0) {
                note_str = "";
                duration_ += calculateTick(rhythm_);
            } else if (note_str.startsWith("-")) {
                note_str = "";
                if (note_ != INVALID_NOTE_NUMBER) {
                    createNoteOff(msg, duration_, note_);
                    note_ = INVALID_NOTE_NUMBER;
                    duration_ = calculateTick(rhythm_);
                    return true;
                } else {
                    duration_ += calculateTick(rhythm_);
                }
            } else {
                note_ = note_str.toInt();
                note_str = "";
                createNoteOn(msg, duration_, note_);
                duration_ = calculateTick(rhythm_);
                return true;
            }
        }
    }
    return false;
}

uint16_t TextScoreParser::getRootTick() {
    return kDefaultTick;
}

String TextScoreParser::getFileName() {
    return reader_.name();
}

int TextScoreParser::getNumberOfScores() {
    return musics_.size();
}

bool TextScoreParser::loadScore(int id) {
    if (id < 0 || musics_.size() <= (size_t)id) {
        return false;
    }
    reader_.seek(musics_[id].offset);
    is_end_of_music_ = false;
    tempo_ = musics_[id].tempo;
    tone_ = musics_[id].tone;
    rhythm_ = musics_[id].rhythm;
    note_ = INVALID_NOTE_NUMBER;
    duration_ = 0;
    return true;
}

String TextScoreParser::getTitle(int id) {
    if (id < 0 || musics_.size() <= (size_t)id) {
        return String();
    }
    return musics_[id].title;
}

#endif
