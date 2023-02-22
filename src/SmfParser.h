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
    struct Track {
        int track_id;
        String name;
        uint32_t offset;
        uint32_t size;
    };

    class TrackReader {
    public:
        TrackReader();
        TrackReader(File* file, uint32_t offset, uint32_t size);
        TrackReader(const TrackReader& rhs);
        ~TrackReader();
        size_t available();
        boolean reset();
        int read(void);

    private:
        File* file_;
        uint32_t file_pos_;
        uint32_t track_offset_;
        size_t track_size_;
        uint8_t* buf_;
        size_t buf_size_;
        size_t buf_pos_;
    };

    struct TrackParser {
    public:
        MidiMessage msg;
        TrackParser();
        TrackParser(File* file, size_t offset, size_t size);
        TrackParser(const TrackParser& rhs);
        bool available();
        bool discard();
        bool eot();
        bool parse();

    private:
        bool parseMIDIEvent();
        bool parseMetaEvent();
        TrackReader reader_;
        bool is_registered_;
        uint8_t running_status_;
        bool at_eot_;
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
    std::vector<Track> tracks_;

    // MIDI variables
    bool is_end_;

    std::vector<TrackParser> parsers_;

    bool parse();
};

#endif  // SMF_PARSER_H_
