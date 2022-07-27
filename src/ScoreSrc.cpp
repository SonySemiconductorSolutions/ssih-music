/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ScoreSrc.h"

#include <time.h>

#include "ToneFilter.h"

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

enum PlayState {
    kStaying = 0,
    kPlaying,
    kResting,
};

// others getter setter
void ScoreSrc::setScore(unsigned int new_score_num) {
    score_num_ = constrain(new_score_num, 0, (scores_.size() - 1));
    selectScore(score_num_);
    is_music_start_ = true;
}

int ScoreSrc::getScoreNum() {
    return constrain(score_num_, 0, (scores_.size() - 1));
}

ScoreSrc::ScoreSrc(const String& file_name, Filter& filter)
    : BaseFilter(filter),
      score_num_(0),
      prev_note_(INVALID_NOTE_NUMBER),
      bpm_(0),
      now_rhythm_(0),
      beat_ms_(0),
      triplets_beat_ms_(0),
      note_interval_(0),
      play_state_(kStaying),
      play_start_(0),
      play_end_(0),
      is_music_start_(true),
      note_(ScoreReader::Note()),
      is_waiting_(false) {
    sr_ = new ScoreReader(file_name);
    scores_ = sr_->getScoresData();
}

ScoreSrc::ScoreSrc(Filter& filter) : ScoreSrc("SCORE", filter) {
}

ScoreSrc::~ScoreSrc() {
    delete sr_;
}

bool ScoreSrc::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return false;
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE) {
        setScore((unsigned int)value);
        return true;
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE_NAME) {
        return false;
    } else if (ScoreSrc::PARAMID_SCORE_NAME <= param_id && param_id < (ScoreSrc::PARAMID_SCORE_NAME + (int)scores_.size())) {
        return false;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

intptr_t ScoreSrc::getParam(int param_id) {
    static char error_message[] = "Score Not Found";
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return scores_.size();
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE) {
        return getScoreNum();
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE_NAME) {
        if (scores_.size() != 0) {
            return (intptr_t)scores_[getScoreNum()].title.c_str();
        } else {
            return (intptr_t)error_message;
        }
    } else if (ScoreSrc::PARAMID_SCORE_NAME <= param_id && param_id < (ScoreSrc::PARAMID_SCORE_NAME + (int)scores_.size())) {
        size_t index = param_id - ScoreSrc::PARAMID_SCORE_NAME;
        if (index < scores_.size()) {
            return (intptr_t)scores_[index].title.c_str();
        } else {
            return (intptr_t)error_message;
        }
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool ScoreSrc::isAvailable(int param_id) {
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return true;
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE) {
        return true;
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE_NAME) {
        return true;
    } else if (ScoreSrc::PARAMID_SCORE_NAME <= param_id && param_id < (ScoreSrc::PARAMID_SCORE_NAME + (int)scores_.size())) {
        size_t index = param_id - ScoreSrc::PARAMID_SCORE_NAME;
        if (index < scores_.size()) {
            return true;
        } else {
            return false;
        }
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

void ScoreSrc::selectScore(int id) {
    // sr_->printScoresData(scores_);
    sr_->load(scores_[id]);
    if (sr_->isMidiFile()) {
        bpm_ = scores_[id].bpm;
        now_rhythm_ = scores_[id].rhythm;
        debug_printf("ScoreSrc: load midi\n");
    } else {
        setBpm(scores_[id].bpm);
        setRhythm(scores_[id].rhythm);
        debug_printf("ScoreSrc: load txt\n");
    }

    if (BaseFilter::isAvailable(ToneFilter::PARAMID_TONE)) {
        setParam(ToneFilter::PARAMID_TONE, scores_[id].tone);
    }
    trace_printf("ScoreSrc: set score:%d name:%s\n", id, scores_[id].title.c_str());
}

bool ScoreSrc::begin() {
    if (scores_.size() != 0) {
        selectScore(getScoreNum());
    } else {
        error_printf("ScoreSrc: ERROR: Score not found.\n");
    }

    return BaseFilter::begin();
}

void ScoreSrc::update() {
    if (scores_.size() != 0) {
        if (sr_->isMidiFile()) {
            playNoteMidi();
        } else {
            playNoteTxt();
        }
    }
    BaseFilter::update();
}

bool ScoreSrc::playNoteMidi() {
    if (!is_waiting_) {
        note_ = sr_->getNote();
        trace_printf("ScoreSrc: dt:%d nn:%d ve:%02x ch:%d\n", note_.delta_time, note_.note_num, note_.velocity, note_.ch);
        note_interval_ = sr_->calcDeltaTime(bpm_, note_.delta_time, now_rhythm_);
        play_end_ = millis() + note_interval_;
        trace_printf("ScoreSrc: intarval:%lu, end:%lu\n", note_interval_, play_end_);
        is_waiting_ = true;
    }

    if (play_end_ <= millis()) {
        if (note_.note_num == ScoreReader::Command::kCommandRhythmChange) {
            now_rhythm_ = note_.velocity;
        } else if (NOTE_NUMBER_MIN <= note_.note_num && note_.note_num <= NOTE_NUMBER_MAX) {
            if (note_.velocity == 0) {
                sendNoteOff(note_.note_num, note_.velocity, note_.ch);
            } else {
                sendNoteOn(note_.note_num, note_.velocity, note_.ch);
            }
        }
        is_waiting_ = false;
    }

    return true;
}

bool ScoreSrc::playNoteTxt() {
    bool note_fin = false;          //ノートの再生が終了したかを返す
    if (play_state_ == kStaying) {  //ノート受付可能状態
        ScoreReader::Note note = sr_->getNote();
        trace_printf("ScoreSrc: dt:%d nn:%d ve:%02x ch:%d\n", note.delta_time, note.note_num, note.velocity, note.ch);
        debug_printf("ScoreSrc: playNote NUM:%d\n", note.note_num);

        if ((NOTE_NUMBER_MIN <= note.note_num && note.note_num <= NOTE_NUMBER_MAX) || note.note_num == INVALID_NOTE_NUMBER) {
            if (is_music_start_) {  //ここはフラグの方が適切(曲切り替えの対応が必要)
                play_start_ = millis();
                is_music_start_ = false;
                trace_printf("ScoreSrc: start:%d\n", play_start_);
            } else {
                play_start_ = play_end_;
            }
        }
        if (note.note_num == INVALID_NOTE_NUMBER) {  //休符
            sendNoteOff(prev_note_, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
            play_state_ = kResting;                                                         //休符実行中状態へ変更
        } else if (NOTE_NUMBER_MIN <= note.note_num && note.note_num <= NOTE_NUMBER_MAX) {  //有効ノート
            if (prev_note_ != note.note_num) {
                sendNoteOff(prev_note_, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                sendNoteOn(note.note_num, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
            }
            play_state_ = kPlaying;                                                                               //ノート再生中状態へ変更
        } else if (ScoreReader::kCommandNumMin < note.note_num && note.note_num < ScoreReader::kCommandNumMax) {  //特殊ノート等
            executeCommand(note);
            return true;
        } else {
            trace_printf("ScoreSrc: ERROR\n");
            note_fin = true;
        }
        prev_note_ = note.note_num;

        play_end_ = play_start_ + note_interval_;

        trace_printf("ScoreSrc: start:%d, end:%d, sendnote:%d\n", play_start_, play_end_);

    } else if (play_state_ == kResting || play_state_ == kPlaying) {  //休符実行中状態
        if (play_end_ <= millis()) {
            play_state_ = kStaying;
            note_fin = true;
        }
    }
    return note_fin;
}

bool ScoreSrc::executeCommand(ScoreReader::Note note) {
    if (note.note_num == ScoreReader::kCommandRhythmChange) {
        setRhythm(note.velocity);
    } else if (note.note_num == ScoreReader::kCommandToneChange) {
        if (BaseFilter::isAvailable(ToneFilter::PARAMID_TONE)) {
            setParam(ToneFilter::PARAMID_TONE, note.velocity);
        }
    } else if (note.note_num == ScoreReader::kCommandBpmChange) {
        setBpm(note.velocity);
    } else if (note.note_num == ScoreReader::kCommandDelay) {
        sendNoteOff(prev_note_, DEFAULT_VELOCITY, DEFAULT_CHANNEL);  // velocity
        play_end_ = play_start_ + note.velocity;
        play_state_ = kResting;
    } else if (note.note_num == ScoreReader::kCommandScoreEnd) {
        sendNoteOff(prev_note_, DEFAULT_VELOCITY, DEFAULT_CHANNEL);  // velocity
        play_state_ = kResting;
    } else {
        trace_printf("ScoreSrc: ERROR\n");
        return false;
    }
    return true;
}

// timing function
bool ScoreSrc::updateNoteInterval() {
    switch (now_rhythm_) {
        case ScoreReader::kRhythmNormal:
            note_interval_ = beat_ms_;
            trace_printf("ScoreSrc: NORMAL(16 NOTE) RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm8Note:
            note_interval_ = beat_ms_ * 2;
            trace_printf("ScoreSrc: 8_NOTE RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm4Note:
            note_interval_ = beat_ms_ * 4;
            trace_printf("ScoreSrc: 4_NOTE RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm1NoteTriplets:
            note_interval_ = triplets_beat_ms_ * 8;
            trace_printf("ScoreSrc: 2 NOTE TRIPLETS RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm2NoteTriplets:
            note_interval_ = triplets_beat_ms_ * 4;
            trace_printf("ScoreSrc: 4 NOTE TRIPLETS RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm4NoteTriplets:
            note_interval_ = triplets_beat_ms_ * 2;
            trace_printf("ScoreSrc: 8 NOTE TRIPLETS RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm8NoteTriplets:
            note_interval_ = triplets_beat_ms_;
            trace_printf("ScoreSrc: 16 NOTE TRIPLETS RHYTHM:%d\n", note_interval_);
            break;
    }
    return true;
}

void ScoreSrc::setRhythm(int new_rhythm) {
    now_rhythm_ = new_rhythm;
    updateNoteInterval();
}

void ScoreSrc::calcNoteInterval() {
    beat_ms_ = (int)(60000 / bpm_);
    beat_ms_ = beat_ms_ / 4;  // 16分音符の時間

    triplets_beat_ms_ = (beat_ms_ * 2) / 3;  //半拍3連 (24分音符)

    updateNoteInterval();
}

void ScoreSrc::setBpm(int new_bpm) {
    bpm_ = new_bpm;
    calcNoteInterval();

    trace_printf("ScoreSrc: BPM = %d\n", bpm_);
    trace_printf("ScoreSrc: 1 Beat milliseconds = %d\n", beat_ms_);
    trace_printf("ScoreSrc: 4拍3連 = %d, ", triplets_beat_ms_ * 8);
    trace_printf("ScoreSrc: 2拍3連 = %d, ", triplets_beat_ms_ * 4);
    trace_printf("ScoreSrc: 1拍3連 = %d, ", triplets_beat_ms_ * 2);
    trace_printf("ScoreSrc: 1/2拍3連 = %d\n", triplets_beat_ms_);
}

#endif  // ARDUINO_ARCH_SPRESENSE
