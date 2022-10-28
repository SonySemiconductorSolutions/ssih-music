/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SCORE_PARSER_H_
#define SCORE_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

#include "BufferedFileReader.h"
#include "YuruInstrumentFilter.h"

static const unsigned kSysExMaxSize = 128;

class ScoreParser {
public:
    struct MidiMessage {
        uint32_t delta_time;  // MIDI用(ScoreSrcでは使用しない)

        uint8_t status_byte;  // SendNoteOn等のMIDIメッセージとチャンネルと各種イベント
        //通常のMIDIイベント
        uint8_t data_byte1;  // MIDIメッセージのデータ部1
        uint8_t data_byte2;  // MIDIメッセージのデータ部2

        uint32_t event_length;  //メタ・SysExイベントの長さ
        //メタイベント専用
        uint8_t event_code;  //メタイベントのイベントコード

        //イベントのデータ部
        uint8_t sysex_array[kSysExMaxSize];
    };

    enum MetaEventCode {
        kSequenceNumber = 0x00,
        kTextEvent = 0x01,
        kCopyright = 0x02,
        kTrackName = 0x03,
        kInstrumentName = 0x04,
        kLyrics = 0x05,
        kMarker = 0x06,
        kCuePoint = 0x07,
        kEndOfTrack = 0x2f,
        kSetTempo = 0x51,
        kMetroNome = 0x52,  // time, rhythm
        kMusicalKeySignature = 0x59,
        kSequencerEvent = 0x7F
    };

    enum MidiMessageType {
        kNoteOff = 0x80,
        kNoteOn = 0x90,
        kPolyphonicKeyPressure = 0xa0,
        kControlChange = 0xb0,
        kProgramChange = 0xc0,
        kChannelPressure = 0xd0,
        kPitchBendChange = 0xe0,
        kSysExEvent = 0xf0,
        kMetaEvent = 0xff
    };

    virtual ~ScoreParser(){};

    virtual uint16_t getRootTick() = 0;
    virtual String getFileName() = 0;
    virtual int getNumberOfScores() = 0;
    virtual bool loadScore(int id) = 0;
    virtual String getTitle(int id) = 0;

    virtual bool getMidiMessage(MidiMessage* midi_message) = 0;
};

#endif  // SCORE_PARSER_H_
