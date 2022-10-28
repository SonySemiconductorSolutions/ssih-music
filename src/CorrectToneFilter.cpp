/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "CorrectToneFilter.h"

#include "path_util.h"
#include "ScoreSrc.h"
#include "SmfParser.h"
#include "TextScoreParser.h"
#include "ToneFilter.h"

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

static const char kClassName[] = "CorrectToneFilter";

const int kDefaultTick = 960;
const int kDefaultTempo = (int)60000000 / 120;

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
void CorrectToneFilter::setScore(int score_id) {
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

int CorrectToneFilter::getScoreNum() {
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

CorrectToneFilter::CorrectToneFilter(const String& file_name, Filter& filter) : CorrectToneFilter(file_name, false, filter) {
}

CorrectToneFilter::CorrectToneFilter(const String& file_name, bool auto_start, Filter& filter)
    : BaseFilter(filter),
      parser_(nullptr),
      directory_name_(file_name),
      registered_midi_msg_(),
      playing_midi_msg_(),
      is_end_track_(false),
      is_note_playing_(false),
      is_music_start_(true),
      score_num_(0),
      root_tick_(kDefaultTick),
      now_tempo_(kDefaultTempo),
      duration_(0),
      play_state_(auto_start ? CorrectToneFilter::PLAYING : CorrectToneFilter::PAUSE),
      default_state_(auto_start ? CorrectToneFilter::PLAYING : CorrectToneFilter::PAUSE),
      schedule_time_(0),
      is_waiting_(false) {
}

CorrectToneFilter::~CorrectToneFilter() {
    if (parser_) {
        delete parser_;
        parser_ = nullptr;
    }
}

bool CorrectToneFilter::setParam(int param_id, intptr_t value) {
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return false;
    } else if (param_id == ScoreSrc::PARAMID_SCORE) {
        setScore((unsigned int)value);
        return true;
    } else if (param_id == ScoreSrc::PARAMID_SCORE_NAME) {
        return false;
    } else if (param_id == ScoreSrc::PARAMID_STATUS) {
        if (!(play_state_ == ScoreSrc::END_SCORE)) {
            play_state_ = value;
        }
        return true;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

intptr_t CorrectToneFilter::getParam(int param_id) {
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
    } else if (param_id == ScoreSrc::PARAMID_STATUS) {
        return play_state_;
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool CorrectToneFilter::isAvailable(int param_id) {
    if (param_id == ScoreSrc::PARAMID_NUMBER_OF_SCORES) {
        return true;
    } else if (param_id == ScoreSrc::PARAMID_SCORE) {
        return true;
    } else if (param_id == ScoreSrc::PARAMID_SCORE_NAME) {
        return false;
    } else if (param_id == ScoreSrc::PARAMID_STATUS) {
        return true;
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

bool CorrectToneFilter::begin() {
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
bool CorrectToneFilter::readDirectoryScores(const String& dir_name) {
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

bool CorrectToneFilter::readScore(File& file) {
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

void CorrectToneFilter::selectScore(int id) {
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
    root_tick_ = parser_->getRootTick();
    is_end_track_ = false;
    is_music_start_ = true;
    memset(&playing_midi_msg_, 0x00, sizeof(playing_midi_msg_));
    memset(&registered_midi_msg_, 0x00, sizeof(registered_midi_msg_));
    memset(&midi_message_, 0x00, sizeof(midi_message_));
    schedule_time_ = 0;
    duration_ = 0;
    is_waiting_ = false;
}

void CorrectToneFilter::update() {
    if (parser_ == nullptr) {
        error_printf("[%s::%s] error: parser isnot available\n", kClassName, __func__);
    } else if (parser_->getNumberOfScores() > 0) {
        registerNote();
    }
    if (is_note_playing_) {
        changePlayingNote();
    }
    BaseFilter::update();
}

bool CorrectToneFilter::registerNote() {
    if (play_state_ == CorrectToneFilter::Status::END_SCORE) {
        return false;
    }
    if (is_music_start_) {
        schedule_time_ = millis();
        is_music_start_ = false;
    }
    if (parser_ != nullptr) {
        if (!is_waiting_) {
            parser_->getMidiMessage(&midi_message_);
            debug_printf("[%s::%s] delta_time:%d, ", kClassName, __func__, midi_message_.delta_time);
            debug_printf("status_byte:%02x, data_byte1:%02x, data_byte2:%02x, ", midi_message_.status_byte, midi_message_.data_byte1, midi_message_.data_byte2);
            debug_printf("event_code:%02x, event_length:%02x\n", midi_message_.event_code, midi_message_.event_length);
            duration_ = (unsigned long)(((midi_message_.delta_time * now_tempo_) / root_tick_) / 1000);
            schedule_time_ += duration_;
            debug_printf("[%s::%s] duration_:%d = dt:%d * tempo_:%d / tick_:%d / 1000\n", kClassName, __func__, (int)duration_, midi_message_.delta_time,
                         now_tempo_, root_tick_);
            is_waiting_ = true;
        }
        if (default_state_ != play_state_) {
            trace_printf("[%s::%s] old stat:%d, now stat:%d\n", kClassName, __func__, default_state_, play_state_);
            if (play_state_ == CorrectToneFilter::PAUSE) {
                duration_ = schedule_time_ - millis();  //残時間の取得
                uint8_t ch = playing_midi_msg_.status_byte & 0x0f;
                BaseFilter::sendNoteOff(playing_midi_msg_.data_byte1, playing_midi_msg_.data_byte2, ch);
            } else if (play_state_ == CorrectToneFilter::PLAYING) {
                schedule_time_ = millis() + duration_;  //残時間の再生開始
            }
            default_state_ = play_state_;
        } else if (schedule_time_ <= millis() && play_state_ == PLAYING) {
            if (midi_message_.status_byte == ScoreParser::kMetaEvent) {  // メタイベントの処理
                executeMetaEvent(midi_message_);
            } else if (midi_message_.status_byte == ScoreParser::kSysExEvent) {  // SysExイベントの処理
                // executeSysExEvent();
            } else {  // MIDIイベントの処理
                executeMIDIEvent(midi_message_);
            }
            memset(&midi_message_, 0x00, sizeof(midi_message_));
            is_waiting_ = false;
        }
    }
    return true;
}

bool CorrectToneFilter::executeMetaEvent(const ScoreParser::MidiMessage& midi_message) {
    int data = 0;                                             //他のメタイベント対応する場合にも使いまわしたい
    if (midi_message.event_code == ScoreParser::kSetTempo) {  //テンポ変更
        for (unsigned int i = 0; i < midi_message.event_length; i++) {
            data = data << 8;
            data += midi_message.sysex_array[i];
        }
        now_tempo_ = data;
        debug_printf("[%s::%s] now_tempo_:%d\n", kClassName, __func__, now_tempo_);
    } else if (midi_message.event_code == ScoreParser::kEndOfTrack) {  //楽譜終了
        play_state_ = CorrectToneFilter::END_SCORE;
    } else if (midi_message.event_code < 0x80) {
        debug_printf("[%s::%s] %02x is unused Meta event.\n", kClassName, __func__, midi_message.event_code);
    } else {
        error_printf("[%s::%s] error: This Meta event is not supported.\n", kClassName, __func__);
        return false;
    }
    return true;
}

bool CorrectToneFilter::executeMIDIEvent(const ScoreParser::MidiMessage& midi_message) {
    uint8_t status = midi_message.status_byte & 0xf0;
    if (status == ScoreParser::kNoteOff) {  // Note off
        registered_midi_msg_ = midi_message;
    } else if (status == ScoreParser::kNoteOn) {  // Note on
        registered_midi_msg_ = midi_message;
    } else if (status == ScoreParser::kPolyphonicKeyPressure) {  // Polyphonic Key Pressure
        debug_printf("[%s::%s] Polyphonic Key Pressure is unused Meta event.\n", kClassName, __func__);
    } else if (status == ScoreParser::kControlChange) {  // Control Change
        debug_printf("[%s::%s] Control Change is unused Meta event.\n", kClassName, __func__);
    } else if (status == ScoreParser::kProgramChange) {  // Program Change
        debug_printf("[%s::%s] Program Change is unused Meta event.\n", kClassName, __func__);
    } else if (status == ScoreParser::kChannelPressure) {  // Channel Pressure
        debug_printf("[%s::%s] Channel Pressure is unused Meta event.\n", kClassName, __func__);
    } else if (status == ScoreParser::kPitchBendChange) {  // Pitch Bend Change
        debug_printf("[%s::%s] Pitch Bend Change is unused Meta event.\n", kClassName, __func__);
    }
    return true;
}

bool CorrectToneFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    playNote();
    return true;
}

void CorrectToneFilter::playNote() {
    if (!is_note_playing_) {  //ノート停止状態
        uint8_t status = registered_midi_msg_.status_byte & 0xf0;
        if (status == ScoreParser::kNoteOn) {  //有効ノート
            uint8_t ch = registered_midi_msg_.status_byte & 0x0f;
            BaseFilter::sendNoteOn(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, ch);
        }
        playing_midi_msg_ = registered_midi_msg_;  //再生中のノートを保持
        is_note_playing_ = true;                   //ノート再生状態
    } else {                                       //ノート再生状態
        changePlayingNote();
    }
}

bool CorrectToneFilter::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    stopNote(velocity, channel);
    return true;
}

void CorrectToneFilter::stopNote(uint8_t velocity, uint8_t channel) {
    if (is_note_playing_) {  //ノート再生状態
        if (isMIDINum(playing_midi_msg_.data_byte1)) {
            uint8_t ch = playing_midi_msg_.status_byte & 0x0f;
            BaseFilter::sendNoteOff(playing_midi_msg_.data_byte1, playing_midi_msg_.data_byte2, ch);
        }
        is_note_playing_ = false;  //ノート停止状態
    }
}

void CorrectToneFilter::changePlayingNote() {
    if (playing_midi_msg_.status_byte != registered_midi_msg_.status_byte || playing_midi_msg_.data_byte1 != registered_midi_msg_.data_byte1) {
        uint8_t playing_status = registered_midi_msg_.status_byte & 0xf0;
        if (isMIDINum(playing_midi_msg_.data_byte1) && playing_status == ScoreParser::kNoteOn) {
            uint8_t ch = playing_midi_msg_.status_byte & 0x0f;
            BaseFilter::sendNoteOff(playing_midi_msg_.data_byte1, playing_midi_msg_.data_byte2, ch);
        }
        if (isMIDINum(registered_midi_msg_.data_byte1)) {  //有効ノート
            int8_t regist_ch = registered_midi_msg_.status_byte & 0x0f;
            uint8_t regist_status = registered_midi_msg_.status_byte & 0xf0;
            if (regist_status == ScoreParser::kNoteOn && play_state_ == PLAYING) {
                BaseFilter::sendNoteOn(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, regist_ch);
            } else if (regist_status == ScoreParser::kNoteOff) {
                BaseFilter::sendNoteOff(registered_midi_msg_.data_byte1, registered_midi_msg_.data_byte2, regist_ch);
            }
        }
        playing_midi_msg_ = registered_midi_msg_;  //再生中のノートを保持
    }
}

bool CorrectToneFilter::isMIDINum(int note) {
    return (NOTE_NUMBER_MIN <= note && note <= NOTE_NUMBER_MAX);
}

#endif  // ARDUINO_ARCH_SPRESENSE
