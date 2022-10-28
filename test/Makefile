#
# SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
#
# Copyright 2022 Sony Semiconductor Solutions Corporation
#

default: all

INCLUDES = -I stub -I ../src
CXX = g++
CXXFLAGS = -Wall -Werror -g -fprofile-arcs -ftest-coverage -pthread -DARDUINO_ARCH_SPRESENSE
GTESTFLAGS = -lgtest -lgtest_main

GCOVFLAGS = --branch-probabilities --branch-counts

ALL_OBJECTS = ../src/BaseFilter.o \
              ../src/BufferedFileReader.o \
			  ../src/CorrectToneFilter.o \
              ../src/NullFilter.o \
              ../src/OctaveShift.o \
			  ../src/OneKeySynthesizerFilter.o \
              ../src/path_util.o \
              ../src/PcmRenderer.o \
              ../src/ScoreSrc.o \
              ../src/SDSeekSink.o \
              ../src/SDSink.o \
              ../src/SmfParser.o \
              ../src/SFZParser.o \
              ../src/SFZSink.o \
              ../src/TarReader.o \
              ../src/TextScoreParser.o \
              ../src/ToneFilter.o \
              ../src/VoiceCapture.o \
              ../src/VoiceTriggerSrc.o \
              ../src/WavReader.o \
              ../src/YuruhornSrc.o \
              ../src/YuruInstrumentConfig.o \
              ../src/YuruInstrumentConsole.o \
			  stub/stub.o

YURUHORNSRC_TEST_OBJECTS = yuruhornsrc_test.o $(ALL_OBJECTS)
SCORESRC_TEST_OBJECTS = scoresrc_test.o $(ALL_OBJECTS)
SDSINK_TEST_OBJECTS = sdsink_test.o $(ALL_OBJECTS)
SFZSINK_TEST_OBJECTS = sfzsink_test.o $(ALL_OBJECTS)
SFZSINK_NOTENAME_TEST_OBJECTS = sfzsink_notename_test.o $(ALL_OBJECTS)
SDSEEKSINK_TEST_OBJECTS = sdseeksink_test.o $(ALL_OBJECTS)
SFZPARSER_TEST_OBJECTS = sfzparser_test.o $(ALL_OBJECTS)

%PHONY: default
default: run

%PHONY: all
all: yuruhornsrc_test.exe scoresrc_test.exe sdsink_test.exe sfzsink_test.exe sfzsink_notename_test.exe sdseeksink_test.exe sfzparser_test.exe

%PHONY: check
check: all
	./yuruhornsrc_test.exe --gtest_output=xml:yuruhornsrc_test.xml
	./scoresrc_test.exe --gtest_output=xml:scoresrc_test.xml
	./sdsink_test.exe --gtest_output=xml:sdsink_test.xml
	./sfzsink_test.exe --gtest_output=xml:sfzsink_test.xml
	./sfzsink_notename_test.exe --gtest_output=xml:sfzsink_notename_test.xml
	./sdseeksink_test.exe --gtest_output=xml:sdseeksink_test.xml
	./sfzparser_test.exe --gtest_filter="SFZParser_A_OK.*" --gtest_output=xml:sfzpaser_test.xml

%PHONY: run
run: all
	./yuruhornsrc_test.exe --gtest_output=xml:yuruhornsrc_test.xml || true
	./scoresrc_test.exe --gtest_output=xml:scoresrc_test.xml || true
	./sdsink_test.exe --gtest_output=xml:sdsink_test.xml || true
	./sfzsink_test.exe --gtest_output=xml:sfzsink_test.xml || true
	./sfzsink_notename_test.exe --gtest_output=xml:sfzsink_notename_test.xml || true
	./sdseeksink_test.exe --gtest_output=xml:sdseeksink_test.xml || true
	./sfzparser_test.exe --gtest_filter="SFZParser_A_OK.*" --gtest_output=xml:sfzpaser_test.xml || true

%PHONY: clean
clean:
	rm -f *.gcno *.gcda *.gcov
	rm -f $(YURUHORNSRC_TEST_OBJECTS)
	rm -f yuruhornsrc_test.exe
	rm -f $(SCORESRC_TEST_OBJECTS)
	rm -f scoresrc_test.exe
	rm -f $(SDSINK_TEST_OBJECTS)
	rm -f sdsink_test.exe
	rm -f $(SFZSINK_TEST_OBJECTS)
	rm -f sfzsink_test.exe
	rm -f $(SFZSINK_NOTENAME_TEST_OBJECTS)
	rm -f sfzsink_notename_test.exe
	rm -f $(SDSEEKSINK_TEST_OBJECTS)
	rm -f sdseeksink_test.exe
	rm -f $(SFZPARSER_TEST_OBJECTS)
	rm -f sfzparser_test.exe

%PHONY: gcov
gcov: run
	gcov $(GCOVFLAGS) yuruhornsrc_test.exe
	gcov $(GCOVFLAGS) scoresrc_test.exe
	gcov $(GCOVFLAGS) sdsink_test.exe
	gcov $(GCOVFLAGS) sfzsink_test.exe --gtest_filter="-SFZSink.outputtest*"
	gcov $(GCOVFLAGS) sfzsink_notename_test.exe
	gcov $(GCOVFLAGS) sdseeksink_test.exe
	gcov $(GCOVFLAGS) sfzparser_test.exe --gtest_filter="SFZParser_A_OK.*"
	gcovr --xml-pretty --exclude-unreachable-branches --print-summary -o coverage.xml --root ../YuruHorn

%.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $< -c -o $@

yuruhornsrc_test.exe: $(YURUHORNSRC_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

scoresrc_test.exe: $(SCORESRC_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sdsink_test.exe: $(SDSINK_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sfzsink_test.exe: $(SFZSINK_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sfzsink_notename_test.exe: $(SFZSINK_NOTENAME_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sdseeksink_test.exe: $(SDSEEKSINK_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sfzparser_test.exe: $(SFZPARSER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@
