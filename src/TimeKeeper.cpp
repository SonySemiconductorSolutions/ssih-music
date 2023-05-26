/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "TimeKeeper.h"

#include <Arduino.h>

#include "midi_util.h"

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

static const char kClassName[] = "TimeKeeper";

static const int kDefaultTempo = (int)60000000 / 120;  // 120BPM = 500,000us

TimeKeeper::TimeKeeper()
    : division_(0),
      tempo_(kDefaultTempo),
      total_tick_(0),
      reference_tick_(0),
      reference_ms_(0),
      prev_delta_time_(0),
      current_time_(0),
      start_time_(0),
      prev_time_(0),
      schedule_time_(0),
      duration_(0) {
    reset(96, kDefaultTempo);
}

void TimeKeeper::reset(uint16_t division, uint32_t tempo) {
    setDivision(division);
    setTempo(tempo);
    total_tick_ = 0;
    reference_tick_ = 0;
    reference_ms_ = 0;
}

uint16_t TimeKeeper::getDivision() {
    return division_;
}

void TimeKeeper::setDivision(uint16_t division) {
    division_ = division;
}

uint32_t TimeKeeper::getTempo() {
    return tempo_;
}

void TimeKeeper::setTempo(uint32_t tempo) {
    uint32_t tick = total_tick_ - reference_tick_;
    reference_ms_ += deltaTimeToMs(division_, tempo_, tick);
    reference_tick_ = total_tick_;
    tempo_ = tempo;
}

uint32_t TimeKeeper::getTotalTick() {
    return total_tick_;
}

void TimeKeeper::forward(uint32_t delta_time) {
    total_tick_ += delta_time;
}

unsigned long TimeKeeper::calculateDurationMs(uint32_t delta_time) {
    uint32_t tick = total_tick_ - reference_tick_;
    unsigned long last_ms = deltaTimeToMs(division_, tempo_, tick);
    unsigned long next_ms = deltaTimeToMs(division_, tempo_, tick + delta_time);
    return next_ms - last_ms;
}

unsigned long TimeKeeper::calculateCurrentMs(uint32_t delta_time) {
    uint32_t tick = total_tick_ - reference_tick_;
    return reference_ms_ + deltaTimeToMs(division_, tempo_, tick + delta_time);
}

void TimeKeeper::startSmfTimer() {
    start_time_ = prev_time_ = schedule_time_ = current_time_;
}

void TimeKeeper::stopSmfTimer() {
    duration_ = schedule_time_ - current_time_;
}

void TimeKeeper::continueSmfTimer() {
    schedule_time_ = current_time_ + duration_;
}

void TimeKeeper::setSmfDuration(unsigned long duration) {
    duration_ = duration;
}

void TimeKeeper::setCurrentTime() {
    current_time_ = millis();
}

void TimeKeeper::setScheduleTime(uint32_t delta_time) {
    prev_time_ = schedule_time_;
    prev_delta_time_ = delta_time;
    schedule_time_ += calculateDurationMs(delta_time);
}

bool TimeKeeper::isBeforeScheduledMs(unsigned long target_ms) {
    return target_ms < calculateCurrentMs(prev_delta_time_);
}

void TimeKeeper::rescheduleTime(unsigned long target_ms) {
    if (!isBeforeScheduledMs(target_ms)) {
        return;
    }
    unsigned long current_ms = calculateCurrentMs(0) + (current_time_ - prev_time_);

    unsigned long correct_time = schedule_time_;
    if (target_ms < current_ms) {
        unsigned long dur = current_ms - target_ms;
        correct_time += dur;
    } else {
        unsigned long dur = target_ms - current_ms;
        correct_time -= dur;
    }

    debug_printf("[%s::%s] re-schedule_time %d => %d\n", kClassName, __func__, (int)schedule_time_, (int)correct_time);
    schedule_time_ = correct_time;
}

bool TimeKeeper::isScheduledTime() {
    if (schedule_time_ <= current_time_) {
        return true;
    } else if (0xFFFFFC00 < schedule_time_ && current_time_ < 0x3FF) {
        return true;
    }
    return false;
}

uint32_t TimeKeeper::midiBeatToTick(uint16_t beats) {
    // 1 beat = 24 MIDI clock, 1 MIDI beat = 6 MIDI clock, 1 beat = 4 MIDI beat
    debug_printf("[%s::%s] beats=%d, division=%d\n", kClassName, __func__, (int)beats, (int)division_);
    return division_ * beats / 4;
}

unsigned long TimeKeeper::deltaTimeToMs(uint16_t division, uint32_t tempo, uint32_t delta_time) {
    if (division != 0) {
        if ((division & 0x8000) == 0) {
            // tempo[us/beat] / divison[tick/beat] * tick / 1000 = ms
            return (tempo / division) * delta_time / 1000;
        } else {
            uint8_t frame_type = 256 - ((division >> 8) & 0xFF);
            uint8_t resolution = (division >> 0) & 0xFF;
            if (resolution == 0) {
                resolution = 1;
            }
            if (frame_type == 24) {
                // 24fps
                return (1000000 / 24 / resolution) * delta_time / 1000;
            } else if (frame_type == 25) {
                // 25fps
                return (1000000 / 25 / resolution) * delta_time / 1000;
            } else if (frame_type == 29) {
                // 30fps(DF) = 29.97fps
                return (1001000 / 30 / resolution) * delta_time / 1000;
            } else if (frame_type == 30) {
                // 30fps(NDF)
                return (1000000 / 30 / resolution) * delta_time / 1000;
            }
        }
    }
    // error
    return delta_time;
}

unsigned long TimeKeeper::mtcToMs(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr) {
    uint8_t t = (hr >> 5) & 0x03;
    uint8_t h = (hr >> 0) & 0x1F;
    uint8_t m = (mn >> 0) & 0x3F;
    uint8_t s = (sc >> 0) & 0x3F;
    uint8_t f = (fr >> 0) & 0x1F;
    if (t == 0x00) {
        // 0b00 = 24fps
        unsigned long n = (((((h * 60) + m) * 60) + s) * 24) + f;
        return 1000 * n / 24;
    } else if (t == 0x01) {
        // 0b01 = 25fps
        unsigned long n = (((((h * 60) + m) * 60) + s) * 25) + f;
        return 1000 * n / 25;
    } else if (t == 0x02) {
        // 0b10 = 30fps(DF) = 29.97fps
        if ((s == 0) && (m % 10 != 0) && (f < 2)) {
            f = 2;
        }
        unsigned long df = ((h * (60 - 6)) + m - (m / 10)) * 2;
        unsigned long n = (((((h * 60) + m) * 60) + s) * 30) + f - df;
        return 1001 * n / 30;
    } else if (t == 0x03) {
        // 0b11 = 30fps(NDF)
        unsigned long n = (((((h * 60) + m) * 60) + s) * 30) + f;
        return 1000 * n / 30;
    }
    return 0;
}

unsigned long TimeKeeper::mtcToFrames(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr) {
    uint8_t t = (hr >> 5) & 0x03;
    uint8_t h = (hr >> 0) & 0x1F;
    uint8_t m = (mn >> 0) & 0x3F;
    uint8_t s = (sc >> 0) & 0x3F;
    uint8_t f = (fr >> 0) & 0x1F;
    if (t == 0x00) {
        // 0b00 = 24fps
        return (((((h * 60) + m) * 60) + s) * 24) + f;
    } else if (t == 0x01) {
        // 0b01 = 25fps
        return (((((h * 60) + m) * 60) + s) * 25) + f;
    } else if (t == 0x02) {
        // 0b10 = 30fps(DF) = 29.97fps
        if ((m % 10 != 0) && (s == 0) && (f < 2)) {
            f = 2;
        }
        unsigned long drop = (m * 2) - ((m / 10) * 2) + (h * (60 - 6) * 2);
        return (((((h * 60) + m) * 60) + s) * 30) + f - drop;
    } else if (t == 0x03) {
        // 0b11 = 30fps(NDF)
        return (((((h * 60) + m) * 60) + s) * 30) + f;
    }
    return 0;
}

#endif  // ARDUINO_ARCH_SPRESENSE
