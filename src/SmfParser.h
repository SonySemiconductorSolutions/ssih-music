/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SMF_PARSER_H_
#define SMF_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

#include "BufferedFileReader.h"
#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

class SmfParser : public ScoreParser {
public:
    struct TrackData {
        int track_id;
        uint32_t start_byte;
        uint32_t data_length;
        bool is_conductor;
        int note_count;
        String name;
    };

    SmfParser(const String& path);
    virtual ~SmfParser();

    bool getMidiMessage(MidiMessage* midi_message) override;

    uint16_t getRootTick() override;
    String getFileName() override;
    int getNumberOfScores() override;
    bool loadScore(int id) override;
    String getTitle(int id) override;

private:
    //楽譜一覧解析
    std::vector<MidiMessage> smf_music_conductor_;
    int conductor_index_;
    uint32_t conductor_delta_time_;
    MidiMessage score_midi_message_;

    uint16_t root_tick_;
    std::vector<TrackData> track_data_;

    // MIDI用の変数
    bool is_smf_score_end_;
    uint8_t running_status_;

    File file_;
    BufferedFileReader reader_;

    bool parseSMF();
    bool parseSMFTrack();

    bool parseMTrkEvent(ScoreParser::MidiMessage* midi_message);

    bool parseMetaEvent(ScoreParser::MidiMessage* midi_message);
    bool parseSMFEvent(ScoreParser::MidiMessage* midi_message, uint8_t status_byte);
    bool parseSMFEventParam(ScoreParser::MidiMessage* midi_message, uint8_t stat, uint8_t param);
    uint32_t parseVariableLength();
};

#endif  // SMF_PARSER_H_
