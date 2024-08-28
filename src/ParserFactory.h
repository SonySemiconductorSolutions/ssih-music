/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file ParserFactory.h
 */
#ifndef PARSER_FACTORY_H_
#define PARSER_FACTORY_H_

#include <vector>

#include <File.h>

#include "ScoreParser.h"
#include "YuruInstrumentConfig.h"

/**
 * @brief @~japanese ScoreParser オブジェクトの Factory クラスです。
 */
class ParserFactory {
public:
    ParserFactory();

    ~ParserFactory();

    /**
     * @brief @~japanese ファイル形式に応じた ScoreParser オブジェクトを生成します。
     * @details @~japanese 拡張子ごとに適した ScoreParser オブジェクトを生成します。
     * 生成された ScoreParser オブジェクトは、ユーザーの責任で delete してください。
     * 拡張子が ".mid" は SmfParser オブジェクト、 ".m3u" は PlaylistParser オブジェクト、 ".txt" は TextScoreParser オブジェクトを生成します。
     * フォルダが指定された場合は自動的にプレイリストファイルを自動作成し、これを参照する PlaylistParser オブジェクトを生成します。
     * @param[in] path @~japanese 楽譜ファイル名、または楽譜ファイルを含むフォルダ名
     * @return @~japanese ScoreParser オブジェクト
     */
    ScoreParser* getScoreParser(const String& path);

private:
    String playlist_path_;
    bool createPlaylist(const String& path);
};

#endif  // PARSER_FACTORY_H_
