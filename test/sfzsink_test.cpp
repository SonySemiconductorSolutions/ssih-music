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
    virtual void SetUp() {
        char *data = new char[1024 * 4 + 1];
        if (data) {
            memset(data, 0x00, 1024 * 4 + 1);
            memset(data, 1, 1024 * 4);
        }
        create_file("testdata/SFZSink/test.raw", data);
        delete[] data;
    }
    std::vector<SFZSink::Region> regions;
};

TEST_F(SfzTest, kyoukai_lokey1) {
    create_file("testdata/SFZSink/kyoukai_lokey1.sfz",
                "<region> sample=test.raw lokey=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_lokey2) {
    create_file("testdata/SFZSink/kyoukai_lokey2.sfz",
                "<region> sample=test.raw lokey=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 0);
    }
}

TEST_F(SfzTest, kyoukai_lokey3) {
    create_file("testdata/SFZSink/kyoukai_lokey3.sfz",
                "<region> sample=test.raw lokey=1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 1);
    }
}

TEST_F(SfzTest, kyoukai_lokey4) {
    create_file("testdata/SFZSink/kyoukai_lokey4.sfz",
                "<region> sample=test.raw lokey=60\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 60);
    }
}

TEST_F(SfzTest, kyoukai_lokey5) {
    create_file("testdata/SFZSink/kyoukai_lokey5.sfz",
                "<region> sample=test.raw lokey=126\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 126);
    }
}

TEST_F(SfzTest, kyoukai_lokey6) {
    create_file("testdata/SFZSink/kyoukai_lokey6.sfz",
                "<region> sample=test.raw lokey=127\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 127);
    }
}

TEST_F(SfzTest, kyoukai_lokey7) {
    create_file("testdata/SFZSink/kyoukai_lokey7.sfz",
                "<region> sample=test.raw lokey=128\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_lokey8) {
    create_file("testdata/SFZSink/kyoukai_lokey8.sfz",
                "<region> sample=test.raw lokey==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_lokey9) {
    create_file("testdata/SFZSink/kyoukai_lokey9.sfz",
                "<region> sample=test.raw lokey=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_lokey10) {
    create_file("testdata/SFZSink/kyoukai_lokey10.sfz",
                "<region> sample=test.raw lokey=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_lokey11) {
    create_file("testdata/SFZSink/kyoukai_lokey11.sfz",
                "<region> sample=test.raw lokey=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_lokey11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_hikey1) {
    create_file("testdata/SFZSink/kyoukai_hikey1.sfz",
                "<region> sample=test.raw hikey=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_hikey2) {
    create_file("testdata/SFZSink/kyoukai_hikey2.sfz",
                "<region> sample=test.raw hikey=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 0);
    }
}

TEST_F(SfzTest, kyoukai_hikey3) {
    create_file("testdata/SFZSink/kyoukai_hikey3.sfz",
                "<region> sample=test.raw hikey=1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 1);
    }
}

TEST_F(SfzTest, kyoukai_hikey4) {
    create_file("testdata/SFZSink/kyoukai_hikey4.sfz",
                "<region> sample=test.raw hikey=60\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 60);
    }
}

TEST_F(SfzTest, kyoukai_hikey5) {
    create_file("testdata/SFZSink/kyoukai_hikey5.sfz",
                "<region> sample=test.raw hikey=126\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 126);
    }
}

TEST_F(SfzTest, kyoukai_hikey6) {
    create_file("testdata/SFZSink/kyoukai_hikey6.sfz",
                "<region> sample=test.raw hikey=127\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 127);
    }
}

TEST_F(SfzTest, kyoukai_hikey7) {
    create_file("testdata/SFZSink/kyoukai_hikey7.sfz",
                "<region> sample=test.raw hikey=128\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_hikey8) {
    create_file("testdata/SFZSink/kyoukai_hikey8.sfz",
                "<region> sample=test.raw hikey==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_hikey9) {
    create_file("testdata/SFZSink/kyoukai_hikey9.sfz",
                "<region> sample=test.raw hikey=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_hikey10) {
    create_file("testdata/SFZSink/kyoukai_hikey10.sfz",
                "<region> sample=test.raw hikey=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_hikey11) {
    create_file("testdata/SFZSink/kyoukai_hikey11.sfz",
                "<region> sample=test.raw hikey=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_hikey11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_key1) {
    create_file("testdata/SFZSink/kyoukai_key1.sfz",
                "<region> sample=test.raw key=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_key2) {
    create_file("testdata/SFZSink/kyoukai_key2.sfz",
                "<region> sample=test.raw key=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 0);
        EXPECT_EQ(regions[0].hikey, 0);
    }
}

TEST_F(SfzTest, kyoukai_key3) {
    create_file("testdata/SFZSink/kyoukai_key3.sfz",
                "<region> sample=test.raw key=1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 1);
        EXPECT_EQ(regions[0].hikey, 1);
    }
}

TEST_F(SfzTest, kyoukai_key4) {
    create_file("testdata/SFZSink/kyoukai_key4.sfz",
                "<region> sample=test.raw key=60\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 60);
        EXPECT_EQ(regions[0].hikey, 60);
    }
}

TEST_F(SfzTest, kyoukai_key5) {
    create_file("testdata/SFZSink/kyoukai_key5.sfz",
                "<region> sample=test.raw key=126\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 126);
        EXPECT_EQ(regions[0].hikey, 126);
    }
}

TEST_F(SfzTest, kyoukai_key6) {
    create_file("testdata/SFZSink/kyoukai_key6.sfz",
                "<region> sample=test.raw key=127\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 127);
        EXPECT_EQ(regions[0].hikey, 127);
    }
}

TEST_F(SfzTest, kyoukai_key7) {
    create_file("testdata/SFZSink/kyoukai_key7.sfz",
                "<region> sample=test.raw key=128\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_key8) {
    create_file("testdata/SFZSink/kyoukai_key8.sfz",
                "<region> sample=test.raw key==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_key9) {
    create_file("testdata/SFZSink/kyoukai_key9.sfz",
                "<region> sample=test.raw key=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_key10) {
    create_file("testdata/SFZSink/kyoukai_key10.sfz",
                "<region> sample=test.raw key=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_key11) {
    create_file("testdata/SFZSink/kyoukai_key11.sfz",
                "<region> sample=test.raw key=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_key11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_offset1) {
    create_file("testdata/SFZSink/kyoukai_offset1.sfz",
                "<region> sample=test.raw offset=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_offset2) {
    create_file("testdata/SFZSink/kyoukai_offset2.sfz",
                "<region> sample=test.raw offset=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].offset, 0);
    }
}

TEST_F(SfzTest, kyoukai_offset3) {
    create_file("testdata/SFZSink/kyoukai_offset3.sfz",
                "<region> sample=test.raw offset=1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].offset, 4);
    }
}

TEST_F(SfzTest, kyoukai_offset4) {
    create_file("testdata/SFZSink/kyoukai_offset4.sfz",
                "<region> sample=test.raw offset=10000\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].offset, 4096);
    }
}

TEST_F(SfzTest, kyoukai_offset5) {
    create_file("testdata/SFZSink/kyoukai_offset5.sfz",
                "<region> sample=test.raw offset=2147483647\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].offset, 4096);
    }
}

TEST_F(SfzTest, kyoukai_offset6) {
    create_file("testdata/SFZSink/kyoukai_offset6.sfz",
                "<region> sample=test.raw offset=2147483648\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].offset, 4096);
    }
}

TEST_F(SfzTest, kyoukai_offset7) {
    create_file("testdata/SFZSink/kyoukai_offset7.sfz",
                "<region> sample=test.raw offset=4294967294\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].offset, 4096);
    }
}

TEST_F(SfzTest, kyoukai_offset8) {
    create_file("testdata/SFZSink/kyoukai_offset8.sfz",
                "<region> sample=test.raw offset=4294967295\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].offset, 4096);
    }
}

TEST_F(SfzTest, kyoukai_offset9) {
    create_file("testdata/SFZSink/kyoukai_offset9.sfz",
                "<region> sample=test.raw offset=4294967296\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_offset10) {
    create_file("testdata/SFZSink/kyoukai_offset10.sfz",
                "<region> sample=test.raw offset==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_offset11) {
    create_file("testdata/SFZSink/kyoukai_offset11.sfz",
                "<region> sample=test.raw offset=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_offset12) {
    create_file("testdata/SFZSink/kyoukai_offset12.sfz",
                "<region> sample=test.raw offset=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset12.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_offset13) {
    create_file("testdata/SFZSink/kyoukai_offset13.sfz",
                "<region> sample=test.raw offset=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_offset13.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_end1) {
    create_file("testdata/SFZSink/kyoukai_end1.sfz",
                "<region> sample=test.raw end=-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_end2) {
    create_file("testdata/SFZSink/kyoukai_end2.sfz",
                "<region> sample=test.raw end=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].silence, true);
    }
}

TEST_F(SfzTest, kyoukai_end3) {
    create_file("testdata/SFZSink/kyoukai_end3.sfz",
                "<region> sample=test.raw end=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].end, 4);
    }
}

TEST_F(SfzTest, kyoukai_end4) {
    create_file("testdata/SFZSink/kyoukai_end4.sfz",
                "<region> sample=test.raw end=10000\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].end, 4096);
    }
}

TEST_F(SfzTest, kyoukai_end5) {
    create_file("testdata/SFZSink/kyoukai_end5.sfz",
                "<region> sample=test.raw end=2147483647\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].end, 4096);
    }
}

TEST_F(SfzTest, kyoukai_end6) {
    create_file("testdata/SFZSink/kyoukai_end6.sfz",
                "<region> sample=test.raw end=2147483648\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].end, 4096);
    }
}

TEST_F(SfzTest, kyoukai_end7) {
    create_file("testdata/SFZSink/kyoukai_end7.sfz",
                "<region> sample=test.raw end=4294967294\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].end, 4096);
    }
}

TEST_F(SfzTest, kyoukai_end8) {
    create_file("testdata/SFZSink/kyoukai_end8.sfz",
                "<region> sample=test.raw end=4294967295\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].end, 4096);
    }
}

TEST_F(SfzTest, kyoukai_end9) {
    create_file("testdata/SFZSink/kyoukai_end9.sfz",
                "<region> sample=test.raw end=4294967296\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_end10) {
    create_file("testdata/SFZSink/kyoukai_end.sfz",
                "<region> sample=test.raw end==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_end11) {
    create_file("testdata/SFZSink/kyoukai_end11.sfz",
                "<region> sample=test.raw end=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_end12) {
    create_file("testdata/SFZSink/kyoukai_end12.sfz",
                "<region> sample=test.raw end=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end12.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_end13) {
    create_file("testdata/SFZSink/kyoukai_end13.sfz",
                "<region> sample=test.raw end=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_end13.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_count1) {
    create_file("testdata/SFZSink/kyoukai_count1.sfz",
                "<region> sample=test.raw count=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_count2) {
    create_file("testdata/SFZSink/kyoukai_count2.sfz",
                "<region> sample=test.raw count=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].count, 0);
    }
}

TEST_F(SfzTest, kyoukai_count3) {
    create_file("testdata/SFZSink/kyoukai_count3.sfz",
                "<region> sample=test.raw count=1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].count, 1);
    }
}

TEST_F(SfzTest, kyoukai_count4) {
    create_file("testdata/SFZSink/kyoukai_count4.sfz",
                "<region> sample=test.raw count=10000\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].count, 10000);
    }
}

TEST_F(SfzTest, kyoukai_count5) {
    create_file("testdata/SFZSink/kyoukai_count5.sfz",
                "<region> sample=test.raw count=2147483647\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].count, 2147483647);
    }
}

TEST_F(SfzTest, kyoukai_count6) {
    create_file("testdata/SFZSink/kyoukai_count6.sfz",
                "<region> sample=test.raw count=2147483648\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].count, 2147483648);
    }
}

TEST_F(SfzTest, kyoukai_count7) {
    create_file("testdata/SFZSink/kyoukai_count7.sfz",
                "<region> sample=test.raw count=4294967294\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].count, 4294967294);
    }
}

TEST_F(SfzTest, kyoukai_count8) {
    create_file("testdata/SFZSink/kyoukai_count8.sfz",
                "<region> sample=test.raw count=4294967295\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].count, 4294967295);
    }
}

TEST_F(SfzTest, kyoukai_count9) {
    create_file("testdata/SFZSink/kyoukai_count9.sfz",
                "<region> sample=test.raw count=4294967296\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_count10) {
    create_file("testdata/SFZSink/kyoukai_count10.sfz",
                "<region> sample=test.raw count==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_count11) {
    create_file("testdata/SFZSink/kyoukai_count11.sfz",
                "<region> sample=test.raw count=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_count12) {
    create_file("testdata/SFZSink/kyoukai_count12.sfz",
                "<region> sample=test.raw count=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count12.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, kyoukai_count13) {
    create_file("testdata/SFZSink/kyoukai_count13.sfz",
                "<region> sample=test.raw count=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_count13.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_start1) {
    create_file("testdata/SFZSink/kyoukai_loop_start1.sfz",
                "<region> sample=test.raw loop_start=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_start2) {
    create_file("testdata/SFZSink/kyoukai_loop_start2.sfz",
                "<region> sample=test.raw loop_start=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_start, 0);
    }
}

TEST_F(SfzTest, loop_start3) {
    create_file("testdata/SFZSink/kyoukai_loop_start3.sfz",
                "<region> sample=test.raw loop_start=1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_start, 4);
    }
}

TEST_F(SfzTest, loop_start4) {
    create_file("testdata/SFZSink/kyoukai_loop_start4.sfz",
                "<region> sample=test.raw loop_start=10000\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 7) {
        EXPECT_EQ(regions[0].loop_start, 4096);
    }
}

TEST_F(SfzTest, loop_start5) {
    create_file("testdata/SFZSink/kyoukai_loop_start5.sfz",
                "<region> sample=test.raw loop_start=2147483647\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_start, 4096);
    }
}

TEST_F(SfzTest, loop_start6) {
    create_file("testdata/SFZSink/kyoukai_loop_start6.sfz",
                "<region> sample=test.raw loop_start=2147483648\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_start, 4096);
    }
}

TEST_F(SfzTest, loop_start7) {
    create_file("testdata/SFZSink/kyoukai_loop_start7.sfz",
                "<region> sample=test.raw loop_start=4294967294\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_start, 4096);
    }
}

TEST_F(SfzTest, loop_start8) {
    create_file("testdata/SFZSink/kyoukai_loop_start8.sfz",
                "<region> sample=test.raw loop_start=4294967295\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_start, 4096);
    }
}

TEST_F(SfzTest, loop_start9) {
    create_file("testdata/SFZSink/kyoukai_loop_start9.sfz",
                "<region> sample=test.raw loop_start=4294967296\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_start10) {
    create_file("testdata/SFZSink/kyoukai_loop_start10.sfz",
                "<region> sample=test.raw loop_start==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_start11) {
    create_file("testdata/SFZSink/kyoukai_loop_start11.sfz",
                "<region> sample=test.raw loop_start=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_start12) {
    create_file("testdata/SFZSink/kyoukai_loop_start12.sfz",
                "<region> sample=test.raw loop_start=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start12.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_start13) {
    create_file("testdata/SFZSink/kyoukai_loop_start13.sfz",
                "<region> sample=test.raw loop_start=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_start13.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_end1) {
    create_file("testdata/SFZSink/kyoukai_loop_end1.sfz",
                "<region> sample=test.raw loop_end=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_end2) {
    create_file("testdata/SFZSink/kyoukai_loop_end2.sfz",
                "<region> sample=test.raw loop_end=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_end, 4);
    }
}

TEST_F(SfzTest, loop_end3) {
    create_file("testdata/SFZSink/kyoukai_loop_end3.sfz",
                "<region> sample=test.raw loop_end=1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_end, 8);
    }
}

TEST_F(SfzTest, loop_end4) {
    create_file("testdata/SFZSink/kyoukai_loop_end4.sfz",
                "<region> sample=test.raw loop_end=10000\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_end, 4096);
    }
}

TEST_F(SfzTest, loop_end5) {
    create_file("testdata/SFZSink/kyoukai_loop_end5.sfz",
                "<region> sample=test.raw loop_end=2147483647\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_end, 4096);
    }
}

TEST_F(SfzTest, loop_end6) {
    create_file("testdata/SFZSink/kyoukai_loop_end6.sfz",
                "<region> sample=test.raw loop_end=2147483648\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_end, 4096);
    }
}

TEST_F(SfzTest, loop_end7) {
    create_file("testdata/SFZSink/kyoukai_loop_end7.sfz",
                "<region> sample=test.raw loop_end=4294967294\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_end, 4096);
    }
}

TEST_F(SfzTest, loop_end8) {
    create_file("testdata/SFZSink/kyoukai_loop_end8.sfz",
                "<region> sample=test.raw loop_end=4294967295\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_end, 4096);
    }
}

TEST_F(SfzTest, loop_end9) {
    create_file("testdata/SFZSink/kyoukai_loop_end9.sfz",
                "<region> sample=test.raw loop_end=4294967296\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_end10) {
    create_file("testdata/SFZSink/kyoukai_loop_end10.sfz",
                "<region> sample=test.raw loop_end==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_end11) {
    create_file("testdata/SFZSink/kyoukai_loop_end11.sfz",
                "<region> sample=test.raw loop_end=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_end12) {
    create_file("testdata/SFZSink/kyoukai_loop_end12.sfz",
                "<region> sample=test.raw loop_end=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end12.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, loop_end13) {
    create_file("testdata/SFZSink/kyoukai_loop_end13.sfz",
                "<region> sample=test.raw loop_end=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/kyoukai_loop_end13.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, verify_sample1) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/guitar_c4_ff.wav", data);
    delete[] data;

    create_file("testdata/SFZSink/verify_sample1.sfz",
                "<region> sample=guitar_c4_ff.wav\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        const String default_sample = "";
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/guitar_c4_ff.wav");
    }
}

TEST_F(SfzTest, verify_sample2) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/dog kick.ogg", data);
    delete[] data;

    create_file("testdata/SFZSink/verify_sample2.sfz",
                "<region> sample=dog kick.ogg\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        const String default_sample = "";
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/dog kick.ogg");
    }
}

TEST_F(SfzTest, verify_sample3) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/out of tune trombone (redundant).wav", data);
    delete[] data;

    create_file("testdata/SFZSink/verify_sample3.sfz",
                "<region> sample=out of tune trombone (redundant).wav\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/out of tune trombone (redundant).wav");
    }
}

TEST_F(SfzTest, verify_sample4) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/staccatto_snare.ogg", data);
    delete[] data;

    create_file("testdata/SFZSink/verify_sample4.sfz",
                "<region> sample=staccatto_snare.ogg\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/staccatto_snare.ogg");
    }
}

TEST_F(SfzTest, verify_sample5) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/dog      kick.ogg", data);
    delete[] data;

    create_file("testdata/SFZSink/verify_sample5.sfz",
                "<region> sample=dog      kick.ogg\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/dog      kick.ogg");
    }
}

TEST_F(SfzTest, verify_sample6) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/dog　kick.ogg", data);
    delete[] data;

    create_file("testdata/SFZSink/verify_sample6.sfz",
                "<region> sample=dog　kick.ogg\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/dog　kick.ogg");
    }
}

TEST_F(SfzTest, verify_sample7) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/てすと.wav", data);
    delete[] data;

    create_file("testdata/SFZSink/verify_sample7.sfz",
                "<region> sample=てすと.wav\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/てすと.wav");
    }
}

TEST_F(SfzTest, verify_sample8) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/guitar=c4_ff.wav", data);
    delete[] data;

    create_file("testdata/SFZSink/verify_sample8.sfz",
                "<region> sample=guitar=c4_ff.wav\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_sample8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/guitar=c4_ff.wav");
    }
}

TEST_F(SfzTest, verify_loop_mode1) {
    create_file("testdata/SFZSink/verify_loop_mode1.sfz",
                "<region> sample=test.raw loop_mode=no_loop\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_mode, SFZSink::kNoLoop);
    }
}

TEST_F(SfzTest, verify_loop_mode2) {
    create_file("testdata/SFZSink/verify_loop_mode2.sfz",
                "<region> sample=test.raw loop_mode=one_shot\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_mode, SFZSink::kOneShot);
    }
}

TEST_F(SfzTest, verify_loop_mode3) {
    create_file("testdata/SFZSink/verify_loop_mode3.sfz",
                "<region> sample=test.raw loop_mode=loop_continuous\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_mode, SFZSink::kLoopContinuous);
    }
}

TEST_F(SfzTest, verify_loop_mode4) {
    create_file("testdata/SFZSink/verify_loop_mode4.sfz",
                "<region> sample=test.raw loop_mode=loop_sustain\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].loop_mode, SFZSink::kLoopSustain);
    }
}

TEST_F(SfzTest, verify_loop_mode5) {
    create_file("testdata/SFZSink/verify_loop_mode5.sfz",
                "<region> sample=test.raw loop_mode=test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, verify_loop_mode6) {
    create_file("testdata/SFZSink/verify_loop_mode6.sfz",
                "<region> sample=test.raw loop_mode=123\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, verify_loop_mode7) {
    create_file("testdata/SFZSink/verify_loop_mode7.sfz",
                "<region> sample=test.raw loop_mode=ああああ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, verify_loop_mode8) {
    create_file("testdata/SFZSink/verify_loop_mode8.sfz",
                "<region> sample=test.raw loop_mode==no_loop\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, verify_loop_mode9) {
    create_file("testdata/SFZSink/verify_loop_mode9.sfz",
                "<region> sample=test.raw loop_mode==nno_loop\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, verify_loop_mode10) {
    create_file("testdata/SFZSink/verify_loop_mode10.sfz",
                "<region> sample=test.raw loop_mode==one_shott\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/verify_loop_mode10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
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
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/test1_space.raw", data);
    create_file("testdata/SFZSink/test2_tab.raw", data);
    delete[] data;

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

TEST_F(SfzTest, define1) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/$AAtest1_spacedef1.raw", data);
    delete[] data;

    create_file("testdata/SFZSink/define1.sfz",
                "#define    $A def1  \n"
                "#define\t$AA\tdef2    \n"
                "<region> sample=$AAtest1_space$A.raw\n"
                "");

    SFZSink sfz_test = SFZSink("testdata/SFZSink/define1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/$AAtest1_spacedef1.raw");
    }
}

TEST_F(SfzTest, define2) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/test2.raw", data);
    delete[] data;

    create_file("testdata/SFZSink/define2.sfz",
                "#define\t$PLE ple    \n"
                "<region> sam$PLE=test2.raw\n"
                "");

    SFZSink sfz_test = SFZSink("testdata/SFZSink/define2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/test2.raw");
    }
}

TEST_F(SfzTest, define3) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/$ERR1test3.raw", data);
    delete[] data;

    create_file("testdata/SFZSink/define3.sfz",
                "#define\t$ERR1\n"
                "<region> sample=$ERR1test3.raw\n"
                "");

    SFZSink sfz_test = SFZSink("testdata/SFZSink/define3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/$ERR1test3.raw");
    }
}

TEST_F(SfzTest, define4) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/ERR2test4.raw", data);
    delete[] data;

    create_file("testdata/SFZSink/define4.sfz",
                "#define ERR2 error2\n"
                "<region> sample=ERR2test4.raw\n"
                "");

    SFZSink sfz_test = SFZSink("testdata/SFZSink/define4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/ERR2test4.raw");
    }
}

TEST_F(SfzTest, define5) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/$TEST5test5.raw", data);
    delete[] data;

    create_file("testdata/SFZSink/define5.sfz",
                "#define $TEST5 \n"
                "<region> sample=$TEST5test5.raw\n"
                "");

    SFZSink sfz_test = SFZSink("testdata/SFZSink/define5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/$TEST5test5.raw");
    }
}

TEST_F(SfzTest, define6) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/$TEST6test6.raw", data);
    delete[] data;

    create_file("testdata/SFZSink/define6.sfz",
                "#define $TEST6     \n"
                "<region> sample=$TEST6test6.raw\n"
                "");

    SFZSink sfz_test = SFZSink("testdata/SFZSink/define6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/$TEST6test6.raw");
    }
}

TEST_F(SfzTest, define7) {
    char *data = new char[1024 * 4 + 1];
    if (data) {
        memset(data, 0x00, 1024 * 4 + 1);
        memset(data, 1, 1024 * 4);
    }
    create_file("testdata/SFZSink/$test7.raw", data);
    delete[] data;

    create_file("testdata/SFZSink/define7.sfz",
                "#define $     \n"
                "<region> sample=$test7.raw\n"
                "");

    SFZSink sfz_test = SFZSink("testdata/SFZSink/define7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sample, "testdata/SFZSink/$test7.raw");
    }
}

TEST_F(SfzTest, sw_last1) {
    create_file("testdata/SFZSink/sw_last1.sfz",
                "<region> sample=test.raw sw_last=-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, sw_last2) {
    create_file("testdata/SFZSink/sw_last2.sfz",
                "<region> sample=test.raw sw_last=0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sw_last, 0);
    }
}

TEST_F(SfzTest, sw_last3) {
    create_file("testdata/SFZSink/sw_last3.sfz",
                "<region> sample=test.raw sw_last=1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sw_last, 1);
    }
}

TEST_F(SfzTest, sw_last4) {
    create_file("testdata/SFZSink/sw_last4.sfz",
                "<region> sample=test.raw sw_last=60\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sw_last, 60);
    }
}

TEST_F(SfzTest, sw_last5) {
    create_file("testdata/SFZSink/sw_last5.sfz",
                "<region> sample=test.raw sw_last=126\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sw_last, 126);
    }
}

TEST_F(SfzTest, sw_last6) {
    create_file("testdata/SFZSink/sw_last6.sfz",
                "<region> sample=test.raw sw_last=127\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].sw_last, 127);
    }
}

TEST_F(SfzTest, sw_last7) {
    create_file("testdata/SFZSink/sw_last7.sfz",
                "<region> sample=test.raw sw_last=128\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, sw_last8) {
    create_file("testdata/SFZSink/sw_last8.sfz",
                "<region> sample=test.raw sw_last==64\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, sw_last9) {
    create_file("testdata/SFZSink/sw_last9.sfz",
                "<region> sample=test.raw sw_last=aaaa\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, sw_last10) {
    create_file("testdata/SFZSink/sw_last10.sfz",
                "<region> sample=test.raw sw_last=60test\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, sw_last11) {
    create_file("testdata/SFZSink/sw_last11.sfz",
                "<region> sample=test.raw sw_last=あ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/sw_last11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, sw_lokey_hikey1) {
    create_file("testdata/SFZSink/sw_lokey_hikey1.sfz",
                "<region> sample=test.raw sw_lokey=-1 sw_hikey=-1\n"
                "");
    SFZSink sfz_test1 = SFZSink("testdata/SFZSink/sw_lokey_hikey1.sfz");
    EXPECT_EQ(getSfzData(sfz_test1).size(), 0);
}

TEST_F(SfzTest, sw_lokey_hikey2) {
    create_file("testdata/SFZSink/sw_lokey_hikey2.sfz",
                "<region> sample=test.raw sw_lokey=0 sw_hikey=0\n"
                "");
    SFZSink sfz_test2 = SFZSink("testdata/SFZSink/sw_lokey_hikey2.sfz");
    EXPECT_EQ(getSfzData(sfz_test2).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test2), 0);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test2), 0);
}

TEST_F(SfzTest, sw_lokey_hikey3) {
    create_file("testdata/SFZSink/sw_lokey_hikey3.sfz",
                "<region> sample=test.raw sw_lokey=1 sw_hikey=1\n"
                "");
    SFZSink sfz_test3 = SFZSink("testdata/SFZSink/sw_lokey_hikey3.sfz");
    EXPECT_EQ(getSfzData(sfz_test3).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test3), 1);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test3), 1);
}

TEST_F(SfzTest, sw_lokey_hikey4) {
    create_file("testdata/SFZSink/sw_lokey_hikey4.sfz",
                "<region> sample=test.raw sw_lokey=60 sw_hikey=60\n"
                "");
    SFZSink sfz_test4 = SFZSink("testdata/SFZSink/sw_lokey_hikey4.sfz");
    EXPECT_EQ(getSfzData(sfz_test4).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test4), 60);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test4), 60);
}

TEST_F(SfzTest, sw_lokey_hikey5) {
    create_file("testdata/SFZSink/sw_lokey_hikey5.sfz",
                "<region> sample=test.raw sw_lokey=126 sw_hikey=126\n"
                "");
    SFZSink sfz_test5 = SFZSink("testdata/SFZSink/sw_lokey_hikey5.sfz");
    EXPECT_EQ(getSfzData(sfz_test5).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test5), 126);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test5), 126);
}

TEST_F(SfzTest, sw_lokey_hikey6) {
    create_file("testdata/SFZSink/sw_lokey_hikey6.sfz",
                "<region> sample=test.raw sw_lokey=127 sw_hikey=127\n"
                "");
    SFZSink sfz_test6 = SFZSink("testdata/SFZSink/sw_lokey_hikey6.sfz");
    EXPECT_EQ(getSfzData(sfz_test6).size(), 1);
    EXPECT_EQ(getSfzDataSwlokey(sfz_test6), 127);
    EXPECT_EQ(getSfzDataSwhikey(sfz_test6), 127);
}

TEST_F(SfzTest, sw_lokey_hikey7) {
    create_file("testdata/SFZSink/sw_lokey_hikey7.sfz",
                "<region> sample=test.raw sw_lokey=128 sw_hikey=128\n"
                "\n");
    SFZSink sfz_test7 = SFZSink("testdata/SFZSink/sw_lokey_hikey7.sfz");
    EXPECT_EQ(getSfzData(sfz_test7).size(), 0);
}

TEST_F(SfzTest, sw_lokey_hikey8) {
    create_file("testdata/SFZSink/sw_lokey_hikey8.sfz",
                "<region> sample=test.raw sw_lokey==64 sw_hikey==64\n"
                "");
    SFZSink sfz_test8 = SFZSink("testdata/SFZSink/sw_lokey_hikey8.sfz");
    EXPECT_EQ(getSfzData(sfz_test8).size(), 0);
}

TEST_F(SfzTest, sw_lokey_hikey9) {
    create_file("testdata/SFZSink/sw_lokey_hikey9.sfz",
                "<region> sample=test.raw sw_lokey=aaaa sw_hikey=aaaa\n"
                "");
    SFZSink sfz_test9 = SFZSink("testdata/SFZSink/sw_lokey_hikey9.sfz");
    EXPECT_EQ(getSfzData(sfz_test9).size(), 0);
}

TEST_F(SfzTest, sw_lokey_hikey10) {
    create_file("testdata/SFZSink/sw_lokey_hikey10.sfz",
                "<region> sample=test.raw sw_lokey=60test sw_hikey=60test\n"
                "");
    SFZSink sfz_test10 = SFZSink("testdata/SFZSink/sw_lokey_hikey10.sfz");
    EXPECT_EQ(getSfzData(sfz_test10).size(), 0);
}

TEST_F(SfzTest, sw_default11) {
    create_file("testdata/SFZSink/sw_default11.sfz",
                "<region> sample=test.raw sw_default=あ\n"
                "");
    SFZSink sfz_test11 = SFZSink("testdata/SFZSink/sw_default11.sfz");
    EXPECT_EQ(getSfzData(sfz_test11).size(), 0);
}
