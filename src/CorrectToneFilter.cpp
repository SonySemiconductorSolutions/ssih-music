/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "CorrectToneFilter.h"

#include <time.h>

#include "ScoreSrc.h"
#include "ToneFilter.h"

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

// others getter setter
void CorrectToneFilter::setScore(unsigned int new_score_num) {
    score_num_ = constrain(new_score_num, 0, (scores_.size() - 1));
    selectScore(score_num_);
    is_music_start_ = true;
}

int CorrectToneFilter::getScoreNum() {
    return constrain(score_num_, 0, (scores_.size() - 1));
}

CorrectToneFilter::CorrectToneFilter(const String& file_name, Filter& filter)
    : BaseFilter(filter),
      is_note_registered_(false),
      is_note_playing_(false),
      play_start_(0),
      play_end_(0),
      is_music_start_(true),
      score_num_(0),
      note_registered_(INVALID_NOTE_NUMBER),
      note_playing_(INVALID_NOTE_NUMBER),
      bpm_(0),
      now_rhythm_(0),
      beat_ms_(0),
      triplets_beat_ms_(0),
      note_interval_(0),
      note_(ScoreReader::Note::Note()),
      is_waiting_(false) {
    sr_ = new ScoreReader(file_name);
    scores_ = sr_->getScoresData();
}

CorrectToneFilter::CorrectToneFilter(Filter& filter) : CorrectToneFilter("SCORE", filter) {
}

CorrectToneFilter::~CorrectToneFilter() {
    delete sr_;
}

bool CorrectToneFilter::setParam(int param_id, intptr_t value) {
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

intptr_t CorrectToneFilter::getParam(int param_id) {
    static char error_message[] = "Score Not Found";
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return constrain(getScoreNum(), 0, (scores_.size() - 1));
        // SCORE's Info
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE) {
        return scores_.size();
    } else if (param_id == ScoreSrc::PARAMID_PLAYING_SCORE_NAME) {
        if (scores_.size() != 0) {
            return (intptr_t)scores_[getScoreNum()].title.c_str();
        } else {
            return (intptr_t)error_message;
        }
    } else if (param_id >= ScoreSrc::PARAMID_SCORE_NAME) {
        if (scores_.size() != 0) {
            int index = constrain((param_id - ScoreSrc::PARAMID_SCORE_NAME), 0, scores_.size() - 1);
            return (intptr_t)scores_[index].title.c_str();
        } else {
            return (intptr_t)error_message;
        }
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool CorrectToneFilter::isAvailable(int param_id) {
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

void CorrectToneFilter::selectScore(int id) {
    // sr_->printScoresData(scores_);
    sr_->load(scores_[id]);
    if (sr_->isMidiFile()) {
        bpm_ = scores_[id].bpm;
        now_rhythm_ = scores_[id].rhythm;
        trace_printf("CorrectToneFilter: load midi\n");
    } else {
        setBpm(scores_[id].bpm);
        setRhythm(scores_[id].rhythm);
        trace_printf("CorrectToneFilter: load txt\n");
    }

    if (BaseFilter::isAvailable(ToneFilter::PARAMID_TONE)) {
        setParam(ToneFilter::PARAMID_TONE, scores_[id].tone);
    }
    trace_printf("CorrectToneFilter: set score:%d name:%s\n", id, scores_[id].title.c_str());
}

bool CorrectToneFilter::begin() {
    if (scores_.size() != 0) {
        selectScore(getScoreNum());
    } else {
        error_printf("CorrectToneFilter: ERROR: Score not found.\n");
    }

    return BaseFilter::begin();
}

void CorrectToneFilter::update() {
    if (scores_.size() != 0) {
        if (sr_->isMidiFile()) {
            registerNoteMidi();
        } else {
            registerNoteTxt();
        }
    }
    if (is_note_playing_) {
        changePlayingNote();
    }
    BaseFilter::update();
}

bool CorrectToneFilter::registerNoteMidi() {
    if (!is_waiting_) {
        note_ = sr_->getNote();
        trace_printf("CorrectToneFilter: dt:%d nn:%d ve:%02x ch:%d\n", note_.delta_time, note_.note_num, note_.velocity, note_.ch);
        note_interval_ = sr_->calcDeltaTime(bpm_, note_.delta_time, now_rhythm_);
        play_end_ = millis() + note_interval_;
        trace_printf("CorrectToneFilter: intarval:%lu, end:%lu\n", note_interval_, play_end_);
        is_waiting_ = true;
    }

    if (play_end_ <= millis()) {
        if (note_.note_num == ScoreReader::Command::kCommandRhythmChange) {
            now_rhythm_ = note_.velocity;
        } else if (NOTE_NUMBER_MIN <= note_.note_num && note_.note_num <= NOTE_NUMBER_MAX) {
            if (note_.velocity == 0) {
                note_registered_ = INVALID_NOTE_NUMBER;
                is_note_registered_ = true;  //ノート登録状態へ変更
            } else {
                note_registered_ = note_.note_num;
                is_note_registered_ = true;  //ノート登録状態へ変更
            }
        }
        is_waiting_ = false;
    }
    return true;
}

bool CorrectToneFilter::registerNoteTxt() {
    if (!is_note_registered_) {  //ノート受付可能状態
        ScoreReader::Note note = sr_->getNote();
        // sr_->printNote(note);
        if ((NOTE_NUMBER_MIN <= note.note_num && note.note_num <= NOTE_NUMBER_MAX) || note.note_num == INVALID_NOTE_NUMBER) {
            if (is_music_start_) {  //ここはフラグの方が適切(曲切り替えの対応が必要)
                play_start_ = millis();
                is_music_start_ = false;
                trace_printf("CorrectToneFilter: start:%d\n", play_start_);
            } else {
                play_start_ = play_end_;
            }
        }
        if (ScoreReader::kCommandNumMin < note.note_num && note.note_num < ScoreReader::kCommandNumMax) {
            executeCommand(note);
            return true;
        } else if (!(note.note_num == INVALID_NOTE_NUMBER || isMIDINum(note.note_num))) {
            return false;
        }
        debug_printf("CorrectToneFilter: playNote NUM:%d time:%d\n", note.note_num, note_interval_);
        note_registered_ = note.note_num;
        is_note_registered_ = true;  //ノート登録状態へ変更

        play_end_ = play_start_ + note_interval_;

        trace_printf("CorrectToneFilter: start:%d, end:%d, sendnote:%d\n", play_start_, play_end_);
    } else {                          //ノート再生中状態
        if (play_end_ <= millis()) {  // ノートの登録期間の確認
            is_note_registered_ = false;
        }
    }
    return true;
}

bool CorrectToneFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    velocity_ = velocity;
    channel_ = channel;
    playNote();
    return true;
}

void CorrectToneFilter::playNote() {
    if (!is_note_playing_) {                //ノート停止状態
        if (isMIDINum(note_registered_)) {  //有効ノート
            BaseFilter::sendNoteOn(note_registered_, velocity_, channel_);
        }
        note_playing_ = note_registered_;  //再生中のノートを保持
        is_note_playing_ = true;           //ノート再生状態
    } else {                               //ノート再生状態
        changePlayingNote();
    }
}

bool CorrectToneFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    stopNote(velocity, channel);
    return true;
}

void CorrectToneFilter::stopNote(uint8_t velocity, uint8_t channel) {
    if (is_note_playing_) {              //ノート再生状態
        if (isMIDINum(note_playing_)) {  //有効ノート
            BaseFilter::sendNoteOff(note_playing_, velocity, channel);
        }
        note_playing_ = INVALID_NOTE_NUMBER;
        is_note_playing_ = false;  //ノート停止状態
    }
}

void CorrectToneFilter::changePlayingNote() {
    trace_printf("CorrectToneFilter: note_playing_:%d, note_registered_%d\n", note_playing_, note_registered_);
    if (note_playing_ != note_registered_) {
        trace_printf("CorrectToneFilter: change\n");
        if (isMIDINum(note_playing_)) {
            BaseFilter::sendNoteOff(note_playing_, velocity_, channel_);
        }
        if (isMIDINum(note_registered_)) {  //有効ノート
            BaseFilter::sendNoteOn(note_registered_, velocity_, channel_);
        }
        note_playing_ = note_registered_;  //再生中のノートを保持
    }
}

bool CorrectToneFilter::isMIDINum(int note) {
    return (NOTE_NUMBER_MIN <= note && note <= NOTE_NUMBER_MAX);
}

bool CorrectToneFilter::executeCommand(ScoreReader::Note note) {
    if (note.note_num == ScoreReader::kCommandRhythmChange) {
        setRhythm(note.velocity);
    } else if (note.note_num == ScoreReader::kCommandToneChange) {
        if (BaseFilter::isAvailable(ToneFilter::PARAMID_TONE)) {
            setParam(ToneFilter::PARAMID_TONE, note.velocity);
        }
    } else if (note.note_num == ScoreReader::kCommandBpmChange) {
        setBpm(note.velocity);
    } else if (note.note_num == ScoreReader::kCommandDelay) {
        stopNote(DEFAULT_VELOCITY, DEFAULT_CHANNEL);  // velocity
        play_end_ = play_start_ + note.velocity;
        note_registered_ = note.note_num;
        is_note_registered_ = true;  //ノート登録状態へ変更
    } else if (note.note_num == ScoreReader::kCommandScoreEnd) {
        stopNote(DEFAULT_VELOCITY, DEFAULT_CHANNEL);  // velocity
        note_registered_ = note.note_num;
        is_note_registered_ = false;  //ノート登録状態へ変更
        is_note_playing_ = false;
    } else {
        trace_printf("CorrectToneFilter: ERROR\n");
        return false;
    }
    return true;
}

// timing function
bool CorrectToneFilter::updateNoteInterval() {
    switch (now_rhythm_) {
        case ScoreReader::kRhythmNormal:
            note_interval_ = beat_ms_;
            trace_printf("CorrectToneFilter: NORMAL(16 NOTE) RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm8Note:
            note_interval_ = beat_ms_ * 2;
            trace_printf("CorrectToneFilter: 8_NOTE RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm4Note:
            note_interval_ = beat_ms_ * 4;
            trace_printf("CorrectToneFilter: 4_NOTE RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm1NoteTriplets:
            note_interval_ = triplets_beat_ms_ * 8;
            trace_printf("CorrectToneFilter: 2 NOTE TRIPLETS RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm2NoteTriplets:
            note_interval_ = triplets_beat_ms_ * 4;
            trace_printf("CorrectToneFilter: 4 NOTE TRIPLETS RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm4NoteTriplets:
            note_interval_ = triplets_beat_ms_ * 2;
            trace_printf("CorrectToneFilter: 8 NOTE TRIPLETS RHYTHM:%d\n", note_interval_);
            break;
        case ScoreReader::kRhythm8NoteTriplets:
            note_interval_ = triplets_beat_ms_;
            trace_printf("CorrectToneFilter: 16 NOTE TRIPLETS RHYTHM:%d\n", note_interval_);
            break;
    }
    return true;
}

void CorrectToneFilter::setRhythm(int new_rhythm) {
    now_rhythm_ = new_rhythm;
    updateNoteInterval();
}

void CorrectToneFilter::calcNoteInterval() {
    beat_ms_ = (int)(60000 / bpm_);
    beat_ms_ = beat_ms_ / 4;  // 16分音符の時間

    triplets_beat_ms_ = (beat_ms_ * 2) / 3;  //半拍3連 (24分音符)

    updateNoteInterval();
}

void CorrectToneFilter::setBpm(int new_bpm) {
    bpm_ = new_bpm;
    calcNoteInterval();

    trace_printf("CorrectToneFilter: BPM = %d\n", bpm_);
    trace_printf("CorrectToneFilter: 1 Beat milliseconds = %d\n", beat_ms_);
    trace_printf("CorrectToneFilter: 4拍3連 = %d, ", triplets_beat_ms_ * 8);
    trace_printf("CorrectToneFilter: 2拍3連 = %d, ", triplets_beat_ms_ * 4);
    trace_printf("CorrectToneFilter: 1拍3連 = %d, ", triplets_beat_ms_ * 2);
    trace_printf("CorrectToneFilter: 1/2拍3連 = %d\n", triplets_beat_ms_);
}

#endif  // ARDUINO_ARCH_SPRESENSE
