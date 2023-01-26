/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include "ScoreParser.h"

// #define DEBUG (1)

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

static const char kClassName[] = "ScoreParser";
static const int kMaxTrack = 32;

ScoreParser::ScoreParser() : play_track_flags_(~0) {
}

ScoreParser::~ScoreParser() {
}

bool ScoreParser::setEnableTrack(uint16_t value) {
    if (value < kMaxTrack) {
        return setPlayTrack(getPlayTrack() | (1U << value));
    }
    return false;
}

bool ScoreParser::setDisableTrack(uint16_t value) {
    if (value < kMaxTrack) {
        return setPlayTrack(getPlayTrack() & ~(1U << value));
    }
    return false;
}

bool ScoreParser::setPlayTrack(uint32_t mask) {
    play_track_flags_ = mask;
    return true;
}

uint32_t ScoreParser::getPlayTrack() {
    return play_track_flags_;
}
