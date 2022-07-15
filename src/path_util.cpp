/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include "path_util.h"

#include <Arduino.h>

String getFolderPath(const String& path) {
    int sep1 = path.lastIndexOf("/");
    int sep2 = path.lastIndexOf("\\");
    if (sep1 < 0 && sep2 < 0) {
        return "";
    } else {
        return path.substring(0, ((sep1 > sep2) ? sep1 : sep2) + 1);
    }
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
