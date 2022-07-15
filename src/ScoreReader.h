/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SCORE_READER_H_
#define SCORE_READER_H_

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

#include "YuruInstrumentFilter.h"

const unsigned int kMidiChunkSize = 4;

class ScoreReader {
public:
    //ネームスペース(仮置き)
    enum Command {
        kCommandNumMin = 200,
        kCommandRhythmChange,
        kCommandToneChange,
        kCommandBpmChange,
        kCommandDelay,
        kCommandScoreEnd,  //楽譜の終端を示す
        kCommandError,
        kCommandNumMax,
    };

    enum Rhythm {
        kRhythmNormal = 0,
        kRhythm8Note,
        kRhythm4Note,
        kRhythm1NoteTriplets,
        kRhythm2NoteTriplets,
        kRhythm4NoteTriplets,
        kRhythm8NoteTriplets,
    };

    enum ScoreFileType {
        kScoreFileTypeTxt = 0,
        kScoreFileTypeMidi,
        kScoreFileTypeOthers,
    };

    struct Note {
        Note();
        uint32_t delta_time;  // MIDI用(ScoreSrcでは使用しない)
        uint8_t note_num;     // ノート番号(0~127) 特殊ノート(201~206, 255)
        int velocity;         // MIDI用(ScoreSrcでは固定値とする)
        int ch;               // MIDI用(ScoreSrcでは固定値とする)
    };

    struct ScoreData {
        ScoreData();
        String file_name;
        int track_num;  //ファイル内の何トラック目かを示す
        String title;
        int bpm;
        int tone;
        int rhythm;
        int start;       //楽譜開始位置
        int end;         //楽譜終了位置
        int start_byte;  //楽譜開始位置(バイト)
    };

    struct MidiHeader {
        char chunk[kMidiChunkSize];
        uint32_t header_len;
        uint16_t format;     // 2Byte
        uint16_t truck_num;  // 2Byte
        uint16_t time;       // 2Byte
    };

    struct MusicConductor {
        MusicConductor();
        String file_name;
        std::vector<ScoreReader::Note> special_notes;
    };

    ScoreReader();
    ScoreReader(const String& dir_name);

    //楽譜一覧解析
    const std::vector<ScoreReader::ScoreData>& getScoresData();

    //楽譜解析
    bool load(const ScoreData& score_data);
    const ScoreReader::Note& getNote();

    uint32_t calcDeltaTime(uint32_t root_tick, uint32_t delta_time, uint32_t tempo);
    int getFileType(String& file_type);
    bool isMidiFile();

private:
    SDClass sd_;

    //楽譜一覧解析
    std::vector<ScoreReader::MusicConductor> midis_music_conductor_;

    //楽譜一覧解析
    std::vector<ScoreReader::ScoreData> scores_;
    ScoreReader::ScoreData parsing_score_;

    int prev_note_num_;

    //楽譜解析
    std::vector<ScoreReader::Note> measure_;
    unsigned int index_of_measure_;
    File score_file_;

    // MIDI用の変数
    ScoreFileType score_file_type_;
    unsigned int tick_time_;
    bool is_midi_score_end_;
    unsigned char running_status_;

    //楽譜一覧解析
    bool readDirectoryScores(const String& dir_name);
    bool readScore(File& file);
    bool parseLine(const String& file_name, const String& buf, int score_line, int* read_byte, int* track_num);
    void parseHeaderCommand(const String& file_name, const String& command, int score_line, int* read_byte, int* track_num);

    const ScoreReader::Note& getNoteText();
    const ScoreReader::Note& getNoteMIDI();
    //楽譜解析
    bool parseScore();
    void parseScoreCommand(const String& command);
    void parseMeasure(const String& data);

    bool parseMIDI(File& file, int* read_byte, int* track_num);
    bool parseMIDIHeader(File& file, ScoreReader::MidiHeader* header, int* read_byte);
    bool parseMIDITrack(File& file, int* read_byte, int* track_num);

    bool parseConductorTrack(File& file);

    bool parseMTrkEvent(File& file, ScoreReader::Note& note);

    bool parseMetaEvent(File& file, ScoreReader::Note& note);
    bool parseMIDIEvent(File& file, ScoreReader::Note& note, unsigned char status_byte);
    bool parseMIDIEventParam(File& file, ScoreReader::Note& note, unsigned char stat, unsigned char param);
    uint32_t parseVariableLength(File& file);
};

// class ScoreReader
#endif  // SCORE_READER_H_
