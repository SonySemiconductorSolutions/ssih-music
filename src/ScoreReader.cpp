/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ScoreReader.h"

#include "YuruInstrumentFilter.h"

//#define DEBUG

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#ifdef DEBUG
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else  // DEBUG
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // DEBUG

const int kDefaultBpm = 120;

int ScoreReader::getFileType(String& file_type) {
    file_type.toLowerCase();
    if (file_type == ".txt") {
        return kScoreFileTypeTxt;
    } else if (file_type == ".mid" || file_type == ".midi") {
        return kScoreFileTypeMidi;
    } else {
        return kScoreFileTypeOthers;
    }
}

uint32_t ScoreReader::calcDeltaTime(uint32_t root_tick, uint32_t delta_time, uint32_t tempo) {
    return ((delta_time * tempo) / root_tick) / 1000;  // マイクロ秒 -> ミリ秒変換
}
#if 0
uint32_t swap32(uint32_t v) {
    return (((v >> 0) & 0xFF) << 24) | (((v >> 8) & 0xFF) << 16) |
           (((v >> 16) & 0xFF) << 8) | (((v >> 24) & 0xFF) << 0);
}

uint16_t swap16(uint16_t v) {
    return (((v >> 0) & 0xFF) << 8) | (((v >> 8) & 0xFF) << 0);
}

uint8_t getUInt8(const unsigned char* buf) {
    return (*buf);
}

uint16_t getUInt16(const unsigned char* buf) {
    uint16_t ret = 0;
    for (int i = 0; i < 2; i++) {
        ret = ret << sizeof(uint8_t) * 8;
        ret |= getUInt8(&buf[i]);
    }
    return ret;
}

uint32_t getUInt32(const unsigned char* buf) {
    uint32_t ret = 0;
    for (int i = 0; i < 2; i++) {
        ret = ret << sizeof(uint16_t) * 8;
        ret |= getUInt16(&buf[i * 2]);
    }
    return ret;
}

int8_t getInt8(const unsigned char* buf) {
    return (int8_t)(*buf);
}

int16_t getInt16(const unsigned char* buf) {
    int16_t ret = 0;
    for (int i = 0; i < 2; i++) {
        ret = ret << sizeof(uint8_t) * 8;
        ret |= getInt8(&buf[i]);
    }
    return ret;
}

int32_t getInt32(const unsigned char* buf) {
    int32_t ret = 0;
    for (int i = 0; i < 2; i++) {
        ret = ret << sizeof(uint16_t) * 8;
        ret |= getInt16(&buf[i * 2]);
    }
    return ret;
}
#endif

uint32_t ScoreReader::parseVariableLength(File& file) {
    uint32_t length = 0;
    unsigned char c = (unsigned char)file.read();

    length = c;
    if (length & 0x80) {
        length &= 0x7F;
        do {
            if (!file.available()) {
                error_printf("ScoreReader: %s() Error:End Of file.", __func__);
                break;
            }
            c = (unsigned char)file.read();
            // 7ビットシフト
            length = length << 7;
            //取得バイトの下位7ビット取得
            //下位7ビットの結合
            length += (c & 0x7F);
        } while (c & 0x80);
    } else {
        trace_printf("ScoreReader: parseLength Finish.\n");
    }
    return length;
}

ScoreReader::MusicConductor::MusicConductor() : file_name("") {
}

ScoreReader::Note::Note() : delta_time(0), note_num(0), velocity(0), ch(0) {
}

ScoreReader::ScoreData::ScoreData() : file_name(""), track_num(0), title(""), bpm(kDefaultBpm), tone(0), rhythm(0), start(0), end(0), start_byte(0) {
}

ScoreReader::ScoreReader() : ScoreReader("SCORE") {
}

ScoreReader::ScoreReader(const String& dir_name)
    : parsing_score_(ScoreData()),
      prev_note_num_(INVALID_NOTE_NUMBER),
      index_of_measure_(0),
      score_file_type_(kScoreFileTypeOthers),
      tick_time_(0),
      is_midi_score_end_(false),
      running_status_(0) {
    readDirectoryScores(dir_name);
}

//デバッグ用
#if 0
static void printNote(const ScoreReader::Note& note) {
    debug_printf("ScoreReader: Delta time:%lu   ", note.delta_time);
    debug_printf("ScoreReader: note num  :%5d   ", note.note_num);
    debug_printf("ScoreReader: velocity  :%3d\n", note.velocity);
}

static void printScoresData(const std::vector<ScoreReader::ScoreData>& scores) {
    for (const auto& data : scores) {
        debug_printf("ScoreReader: File Name    :%s\n", data.file_name.c_str());
        debug_printf("ScoreReader: TRUCK NUM    :%d\n", data.track_num);
        debug_printf("ScoreReader: Title        :%s\n", data.title.c_str());
        debug_printf("ScoreReader: BPM          :%d\n", data.bpm);
        debug_printf("ScoreReader: Tone         :%d\n", data.tone);
        debug_printf("ScoreReader: Rhythm       :%d\n", data.rhythm);
        debug_printf("ScoreReader: Score Start  :%d\n", data.start);
        debug_printf("ScoreReader: Score End    :%d\n", data.end);
        debug_printf("ScoreReader: Score Start B:%d\n", data.start_byte);
    }
}
#endif

//楽譜一覧解析
bool ScoreReader::readDirectoryScores(const String& dir_name) {
    //ファイルオープン
    if (!sd_.begin()) {
        error_printf("ScoreReader: SD: %s begin error.\n", dir_name.c_str());
    }
    File dir = sd_.open(dir_name.c_str());
    if (!dir) {  //ファイル読み込み(失敗)
        error_printf("ScoreReader: SD: %s open error.\n", dir_name.c_str());
        return false;
    } else {  //ファイル読み込み(成功)
        debug_printf("ScoreReader: SD: %s open success. ", dir_name.c_str());
    }

    if (dir.isDirectory()) {  // 対象がディレクトリの場合
        debug_printf("(Directory)\n");
        while (true) {
            File file = dir.openNextFile();
            file.setTimeout(0);
            //ファイルの読み込み確認
            if (!file) {
                break;
            }
            readScore(file);
            file.close();
        }
    } else {  //対象がファイルの場合
        debug_printf("(file)\n");
        dir.setTimeout(0);
        readScore(dir);
        dir.close();
    }

    // printScoresData(scores_);
    return true;
}

bool ScoreReader::readScore(File& file) {
    debug_printf("ScoreReader: SD: file:%s open success.\n", file.name());
    String file_name = file.name();
    String file_type = file_name;
    int dot_position = file_type.lastIndexOf(".");
    if (dot_position < 0) {
        error_printf("ScoreReader: SD: file:%s This file is not supported.\n", file.name());
        return false;
    } else {
        file_type = file_type.substring(dot_position);
    }

    debug_printf("ScoreReader: SD: file type:%s\n", file_type.c_str());

    int read_byte = 0;
    int track_num = 1;
    if (getFileType(file_type) == kScoreFileTypeTxt) {
        int score_line = 1;

        //楽譜の解析開始
        while (file.available()) {
            parseLine(file.name(), file.readStringUntil('\n'), score_line, &read_byte, &track_num);  //改行で判定
            score_line++;
        }
    } else if (getFileType(file_type) == kScoreFileTypeMidi) {
        if (parseMIDI(file, &read_byte, &track_num)) {
            trace_printf("ScoreReader: MIDI parse success.\n");
        } else {
            trace_printf("ScoreReader: MIDI parse failed.\n");
        }
    } else {
        error_printf("ScoreReader: SD: file:%s This file is not supported.\n", file.name());
    }

    return true;
}

// --------------------------------- My Score ---------------------------------
bool ScoreReader::parseLine(const String& file_name, const String& buf, int score_line, int* read_byte, int* track_num) {
    String line = buf;
    (*read_byte) += buf.length() + 1;  //改行コード分を含める
    trace_printf("ScoreReader: L%d str_len:%d  str:%s\n", score_line, buf.length(), buf.c_str());
    line.trim();

    if (line.length() == 0) {  //改行のみの場合はその行を無視
        return true;
    }
    trace_printf("ScoreReader: %s    :%d\n", line.c_str(), line.length());
    if (line.startsWith("//")) {  //コメントを無視
        return true;
    }
    trace_printf("ScoreReader: L%d : %s\n", score_line, buf.c_str());

    if (line.startsWith("#MUSIC")) {
        parseHeaderCommand(file_name, line, score_line, read_byte, track_num);
        return true;
    }
    return true;
}

void ScoreReader::parseHeaderCommand(const String& file_name, const String& command, int score_line, int* read_byte, int* track_num) {
    trace_printf("ScoreReader: ScoreReader::COMMAND:%s -> ", command.c_str());
    if (command.startsWith("#MUSIC_TITLE:")) {
        trace_printf("ScoreReader:  BPMCHANGE:%s %d\n", command.substring(command.indexOf(":") + 1).toInt());
        parsing_score_.title = command.substring(command.indexOf(":") + 1);
    } else if (command.startsWith("#MUSIC_BPM:")) {
        trace_printf("ScoreReader:  BPMCHANGE:%s %d\n", command.substring(command.indexOf(":") + 1).toInt());
        parsing_score_.bpm = command.substring(command.indexOf(":") + 1).toInt();
    } else if (command.startsWith("#MUSIC_TONE:")) {
        trace_printf("ScoreReader:  TONECHANGE:%d\n", command.substring(command.indexOf(":") + 1).toInt());
        parsing_score_.tone = command.substring(command.indexOf(":") + 1).toInt();
    } else if (command.equals("#MUSIC_START")) {
        trace_printf("ScoreReader:  Score START\n");
        parsing_score_.start = score_line;
        parsing_score_.start_byte = (*read_byte);
    } else if (command.equals("#MUSIC_END")) {
        trace_printf("ScoreReader:  Score END\n");
        parsing_score_.end = score_line;
        parsing_score_.file_name = file_name;
        parsing_score_.track_num = (*track_num);
        scores_.push_back(parsing_score_);
        (*track_num)++;
        //初期化
        parsing_score_ = ScoreData();
    }
}

// -------------------------------- MIDI Score --------------------------------

bool ScoreReader::parseMIDI(File& file, int* read_byte, int* track_num) {
    struct ScoreReader::MidiHeader h;
    if (!parseMIDIHeader(file, &h, read_byte)) {
        return false;
    }
    int first_track_byte = file.position();

    trace_printf("ScoreReader: chunk:%s, length:%d, format:%d, truck:%d, time:%d\n", h.chunk, h.header_len, h.format, h.truck_num, h.time);
    for (int i = 0; i < h.truck_num; i++) {
        parsing_score_ = ScoreData();
        parseMIDITrack(file, read_byte, track_num);
        (*track_num)++;
        parsing_score_.bpm = h.time;
        scores_.push_back(parsing_score_);
    }

    if (1 < h.truck_num) {
        file.seek(first_track_byte);
        parseConductorTrack(file);
    }

#ifdef DEBUG
    printScoresData(scores_);
    for (size_t i = 0; i < midis_music_conductor_.size(); i++) {
        debug_printf("ScoreReader: MIDI file name:%s\n", midis_music_conductor_[i].file_name.c_str());
        debug_printf("ScoreReader: This MIDI conduct num:%d\n", (int)midis_music_conductor_[i].special_notes.size());
        for (size_t j = 0; j < midis_music_conductor_[i].special_notes.size(); j++) {
            debug_printf("ScoreReader: Event code:%d ", midis_music_conductor_[i].special_notes[j].note_num);
            debug_printf("ScoreReader: Delta time:%d ", midis_music_conductor_[i].special_notes[j].delta_time);
            debug_printf("ScoreReader: Parameter1:%d ", midis_music_conductor_[i].special_notes[j].velocity);
            debug_printf("ScoreReader: Parameter2:%d\n", midis_music_conductor_[i].special_notes[j].ch);
        }
    }
#endif

    return true;
}

bool ScoreReader::parseMIDIHeader(File& file, ScoreReader::MidiHeader* header, int* read_byte) {
    // file.read(header, sizeof(*header));
    char chunk[kMidiChunkSize + 1];
    for (unsigned int i = 0; i < kMidiChunkSize; i++) {
        chunk[i] = file.read();
    }
    chunk[kMidiChunkSize] = '\0';
    strncpy(header->chunk, chunk, kMidiChunkSize);
    if (strncmp(header->chunk, "MThd", kMidiChunkSize) != 0) {
        return false;
    }

    for (unsigned int i = 0; i < sizeof(header->header_len); i++) {
        header->header_len = header->header_len << 8;
        header->header_len += file.read();
    }
    for (unsigned int i = 0; i < sizeof(header->format); i++) {
        header->format = header->format << 8;
        header->format += file.read();
    }
    for (unsigned int i = 0; i < sizeof(header->truck_num); i++) {
        header->truck_num = header->truck_num << 8;
        header->truck_num += file.read();
    }
    for (unsigned int i = 0; i < sizeof(header->time); i++) {
        header->time = header->time << 8;
        header->time += file.read();
    }
    if ((header->time >> 8) & 0x80) {
        error_printf("ScoreReader: This MIDI file is not supported.\n");
        return false;
    }
    tick_time_ = header->time;
    (*read_byte) = file.position();

    return true;
}

bool ScoreReader::parseMIDITrack(File& file, int* read_byte, int* track_num) {
    //トラックヘッダの取得
    (*read_byte) += kMidiChunkSize;

    char chunk[kMidiChunkSize + 1];
    for (unsigned int i = 0; i < kMidiChunkSize; i++) {
        chunk[i] = file.read();
    }
    chunk[kMidiChunkSize] = '\0';
    if (strncmp(chunk, "MTrk", kMidiChunkSize) != 0) {
        return false;
    }

    //トラックデータ長の取得
    uint32_t track_size = 0;
    for (unsigned int i = 0; i < sizeof(track_size); i++) {
        track_size = track_size << 8;
        track_size += file.read();
    }
    (*read_byte) += sizeof(track_size);

#ifdef DEBUG
    debug_printf("ScoreReader: This Track size:%d\n", track_size);
    trace_printf("ScoreReader: deta:");
    for (unsigned int i = 0; i < track_size; i++) {
        unsigned char track_data = (unsigned char)(file.read());
        trace_printf("ScoreReader: %02x ", track_data);
        (void)track_data;
    }
    trace_printf("ScoreReader: \n");
#else
    file.seek(file.position() + track_size);
#endif
    String file_name = file.name();
    parsing_score_.file_name = file_name;

    int slash_position = file_name.lastIndexOf("/");
    int dot_position = file_name.lastIndexOf(".");

    String score_name = "";
    if (dot_position < 0) {
        error_printf("ScoreReader: SD: file:%s This file is not supported.\n", file.name());
    } else {
        score_name = file_name.substring(slash_position + 1, dot_position);
        score_name = score_name + "_T" + (*track_num);
    }

    parsing_score_.title = score_name;

    parsing_score_.start_byte = (*read_byte);
    parsing_score_.track_num = (*track_num);

    (*read_byte) += track_size;

    return true;
}

bool ScoreReader::parseConductorTrack(File& file) {
    ScoreReader::Note special_note = Note();
    ScoreReader::MusicConductor music_conductor = MusicConductor();
    music_conductor.file_name = file.name();
    uint32_t music_time = 0;

    bool is_first_tempo = true;           // 基礎となる時間情報であるかの確認
    bool is_pure_conductor_track = true;  // MIDIノートがないトラックの確認

    //コンダクタートラックのヘッダー確認
    char chunk[kMidiChunkSize + 1];
    for (unsigned int i = 0; i < kMidiChunkSize; i++) {
        chunk[i] = file.read();
    }
    chunk[kMidiChunkSize] = '\0';
    if (strncmp(chunk, "MTrk", kMidiChunkSize) != 0) {
        return false;
    }

    uint32_t track_len = 0;
    for (unsigned int i = 0; i < sizeof(track_len); i++) {
        track_len = track_len << 8;
        track_len += file.read();
    }

    while (true) {
        special_note = Note();
        parseMTrkEvent(file, special_note);
        music_time += special_note.delta_time;
        trace_printf("ScoreReader: (Conductor) delta time:%u, note num:%d param:%d\n", (unsigned int)special_note.delta_time, special_note.note_num,
                     special_note.velocity);
        if (special_note.note_num == kCommandScoreEnd) {
            if (is_pure_conductor_track) {
                trace_printf("ScoreReader: This Track 1 is pure conductor Track.\n");
                for (unsigned int i = 0; i < scores_.size(); i++) {
                    if (scores_[i].file_name.equals(file.name()) && scores_[i].track_num == 1) {
                        auto conductor_track_itr = scores_.begin() + i;
                        scores_.erase(conductor_track_itr);
                        break;
                    }
                }
            }
            break;
        } else if (special_note.note_num == kCommandRhythmChange) {
            special_note.delta_time = music_time;
            if (is_first_tempo) {
                trace_printf("ScoreReader: Tempo:%d\n", special_note.velocity);
                for (unsigned int i = 0; i < scores_.size(); i++) {
                    if (scores_[i].file_name.equals(file.name())) {
                        scores_[i].rhythm = special_note.velocity;  // BPMに時間情報を格納する
                    }
                }
                is_first_tempo = false;
            } else if (NOTE_NUMBER_MIN <= special_note.note_num && special_note.note_num <= NOTE_NUMBER_MAX) {
                is_pure_conductor_track = false;
            } else {
                music_conductor.special_notes.push_back(special_note);
            }
        }
        music_time = 0;
    }
    midis_music_conductor_.push_back(music_conductor);

    return true;
}

bool ScoreReader::parseMTrkEvent(File& file, ScoreReader::Note& note) {
    note.delta_time = parseVariableLength(file);

    //ステータスバイトの取得
    unsigned char status_byte = (unsigned char)(file.read());
    trace_printf("ScoreReader: status byte:%02x\n", status_byte);

    uint32_t deta_len = 0;
    if (status_byte == 0xFF) {  // Meta Event
        parseMetaEvent(file, note);
    } else if (status_byte == 0xF0 || status_byte == 0xF7) {  // SysEx Event
        deta_len = parseVariableLength(file);
        file.seek(file.position() + deta_len);
    } else {  // MIDI Event
        parseMIDIEvent(file, note, status_byte);
    }

    return true;
}

bool ScoreReader::parseMetaEvent(File& file, ScoreReader::Note& note) {
    uint32_t deta_len = 0;

    trace_printf("ScoreReader: (Meta Event) Dt:%d ", note.delta_time);
    // metaイベントコードバイトの取得
    unsigned char event_code = (unsigned char)file.read();
    trace_printf("ScoreReader: event_code:%02x ", event_code);

    deta_len = parseVariableLength(file);
    trace_printf("ScoreReader: deta_len:%u ", (unsigned int)deta_len);

    if (event_code == 0x51) {
        note.note_num = kCommandRhythmChange;
        uint32_t tempo = 0;
        for (unsigned int i = 0; i < deta_len; i++) {
            tempo = tempo << 8;
            tempo += file.read();
        }
        trace_printf("ScoreReader: Set tempo:%d\n", tempo);
        note.velocity = tempo;
    } else if (event_code == 0x2F) {
        note.note_num = kCommandScoreEnd;
        trace_printf("ScoreReader: (Score end)\n");
    } else if (event_code < 0x80) {
        file.seek(file.position() + deta_len);
        note.note_num = INVALID_NOTE_NUMBER;
        trace_printf("ScoreReader: \n");
    } else {
        return false;
    }
    return true;
}

bool ScoreReader::parseMIDIEvent(File& file, ScoreReader::Note& note, unsigned char status_byte) {
    if (0x80 <= status_byte) {
        debug_printf("ScoreReader: %02x (MIDI) ", status_byte);
        unsigned char stat = status_byte;
        unsigned char param = 0xff;
        parseMIDIEventParam(file, note, stat, param);
        running_status_ = stat;
    } else {
        debug_printf("ScoreReader: %02x (MIDI(rs(%02x))) ", status_byte, running_status_);
        unsigned char param = status_byte;
        parseMIDIEventParam(file, note, running_status_, param);
    }
    return true;
}

bool ScoreReader::parseMIDIEventParam(File& file, ScoreReader::Note& note, unsigned char stat, unsigned char param) {
    if ((0x90 <= stat && stat <= 0x9f)) {  // Note On event
        if (param == 0xff) {
            note.note_num = (uint8_t)(file.read());
            note.velocity = (uint8_t)(file.read());
            debug_printf("ScoreReader: d:%02x %02x\n", note.note_num, note.velocity);
        } else {
            note.note_num = (uint8_t)param;
            note.velocity = (uint8_t)(file.read());
            debug_printf("ScoreReader: d:%02x %02x\n", note.note_num, note.velocity);
        }
        note.ch = stat & 0x0f;
    } else if ((0x80 <= stat && stat <= 0x8f)) {  // Note Off event.
        if (param == 0xff) {
            note.note_num = (uint8_t)file.read();
            note.velocity = (uint8_t)file.read();
            debug_printf("ScoreReader: d:%02x %02x(0x00)\n", note.note_num, note.velocity);
            note.velocity = 0;
        } else {
            note.note_num = (uint8_t)param;
            note.velocity = (uint8_t)file.read();
            debug_printf("ScoreReader: d:%02x %02x(0x00)\n", note.note_num, note.velocity);
            note.velocity = 0;
        }
        note.ch = stat & 0x0f;
    } else if ((0xc0 <= stat && stat <= 0xcf) ||  // Program Change
               (0xd0 <= stat && stat <= 0xdf)) {  // Channel Pressure
        if (param == 0xff) {
#if defined(DEBUG)
            unsigned char c = file.read();
            debug_printf("ScoreReader: d:%02x\n", c);
#else
            file.read();
#endif
        }
        note.note_num = INVALID_NOTE_NUMBER;
    } else {  // Control Change & Polyphonic Key Pressure & Pitch Bend Change
        if (param == 0xff) {
#if defined(DEBUG)
            unsigned char c1 = file.read();
            unsigned char c2 = file.read();
            debug_printf("ScoreReader: d:%02x %02x\n", c1, c2);
#else
            file.read();
            file.read();
#endif
        } else {
#if defined(DEBUG)
            unsigned char c = file.read();
            debug_printf("ScoreReader: d:%02x\n", c);
#else
            file.read();
#endif
        }
        note.note_num = INVALID_NOTE_NUMBER;
    }

    return true;
}

// -------------------------------- MIDI Score --------------------------------
const std::vector<ScoreReader::ScoreData>& ScoreReader::getScoresData() {
    return scores_;
}

//楽譜解析
bool ScoreReader::load(const ScoreData& score_data) {
    if (score_file_) {
        score_file_.close();
        index_of_measure_ = 0;
        measure_.clear();
    }
    // Arduino内でフルパス指定されるときに付与される文字列
    String SD_FULLPATH = "/mnt/sd0/";
    //ファイル名をフルパス->カレントパスに変換する
    String file_name_current;
    if (score_data.file_name.startsWith(SD_FULLPATH)) {
        file_name_current = score_data.file_name.substring(SD_FULLPATH.length());
    } else {
        file_name_current = score_data.file_name;
    }
    score_file_ = sd_.open(file_name_current.c_str());
    score_file_.setTimeout(0);

    if (!score_file_) {  //ファイル読み込み(失敗)
        error_printf("ScoreReader: SD: Score File:%s open error.\n\n", file_name_current.c_str());
        return false;
    } else {  //ファイル読み込み(成功)
        debug_printf("ScoreReader: SD: Score File:%s open success.\n\n", file_name_current.c_str());
    }

    String file_name = score_file_.name();
    String file_type = file_name;
    int dot_position = file_type.lastIndexOf(".");
    if (dot_position < 0) {
        error_printf("ScoreReader: SD: file:%s This file is not supported.\n", file_name.c_str());
        return false;
    } else {
        file_type = file_type.substring(dot_position);
    }

    if (getFileType(file_type) == kScoreFileTypeTxt) {
        score_file_type_ = kScoreFileTypeTxt;
        trace_printf("ScoreReader: this file is txt.\n");
    } else if (getFileType(file_type) == kScoreFileTypeMidi) {
        score_file_type_ = kScoreFileTypeMidi;
        is_midi_score_end_ = false;
        trace_printf("ScoreReader: this file is MIDI.\n");
    } else {
        score_file_type_ = kScoreFileTypeOthers;
        error_printf("ScoreReader: This file is not supported.\n\n");
        return false;
    }

    //シーク処理
    score_file_.seek(score_data.start_byte);
    trace_printf("ScoreReader: %s\n", score_file_.readStringUntil('\n').c_str());
    //ノートスタックの初期化
    measure_.clear();
    return true;
}

const ScoreReader::Note& ScoreReader::getNote() {
    if (score_file_type_ == kScoreFileTypeMidi) {
        return getNoteMIDI();
    } else {
        return getNoteText();
    }
}

const ScoreReader::Note& ScoreReader::getNoteMIDI() {
    measure_.clear();
    measure_.push_back(Note());
    if (is_midi_score_end_) {
        measure_[0].note_num = kCommandScoreEnd;
    } else {
        while (true) {
            parseMTrkEvent(score_file_, measure_[0]);
            if (NOTE_NUMBER_MIN <= measure_[0].note_num && measure_[0].note_num <= NOTE_NUMBER_MAX) {
                break;
            }
            if (measure_[0].note_num == kCommandScoreEnd) {
                is_midi_score_end_ = true;
                break;
            }
        }
    }
    return measure_[0];
}
//ノート取得
const ScoreReader::Note& ScoreReader::getNoteText() {
    trace_printf("ScoreReader: %d\n", measure_.size());
    if (measure_.empty()) {  //初期値
        trace_printf("ScoreReader: measure_ is empty\n");
        while (!parseScore())
            ;
        index_of_measure_ = 0;
    } else if (measure_[0].note_num == kCommandScoreEnd) {
        return measure_[0];
    } else if (measure_.size() <= index_of_measure_ + 1) {
        trace_printf("ScoreReader: measure_ stack is empty\n");
        measure_.clear();
        while (!parseScore())
            ;
        index_of_measure_ = 0;
    } else {
        index_of_measure_++;
    }

    return measure_[index_of_measure_];
}

bool ScoreReader::parseScore() {
    String buf = "";
    if (score_file_.available()) {
        buf = score_file_.readStringUntil('\n');
    } else {
        parseScoreCommand("#MUSIC_END");
        return true;
    }
    trace_printf("ScoreReader: %s\n", buf.c_str());
    if (buf.length() == 0) {  //改行のみの行を無視
        return false;
    }
    trace_printf("ScoreReader: %s    :%d\n", line.c_str(), line.length());
    if (buf.startsWith("//")) {  //コメントを無視
        return false;
    }

    if (buf.startsWith("#")) {  //ヘッダーコマンド
        parseScoreCommand(buf);
        return true;
    }

    parseMeasure(buf);  //小節
    return true;
}

//　楽譜内コマンドの解析
void ScoreReader::parseScoreCommand(const String& command) {
    ScoreReader::Note note = Note();
    if (command.startsWith("#BPMCHANGE ")) {
        note.note_num = kCommandBpmChange;
        int new_bpm = command.substring(command.indexOf(" ")).toInt();
        if (!(new_bpm > 0)) {  // BPMが0以下の場合はエラーとして格納する
            new_bpm = -1;
        }
        note.velocity = new_bpm;
    } else if (command.startsWith("#TONECHANGE ")) {
        note.note_num = kCommandToneChange;
        note.velocity = command.substring(command.indexOf(" ")).toInt();
    } else if (command.startsWith("#RHYTHMCHANGE ")) {
        note.note_num = kCommandRhythmChange;
        note.velocity = command.substring(command.indexOf(" ")).toInt();
    } else if (command.startsWith("#DELAY ")) {
        note.note_num = kCommandDelay;
        note.velocity = command.substring(command.indexOf(" ")).toInt();
    } else if (command.startsWith("#MUSIC_END")) {
        note.note_num = kCommandScoreEnd;
    } else {
        note.note_num = kCommandError;
    }
    measure_.push_back(note);
}

void ScoreReader::parseMeasure(const String& data) {
    ScoreReader::Note note = Note();

    String note_str = "";

    for (unsigned int i = 0; i < data.length(); i++) {
        char c = data[i];

        if (c == ',' || c == ';') {         // ノートの区切り
            if (!note_str.length() == 0) {  //前回のノートが続く
                note.note_num = note_str.toInt();
                prev_note_num_ = note.note_num;
            } else {  //新たなノート
                note.note_num = prev_note_num_;
            }
            note.velocity = DEFAULT_VELOCITY;
            note_str = "";
            note.delta_time = 0;
            measure_.push_back(note);

            if (c == ';') {
                break;
            }
        } else if (c == '/') {  // コメント
            break;
        } else if (isDigit(c) || c == '-') {  // 数字or'-'
            note_str += c;
        }
    }
}

bool ScoreReader::isMidiFile() {
    if (score_file_type_ == kScoreFileTypeMidi) {
        return true;
    } else {
        return false;
    }
}
// class ScoreReader

#endif  // ARDUINO_ARCH_SPRESENSE
