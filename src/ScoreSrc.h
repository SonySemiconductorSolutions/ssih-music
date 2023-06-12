/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file ScoreSrc.h
 */
#ifndef SCORE_SRC_H_
#define SCORE_SRC_H_

#include "ScoreFilter.h"
#include "TimeKeeper.h"
#include "YuruInstrumentFilter.h"

/**
 * @brief @~japanese 楽譜ファイルを再生する楽器部品です。
 */
class ScoreSrc : public ScoreFilter {
public:
    enum ParamId {  // MAGIC CHAR = '#'
        /**
         * @brief [get,set] @~japanese 曲の先頭まで達したときに、次の曲を再生するかを設定します。
         */
        PARAMID_CONTINUOUS_PLAYBACK = ('#' << 8)
    };

    /**
     * @brief @~japanese ScoreSrc オブジェクトを生成します。
     * @param[in] file_name @~japanese 楽譜ファイル、または楽譜ファイルを含むフォルダ名を指定します。ファイルを指定する場合、
     * 拡張子は .mid(スタンダードMIDIファイル) 、 .txt(独自形式の楽譜ファイル) 、および .m3u(プレイリストファイル) に対応しています。
     * フォルダまたはプレイリストファイルが指定されたとき、 ScoreSrc は複数曲の演奏ができる状態になります。
     * この状態で Filter::setParam() の第1引数に ScoreFilter::PARAMID_SCORE を指定すると、演奏する曲を変更することができます。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    ScoreSrc(const String& file_name, Filter& filter);

    virtual ~ScoreSrc();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendSongPositionPointer(uint16_t beats) override;
    bool sendSongSelect(uint8_t song) override;
    bool sendContinue() override;
    bool sendStop() override;

    bool sendMtcFullMessage(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr) override;

    bool setScoreIndex(int index) override;

private:
    // schedule
    TimeKeeper time_keeper_;

    // playback
    int default_state_;
    int play_state_;
    int next_state_;
    bool is_continuous_playback_;
    bool is_event_available_;
    bool is_music_start_;

    ScoreParser::MidiMessage midi_message_;

    bool executeMidiEvent(const ScoreParser::MidiMessage& msg);
};

#endif  // SCORE_SRC_H_
