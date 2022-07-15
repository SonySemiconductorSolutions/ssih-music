/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "OneKeySynthesizerFilter.h"

#include <time.h>

#include "ScoreSrc.h"
#include "ToneFilter.h"

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#define DEBUG
#ifdef DEBUG
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else  // DEBUG
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // DEBUG

// others getter setter
void OneKeySynthesizerFilter::setScore(unsigned int new_score_num) {
    score_num_ = constrain(new_score_num, 0, (scores_.size() - 1));

    selectScore(score_num_);
}

int OneKeySynthesizerFilter::getScoreNum() {
    return constrain(score_num_, 0, (scores_.size() - 1));
}

OneKeySynthesizerFilter::OneKeySynthesizerFilter(const String& file_name, Filter& filter)
    : BaseFilter(filter),
      score_num_(0),
      prev_note_(INVALID_NOTE_NUMBER),
      note_registered_(INVALID_NOTE_NUMBER),
      is_note_registered_(false),
      is_note_playing_(false) {
    sr_ = new ScoreReader(file_name);
    scores_ = sr_->getScoresData();
}

OneKeySynthesizerFilter::OneKeySynthesizerFilter(Filter& filter) : OneKeySynthesizerFilter("SCORE", filter) {
}

OneKeySynthesizerFilter::~OneKeySynthesizerFilter() {
    delete sr_;
}

bool OneKeySynthesizerFilter::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        setScore((unsigned int)value);
        return true;
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE) {
        return false;
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE_NAME) {
        return false;
    } else if (param_id >= ScoreSrc::PARAMID_SCORE_NAME) {
        return false;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

intptr_t OneKeySynthesizerFilter::getParam(int param_id) {
    static char error_message[] = "Score Not Found";
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return getScoreNum();
        // SCORE's Info
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE) {
        return scores_.size();
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE_NAME) {
        if (scores_.size() > 0) {
            return (intptr_t)scores_[getScoreNum()].title.c_str();
        } else {
            return (intptr_t)error_message;
        }
    } else if (param_id >= ScoreSrc::PARAMID_SCORE_NAME) {
        if (scores_.size() > 0) {
            int index = constrain((param_id - ScoreSrc::PARAMID_SCORE_NAME), 0, (int)scores_.size() - 1);
            return (intptr_t)scores_[index].title.c_str();
        } else {
            return (intptr_t)error_message;
        }
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool OneKeySynthesizerFilter::isAvailable(int param_id) {
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return true;
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE_NAME) {
        if (scores_.size() != 0) {
            return true;
        } else {
            return false;
        }
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE) {
        return false;
    } else if (param_id >= ScoreSrc::PARAMID_SCORE_NAME) {
        return false;
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

void OneKeySynthesizerFilter::selectScore(int id) {
    sr_->load(scores_[id]);
    if (BaseFilter::isAvailable(ToneFilter::PARAMID_TONE)) {
        setParam(ToneFilter::PARAMID_TONE, scores_[id].tone);
    }
}

bool OneKeySynthesizerFilter::begin() {
    if (scores_.size() != 0) {
        selectScore(getScoreNum());
    } else {
        error_printf("OneKeySynthesizerFilter: ERROR: Score not found.\n");
    }

    return BaseFilter::begin();
}

bool OneKeySynthesizerFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (scores_.size() != 0) {
        registerNote();
    }
    if (is_note_registered_) {    //ノート登録状態
        if (!is_note_playing_) {  //ノート停止状態
            BaseFilter::sendNoteOn(note_registered_, velocity, channel);
            is_note_playing_ = true;  //ノート再生状態
        }
    }
    return true;
}

bool OneKeySynthesizerFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (is_note_playing_) {  //ノート再生状態
        BaseFilter::sendNoteOff(note_registered_, velocity, channel);
        is_note_registered_ = false;  //ノート未登録状態
        is_note_playing_ = false;     //ノート停止状態
    }
    return true;
}

void OneKeySynthesizerFilter::update() {
    BaseFilter::update();
}

void OneKeySynthesizerFilter::registerNote() {
    if (!is_note_registered_) {  //ノート未登録状態
        bool straddle_rest = false;
        while (true) {
            ScoreReader::Note note = sr_->getNote();
            if (NOTE_NUMBER_MIN <= note.note_num && note.note_num <= NOTE_NUMBER_MAX) {  //有効ノート
                if (note.velocity == 0) {
                    straddle_rest = true;
                } else if ((note_registered_ != note.note_num) || straddle_rest) {
                    note_registered_ = note.note_num;
                    is_note_registered_ = true;  //ノート登録状態
                    break;
                }
            } else if (note.note_num == INVALID_NOTE_NUMBER) {
                straddle_rest = true;
            } else if (note.note_num == ScoreReader::kCommandScoreEnd) {
                break;
            }
        }
    }
}

#endif  // ARDUINO_ARCH_SPRESENSE
