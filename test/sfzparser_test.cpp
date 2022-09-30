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

class TestHandler : public SFZHandler {
public:
    std::vector<String> headers;
    std::vector<String> opcodes;
    std::vector<String> values;
    void startSfz() override {
        headers.clear();
        opcodes.clear();
        values.clear();
    }
    void endSfz() override {
    }
    void header(const String &header) override {
        headers.push_back(header);
    }
    void opcode(const String &opcode, const String &value) override {
        opcodes.push_back(opcode);
        values.push_back(value);
    }
};

TEST(SFZParser, parserTest1) {
    create_file("testdata/SFZSink/outputtest1.sfz",
                "<region>\n key=1 sample=1.bin offset=10 end=19 loop_mode=no_loop\n"
                "<region>\n key=2 sample=2.bin offset=20 end=29 count=10\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/outputtest1.sfz");
    parser.parse(file, "testdata/SFZSink/outputtest1.sfz", &handler);
    EXPECT_EQ(2, handler.headers.size());
    EXPECT_EQ(10, handler.opcodes.size());
}

TEST(SFZParser, parserTest2) {
    create_file("testdata/SFZSink/outputtest2.sfz",
                "test==test\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/outputtest2.sfz");
    parser.parse(file, "testdata/SFZSink/outputtest2.sfz", &handler);
    EXPECT_EQ(1, handler.opcodes.size());
}


TEST(SFZParser_A_OK, sfz_parser1) {
    create_file("testdata/SFZSink/sfz_parser1.sfz",
                "<test>\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser1.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser1.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    if (handler.headers.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test");
    }
}

TEST(SFZParser_A_NG, sfz_parser2) {
    create_file("testdata/SFZSink/sfz_parser2.sfz",
                "<test\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser2.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser2.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 0);
}

TEST(SFZParser_B_OK, sfz_parser3) {
    create_file("testdata/SFZSink/sfz_parser3.sfz",
                "<test\n"
                ">\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser3.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser3.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    if (handler.headers.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test");
    }
}

TEST(SFZParser_A_OK, sfz_parser4) {
    create_file("testdata/SFZSink/sfz_parser4.sfz",
                "<test1><test2> <test3>\t<test4>\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser4.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser4.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 4);
    if (handler.headers.size() >= 4) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test1");
        EXPECT_STREQ(handler.headers[1].c_str(), "test2");
        EXPECT_STREQ(handler.headers[2].c_str(), "test3");
        EXPECT_STREQ(handler.headers[3].c_str(), "test4");
    }
}

TEST(SFZParser_A_NG, sfz_parser5) {
    create_file("testdata/SFZSink/sfz_parser5.sfz",
                "<test1><test2 <test3>\t<test4>\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser5.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser5.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 3);
    if (handler.headers.size() >= 3) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test1");
        EXPECT_STREQ(handler.headers[1].c_str(), "test3");
        EXPECT_STREQ(handler.headers[2].c_str(), "test4");
    }
}

TEST(SFZParser_A_OK, sfz_parser6) {
    create_file("testdata/SFZSink/sfz_parser6.sfz",
                "test=test_value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser6.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser6.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_A_OK, sfz_parser7) {
    create_file("testdata/SFZSink/sfz_parser7.sfz",
                "test1=test2=test3\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser7.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser7.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test1");
        EXPECT_STREQ(handler.values[0].c_str(), "test2=test3");
    }
}

TEST(SFZParser_B_OK, sfz_parser8) {
    create_file("testdata/SFZSink/sfz_parser8.sfz",
                "test=\n"
                "test_value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser8.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser8.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_NG, sfz_parser9) {
    create_file("testdata/SFZSink/sfz_parser9.sfz",
                "test_value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser9.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser9.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 0);
}

TEST(SFZParser_A_OK, sfz_parser10) {
    create_file("testdata/SFZSink/sfz_parser10.sfz",
                "test1=test_value1    test2=test_value2\t test3=test_value3\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser10.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser10.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 3);
    if (handler.opcodes.size() >= 3) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test1");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value1");
        EXPECT_STREQ(handler.opcodes[1].c_str(), "test2");
        EXPECT_STREQ(handler.values[1].c_str(), "test_value2");
        EXPECT_STREQ(handler.opcodes[2].c_str(), "test3");
        EXPECT_STREQ(handler.values[2].c_str(), "test_value3");
    }
}

TEST(SFZParser_A_OK, sfz_parser11) {
    create_file("testdata/SFZSink/sfz_parser11.sfz",
                "<test>test1=test_value1 test2=test_value2 test3=test_value3\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser11.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser11.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 3);
    if (handler.opcodes.size() >= 3) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test1");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value1");
        EXPECT_STREQ(handler.opcodes[1].c_str(), "test2");
        EXPECT_STREQ(handler.values[1].c_str(), "test_value2");
        EXPECT_STREQ(handler.opcodes[2].c_str(), "test3");
        EXPECT_STREQ(handler.values[2].c_str(), "test_value3");
    }
}

TEST(SFZParser_A_NG, sfz_parser12) {
    create_file("testdata/SFZSink/sfz_parser12.sfz",
                "<test>test_value0 test1=test_value1    test_value2\n"
                "test_value3\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser12.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser12.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test1");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value1    test_value2");
    }
}

TEST(SFZParser_A_QOK, parserTest13) {
    create_file("testdata/SFZSink/sfz_parser12.sfz",
                "test==test\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser12.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser12.sfz", &handler);
    EXPECT_EQ(1, handler.opcodes.size());
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "=test");
    }
}

TEST(SFZParser_A_OK, sfz_parser14) {
    create_file("testdata/SFZSink/sfz_parser14.sfz",
                "test1=test_value1\n"
                "//test2=test_value2\n"
                "test3=test_value3\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser14.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser14.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 2);
    if (handler.opcodes.size() >= 2) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test1");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value1");
        EXPECT_STREQ(handler.opcodes[1].c_str(), "test3");
        EXPECT_STREQ(handler.values[1].c_str(), "test_value3");
    }
}

TEST(SFZParser_A_OK, sfz_parser15) {
    create_file("testdata/SFZSink/sfz_parser15.sfz",
                "test1=test_value1\n"
                "//////////test2=test_value2\n"
                "test3=test_value3\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser15.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser15.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 2);
    if (handler.opcodes.size() >= 2) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test1");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value1");
        EXPECT_STREQ(handler.opcodes[1].c_str(), "test3");
        EXPECT_STREQ(handler.values[1].c_str(), "test_value3");
    }
}

TEST(SFZParser_A_NG, sfz_parser16) {
    create_file("testdata/SFZSink/sfz_parser16.sfz",
                "<test//>\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser16.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser16.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 0);
}

TEST(SFZParser_A_NG, sfz_parser17) {
    create_file("testdata/SFZSink/sfz_parser17.sfz",
                "test//=value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser17.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser17.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 0);
}

TEST(SFZParser_A_NG, sfz_parser18) {
    create_file("testdata/SFZSink/sfz_parser18.sfz",
                "test=//value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser18.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser18.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 0);
}

TEST(SFZParser_B_OK, sfz_parser19) {
    create_file("testdata/SFZSink/sfz_parser19.sfz",
                "/test=value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser19.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser19.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "/test");
        EXPECT_STREQ(handler.values[0].c_str(), "value");
    }
}

TEST(SFZParser_A_OK, sfz_parser20) {
    create_file("testdata/SFZSink/sfz_parser20.sfz",
                "test=value//comment\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser20.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser20.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "value");
    }
}

TEST(SFZParser_A_OK, sfz_parser21) {
    create_file("testdata/SFZSink/sfz_parser21.sfz",
                "//test=test_value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser21.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser21.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 0);
}

TEST(SFZParser_B_OK, sfz_parser22) {
    create_file("testdata/SFZSink/sfz_parser22.sfz",
                "/test=test_value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser22.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser22.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "/test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_OK, sfz_parser23) {
    create_file("testdata/SFZSink/sfz_parser23.sfz",
                "//てすと\n"
                "test=test_value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser23.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser23.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_A_OK, sfz_parser24) {
    create_file("testdata/SFZSink/sfz_parser24.sfz",
                "//\ttest\n"
                "test=test_value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser24.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser24.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_A_OK, sfz_parser25) {
    create_file("testdata/SFZSink/sfz_parser25.sfz",
                "// <test1> test2=test_value\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser25.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser25.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 0);
}

TEST(SFZParser_B_QOK, sfz_parser26) {
    create_file("testdata/SFZSink/sfz_parser26.sfz",
                "# include \"include.sfz\"\n"
                "# define $TEST test \n"
                "test2=$TEST.raw");
    create_file("testdata/SFZSink/include.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser26.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser26.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test2");
        EXPECT_STREQ(handler.values[0].c_str(), "$TEST.raw");
    }
}

TEST(SFZParser_B_QOK, sfz_parser27) {
    create_file("testdata/SFZSink/sfz_parser27.sfz",
                "#TEST \"include.sfz\"\n"
                "#TEST2 $TEST test \n"
                "test2=$TEST.raw");
    create_file("testdata/SFZSink/include.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser27.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser27.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test2");
        EXPECT_STREQ(handler.values[0].c_str(), "$TEST.raw");
    }
}

TEST(SFZParser_A_OK, sfz_parser28) {
    create_file("testdata/SFZSink/sfz_parser28.sfz",
                "#include \"include.sfz\"\n"
                "");
    create_file("testdata/SFZSink/include.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser28.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser28.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_NG, sfz_parser29) {
    create_file("testdata/SFZSink/sfz_parser29.sfz",
                "#include test \"include.sfz\"\n"
                "");
    create_file("testdata/SFZSink/include.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser29.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser29.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 0);
}

TEST(SFZParser_B_NG, sfz_parser30) {
    create_file("testdata/SFZSink/sfz_parser30.sfz",
                "#include \"include.sfz\n"
                "#include \"include2.sfz\"\n"
                "");
    create_file("testdata/SFZSink/include.sfz",
                "<test> test=test_value\n"
                "");
    create_file("testdata/SFZSink/include2.sfz",
                "<test2> test2=test_value2\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser30.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser30.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test2");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value2");
    }
}

TEST(SFZParser_A_OK, sfz_parser31) {
    create_file("testdata/SFZSink/sfz_parser31.sfz",
                "#include      \"include.sfz\"\n"
                "");
    create_file("testdata/SFZSink/include.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser31.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser31.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_A_OK, sfz_parser32) {
    create_file("testdata/SFZSink/sfz_parser32.sfz",
                "#include\t\t\t\"include.sfz\"\n"
                "");
    create_file("testdata/SFZSink/include.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser32.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser32.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_NG, sfz_parser33) {
    create_file("testdata/SFZSink/sfz_parser33.sfz",
                "#include\"include.sfz\"\n"
                "");
    create_file("testdata/SFZSink/include.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser33.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser33.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 0);
}

TEST(SFZParser_A_OK, sfz_parser34) {
    create_file("testdata/SFZSink/sfz_parser34.sfz",
                "#include \"include space.sfz\"\n"
                "");
    create_file("testdata/SFZSink/include space.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser34.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser34.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_OK, sfz_parser35) {
    create_file("testdata/SFZSink/sfz_parser35.sfz",
                "#include \"include　space.sfz\"\n"
                "");
    create_file("testdata/SFZSink/include　space.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser35.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser35.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_OK, sfz_parser36) {
    create_file("testdata/SFZSink/sfz_parser36.sfz",
                "#include \"てすと.sfz\"\n"
                "");
    create_file("testdata/SFZSink/てすと.sfz",
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser36.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser36.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_A_OK, sfz_parser37) {
    create_file("testdata/SFZSink/sfz_parser37.sfz",
                "#define $TEST DEFINE_VALUE\n"
                "<test> test$TEST=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser37.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser37.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "testDEFINE_VALUE");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_A_OK, sfz_parser38) {
    create_file("testdata/SFZSink/sfz_parser38.sfz",
                "#define    $TEST      DEFINE_VALUE\n"
                "<test> test$TEST=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser38.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser38.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "testDEFINE_VALUE");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_A_OK, sfz_parser39) {
    create_file("testdata/SFZSink/sfz_parser39.sfz",
                "#define\t$TEST   \t   DEFINE_VALUE\n"
                "<test> test$TEST=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser39.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser39.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "testDEFINE_VALUE");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_NG, sfz_parser40) {
    create_file("testdata/SFZSink/sfz_parser40.sfz",
                "#define$TEST DEFINE_VALUE\n"
                "<test> test$TEST=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser40.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser40.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test$TEST");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_OK, sfz_parser41) {
    create_file("testdata/SFZSink/sfz_parser41.sfz",
                "#define $$TEST DEFINE_VALUE\n"
                "<test> test$$TEST=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser41.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser41.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "testDEFINE_VALUE");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_NG, sfz_parser42) {
    create_file("testdata/SFZSink/sfz_parser42.sfz",
                "#define test $TEST DEFINE_VALUE\n"
                "<test> test$TEST=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser42.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser42.sfz", &handler);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test$TEST");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_QOK, sfz_parser43) {
    create_file("testdata/SFZSink/sfz_parser43.sfz",
                "<test test>\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser43.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser43.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    if (handler.headers.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test test");
    }
}

TEST(SFZParser_B_QOK, sfz_parser44) {
    create_file("testdata/SFZSink/sfz_parser44.sfz",
                "<test \t test>\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser44.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser44.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    if (handler.headers.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test  test");
    }
}

TEST(SFZParser_B_QOK, sfz_parser45) {
    create_file("testdata/SFZSink/sfz_parser45.sfz",
                "<test　test>\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser45.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser45.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    if (handler.headers.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "testtest");
    }
}

TEST(SFZParser_B_QOK, sfz_parser46) {
    create_file("testdata/SFZSink/sfz_parser46.sfz",
                "<てすと>\n");
    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser46.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser46.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    if (handler.headers.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "てすと");
    }
}

TEST(SFZParser_A_NG, sfz_parser47) {
    create_file("testdata/SFZSink/sfz_parser47.sfz",
                "=test\n"
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser47.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser47.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.headers.size() >= 1 && handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test");
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser_B_QOK, sfz_parser48) {
    create_file("testdata/SFZSink/sfz_parser48.sfz",
                "=test\n"
                "<test> key=60 =test\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser48.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser48.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.headers.size() >= 1 && handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test");
        EXPECT_STREQ(handler.opcodes[0].c_str(), "key");
        EXPECT_STREQ(handler.values[0].c_str(), "60");
    }
}

TEST(SFZParser_B_NG, sfz_parser49) {
    create_file("testdata/SFZSink/sfz_parser49.sfz",
                "てすと\n"
                "<test> test=test_value\n"
                "");

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/sfz_parser49.sfz");
    parser.parse(file, "testdata/SFZSink/sfz_parser49.sfz", &handler);
    EXPECT_EQ(handler.headers.size(), 1);
    EXPECT_EQ(handler.opcodes.size(), 1);
    if (handler.headers.size() >= 1 && handler.opcodes.size() >= 1) {
        EXPECT_STREQ(handler.headers[0].c_str(), "test");
        EXPECT_STREQ(handler.opcodes[0].c_str(), "test");
        EXPECT_STREQ(handler.values[0].c_str(), "test_value");
    }
}

TEST(SFZParser, include) {
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

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/include.sfz");
    parser.parse(file, "testdata/SFZSink/include.sfz", &handler);

    EXPECT_EQ(handler.values.size(), 2);

    if (handler.values.size() >= 2) {
        EXPECT_STREQ(handler.values[0].c_str(), "test1_space.raw");
        EXPECT_STREQ(handler.values[1].c_str(), "test2_tab.raw");
    }
}

TEST(SFZParser, define1) {
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

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/define1.sfz");
    parser.parse(file, "testdata/SFZSink/define1.sfz", &handler);

    EXPECT_EQ(handler.values.size(), 1);
    if (handler.values.size() >= 1) {
        EXPECT_STREQ(handler.values[0].c_str(), "$AAtest1_spacedef1.raw");
    }
}

TEST(SfzParser, define2) {
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

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/define2.sfz");
    parser.parse(file, "testdata/SFZSink/define2.sfz", &handler);

    EXPECT_EQ(handler.values.size(), 1);
    if (handler.values.size() >= 1) {
        EXPECT_STREQ(handler.values[0].c_str(), "test2.raw");
    }
}


TEST(SfzParser, define3) {
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

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/define3.sfz");
    parser.parse(file, "testdata/SFZSink/define3.sfz", &handler);

    EXPECT_EQ(handler.values.size(), 1);
    if (handler.values.size() >= 1) {
        EXPECT_STREQ(handler.values[0].c_str(), "$ERR1test3.raw");
    }
}

TEST(SfzParser, define4) {
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

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/define4.sfz");
    parser.parse(file, "testdata/SFZSink/define4.sfz", &handler);

    EXPECT_EQ(handler.values.size(), 1);
    if (handler.values.size() >= 1) {
        EXPECT_STREQ(handler.values[0].c_str(), "ERR2test4.raw");
    }
}

TEST(SfzParser, define5) {
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

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/define5.sfz");
    parser.parse(file, "testdata/SFZSink/define5.sfz", &handler);

    EXPECT_EQ(handler.values.size(), 1);
    if (handler.values.size() >= 1) {
        EXPECT_STREQ(handler.values[0].c_str(), "$TEST5test5.raw");
    }
}

TEST(SfzParser, define6) {
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

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/define6.sfz");
    parser.parse(file, "testdata/SFZSink/define6.sfz", &handler);

    EXPECT_EQ(handler.values.size(), 1);
    if (handler.values.size() >= 1) {
        EXPECT_STREQ(handler.values[0].c_str(), "$TEST6test6.raw");
    }
}

TEST(SfzParser, define7) {
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

    SFZParser parser;
    TestHandler handler;
    File file("testdata/SFZSink/define7.sfz");
    parser.parse(file, "testdata/SFZSink/define7.sfz", &handler);

    EXPECT_EQ(handler.values.size(), 1);
    if (handler.values.size() >= 1) {
        EXPECT_STREQ(handler.values[0].c_str(), "$test7.raw");
    }
}
