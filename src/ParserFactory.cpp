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

// #define DEBUG

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#if defined(DEBUG)
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // if defined(DEBUG)

static const char kClassName[] = "ParserFactory";
const String kPlaylistName = ".playlist.m3u";

enum ScoreFileType { kScoreFileTypeTxt = 0, kScoreFileTypeMidi, kScoreFileTypePlaylist, kScoreFileTypeOthers, kScoreFileTypeHidden };

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
    } else if (ext == ".m3u") {
        return kScoreFileTypePlaylist;
    } else {
        return kScoreFileTypeOthers;
    }
}

ParserFactory::ParserFactory() {
}

ParserFactory::~ParserFactory() {
}

ScoreParser* ParserFactory::getScoreParser(const String& path) {
    ScoreFileType type = getFileType(path);
    debug_printf("[%s::%s]: path:%s\n", kClassName, __func__, path.c_str());
    SDClass sd;
    // File Open
    if (!sd.begin()) {
        error_printf("[%s::%s]: SD begin error.\n", kClassName, __func__);
        return nullptr;
    }
    File dir = sd.open(path.c_str());
    if (dir.isDirectory()) {
        if (createPlaylist(path)) {
            debug_printf("[%s::%s]: Directory\n", kClassName, __func__);
            ScoreParser* parser = new PlaylistParser(playlist_path_.c_str());
            dir.close();
            return parser;
        }
    } else if (type == kScoreFileTypeMidi) {
        ScoreParser* parser = new SmfParser(path);
        debug_printf("[%s::%s]: MIDI\n", kClassName, __func__);
        dir.close();
        return parser;
    } else if (type == kScoreFileTypeTxt) {
        ScoreParser* parser = new TextScoreParser(path);
        debug_printf("[%s::%s]: Text\n", kClassName, __func__);
        dir.close();
        return parser;
    } else if (type == kScoreFileTypePlaylist) {
        ScoreParser* parser = new PlaylistParser(path);
        debug_printf("[%s::%s]: Playlist\n", kClassName, __func__);
        dir.close();
        return parser;
    }
    dir.close();

    error_printf("[%s::%s] SD: file:%s This file is not supported.\n", kClassName, __func__, path.c_str());
    return nullptr;
}

// score list analysis
bool ParserFactory::createPlaylist(const String& path) {
    SDClass sd;
    // File Open
    if (!sd.begin()) {
        error_printf("[%s::%s]: SD begin error.\n", kClassName, __func__);
        return false;
    }
    File dir = sd.open(path.c_str());
    if (!dir) {  // Read File (failed)
        error_printf("[%s::%s] error:%s open error.\n", kClassName, __func__, path.c_str());
        return false;
    }

    if (dir.isDirectory()) {  // If the target is a directory
        playlist_path_ = path + "/" + kPlaylistName;
        if (sd.exists(playlist_path_.c_str())) {
            sd.remove(playlist_path_.c_str());
        }
        File playlist_file = sd.open(playlist_path_.c_str(), FILE_WRITE);
        debug_printf("[%s::%s] (Directory)\n", kClassName, __func__);
        while (true) {
            File file = dir.openNextFile();
            // File Read Confirmation
            if (!file) {
                break;
            }
            if (file.isDirectory()) {  // Ignore retrieved files if they are directories
                continue;
            }
            String file_name = file.name();
            ScoreFileType type = getFileType(file_name);
            if (type == kScoreFileTypeMidi || type == kScoreFileTypeTxt) {
#ifdef EMMC_USE
                String kSDFullPath = "/mnt/emmc/";
#else
                String kSDFullPath = "/mnt/sd0/";
#endif
                // Convert file name to full path - > current path
                String sd_current_path;
                if (file_name.startsWith(kSDFullPath)) {
                    sd_current_path = file_name.substring(kSDFullPath.length());
                } else {
                    sd_current_path = file_name;
                }

                String folder_path = getFolderPath(sd_current_path);
                // Convert to current path from file
                String file_current_path = sd_current_path.substring(folder_path.length());
                debug_printf("[%s::%s]:current \"%s\"\n", kClassName, __func__, file_current_path.c_str());
                playlist_file.println(file_current_path.c_str());
            } else if (type == kScoreFileTypeOthers) {
                error_printf("[%s::%s] error:%s This file is not supported.\n", kClassName, __func__, file_name.c_str());
            }
            file.close();
        }
        playlist_file.close();
    }

    return true;
}

#endif  // ARDUINO_ARCH_SPRESENSE
