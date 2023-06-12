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

static void create_file(const String& file_path, const String& text) {
    registerDummyFile(file_path, (uint8_t*)text.c_str(), text.length());
}

class SfzNotenameTest : public ::testing::Test {
public:
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
};

TEST_F(SfzNotenameTest, key_notename1) {
    create_file("testdata/SFZSink/key_notename1.sfz",
                "<region> sample=test.raw key=Cb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename1.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 107);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 107);
    }
}

TEST_F(SfzNotenameTest, key_notename2) {
    create_file("testdata/SFZSink/key_notename2.sfz",
                "<region> sample=test.raw key=C-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename2.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 0);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 0);
    }
}

TEST_F(SfzNotenameTest, key_notename3) {
    create_file("testdata/SFZSink/key_notename3.sfz",
                "<region> sample=test.raw key=C8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename3.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 108);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 108);
    }
}

TEST_F(SfzNotenameTest, key_notename4) {
    create_file("testdata/SFZSink/key_notename4.sfz",
                "<region> sample=test.raw key=C#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename4.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 13);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 13);
    }
}

TEST_F(SfzNotenameTest, key_notename5) {
    create_file("testdata/SFZSink/key_notename5.sfz",
                "<region> sample=test.raw key=C#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename5.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 49);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 49);
    }
}

TEST_F(SfzNotenameTest, key_notename6) {
    create_file("testdata/SFZSink/key_notename6.sfz",
                "<region> sample=test.raw key=Db3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename6.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 49);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 49);
    }
}

TEST_F(SfzNotenameTest, key_notename7) {
    create_file("testdata/SFZSink/key_notename7.sfz",
                "<region> sample=test.raw key=Db9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename7.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 121);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 121);
    }
}

TEST_F(SfzNotenameTest, key_notename8) {
    create_file("testdata/SFZSink/key_notename8.sfz",
                "<region> sample=test.raw key=D-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename8.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 2);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 2);
    }
}

TEST_F(SfzNotenameTest, key_notename9) {
    create_file("testdata/SFZSink/key_notename9.sfz",
                "<region> sample=test.raw key=D8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename9.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 110);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 110);
    }
}

TEST_F(SfzNotenameTest, key_notename10) {
    create_file("testdata/SFZSink/key_notename10.sfz",
                "<region> sample=test.raw key=D#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename10.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 15);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 15);
    }
}

TEST_F(SfzNotenameTest, key_notename11) {
    create_file("testdata/SFZSink/key_notename11.sfz",
                "<region> sample=test.raw key=Eb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename11.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 51);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 51);
    }
}

TEST_F(SfzNotenameTest, key_notename12) {
    create_file("testdata/SFZSink/key_notename12.sfz",
                "<region> sample=test.raw key=Eb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename12.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 111);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 111);
    }
}

TEST_F(SfzNotenameTest, key_notename13) {
    create_file("testdata/SFZSink/key_notename13.sfz",
                "<region> sample=test.raw key=E-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename13.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 4);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 4);
    }
}

TEST_F(SfzNotenameTest, key_notename14) {
    create_file("testdata/SFZSink/key_notename14.sfz",
                "<region> sample=test.raw key=E#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename14.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 17);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 17);
    }
}

TEST_F(SfzNotenameTest, key_notename15) {
    create_file("testdata/SFZSink/key_notename15.sfz",
                "<region> sample=test.raw key=E#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename15.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 125);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 125);
    }
}

TEST_F(SfzNotenameTest, key_notename16) {
    create_file("testdata/SFZSink/key_notename16.sfz",
                "<region> sample=test.raw key=Fb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename16.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 16);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 16);
    }
}

TEST_F(SfzNotenameTest, key_notename17) {
    create_file("testdata/SFZSink/key_notename17.sfz",
                "<region> sample=test.raw key=Fb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename17.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 52);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 52);
    }
}

TEST_F(SfzNotenameTest, key_notename18) {
    create_file("testdata/SFZSink/key_notename18.sfz",
                "<region> sample=test.raw key=F8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename18.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 113);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 113);
    }
}

TEST_F(SfzNotenameTest, key_notename19) {
    create_file("testdata/SFZSink/key_notename19.sfz",
                "<region> sample=test.raw key=F#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename19.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 6);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 6);
    }
}

TEST_F(SfzNotenameTest, key_notename20) {
    create_file("testdata/SFZSink/key_notename20.sfz",
                "<region> sample=test.raw key=F#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename20.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 126);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 126);
    }
}

TEST_F(SfzNotenameTest, key_notename21) {
    create_file("testdata/SFZSink/key_notename21.sfz",
                "<region> sample=test.raw key=Gb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename21.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 114);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 114);
    }
}

TEST_F(SfzNotenameTest, key_notename22) {
    create_file("testdata/SFZSink/key_notename22.sfz",
                "<region> sample=test.raw key=Gb9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename22.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 126);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 126);
    }
}

TEST_F(SfzNotenameTest, key_notename23) {
    create_file("testdata/SFZSink/key_notename23.sfz",
                "<region> sample=test.raw key=G-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename23.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 7);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 7);
    }
}

TEST_F(SfzNotenameTest, key_notename24) {
    create_file("testdata/SFZSink/key_notename24.sfz",
                "<region> sample=test.raw key=G0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename24.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 19);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 19);
    }
}

TEST_F(SfzNotenameTest, key_notename25) {
    create_file("testdata/SFZSink/key_notename25.sfz",
                "<region> sample=test.raw key=G#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename25.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 56);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 56);
    }
}

TEST_F(SfzNotenameTest, key_notename26) {
    create_file("testdata/SFZSink/key_notename26.sfz",
                "<region> sample=test.raw key=Ab0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename26.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 20);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 20);
    }
}

TEST_F(SfzNotenameTest, key_notename27) {
    create_file("testdata/SFZSink/key_notename27.sfz",
                "<region> sample=test.raw key=A3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename27.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 57);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 57);
    }
}

TEST_F(SfzNotenameTest, key_notename28) {
    create_file("testdata/SFZSink/key_notename28.sfz",
                "<region> sample=test.raw key=A#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename28.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 10);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 10);
    }
}

TEST_F(SfzNotenameTest, key_notename29) {
    create_file("testdata/SFZSink/key_notename29.sfz",
                "<region> sample=test.raw key=A#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename29.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 106);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 106);
    }
}

TEST_F(SfzNotenameTest, key_notename30) {
    create_file("testdata/SFZSink/key_notename30.sfz",
                "<region> sample=test.raw key=A#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename30.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 118);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 118);
    }
}

TEST_F(SfzNotenameTest, key_notename31) {
    create_file("testdata/SFZSink/key_notename31.sfz",
                "<region> sample=test.raw key=Bb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename31.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 10);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 10);
    }
}

TEST_F(SfzNotenameTest, key_notename32) {
    create_file("testdata/SFZSink/key_notename32.sfz",
                "<region> sample=test.raw key=Bb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename32.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 22);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 22);
    }
}

TEST_F(SfzNotenameTest, key_notename33) {
    create_file("testdata/SFZSink/key_notename33.sfz",
                "<region> sample=test.raw key=B3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename33.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 59);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 59);
    }
}

TEST_F(SfzNotenameTest, key_notename34) {
    create_file("testdata/SFZSink/key_notename34.sfz",
                "<region> sample=test.raw key=B8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename34.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 119);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 119);
    }
}

TEST_F(SfzNotenameTest, key_notename35) {
    create_file("testdata/SFZSink/key_notename35.sfz",
                "<region> sample=test.raw key=B#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename35.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 108);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 108);
    }
}

TEST_F(SfzNotenameTest, key_notename36) {
    create_file("testdata/SFZSink/key_notename36.sfz",
                "<region> sample=test.raw key=cb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename36.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename37) {
    create_file("testdata/SFZSink/key_notename37.sfz",
                "<region> sample=test.raw key=cb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename37.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 47);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 47);
    }
}

TEST_F(SfzNotenameTest, key_notename38) {
    create_file("testdata/SFZSink/key_notename38.sfz",
                "<region> sample=test.raw key=cb7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename38.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 95);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 95);
    }
}

TEST_F(SfzNotenameTest, key_notename39) {
    create_file("testdata/SFZSink/key_notename39.sfz",
                "<region> sample=test.raw key=c9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename39.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 120);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 120);
    }
}

TEST_F(SfzNotenameTest, key_notename40) {
    create_file("testdata/SFZSink/key_notename40.sfz",
                "<region> sample=test.raw key=c#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename40.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 13);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 13);
    }
}

TEST_F(SfzNotenameTest, key_notename41) {
    create_file("testdata/SFZSink/key_notename41.sfz",
                "<region> sample=test.raw key=db3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename41.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 49);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 49);
    }
}

TEST_F(SfzNotenameTest, key_notename42) {
    create_file("testdata/SFZSink/key_notename42.sfz",
                "<region> sample=test.raw key=d-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename42.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 2);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 2);
    }
}

TEST_F(SfzNotenameTest, key_notename43) {
    create_file("testdata/SFZSink/key_notename43.sfz",
                "<region> sample=test.raw key=d0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename43.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 14);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 14);
    }
}

TEST_F(SfzNotenameTest, key_notename44) {
    create_file("testdata/SFZSink/key_notename44.sfz",
                "<region> sample=test.raw key=d#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename44.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 111);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 111);
    }
}

TEST_F(SfzNotenameTest, key_notename45) {
    create_file("testdata/SFZSink/key_notename45.sfz",
                "<region> sample=test.raw key=d#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename45.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 123);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 123);
    }
}

TEST_F(SfzNotenameTest, key_notename46) {
    create_file("testdata/SFZSink/key_notename46.sfz",
                "<region> sample=test.raw key=eb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename46.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 111);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 111);
    }
}

TEST_F(SfzNotenameTest, key_notename47) {
    create_file("testdata/SFZSink/key_notename47.sfz",
                "<region> sample=test.raw key=e0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename47.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 16);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 16);
    }
}

TEST_F(SfzNotenameTest, key_notename48) {
    create_file("testdata/SFZSink/key_notename48.sfz",
                "<region> sample=test.raw key=e#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename48.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 5);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 5);
    }
}

TEST_F(SfzNotenameTest, key_notename49) {
    create_file("testdata/SFZSink/key_notename49.sfz",
                "<region> sample=test.raw key=e#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename49.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 53);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 53);
    }
}

TEST_F(SfzNotenameTest, key_notename50) {
    create_file("testdata/SFZSink/key_notename50.sfz",
                "<region> sample=test.raw key=e#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename50.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 125);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 125);
    }
}

TEST_F(SfzNotenameTest, key_notename51) {
    create_file("testdata/SFZSink/key_notename51.sfz",
                "<region> sample=test.raw key=fb-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename51.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 4);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 4);
    }
}

TEST_F(SfzNotenameTest, key_notename52) {
    create_file("testdata/SFZSink/key_notename52.sfz",
                "<region> sample=test.raw key=f3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename52.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 53);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 53);
    }
}

TEST_F(SfzNotenameTest, key_notename53) {
    create_file("testdata/SFZSink/key_notename53.sfz",
                "<region> sample=test.raw key=f#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename53.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 18);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 18);
    }
}

TEST_F(SfzNotenameTest, key_notename54) {
    create_file("testdata/SFZSink/key_notename54.sfz",
                "<region> sample=test.raw key=f#8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename54.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 114);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 114);
    }
}

TEST_F(SfzNotenameTest, key_notename55) {
    create_file("testdata/SFZSink/key_notename55.sfz",
                "<region> sample=test.raw key=f#9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename55.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 126);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 126);
    }
}

TEST_F(SfzNotenameTest, key_notename56) {
    create_file("testdata/SFZSink/key_notename56.sfz",
                "<region> sample=test.raw key=g9\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename56.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 127);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 127);
    }
}

TEST_F(SfzNotenameTest, key_notename57) {
    create_file("testdata/SFZSink/key_notename57.sfz",
                "<region> sample=test.raw key=g#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename57.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 8);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 8);
    }
}

TEST_F(SfzNotenameTest, key_notename58) {
    create_file("testdata/SFZSink/key_notename58.sfz",
                "<region> sample=test.raw key=g#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename58.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 56);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 56);
    }
}

TEST_F(SfzNotenameTest, key_notename59) {
    create_file("testdata/SFZSink/key_notename59.sfz",
                "<region> sample=test.raw key=g#7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename59.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 104);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 104);
    }
}

TEST_F(SfzNotenameTest, key_notename60) {
    create_file("testdata/SFZSink/key_notename60.sfz",
                "<region> sample=test.raw key=gb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename60.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 18);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 18);
    }
}

TEST_F(SfzNotenameTest, key_notename61) {
    create_file("testdata/SFZSink/key_notename61.sfz",
                "<region> sample=test.raw key=ab7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename61.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 104);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 104);
    }
}

TEST_F(SfzNotenameTest, key_notename62) {
    create_file("testdata/SFZSink/key_notename62.sfz",
                "<region> sample=test.raw key=ab8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename62.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 116);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 116);
    }
}

TEST_F(SfzNotenameTest, key_notename63) {
    create_file("testdata/SFZSink/key_notename63.sfz",
                "<region> sample=test.raw key=a3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename63.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 57);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 57);
    }
}

TEST_F(SfzNotenameTest, key_notename64) {
    create_file("testdata/SFZSink/key_notename64.sfz",
                "<region> sample=test.raw key=a#-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename64.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 10);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 10);
    }
}

TEST_F(SfzNotenameTest, key_notename65) {
    create_file("testdata/SFZSink/key_notename65.sfz",
                "<region> sample=test.raw key=a#0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename65.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 22);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 22);
    }
}

TEST_F(SfzNotenameTest, key_notename66) {
    create_file("testdata/SFZSink/key_notename66.sfz",
                "<region> sample=test.raw key=bb0\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename66.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 22);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 22);
    }
}

TEST_F(SfzNotenameTest, key_notename67) {
    create_file("testdata/SFZSink/key_notename67.sfz",
                "<region> sample=test.raw key=bb8\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename67.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 118);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 118);
    }
}

TEST_F(SfzNotenameTest, key_notename68) {
    create_file("testdata/SFZSink/key_notename68.sfz",
                "<region> sample=test.raw key=b-1\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename68.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 11);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 11);
    }
}

TEST_F(SfzNotenameTest, key_notename69) {
    create_file("testdata/SFZSink/key_notename69.sfz",
                "<region> sample=test.raw key=b7\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename69.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 107);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 107);
    }
}

TEST_F(SfzNotenameTest, key_notename70) {
    create_file("testdata/SFZSink/key_notename70.sfz",
                "<region> sample=test.raw key=b#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename70.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 60);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 60);
    }
}

TEST_F(SfzNotenameTest, key_notename71) {
    create_file("testdata/SFZSink/key_notename71.sfz",
                "<region> sample=test.raw key=C##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename71.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename72) {
    create_file("testdata/SFZSink/key_notename72.sfz",
                "<region> sample=test.raw key=C#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename72.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename73) {
    create_file("testdata/SFZSink/key_notename73.sfz",
                "<region> sample=test.raw key=C#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename73.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename74) {
    create_file("testdata/SFZSink/key_notename74.sfz",
                "<region> sample=test.raw key=C-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename74.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename75) {
    create_file("testdata/SFZSink/key_notename75.sfz",
                "<region> sample=test.raw key=C10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename75.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename76) {
    create_file("testdata/SFZSink/key_notename76.sfz",
                "<region> sample=test.raw key=CbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename76.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename77) {
    create_file("testdata/SFZSink/key_notename77.sfz",
                "<region> sample=test.raw key=D#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename77.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename78) {
    create_file("testdata/SFZSink/key_notename78.sfz",
                "<region> sample=test.raw key=D#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename78.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename79) {
    create_file("testdata/SFZSink/key_notename79.sfz",
                "<region> sample=test.raw key=D#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename79.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename80) {
    create_file("testdata/SFZSink/key_notename80.sfz",
                "<region> sample=test.raw key=D#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename80.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename81) {
    create_file("testdata/SFZSink/key_notename81.sfz",
                "<region> sample=test.raw key=Db\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename81.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename82) {
    create_file("testdata/SFZSink/key_notename82.sfz",
                "<region> sample=test.raw key=Db-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename82.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename83) {
    create_file("testdata/SFZSink/key_notename83.sfz",
                "<region> sample=test.raw key=E#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename83.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename84) {
    create_file("testdata/SFZSink/key_notename84.sfz",
                "<region> sample=test.raw key=E#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename84.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename85) {
    create_file("testdata/SFZSink/key_notename85.sfz",
                "<region> sample=test.raw key=E#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename85.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename86) {
    create_file("testdata/SFZSink/key_notename86.sfz",
                "<region> sample=test.raw key=E-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename86.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename87) {
    create_file("testdata/SFZSink/key_notename87.sfz",
                "<region> sample=test.raw key=E10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename87.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename88) {
    create_file("testdata/SFZSink/key_notename88.sfz",
                "<region> sample=test.raw key=Eb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename88.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename89) {
    create_file("testdata/SFZSink/key_notename89.sfz",
                "<region> sample=test.raw key=F##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename89.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename90) {
    create_file("testdata/SFZSink/key_notename90.sfz",
                "<region> sample=test.raw key=F#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename90.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename91) {
    create_file("testdata/SFZSink/key_notename91.sfz",
                "<region> sample=test.raw key=F#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename91.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename92) {
    create_file("testdata/SFZSink/key_notename92.sfz",
                "<region> sample=test.raw key=FZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename92.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename93) {
    create_file("testdata/SFZSink/key_notename93.sfz",
                "<region> sample=test.raw key=Fb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename93.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename94) {
    create_file("testdata/SFZSink/key_notename94.sfz",
                "<region> sample=test.raw key=Fbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename94.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename95) {
    create_file("testdata/SFZSink/key_notename95.sfz",
                "<region> sample=test.raw key=G##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename95.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename96) {
    create_file("testdata/SFZSink/key_notename96.sfz",
                "<region> sample=test.raw key=G#-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename96.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename97) {
    create_file("testdata/SFZSink/key_notename97.sfz",
                "<region> sample=test.raw key=G#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename97.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename98) {
    create_file("testdata/SFZSink/key_notename98.sfz",
                "<region> sample=test.raw key=G10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename98.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename99) {
    create_file("testdata/SFZSink/key_notename99.sfz",
                "<region> sample=test.raw key=Gb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename99.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename100) {
    create_file("testdata/SFZSink/key_notename100.sfz",
                "<region> sample=test.raw key=Gb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename100.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename101) {
    create_file("testdata/SFZSink/key_notename101.sfz",
                "<region> sample=test.raw key=A#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename101.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename102) {
    create_file("testdata/SFZSink/key_notename102.sfz",
                "<region> sample=test.raw key=A#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename102.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename103) {
    create_file("testdata/SFZSink/key_notename103.sfz",
                "<region> sample=test.raw key=AZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename103.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename104) {
    create_file("testdata/SFZSink/key_notename104.sfz",
                "<region> sample=test.raw key=Ab\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename104.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename105) {
    create_file("testdata/SFZSink/key_notename105.sfz",
                "<region> sample=test.raw key=Ab-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename105.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename106) {
    create_file("testdata/SFZSink/key_notename106.sfz",
                "<region> sample=test.raw key=Ab-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename106.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename107) {
    create_file("testdata/SFZSink/key_notename107.sfz",
                "<region> sample=test.raw key=B#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename107.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename108) {
    create_file("testdata/SFZSink/key_notename108.sfz",
                "<region> sample=test.raw key=B#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename108.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 0);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 0);
    }
}

TEST_F(SfzNotenameTest, key_notename109) {
    create_file("testdata/SFZSink/key_notename109.sfz",
                "<region> sample=test.raw key=B-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename109.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename110) {
    create_file("testdata/SFZSink/key_notename110.sfz",
                "<region> sample=test.raw key=Bb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename110.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename111) {
    create_file("testdata/SFZSink/key_notename111.sfz",
                "<region> sample=test.raw key=Bb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename111.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename112) {
    create_file("testdata/SFZSink/key_notename112.sfz",
                "<region> sample=test.raw key=BbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename112.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename113) {
    create_file("testdata/SFZSink/key_notename113.sfz",
                "<region> sample=test.raw key=c#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename113.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename114) {
    create_file("testdata/SFZSink/key_notename114.sfz",
                "<region> sample=test.raw key=c#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename114.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename115) {
    create_file("testdata/SFZSink/key_notename115.sfz",
                "<region> sample=test.raw key=c#b\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename115.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename116) {
    create_file("testdata/SFZSink/key_notename116.sfz",
                "<region> sample=test.raw key=c-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename116.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename117) {
    create_file("testdata/SFZSink/key_notename117.sfz",
                "<region> sample=test.raw key=cb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename117.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename118) {
    create_file("testdata/SFZSink/key_notename118.sfz",
                "<region> sample=test.raw key=cbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename118.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename119) {
    create_file("testdata/SFZSink/key_notename119.sfz",
                "<region> sample=test.raw key=d##\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename119.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename120) {
    create_file("testdata/SFZSink/key_notename120.sfz",
                "<region> sample=test.raw key=d-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename120.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename121) {
    create_file("testdata/SFZSink/key_notename121.sfz",
                "<region> sample=test.raw key=d-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename121.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename122) {
    create_file("testdata/SFZSink/key_notename122.sfz",
                "<region> sample=test.raw key=d10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename122.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename123) {
    create_file("testdata/SFZSink/key_notename123.sfz",
                "<region> sample=test.raw key=dbZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename123.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename124) {
    create_file("testdata/SFZSink/key_notename124.sfz",
                "<region> sample=test.raw key=dbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename124.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename125) {
    create_file("testdata/SFZSink/key_notename125.sfz",
                "<region> sample=test.raw key=e#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename125.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename126) {
    create_file("testdata/SFZSink/key_notename126.sfz",
                "<region> sample=test.raw key=e#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename126.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename127) {
    create_file("testdata/SFZSink/key_notename127.sfz",
                "<region> sample=test.raw key=e#10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename127.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename128) {
    create_file("testdata/SFZSink/key_notename128.sfz",
                "<region> sample=test.raw key=eZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename128.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename129) {
    create_file("testdata/SFZSink/key_notename129.sfz",
                "<region> sample=test.raw key=eb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename129.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename130) {
    create_file("testdata/SFZSink/key_notename130.sfz",
                "<region> sample=test.raw key=eb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename130.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename131) {
    create_file("testdata/SFZSink/key_notename131.sfz",
                "<region> sample=test.raw key=f#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename131.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename132) {
    create_file("testdata/SFZSink/key_notename132.sfz",
                "<region> sample=test.raw key=f#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename132.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename133) {
    create_file("testdata/SFZSink/key_notename133.sfz",
                "<region> sample=test.raw key=f-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename133.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename134) {
    create_file("testdata/SFZSink/key_notename134.sfz",
                "<region> sample=test.raw key=fb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename134.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename135) {
    create_file("testdata/SFZSink/key_notename135.sfz",
                "<region> sample=test.raw key=fb10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename135.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename136) {
    create_file("testdata/SFZSink/key_notename136.sfz",
                "<region> sample=test.raw key=fbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename136.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename137) {
    create_file("testdata/SFZSink/key_notename137.sfz",
                "<region> sample=test.raw key=g#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename137.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename138) {
    create_file("testdata/SFZSink/key_notename138.sfz",
                "<region> sample=test.raw key=g10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename138.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename139) {
    create_file("testdata/SFZSink/key_notename139.sfz",
                "<region> sample=test.raw key=gb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename139.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename140) {
    create_file("testdata/SFZSink/key_notename140.sfz",
                "<region> sample=test.raw key=gb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename140.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename141) {
    create_file("testdata/SFZSink/key_notename141.sfz",
                "<region> sample=test.raw key=gb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename141.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename142) {
    create_file("testdata/SFZSink/key_notename142.sfz",
                "<region> sample=test.raw key=gb-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename142.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename143) {
    create_file("testdata/SFZSink/key_notename143.sfz",
                "<region> sample=test.raw key=a#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename143.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename144) {
    create_file("testdata/SFZSink/key_notename144.sfz",
                "<region> sample=test.raw key=a-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename144.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename145) {
    create_file("testdata/SFZSink/key_notename145.sfz",
                "<region> sample=test.raw key=a10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename145.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename146) {
    create_file("testdata/SFZSink/key_notename146.sfz",
                "<region> sample=test.raw key=ab#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename146.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename147) {
    create_file("testdata/SFZSink/key_notename147.sfz",
                "<region> sample=test.raw key=ab-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename147.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename148) {
    create_file("testdata/SFZSink/key_notename148.sfz",
                "<region> sample=test.raw key=abb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename148.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename149) {
    create_file("testdata/SFZSink/key_notename149.sfz",
                "<region> sample=test.raw key=b#-2\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename149.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 1);
    if (sfz_test.getNumberOfRegions() >= 1) {
        EXPECT_EQ(sfz_test.getRegion(0)->lokey, 0);
        EXPECT_EQ(sfz_test.getRegion(0)->hikey, 0);
    }
}

TEST_F(SfzNotenameTest, key_notename150) {
    create_file("testdata/SFZSink/key_notename150.sfz",
                "<region> sample=test.raw key=b#ZZ\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename150.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename151) {
    create_file("testdata/SFZSink/key_notename151.sfz",
                "<region> sample=test.raw key=b10\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename151.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename152) {
    create_file("testdata/SFZSink/key_notename152.sfz",
                "<region> sample=test.raw key=bb#\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename152.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename153) {
    create_file("testdata/SFZSink/key_notename153.sfz",
                "<region> sample=test.raw key=bb-100\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename153.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename154) {
    create_file("testdata/SFZSink/key_notename154.sfz",
                "<region> sample=test.raw key=bbb\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename154.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename155) {
    create_file("testdata/SFZSink/key_notename155.sfz",
                "<region> sample=test.raw key=Z3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename155.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename156) {
    create_file("testdata/SFZSink/key_notename156.sfz",
                "<region> sample=test.raw key=Zb3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename156.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename157) {
    create_file("testdata/SFZSink/key_notename157.sfz",
                "<region> sample=test.raw key=Z#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename157.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename158) {
    create_file("testdata/SFZSink/key_notename158.sfz",
                "<region> sample=test.raw key=10#3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename158.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename159) {
    create_file("testdata/SFZSink/key_notename159.sfz",
                "<region> sample=test.raw key=10b3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename159.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename160) {
    create_file("testdata/SFZSink/key_notename160.sfz",
                "<region> sample=test.raw key==C3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename160.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename161) {
    create_file("testdata/SFZSink/key_notename161.sfz",
                "<region> sample=test.raw key=Cz3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename161.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename162) {
    create_file("testdata/SFZSink/key_notename162.sfz",
                "<region> sample=test.raw key=C♭3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename162.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}

TEST_F(SfzNotenameTest, key_notename163) {
    create_file("testdata/SFZSink/key_notename163.sfz",
                "<region> sample=test.raw key=C=3\n"
                "");
    SFZSink sfz_test = SFZSink("testdata/SFZSink/key_notename163.sfz");
    sfz_test.begin();

    EXPECT_EQ(sfz_test.getNumberOfRegions(), 0);
}
