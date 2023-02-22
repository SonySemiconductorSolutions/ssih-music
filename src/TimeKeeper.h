/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

#ifndef TIME_KEEPER_H_
#define TIME_KEEPER_H_

#include <stdint.h>

class TimeKeeper {
public:
    TimeKeeper();
    void reset(uint16_t division, uint32_t tempo);
    uint16_t getDivision();
    void setDivision(uint16_t division);
    uint32_t getTempo();
    void setTempo(uint32_t tempo);
    uint32_t getTotalTick();
    void forward(uint32_t delta_time);
    unsigned long calculateDurationMs(uint32_t delta_time);
    unsigned long calculateCurrentMs(uint32_t delta_time);

    void startSmfTimer();
    void stopSmfTimer();
    void continueSmfTimer();
    void setSmfDuration(unsigned long duration);

    void setCurrentTime();
    void setScheduleTime(uint32_t delta_time);
    bool isBeforeScheduledMs(unsigned long target_ms);
    void rescheduleTime(unsigned long target_ms);
    bool isScheduledTime();

    uint32_t midiBeatToTick(uint16_t beats);
    unsigned long deltaTimeToMs(uint16_t division, uint32_t tempo, uint32_t delta_time);
    unsigned long mtcToMs(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr);
    unsigned long mtcToFrames(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr);

private:
    uint16_t division_;
    uint32_t tempo_;
    uint32_t total_tick_;
    uint32_t reference_tick_;
    unsigned long reference_ms_;

    uint32_t prev_delta_time_;
    unsigned long current_time_;
    unsigned long start_time_;
    unsigned long prev_time_;
    unsigned long schedule_time_;
    unsigned long duration_;
};

#endif  // TIME_KEEPER_H_
