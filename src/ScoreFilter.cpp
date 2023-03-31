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

#if defined(DEBUG)
static void showRegistingNote(const std::vector<ScoreFilter::Note>& playing_notes) {
    const char* status_labels[] = {"PAUSE", "PLAY", "END"};
    for (const auto& e : playing_notes) {
        debug_printf("[%s::%s]: note:%d, velocity:%d, channel:%d -> stat:%s\n", kClassName, __func__, e.note, e.velocity, e.channel, status_labels[e.stat]);
    }
}
#endif  // defined(DEBUG)

ScoreFilter::ScoreFilter(const String& file_name, Filter& filter)
    : BaseFilter(filter),
      file_name_(file_name),
      score_name_(),
      parser_(nullptr),
      score_index_(0),
      root_tick_(kDefaultTick),
      playing_notes_(),
      play_track_flags_(~0) {
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
    parser_->setPlayTrack(play_track_flags_);
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
            score_name_ = parser_->getFileName();
            return (intptr_t)score_name_.c_str();
        }
    } else {
        return BaseFilter::getParam(param_id);
    }
    return false;
}

bool ScoreFilter::setParam(int param_id, intptr_t value) {
    trace_printf("[%s::%s] id:%d, value:%d, (target:%d)\n", kClassName, __func__, param_id, (int)value, ScoreFilter::PARAMID_STATUS);
    if (param_id == ScoreFilter::PARAMID_NUMBER_OF_SCORES) {
        return false;
    } else if (param_id == ScoreFilter::PARAMID_ENABLE_TRACK) {
        if (value >= kMaxTrack) {
            return false;
        }
        play_track_flags_ = play_track_flags_ | (1U << value);
        if (parser_) {
            parser_->setPlayTrack(play_track_flags_);
        }
        return true;
    } else if (param_id == ScoreFilter::PARAMID_DISABLE_TRACK) {
        if (value >= kMaxTrack) {
            return false;
        }
        play_track_flags_ = play_track_flags_ & ~(1U << value);
        if (parser_) {
            parser_->setPlayTrack(play_track_flags_);
        }
        return true;
    } else if (param_id == ScoreFilter::PARAMID_TRACK_MASK) {
        play_track_flags_ = value;
        if (parser_) {
            parser_->setPlayTrack((uint32_t)value);
            return true;
        }
    } else if (param_id == ScoreFilter::PARAMID_SCORE) {
        return setScoreIndex((uint8_t)value);
    } else if (param_id == ScoreFilter::PARAMID_SCORE_NAME) {
        return false;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
    return false;
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

#if defined(DEBUG)
    showRegistingNote(playing_notes_);
#endif  // defined(DEBUG)

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

#if defined(DEBUG)
    showRegistingNote(playing_notes_);
#endif  // defined(DEBUG)

    return BaseFilter::sendNoteOn(note, velocity, channel);
}

bool ScoreFilter::sendSongPositionPointer(uint16_t beats) {
    debug_printf("[%s::%s] enter(%d)\n", kClassName, __func__, (int)beats);
    for (auto& e : playing_notes_) {
        if (e.stat != END) {
            sendNoteOff(e.note, e.velocity, e.channel);
            e.stat = END;
        }
    }
    return BaseFilter::sendSongPositionPointer(beats);
}

bool ScoreFilter::sendSongSelect(uint8_t song) {
    debug_printf("[%s::%s] enter(%d)\n", kClassName, __func__, (int)song);
    bool ret = true;
    if (setScoreIndex(song) == false) {
        ret = false;
    }
    if (BaseFilter::sendSongPositionPointer(song) == false) {
        ret = false;
    }
    return ret;
}

bool ScoreFilter::sendContinue() {
    debug_printf("[%s::%s] enter()\n", kClassName, __func__);
    for (uint8_t ch = 0; ch < 16; ch++) {
        BaseFilter::sendControlChange(0x7B, 0, ch + 1);
    }

    for (auto& e : playing_notes_) {
        if (e.stat == PAUSE) {
            sendNoteOn(e.note, e.velocity, e.channel);
            e.stat = PLAY;
        }
    }
    return BaseFilter::sendContinue();
}

bool ScoreFilter::sendStop() {
    debug_printf("[%s::%s] enter()\n", kClassName, __func__);

    for (uint8_t ch = 0; ch < 16; ch++) {
        BaseFilter::sendControlChange(0x7B, 0, ch + 1);
    }
    
    for (auto& e : playing_notes_) {
        if (e.stat == PLAY) {
            e.stat = PAUSE;
        }
    }
    return BaseFilter::sendStop();
}

bool ScoreFilter::sendMtcFullMessage(uint8_t /*hr*/, uint8_t /*mn*/, uint8_t /*sc*/, uint8_t /*fr*/) {
    // debug_printf("[%s::%s] enter(%d,%02d:%02d:%02d:%02d)\n", kClassName, __func__,  //
    //              (hr >> 5) & 0x03, hr & 0x1F, mn & 0x3F, sc & 0x3F, fr & 0x1F);
    return true;
}

bool ScoreFilter::sendMidiMessage(uint8_t* msg, size_t length) {
    debug_printf("[%s::%s] enter(%p,%d)\n", kClassName, __func__, msg, (int)length);
    if (msg == nullptr || length == 0) {
        return false;
    }
    if (msg[0] == MIDI_MSG_SYS_EX_EVENT) {
        if (length >= 10) {
            if (msg[1] == 0x7F &&  // real-time universal message
                msg[2] == 0x7F &&  // <device ID> = global broadcast
                msg[3] == 0x01 &&  // MIDI Time Code
                msg[4] == 0x01 &&  // Full Time Code Message
                msg[9] == 0xF7) {  // EOX
                return sendMtcFullMessage(msg[5], msg[6], msg[7], msg[8]);
            }
        }
    }
    return BaseFilter::sendMidiMessage(msg, length);
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
    return true;
}

uint16_t ScoreFilter::getRootTick() {
    return root_tick_;
}

bool ScoreFilter::getMidiMessage(ScoreParser::MidiMessage* midi_message) {
    return parser_->getMidiMessage(midi_message);
}

#endif  // ARDUINO_ARCH_SPRESENSE
