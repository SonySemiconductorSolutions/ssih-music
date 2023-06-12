/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file OneKeySynthesizerFilter.h
 */
#ifndef ONE_KEY_SYNTHESIZER_FILTER_H_
#define ONE_KEY_SYNTHESIZER_FILTER_H_

#include "ScoreFilter.h"
#include "YuruInstrumentFilter.h"

/**
 * @brief @~japanese ノート・オンメッセージを受けるごとに楽譜ファイルの音を1つずつ演奏する楽器部品です。
 * @details @~japanese 楽譜ファイルを演奏するだけではなく、音を出すタイミングをユーザーにゆだねるような楽器を意図した試作品です。
 */
class OneKeySynthesizerFilter : public ScoreFilter {
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
     * @brief @~japanese OneKeySynthesizerFilter オブジェクトを生成します。
     * @param[in] file_name @~japanese 楽譜ファイル、または楽譜ファイルを含むフォルダ名を指定します。ファイルを指定する場合、
     * 拡張子は .mid(スタンダードMIDIファイル) 、 .txt(独自形式の楽譜ファイル) 、および .m3u(プレイリストファイル) に対応しています。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    OneKeySynthesizerFilter(const String& file_name, Filter& filter);

    ~OneKeySynthesizerFilter();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

    bool setScoreIndex(int id) override;

private:
    int play_state_;

    ScoreParser::MidiMessage midi_message_;

    Note assigned_notes_[16][ASSIGNABLE_SIZE];
    Note playing_notes_[16][ASSIGNABLE_SIZE];

    uint16_t updateNotes();
};

#endif  // ONE_KEY_SYNTHESIZER_FILTER_H_
