/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ScoreFilter.h"

#include "midi_util.h"
#include "path_util.h"
#include "SmfParser.h"
#include "TextScoreParser.h"

// #define DEBUG (1)

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#if defined(DEBUG)
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // if defined(DEBUG)

static const char kClassName[] = "ScoreFilter";

const int kDefaultTick = 960;
const int kDefaultTempo = (int)60000000 / 120;

static const int kMaxTrack = 32;

static void showRegistingNote(const std::vector<ScoreFilter::Note>& playing_notes) {
#if defined(DEBUG)
    const char* status_labels[] = {"PAUSE", "PLAY", "END"};
    for (const auto& e : playing_notes) {
        debug_printf("[%s::%s]: note:%d, velocity:%d, channel:%d -> stat:%s\n", kClassName, __func__, e.note, e.velocity, e.channel, status_labels[e.stat]);
    }
#endif  // defined(DEBUG)
}

ScoreFilter::ScoreFilter(const String& file_name, Filter& filter)
    : BaseFilter(filter), file_name_(file_name), parser_(nullptr), score_index_(0), root_tick_(kDefaultTick), playing_notes_(), play_track_flags_(~0) {
}

ScoreFilter::~ScoreFilter() {
    if (parser_) {
        delete parser_;
        parser_ = nullptr;
    }
}

bool ScoreFilter::begin() {
    ParserFactory parser_factory;
    parser_ = parser_factory.getScoreParser(file_name_.c_str());
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return false;
    }
    debug_printf("[%s::%s] score num:%d\n", kClassName, __func__, parser_->getNumberOfScores());
    if (parser_->getNumberOfScores() <= 0) {
        error_printf("[%s::%s] error: Score not found.\n", kClassName, __func__);
        return false;
    }
    debug_printf("[%s::%s] parser initialize success.\n", kClassName, __func__);

    return BaseFilter::begin();
}

bool ScoreFilter::isAvailable(int param_id) {
    if (param_id == ScoreFilter::PARAMID_NUMBER_OF_SCORES) {
        return true;
    } else if (param_id == ScoreFilter::PARAMID_ENABLE_TRACK) {
        return true;
    } else if (param_id == ScoreFilter::PARAMID_DISABLE_TRACK) {
        return true;
    } else if (param_id == ScoreFilter::PARAMID_TRACK_MASK) {
        return true;
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        return true;
    } else if (param_id == ScoreFilter::PARAMID_SCORE_NAME) {
        return true;
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

intptr_t ScoreFilter::getParam(int param_id) {
    if (param_id == ScoreFilter::PARAMID_NUMBER_OF_SCORES) {
        if (parser_ == nullptr) {
            return 0;
        } else {
            return parser_->getNumberOfScores();
        }
    } else if (param_id == ScoreFilter::PARAMID_ENABLE_TRACK) {
        if (parser_ == nullptr) {
            return play_track_flags_;
        } else {
            return (intptr_t)parser_->getPlayTrack();
        }
    } else if (param_id == ScoreFilter::PARAMID_DISABLE_TRACK) {
        if (parser_ == nullptr) {
            return play_track_flags_;
        } else {
            return (intptr_t)parser_->getPlayTrack();
        }
    } else if (param_id == ScoreFilter::PARAMID_TRACK_MASK) {
        if (parser_ == nullptr) {
            return play_track_flags_;
        } else {
            return (intptr_t)parser_->getPlayTrack();
        }
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        return getScoreIndex();
    } else if (param_id == ScoreFilter::PARAMID_SCORE_NAME) {
        if (parser_ == nullptr) {
            return (intptr_t) nullptr;
        } else {
            return (intptr_t)parser_->getFileName().c_str();
        }
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool ScoreFilter::setParam(int param_id, intptr_t value) {
    trace_printf("[%s::%s] id:%d, value:%d, (target:%d)\n", kClassName, __func__, param_id, (int)value, ScoreFilter::PARAMID_STATUS);
    if (param_id == ScoreFilter::PARAMID_NUMBER_OF_SCORES) {
        return false;
    } else if (param_id == ScoreFilter::PARAMID_ENABLE_TRACK) {
        if (parser_ == nullptr) {
            if (value < kMaxTrack) {
                play_track_flags_ = play_track_flags_ | (1U << value);
                return true;
            }
            return false;
        } else {
            return parser_->setEnableTrack((uint16_t)value);
        }
    } else if (param_id == ScoreFilter::PARAMID_DISABLE_TRACK) {
        if (parser_ == nullptr) {
            if (value < kMaxTrack) {
                play_track_flags_ = play_track_flags_ & ~(1U << value);
                return true;
            }
            return false;
        } else {
            return parser_->setDisableTrack((uint16_t)value);
        }
    } else if (param_id == ScoreFilter::PARAMID_TRACK_MASK) {
        if (parser_ == nullptr) {
            play_track_flags_ = value;
            return true;
        } else {
            return parser_->setPlayTrack((uint32_t)value);
        }
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        return setScoreIndex((int)value);
    } else if (param_id == ScoreFilter::PARAMID_SCORE_NAME) {
        return false;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

bool ScoreFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (note < NOTE_NUMBER_MIN || NOTE_NUMBER_MAX < note) {
        return false;
    }
    for (auto& e : playing_notes_) {
        if (e.note == note && e.channel == channel) {
            e.stat = END;
            break;
        }
    }

    showRegistingNote(playing_notes_);

    return BaseFilter::sendNoteOff(note, velocity, channel);
}

bool ScoreFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (note < NOTE_NUMBER_MIN || NOTE_NUMBER_MAX < note) {
        return false;
    }
    if (velocity == 0) {
        return sendNoteOff(note, velocity, channel);
    }

    for (auto& e : playing_notes_) {
        if (e.note == note && e.channel == channel) {
            if (e.stat != END) {
                // already playing
                return false;
            }
        }
    }

    Note* playing_note = nullptr;
    for (auto& e : playing_notes_) {
        if (e.stat == END) {
            playing_note = &e;
            break;
        }
    }
    if (playing_note == nullptr) {
        playing_notes_.push_back(Note());
        Note& e = playing_notes_.back();
        playing_note = &e;
    }
    playing_note->note = note;
    playing_note->velocity = velocity;
    playing_note->channel = channel;
    playing_note->stat = PLAY;

    showRegistingNote(playing_notes_);

    return BaseFilter::sendNoteOn(note, velocity, channel);
}

bool ScoreFilter::sendSongPositionPointer(uint16_t beats) {
    for (auto& e : playing_notes_) {
        if (e.stat != END) {
            sendNoteOff(e.note, e.velocity, e.channel);
            e.stat = END;
        }
    }
    return true;
}

bool ScoreFilter::sendSongSelect(uint8_t song) {
    bool ret = true;
    if (sendSongPositionPointer(0) == false) {
        ret = false;
    }
    setScoreIndex(song);
    return ret;
}

bool ScoreFilter::sendStart() {
    bool ret = true;
    if (sendSongPositionPointer(0) == false) {
        ret = false;
    }
    if (sendContinue() == false) {
        ret = false;
    }
    return ret;
}

bool ScoreFilter::sendContinue() {
    for (auto& e : playing_notes_) {
        if (e.stat == PAUSE) {
            sendNoteOn(e.note, e.velocity, e.channel);
            e.stat = PLAY;
        }
    }
    return true;
}

bool ScoreFilter::sendStop() {
    for (auto& e : playing_notes_) {
        if (e.stat == PLAY) {
            sendNoteOff(e.note, e.velocity, e.channel);
            e.stat = PAUSE;
        }
    }
    return true;
}

bool ScoreFilter::sendMidiMessage(uint8_t* msg, size_t length) {
    if (msg == nullptr || length == 0) {
        return false;
    }
    debug_printf("[%s::%s] Execute MIDI Message\n", kClassName, __func__);
    if (msg[0] == MIDI_MSG_SONG_POSITION_POINTER && length >= MIDI_MSGLEN_SONG_POSITION_POINTER) {
        return sendSongPositionPointer(((msg[2] & 0x7F) << 7) | ((msg[1] & 0x7F) << 0));
    } else if (msg[0] == MIDI_MSG_SONG_SELECT && length >= MIDI_MSGLEN_SONG_SELECT) {
        return sendSongSelect(msg[1] & 0x7F);
    } else if (msg[0] == MIDI_MSG_START && length >= MIDI_MSGLEN_START) {
        return sendStart();
    } else if (msg[0] == MIDI_MSG_CONTINUE && length >= MIDI_MSGLEN_CONTINUE) {
        return sendContinue();
    } else if (msg[0] == MIDI_MSG_STOP && length >= MIDI_MSGLEN_STOP) {
        return sendStop();
    } else {
        return BaseFilter::sendMidiMessage(msg, length);
    }
}

bool ScoreFilter::isParserAvailable() {
    return parser_ != nullptr;
}

int ScoreFilter::getNumberOfScores() {
    if (parser_ == nullptr) {
        return 0;
    }
    return parser_->getNumberOfScores();
}

int ScoreFilter::getScoreIndex() {
    return score_index_;
}

bool ScoreFilter::setScoreIndex(int index) {
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return false;
    }
    if (parser_->getNumberOfScores() == 0) {
        error_printf("[%s::%s] error: cannot find playable track\n", kClassName, __func__);
        return false;
    }
    if (index < 0 || parser_->getNumberOfScores() <= index) {
        error_printf("[%s::%s] error: out of track number (%d/%d)\n", kClassName, __func__, index, parser_->getNumberOfScores());
        return false;
    }

    score_index_ = index;
    parser_->setPlayTrack(play_track_flags_);
    parser_->loadScore(index);
    root_tick_ = parser_->getRootTick();
    debug_printf("[%s::%s] select title:%s, root_tick_:%d\n", kClassName, __func__, parser_->getTitle(index).c_str(), root_tick_);
    sendSongPositionPointer(0);
    return true;
}

uint16_t ScoreFilter::getRootTick() {
    return root_tick_;
}

bool ScoreFilter::getMidiMessage(ScoreParser::MidiMessage* midi_message) {
    return parser_->getMidiMessage(midi_message);
}

#endif  // ARDUINO_ARCH_SPRESENSE
