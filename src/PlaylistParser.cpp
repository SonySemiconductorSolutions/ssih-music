/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ParserFactory.h"
#include "path_util.h"
#include "PlaylistParser.h"
#include "SmfParser.h"
#include "TextScoreParser.h"

// #define DEBUG (1)
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

static const char kClassName[] = "PlaylistParser";

PlaylistParser::PlaylistParser(const String& path) : parser_(nullptr) {
    SDClass sd;
    // File Open
    if (!sd.begin()) {
        error_printf("[%s::%s]: SD begin error.\n", kClassName, __func__);
    } else {
        File file = sd.open(path);
        String current_dir = getFolderPath(path);
        trace_printf("[%s::%s]:current \"%s\"\n", kClassName, __func__, current_dir.c_str());

        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            debug_printf("[%s::%s]:score file name \"%s\"\n", kClassName, __func__, line.c_str());
            if (line == "") {
                continue;
            } else if (line.startsWith("#")) {
                continue;
            }

            String score_path = joinPath(current_dir, line);
            debug_printf("[%s::%s]:score path \"%s\"\n", kClassName, __func__, score_path.c_str());

            String normalize_path = normalizePath(score_path);
            debug_printf("[%s::%s]:this folder path:%s\n", kClassName, __func__, normalize_path.c_str());

            if (normalize_path.indexOf('/') == 0) {
                normalize_path = normalize_path.substring(1);
            }
            ParserFactory factory;
            ScoreParser* parser = factory.getScoreParser(normalize_path);
            if (parser == nullptr) {
                continue;
            }

            for (int i = 0; i < parser->getNumberOfScores(); i++) {
                PlaylistData data;
                data.file_name = normalize_path;
                data.index = i;
                data.title = parser->getTitle(i);
                playlist_data_.push_back(data);
            }

            delete parser;
        }
    }
    for (size_t i = 0; i < playlist_data_.size(); i++) {
        debug_printf("[%s::%s]:", kClassName, __func__);
        debug_printf(" index:%d,", playlist_data_[i].index);
        debug_printf(" file name:%s,", playlist_data_[i].file_name.c_str());
        debug_printf(" title:%s\n", playlist_data_[i].title.c_str());
    }
}

PlaylistParser::~PlaylistParser() {
    if (parser_ != nullptr) {
        delete parser_;
    }
}

uint16_t PlaylistParser::getRootTick() {
    if (parser_ == nullptr) {
        error_printf("[%s::%s]: Score is not loading.\n", kClassName, __func__);
        return 0;
    }
    return parser_->getRootTick();
}

String PlaylistParser::getFileName() {
    return "";
}

int PlaylistParser::getNumberOfScores() {
    return playlist_data_.size();
}

bool PlaylistParser::loadScore(int index) {
    if (index < 0 || getNumberOfScores() <= index) {
        error_printf("[%s::%s]Error: Out of index.\n", kClassName, __func__);
        return false;
    }
    if (parser_ != nullptr) {
        delete parser_;
    }
    debug_printf("[%s::%s]:file name:%s\n", kClassName, __func__, playlist_data_[index].file_name.c_str());
    ParserFactory factory;
    parser_ = factory.getScoreParser(playlist_data_[index].file_name);
    debug_printf("[%s::%s]:index:%d, Title:%s, file name:%s, RootTick:%d\n", kClassName, __func__, index,
                 parser_->getTitle(playlist_data_[index].index).c_str(), parser_->getFileName().c_str(), parser_->getRootTick());
    // parser assignment tracks flag
    parser_->setPlayTrack(getPlayTrack());
    return parser_->loadScore(playlist_data_[index].index);
}

String PlaylistParser::getTitle(int index) {
    if (index < 0 || getNumberOfScores() <= index) {
        error_printf("[%s::%s]Error: Out of index.\n", kClassName, __func__);
        return "";
    }
    if (parser_ == nullptr) {
        error_printf("[%s::%s]: Score is not loading.\n", kClassName, __func__);
        return "";
    }
    return playlist_data_[index].title;
}

bool PlaylistParser::getMidiMessage(ScoreParser::MidiMessage* midi_message) {
    if (parser_ == nullptr) {
        error_printf("[%s::%s]: Score is not loading.\n", kClassName, __func__);
        return false;
    }
    return parser_->getMidiMessage(midi_message);
}

#endif  // ARDUINO_ARCH_SPRESENSE
