/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file ScoreParser.h
 */
#ifndef SCORE_PARSER_H_
#define SCORE_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

#include "YuruInstrumentFilter.h"

/**
 * @brief @~japanese スタンダードMIDIファイルなどの楽譜ファイルを解析し、MIDIメッセージを出力します。
 * @details @~japanese スタンダードMIDIファイルは基本的に1ファイル1曲が想定されていますが、 ScoreParser にその制約はありません。
 * 1曲分のMIDIメッセージを読み出す手順は、(1) ファイルを与えて、(2) 何曲目を処理するかを指定して、(3) MIDIメッセージを読み出すとなります。
 * @code {.cpp}
 * #include <ParserFactory.h>
 * #include <ScoreParser.h>
 * 
 * void begin() {
 *     ParserFactory factory;
 *     ScoreParser *parser = factory.getScoreParser(score_path);   // (1)
 *     if (parser && parser->getNumberOfScores() > 0) {
 *        parser->loadScore(0);                                    // (2)
 *         ScoreParser::MidiMessage msg;
 *         while (parser->getMidiMessage(&msg)) {                  // (3)
 *             if ((msg.status_byte & 0xF0) == 0x80) {
 *                 Serial.println("Note off");
 *             } else if ((msg.status_byte & 0xF0) == 0x90) {
 *                 Serial.println("Note on");
 *             }
 *         }
 *         delete parser;
 *     }
 * }
 * 
 * void loop() {
 * }
 * @endcode
 * @see ParserFactory
 */
class ScoreParser {
public:
    static const unsigned kSysExMaxSize = 128;
    /**
     * @brief @~japanese MIDIメッセージが格納される構造体です。
     */
    struct MidiMessage {
        uint32_t delta_time;
        uint8_t status_byte;
        uint8_t data_byte1;
        uint8_t data_byte2;
        // for sysex event and meta-event
        uint8_t event_code;
        uint32_t event_length;
        uint8_t sysex_array[kSysExMaxSize];
    };

    ScoreParser();

    virtual ~ScoreParser();

    /**
     * @brief @~japanese 処理中の曲のdivision値を取得します。
     */
    virtual uint16_t getRootTick() = 0;

    /**
     * @brief @~japanese 楽譜のファイル名を取得します。
     */
    virtual String getFileName() = 0;

    /**
     * @brief @~japanese 処理できる曲数を取得します。
     * @see ScoreParser::loadScore()
     */
    virtual int getNumberOfScores() = 0;

    /**
     * @brief @~japanese 処理対象の曲を選択します。idに指定する値は ScoreParser::getNumberOfScores() で得られた値未満でなければなりません。
     * @param[in] id @~japanese 曲番号
     * @retval true Success
     * @retval false Fail
     */
    virtual bool loadScore(int id) = 0;

    /**
     * @brief @~japanese 曲名を取得します。
     * @param[in] id @~japanese 曲番号
     * @return @~japanese 曲名
     */
    virtual String getTitle(int id) = 0;

    /**
     * @brief @~japanese 指定したトラックを処理対象に追加します。
     * @param[in] value @~japanese トラック番号 (0オリジン, 最大値=31)
     * @retval true Success
     * @retval false Fail
     */
    bool setEnableTrack(uint16_t value);

    /**
     * @brief @~japanese 指定したトラックを処理対象から削除します。
     * @param[in] value @~japanese トラック番号 (0オリジン, 最大値=31)
     * @retval true Success
     * @retval false Fail
     */
    bool setDisableTrack(uint16_t value);

    /**
     * @brief @~japanese 処理対象トラックのビットマスクを設定します。
     * @details @~japanese ビットマスクのビット0が1のときトラック1が有効、ビット15が1のときトラック16が有効です。
     * 値を0にするとすべてのトラックが処理費対象となります。
     * @param[in] mask
     * @return true Success
     * @return false Fail
     */
    bool setPlayTrack(uint32_t mask);

    /**
     * @brief @~japanese 処理対象トラックのビットマスクを取得します。
     */
    uint32_t getPlayTrack();

    /**
     * @brief @~japanese 処理中の曲から次に実行するMIDIメッセージを取得します。
     * @param[out] midi_message @~japanese MIDIメッセージ
     * @retval true @~japanese 処理すべきMIDIメッセージがまだある
     * @retval false @~japanese 処理すべきMIDIメッセージがもうない
     */
    virtual bool getMidiMessage(MidiMessage* midi_message) = 0;

private:
    uint32_t play_track_flags_;
};

#endif  // SCORE_PARSER_H_
