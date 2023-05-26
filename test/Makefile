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
			  ../src/ChannelFilter.o \
              ../src/CorrectToneFilter.o \
              ../src/midi_util.o \
              ../src/NullFilter.o \
              ../src/OctaveShift.o \
              ../src/OneKeySynthesizerFilter.o \
              ../src/path_util.o \
              ../src/ParserFactory.o \
              ../src/PcmRenderer.o \
              ../src/PlaylistParser.o \
              ../src/ScoreFilter.o \
              ../src/ScoreParser.o \
              ../src/ScoreSrc.o \
              ../src/SDSink.o \
              ../src/SmfParser.o \
              ../src/SFZParser.o \
              ../src/SFZSink.o \
              ../src/TextScoreParser.o \
              ../src/TimeKeeper.o \
              ../src/ToneFilter.o \
              ../src/VoiceCapture.o \
              ../src/VoiceTriggerSrc.o \
              ../src/WavReader.o \
              ../src/YuruhornSrc.o \
              ../src/YuruInstrumentConfig.o \
              ../src/YuruInstrumentConsole.o \
              stub/stub.o

YURUHORNSRC_TEST_OBJECTS = yuruhornsrc_test.o $(ALL_OBJECTS)
SCOREFILTER_TEST_OBJECTS = scorefilter_test.o $(ALL_OBJECTS)
SCORESRC_TEST_OBJECTS = scoresrc_test.o $(ALL_OBJECTS)
SDSINK_TEST_OBJECTS = sdsink_test.o $(ALL_OBJECTS)
SFZSINK_TEST_OBJECTS = sfzsink_test.o $(ALL_OBJECTS)
SFZSINK_NOTENAME_TEST_OBJECTS = sfzsink_notename_test.o $(ALL_OBJECTS)
SFZPARSER_TEST_OBJECTS = sfzparser_test.o $(ALL_OBJECTS)
CHANNELFILTER_TEST_OBJECTS = channelfilter_test.o $(ALL_OBJECTS)
CORRECTTONEFILTER_TEST_OBJECTS = correcttonefilter_test.o $(ALL_OBJECTS)
OCTAVESHIFT_TEST_OBJECTS = octaveshift_test.o $(ALL_OBJECTS)
ONEKEYSYNTHESIZERFILTER_TEST_OBJECTS = onekeysynthesizerfilter_test.o $(ALL_OBJECTS)
PARSERFACTORY_TEST_OBJECTS = parserfactory_test.o $(ALL_OBJECTS)
PLAYLISTPARSER_TEST_OBJECTS = playlistparser_test.o $(ALL_OBJECTS)
SMFPARSER_TEST_OBJECTS = smfparser_test.o $(ALL_OBJECTS)
TEXTSCOREPARSER_TEST_OBJECTS = textscoreparser_test.o $(ALL_OBJECTS)
TIMEKEEPER_TEST_OBJECTS = timekeeper_test.o $(ALL_OBJECTS)

%PHONY: default
default: run

%PHONY: all
all: yuruhornsrc_test.exe scorefilter_test.exe scoresrc_test.exe sdsink_test.exe sfzsink_test.exe sfzsink_notename_test.exe sfzparser_test.exe channelfilter_test.exe correcttonefilter_test.exe octaveshift_test.exe onekeysynthesizerfilter_test.exe parserfactory_test.exe playlistparser_test.exe smfparser_test.exe textscoreparser_test.exe timekeeper_test.exe

%PHONY: check
check: all
	./yuruhornsrc_test.exe --gtest_output=xml:yuruhornsrc_test.xml
	./scorefilter_test.exe --gtest_output=xml:scorefilter_test.xml
	./scoresrc_test.exe --gtest_output=xml:scoresrc_test.xml
	./sdsink_test.exe --gtest_output=xml:sdsink_test.xml
	./sfzsink_test.exe --gtest_output=xml:sfzsink_test.xml
	./sfzsink_notename_test.exe --gtest_output=xml:sfzsink_notename_test.xml
	./sfzparser_test.exe --gtest_filter="SFZParser_A_OK.*" --gtest_output=xml:sfzpaser_test.xml
	./channelfilter_test.exe --gtest_output=xml:channelfilter_test.xml
	./correcttonefilter_test.exe --gtest_output=xml:correcttonefilter_test.xml
	./octaveshift_test.exe --gtest_output=xml:octaveshift_test.xml
	./onekeysynthesizerfilter_test.exe --gtest_output=xml:onekeysynthesizerfilter_test.xml
	./parserfactory_test.exe --gtest_output=xml:parserfactory_test.xml
	./playlistparser_test.exe --gtest_output=xml:playlistparser_test.xml
	./smfparser_test.exe --gtest_output=xml:smfparser_test.xml
	./textscoreparser_test.exe --gtest_output=xml:textscoreparser_test.xml
	./timekeeper_test.exe --gtest_output=xml:timekeeper_test.xml

%PHONY: run
run: all
	./yuruhornsrc_test.exe --gtest_output=xml:yuruhornsrc_test.xml || true
	./scorefilter_test.exe --gtest_output=xml:scorefilter_test.xml || true
	./scoresrc_test.exe --gtest_output=xml:scoresrc_test.xml || true
	./sdsink_test.exe --gtest_output=xml:sdsink_test.xml || true
	./sfzsink_test.exe --gtest_output=xml:sfzsink_test.xml || true
	./sfzsink_notename_test.exe --gtest_output=xml:sfzsink_notename_test.xml || true
	./sfzparser_test.exe --gtest_filter="SFZParser_A_OK.*" --gtest_output=xml:sfzpaser_test.xml || true
	./channelfilter_test.exe --gtest_output=xml:channelfilter_test.xml || true
	./correcttonefilter_test.exe --gtest_output=xml:correcttonefilter_test.xml || true
	./octaveshift_test.exe --gtest_output=xml:octaveshift_test.xml || true
	./onekeysynthesizerfilter_test.exe --gtest_output=xml:onekeysynthesizerfilter_test.xml || true
	./parserfactory_test.exe --gtest_output=xml:parserfactory_test.xml || true
	./playlistparser_test.exe --gtest_output=xml:playlistparser_test.xml || true
	./smfparser_test.exe --gtest_output=xml:smfparser_test.xml || true
	./textscoreparser_test.exe --gtest_output=xml:textscoreparser_test.xml || true
	./timekeeper_test.exe --gtest_output=xml:timekeeper_test.xml || true

%PHONY: clean
clean:
	rm -f *.gcno *.gcda *.gcov
	rm -f $(YURUHORNSRC_TEST_OBJECTS)
	rm -f yuruhornsrc_test.exe
	rm -f $(SCOREFILTER_TEST_OBJECTS)
	rm -f scorefilter_test.exe
	rm -f $(SCORESRC_TEST_OBJECTS)
	rm -f scoresrc_test.exe
	rm -f $(SDSINK_TEST_OBJECTS)
	rm -f sdsink_test.exe
	rm -f $(SFZSINK_TEST_OBJECTS)
	rm -f sfzsink_test.exe
	rm -f $(SFZSINK_NOTENAME_TEST_OBJECTS)
	rm -f sfzsink_notename_test.exe
	rm -f $(SFZPARSER_TEST_OBJECTS)
	rm -f sfzparser_test.exe
	rm -f $(CHANNELFILTER_TEST_OBJECTS)
	rm -f channelfilter_test.exe
	rm -f $(CORRECTTONEFILTER_TEST_OBJECTS)
	rm -f correcttonefilter_test.exe
	rm -f $(OCTAVESHIFT_TEST_OBJECTS)
	rm -f octaveshift_test.exe
	rm -f $(ONEKEYSYNTHESIZERFILTER_TEST_OBJECTS)
	rm -f onekeysynthesizerfilter_test.exe
	rm -f $(PARSERFACTORY_TEST_OBJECTS)
	rm -f parserfactory_test.exe
	rm -f $(PLAYLISTPARSER_TEST_OBJECTS)
	rm -f playlistparser_test.exe
	rm -f $(SMFPARSER_TEST_OBJECTS)
	rm -f smfparser_test.exe
	rm -f $(TEXTSCOREPARSER_TEST_OBJECTS)
	rm -f textscoreparser_test.exe
	rm -f $(TIMEKEEPER_TEST_OBJECTS)
	rm -f timekeeper_test.exe

%PHONY: gcov
gcov: run
	gcov $(GCOVFLAGS) yuruhornsrc_test.exe
	gcov $(GCOVFLAGS) scorefilter_test.exe
	gcov $(GCOVFLAGS) scoresrc_test.exe
	gcov $(GCOVFLAGS) sdsink_test.exe
	gcov $(GCOVFLAGS) sfzsink_test.exe --gtest_filter="-SFZSink.outputtest*"
	gcov $(GCOVFLAGS) sfzsink_notename_test.exe
	gcov $(GCOVFLAGS) sfzparser_test.exe --gtest_filter="SFZParser_A_OK.*"
	gcov $(GCOVFLAGS) channelfilter_test.exe
	gcov $(GCOVFLAGS) correcttonefilter_test.exe
	gcov $(GCOVFLAGS) octaveshift_test.exe
	gcov $(GCOVFLAGS) onekeysynthesizerfilter_test.exe
	gcov $(GCOVFLAGS) parserfactory_test.exe
	gcov $(GCOVFLAGS) playlistparser_test.exe
	gcov $(GCOVFLAGS) smfparser_test.exe
	gcov $(GCOVFLAGS) textscoreparser_test.exe
	gcov $(GCOVFLAGS) timekeeper_test.exe
	gcovr --xml-pretty --exclude-unreachable-branches --print-summary -o coverage.xml --root ../YuruHorn

%.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $< -c -o $@

yuruhornsrc_test.exe: $(YURUHORNSRC_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

scorefilter_test.exe: $(SCOREFILTER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

scoresrc_test.exe: $(SCORESRC_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sdsink_test.exe: $(SDSINK_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sfzsink_test.exe: $(SFZSINK_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sfzsink_notename_test.exe: $(SFZSINK_NOTENAME_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

sfzparser_test.exe: $(SFZPARSER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

channelfilter_test.exe: $(CHANNELFILTER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

correcttonefilter_test.exe: $(CORRECTTONEFILTER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

octaveshift_test.exe: $(OCTAVESHIFT_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

onekeysynthesizerfilter_test.exe: $(ONEKEYSYNTHESIZERFILTER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

parserfactory_test.exe: $(PARSERFACTORY_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

playlistparser_test.exe: $(PLAYLISTPARSER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

smfparser_test.exe: $(SMFPARSER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

textscoreparser_test.exe: $(TEXTSCOREPARSER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@

timekeeper_test.exe: $(TIMEKEEPER_TEST_OBJECTS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $^ $(GTESTFLAGS) -o $@
