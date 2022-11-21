/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include "path_util.h"

#include <Arduino.h>

/**
 * @brief Get the Folder Path object
 *
 * @param path "/path/to.file"
 * @return String "/path/"
 */
String getFolderPath(const String& path) {
    int sep1 = path.lastIndexOf("/");
    int sep2 = path.lastIndexOf("\\");
    int sep = (sep1 > sep2) ? sep1 : sep2;
    if (sep < 0) {
        return "";
    } else {
        return path.substring(0, sep + 1);
    }
}

/**
 * @brief Get the Base Name object
 *
 * @param path "/path/to.file"
 * @return String "to.file"
 */
String getBaseName(const String& path) {
    int sep1 = path.lastIndexOf("/");
    int sep2 = path.lastIndexOf("\\");
    int sep = (sep1 > sep2) ? sep1 : sep2;
    if (sep < 0) {
        return path;
    } else {
        return path.substring(sep + 1);
    }
}

/**
 * @brief Get the Extension object
 *
 * @param path "/path/to.file"
 * @return String ".file"
 */
String getExtension(const String& path) {
    String basename = getBaseName(path);
    int sep = path.lastIndexOf(".");
    if (sep < 0) {
        return path;
    } else {
        return path.substring(sep);
    }
}

bool isHidden(const String& path) {
    String basename = getBaseName(path);
    return basename[0] == '.';
}

String normalizePath(const String& path) {
    String output_path = path;
    int dot_index;
    if (output_path.indexOf("./") == 0) {
        output_path = path.substring(2);
    }
    while ((dot_index = output_path.indexOf("/./")) >= 0) {
        String dir = output_path.substring(0, dot_index);
        String remain = output_path.substring(dot_index + 2);
        output_path = dir + remain;
    }
    while ((dot_index = output_path.indexOf("/../")) >= 0) {
        String dir = output_path.substring(0, dot_index);
        int slash_index = dir.lastIndexOf('/');
        String parent = "";
        if (slash_index != -1) {
            parent = dir.substring(0, slash_index);
        }
        String remain = output_path.substring(dot_index + 3);
        output_path = parent + remain;
    }

    return output_path;
}

// joinPath("/path/to.dir", "file") => "/path/to.dir/file"
// joinPath("/path/to.dir", "/path/to.file") => "/path/to.file"
// joinPath("", "path/to.file") => "path/to.file"
// joinPath("/path/to.dir", "") => "/path/to.dir/"
String joinPath(const String& dir, const String& path) {
    if (dir.length() == 0) {
        return path;
    }
    if (path.length() == 0) {
        return dir;
    }
    if (path[0] == '/' || path[0] == '\\') {
        return path;
    }
    char last_ch = dir[dir.length() - 1];
    String joined_path = dir;
    if (last_ch != '/' && last_ch != '\\') {
        joined_path += "/";
    }
    joined_path += path;
    return joined_path;
}
