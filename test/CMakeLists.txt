#
# SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
#
# Copyright 2023 Sony Semiconductor Solutions Corporation
#

cmake_minimum_required(VERSION 3.10)
enable_testing()
include(GoogleTest)

project(ssprocLib CXX)

# library
add_library(stub SHARED stub/stub.cpp)
target_include_directories(stub PUBLIC stub)
target_compile_options(stub PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_compile_features(stub PRIVATE cxx_std_11)
target_link_libraries(stub stdc++)

add_library(ssproc SHARED
    ../src/BaseFilter.cpp
    ../src/ChannelFilter.cpp
    ../src/CorrectToneFilter.cpp
    ../src/midi_util.cpp
    ../src/NullFilter.cpp
    ../src/OctaveShift.cpp
    ../src/OneKeySynthesizerFilter.cpp
    ../src/ParserFactory.cpp
    ../src/path_util.cpp
    ../src/PcmRenderer.cpp
    ../src/PlaylistParser.cpp
    ../src/ScoreFilter.cpp
    ../src/ScoreParser.cpp
    ../src/ScoreSrc.cpp
    ../src/SDSink.cpp
    ../src/SFZParser.cpp
    ../src/SFZSink.cpp
    ../src/SmfParser.cpp
    ../src/TextScoreParser.cpp
    ../src/TimeKeeper.cpp
    ../src/ToneFilter.cpp
    ../src/VoiceCapture.cpp
    ../src/VoiceTriggerSrc.cpp
    ../src/WavReader.cpp
    ../src/YuruhornSrc.cpp
    ../src/YuruInstrumentConfig.cpp
    ../src/YuruInstrumentConsole.cpp
)
target_include_directories(ssproc PUBLIC ../src)
target_compile_definitions(ssproc PUBLIC ARDUINO_ARCH_SPRESENSE=1)
target_compile_options(ssproc PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g --coverage -O3 -Wall -Werror>
)
target_compile_features(ssproc PRIVATE cxx_std_11)
target_link_libraries(ssproc stub stdc++ --coverage)

# tests
add_executable(scoresrc_test scoresrc_test.cpp)
target_compile_options(scoresrc_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(scoresrc_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET scoresrc_test)

add_executable(sdsink_test sdsink_test.cpp)
target_compile_options(sdsink_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(sdsink_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET sdsink_test)

add_executable(sfzparser_test sfzparser_test.cpp)
target_compile_options(sfzparser_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(sfzparser_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET sfzparser_test)

add_executable(sfzsink_test sfzsink_test.cpp)
target_compile_options(sfzsink_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(sfzsink_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET sfzsink_test)

add_executable(sfzsink_notename_test sfzsink_notename_test.cpp)
target_compile_options(sfzsink_notename_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(sfzsink_notename_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET sfzsink_notename_test)

add_executable(yuruhornsrc_test yuruhornsrc_test.cpp)
target_compile_options(yuruhornsrc_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(yuruhornsrc_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET yuruhornsrc_test)

add_executable(scorefilter_test scorefilter_test.cpp)
target_compile_options(scorefilter_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(scorefilter_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET scorefilter_test)

add_executable(correcttonefilter_test correcttonefilter_test.cpp)
target_compile_options(correcttonefilter_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(correcttonefilter_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET correcttonefilter_test)

add_executable(onekeysynthesizerfilter_test onekeysynthesizerfilter_test.cpp)
target_compile_options(onekeysynthesizerfilter_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(onekeysynthesizerfilter_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET onekeysynthesizerfilter_test)

add_executable(timekeeper_test timekeeper_test.cpp)
target_compile_options(timekeeper_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)
target_link_libraries(timekeeper_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET timekeeper_test)

add_executable(parserfactory_test parserfactory_test.cpp)
target_compile_options(parserfactory_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)

target_link_libraries(parserfactory_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET parserfactory_test)

add_executable(textscoreparser_test textscoreparser_test.cpp)
target_compile_options(textscoreparser_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)

target_link_libraries(textscoreparser_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET textscoreparser_test)

add_executable(smfparser_test smfparser_test.cpp)
target_compile_options(smfparser_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)

target_link_libraries(smfparser_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET smfparser_test)

add_executable(playlistparser_test playlistparser_test.cpp)
target_compile_options(playlistparser_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)

target_link_libraries(playlistparser_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET playlistparser_test)

add_executable(channelfilter_test channelfilter_test.cpp)
target_compile_options(channelfilter_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)

target_link_libraries(channelfilter_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET channelfilter_test)

add_executable(octaveshift_test octaveshift_test.cpp)
target_compile_options(octaveshift_test PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-g -O3 -Wall -Werror>
)

target_link_libraries(octaveshift_test ssproc stub stdc++ pthread gtest gtest_main)
gtest_add_tests(TARGET octaveshift_test)
