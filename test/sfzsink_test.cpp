/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "gtest/gtest.h"

#include <Arduino.h>

#include <OutputMixer.h>

#include "SFZSink.h"

void create_file(const String &file_path, const String &text) {
    registerDummyFile(file_path, text);
}

SFZSink sink("yuruhorn.sfz");

TEST(SFZSink, begin) {
    EXPECT_EQ(true, sink.begin());
}

TEST(SFZSink, sendNoteOff) {
    EXPECT_EQ(true, sink.sendNoteOff(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL));
}

TEST(SFZSink, sendNoteOn) {
    EXPECT_EQ(false, sink.sendNoteOn(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL));
    sink.sendNoteOff(60, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
}

TEST(SFZSink, setParam) {
    EXPECT_EQ(true, sink.setParam(Filter::PARAMID_OUTPUT_LEVEL, 100));
}

TEST(SFZSink, getParam) {
    sink.setParam(Filter::PARAMID_OUTPUT_LEVEL, 77);
    EXPECT_EQ(77, sink.getParam(Filter::PARAMID_OUTPUT_LEVEL));
}

TEST(SFZSink, isAvailable) {
    EXPECT_EQ(true, sink.isAvailable(Filter::PARAMID_OUTPUT_LEVEL));
}

size_t g_datacount[256] = {0};
void *test_handler(void *arg, AsSendDataOutputMixer *regions) {
    uint8_t *addr = (uint8_t *)regions->pcm.mh.getPa();
    if (regions->pcm.size > 0 && addr) {
        // printf("%s(%p,%p), %d, %d ", __func__, arg, regions, (int)regions->pcm.identifier, regions->pcm.size);
        uint8_t prev = ~addr[0];
        for (size_t i = 0; i < regions->pcm.size; i++) {
            if (prev != addr[i]) {
                // printf("[%d]=0x%02X, ", (int)i, addr[i]);
                prev = addr[i];
            }
            g_datacount[addr[i]]++;
        }
        // printf("\n");
    }
    return nullptr;
}

TEST(SFZSink, outputtest1) {
    char *data1 = new char[1024 * 4 + 1];
    char *data2 = new char[1024 * 4 + 1];
    if (data1) {
        memset(data1, 0x00, 1024 * 4 + 1);
        memset(data1, 1, 1024 * 4);
    }
    if (data2) {
        memset(data2, 0x00, 1024 * 4 + 1);
        memset(data2, 2, 1024 * 4);
    }
    create_file("testdata/SFZSink/1.bin", data1);
    create_file("testdata/SFZSink/2.bin", data2);
    delete[] data1;
    delete[] data2;

    create_file("testdata/SFZSink/outputtest1.sfz",
                "<region>\n key=1 sample=1.bin offset=10 end=19 loop_mode=no_loop\n"
                "<region>\n key=2 sample=2.bin offset=20 end=29 count=10\n");

    memset(g_datacount, 0x00, sizeof(g_datacount));
    OutputMixer *theMixer = OutputMixer::getInstance();
    theMixer->clear();
    theMixer->setOutputHandler(test_handler, nullptr);
    SFZSink sink("testdata/SFZSink/outputtest1.sfz");
    sink.begin();

    sink.sendNoteOn(1, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
    for (int i = 0; i < 4; i++) {
        sink.update();
        theMixer->flush();
    }
    sink.sendNoteOff(1, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
    for (int i = 0; i < 2; i++) {
        sink.update();
        theMixer->flush();
    }

    EXPECT_EQ(g_datacount[1], 1 * 10 * 4);
    theMixer->setOutputHandler(nullptr, nullptr);
}

TEST(SFZSink, outputtest2) {
    char *data1 = new char[1024 * 4 + 1];
    char *data2 = new char[1024 * 4 + 1];
    if (data1) {
        memset(data1, 0x00, 1024 * 4 + 1);
        memset(data1, 1, 1024 * 4);
    }
    if (data2) {
        memset(data2, 0x00, 1024 * 4 + 1);
        memset(data2, 2, 1024 * 4);
    }
    create_file("testdata/SFZSink/1.bin", data1);
    create_file("testdata/SFZSink/2.bin", data2);
    delete[] data1;
    delete[] data2;

    create_file("testdata/SFZSink/outputtest2.sfz",
                "<region>\n key=1 sample=1.bin offset=10 end=19 loop_mode=no_loop\n"
                "<region>\n key=2 sample=2.bin offset=20 end=29 count=10\n");

    memset(g_datacount, 0x00, sizeof(g_datacount));
    OutputMixer *theMixer = OutputMixer::getInstance();
    theMixer->clear();
    theMixer->setOutputHandler(test_handler, nullptr);
    SFZSink sink("testdata/SFZSink/outputtest2.sfz");
    sink.begin();

    sink.sendNoteOn(2, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
    for (int i = 0; i < 4; i++) {
        sink.update();
        theMixer->flush();
    }
    sink.sendNoteOff(2, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
    for (int i = 0; i < 2; i++) {
        sink.update();
        theMixer->flush();
    }

    EXPECT_EQ(g_datacount[2], 10 * 10 * 4);
    theMixer->setOutputHandler(nullptr, nullptr);
}

class SfzTest : public ::testing::Test {
public:
    int getSfzDataNum(const SFZSink &sfz_test) {
        return sfz_test.regions_.size();
    }
    int getSfzDataSwlokey(const SFZSink &sfz_test) {
        return sfz_test.sw_lokey_;
    }
    int getSfzDataSwhikey(const SFZSink &sfz_test) {
        return sfz_test.sw_hikey_;
    }
    int getSfzDataSwkey(const SFZSink &sfz_test) {
        return sfz_test.sw_last_;
    }
    std::vector<SFZSink::Region> getSfzData(const SFZSink &sfz_test) {
        return sfz_test.regions_;
    }

protected:
    std::vector<SFZSink::Region> regions;
};

TEST_F(SfzTest, kyoukai_lokey) {
    create_file("testdata/SFZSink/kyoukai_lokey.sfz",
                "<region> lokey=-1\n"
                "<region> lokey=0\n"
                "<region> lokey=1\n"
                "<region> lokey=60\n"
                "<region> lokey=126\n"
                "<region> lokey=127\n"
                "<region> lokey=128\n"
                "<region> lokey==64\n"
                "<region> lokey=aaaa\n"
                "<region> lokey=60test\n"
                "<region> lokey=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 11);
    if (regions.size() >= 11) {
        const uint32_t default_lokey = 0;
        EXPECT_EQ(regions[0].lokey, default_lokey);
        EXPECT_EQ(regions[1].lokey, 0);
        EXPECT_EQ(regions[2].lokey, 1);
        EXPECT_EQ(regions[3].lokey, 60);
        EXPECT_EQ(regions[4].lokey, 126);
        EXPECT_EQ(regions[5].lokey, 127);
        EXPECT_EQ(regions[6].lokey, default_lokey);
        EXPECT_EQ(regions[7].lokey, default_lokey);
        EXPECT_EQ(regions[8].lokey, default_lokey);
        EXPECT_EQ(regions[9].lokey, default_lokey);
        EXPECT_EQ(regions[10].lokey, default_lokey);
    }
}

TEST_F(SfzTest, kyoukai_hikey) {
    create_file("testdata/SFZSink/kyoukai_hikey.sfz",
                "<region> hikey=-1\n"
                "<region> hikey=0\n"
                "<region> hikey=1\n"
                "<region> hikey=60\n"
                "<region> hikey=126\n"
                "<region> hikey=127\n"
                "<region> hikey=128\n"
                "<region> hikey==64\n"
                "<region> hikey=aaaa\n"
                "<region> hikey=60test\n"
                "<region> hikey=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 11);
    if (regions.size() >= 11) {
        const uint32_t default_hikey = 127;
        EXPECT_EQ(regions[0].hikey, default_hikey);
        EXPECT_EQ(regions[1].hikey, 0);
        EXPECT_EQ(regions[2].hikey, 1);
        EXPECT_EQ(regions[3].hikey, 60);
        EXPECT_EQ(regions[4].hikey, 126);
        EXPECT_EQ(regions[5].hikey, 127);
        EXPECT_EQ(regions[6].hikey, default_hikey);
        EXPECT_EQ(regions[7].hikey, default_hikey);
        EXPECT_EQ(regions[8].hikey, default_hikey);
        EXPECT_EQ(regions[9].hikey, default_hikey);
        EXPECT_EQ(regions[10].hikey, default_hikey);
    }
}

TEST_F(SfzTest, kyoukai_key) {
    create_file("testdata/SFZSink/kyoukai_key.sfz",
                "<region> key=-1\n"
                "<region> key=0\n"
                "<region> key=1\n"
                "<region> key=60\n"
                "<region> key=126\n"
                "<region> key=127\n"
                "<region> key=128\n"
                "<region> key==64\n"
                "<region> key=aaaa\n"
                "<region> key=60test\n"
                "<region> key=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 11);
    if (regions.size() >= 11) {
        const uint32_t default_lokey = 0;
        const uint32_t default_hikey = 127;
        EXPECT_EQ(regions[0].lokey, default_lokey);
        EXPECT_EQ(regions[0].hikey, default_hikey);
        EXPECT_EQ(regions[1].lokey, 0);
        EXPECT_EQ(regions[1].hikey, 0);
        EXPECT_EQ(regions[2].lokey, 1);
        EXPECT_EQ(regions[2].hikey, 1);
        EXPECT_EQ(regions[3].lokey, 60);
        EXPECT_EQ(regions[3].hikey, 60);
        EXPECT_EQ(regions[4].lokey, 126);
        EXPECT_EQ(regions[4].hikey, 126);
        EXPECT_EQ(regions[5].lokey, 127);
        EXPECT_EQ(regions[5].hikey, 127);
        EXPECT_EQ(regions[6].lokey, default_lokey);
        EXPECT_EQ(regions[6].hikey, default_hikey);
        EXPECT_EQ(regions[7].lokey, default_lokey);
        EXPECT_EQ(regions[7].hikey, default_hikey);
        EXPECT_EQ(regions[8].lokey, default_lokey);
        EXPECT_EQ(regions[8].hikey, default_hikey);
        EXPECT_EQ(regions[9].lokey, default_lokey);
        EXPECT_EQ(regions[9].hikey, default_hikey);
        EXPECT_EQ(regions[10].lokey, default_lokey);
        EXPECT_EQ(regions[10].hikey, default_hikey);
    }
}

TEST_F(SfzTest, kyoukai_offset) {
    create_file("testdata/SFZSink/kyoukai_offset.sfz",
                "<region> offset=-1\n"
                "<region> offset=0\n"
                "<region> offset=1\n"
                "<region> offset=10000\n"
                "<region> offset=2147483647\n"
                "<region> offset=2147483648\n"
                "<region> offset=4294967294\n"
                "<region> offset=4294967295\n"
                "<region> offset=4294967296\n"
                "<region> offset==64\n"
                "<region> offset=aaaa\n"
                "<region> offset=60test\n"
                "<region> offset=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 13);
    if (regions.size() >= 13) {
        const uint32_t default_offset = 0;
        EXPECT_EQ(regions[0].offset, default_offset);
        EXPECT_EQ(regions[1].offset, default_offset);
        EXPECT_EQ(regions[2].offset, default_offset);
        EXPECT_EQ(regions[3].offset, default_offset);
        EXPECT_EQ(regions[4].offset, default_offset);
        EXPECT_EQ(regions[5].offset, default_offset);
        EXPECT_EQ(regions[6].offset, default_offset);
        EXPECT_EQ(regions[7].offset, default_offset);
        EXPECT_EQ(regions[8].offset, default_offset);
        EXPECT_EQ(regions[9].offset, default_offset);
        EXPECT_EQ(regions[10].offset, default_offset);
        EXPECT_EQ(regions[11].offset, default_offset);
        EXPECT_EQ(regions[12].offset, default_offset);
    }
}

TEST_F(SfzTest, kyoukai_end) {
    create_file("testdata/SFZSink/kyoukai_end.sfz",
                "<region> end=-2\n"
                "<region> end=-1\n"
                "<region> end=0\n"
                "<region> end=10000\n"
                "<region> end=2147483647\n"
                "<region> end=2147483648\n"
                "<region> end=4294967294\n"
                "<region> end=4294967295\n"
                "<region> end=4294967296\n"
                "<region> end==64\n"
                "<region> end=aaaa\n"
                "<region> end=60test\n"
                "<region> end=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 13);
    if (regions.size() >= 13) {
        const uint32_t default_end = 0;
        EXPECT_EQ(regions[0].end, default_end);
        EXPECT_EQ(regions[1].silence, true);
        EXPECT_EQ(regions[2].end, default_end);
        EXPECT_EQ(regions[3].end, default_end);
        EXPECT_EQ(regions[4].end, default_end);
        EXPECT_EQ(regions[5].end, default_end);
        EXPECT_EQ(regions[6].end, default_end);
        EXPECT_EQ(regions[7].end, default_end);
        EXPECT_EQ(regions[8].end, default_end);
        EXPECT_EQ(regions[9].end, default_end);
        EXPECT_EQ(regions[10].end, default_end);
        EXPECT_EQ(regions[11].end, default_end);
        EXPECT_EQ(regions[12].end, default_end);
    }
}

TEST_F(SfzTest, kyoukai_count) {
    create_file("testdata/SFZSink/kyoukai_count.sfz",
                "<region> count=-1\n"
                "<region> count=0\n"
                "<region> count=1\n"
                "<region> count=10000\n"
                "<region> count=2147483647\n"
                "<region> count=2147483648\n"
                "<region> count=4294967294\n"
                "<region> count=4294967295\n"
                "<region> count=4294967296\n"
                "<region> count==64\n"
                "<region> count=aaaa\n"
                "<region> count=60test\n"
                "<region> count=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 13);
    if (regions.size() >= 13) {
        const uint32_t default_count = 1;
        EXPECT_EQ(regions[0].count, default_count);
        EXPECT_EQ(regions[1].count, 0);
        EXPECT_EQ(regions[2].count, 1);
        EXPECT_EQ(regions[3].count, 10000);
        EXPECT_EQ(regions[4].count, 2147483647);
        EXPECT_EQ(regions[5].count, 2147483648);
        EXPECT_EQ(regions[6].count, 4294967294);
        EXPECT_EQ(regions[7].count, 4294967295);
        EXPECT_EQ(regions[8].count, default_count);
        EXPECT_EQ(regions[9].count, default_count);
        EXPECT_EQ(regions[10].count, default_count);
        EXPECT_EQ(regions[11].count, default_count);
        EXPECT_EQ(regions[12].count, default_count);
    }
}

TEST_F(SfzTest, loop_start) {
    create_file("testdata/SFZSink/kyoukai_loop_start.sfz",
                "<region> loop_start=-1\n"
                "<region> loop_start=0\n"
                "<region> loop_start=1\n"
                "<region> loop_start=10000\n"
                "<region> loop_start=2147483647\n"
                "<region> loop_start=2147483648\n"
                "<region> loop_start=4294967294\n"
                "<region> loop_start=4294967295\n"
                "<region> loop_start=4294967296\n"
                "<region> loop_start==64\n"
                "<region> loop_start=aaaa\n"
                "<region> loop_start=60test\n"
                "<region> loop_start=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 13);
    if (regions.size() >= 13) {
        const uint32_t default_loop_start = 0;
        EXPECT_EQ(regions[0].loop_start, default_loop_start);
        EXPECT_EQ(regions[1].loop_start, default_loop_start);
        EXPECT_EQ(regions[2].loop_start, default_loop_start);
        EXPECT_EQ(regions[3].loop_start, default_loop_start);
        EXPECT_EQ(regions[4].loop_start, default_loop_start);
        EXPECT_EQ(regions[5].loop_start, default_loop_start);
        EXPECT_EQ(regions[6].loop_start, default_loop_start);
        EXPECT_EQ(regions[7].loop_start, default_loop_start);
        EXPECT_EQ(regions[8].loop_start, default_loop_start);
        EXPECT_EQ(regions[9].loop_start, default_loop_start);
        EXPECT_EQ(regions[10].loop_start, default_loop_start);
        EXPECT_EQ(regions[11].loop_start, default_loop_start);
        EXPECT_EQ(regions[12].loop_start, default_loop_start);
    }
}

TEST_F(SfzTest, loop_end) {
    create_file("testdata/SFZSink/kyoukai_loop_end.sfz",
                "<region> loop_end=-1\n"
                "<region> loop_end=0\n"
                "<region> loop_end=1\n"
                "<region> loop_end=10000\n"
                "<region> loop_end=2147483647\n"
                "<region> loop_end=2147483648\n"
                "<region> loop_end=4294967294\n"
                "<region> loop_end=4294967295\n"
                "<region> loop_end=4294967296\n"
                "<region> loop_end==64\n"
                "<region> loop_end=aaaa\n"
                "<region> loop_end=60test\n"
                "<region> loop_end=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 13);
    if (regions.size() >= 13) {
        const uint32_t default_loop_end = 0;
        EXPECT_EQ(regions[0].loop_end, default_loop_end);
        EXPECT_EQ(regions[1].loop_end, default_loop_end);
        EXPECT_EQ(regions[2].loop_end, default_loop_end);
        EXPECT_EQ(regions[3].loop_end, default_loop_end);
        EXPECT_EQ(regions[4].loop_end, default_loop_end);
        EXPECT_EQ(regions[5].loop_end, default_loop_end);
        EXPECT_EQ(regions[6].loop_end, default_loop_end);
        EXPECT_EQ(regions[7].loop_end, default_loop_end);
        EXPECT_EQ(regions[8].loop_end, default_loop_end);
        EXPECT_EQ(regions[9].loop_end, default_loop_end);
        EXPECT_EQ(regions[10].loop_end, default_loop_end);
        EXPECT_EQ(regions[11].loop_end, default_loop_end);
        EXPECT_EQ(regions[12].loop_end, default_loop_end);
    }
}

TEST_F(SfzTest, verify_sample) {
    create_file("testdata/SFZSink/verify_sample.sfz",
                "<region> sample=guitar_c4_ff.wav\n"
                "<region> sample=dog kick.ogg\n"
                "<region> sample=out of tune trombone (redundant).wav\n"
                "<region> sample=staccatto_snare.ogg\n"
                "<region> sample=dog      kick.ogg\n"
                "<region> sample=dog　kick.ogg\n"
                "<region> sample=てすと.wav\n"
                "<region> sample=guitar=c4_ff.wav\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 8);
    if (regions.size() >= 8) {
        const String default_sample = "";
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/guitar_c4_ff.wav");
        EXPECT_EQ(regions[1].sample, "testdata/SFZSink/dog kick.ogg");
        EXPECT_EQ(regions[2].sample, "testdata/SFZSink/out of tune trombone (redundant).wav");
        EXPECT_EQ(regions[3].sample, "testdata/SFZSink/staccatto_snare.ogg");
        EXPECT_EQ(regions[4].sample, "testdata/SFZSink/dog      kick.ogg");
        EXPECT_EQ(regions[5].sample, "testdata/SFZSink/dog　kick.ogg");
        EXPECT_EQ(regions[6].sample, "testdata/SFZSink/てすと.wav");
        EXPECT_EQ(regions[7].sample, "testdata/SFZSink/guitar");
    }
}

TEST_F(SfzTest, verify_loop_mode) {
    create_file("testdata/SFZSink/verify_loop_mode.sfz",
                "<region> loop_mode=no_loop\n"
                "<region> loop_mode=one_shot\n"
                "<region> loop_mode=loop_continuous\n"
                "<region> loop_mode=loop_sustain\n"
                "<region> loop_mode=test\n"
                "<region> loop_mode=123\n"
                "<region> loop_mode=ああああ\n"
                "<region> loop_mode==no_loop\n"
                "<region> loop_mode==nno_loop\n"
                "<region> loop_mode==one_shott\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 10);
    if (regions.size() >= 10) {
        const uint32_t default_loop_mode = SFZSink::kNoLoop;
        EXPECT_EQ(regions[0].loop_mode, SFZSink::kNoLoop);
        EXPECT_EQ(regions[1].loop_mode, SFZSink::kOneShot);
        EXPECT_EQ(regions[2].loop_mode, SFZSink::kLoopContinuous);
        EXPECT_EQ(regions[3].loop_mode, SFZSink::kLoopSustain);
        EXPECT_EQ(regions[4].loop_mode, default_loop_mode);
        EXPECT_EQ(regions[5].loop_mode, default_loop_mode);
        EXPECT_EQ(regions[6].loop_mode, default_loop_mode);
        EXPECT_EQ(regions[7].loop_mode, default_loop_mode);
        EXPECT_EQ(regions[8].loop_mode, default_loop_mode);
        EXPECT_EQ(regions[9].loop_mode, default_loop_mode);
    }
}

TEST_F(SfzTest, group_region_test1) {
    create_file("testdata/SFZSink/group_region_test1.sfz",
                "<group>  lokey=60 hikey=60 offset=10000 end=10000 count=3 loop_mode=one_shot        loop_start=10000 loop_end=10000\n"
                "<region> lokey=65 hikey=65 offset=20000 end=20000 count=2 loop_mode=loop_continuous loop_start=20000 loop_end=20000\n"
                "<region>\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/group_region_test1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 2);
    if (regions.size() >= 2) {
        EXPECT_EQ(regions[0].lokey, 65);
        EXPECT_EQ(regions[0].hikey, 65);
        EXPECT_EQ(regions[1].lokey, 60);
        EXPECT_EQ(regions[1].hikey, 60);
    }
}

TEST_F(SfzTest, group_region_test2) {
    create_file("testdata/SFZSink/group_region_test2.sfz",
                "<group> lokey=60 hikey=60 offset=10000 end=10000 count=3 loop_mode=one_shot loop_start=10000 loop_end=10000\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/group_region_test2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 60);
        EXPECT_EQ(regions[0].hikey, 60);
    }
}

TEST_F(SfzTest, group_region_test3) {
    create_file("testdata/SFZSink/group_region_test3.sfz",
                "<group>  offset=10000 end=10000 loop_mode=loop_continuous loop_start=10000 loop_end=10000\n"
                "<region> lokey=65 hikey=65\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/group_region_test3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 65);
        EXPECT_EQ(regions[0].hikey, 65);
    }
}

TEST_F(SfzTest, group_region_test4) {
    create_file("testdata/SFZSink/group_region_test4.sfz",
                "<group>  offset=10000 end=10000 loop_mode=loop_continuous loop_start=10000 loop_end=10000\n"
                "<region> lokey=65 hikey=65 loop_start=15000 loop_end=20000\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/group_region_test4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 65);
        EXPECT_EQ(regions[0].hikey, 65);
        EXPECT_EQ(regions[0].loop_mode, SFZSink::kLoopContinuous);
    }
}

TEST_F(SfzTest, read_sfzfile) {
    create_file("testdata/SFZSink/read_sfzfile.sfz",
                "<region> \n"
                "    lokey=65\n"
                "    hikey=65\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/read_sfzfile.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 65);
        EXPECT_EQ(regions[0].hikey, 65);
    }
}

TEST_F(SfzTest, include) {
    create_file("testdata/SFZSink/include.sfz",
                "#include    \"include2_space.sfz\" \n"
                "#include\t\"include3_tab.sfz\"    \n"
                "#include    \"\"    \n"
                "#include    \"include4.sfz\n"
                "");
    create_file("testdata/SFZSink/include2_space.sfz",
                "<region> sample=test1_space.raw\n"
                "");
    create_file("testdata/SFZSink/include3_tab.sfz",
                "<region> sample=test2_tab.raw\n"
                "");
    create_file("testdata/SFZSink/include4.sfz",
                "<region> sample=test3.raw\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/include.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 2);
    if (regions.size() >= 2) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/test1_space.raw");
        EXPECT_EQ(regions[1].sample, "testdata/SFZSink/test2_tab.raw");
    }
}

TEST_F(SfzTest, define) {
    create_file("testdata/SFZSink/define.sfz",
                "#define    $A def1  \n"
                "#define\t$AA\tdef2    \n"
                "<region> sample=$AAtest1_space$A.raw\n"
                "#define\t$PLE ple    \n"
                "<region> sam$PLE=test2.raw\n"
                "#define\t$ERR1\n"
                "<region> sample=$ERR1test3.raw\n"
                "#define ERR2 error2\n"
                "<region> sample=ERR2test4.raw\n"
                "#define $TEST5 \n"
                "<region> sample=$TEST5test5.raw\n"
                "#define $TEST6     \n"
                "<region> sample=$TEST6test6.raw\n"
                "#define $     \n"
                "<region> sample=$test7.raw\n"
                "");

    SFZSink sfz_test = SFZSink("testdata/SFZSink/define.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 7);
    if (regions.size() >= 7) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/$AAtest1_spacedef1.raw");
        EXPECT_EQ(regions[1].sample, "testdata/SFZSink/test2.raw");
        EXPECT_EQ(regions[2].sample, "testdata/SFZSink/$ERR1test3.raw");
        EXPECT_EQ(regions[3].sample, "testdata/SFZSink/ERR2test4.raw");
        EXPECT_EQ(regions[4].sample, "testdata/SFZSink/$TEST5test5.raw");
        EXPECT_EQ(regions[5].sample, "testdata/SFZSink/$TEST6test6.raw");
        EXPECT_EQ(regions[6].sample, "testdata/SFZSink/$test7.raw");
    }
}

TEST_F(SfzTest, sw_last) {
    create_file("testdata/SFZSink/sw_last.sfz",
                "<region> sw_last=-1\n"
                "<region> sw_last=0\n"
                "<region> sw_last=1\n"
                "<region> sw_last=60\n"
                "<region> sw_last=126\n"
                "<region> sw_last=127\n"
                "<region> sw_last=128\n"
                "<region> sw_last==64\n"
                "<region> sw_last=aaaa\n"
                "<region> sw_last=60test\n"
                "<region> sw_last=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 11);
    if (regions.size() >= 11) {
        const uint32_t default_sw_last = INVALID_NOTE_NUMBER;
        EXPECT_EQ(regions[0].sw_last, default_sw_last);
        EXPECT_EQ(regions[1].sw_last, 0);
        EXPECT_EQ(regions[2].sw_last, 1);
        EXPECT_EQ(regions[3].sw_last, 60);
        EXPECT_EQ(regions[4].sw_last, 126);
        EXPECT_EQ(regions[5].sw_last, 127);
        EXPECT_EQ(regions[6].sw_last, default_sw_last);
        EXPECT_EQ(regions[7].sw_last, default_sw_last);
        EXPECT_EQ(regions[8].sw_last, default_sw_last);
        EXPECT_EQ(regions[9].sw_last, default_sw_last);
        EXPECT_EQ(regions[10].sw_last, default_sw_last);
    }
}

TEST_F(SfzTest, sw_lokey_hikey) {
    const uint32_t default_sw_lokey = NOTE_NUMBER_MIN;
    const uint32_t default_sw_hikey = NOTE_NUMBER_MAX;
    create_file("testdata/SFZSink/sw_lokey_hikey1.sfz",
                "<region> sw_lokey=-1 sw_hikey=-1\n"
                "");
    SFZSink sfz_test1 = SFZSink("testdata/SFZSink/sw_lokey_hikey1.sfz");
    EXPECT_EQ(getSfzData(sfz_test1).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test1), default_sw_lokey);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test1), default_sw_hikey);

    create_file("testdata/SFZSink/sw_lokey_hikey2.sfz",
                "<region> sw_lokey=0 sw_hikey=0\n"
                "");
    SFZSink sfz_test2 = SFZSink("testdata/SFZSink/sw_lokey_hikey2.sfz");
    EXPECT_EQ(getSfzData(sfz_test2).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test2), 0);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test2), 0);

    create_file("testdata/SFZSink/sw_lokey_hikey3.sfz",
                "<region> sw_lokey=1 sw_hikey=1\n"
                "");
    SFZSink sfz_test3 = SFZSink("testdata/SFZSink/sw_lokey_hikey3.sfz");
    EXPECT_EQ(getSfzData(sfz_test3).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test3), 1);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test3), 1);

    create_file("testdata/SFZSink/sw_lokey_hikey4.sfz",
                "<region> sw_lokey=60 sw_hikey=60\n"
                "");
    SFZSink sfz_test4 = SFZSink("testdata/SFZSink/sw_lokey_hikey4.sfz");
    EXPECT_EQ(getSfzData(sfz_test4).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test4), 60);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test4), 60);

    create_file("testdata/SFZSink/sw_lokey_hikey5.sfz",
                "<region> sw_lokey=126 sw_hikey=126\n"
                "");
    SFZSink sfz_test5 = SFZSink("testdata/SFZSink/sw_lokey_hikey5.sfz");
    EXPECT_EQ(getSfzData(sfz_test5).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test5), 126);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test5), 126);

    create_file("testdata/SFZSink/sw_lokey_hikey6.sfz",
                "<region> sw_lokey=127 sw_hikey=127\n"
                "");
    SFZSink sfz_test6 = SFZSink("testdata/SFZSink/sw_lokey_hikey6.sfz");
    EXPECT_EQ(getSfzData(sfz_test6).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test6), 127);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test6), 127);

    create_file("testdata/SFZSink/sw_lokey_hikey7.sfz",
                "<region> sw_lokey=128 sw_hikey=128\n"
                "\n");
    SFZSink sfz_test7 = SFZSink("testdata/SFZSink/sw_lokey_hikey7.sfz");
    EXPECT_EQ(getSfzData(sfz_test7).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test7), default_sw_lokey);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test7), default_sw_hikey);

    create_file("testdata/SFZSink/sw_lokey_hikey8.sfz",
                "<region> sw_lokey==64 sw_hikey==64\n"
                "");
    SFZSink sfz_test8 = SFZSink("testdata/SFZSink/sw_lokey_hikey8.sfz");
    EXPECT_EQ(getSfzData(sfz_test8).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test8), default_sw_lokey);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test8), default_sw_hikey);

    create_file("testdata/SFZSink/sw_lokey_hikey9.sfz",
                "<region> sw_lokey=aaaa sw_hikey=aaaa\n"
                "");
    SFZSink sfz_test9 = SFZSink("testdata/SFZSink/sw_lokey_hikey9.sfz");
    EXPECT_EQ(getSfzData(sfz_test9).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test9), default_sw_lokey);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test9), default_sw_hikey);

    create_file("testdata/SFZSink/sw_lokey_hikey10.sfz",
                "<region> sw_lokey=60test sw_hikey=60test\n"
                "");
    SFZSink sfz_test10 = SFZSink("testdata/SFZSink/sw_lokey_hikey10.sfz");
    EXPECT_EQ(getSfzData(sfz_test10).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test10), default_sw_lokey);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test10), default_sw_hikey);

    create_file("testdata/SFZSink/sw_lokey_hikey11.sfz",
                "<region> sw_lokey=あ sw_hikey=あ\n"
                "");
    SFZSink sfz_test11 = SFZSink("testdata/SFZSink/sw_lokey_hikey11.sfz");
    EXPECT_EQ(getSfzData(sfz_test11).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test11), default_sw_lokey);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test11), default_sw_hikey);
}

TEST_F(SfzTest, sw_default) {
    const uint32_t default_sw_last = INVALID_NOTE_NUMBER;
    create_file("testdata/SFZSink/sw_default1.sfz",
                "<region> sw_default=-1\n"
                "");
    SFZSink sfz_test1 = SFZSink("testdata/SFZSink/sw_default1.sfz");
    EXPECT_EQ(getSfzData(sfz_test1).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test1), default_sw_last);

    create_file("testdata/SFZSink/sw_default2.sfz",
                "<region> sw_default=0\n"
                "");
    SFZSink sfz_test2 = SFZSink("testdata/SFZSink/sw_default2.sfz");
    EXPECT_EQ(getSfzData(sfz_test2).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test2), 0);

    create_file("testdata/SFZSink/sw_default3.sfz",
                "<region> sw_default=1\n"
                "");
    SFZSink sfz_test3 = SFZSink("testdata/SFZSink/sw_default3.sfz");
    EXPECT_EQ(getSfzData(sfz_test3).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test3), 1);

    create_file("testdata/SFZSink/sw_default4.sfz",
                "<region> sw_default=60\n"
                "");
    SFZSink sfz_test4 = SFZSink("testdata/SFZSink/sw_default4.sfz");
    EXPECT_EQ(getSfzData(sfz_test4).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test4), 60);

    create_file("testdata/SFZSink/sw_default5.sfz",
                "<region> sw_default=126\n"
                "");
    SFZSink sfz_test5 = SFZSink("testdata/SFZSink/sw_default5.sfz");
    EXPECT_EQ(getSfzData(sfz_test5).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test5), 126);

    create_file("testdata/SFZSink/sw_default6.sfz",
                "<region> sw_default=127\n"
                "");
    SFZSink sfz_test6 = SFZSink("testdata/SFZSink/sw_default6.sfz");
    EXPECT_EQ(getSfzData(sfz_test6).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test6), 127);

    create_file("testdata/SFZSink/sw_default7.sfz",
                "<region> sw_default=128\n"
                "");
    SFZSink sfz_test7 = SFZSink("testdata/SFZSink/sw_default7.sfz");
    EXPECT_EQ(getSfzData(sfz_test7).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test7), default_sw_last);

    create_file("testdata/SFZSink/sw_default8.sfz",
                "<region> sw_default==64\n"
                "");
    SFZSink sfz_test8 = SFZSink("testdata/SFZSink/sw_default8.sfz");
    EXPECT_EQ(getSfzData(sfz_test8).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test8), default_sw_last);

    create_file("testdata/SFZSink/sw_default9.sfz",
                "<region> sw_default=aaaa\n"
                "");
    SFZSink sfz_test9 = SFZSink("testdata/SFZSink/sw_default9.sfz");
    EXPECT_EQ(getSfzData(sfz_test9).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test9), default_sw_last);

    create_file("testdata/SFZSink/sw_default10.sfz",
                "<region> sw_default=60test\n"
                "");
    SFZSink sfz_test10 = SFZSink("testdata/SFZSink/sw_default10.sfz");
    EXPECT_EQ(getSfzData(sfz_test10).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test10), default_sw_last);

    create_file("testdata/SFZSink/sw_default11.sfz",
                "<region> sw_default=あ\n"
                "");
    SFZSink sfz_test11 = SFZSink("testdata/SFZSink/sw_default11.sfz");
    EXPECT_EQ(getSfzData(sfz_test11).size(), 1);
    EXPECT_EQ(getSfzDataSwkey(sfz_test11), default_sw_last);
}
