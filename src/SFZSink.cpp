/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "SFZSink.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>

#include <vector>

#include <Arduino.h>

#include <SDHCI.h>

// #define DEBUG (1)

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#if defined(DEBUG)
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // if defined(DEBUG)

struct OpcodeSpec {
    const char* opcode_str;
    SFZSink::Opcode opcode_enum;
    uint32_t min;
    uint32_t max;
    bool (*parser)(const String&, uint32_t*);
};

// playback parameters
// NOTICE: PcmRenderer only supports 48kHz/16bit/2ch. Otherwise, it will not work.
const int kPbSampleFrq = 48000;
const int kPbBitDepth = 16;
const int kPbChannelCount = 2;
const int kPbSampleCount = 240;
const int kPbBlockSize = kPbSampleCount * (kPbBitDepth / 8) * kPbChannelCount;
const int kPbCacheSize = (24 * 1024);

// cache parameter
const int kPreLoadFrameNum = 3;
const int kLoadFrameNum = 3;

static void writeToRenderer(PcmRenderer& renderer, int ch, File& file, size_t end) {
    trace_printf("enter: %s(%d, %s, %d, %d)\n", __func__, ch, file.name(), (int)file.position(), (int)end);
    if (renderer.getWritableSize(ch) < kPbBlockSize) {
        return;
    }
    size_t pos = file.position();
    if (pos >= end) {
        return;
    }
    size_t read_size = end - pos;
    read_size = (read_size < kPbBlockSize) ? read_size : kPbBlockSize;
    uint8_t buffer[read_size];
    file.read(buffer, read_size);
    renderer.write(ch, buffer, read_size);
}

static SFZSink::SfzHeader convertHeaderToNum(const String& str) {
    if (str == "global") {
        return SFZSink::kGlobal;
    } else if (str == "group") {
        return SFZSink::kGroup;
    } else if (str == "control") {
        return SFZSink::kControl;
    } else if (str == "region") {
        return SFZSink::kRegion;
    } else {
        return SFZSink::kHUnSppported;
    }
}

static bool parseLoopmode(const String& str, uint32_t* out) {
    if (out == nullptr) {
        return false;
    }
    if (str == "no_loop") {
        *out = SFZSink::kNoLoop;
    } else if (str == "one_shot") {
        *out = SFZSink::kOneShot;
    } else if (str == "loop_continuous") {
        *out = SFZSink::kLoopContinuous;
    } else if (str == "loop_sustain") {
        *out = SFZSink::kLoopSustain;
    } else {
        return false;
    }
    return true;
}

static bool parseUint32(const String& str, uint32_t* out) {
    const char* strptr = str.c_str();
    char* endptr = nullptr;
    if (out == nullptr) {
        return false;
    }
    if (!isDigit(*strptr)) {
        return false;
    }
    unsigned long val = strtoul(strptr, &endptr, 0);
    if (val == ULONG_MAX && errno == ERANGE) {
        return false;
    }
    if (endptr != nullptr && endptr != strptr && *endptr == '\0') {
        if (0 <= val && val <= UINT32_MAX) {
            *out = uint32_t(val);
            return true;
        }
    }
    return false;
}

static bool parseInt32(const String& str, int32_t* out) {
    const char* strptr = str.c_str();
    char* endptr = nullptr;
    if (out == nullptr) {
        return false;
    }
    long val = strtol(strptr, &endptr, 0);
    if ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE) {
        return false;
    }
    if (endptr != nullptr && endptr != strptr && *endptr == '\0') {
        if (INT32_MIN <= val && val <= INT32_MAX) {
            *out = int32_t(val);
            return true;
        }
    }
    return false;
}

static bool parseNotename(const String& str, uint32_t* out) {
    const unsigned char kBasenote[] = {69, 71, 60, 62, 64, 65, 67};

    if (out == nullptr) {
        return false;
    }
    if (parseUint32(str, out)) {
        return true;
    }
    if (str.length() < 2) {
        return false;
    }
    int index = 0;
    char pitch = toupper(str[index++]);
    if (pitch < 'A' || 'G' < pitch) {
        return false;
    }
    int note = 0;
    if (str[index] == '#') {
        note = kBasenote[pitch - 'A'] + 1;
        index++;
    } else if (str[index] == 'b') {
        note = kBasenote[pitch - 'A'] - 1;
        index++;
    } else {
        note = kBasenote[pitch - 'A'];
    }
    int32_t octave = 0;
    if (parseInt32(str.substring(index), &octave)) {
        *out = note + (octave - 4) * PITCH_NUM;
        return true;
    }
    return false;
}

static SFZSink::Region buildRegion(const SFZSink::OpcodeContainer& container) {
    const int kSampleSize = (kPbBitDepth / 8) * kPbChannelCount;

    SFZSink::Region region;

    region.sample = normalizePath(container.sample);
    SDClass sdcard;
    if (sdcard.begin()) {
        File file = sdcard.open(region.sample);
        if (file) {
            WavReader wav = WavReader(file);
            region.pcm_offset = wav.getPcmOffset();
            region.pcm_size = wav.getPcmSize();
            file.close();
        } else {
            error_printf("SFZSink: cannot open '%s'\n", region.sample.c_str());
            region.pcm_offset = 0;
            region.pcm_size = 0;
        }
    }
    region.silence = container.silence;

    size_t pcm_samples = region.pcm_size / kSampleSize;

    region.lokey = container.opcode[SFZSink::kOpcodeLokey];
    region.hikey = container.opcode[SFZSink::kOpcodeHikey];
    region.sw_last = container.opcode[SFZSink::kOpcodeSwLast];
    size_t offset_samples = (pcm_samples < container.opcode[SFZSink::kOpcodeOffset]) ? pcm_samples : container.opcode[SFZSink::kOpcodeOffset];
    region.offset = region.pcm_offset + offset_samples * kSampleSize;
    if (pcm_samples > 0) {
        if (container.specified & (1U << SFZSink::kOpcodeEnd)) {
            size_t end_samples = (pcm_samples - 1 < container.opcode[SFZSink::kOpcodeEnd]) ? pcm_samples - 1 : container.opcode[SFZSink::kOpcodeEnd];
            region.end = region.pcm_offset + (end_samples + 1) * kSampleSize;
        } else {
            region.end = region.pcm_offset + region.pcm_size;
        }
    } else {
        region.end = 0;
    }
    region.count = container.opcode[SFZSink::kOpcodeCount];
    if (container.specified & (1U << SFZSink::kOpcodeCount)) {
        region.loop_mode = SFZSink::kOneShot;
    } else {
        region.loop_mode = (SFZSink::LoopMode)container.opcode[SFZSink::kOpcodeLoopMode];
    }
    size_t loop_start_samples = (pcm_samples < container.opcode[SFZSink::kOpcodeLoopStart]) ? pcm_samples : container.opcode[SFZSink::kOpcodeLoopStart];
    region.loop_start = region.pcm_offset + loop_start_samples * kSampleSize;
    region.loop_start = (region.offset > region.loop_start) ? region.offset : region.loop_start;
    if (pcm_samples > 0) {
        if (container.specified & (1U << SFZSink::kOpcodeLoopEnd)) {
            size_t loop_end_samples =
                (pcm_samples - 1 < container.opcode[SFZSink::kOpcodeLoopEnd]) ? pcm_samples - 1 : container.opcode[SFZSink::kOpcodeLoopEnd];
            region.loop_end = region.pcm_offset + (loop_end_samples + 1) * kSampleSize;
        } else {
            region.loop_end = region.pcm_offset + region.pcm_size;
        }
    } else {
        region.loop_end = 0;
    }
    region.loop_end = (region.end < region.loop_end) ? region.end : region.loop_end;

    return region;
}

SFZSink::SFZSink(const String& sfz_filename)
    : NullFilter(),
      SFZHandler(),
      sfz_folder_path_(getFolderPath(sfz_filename)),
      regions_(),
      global_(),
      group_(),
      region_(),
      current_header_(kHUnSppported),
      current_group_(0),
      regions_in_group_(-1),
      sw_lokey_(NOTE_NUMBER_MIN),
      sw_hikey_(NOTE_NUMBER_MAX),
      sw_last_(INVALID_NOTE_NUMBER),
      playing_region_(nullptr),
      file_(),
      loop_count_(0),
      volume_(0),
      renderer_(kPbSampleFrq, kPbBitDepth, kPbChannelCount, kPbSampleCount, kPbCacheSize, 1) {
    SDClass sdcard;
    if (!sdcard.begin()) {
        error_printf("error: cannot access sdcard\n");
        return;
    }

    File sfz_file = sdcard.open(sfz_filename.c_str());
    if (!sfz_file) {
        error_printf("error: file open error(%s)\n", sfz_filename.c_str());
        return;
    }
    SFZParser sfz_parser;
    sfz_parser.parse(sfz_file, sfz_filename, this);
    sfz_file.close();

#if DEBUG
    debug_printf("regions: %d\n", (int)regions_.size());
    for (size_t i = 0; i < regions_.size(); i++) {
        // clang-format off
        debug_printf("%u/%u: \"%s\" lk,hk,sl:%d,%d,%d o,e:%d,%d cnt,lm:%d,%d ls,le:%d,%d\n",
                     (unsigned int)(i + 1), (unsigned int)regions_.size(), regions_[i].sample.c_str(),
                     regions_[i].lokey, regions_[i].hikey, regions_[i].sw_last,
                     regions_[i].offset, regions_[i].end,
                     regions_[i].count, (int)regions_[i].loop_mode,
                     regions_[i].loop_start, regions_[i].loop_end);
        // clang-format on
    }
#endif
}

SFZSink::~SFZSink() {
}

bool SFZSink::begin() {
    NullFilter::begin();
    debug_printf("start playback\n");
    renderer_.setState(PcmRenderer::kStateReady);
    renderer_.begin();
    renderer_.clear(0);
    for (int i = 0; i < kPreLoadFrameNum; i++) {
        if (!renderer_.render()) {
            break;
        }
    }
    return true;
}

void SFZSink::update() {
    NullFilter::update();
    loadSound(kLoadFrameNum);
}

bool SFZSink::isAvailable(int param_id) {
    trace_printf("SFZSink::isAvailable(%d)\n", param_id);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        return true;
    }
    return NullFilter::isAvailable(param_id);
}

intptr_t SFZSink::getParam(int param_id) {
    trace_printf("SFZSink::getParam(%d)\n", param_id);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        return volume_;
    }
    return NullFilter::getParam(param_id);
}

bool SFZSink::setParam(int param_id, intptr_t value) {
    trace_printf("SFZSink::setParam(%d, %d)\n", param_id, value);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        volume_ = constrain(value, -1020, 120);
        renderer_.setVolume(volume_, 0, 0);
        return true;
    }
    return NullFilter::setParam(param_id, value);
}

bool SFZSink::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    debug_printf("SFZSink::sendNoteOff(%d,%d,%d)\n", note, velocity, channel);
    if (playing_region_ && playing_region_->loop_mode == kOneShot) {
        debug_printf("%s: playing one_shot\n", __func__);
        return true;
    }
    stopSound();

    return true;
}

bool SFZSink::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    debug_printf("SFZSink::sendNoteOn(%d,%d,%d)\n", note, velocity, channel);

    if (playing_region_ && playing_region_->loop_mode == kOneShot) {
        debug_printf("%s: playing one_shot\n", __func__);
        return true;
    }

    if (velocity == 0) {
        return sendNoteOff(note, velocity, channel);
    }

    if (sw_lokey_ <= note && note <= sw_hikey_) {
        sw_last_ = note;
    }

    playing_region_ = nullptr;
    for (const auto& e : regions_) {
        if (e.sw_last != INVALID_NOTE_NUMBER && e.sw_last != sw_last_) {
            continue;
        }
        if (e.lokey <= note && note <= e.hikey) {
            playing_region_ = &e;
            break;
        }
    }
    if (playing_region_ == nullptr) {
        error_printf("%s: no match region for %d\n", __func__, note);
        return false;
    }

    if (note == INVALID_NOTE_NUMBER) {
        sendNoteOff(note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
    } else {
        debug_printf("%s: play note %d\n", __func__, note);
        startSound();
    }

    return true;
}

void SFZSink::startSfz() {
    regions_.clear();
    {
        global_.is_valid = true;
        global_.group_id = 0;
        global_.sample = "";
        global_.silence = false;
        global_.specified = 0;
        global_.opcode[kOpcodeSample] = 0;
        global_.opcode[kOpcodeLokey] = NOTE_NUMBER_MIN;
        global_.opcode[kOpcodeHikey] = NOTE_NUMBER_MAX;
        global_.opcode[kOpcodeSwLokey] = NOTE_NUMBER_MIN;
        global_.opcode[kOpcodeSwHikey] = NOTE_NUMBER_MAX;
        global_.opcode[kOpcodeSwLast] = INVALID_NOTE_NUMBER;
        global_.opcode[kOpcodeOffset] = 0;
        global_.opcode[kOpcodeEnd] = 0;
        global_.opcode[kOpcodeCount] = 1;
        global_.opcode[kOpcodeLoopMode] = kNoLoop;
        global_.opcode[kOpcodeLoopStart] = 0;
        global_.opcode[kOpcodeLoopEnd] = 0;
        global_.opcode[kOpcodeSwDefault] = INVALID_NOTE_NUMBER;
    }
    group_ = global_;
    region_ = group_;

    sw_lokey_ = NOTE_NUMBER_MIN;
    sw_hikey_ = NOTE_NUMBER_MAX;
    sw_last_ = INVALID_NOTE_NUMBER;
    current_header_ = kHUnSppported;
    current_group_ = 0;
    regions_in_group_ = -1;
}

void SFZSink::endSfz() {
    header("end last header");
}

void SFZSink::header(const String& header) {
    trace_printf("SFZSink::header('%s')\n", header.c_str());
    // end header
    if (current_header_ == kGlobal) {
        global_ = region_;
    } else if (current_header_ == kGroup) {
        group_ = region_;
    } else if (current_header_ == kRegion) {
        if (region_.is_valid) {
            regions_.push_back(buildRegion(region_));
        }
    }

    // manage group that has no region
    current_header_ = convertHeaderToNum(header);
    if (current_header_ != kRegion) {
        if (regions_in_group_ == 0) {
            if (region_.is_valid) {
                regions_.push_back(buildRegion(group_));
            }
        }
    }

    // start header
    if (current_header_ == kGlobal) {
        region_ = global_;
        regions_in_group_ = -1;
    } else if (current_header_ == kGroup) {
        region_ = group_;
        region_.group_id = ++current_group_;
        regions_in_group_ = 0;
    } else if (current_header_ == kRegion) {
        region_ = group_;
        if (regions_in_group_ >= 0) {
            regions_in_group_++;
        }
    } else {
        regions_in_group_ = -1;
    }
}

void SFZSink::opcode(const String& opcode, const String& value) {
    trace_printf("SFZSink::opcode('%s', '%s')\n", opcode.c_str(), value.c_str());
    // clang-format off
    const  OpcodeSpec opcode_spec[] = {
    //   opcode_str    opcode_enum       min              max              parser
        {"sample",     kOpcodeSample,    0,               0,               nullptr      },
        {"hikey",      kOpcodeHikey,     NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"lokey",      kOpcodeLokey,     NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"key",        kOpcodeHikey,     NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"key",        kOpcodeLokey,     NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"offset",     kOpcodeOffset,    0,               UINT32_MAX,      parseUint32  },
        {"end",        kOpcodeEnd,       0,               UINT32_MAX,      parseUint32  },
        {"count",      kOpcodeCount,     0,               UINT32_MAX,      parseUint32  },
        {"loop_mode",  kOpcodeLoopMode,  kNoLoop,         kLoopSustain,    parseLoopmode},
        {"loop_start", kOpcodeLoopStart, 0,               UINT32_MAX,      parseUint32  },
        {"loop_end",   kOpcodeLoopEnd,   0,               UINT32_MAX,      parseUint32  },
        {"loopmode",   kOpcodeLoopMode,  kNoLoop,         kLoopSustain,    parseLoopmode},
        {"loopstart",  kOpcodeLoopStart, 0,               UINT32_MAX,      parseUint32  },
        {"loopend",    kOpcodeLoopEnd,   0,               UINT32_MAX,      parseUint32  },
        {"sw_last",    kOpcodeSwLast,    NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"sw_lokey",   kOpcodeSwLokey,   NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"sw_hikey",   kOpcodeSwHikey,   NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"sw_default", kOpcodeSwDefault, NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename}
    };
    // clang-format on

    // find opcode spec
    for (size_t i = 0; i < sizeof(opcode_spec) / sizeof(opcode_spec[0]); i++) {
        const OpcodeSpec* spec = &opcode_spec[i];
        if (opcode != opcode_spec[i].opcode_str) {
            continue;
        }

        if (spec->opcode_enum == kOpcodeSample) {
            region_.sample = normalizePath(sfz_folder_path_ + value);
            SDClass sdcard;
            if (sdcard.begin()) {
                if (!sdcard.exists(region_.sample)) {
                    error_printf("SFZSink: no such file '%s'\n", region_.sample.c_str());
                    region_.is_valid = false;
                }
            }
        } else {
            if (spec->opcode_enum == kOpcodeEnd && value == "-1") {
                region_.silence = true;
                continue;
            }
            uint32_t int_value = 0;
            if (!spec->parser(value, &int_value)) {
                error_printf("SFZSink: parse error '%s=%s'\n", opcode.c_str(), value.c_str());
                region_.is_valid = false;
                continue;
            }
            if (int_value < spec->min || spec->max < int_value) {
                error_printf("SFZSink: out of range '%s=%s'\n", opcode.c_str(), value.c_str());
                region_.is_valid = false;
                continue;
            }
            region_.opcode[spec->opcode_enum] = int_value;
            if (spec->opcode_enum == kOpcodeSwLokey) {
                sw_lokey_ = int_value;
            } else if (spec->opcode_enum == kOpcodeSwHikey) {
                sw_hikey_ = int_value;
            } else if (spec->opcode_enum == kOpcodeSwDefault) {
                sw_last_ = int_value;
            }
        }
        region_.specified |= (1U << spec->opcode_enum);
    }
}

void SFZSink::startSound() {
    trace_printf("enter: %s()\n", __func__);

    if (playing_region_ == nullptr) {
        printf("SFZ data not found.\n");
        return;
    }

    SDClass sdcard;
    if (!sdcard.begin()) {
        error_printf("error: cannot access sdcard\n");
        return;
    }
    if (file_) {
        file_.close();
    }
    file_ = sdcard.open(playing_region_->sample.c_str());
    if (!file_) {
        error_printf("error: file open error(%s)\n", playing_region_->sample.c_str());
        stopSound();
        return;
    }
    file_.seek(playing_region_->offset);

    loop_count_ = 0;

    renderer_.clear(0);
    renderer_.setState(PcmRenderer::kStateActive);
    loadSound(kPreLoadFrameNum);
}

void SFZSink::loadSound(int frame) {
    trace_printf("enter: %s()\n", __func__);
    for (int i = 0; i < frame; i++) {
        if (renderer_.getState() != PcmRenderer::kStateActive) {
            break;
        }
        if (playing_region_ == nullptr) {
            break;
        }

        // end of pcm
        if (playing_region_->loop_mode == kNoLoop) {
            if (file_.position() >= playing_region_->end) {
                debug_printf("SFZSink: no_loop end\n");
                stopSound();
                break;
            }
        } else {
            if (file_.position() >= playing_region_->loop_end) {
                file_.seek(playing_region_->loop_start);
                loop_count_++;
            }
        }

        // one_shot
        if (playing_region_->loop_mode == kOneShot) {
            if (loop_count_ >= playing_region_->count) {
                debug_printf("SFZSink: one_shot end\n");
                stopSound();
                break;
            }
        }

        writeToRenderer(renderer_, 0, file_, playing_region_->loop_end);
    }
}

void SFZSink::stopSound() {
    trace_printf("enter: %s()\n", __func__);
    renderer_.setState(PcmRenderer::kStatePause);
    playing_region_ = nullptr;
}

#endif  // ARDUINO_ARCH_SPRESENSE
