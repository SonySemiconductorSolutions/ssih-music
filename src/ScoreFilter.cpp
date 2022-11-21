/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ScoreFilter.h"

#include "path_util.h"
#include "SmfParser.h"
#include "TextScoreParser.h"

//#define DEBUG (1)

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

static void showRegistingNote(const std::vector<ScoreFilter::Note>& playing_notes) {
    for (const auto& e : playing_notes) {
        debug_printf("[%s::%s]: note:%d, velocity:%d, channel:%d -> stat:", kClassName, __func__, e.note, e.velocity, e.channel);
        if (e.stat == ScoreFilter::kNoteStatePlaying) {
            debug_printf("Playing\n");
        } else if (e.stat == ScoreFilter::kNoteStateEnd) {
            debug_printf("End\n");
        } else if (e.stat == ScoreFilter::kNoteStatePause) {
            debug_printf("Pause\n");
        }
    }
    debug_printf("\n");
}

ScoreFilter::ScoreFilter(const String& file_name, Filter& filter)
    : BaseFilter(filter), file_name_(file_name), score_index_(0), root_tick_(kDefaultTick), playing_notes_() {
}

ScoreFilter::~ScoreFilter() {
    if (parser_) {
        delete parser_;
        parser_ = nullptr;
    }
}

bool ScoreFilter::setParam(int param_id, intptr_t value) {
    trace_printf("[%s::%s] id:%d, value:%d, (target:%d)\n", kClassName, __func__, param_id, (int)value, ScoreFilter::PARAMID_STATUS);
    if (param_id == ScoreFilter::PARAMID_NUMBER_OF_SCORES) {
        return false;
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        setScoreIndex((int)value);
        return true;
    } else if (param_id == ScoreFilter::PARAMID_SCORE_NAME) {
        return false;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

intptr_t ScoreFilter::getParam(int param_id) {
    if (param_id == ScoreFilter::PARAMID_NUMBER_OF_SCORES) {
        if (parser_ == nullptr) {
            return 0;
        } else {
            return parser_->getNumberOfScores();
        }
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        return score_index_;
    } else if (param_id == ScoreFilter::PARAMID_SCORE_NAME) {
        if (parser_ == nullptr) {
            return (intptr_t) nullptr;
        } else if (parser_->getNumberOfScores() > 0) {
            return (intptr_t)parser_->getFileName().c_str();
        } else {
            return (intptr_t) nullptr;
        }
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool ScoreFilter::isAvailable(int param_id) {
    if (param_id == ScoreFilter::PARAMID_NUMBER_OF_SCORES) {
        return true;
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        return true;
    } else if (param_id == ScoreFilter::PARAMID_SCORE_NAME) {
        return true;
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

bool ScoreFilter::begin() {
    ParserFactory parser_factory;

    parser_ = parser_factory.getScoreParser(file_name_.c_str());
    debug_printf("[%s::%s] score num:%d\n", kClassName, __func__, parser_->getNumberOfScores());
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return false;
    } else if (parser_->getNumberOfScores() > 0) {
        debug_printf("[%s::%s] : parser initialize success.\n", kClassName, __func__);
    } else {
        error_printf("[%s::%s] error: Score not found.\n", kClassName, __func__);
        return false;
    }

    return BaseFilter::begin();
}

bool ScoreFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (note < NOTE_NUMBER_MIN || NOTE_NUMBER_MAX < note) {
        return false;
    }
    if (velocity == 0) {
        return sendNoteOff(note, velocity, channel);
    }

    for (auto& e : playing_notes_) {
        if (e.note == note && e.channel == channel && e.stat != kNoteStateEnd) {
            return false;
        }
    }

    Note* midi_note = nullptr;
    for (auto& e : playing_notes_) {
        if (e.stat == kNoteStateEnd) {
            midi_note = &e;
            break;
        }
    }

    if (midi_note == nullptr) {
        playing_notes_.push_back(Note());
        Note& e = playing_notes_.back();
        midi_note = &e;
    }

    midi_note->note = note;
    midi_note->velocity = velocity;
    midi_note->channel = channel;
    midi_note->stat = kNoteStatePlaying;

    showRegistingNote(playing_notes_);

    return BaseFilter::sendNoteOn(note, velocity, channel);
}
bool ScoreFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (note < NOTE_NUMBER_MIN || NOTE_NUMBER_MAX < note) {
        return false;
    }
    for (auto& e : playing_notes_) {
        if (e.note == note && e.channel == channel) {
            e.stat = kNoteStateEnd;
            break;
        }
    }

    showRegistingNote(playing_notes_);

    return BaseFilter::sendNoteOff(note, velocity, channel);
}

bool ScoreFilter::setScoreIndex(int index) {
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return false;
    } else if (parser_->getNumberOfScores() == 0) {
        error_printf("[%s::%s] error: cannot find playable track\n", kClassName, __func__);
        return false;
    } else if (!(0 <= index && index < parser_->getNumberOfScores())) {
        error_printf("[%s::%s] error: out of track number (%d/%d)\n", kClassName, __func__, index, parser_->getNumberOfScores());
        return false;
    }
    score_index_ = index;
    parser_->loadScore(index);

    root_tick_ = parser_->getRootTick();
    debug_printf("select title:%s\n", parser_->getTitle(index).c_str());
    debug_printf("[%s::%s] root_tick_:%d\n", kClassName, __func__, root_tick_);
    return true;
}

bool ScoreFilter::getMidiMessage(ScoreParser::MidiMessage* midi_message) {
    return parser_->getMidiMessage(midi_message);
}

int ScoreFilter::getScoreIndex() {
    return score_index_;
}
bool ScoreFilter::isScoreParserReady() {
    bool ret = false;
    if (parser_ != nullptr) {
        ret = true;
    }
    return ret;
}

uint16_t ScoreFilter::getRootTick() {
    return root_tick_;
}

int ScoreFilter::getNumberOfScores() {
    if (parser_ == nullptr) {
        return 0;
    }
    return parser_->getNumberOfScores();
}

void ScoreFilter::pauseAllNotes() {
    for (const auto& e : playing_notes_) {
        if (e.stat == kNoteStatePlaying) {
            BaseFilter::sendNoteOff(e.note, e.velocity, e.channel);
            e.stat == kNoteStatePause;
        }
    }
}

void ScoreFilter::resumeAllNotes() {
    for (const auto& e : playing_notes_) {
        if (e.stat == kNoteStatePause) {
            BaseFilter::sendNoteOn(e.note, e.velocity, e.channel);
            e.stat == kNoteStatePlaying;
        }
    }
}

#endif  // ARDUINO_ARCH_SPRESENSE
