/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file ScoreFilter.h
 */
#ifndef SCORE_FILTER_H_
#define SCORE_FILTER_H_

#include <vector>

#include "ParserFactory.h"
#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

/**
 * @brief @~japanese 楽譜ファイルを解析し、MIDIイベントを発行する拡張機能
 */
class ScoreFilter : public BaseFilter {
public:
    enum ParamId {  // MAGIC CHAR = 'G'
        /**
         * @brief [get] @~japanese 処理できる曲数です。
         */
        PARAMID_NUMBER_OF_SCORES = ('G' << 8),
        /**
         * @brief [set] @~japanese 指定したトラックを処理対象に追加します。
         */
        PARAMID_ENABLE_TRACK,
        /**
         * @brief [set] @~japanese 指定したトラックを処理対象から削除します。
         */
        PARAMID_DISABLE_TRACK,
        /**
         * @brief [get,set] @~japanese 処理対象トラックのビットマスクです。
         */
        PARAMID_TRACK_MASK,
        /**
         * @brief [get,set] @~japanese 処理中の曲番号です。
         */
        PARAMID_SCORE,
        /**
         * @brief [get] @~japanese 処理中の曲名です。
         */
        PARAMID_SCORE_NAME,
        PARAMID_STATUS
    };

    enum Status { PAUSE = 0, PLAY, END };

    struct Note {
        uint8_t note;
        uint8_t velocity;
        uint8_t channel;
        int stat;
    };

    /**
     * @brief @~japanese ScoreFilter オブジェクトを生成します。
     * @param[in] file_name @~japanese 楽譜ファイル名、または楽譜ファイルを含むフォルダ名を指定します。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    ScoreFilter(const String& file_name, Filter& filter);

    virtual ~ScoreFilter();

    bool begin() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

    bool sendSongPositionPointer(uint16_t beats) override;
    bool sendSongSelect(uint8_t song) override;
    bool sendContinue() override;
    bool sendStop() override;
    bool sendMidiMessage(uint8_t* msg, size_t length) override;

    /**
     * @brief @~japanese Send "MTC Full Message"
     * @param[in] hr @~japanese 時
     * @param[in] mn @~japanese 分
     * @param[in] sc @~japanese 秒
     * @param[in] fr @~japanese フレーム
     * @retval true Success
     * @retval false Fail
     */
    virtual bool sendMtcFullMessage(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr);

    /**
     * @brief @~japanese 楽譜処理が有効かどうかを取得します。
     * @retval true @~japanese 有効
     * @retval false @~japanese 無効
     */
    bool isParserAvailable();

    /**
     * @brief @~japanese 処理できる曲数を取得します。
     * @see ScoreParser::loadScore()
     */
    int getNumberOfScores();

    /**
     * @brief @~japanese 処理中の曲番号を取得します。
     * @return Active score index
     */
    int getScoreIndex();

    /**
     * @brief @~japanese 処理する楽譜ファイルを変更します。
     * @param[in] index @~japanese 処理対象の曲番号を指定します。
     * @retval true Success
     * @retval false Fail
     * @see Filter::sendSongSelect(), ScoreFilter::PARAMID_SCORE
     */
    virtual bool setScoreIndex(int index);

    /**
     * @brief @~japanese 処理中の曲のdivision値を取得します。
     */
    uint16_t getRootTick();

protected:
    /**
     * @brief @~japanese 処理中の曲から次に実行するMIDIメッセージを取得します。
     * @param[out] midi_message @~japanese MIDIメッセージ
     * @retval true @~japanese 処理すべきMIDIメッセージがまだある
     * @retval false @~japanese 処理すべきMIDIメッセージがもうない
     */
    bool getMidiMessage(ScoreParser::MidiMessage* midi_message);

private:
    String file_name_;
    String score_name_;
    ScoreParser* parser_;
    int score_index_;
    uint16_t root_tick_;
    std::vector<Note> playing_notes_;

    uint32_t play_track_flags_;
};

#endif  // SCORE_FILTER_H_
