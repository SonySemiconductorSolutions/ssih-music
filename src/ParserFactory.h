/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef PARSER_FACTORY_H_
#define PARSER_FACTORY_H_

#include <vector>

#include <File.h>

#include "ScoreParser.h"

class ParserFactory {
public:
    ParserFactory();
    ~ParserFactory();

    ScoreParser* getScoreParser(const String& path);

private:
    String playlist_path_;
    bool writePlaylist(const String& file_name);
    bool createPlaylist(const String& path);
};

#endif  // PARSER_FACTORY_H_
