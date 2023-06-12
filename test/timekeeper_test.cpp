/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

#include <gtest/gtest.h>

#include "CorrectToneFilter.h"
#include "midi_util.h"
#include "OneKeySynthesizerFilter.h"
#include "ScoreSrc.h"
#include "TimeKeeper.h"
#include "YuruhornSrc.h"

class TestScoreFilter : public ScoreFilter {
public:
    TestScoreFilter(const String& file_name, Filter& filter) : ScoreFilter(file_name, filter) {
    }

    bool getScoreMidiMessage(ScoreParser::MidiMessage* midi_message) {
        return getMidiMessage(midi_message);
    }

private:
};

// TimeKeeper Positive Test
// 1. Base Test
TEST(TimeKeeper, TimeKeeperTest1) {
    TimeKeeper t;
    t.setDivision(96);
    t.setTempo(500000);
    setTime(0);

    EXPECT_EQ(96, t.getDivision());
    EXPECT_EQ(500000, t.getTempo());

    t.setCurrentTime();
    t.startSmfTimer();
    t.setScheduleTime(96);

    setTime(250);
    t.setCurrentTime();
    EXPECT_FALSE(t.isScheduledTime());

    setTime(500);
    t.setCurrentTime();
    EXPECT_TRUE(t.isScheduledTime());

    t.forward(96);
    EXPECT_EQ(96, t.getTotalTick());

    t.setTempo(250000);
    t.setCurrentTime();
    t.setScheduleTime(96);

    setTime(700);
    t.setCurrentTime();
    EXPECT_FALSE(t.isScheduledTime());

    setTime(750);
    t.setCurrentTime();
    EXPECT_TRUE(t.isScheduledTime());
}

// 2. Stop And Continue Test
TEST(TimeKeeper, TimeKeeperTest2) {
    TimeKeeper t;
    setTime(0);
    t.setCurrentTime();
    t.startSmfTimer();
    t.setScheduleTime((96 * 2));

    setTime(500);
    t.setCurrentTime();
    t.stopSmfTimer();

    setTime(1500);
    t.setCurrentTime();
    t.continueSmfTimer();
    EXPECT_FALSE(t.isScheduledTime());

    setTime(2000);
    t.setCurrentTime();
    EXPECT_TRUE(t.isScheduledTime());

    t.forward((96 * 2));
    t.setCurrentTime();
    t.setScheduleTime((96 * 2));

    setTime(2500);
    t.setCurrentTime();
    t.stopSmfTimer();
    t.setSmfDuration(200);

    setTime(2600);
    t.setCurrentTime();
    t.continueSmfTimer();
    EXPECT_FALSE(t.isScheduledTime());

    setTime(2800);
    t.setCurrentTime();
    EXPECT_TRUE(t.isScheduledTime());
}

// 3. Reschedule
TEST(TimeKeeper, TimeKeeperTest3) {
    TimeKeeper t;
    setTime(0);
    t.setCurrentTime();
    t.startSmfTimer();
    t.setScheduleTime((96 * 2));

    EXPECT_FALSE(t.isBeforeScheduledMs(2000));
    EXPECT_TRUE(t.isBeforeScheduledMs(700));
    t.rescheduleTime(700);

    setTime(700);
    t.setCurrentTime();
    EXPECT_TRUE(t.isScheduledTime());

    setTime(0);
    t.setCurrentTime();
    t.startSmfTimer();
    t.setScheduleTime((96 * 2));

    setTime(900);
    t.setCurrentTime();
    EXPECT_TRUE(t.isBeforeScheduledMs(700));
    t.rescheduleTime(700);

    setTime(1000);
    t.setCurrentTime();
    EXPECT_FALSE(t.isScheduledTime());

    setTime(1200);
    t.setCurrentTime();
    EXPECT_TRUE(t.isScheduledTime());
}

// 4. Time OverFlow
TEST(TimeKeeper, TimeKeeperTest4) {
    TimeKeeper t;
    setTime(0);
    t.setCurrentTime();
    t.startSmfTimer();
    t.setSmfDuration(0xFFFFFFF0);
    t.continueSmfTimer();
    EXPECT_TRUE(t.isScheduledTime());
}

// 5. unit change (Delta time to ms)
TEST(TimeKeeper, TimeKeeperTest5) {
    TimeKeeper t;
    EXPECT_EQ(1000, t.deltaTimeToMs(0x03E8, 100000, 10000));
    // SMPTY format
    // frame type 24(256 - 24) (10,416)
    uint16_t division_24 = 0x8000 | ((256 - 24) << 8) | 4;
    EXPECT_EQ(1041, t.deltaTimeToMs(division_24, 0, 100));
    // printf("%x, %d\n", division_24, frame);

    // frame type 25 (10,000)
    uint16_t division_25 = 0x8000 | ((256 - 25) << 8) | 4;
    EXPECT_EQ(1000, t.deltaTimeToMs(division_25, 0, 100));
    // printf("%x, %d\n", division_25, frame);

    // frame type 29 (8,620)
    uint16_t division_29 = 0x8000 | ((256 - 29) << 8) | 4;
    EXPECT_EQ(834, t.deltaTimeToMs(division_29, 0, 100));
    // printf("%x, %d\n", division_29, frame);

    // frame type 30 (8,333)
    uint16_t division_30 = 0x8000 | ((256 - 30) << 8) | 4;
    EXPECT_EQ(833, t.deltaTimeToMs(division_30, 0, 100));
    // printf("%x, %d\n", division_30, frame);

    // frame type 25 (10,000)
    uint16_t division = 0x8000 | ((256 - 25) << 8);
    EXPECT_EQ(4000, t.deltaTimeToMs(division, 0, 100));
}

// 6. unit change (Delta time mtc to others)
TEST(TimeKeeper, TimeKeeperTest6) {
    const uint8_t kFrameRate24Fps = 0x00, kFrameRate25Fps = 0x20, kFrameRate30FpsDf = 0x40, kFrameRate30FpsNdf = 0x60;
    TimeKeeper t;

    // 01:02:03:04 (24fps)
    // (1 * 86400) + (2 * 1440) + (3 * 24) + 4 = 89356[frame]
    // (1 * 3600) + (2 * 60) + (3) + (4 / 24) = 3723166.667[ms]
    uint8_t hr = kFrameRate24Fps | 1;
    uint8_t mn = 2;
    uint8_t sc = 3;
    uint8_t fr = 4;
    EXPECT_EQ(89356, t.mtcToFrames(hr, mn, sc, fr));
    EXPECT_TRUE(3723166 <= t.mtcToMs(hr, mn, sc, fr) && t.mtcToMs(hr, mn, sc, fr) <= 3723167);

    // 04:03:02:01 (25fps)
    // (4 * 90000) + (3 * 1500) + (2 * 25) + 1 = 364551[frame]
    // (4 * 3600) + (3 * 60) + (2) + (1 / 25) = 14582040[ms]
    hr = kFrameRate25Fps | 4;
    mn = 3;
    sc = 2;
    fr = 1;
    EXPECT_EQ(364551, t.mtcToFrames(hr, mn, sc, fr));
    EXPECT_EQ(14582040, t.mtcToMs(hr, mn, sc, fr));

    // 02:01:04;03 (29.97fps)
    // (2 * 108000) + (1 * 1800) + (4 * 30) + 3 - DropFrame = 217705[frame] ~= 7264100[ms]
    hr = kFrameRate30FpsDf | 2;
    mn = 1;
    sc = 4;
    fr = 3;
    EXPECT_EQ(217705, t.mtcToFrames(hr, mn, sc, fr));
    EXPECT_TRUE(7263100 <= t.mtcToMs(hr, mn, sc, fr) && t.mtcToMs(hr, mn, sc, fr) <= 7265100);

    // 02:01:04:03 (29.97fps)
    // (2 * 108000) + (1 * 1800) + (4 * 30) + 3 = 217923[frame]
    // (2 * 3600) + (1 * 60) + (4) + (3 / 30) = 7264100[ms]
    hr = kFrameRate30FpsNdf | 2;
    EXPECT_EQ(217923, t.mtcToFrames(hr, mn, sc, fr));
    EXPECT_EQ(7264100, t.mtcToMs(hr, mn, sc, fr));
}
