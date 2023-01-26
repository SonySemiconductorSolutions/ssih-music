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

class SfzTest : public ::testing::Test {
public:
    int getSfzDataNum(SFZSink &sfz_test) {
        return sfz_test.regions_.size();
    }
    int getSfzDataSwlokey(SFZSink &sfz_test) {
        return sfz_test.sw_lokey_;
    }
    int getSfzDataSwhikey(SFZSink &sfz_test) {
        return sfz_test.sw_hikey_;
    }
    int getSfzDataSwkey(SFZSink &sfz_test) {
        return sfz_test.sw_last_;
    }
    std::vector<SFZSink::Region> getSfzData(SFZSink &sfz_test) {
        sfz_test.begin();
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

TEST_F(SfzTest, lokey_notename1) {
    create_file("testdata/SFZSink/lokey_notename1.sfz",
                "<region> sample=test.raw lokey=Cb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 107);
    }
}

TEST_F(SfzTest, lokey_notename2) {
    create_file("testdata/SFZSink/lokey_notename2.sfz",
                "<region> sample=test.raw lokey=C-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 0);
    }
}

TEST_F(SfzTest, lokey_notename3) {
    create_file("testdata/SFZSink/lokey_notename3.sfz",
                "<region> sample=test.raw lokey=C8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 108);
    }
}

TEST_F(SfzTest, lokey_notename4) {
    create_file("testdata/SFZSink/lokey_notename4.sfz",
                "<region> sample=test.raw lokey=C#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 13);
    }
}

TEST_F(SfzTest, lokey_notename5) {
    create_file("testdata/SFZSink/lokey_notename5.sfz",
                "<region> sample=test.raw lokey=C#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 49);
    }
}

TEST_F(SfzTest, lokey_notename6) {
    create_file("testdata/SFZSink/lokey_notename6.sfz",
                "<region> sample=test.raw lokey=Db3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 49);
    }
}

TEST_F(SfzTest, lokey_notename7) {
    create_file("testdata/SFZSink/lokey_notename7.sfz",
                "<region> sample=test.raw lokey=Db9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 121);
    }
}

TEST_F(SfzTest, lokey_notename8) {
    create_file("testdata/SFZSink/lokey_notename8.sfz",
                "<region> sample=test.raw lokey=D-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 2);
    }
}

TEST_F(SfzTest, lokey_notename9) {
    create_file("testdata/SFZSink/lokey_notename9.sfz",
                "<region> sample=test.raw lokey=D8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 110);
    }
}

TEST_F(SfzTest, lokey_notename10) {
    create_file("testdata/SFZSink/lokey_notename10.sfz",
                "<region> sample=test.raw lokey=D#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 15);
    }
}

TEST_F(SfzTest, lokey_notename11) {
    create_file("testdata/SFZSink/lokey_notename11.sfz",
                "<region> sample=test.raw lokey=Eb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 51);
    }
}

TEST_F(SfzTest, lokey_notename12) {
    create_file("testdata/SFZSink/lokey_notename12.sfz",
                "<region> sample=test.raw lokey=Eb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename12.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 111);
    }
}

TEST_F(SfzTest, lokey_notename13) {
    create_file("testdata/SFZSink/lokey_notename13.sfz",
                "<region> sample=test.raw lokey=E-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename13.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 4);
    }
}

TEST_F(SfzTest, lokey_notename14) {
    create_file("testdata/SFZSink/lokey_notename14.sfz",
                "<region> sample=test.raw lokey=E#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename14.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 17);
    }
}

TEST_F(SfzTest, lokey_notename15) {
    create_file("testdata/SFZSink/lokey_notename15.sfz",
                "<region> sample=test.raw lokey=E#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename15.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 125);
    }
}

TEST_F(SfzTest, lokey_notename16) {
    create_file("testdata/SFZSink/lokey_notename16.sfz",
                "<region> sample=test.raw lokey=Fb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename16.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 16);
    }
}

TEST_F(SfzTest, lokey_notename17) {
    create_file("testdata/SFZSink/lokey_notename17.sfz",
                "<region> sample=test.raw lokey=Fb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename17.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 52);
    }
}

TEST_F(SfzTest, lokey_notename18) {
    create_file("testdata/SFZSink/lokey_notename18.sfz",
                "<region> sample=test.raw lokey=F8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename18.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 113);
    }
}

TEST_F(SfzTest, lokey_notename19) {
    create_file("testdata/SFZSink/lokey_notename19.sfz",
                "<region> sample=test.raw lokey=F#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename19.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 6);
    }
}

TEST_F(SfzTest, lokey_notename20) {
    create_file("testdata/SFZSink/lokey_notename20.sfz",
                "<region> sample=test.raw lokey=F#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename20.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 126);
    }
}

TEST_F(SfzTest, lokey_notename21) {
    create_file("testdata/SFZSink/lokey_notename21.sfz",
                "<region> sample=test.raw lokey=Gb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename21.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 114);
    }
}

TEST_F(SfzTest, lokey_notename22) {
    create_file("testdata/SFZSink/lokey_notename22.sfz",
                "<region> sample=test.raw lokey=Gb9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename22.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 126);
    }
}

TEST_F(SfzTest, lokey_notename23) {
    create_file("testdata/SFZSink/lokey_notename23.sfz",
                "<region> sample=test.raw lokey=G-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename23.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 7);
    }
}

TEST_F(SfzTest, lokey_notename24) {
    create_file("testdata/SFZSink/lokey_notename24.sfz",
                "<region> sample=test.raw lokey=G0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename24.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 19);
    }
}

TEST_F(SfzTest, lokey_notename25) {
    create_file("testdata/SFZSink/lokey_notename25.sfz",
                "<region> sample=test.raw lokey=G#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename25.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 56);
    }
}

TEST_F(SfzTest, lokey_notename26) {
    create_file("testdata/SFZSink/lokey_notename26.sfz",
                "<region> sample=test.raw lokey=Ab0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename26.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 20);
    }
}

TEST_F(SfzTest, lokey_notename27) {
    create_file("testdata/SFZSink/lokey_notename27.sfz",
                "<region> sample=test.raw lokey=A3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename27.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 57);
    }
}

TEST_F(SfzTest, lokey_notename28) {
    create_file("testdata/SFZSink/lokey_notename28.sfz",
                "<region> sample=test.raw lokey=A#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename28.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 10);
    }
}

TEST_F(SfzTest, lokey_notename29) {
    create_file("testdata/SFZSink/lokey_notename29.sfz",
                "<region> sample=test.raw lokey=A#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename29.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 106);
    }
}

TEST_F(SfzTest, lokey_notename30) {
    create_file("testdata/SFZSink/lokey_notename30.sfz",
                "<region> sample=test.raw lokey=A#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename30.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 118);
    }
}

TEST_F(SfzTest, lokey_notename31) {
    create_file("testdata/SFZSink/lokey_notename31.sfz",
                "<region> sample=test.raw lokey=Bb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename31.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 10);
    }
}

TEST_F(SfzTest, lokey_notename32) {
    create_file("testdata/SFZSink/lokey_notename32.sfz",
                "<region> sample=test.raw lokey=Bb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename32.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 22);
    }
}

TEST_F(SfzTest, lokey_notename33) {
    create_file("testdata/SFZSink/lokey_notename33.sfz",
                "<region> sample=test.raw lokey=B3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename33.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 59);
    }
}

TEST_F(SfzTest, lokey_notename34) {
    create_file("testdata/SFZSink/lokey_notename34.sfz",
                "<region> sample=test.raw lokey=B8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename34.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 119);
    }
}

TEST_F(SfzTest, lokey_notename35) {
    create_file("testdata/SFZSink/lokey_notename35.sfz",
                "<region> sample=test.raw lokey=B#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename35.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 108);
    }
}

TEST_F(SfzTest, lokey_notename36) {
    create_file("testdata/SFZSink/lokey_notename36.sfz",
                "<region> sample=test.raw lokey=cb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename36.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename37) {
    create_file("testdata/SFZSink/lokey_notename37.sfz",
                "<region> sample=test.raw lokey=cb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename37.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 47);
    }
}

TEST_F(SfzTest, lokey_notename38) {
    create_file("testdata/SFZSink/lokey_notename38.sfz",
                "<region> sample=test.raw lokey=cb7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename38.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 95);
    }
}

TEST_F(SfzTest, lokey_notename39) {
    create_file("testdata/SFZSink/lokey_notename39.sfz",
                "<region> sample=test.raw lokey=c9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename39.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 120);
    }
}

TEST_F(SfzTest, lokey_notename40) {
    create_file("testdata/SFZSink/lokey_notename40.sfz",
                "<region> sample=test.raw lokey=c#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename40.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 13);
    }
}

TEST_F(SfzTest, lokey_notename41) {
    create_file("testdata/SFZSink/lokey_notename41.sfz",
                "<region> sample=test.raw lokey=db3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename41.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 49);
    }
}

TEST_F(SfzTest, lokey_notename42) {
    create_file("testdata/SFZSink/lokey_notename42.sfz",
                "<region> sample=test.raw lokey=d-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename42.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 2);
    }
}

TEST_F(SfzTest, lokey_notename43) {
    create_file("testdata/SFZSink/lokey_notename43.sfz",
                "<region> sample=test.raw lokey=d0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename43.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 14);
    }
}

TEST_F(SfzTest, lokey_notename44) {
    create_file("testdata/SFZSink/lokey_notename44.sfz",
                "<region> sample=test.raw lokey=d#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename44.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 111);
    }
}

TEST_F(SfzTest, lokey_notename45) {
    create_file("testdata/SFZSink/lokey_notename45.sfz",
                "<region> sample=test.raw lokey=d#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename45.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 123);
    }
}

TEST_F(SfzTest, lokey_notename46) {
    create_file("testdata/SFZSink/lokey_notename46.sfz",
                "<region> sample=test.raw lokey=eb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename46.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 111);
    }
}

TEST_F(SfzTest, lokey_notename47) {
    create_file("testdata/SFZSink/lokey_notename47.sfz",
                "<region> sample=test.raw lokey=e0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename47.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 16);
    }
}

TEST_F(SfzTest, lokey_notename48) {
    create_file("testdata/SFZSink/lokey_notename48.sfz",
                "<region> sample=test.raw lokey=e#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename48.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 5);
    }
}

TEST_F(SfzTest, lokey_notename49) {
    create_file("testdata/SFZSink/lokey_notename49.sfz",
                "<region> sample=test.raw lokey=e#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename49.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 53);
    }
}

TEST_F(SfzTest, lokey_notename50) {
    create_file("testdata/SFZSink/lokey_notename50.sfz",
                "<region> sample=test.raw lokey=e#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename50.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 125);
    }
}

TEST_F(SfzTest, lokey_notename51) {
    create_file("testdata/SFZSink/lokey_notename51.sfz",
                "<region> sample=test.raw lokey=fb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename51.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 4);
    }
}

TEST_F(SfzTest, lokey_notename52) {
    create_file("testdata/SFZSink/lokey_notename52.sfz",
                "<region> sample=test.raw lokey=f3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename52.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 53);
    }
}

TEST_F(SfzTest, lokey_notename53) {
    create_file("testdata/SFZSink/lokey_notename53.sfz",
                "<region> sample=test.raw lokey=f#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename53.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 18);
    }
}

TEST_F(SfzTest, lokey_notename54) {
    create_file("testdata/SFZSink/lokey_notename54.sfz",
                "<region> sample=test.raw lokey=f#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename54.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 114);
    }
}

TEST_F(SfzTest, lokey_notename55) {
    create_file("testdata/SFZSink/lokey_notename55.sfz",
                "<region> sample=test.raw lokey=f#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename55.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 126);
    }
}

TEST_F(SfzTest, lokey_notename56) {
    create_file("testdata/SFZSink/lokey_notename56.sfz",
                "<region> sample=test.raw lokey=g9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename56.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 127);
    }
}

TEST_F(SfzTest, lokey_notename57) {
    create_file("testdata/SFZSink/lokey_notename57.sfz",
                "<region> sample=test.raw lokey=g#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename57.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 8);
    }
}

TEST_F(SfzTest, lokey_notename58) {
    create_file("testdata/SFZSink/lokey_notename58.sfz",
                "<region> sample=test.raw lokey=g#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename58.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 56);
    }
}

TEST_F(SfzTest, lokey_notename59) {
    create_file("testdata/SFZSink/lokey_notename59.sfz",
                "<region> sample=test.raw lokey=g#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename59.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 104);
    }
}

TEST_F(SfzTest, lokey_notename60) {
    create_file("testdata/SFZSink/lokey_notename60.sfz",
                "<region> sample=test.raw lokey=gb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename60.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 18);
    }
}

TEST_F(SfzTest, lokey_notename61) {
    create_file("testdata/SFZSink/lokey_notename61.sfz",
                "<region> sample=test.raw lokey=ab7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename61.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 104);
    }
}

TEST_F(SfzTest, lokey_notename62) {
    create_file("testdata/SFZSink/lokey_notename62.sfz",
                "<region> sample=test.raw lokey=ab8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename62.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 116);
    }
}

TEST_F(SfzTest, lokey_notename63) {
    create_file("testdata/SFZSink/lokey_notename63.sfz",
                "<region> sample=test.raw lokey=a3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename63.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 57);
    }
}

TEST_F(SfzTest, lokey_notename64) {
    create_file("testdata/SFZSink/lokey_notename64.sfz",
                "<region> sample=test.raw lokey=a#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename64.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 10);
    }
}

TEST_F(SfzTest, lokey_notename65) {
    create_file("testdata/SFZSink/lokey_notename65.sfz",
                "<region> sample=test.raw lokey=a#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename65.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 22);
    }
}

TEST_F(SfzTest, lokey_notename66) {
    create_file("testdata/SFZSink/lokey_notename66.sfz",
                "<region> sample=test.raw lokey=bb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename66.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 22);
    }
}

TEST_F(SfzTest, lokey_notename67) {
    create_file("testdata/SFZSink/lokey_notename67.sfz",
                "<region> sample=test.raw lokey=bb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename67.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 118);
    }
}

TEST_F(SfzTest, lokey_notename68) {
    create_file("testdata/SFZSink/lokey_notename68.sfz",
                "<region> sample=test.raw lokey=b-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename68.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 11);
    }
}

TEST_F(SfzTest, lokey_notename69) {
    create_file("testdata/SFZSink/lokey_notename69.sfz",
                "<region> sample=test.raw lokey=b7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename69.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 107);
    }
}

TEST_F(SfzTest, lokey_notename70) {
    create_file("testdata/SFZSink/lokey_notename70.sfz",
                "<region> sample=test.raw lokey=b#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename70.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 60);
    }
}

TEST_F(SfzTest, lokey_notename71) {
    create_file("testdata/SFZSink/lokey_notename71.sfz",
                "<region> sample=test.raw lokey=C##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename71.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename72) {
    create_file("testdata/SFZSink/lokey_notename72.sfz",
                "<region> sample=test.raw lokey=C#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename72.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename73) {
    create_file("testdata/SFZSink/lokey_notename73.sfz",
                "<region> sample=test.raw lokey=C#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename73.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename74) {
    create_file("testdata/SFZSink/lokey_notename74.sfz",
                "<region> sample=test.raw lokey=C-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename74.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename75) {
    create_file("testdata/SFZSink/lokey_notename75.sfz",
                "<region> sample=test.raw lokey=C10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename75.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename76) {
    create_file("testdata/SFZSink/lokey_notename76.sfz",
                "<region> sample=test.raw lokey=CbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename76.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename77) {
    create_file("testdata/SFZSink/lokey_notename77.sfz",
                "<region> sample=test.raw lokey=D#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename77.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename78) {
    create_file("testdata/SFZSink/lokey_notename78.sfz",
                "<region> sample=test.raw lokey=D#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename78.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename79) {
    create_file("testdata/SFZSink/lokey_notename79.sfz",
                "<region> sample=test.raw lokey=D#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename79.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename80) {
    create_file("testdata/SFZSink/lokey_notename80.sfz",
                "<region> sample=test.raw lokey=D#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename80.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename81) {
    create_file("testdata/SFZSink/lokey_notename81.sfz",
                "<region> sample=test.raw lokey=Db\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename81.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename82) {
    create_file("testdata/SFZSink/lokey_notename82.sfz",
                "<region> sample=test.raw lokey=Db-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename82.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename83) {
    create_file("testdata/SFZSink/lokey_notename83.sfz",
                "<region> sample=test.raw lokey=E#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename83.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename84) {
    create_file("testdata/SFZSink/lokey_notename84.sfz",
                "<region> sample=test.raw lokey=E#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename84.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename85) {
    create_file("testdata/SFZSink/lokey_notename85.sfz",
                "<region> sample=test.raw lokey=E#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename85.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename86) {
    create_file("testdata/SFZSink/lokey_notename86.sfz",
                "<region> sample=test.raw lokey=E-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename86.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename87) {
    create_file("testdata/SFZSink/lokey_notename87.sfz",
                "<region> sample=test.raw lokey=E10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename87.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename88) {
    create_file("testdata/SFZSink/lokey_notename88.sfz",
                "<region> sample=test.raw lokey=Eb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename88.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename89) {
    create_file("testdata/SFZSink/lokey_notename89.sfz",
                "<region> sample=test.raw lokey=F##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename89.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename90) {
    create_file("testdata/SFZSink/lokey_notename90.sfz",
                "<region> sample=test.raw lokey=F#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename90.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename91) {
    create_file("testdata/SFZSink/lokey_notename91.sfz",
                "<region> sample=test.raw lokey=F#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename91.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename92) {
    create_file("testdata/SFZSink/lokey_notename92.sfz",
                "<region> sample=test.raw lokey=FZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename92.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename93) {
    create_file("testdata/SFZSink/lokey_notename93.sfz",
                "<region> sample=test.raw lokey=Fb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename93.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename94) {
    create_file("testdata/SFZSink/lokey_notename94.sfz",
                "<region> sample=test.raw lokey=Fbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename94.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename95) {
    create_file("testdata/SFZSink/lokey_notename95.sfz",
                "<region> sample=test.raw lokey=G##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename95.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename96) {
    create_file("testdata/SFZSink/lokey_notename96.sfz",
                "<region> sample=test.raw lokey=G#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename96.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename97) {
    create_file("testdata/SFZSink/lokey_notename97.sfz",
                "<region> sample=test.raw lokey=G#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename97.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename98) {
    create_file("testdata/SFZSink/lokey_notename98.sfz",
                "<region> sample=test.raw lokey=G10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename98.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename99) {
    create_file("testdata/SFZSink/lokey_notename99.sfz",
                "<region> sample=test.raw lokey=Gb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename99.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename100) {
    create_file("testdata/SFZSink/lokey_notename100.sfz",
                "<region> sample=test.raw lokey=Gb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename100.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename101) {
    create_file("testdata/SFZSink/lokey_notename101.sfz",
                "<region> sample=test.raw lokey=A#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename101.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename102) {
    create_file("testdata/SFZSink/lokey_notename102.sfz",
                "<region> sample=test.raw lokey=A#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename102.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename103) {
    create_file("testdata/SFZSink/lokey_notename103.sfz",
                "<region> sample=test.raw lokey=AZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename103.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename104) {
    create_file("testdata/SFZSink/lokey_notename104.sfz",
                "<region> sample=test.raw lokey=Ab\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename104.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename105) {
    create_file("testdata/SFZSink/lokey_notename105.sfz",
                "<region> sample=test.raw lokey=Ab-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename105.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename106) {
    create_file("testdata/SFZSink/lokey_notename106.sfz",
                "<region> sample=test.raw lokey=Ab-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename106.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename107) {
    create_file("testdata/SFZSink/lokey_notename107.sfz",
                "<region> sample=test.raw lokey=B#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename107.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename108) {
    create_file("testdata/SFZSink/lokey_notename108.sfz",
                "<region> sample=test.raw lokey=B#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename108.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 0);
    }
}

TEST_F(SfzTest, lokey_notename109) {
    create_file("testdata/SFZSink/lokey_notename109.sfz",
                "<region> sample=test.raw lokey=B-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename109.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename110) {
    create_file("testdata/SFZSink/lokey_notename110.sfz",
                "<region> sample=test.raw lokey=Bb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename110.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename111) {
    create_file("testdata/SFZSink/lokey_notename111.sfz",
                "<region> sample=test.raw lokey=Bb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename111.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename112) {
    create_file("testdata/SFZSink/lokey_notename112.sfz",
                "<region> sample=test.raw lokey=BbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename112.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename113) {
    create_file("testdata/SFZSink/lokey_notename113.sfz",
                "<region> sample=test.raw lokey=c#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename113.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename114) {
    create_file("testdata/SFZSink/lokey_notename114.sfz",
                "<region> sample=test.raw lokey=c#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename114.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename115) {
    create_file("testdata/SFZSink/lokey_notename115.sfz",
                "<region> sample=test.raw lokey=c#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename115.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename116) {
    create_file("testdata/SFZSink/lokey_notename116.sfz",
                "<region> sample=test.raw lokey=c-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename116.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename117) {
    create_file("testdata/SFZSink/lokey_notename117.sfz",
                "<region> sample=test.raw lokey=cb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename117.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename118) {
    create_file("testdata/SFZSink/lokey_notename118.sfz",
                "<region> sample=test.raw lokey=cbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename118.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename119) {
    create_file("testdata/SFZSink/lokey_notename119.sfz",
                "<region> sample=test.raw lokey=d##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename119.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename120) {
    create_file("testdata/SFZSink/lokey_notename120.sfz",
                "<region> sample=test.raw lokey=d-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename120.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename121) {
    create_file("testdata/SFZSink/lokey_notename121.sfz",
                "<region> sample=test.raw lokey=d-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename121.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename122) {
    create_file("testdata/SFZSink/lokey_notename122.sfz",
                "<region> sample=test.raw lokey=d10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename122.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename123) {
    create_file("testdata/SFZSink/lokey_notename123.sfz",
                "<region> sample=test.raw lokey=dbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename123.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename124) {
    create_file("testdata/SFZSink/lokey_notename124.sfz",
                "<region> sample=test.raw lokey=dbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename124.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename125) {
    create_file("testdata/SFZSink/lokey_notename125.sfz",
                "<region> sample=test.raw lokey=e#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename125.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename126) {
    create_file("testdata/SFZSink/lokey_notename126.sfz",
                "<region> sample=test.raw lokey=e#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename126.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename127) {
    create_file("testdata/SFZSink/lokey_notename127.sfz",
                "<region> sample=test.raw lokey=e#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename127.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename128) {
    create_file("testdata/SFZSink/lokey_notename128.sfz",
                "<region> sample=test.raw lokey=eZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename128.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename129) {
    create_file("testdata/SFZSink/lokey_notename129.sfz",
                "<region> sample=test.raw lokey=eb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename129.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename130) {
    create_file("testdata/SFZSink/lokey_notename130.sfz",
                "<region> sample=test.raw lokey=eb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename130.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename131) {
    create_file("testdata/SFZSink/lokey_notename131.sfz",
                "<region> sample=test.raw lokey=f#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename131.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename132) {
    create_file("testdata/SFZSink/lokey_notename132.sfz",
                "<region> sample=test.raw lokey=f#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename132.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename133) {
    create_file("testdata/SFZSink/lokey_notename133.sfz",
                "<region> sample=test.raw lokey=f-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename133.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename134) {
    create_file("testdata/SFZSink/lokey_notename134.sfz",
                "<region> sample=test.raw lokey=fb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename134.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename135) {
    create_file("testdata/SFZSink/lokey_notename135.sfz",
                "<region> sample=test.raw lokey=fb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename135.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename136) {
    create_file("testdata/SFZSink/lokey_notename136.sfz",
                "<region> sample=test.raw lokey=fbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename136.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename137) {
    create_file("testdata/SFZSink/lokey_notename137.sfz",
                "<region> sample=test.raw lokey=g#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename137.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename138) {
    create_file("testdata/SFZSink/lokey_notename138.sfz",
                "<region> sample=test.raw lokey=g10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename138.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename139) {
    create_file("testdata/SFZSink/lokey_notename139.sfz",
                "<region> sample=test.raw lokey=gb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename139.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename140) {
    create_file("testdata/SFZSink/lokey_notename140.sfz",
                "<region> sample=test.raw lokey=gb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename140.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename141) {
    create_file("testdata/SFZSink/lokey_notename141.sfz",
                "<region> sample=test.raw lokey=gb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename141.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename142) {
    create_file("testdata/SFZSink/lokey_notename142.sfz",
                "<region> sample=test.raw lokey=gb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename142.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename143) {
    create_file("testdata/SFZSink/lokey_notename143.sfz",
                "<region> sample=test.raw lokey=a#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename143.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename144) {
    create_file("testdata/SFZSink/lokey_notename144.sfz",
                "<region> sample=test.raw lokey=a-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename144.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename145) {
    create_file("testdata/SFZSink/lokey_notename145.sfz",
                "<region> sample=test.raw lokey=a10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename145.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename146) {
    create_file("testdata/SFZSink/lokey_notename146.sfz",
                "<region> sample=test.raw lokey=ab#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename146.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename147) {
    create_file("testdata/SFZSink/lokey_notename147.sfz",
                "<region> sample=test.raw lokey=ab-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename147.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename148) {
    create_file("testdata/SFZSink/lokey_notename148.sfz",
                "<region> sample=test.raw lokey=abb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename148.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename149) {
    create_file("testdata/SFZSink/lokey_notename149.sfz",
                "<region> sample=test.raw lokey=b#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename149.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 0);
    }
}

TEST_F(SfzTest, lokey_notename150) {
    create_file("testdata/SFZSink/lokey_notename150.sfz",
                "<region> sample=test.raw lokey=b#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename150.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename151) {
    create_file("testdata/SFZSink/lokey_notename151.sfz",
                "<region> sample=test.raw lokey=b10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename151.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename152) {
    create_file("testdata/SFZSink/lokey_notename152.sfz",
                "<region> sample=test.raw lokey=bb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename152.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename153) {
    create_file("testdata/SFZSink/lokey_notename153.sfz",
                "<region> sample=test.raw lokey=bb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename153.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename154) {
    create_file("testdata/SFZSink/lokey_notename154.sfz",
                "<region> sample=test.raw lokey=bbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename154.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename155) {
    create_file("testdata/SFZSink/lokey_notename155.sfz",
                "<region> sample=test.raw lokey=Z3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename155.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename156) {
    create_file("testdata/SFZSink/lokey_notename156.sfz",
                "<region> sample=test.raw lokey=Zb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename156.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename157) {
    create_file("testdata/SFZSink/lokey_notename157.sfz",
                "<region> sample=test.raw lokey=Z#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename157.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename158) {
    create_file("testdata/SFZSink/lokey_notename158.sfz",
                "<region> sample=test.raw lokey=10#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename158.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename159) {
    create_file("testdata/SFZSink/lokey_notename159.sfz",
                "<region> sample=test.raw lokey=10b3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename159.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename160) {
    create_file("testdata/SFZSink/lokey_notename160.sfz",
                "<region> sample=test.raw lokey==C3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename160.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename161) {
    create_file("testdata/SFZSink/lokey_notename161.sfz",
                "<region> sample=test.raw lokey=Cz3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename161.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename162) {
    create_file("testdata/SFZSink/lokey_notename162.sfz",
                "<region> sample=test.raw lokey=C♭3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename162.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, lokey_notename163) {
    create_file("testdata/SFZSink/lokey_notename163.sfz",
                "<region> sample=test.raw lokey=C=3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/lokey_notename163.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename1) {
    create_file("testdata/SFZSink/hikey_notename1.sfz",
                "<region> sample=test.raw hikey=Cb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 107);
    }
}

TEST_F(SfzTest, hikey_notename2) {
    create_file("testdata/SFZSink/hikey_notename2.sfz",
                "<region> sample=test.raw hikey=C-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 0);
    }
}

TEST_F(SfzTest, hikey_notename3) {
    create_file("testdata/SFZSink/hikey_notename3.sfz",
                "<region> sample=test.raw hikey=C8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 108);
    }
}

TEST_F(SfzTest, hikey_notename4) {
    create_file("testdata/SFZSink/hikey_notename4.sfz",
                "<region> sample=test.raw hikey=C#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 13);
    }
}

TEST_F(SfzTest, hikey_notename5) {
    create_file("testdata/SFZSink/hikey_notename5.sfz",
                "<region> sample=test.raw hikey=C#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 49);
    }
}

TEST_F(SfzTest, hikey_notename6) {
    create_file("testdata/SFZSink/hikey_notename6.sfz",
                "<region> sample=test.raw hikey=Db3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 49);
    }
}

TEST_F(SfzTest, hikey_notename7) {
    create_file("testdata/SFZSink/hikey_notename7.sfz",
                "<region> sample=test.raw hikey=Db9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 121);
    }
}

TEST_F(SfzTest, hikey_notename8) {
    create_file("testdata/SFZSink/hikey_notename8.sfz",
                "<region> sample=test.raw hikey=D-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 2);
    }
}

TEST_F(SfzTest, hikey_notename9) {
    create_file("testdata/SFZSink/hikey_notename9.sfz",
                "<region> sample=test.raw hikey=D8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 110);
    }
}

TEST_F(SfzTest, hikey_notename10) {
    create_file("testdata/SFZSink/hikey_notename10.sfz",
                "<region> sample=test.raw hikey=D#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 15);
    }
}

TEST_F(SfzTest, hikey_notename11) {
    create_file("testdata/SFZSink/hikey_notename11.sfz",
                "<region> sample=test.raw hikey=Eb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 51);
    }
}

TEST_F(SfzTest, hikey_notename12) {
    create_file("testdata/SFZSink/hikey_notename12.sfz",
                "<region> sample=test.raw hikey=Eb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename12.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 111);
    }
}

TEST_F(SfzTest, hikey_notename13) {
    create_file("testdata/SFZSink/hikey_notename13.sfz",
                "<region> sample=test.raw hikey=E-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename13.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 4);
    }
}

TEST_F(SfzTest, hikey_notename14) {
    create_file("testdata/SFZSink/hikey_notename14.sfz",
                "<region> sample=test.raw hikey=E#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename14.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 17);
    }
}

TEST_F(SfzTest, hikey_notename15) {
    create_file("testdata/SFZSink/hikey_notename15.sfz",
                "<region> sample=test.raw hikey=E#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename15.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 125);
    }
}

TEST_F(SfzTest, hikey_notename16) {
    create_file("testdata/SFZSink/hikey_notename16.sfz",
                "<region> sample=test.raw hikey=Fb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename16.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 16);
    }
}

TEST_F(SfzTest, hikey_notename17) {
    create_file("testdata/SFZSink/hikey_notename17.sfz",
                "<region> sample=test.raw hikey=Fb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename17.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 52);
    }
}

TEST_F(SfzTest, hikey_notename18) {
    create_file("testdata/SFZSink/hikey_notename18.sfz",
                "<region> sample=test.raw hikey=F8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename18.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 113);
    }
}

TEST_F(SfzTest, hikey_notename19) {
    create_file("testdata/SFZSink/hikey_notename19.sfz",
                "<region> sample=test.raw hikey=F#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename19.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 6);
    }
}

TEST_F(SfzTest, hikey_notename20) {
    create_file("testdata/SFZSink/hikey_notename20.sfz",
                "<region> sample=test.raw hikey=F#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename20.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 126);
    }
}

TEST_F(SfzTest, hikey_notename21) {
    create_file("testdata/SFZSink/hikey_notename21.sfz",
                "<region> sample=test.raw hikey=Gb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename21.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 114);
    }
}

TEST_F(SfzTest, hikey_notename22) {
    create_file("testdata/SFZSink/hikey_notename22.sfz",
                "<region> sample=test.raw hikey=Gb9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename22.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 126);
    }
}

TEST_F(SfzTest, hikey_notename23) {
    create_file("testdata/SFZSink/hikey_notename23.sfz",
                "<region> sample=test.raw hikey=G-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename23.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 7);
    }
}

TEST_F(SfzTest, hikey_notename24) {
    create_file("testdata/SFZSink/hikey_notename24.sfz",
                "<region> sample=test.raw hikey=G0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename24.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 19);
    }
}

TEST_F(SfzTest, hikey_notename25) {
    create_file("testdata/SFZSink/hikey_notename25.sfz",
                "<region> sample=test.raw hikey=G#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename25.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 56);
    }
}

TEST_F(SfzTest, hikey_notename26) {
    create_file("testdata/SFZSink/hikey_notename26.sfz",
                "<region> sample=test.raw hikey=Ab0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename26.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 20);
    }
}

TEST_F(SfzTest, hikey_notename27) {
    create_file("testdata/SFZSink/hikey_notename27.sfz",
                "<region> sample=test.raw hikey=A3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename27.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 57);
    }
}

TEST_F(SfzTest, hikey_notename28) {
    create_file("testdata/SFZSink/hikey_notename28.sfz",
                "<region> sample=test.raw hikey=A#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename28.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 10);
    }
}

TEST_F(SfzTest, hikey_notename29) {
    create_file("testdata/SFZSink/hikey_notename29.sfz",
                "<region> sample=test.raw hikey=A#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename29.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 106);
    }
}

TEST_F(SfzTest, hikey_notename30) {
    create_file("testdata/SFZSink/hikey_notename30.sfz",
                "<region> sample=test.raw hikey=A#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename30.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 118);
    }
}

TEST_F(SfzTest, hikey_notename31) {
    create_file("testdata/SFZSink/hikey_notename31.sfz",
                "<region> sample=test.raw hikey=Bb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename31.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 10);
    }
}

TEST_F(SfzTest, hikey_notename32) {
    create_file("testdata/SFZSink/hikey_notename32.sfz",
                "<region> sample=test.raw hikey=Bb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename32.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 22);
    }
}

TEST_F(SfzTest, hikey_notename33) {
    create_file("testdata/SFZSink/hikey_notename33.sfz",
                "<region> sample=test.raw hikey=B3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename33.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 59);
    }
}

TEST_F(SfzTest, hikey_notename34) {
    create_file("testdata/SFZSink/hikey_notename34.sfz",
                "<region> sample=test.raw hikey=B8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename34.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 119);
    }
}

TEST_F(SfzTest, hikey_notename35) {
    create_file("testdata/SFZSink/hikey_notename35.sfz",
                "<region> sample=test.raw hikey=B#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename35.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 108);
    }
}

TEST_F(SfzTest, hikey_notename36) {
    create_file("testdata/SFZSink/hikey_notename36.sfz",
                "<region> sample=test.raw hikey=cb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename36.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename37) {
    create_file("testdata/SFZSink/hikey_notename37.sfz",
                "<region> sample=test.raw hikey=cb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename37.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 47);
    }
}

TEST_F(SfzTest, hikey_notename38) {
    create_file("testdata/SFZSink/hikey_notename38.sfz",
                "<region> sample=test.raw hikey=cb7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename38.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 95);
    }
}

TEST_F(SfzTest, hikey_notename39) {
    create_file("testdata/SFZSink/hikey_notename39.sfz",
                "<region> sample=test.raw hikey=c9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename39.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 120);
    }
}

TEST_F(SfzTest, hikey_notename40) {
    create_file("testdata/SFZSink/hikey_notename40.sfz",
                "<region> sample=test.raw hikey=c#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename40.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 13);
    }
}

TEST_F(SfzTest, hikey_notename41) {
    create_file("testdata/SFZSink/hikey_notename41.sfz",
                "<region> sample=test.raw hikey=db3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename41.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 49);
    }
}

TEST_F(SfzTest, hikey_notename42) {
    create_file("testdata/SFZSink/hikey_notename42.sfz",
                "<region> sample=test.raw hikey=d-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename42.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 2);
    }
}

TEST_F(SfzTest, hikey_notename43) {
    create_file("testdata/SFZSink/hikey_notename43.sfz",
                "<region> sample=test.raw hikey=d0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename43.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 14);
    }
}

TEST_F(SfzTest, hikey_notename44) {
    create_file("testdata/SFZSink/hikey_notename44.sfz",
                "<region> sample=test.raw hikey=d#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename44.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 111);
    }
}

TEST_F(SfzTest, hikey_notename45) {
    create_file("testdata/SFZSink/hikey_notename45.sfz",
                "<region> sample=test.raw hikey=d#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename45.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 123);
    }
}

TEST_F(SfzTest, hikey_notename46) {
    create_file("testdata/SFZSink/hikey_notename46.sfz",
                "<region> sample=test.raw hikey=eb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename46.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 111);
    }
}

TEST_F(SfzTest, hikey_notename47) {
    create_file("testdata/SFZSink/hikey_notename47.sfz",
                "<region> sample=test.raw hikey=e0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename47.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 16);
    }
}

TEST_F(SfzTest, hikey_notename48) {
    create_file("testdata/SFZSink/hikey_notename48.sfz",
                "<region> sample=test.raw hikey=e#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename48.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 5);
    }
}

TEST_F(SfzTest, hikey_notename49) {
    create_file("testdata/SFZSink/hikey_notename49.sfz",
                "<region> sample=test.raw hikey=e#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename49.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 53);
    }
}

TEST_F(SfzTest, hikey_notename50) {
    create_file("testdata/SFZSink/hikey_notename50.sfz",
                "<region> sample=test.raw hikey=e#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename50.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 125);
    }
}

TEST_F(SfzTest, hikey_notename51) {
    create_file("testdata/SFZSink/hikey_notename51.sfz",
                "<region> sample=test.raw hikey=fb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename51.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 4);
    }
}

TEST_F(SfzTest, hikey_notename52) {
    create_file("testdata/SFZSink/hikey_notename52.sfz",
                "<region> sample=test.raw hikey=f3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename52.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 53);
    }
}

TEST_F(SfzTest, hikey_notename53) {
    create_file("testdata/SFZSink/hikey_notename53.sfz",
                "<region> sample=test.raw hikey=f#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename53.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 18);
    }
}

TEST_F(SfzTest, hikey_notename54) {
    create_file("testdata/SFZSink/hikey_notename54.sfz",
                "<region> sample=test.raw hikey=f#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename54.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 114);
    }
}

TEST_F(SfzTest, hikey_notename55) {
    create_file("testdata/SFZSink/hikey_notename55.sfz",
                "<region> sample=test.raw hikey=f#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename55.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 126);
    }
}

TEST_F(SfzTest, hikey_notename56) {
    create_file("testdata/SFZSink/hikey_notename56.sfz",
                "<region> sample=test.raw hikey=g9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename56.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 127);
    }
}

TEST_F(SfzTest, hikey_notename57) {
    create_file("testdata/SFZSink/hikey_notename57.sfz",
                "<region> sample=test.raw hikey=g#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename57.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 8);
    }
}

TEST_F(SfzTest, hikey_notename58) {
    create_file("testdata/SFZSink/hikey_notename58.sfz",
                "<region> sample=test.raw hikey=g#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename58.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 56);
    }
}

TEST_F(SfzTest, hikey_notename59) {
    create_file("testdata/SFZSink/hikey_notename59.sfz",
                "<region> sample=test.raw hikey=g#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename59.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 104);
    }
}

TEST_F(SfzTest, hikey_notename60) {
    create_file("testdata/SFZSink/hikey_notename60.sfz",
                "<region> sample=test.raw hikey=gb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename60.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 18);
    }
}

TEST_F(SfzTest, hikey_notename61) {
    create_file("testdata/SFZSink/hikey_notename61.sfz",
                "<region> sample=test.raw hikey=ab7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename61.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 104);
    }
}

TEST_F(SfzTest, hikey_notename62) {
    create_file("testdata/SFZSink/hikey_notename62.sfz",
                "<region> sample=test.raw hikey=ab8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename62.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 116);
    }
}

TEST_F(SfzTest, hikey_notename63) {
    create_file("testdata/SFZSink/hikey_notename63.sfz",
                "<region> sample=test.raw hikey=a3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename63.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 57);
    }
}

TEST_F(SfzTest, hikey_notename64) {
    create_file("testdata/SFZSink/hikey_notename64.sfz",
                "<region> sample=test.raw hikey=a#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename64.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 10);
    }
}

TEST_F(SfzTest, hikey_notename65) {
    create_file("testdata/SFZSink/hikey_notename65.sfz",
                "<region> sample=test.raw hikey=a#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename65.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 22);
    }
}

TEST_F(SfzTest, hikey_notename66) {
    create_file("testdata/SFZSink/hikey_notename66.sfz",
                "<region> sample=test.raw hikey=bb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename66.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 22);
    }
}

TEST_F(SfzTest, hikey_notename67) {
    create_file("testdata/SFZSink/hikey_notename67.sfz",
                "<region> sample=test.raw hikey=bb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename67.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 118);
    }
}

TEST_F(SfzTest, hikey_notename68) {
    create_file("testdata/SFZSink/hikey_notename68.sfz",
                "<region> sample=test.raw hikey=b-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename68.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 11);
    }
}

TEST_F(SfzTest, hikey_notename69) {
    create_file("testdata/SFZSink/hikey_notename69.sfz",
                "<region> sample=test.raw hikey=b7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename69.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 107);
    }
}

TEST_F(SfzTest, hikey_notename70) {
    create_file("testdata/SFZSink/hikey_notename70.sfz",
                "<region> sample=test.raw hikey=b#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename70.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 60);
    }
}

TEST_F(SfzTest, hikey_notename71) {
    create_file("testdata/SFZSink/hikey_notename71.sfz",
                "<region> sample=test.raw hikey=C##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename71.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename72) {
    create_file("testdata/SFZSink/hikey_notename72.sfz",
                "<region> sample=test.raw hikey=C#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename72.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename73) {
    create_file("testdata/SFZSink/hikey_notename73.sfz",
                "<region> sample=test.raw hikey=C#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename73.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename74) {
    create_file("testdata/SFZSink/hikey_notename74.sfz",
                "<region> sample=test.raw hikey=C-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename74.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename75) {
    create_file("testdata/SFZSink/hikey_notename75.sfz",
                "<region> sample=test.raw hikey=C10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename75.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename76) {
    create_file("testdata/SFZSink/hikey_notename76.sfz",
                "<region> sample=test.raw hikey=CbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename76.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename77) {
    create_file("testdata/SFZSink/hikey_notename77.sfz",
                "<region> sample=test.raw hikey=D#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename77.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename78) {
    create_file("testdata/SFZSink/hikey_notename78.sfz",
                "<region> sample=test.raw hikey=D#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename78.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename79) {
    create_file("testdata/SFZSink/hikey_notename79.sfz",
                "<region> sample=test.raw hikey=D#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename79.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename80) {
    create_file("testdata/SFZSink/hikey_notename80.sfz",
                "<region> sample=test.raw hikey=D#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename80.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename81) {
    create_file("testdata/SFZSink/hikey_notename81.sfz",
                "<region> sample=test.raw hikey=Db\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename81.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename82) {
    create_file("testdata/SFZSink/hikey_notename82.sfz",
                "<region> sample=test.raw hikey=Db-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename82.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename83) {
    create_file("testdata/SFZSink/hikey_notename83.sfz",
                "<region> sample=test.raw hikey=E#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename83.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename84) {
    create_file("testdata/SFZSink/hikey_notename84.sfz",
                "<region> sample=test.raw hikey=E#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename84.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename85) {
    create_file("testdata/SFZSink/hikey_notename85.sfz",
                "<region> sample=test.raw hikey=E#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename85.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename86) {
    create_file("testdata/SFZSink/hikey_notename86.sfz",
                "<region> sample=test.raw hikey=E-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename86.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename87) {
    create_file("testdata/SFZSink/hikey_notename87.sfz",
                "<region> sample=test.raw hikey=E10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename87.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename88) {
    create_file("testdata/SFZSink/hikey_notename88.sfz",
                "<region> sample=test.raw hikey=Eb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename88.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename89) {
    create_file("testdata/SFZSink/hikey_notename89.sfz",
                "<region> sample=test.raw hikey=F##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename89.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename90) {
    create_file("testdata/SFZSink/hikey_notename90.sfz",
                "<region> sample=test.raw hikey=F#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename90.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename91) {
    create_file("testdata/SFZSink/hikey_notename91.sfz",
                "<region> sample=test.raw hikey=F#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename91.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename92) {
    create_file("testdata/SFZSink/hikey_notename92.sfz",
                "<region> sample=test.raw hikey=FZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename92.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename93) {
    create_file("testdata/SFZSink/hikey_notename93.sfz",
                "<region> sample=test.raw hikey=Fb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename93.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename94) {
    create_file("testdata/SFZSink/hikey_notename94.sfz",
                "<region> sample=test.raw hikey=Fbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename94.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename95) {
    create_file("testdata/SFZSink/hikey_notename95.sfz",
                "<region> sample=test.raw hikey=G##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename95.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename96) {
    create_file("testdata/SFZSink/hikey_notename96.sfz",
                "<region> sample=test.raw hikey=G#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename96.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename97) {
    create_file("testdata/SFZSink/hikey_notename97.sfz",
                "<region> sample=test.raw hikey=G#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename97.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename98) {
    create_file("testdata/SFZSink/hikey_notename98.sfz",
                "<region> sample=test.raw hikey=G10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename98.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename99) {
    create_file("testdata/SFZSink/hikey_notename99.sfz",
                "<region> sample=test.raw hikey=Gb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename99.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename100) {
    create_file("testdata/SFZSink/hikey_notename100.sfz",
                "<region> sample=test.raw hikey=Gb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename100.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename101) {
    create_file("testdata/SFZSink/hikey_notename101.sfz",
                "<region> sample=test.raw hikey=A#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename101.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename102) {
    create_file("testdata/SFZSink/hikey_notename102.sfz",
                "<region> sample=test.raw hikey=A#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename102.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename103) {
    create_file("testdata/SFZSink/hikey_notename103.sfz",
                "<region> sample=test.raw hikey=AZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename103.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename104) {
    create_file("testdata/SFZSink/hikey_notename104.sfz",
                "<region> sample=test.raw hikey=Ab\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename104.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename105) {
    create_file("testdata/SFZSink/hikey_notename105.sfz",
                "<region> sample=test.raw hikey=Ab-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename105.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename106) {
    create_file("testdata/SFZSink/hikey_notename106.sfz",
                "<region> sample=test.raw hikey=Ab-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename106.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename107) {
    create_file("testdata/SFZSink/hikey_notename107.sfz",
                "<region> sample=test.raw hikey=B#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename107.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename108) {
    create_file("testdata/SFZSink/hikey_notename108.sfz",
                "<region> sample=test.raw hikey=B#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename108.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 0);
    }
}

TEST_F(SfzTest, hikey_notename109) {
    create_file("testdata/SFZSink/hikey_notename109.sfz",
                "<region> sample=test.raw hikey=B-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename109.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename110) {
    create_file("testdata/SFZSink/hikey_notename110.sfz",
                "<region> sample=test.raw hikey=Bb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename110.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename111) {
    create_file("testdata/SFZSink/hikey_notename111.sfz",
                "<region> sample=test.raw hikey=Bb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename111.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename112) {
    create_file("testdata/SFZSink/hikey_notename112.sfz",
                "<region> sample=test.raw hikey=BbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename112.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename113) {
    create_file("testdata/SFZSink/hikey_notename113.sfz",
                "<region> sample=test.raw hikey=c#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename113.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename114) {
    create_file("testdata/SFZSink/hikey_notename114.sfz",
                "<region> sample=test.raw hikey=c#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename114.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename115) {
    create_file("testdata/SFZSink/hikey_notename115.sfz",
                "<region> sample=test.raw hikey=c#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename115.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename116) {
    create_file("testdata/SFZSink/hikey_notename116.sfz",
                "<region> sample=test.raw hikey=c-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename116.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename117) {
    create_file("testdata/SFZSink/hikey_notename117.sfz",
                "<region> sample=test.raw hikey=cb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename117.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename118) {
    create_file("testdata/SFZSink/hikey_notename118.sfz",
                "<region> sample=test.raw hikey=cbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename118.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename119) {
    create_file("testdata/SFZSink/hikey_notename119.sfz",
                "<region> sample=test.raw hikey=d##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename119.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename120) {
    create_file("testdata/SFZSink/hikey_notename120.sfz",
                "<region> sample=test.raw hikey=d-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename120.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename121) {
    create_file("testdata/SFZSink/hikey_notename121.sfz",
                "<region> sample=test.raw hikey=d-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename121.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename122) {
    create_file("testdata/SFZSink/hikey_notename122.sfz",
                "<region> sample=test.raw hikey=d10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename122.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename123) {
    create_file("testdata/SFZSink/hikey_notename123.sfz",
                "<region> sample=test.raw hikey=dbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename123.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename124) {
    create_file("testdata/SFZSink/hikey_notename124.sfz",
                "<region> sample=test.raw hikey=dbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename124.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename125) {
    create_file("testdata/SFZSink/hikey_notename125.sfz",
                "<region> sample=test.raw hikey=e#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename125.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename126) {
    create_file("testdata/SFZSink/hikey_notename126.sfz",
                "<region> sample=test.raw hikey=e#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename126.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename127) {
    create_file("testdata/SFZSink/hikey_notename127.sfz",
                "<region> sample=test.raw hikey=e#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename127.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename128) {
    create_file("testdata/SFZSink/hikey_notename128.sfz",
                "<region> sample=test.raw hikey=eZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename128.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename129) {
    create_file("testdata/SFZSink/hikey_notename129.sfz",
                "<region> sample=test.raw hikey=eb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename129.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename130) {
    create_file("testdata/SFZSink/hikey_notename130.sfz",
                "<region> sample=test.raw hikey=eb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename130.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename131) {
    create_file("testdata/SFZSink/hikey_notename131.sfz",
                "<region> sample=test.raw hikey=f#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename131.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename132) {
    create_file("testdata/SFZSink/hikey_notename132.sfz",
                "<region> sample=test.raw hikey=f#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename132.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename133) {
    create_file("testdata/SFZSink/hikey_notename133.sfz",
                "<region> sample=test.raw hikey=f-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename133.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename134) {
    create_file("testdata/SFZSink/hikey_notename134.sfz",
                "<region> sample=test.raw hikey=fb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename134.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename135) {
    create_file("testdata/SFZSink/hikey_notename135.sfz",
                "<region> sample=test.raw hikey=fb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename135.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename136) {
    create_file("testdata/SFZSink/hikey_notename136.sfz",
                "<region> sample=test.raw hikey=fbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename136.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename137) {
    create_file("testdata/SFZSink/hikey_notename137.sfz",
                "<region> sample=test.raw hikey=g#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename137.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename138) {
    create_file("testdata/SFZSink/hikey_notename138.sfz",
                "<region> sample=test.raw hikey=g10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename138.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename139) {
    create_file("testdata/SFZSink/hikey_notename139.sfz",
                "<region> sample=test.raw hikey=gb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename139.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename140) {
    create_file("testdata/SFZSink/hikey_notename140.sfz",
                "<region> sample=test.raw hikey=gb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename140.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename141) {
    create_file("testdata/SFZSink/hikey_notename141.sfz",
                "<region> sample=test.raw hikey=gb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename141.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename142) {
    create_file("testdata/SFZSink/hikey_notename142.sfz",
                "<region> sample=test.raw hikey=gb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename142.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename143) {
    create_file("testdata/SFZSink/hikey_notename143.sfz",
                "<region> sample=test.raw hikey=a#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename143.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename144) {
    create_file("testdata/SFZSink/hikey_notename144.sfz",
                "<region> sample=test.raw hikey=a-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename144.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename145) {
    create_file("testdata/SFZSink/hikey_notename145.sfz",
                "<region> sample=test.raw hikey=a10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename145.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename146) {
    create_file("testdata/SFZSink/hikey_notename146.sfz",
                "<region> sample=test.raw hikey=ab#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename146.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename147) {
    create_file("testdata/SFZSink/hikey_notename147.sfz",
                "<region> sample=test.raw hikey=ab-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename147.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename148) {
    create_file("testdata/SFZSink/hikey_notename148.sfz",
                "<region> sample=test.raw hikey=abb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename148.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename149) {
    create_file("testdata/SFZSink/hikey_notename149.sfz",
                "<region> sample=test.raw hikey=b#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename149.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].hikey, 0);
    }
}

TEST_F(SfzTest, hikey_notename150) {
    create_file("testdata/SFZSink/hikey_notename150.sfz",
                "<region> sample=test.raw hikey=b#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename150.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename151) {
    create_file("testdata/SFZSink/hikey_notename151.sfz",
                "<region> sample=test.raw hikey=b10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename151.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename152) {
    create_file("testdata/SFZSink/hikey_notename152.sfz",
                "<region> sample=test.raw hikey=bb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename152.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename153) {
    create_file("testdata/SFZSink/hikey_notename153.sfz",
                "<region> sample=test.raw hikey=bb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename153.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename154) {
    create_file("testdata/SFZSink/hikey_notename154.sfz",
                "<region> sample=test.raw hikey=bbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename154.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename155) {
    create_file("testdata/SFZSink/hikey_notename155.sfz",
                "<region> sample=test.raw hikey=Z3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename155.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename156) {
    create_file("testdata/SFZSink/hikey_notename156.sfz",
                "<region> sample=test.raw hikey=Zb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename156.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename157) {
    create_file("testdata/SFZSink/hikey_notename157.sfz",
                "<region> sample=test.raw hikey=Z#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename157.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename158) {
    create_file("testdata/SFZSink/hikey_notename158.sfz",
                "<region> sample=test.raw hikey=10#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename158.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename159) {
    create_file("testdata/SFZSink/hikey_notename159.sfz",
                "<region> sample=test.raw hikey=10b3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename159.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename160) {
    create_file("testdata/SFZSink/hikey_notename160.sfz",
                "<region> sample=test.raw hikey==C3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename160.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename161) {
    create_file("testdata/SFZSink/hikey_notename161.sfz",
                "<region> sample=test.raw hikey=Cz3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename161.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename162) {
    create_file("testdata/SFZSink/hikey_notename162.sfz",
                "<region> sample=test.raw hikey=C♭3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename162.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, hikey_notename163) {
    create_file("testdata/SFZSink/hikey_notename163.sfz",
                "<region> sample=test.raw hikey=C=3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/hikey_notename163.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename1) {
    create_file("testdata/SFZSink/key_notename1.sfz",
                "<region> sample=test.raw key=Cb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename1.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 107);
        EXPECT_EQ(regions[0].hikey, 107);
    }
}

TEST_F(SfzTest, key_notename2) {
    create_file("testdata/SFZSink/key_notename2.sfz",
                "<region> sample=test.raw key=C-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename2.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 0);
        EXPECT_EQ(regions[0].hikey, 0);
    }
}

TEST_F(SfzTest, key_notename3) {
    create_file("testdata/SFZSink/key_notename3.sfz",
                "<region> sample=test.raw key=C8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename3.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 108);
        EXPECT_EQ(regions[0].hikey, 108);
    }
}

TEST_F(SfzTest, key_notename4) {
    create_file("testdata/SFZSink/key_notename4.sfz",
                "<region> sample=test.raw key=C#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename4.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 13);
        EXPECT_EQ(regions[0].hikey, 13);
    }
}

TEST_F(SfzTest, key_notename5) {
    create_file("testdata/SFZSink/key_notename5.sfz",
                "<region> sample=test.raw key=C#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename5.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 49);
        EXPECT_EQ(regions[0].hikey, 49);
    }
}

TEST_F(SfzTest, key_notename6) {
    create_file("testdata/SFZSink/key_notename6.sfz",
                "<region> sample=test.raw key=Db3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename6.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 49);
        EXPECT_EQ(regions[0].hikey, 49);
    }
}

TEST_F(SfzTest, key_notename7) {
    create_file("testdata/SFZSink/key_notename7.sfz",
                "<region> sample=test.raw key=Db9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename7.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 121);
        EXPECT_EQ(regions[0].hikey, 121);
    }
}

TEST_F(SfzTest, key_notename8) {
    create_file("testdata/SFZSink/key_notename8.sfz",
                "<region> sample=test.raw key=D-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename8.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 2);
        EXPECT_EQ(regions[0].hikey, 2);
    }
}

TEST_F(SfzTest, key_notename9) {
    create_file("testdata/SFZSink/key_notename9.sfz",
                "<region> sample=test.raw key=D8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename9.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 110);
        EXPECT_EQ(regions[0].hikey, 110);
    }
}

TEST_F(SfzTest, key_notename10) {
    create_file("testdata/SFZSink/key_notename10.sfz",
                "<region> sample=test.raw key=D#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename10.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 15);
        EXPECT_EQ(regions[0].hikey, 15);
    }
}

TEST_F(SfzTest, key_notename11) {
    create_file("testdata/SFZSink/key_notename11.sfz",
                "<region> sample=test.raw key=Eb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename11.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 51);
        EXPECT_EQ(regions[0].hikey, 51);
    }
}

TEST_F(SfzTest, key_notename12) {
    create_file("testdata/SFZSink/key_notename12.sfz",
                "<region> sample=test.raw key=Eb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename12.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 111);
        EXPECT_EQ(regions[0].hikey, 111);
    }
}

TEST_F(SfzTest, key_notename13) {
    create_file("testdata/SFZSink/key_notename13.sfz",
                "<region> sample=test.raw key=E-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename13.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 4);
        EXPECT_EQ(regions[0].hikey, 4);
    }
}

TEST_F(SfzTest, key_notename14) {
    create_file("testdata/SFZSink/key_notename14.sfz",
                "<region> sample=test.raw key=E#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename14.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 17);
        EXPECT_EQ(regions[0].hikey, 17);
    }
}

TEST_F(SfzTest, key_notename15) {
    create_file("testdata/SFZSink/key_notename15.sfz",
                "<region> sample=test.raw key=E#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename15.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 125);
        EXPECT_EQ(regions[0].hikey, 125);
    }
}

TEST_F(SfzTest, key_notename16) {
    create_file("testdata/SFZSink/key_notename16.sfz",
                "<region> sample=test.raw key=Fb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename16.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 16);
        EXPECT_EQ(regions[0].hikey, 16);
    }
}

TEST_F(SfzTest, key_notename17) {
    create_file("testdata/SFZSink/key_notename17.sfz",
                "<region> sample=test.raw key=Fb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename17.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 52);
        EXPECT_EQ(regions[0].hikey, 52);
    }
}

TEST_F(SfzTest, key_notename18) {
    create_file("testdata/SFZSink/key_notename18.sfz",
                "<region> sample=test.raw key=F8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename18.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 113);
        EXPECT_EQ(regions[0].hikey, 113);
    }
}

TEST_F(SfzTest, key_notename19) {
    create_file("testdata/SFZSink/key_notename19.sfz",
                "<region> sample=test.raw key=F#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename19.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 6);
        EXPECT_EQ(regions[0].hikey, 6);
    }
}

TEST_F(SfzTest, key_notename20) {
    create_file("testdata/SFZSink/key_notename20.sfz",
                "<region> sample=test.raw key=F#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename20.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 126);
        EXPECT_EQ(regions[0].hikey, 126);
    }
}

TEST_F(SfzTest, key_notename21) {
    create_file("testdata/SFZSink/key_notename21.sfz",
                "<region> sample=test.raw key=Gb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename21.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 114);
        EXPECT_EQ(regions[0].hikey, 114);
    }
}

TEST_F(SfzTest, key_notename22) {
    create_file("testdata/SFZSink/key_notename22.sfz",
                "<region> sample=test.raw key=Gb9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename22.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 126);
        EXPECT_EQ(regions[0].hikey, 126);
    }
}

TEST_F(SfzTest, key_notename23) {
    create_file("testdata/SFZSink/key_notename23.sfz",
                "<region> sample=test.raw key=G-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename23.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 7);
        EXPECT_EQ(regions[0].hikey, 7);
    }
}

TEST_F(SfzTest, key_notename24) {
    create_file("testdata/SFZSink/key_notename24.sfz",
                "<region> sample=test.raw key=G0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename24.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 19);
        EXPECT_EQ(regions[0].hikey, 19);
    }
}

TEST_F(SfzTest, key_notename25) {
    create_file("testdata/SFZSink/key_notename25.sfz",
                "<region> sample=test.raw key=G#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename25.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 56);
        EXPECT_EQ(regions[0].hikey, 56);
    }
}

TEST_F(SfzTest, key_notename26) {
    create_file("testdata/SFZSink/key_notename26.sfz",
                "<region> sample=test.raw key=Ab0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename26.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 20);
        EXPECT_EQ(regions[0].hikey, 20);
    }
}

TEST_F(SfzTest, key_notename27) {
    create_file("testdata/SFZSink/key_notename27.sfz",
                "<region> sample=test.raw key=A3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename27.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 57);
        EXPECT_EQ(regions[0].hikey, 57);
    }
}

TEST_F(SfzTest, key_notename28) {
    create_file("testdata/SFZSink/key_notename28.sfz",
                "<region> sample=test.raw key=A#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename28.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 10);
        EXPECT_EQ(regions[0].hikey, 10);
    }
}

TEST_F(SfzTest, key_notename29) {
    create_file("testdata/SFZSink/key_notename29.sfz",
                "<region> sample=test.raw key=A#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename29.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 106);
        EXPECT_EQ(regions[0].hikey, 106);
    }
}

TEST_F(SfzTest, key_notename30) {
    create_file("testdata/SFZSink/key_notename30.sfz",
                "<region> sample=test.raw key=A#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename30.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 118);
        EXPECT_EQ(regions[0].hikey, 118);
    }
}

TEST_F(SfzTest, key_notename31) {
    create_file("testdata/SFZSink/key_notename31.sfz",
                "<region> sample=test.raw key=Bb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename31.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 10);
        EXPECT_EQ(regions[0].hikey, 10);
    }
}

TEST_F(SfzTest, key_notename32) {
    create_file("testdata/SFZSink/key_notename32.sfz",
                "<region> sample=test.raw key=Bb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename32.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 22);
        EXPECT_EQ(regions[0].hikey, 22);
    }
}

TEST_F(SfzTest, key_notename33) {
    create_file("testdata/SFZSink/key_notename33.sfz",
                "<region> sample=test.raw key=B3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename33.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 59);
        EXPECT_EQ(regions[0].hikey, 59);
    }
}

TEST_F(SfzTest, key_notename34) {
    create_file("testdata/SFZSink/key_notename34.sfz",
                "<region> sample=test.raw key=B8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename34.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 119);
        EXPECT_EQ(regions[0].hikey, 119);
    }
}

TEST_F(SfzTest, key_notename35) {
    create_file("testdata/SFZSink/key_notename35.sfz",
                "<region> sample=test.raw key=B#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename35.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 108);
        EXPECT_EQ(regions[0].hikey, 108);
    }
}

TEST_F(SfzTest, key_notename36) {
    create_file("testdata/SFZSink/key_notename36.sfz",
                "<region> sample=test.raw key=cb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename36.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename37) {
    create_file("testdata/SFZSink/key_notename37.sfz",
                "<region> sample=test.raw key=cb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename37.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 47);
        EXPECT_EQ(regions[0].hikey, 47);
    }
}

TEST_F(SfzTest, key_notename38) {
    create_file("testdata/SFZSink/key_notename38.sfz",
                "<region> sample=test.raw key=cb7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename38.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 95);
        EXPECT_EQ(regions[0].hikey, 95);
    }
}

TEST_F(SfzTest, key_notename39) {
    create_file("testdata/SFZSink/key_notename39.sfz",
                "<region> sample=test.raw key=c9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename39.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 120);
        EXPECT_EQ(regions[0].hikey, 120);
    }
}

TEST_F(SfzTest, key_notename40) {
    create_file("testdata/SFZSink/key_notename40.sfz",
                "<region> sample=test.raw key=c#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename40.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 13);
        EXPECT_EQ(regions[0].hikey, 13);
    }
}

TEST_F(SfzTest, key_notename41) {
    create_file("testdata/SFZSink/key_notename41.sfz",
                "<region> sample=test.raw key=db3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename41.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 49);
        EXPECT_EQ(regions[0].hikey, 49);
    }
}

TEST_F(SfzTest, key_notename42) {
    create_file("testdata/SFZSink/key_notename42.sfz",
                "<region> sample=test.raw key=d-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename42.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 2);
        EXPECT_EQ(regions[0].hikey, 2);
    }
}

TEST_F(SfzTest, key_notename43) {
    create_file("testdata/SFZSink/key_notename43.sfz",
                "<region> sample=test.raw key=d0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename43.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 14);
        EXPECT_EQ(regions[0].hikey, 14);
    }
}

TEST_F(SfzTest, key_notename44) {
    create_file("testdata/SFZSink/key_notename44.sfz",
                "<region> sample=test.raw key=d#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename44.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 111);
        EXPECT_EQ(regions[0].hikey, 111);
    }
}

TEST_F(SfzTest, key_notename45) {
    create_file("testdata/SFZSink/key_notename45.sfz",
                "<region> sample=test.raw key=d#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename45.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 123);
        EXPECT_EQ(regions[0].hikey, 123);
    }
}

TEST_F(SfzTest, key_notename46) {
    create_file("testdata/SFZSink/key_notename46.sfz",
                "<region> sample=test.raw key=eb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename46.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 111);
        EXPECT_EQ(regions[0].hikey, 111);
    }
}

TEST_F(SfzTest, key_notename47) {
    create_file("testdata/SFZSink/key_notename47.sfz",
                "<region> sample=test.raw key=e0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename47.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 16);
        EXPECT_EQ(regions[0].hikey, 16);
    }
}

TEST_F(SfzTest, key_notename48) {
    create_file("testdata/SFZSink/key_notename48.sfz",
                "<region> sample=test.raw key=e#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename48.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 5);
        EXPECT_EQ(regions[0].hikey, 5);
    }
}

TEST_F(SfzTest, key_notename49) {
    create_file("testdata/SFZSink/key_notename49.sfz",
                "<region> sample=test.raw key=e#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename49.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 53);
        EXPECT_EQ(regions[0].hikey, 53);
    }
}

TEST_F(SfzTest, key_notename50) {
    create_file("testdata/SFZSink/key_notename50.sfz",
                "<region> sample=test.raw key=e#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename50.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 125);
        EXPECT_EQ(regions[0].hikey, 125);
    }
}

TEST_F(SfzTest, key_notename51) {
    create_file("testdata/SFZSink/key_notename51.sfz",
                "<region> sample=test.raw key=fb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename51.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 4);
        EXPECT_EQ(regions[0].hikey, 4);
    }
}

TEST_F(SfzTest, key_notename52) {
    create_file("testdata/SFZSink/key_notename52.sfz",
                "<region> sample=test.raw key=f3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename52.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 53);
        EXPECT_EQ(regions[0].hikey, 53);
    }
}

TEST_F(SfzTest, key_notename53) {
    create_file("testdata/SFZSink/key_notename53.sfz",
                "<region> sample=test.raw key=f#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename53.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 18);
        EXPECT_EQ(regions[0].hikey, 18);
    }
}

TEST_F(SfzTest, key_notename54) {
    create_file("testdata/SFZSink/key_notename54.sfz",
                "<region> sample=test.raw key=f#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename54.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 114);
        EXPECT_EQ(regions[0].hikey, 114);
    }
}

TEST_F(SfzTest, key_notename55) {
    create_file("testdata/SFZSink/key_notename55.sfz",
                "<region> sample=test.raw key=f#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename55.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 126);
        EXPECT_EQ(regions[0].hikey, 126);
    }
}

TEST_F(SfzTest, key_notename56) {
    create_file("testdata/SFZSink/key_notename56.sfz",
                "<region> sample=test.raw key=g9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename56.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 127);
        EXPECT_EQ(regions[0].hikey, 127);
    }
}

TEST_F(SfzTest, key_notename57) {
    create_file("testdata/SFZSink/key_notename57.sfz",
                "<region> sample=test.raw key=g#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename57.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 8);
        EXPECT_EQ(regions[0].hikey, 8);
    }
}

TEST_F(SfzTest, key_notename58) {
    create_file("testdata/SFZSink/key_notename58.sfz",
                "<region> sample=test.raw key=g#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename58.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 56);
        EXPECT_EQ(regions[0].hikey, 56);
    }
}

TEST_F(SfzTest, key_notename59) {
    create_file("testdata/SFZSink/key_notename59.sfz",
                "<region> sample=test.raw key=g#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename59.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 104);
        EXPECT_EQ(regions[0].hikey, 104);
    }
}

TEST_F(SfzTest, key_notename60) {
    create_file("testdata/SFZSink/key_notename60.sfz",
                "<region> sample=test.raw key=gb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename60.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 18);
        EXPECT_EQ(regions[0].hikey, 18);
    }
}

TEST_F(SfzTest, key_notename61) {
    create_file("testdata/SFZSink/key_notename61.sfz",
                "<region> sample=test.raw key=ab7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename61.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 104);
        EXPECT_EQ(regions[0].hikey, 104);
    }
}

TEST_F(SfzTest, key_notename62) {
    create_file("testdata/SFZSink/key_notename62.sfz",
                "<region> sample=test.raw key=ab8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename62.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 116);
        EXPECT_EQ(regions[0].hikey, 116);
    }
}

TEST_F(SfzTest, key_notename63) {
    create_file("testdata/SFZSink/key_notename63.sfz",
                "<region> sample=test.raw key=a3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename63.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 57);
        EXPECT_EQ(regions[0].hikey, 57);
    }
}

TEST_F(SfzTest, key_notename64) {
    create_file("testdata/SFZSink/key_notename64.sfz",
                "<region> sample=test.raw key=a#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename64.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 10);
        EXPECT_EQ(regions[0].hikey, 10);
    }
}

TEST_F(SfzTest, key_notename65) {
    create_file("testdata/SFZSink/key_notename65.sfz",
                "<region> sample=test.raw key=a#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename65.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 22);
        EXPECT_EQ(regions[0].hikey, 22);
    }
}

TEST_F(SfzTest, key_notename66) {
    create_file("testdata/SFZSink/key_notename66.sfz",
                "<region> sample=test.raw key=bb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename66.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 22);
        EXPECT_EQ(regions[0].hikey, 22);
    }
}

TEST_F(SfzTest, key_notename67) {
    create_file("testdata/SFZSink/key_notename67.sfz",
                "<region> sample=test.raw key=bb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename67.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 118);
        EXPECT_EQ(regions[0].hikey, 118);
    }
}

TEST_F(SfzTest, key_notename68) {
    create_file("testdata/SFZSink/key_notename68.sfz",
                "<region> sample=test.raw key=b-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename68.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 11);
        EXPECT_EQ(regions[0].hikey, 11);
    }
}

TEST_F(SfzTest, key_notename69) {
    create_file("testdata/SFZSink/key_notename69.sfz",
                "<region> sample=test.raw key=b7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename69.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 107);
        EXPECT_EQ(regions[0].hikey, 107);
    }
}

TEST_F(SfzTest, key_notename70) {
    create_file("testdata/SFZSink/key_notename70.sfz",
                "<region> sample=test.raw key=b#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename70.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 60);
        EXPECT_EQ(regions[0].hikey, 60);
    }
}

TEST_F(SfzTest, key_notename71) {
    create_file("testdata/SFZSink/key_notename71.sfz",
                "<region> sample=test.raw key=C##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename71.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename72) {
    create_file("testdata/SFZSink/key_notename72.sfz",
                "<region> sample=test.raw key=C#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename72.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename73) {
    create_file("testdata/SFZSink/key_notename73.sfz",
                "<region> sample=test.raw key=C#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename73.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename74) {
    create_file("testdata/SFZSink/key_notename74.sfz",
                "<region> sample=test.raw key=C-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename74.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename75) {
    create_file("testdata/SFZSink/key_notename75.sfz",
                "<region> sample=test.raw key=C10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename75.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename76) {
    create_file("testdata/SFZSink/key_notename76.sfz",
                "<region> sample=test.raw key=CbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename76.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename77) {
    create_file("testdata/SFZSink/key_notename77.sfz",
                "<region> sample=test.raw key=D#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename77.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename78) {
    create_file("testdata/SFZSink/key_notename78.sfz",
                "<region> sample=test.raw key=D#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename78.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename79) {
    create_file("testdata/SFZSink/key_notename79.sfz",
                "<region> sample=test.raw key=D#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename79.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename80) {
    create_file("testdata/SFZSink/key_notename80.sfz",
                "<region> sample=test.raw key=D#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename80.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename81) {
    create_file("testdata/SFZSink/key_notename81.sfz",
                "<region> sample=test.raw key=Db\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename81.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename82) {
    create_file("testdata/SFZSink/key_notename82.sfz",
                "<region> sample=test.raw key=Db-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename82.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename83) {
    create_file("testdata/SFZSink/key_notename83.sfz",
                "<region> sample=test.raw key=E#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename83.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename84) {
    create_file("testdata/SFZSink/key_notename84.sfz",
                "<region> sample=test.raw key=E#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename84.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename85) {
    create_file("testdata/SFZSink/key_notename85.sfz",
                "<region> sample=test.raw key=E#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename85.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename86) {
    create_file("testdata/SFZSink/key_notename86.sfz",
                "<region> sample=test.raw key=E-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename86.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename87) {
    create_file("testdata/SFZSink/key_notename87.sfz",
                "<region> sample=test.raw key=E10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename87.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename88) {
    create_file("testdata/SFZSink/key_notename88.sfz",
                "<region> sample=test.raw key=Eb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename88.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename89) {
    create_file("testdata/SFZSink/key_notename89.sfz",
                "<region> sample=test.raw key=F##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename89.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename90) {
    create_file("testdata/SFZSink/key_notename90.sfz",
                "<region> sample=test.raw key=F#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename90.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename91) {
    create_file("testdata/SFZSink/key_notename91.sfz",
                "<region> sample=test.raw key=F#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename91.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename92) {
    create_file("testdata/SFZSink/key_notename92.sfz",
                "<region> sample=test.raw key=FZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename92.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename93) {
    create_file("testdata/SFZSink/key_notename93.sfz",
                "<region> sample=test.raw key=Fb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename93.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename94) {
    create_file("testdata/SFZSink/key_notename94.sfz",
                "<region> sample=test.raw key=Fbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename94.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename95) {
    create_file("testdata/SFZSink/key_notename95.sfz",
                "<region> sample=test.raw key=G##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename95.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename96) {
    create_file("testdata/SFZSink/key_notename96.sfz",
                "<region> sample=test.raw key=G#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename96.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename97) {
    create_file("testdata/SFZSink/key_notename97.sfz",
                "<region> sample=test.raw key=G#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename97.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename98) {
    create_file("testdata/SFZSink/key_notename98.sfz",
                "<region> sample=test.raw key=G10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename98.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename99) {
    create_file("testdata/SFZSink/key_notename99.sfz",
                "<region> sample=test.raw key=Gb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename99.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename100) {
    create_file("testdata/SFZSink/key_notename100.sfz",
                "<region> sample=test.raw key=Gb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename100.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename101) {
    create_file("testdata/SFZSink/key_notename101.sfz",
                "<region> sample=test.raw key=A#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename101.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename102) {
    create_file("testdata/SFZSink/key_notename102.sfz",
                "<region> sample=test.raw key=A#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename102.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename103) {
    create_file("testdata/SFZSink/key_notename103.sfz",
                "<region> sample=test.raw key=AZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename103.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename104) {
    create_file("testdata/SFZSink/key_notename104.sfz",
                "<region> sample=test.raw key=Ab\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename104.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename105) {
    create_file("testdata/SFZSink/key_notename105.sfz",
                "<region> sample=test.raw key=Ab-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename105.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename106) {
    create_file("testdata/SFZSink/key_notename106.sfz",
                "<region> sample=test.raw key=Ab-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename106.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename107) {
    create_file("testdata/SFZSink/key_notename107.sfz",
                "<region> sample=test.raw key=B#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename107.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename108) {
    create_file("testdata/SFZSink/key_notename108.sfz",
                "<region> sample=test.raw key=B#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename108.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 0);
        EXPECT_EQ(regions[0].hikey, 0);
    }
}

TEST_F(SfzTest, key_notename109) {
    create_file("testdata/SFZSink/key_notename109.sfz",
                "<region> sample=test.raw key=B-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename109.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename110) {
    create_file("testdata/SFZSink/key_notename110.sfz",
                "<region> sample=test.raw key=Bb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename110.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename111) {
    create_file("testdata/SFZSink/key_notename111.sfz",
                "<region> sample=test.raw key=Bb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename111.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename112) {
    create_file("testdata/SFZSink/key_notename112.sfz",
                "<region> sample=test.raw key=BbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename112.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename113) {
    create_file("testdata/SFZSink/key_notename113.sfz",
                "<region> sample=test.raw key=c#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename113.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename114) {
    create_file("testdata/SFZSink/key_notename114.sfz",
                "<region> sample=test.raw key=c#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename114.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename115) {
    create_file("testdata/SFZSink/key_notename115.sfz",
                "<region> sample=test.raw key=c#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename115.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename116) {
    create_file("testdata/SFZSink/key_notename116.sfz",
                "<region> sample=test.raw key=c-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename116.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename117) {
    create_file("testdata/SFZSink/key_notename117.sfz",
                "<region> sample=test.raw key=cb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename117.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename118) {
    create_file("testdata/SFZSink/key_notename118.sfz",
                "<region> sample=test.raw key=cbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename118.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename119) {
    create_file("testdata/SFZSink/key_notename119.sfz",
                "<region> sample=test.raw key=d##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename119.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename120) {
    create_file("testdata/SFZSink/key_notename120.sfz",
                "<region> sample=test.raw key=d-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename120.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename121) {
    create_file("testdata/SFZSink/key_notename121.sfz",
                "<region> sample=test.raw key=d-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename121.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename122) {
    create_file("testdata/SFZSink/key_notename122.sfz",
                "<region> sample=test.raw key=d10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename122.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename123) {
    create_file("testdata/SFZSink/key_notename123.sfz",
                "<region> sample=test.raw key=dbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename123.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename124) {
    create_file("testdata/SFZSink/key_notename124.sfz",
                "<region> sample=test.raw key=dbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename124.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename125) {
    create_file("testdata/SFZSink/key_notename125.sfz",
                "<region> sample=test.raw key=e#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename125.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename126) {
    create_file("testdata/SFZSink/key_notename126.sfz",
                "<region> sample=test.raw key=e#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename126.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename127) {
    create_file("testdata/SFZSink/key_notename127.sfz",
                "<region> sample=test.raw key=e#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename127.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename128) {
    create_file("testdata/SFZSink/key_notename128.sfz",
                "<region> sample=test.raw key=eZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename128.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename129) {
    create_file("testdata/SFZSink/key_notename129.sfz",
                "<region> sample=test.raw key=eb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename129.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename130) {
    create_file("testdata/SFZSink/key_notename130.sfz",
                "<region> sample=test.raw key=eb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename130.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename131) {
    create_file("testdata/SFZSink/key_notename131.sfz",
                "<region> sample=test.raw key=f#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename131.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename132) {
    create_file("testdata/SFZSink/key_notename132.sfz",
                "<region> sample=test.raw key=f#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename132.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename133) {
    create_file("testdata/SFZSink/key_notename133.sfz",
                "<region> sample=test.raw key=f-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename133.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename134) {
    create_file("testdata/SFZSink/key_notename134.sfz",
                "<region> sample=test.raw key=fb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename134.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename135) {
    create_file("testdata/SFZSink/key_notename135.sfz",
                "<region> sample=test.raw key=fb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename135.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename136) {
    create_file("testdata/SFZSink/key_notename136.sfz",
                "<region> sample=test.raw key=fbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename136.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename137) {
    create_file("testdata/SFZSink/key_notename137.sfz",
                "<region> sample=test.raw key=g#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename137.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename138) {
    create_file("testdata/SFZSink/key_notename138.sfz",
                "<region> sample=test.raw key=g10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename138.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename139) {
    create_file("testdata/SFZSink/key_notename139.sfz",
                "<region> sample=test.raw key=gb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename139.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename140) {
    create_file("testdata/SFZSink/key_notename140.sfz",
                "<region> sample=test.raw key=gb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename140.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename141) {
    create_file("testdata/SFZSink/key_notename141.sfz",
                "<region> sample=test.raw key=gb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename141.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename142) {
    create_file("testdata/SFZSink/key_notename142.sfz",
                "<region> sample=test.raw key=gb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename142.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename143) {
    create_file("testdata/SFZSink/key_notename143.sfz",
                "<region> sample=test.raw key=a#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename143.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename144) {
    create_file("testdata/SFZSink/key_notename144.sfz",
                "<region> sample=test.raw key=a-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename144.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename145) {
    create_file("testdata/SFZSink/key_notename145.sfz",
                "<region> sample=test.raw key=a10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename145.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename146) {
    create_file("testdata/SFZSink/key_notename146.sfz",
                "<region> sample=test.raw key=ab#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename146.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename147) {
    create_file("testdata/SFZSink/key_notename147.sfz",
                "<region> sample=test.raw key=ab-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename147.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename148) {
    create_file("testdata/SFZSink/key_notename148.sfz",
                "<region> sample=test.raw key=abb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename148.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename149) {
    create_file("testdata/SFZSink/key_notename149.sfz",
                "<region> sample=test.raw key=b#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename149.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 1);
    if (regions.size() >= 1) {
        EXPECT_EQ(regions[0].lokey, 0);
        EXPECT_EQ(regions[0].hikey, 0);
    }
}

TEST_F(SfzTest, key_notename150) {
    create_file("testdata/SFZSink/key_notename150.sfz",
                "<region> sample=test.raw key=b#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename150.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename151) {
    create_file("testdata/SFZSink/key_notename151.sfz",
                "<region> sample=test.raw key=b10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename151.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename152) {
    create_file("testdata/SFZSink/key_notename152.sfz",
                "<region> sample=test.raw key=bb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename152.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename153) {
    create_file("testdata/SFZSink/key_notename153.sfz",
                "<region> sample=test.raw key=bb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename153.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename154) {
    create_file("testdata/SFZSink/key_notename154.sfz",
                "<region> sample=test.raw key=bbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename154.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename155) {
    create_file("testdata/SFZSink/key_notename155.sfz",
                "<region> sample=test.raw key=Z3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename155.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename156) {
    create_file("testdata/SFZSink/key_notename156.sfz",
                "<region> sample=test.raw key=Zb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename156.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename157) {
    create_file("testdata/SFZSink/key_notename157.sfz",
                "<region> sample=test.raw key=Z#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename157.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename158) {
    create_file("testdata/SFZSink/key_notename158.sfz",
                "<region> sample=test.raw key=10#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename158.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename159) {
    create_file("testdata/SFZSink/key_notename159.sfz",
                "<region> sample=test.raw key=10b3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename159.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename160) {
    create_file("testdata/SFZSink/key_notename160.sfz",
                "<region> sample=test.raw key==C3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename160.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename161) {
    create_file("testdata/SFZSink/key_notename161.sfz",
                "<region> sample=test.raw key=Cz3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename161.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename162) {
    create_file("testdata/SFZSink/key_notename162.sfz",
                "<region> sample=test.raw key=C♭3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename162.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}

TEST_F(SfzTest, key_notename163) {
    create_file("testdata/SFZSink/key_notename163.sfz",
                "<region> sample=test.raw key=C=3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename163.sfz");
    regions = getSfzData(sfz_test);

    EXPECT_EQ(regions.size(), 0);
}
