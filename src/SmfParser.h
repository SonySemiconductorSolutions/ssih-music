/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SMF_PARSER_H_
#define SMF_PARSER_H_

#include <vector>

#include <Arduino.h>

#include <File.h>

#include "ScoreParser.h"
#include "YuruInstrumentFilter.h"

class SmfParser : public ScoreParser {
public:
    struct MTrkSegment {
        int track_id;
        uint32_t offset;
        uint32_t size;
        String name;
    };

    class SegmentReader {
    public:
        SegmentReader(File* file, uint32_t offset, uint32_t size);
        ~SegmentReader();
        size_t available();
        boolean reset();
        int read(void);

    private:
        File* file_;
        uint32_t file_pos_;
        uint32_t seg_offset_;
        size_t seg_size_;
        uint8_t* buf_;
        size_t buf_size_;
        size_t buf_pos_;
    };

    // Have a track end flag because it does not work properly when managed collectively by stat.
    struct TrackReader {
        MidiMessage msg;
        SegmentReader* reader;
        bool is_registered;
        uint8_t running_status;
        bool at_eot;
    };

    SmfParser(const String& path);
    virtual ~SmfParser();

    uint16_t getRootTick() override;
    String getFileName() override;
    int getNumberOfScores() override;
    bool loadScore(int id) override;
    String getTitle(int id) override;

    bool getMidiMessage(MidiMessage* midi_message) override;

private:
    // SMF tracks
    File file_;
    uint16_t root_tick_;
    std::vector<MTrkSegment> mtrks_;

    // MIDI variables
    bool is_end_;

    std::vector<TrackReader> readers_;

    bool parseSMF();
    bool parseMTrk();
    bool parseMTrkEvent(TrackReader& track, ScoreParser::MidiMessage* midi_message);
    bool parseMIDIEvent(TrackReader& track, ScoreParser::MidiMessage* midi_message, uint8_t status_byte);
    bool parseMetaEvent(TrackReader& track, ScoreParser::MidiMessage* midi_message);
};

#endif  // SMF_PARSER_H_
