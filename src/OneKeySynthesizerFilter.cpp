/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "OneKeySynthesizerFilter.h"

#include "path_util.h"
#include "ScoreSrc.h"
#include "SmfParser.h"
#include "TextScoreParser.h"
#include "ToneFilter.h"

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

enum ScoreFileType { kScoreFileTypeTxt = 0, kScoreFileTypeMidi, kScoreFileTypeOthers, kScoreFileTypeHidden };

static ScoreFileType getFileType(const String& path) {
    if (isHidden(path)) {
        return kScoreFileTypeHidden;
    }
    String ext = getExtension(path);
    ext.toLowerCase();
    if (ext == ".mid" || ext == ".midi") {
        return kScoreFileTypeMidi;
    } else if (ext == ".txt") {
        return kScoreFileTypeTxt;
    } else {
        return kScoreFileTypeOthers;
    }
}

// others getter setter
void OneKeySynthesizerFilter::setScore(int score_id) {
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: score was not loaded\n", kClassName, __func__);
        return;
    }
    if (parser_->getNumberOfScores() == 0) {
        error_printf("[%s::%s] error: cannot find track\n", kClassName, __func__);
        return;
    }
    if (0 <= score_id && score_id < parser_->getNumberOfScores()) {
        score_num_ = score_id;
        selectScore(score_num_);
    }
}

int OneKeySynthesizerFilter::getScoreNum() {
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: score was not loaded\n", kClassName, __func__);
        return -1;
    }
    if (parser_->getNumberOfScores() == 0) {
        error_printf("[%s::%s] error: not play track\n", kClassName, __func__);
        return -1;
    }
    if (0 <= score_num_ && score_num_ < parser_->getNumberOfScores()) {
        return score_num_;
    }
    return -1;
}

OneKeySynthesizerFilter::OneKeySynthesizerFilter(const String& file_name, Filter& filter)
    : BaseFilter(filter), directory_name_(file_name), score_num_(0), is_note_registered_(false), is_note_playing_(false), is_end_track_(false) {
}

OneKeySynthesizerFilter::~OneKeySynthesizerFilter() {
    if (parser_) {
        delete parser_;
        parser_ = nullptr;
    }
}

bool OneKeySynthesizerFilter::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return false;
    } else if (param_id == ScoreSrc::PARAMID_SCORE) {
        setScore((int)value);
        return true;
    } else if (param_id == ScoreSrc::PARAMID_SCORE_NAME) {
        return false;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

intptr_t OneKeySynthesizerFilter::getParam(int param_id) {
    static char error_message[] = "Score Not Found";
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        if (parser_ == nullptr) {
            return 0;
        } else {
            return parser_->getNumberOfScores();
        }
    } else if (param_id == ScoreSrc::PARAMID_SCORE) {
        return getScoreNum();
    } else if (param_id == ScoreSrc::PARAMID_SCORE_NAME) {
        if (parser_ == nullptr) {
            return (intptr_t)error_message;
        } else if (parser_->getNumberOfScores() > 0) {
            return (intptr_t)parser_->getFileName().c_str();
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
    } else if (param_id == ScoreSrc::PARAMID_SCORE) {
        return true;
    } else if (param_id == ScoreSrc::PARAMID_SCORE_NAME) {
        return false;
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

bool OneKeySynthesizerFilter::begin() {
    readDirectoryScores(directory_name_);
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
    } else if (parser_->getNumberOfScores() > 0) {
        selectScore(getScoreNum());
    } else {
        error_printf("[%s::%s] error: Score not found.\n", kClassName, __func__);
    }

    return BaseFilter::begin();
}

//楽譜一覧解析
bool OneKeySynthesizerFilter::readDirectoryScores(const String& dir_name) {
    SDClass sd;
    //ファイルオープン
    if (!sd.begin()) {
        error_printf("[%s::%s] SD: %s begin error.\n", kClassName, __func__, dir_name.c_str());
        return false;
    }
    File dir = sd.open(dir_name.c_str());
    if (!dir) {  //ファイル読み込み(失敗)
        error_printf("[%s::%s] SD: %s open error.\n", kClassName, __func__, dir_name.c_str());
        return false;
    }

    debug_printf("[%s::%s] SD: %s open success.\n", kClassName, __func__, dir_name.c_str());
    if (dir.isDirectory()) {  // 対象がディレクトリの場合
        debug_printf("[%s::%s] (Directory)\n", kClassName, __func__);
        while (true) {
            File file = dir.openNextFile();
            file.setTimeout(0);
            //ファイルの読み込み確認
            if (!file) {
                break;
            }
            if (file.isDirectory()) {  //取得したファイルがディレクトリの場合は無視する
                file.close();
                continue;
            }
            readScore(file);
            file.close();
        }
    } else {  //対象がファイルの場合
        debug_printf("[%s::%s] (file)\n", kClassName, __func__);
        dir.setTimeout(0);
        readScore(dir);
        dir.close();
    }

    return true;
}

bool OneKeySynthesizerFilter::readScore(File& file) {
    debug_printf("[%s::%s] SD: file:%s open success.\n", kClassName, __func__, file.name());
    if (parser_) {
        delete parser_;
        parser_ = nullptr;
    }
    ScoreFileType type = getFileType(file.name());
    if (type == kScoreFileTypeMidi) {
        parser_ = new SmfParser(file.name());
        return true;
    } else if (type == kScoreFileTypeTxt) {
        parser_ = new TextScoreParser(file.name());
        return true;
    }
    error_printf("[%s::%s] SD: file:%s This file is not supported.\n", kClassName, __func__, file.name());
    return false;
}

void OneKeySynthesizerFilter::selectScore(int id) {
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return;
    } else if (parser_->getNumberOfScores() == 0) {
        error_printf("[%s::%s] error: cannot find playable track\n", kClassName, __func__);
        return;
    } else if (!(0 <= id && id < parser_->getNumberOfScores())) {
        error_printf("[%s::%s] error: out of track number\n", kClassName, __func__);
        return;
    }
    parser_->loadScore(id);
    is_end_track_ = false;
    is_note_registered_ = false;
    is_note_playing_ = false;
    memset(&registered_midi_msg_, 0x00, sizeof(registered_midi_msg_));
}

bool OneKeySynthesizerFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return false;
    }
    if (is_end_track_) {
        debug_printf("[%s::%s] end of track.\n", kClassName, __func__);
        return true;
    }
    if (parser_->getNumberOfScores() > 0) {
        registerNote();
    }
    if (is_note_registered_) {    //ノート登録状態
        if (!is_note_playing_) {  //ノート停止状態
            uint8_t ch = registered_midi_msg_.status_byte & 0x0f;
            BaseFilter::sendNoteOn(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, ch);
            is_note_playing_ = true;  //ノート再生状態
        }
    }
    return true;
}

bool OneKeySynthesizerFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (is_note_playing_) {  //ノート再生状態
        uint8_t ch = registered_midi_msg_.status_byte & 0x0f;
        BaseFilter::sendNoteOff(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, ch);
        is_note_registered_ = false;  //ノート未登録状態
        is_note_playing_ = false;     //ノート停止状態
    }
    return true;
}

void OneKeySynthesizerFilter::registerNote() {
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
        return;
    }
    if (!is_note_registered_) {  //ノート未登録状態
        while (true) {
            ScoreParser::MidiMessage midi_message;
            memset(&midi_message, 0x00, sizeof(midi_message));
            parser_->getMidiMessage(&midi_message);
            debug_printf("[%s::%s] delta_time:%d, ", kClassName, __func__, midi_message.delta_time);
            debug_printf("[%s::%s] status_byte:%02x, data_byte1:%02x, data_byte2:%02x, ", midi_message.status_byte, midi_message.data_byte1,
                         midi_message.data_byte2);
            debug_printf("[%s::%s] event_code:%02x, event_length:%02x\n", midi_message.event_code, midi_message.event_length);

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
