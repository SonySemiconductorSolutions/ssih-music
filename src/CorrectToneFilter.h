/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file CorrectToneFilter.h
 */
#ifndef CORRECT_TONE_FILTER_H_
#define CORRECT_TONE_FILTER_H_

#include "ScoreFilter.h"
#include "TimeKeeper.h"
#include "YuruInstrumentFilter.h"

/**
 * @brief @~japanese 演奏の音程を楽譜ファイルを使って補正する楽器部品です。
 * @details @~japanese 演奏しようとした音が楽譜から外れているときに、楽譜通りの音に補正します。
 */
class CorrectToneFilter : public ScoreFilter {
public:
    /**
     * @brief Number of notes assignable to a channel
     */
    static const int ASSIGNABLE_SIZE = 7;
    /**
     * @brief Play all notes assigned to a channel
     */
    static const int NOTE_ALL = 127;
    struct Note {
        uint8_t note;
        uint8_t velocity;
    };

    /**
     * @brief @~japanese CorrectToneFilter オブジェクトを生成します。
     * @param[in] file_name @~japanese 楽譜ファイル、または楽譜ファイルを含むフォルダ名を指定します。ファイルを指定する場合、
     * 拡張子は .mid(スタンダードMIDIファイル) 、 .txt(独自形式の楽譜ファイル) 、および .m3u(プレイリストファイル) に対応しています。
     * フォルダまたはプレイリストファイルが指定されたとき、 CorrectToneFilter は複数曲の演奏ができる状態になります。
     * この状態で Filter::setParam() の第1引数に ScoreFilter::PARAMID_SCORE を指定すると、演奏する曲を変更することができます。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    CorrectToneFilter(const String& file_name, Filter& filter);

    ~CorrectToneFilter();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

    bool setScoreIndex(int id) override;

private:
    TimeKeeper time_keeper_;

    int default_state_;
    int play_state_;
    int next_state_;
    bool is_event_available_;
    bool is_music_start_;

    ScoreParser::MidiMessage midi_message_;

    Note assigned_notes_[16][ASSIGNABLE_SIZE];
    Note playing_notes_[16][ASSIGNABLE_SIZE];

    bool updateNotes();
};

#endif  // CORRECT_TONE_FILTER_H_
