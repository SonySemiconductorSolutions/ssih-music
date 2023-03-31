/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file TextScoreParser.h
 */
#ifndef TEXT_SCORE_PARSER_H_
#define TEXT_SCORE_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "ScoreParser.h"

/**
 * @brief @~japanese 独自の楽譜ファイルフォーマットの ScoreParser です。
 * @code {.txt}
 *                                     // {1行コメント}
 * #MUSIC_TITLE: {曲名(1曲目)}
 * #MUSIC_BPM: {テンポ(1曲目)}
 * #MUSIC_RHYTHM: {1音符の長さ(1曲目)} // 0: 4分音符, 1: 8分音符, 2: 16分音符, 3: 4拍3連符, 4: 2拍3連符, 5: 1拍3連符, 6: 半拍3連符
 * #MUSIC_START                        // 曲の開始
 * 60,62,64,-;                         // "ドレミ休"
 *                                     // 1行に1小節分の音符を "," で区切って並べ、小節の終わりの ";" を書く。
 *                                     // ノート番号を書くと音符、"-" を書くと休符、何も書かないと前の音を続ける。
 * 60,,-,;                            // "ドー休ー"
 * #MUSIC_END                          // 曲の終了
 *
 * #MUSIC_TITLE: {曲名(2曲目)}
 * #MUSIC_BPM: {テンポ(2曲目)}
 * #MUSIC_RHYTHM: {1音符の長さ(2曲目)}
 * #MUSIC_START                        // 曲の開始
 * #BPMCHANGE {テンポ}                 // 曲の途中でテンポを変える
 * #RHYTHMCHANGE {1音符の長さ}         // 曲の途中で1音符の長さを変える
 * #DELAY {ミリ秒}                     // 曲の途中で楽譜の進行を止める
 * #MUSIC_END                          // 曲の終了
 * @endcode
 */
class TextScoreParser : public ScoreParser {
public:
    enum Rhythm {
        kRhythm4Note = 0,
        kRhythm8Note,
        kRhythm16Note,
        kRhythm1NoteTriplets,
        kRhythm2NoteTriplets,
        kRhythm4NoteTriplets,
        kRhythm8NoteTriplets,
    };

    struct Music {
        size_t offset;
        int tick;
        String title;
        int tempo;
        int tone;
        int rhythm;
    };

    /**
     * @brief @~japanese TextScoreParser オブジェクトを生成します。
     * @param[in] path @~japanese 独自の楽譜ファイルフォーマット(.txt)のパスを指定します。
     */
    TextScoreParser(const String& path);

    virtual ~TextScoreParser();

    uint16_t getRootTick() override;
    String getFileName() override;
    int getNumberOfScores() override;
    bool loadScore(int id) override;
    String getTitle(int id) override;
    bool getMidiMessage(ScoreParser::MidiMessage* msg) override;

private:
    std::vector<Music> musics_;
    File file_;
    bool is_end_of_music_;
    int tempo_;
    int tone_;
    int rhythm_;

    int note_;
    int duration_;
};

#endif  // TEXT_SCORE_PARSER_H_
