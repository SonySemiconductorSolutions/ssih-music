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
#ifdef EMMC_USE
#include <eMMC.h>
#define EMMC_POWER_PIN 26
#define EMMC_EN 31 // High=eMMC / Low=SD
#else
#include <SDHCI.h>
#endif

#include <Storage.h>

#include "path_util.h"
#include "WavReader.h"

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

static const char kClassName[] = "SFZSink";

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
const int kPreloadFrames = 3;
const int kLoadFrames = 10;

const static int kUnallocatedChannel = -1;
const static int kDeallocatedChannel = -2;

static SFZSink::Header convertHeaderToEnum(const String& str) {
    if (str == "global") {
        return SFZSink::kGlobal;
    } else if (str == "group") {
        return SFZSink::kGroup;
    } else if (str == "control") {
        return SFZSink::kControl;
    } else if (str == "region") {
        return SFZSink::kRegion;
    } else {
        return SFZSink::kInvalidHeader;
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
        if (val <= UINT32_MAX) {
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

static bool parseQ16(const String& str, uint32_t* out) {
    const char* strptr = str.c_str();
    char* endptr = nullptr;
    if (out == nullptr) {
        return false;
    }

    long ival = strtol(strptr, &endptr, 10);
    if ((ival == LONG_MIN || ival == LONG_MAX) && errno == ERANGE) {
        return false;
    }
    if (endptr == nullptr) {
        return false;
    }
    if (ival < INT16_MIN && INT16_MAX < ival) {
        return false;
    }
    unsigned long fval = 0;
    if (*endptr == '.') {
        // INT32_MAX = 2147483647
        char buf[] = "00000000";
        size_t max_length = strlen(buf);
        const char* p = endptr + 1;
        for (size_t i = 0; i < max_length; i++) {
            if (isDigit(*p)) {
                buf[i] = *p++;
                continue;
            }
            break;
        }
        fval = strtoul(buf, &endptr, 10);
        if (fval == ULONG_MAX && errno == ERANGE) {
            return false;
        }
        if (endptr == nullptr) {
            return false;
        }
        if (endptr == nullptr || endptr == buf || *endptr != '\0') {
            return false;
        }
    }
    int32_t val = ival;
    if (strptr[0] == '-') {
        for (int i = 0; i < 4; i++) {
            fval = fval * 16;
            val = (val * 16) - (fval / 100000000);
            fval = fval % 100000000;
        }
    } else {
        for (int i = 0; i < 4; i++) {
            fval = fval * 16;
            val = (val * 16) + (fval / 100000000);
            fval = fval % 100000000;
        }
    }
    *out = (uint32_t)val;

    return true;
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
    region.pcm_offset = 0;
    region.pcm_size = 0;
    File file = File(region.sample.c_str());
    if (!file) {
        error_printf("[%s::%s] cannot open \"%s\"\n", kClassName, __func__, region.sample.c_str());
    }
    if (file.isDirectory()) {
        error_printf("[%s::%s] \"%s\" is directory\n", kClassName, __func__, region.sample.c_str());
    } else {
        WavReader wav = WavReader(file);
        region.pcm_offset = wav.getPcmOffset();
        region.pcm_size = wav.getPcmSize();
    }
    file.close();
    region.silence = container.silence;

    size_t pcm_samples = region.pcm_size / kSampleSize;

    region.lochan = container.opcode[SFZSink::kOpcodeLochan];
    region.hichan = container.opcode[SFZSink::kOpcodeHichan];
    region.lokey = container.opcode[SFZSink::kOpcodeLokey];
    region.hikey = container.opcode[SFZSink::kOpcodeHikey];
    region.lovel = container.opcode[SFZSink::kOpcodeLovel];
    region.hivel = container.opcode[SFZSink::kOpcodeHivel];
    region.sw_last = container.opcode[SFZSink::kOpcodeSwLast];
    region.loprog = container.opcode[SFZSink::kOpcodeLoProg];
    region.hiprog = container.opcode[SFZSink::kOpcodeHiProg];
    region.locc0 = container.opcode[SFZSink::kOpcodeLoCC0];
    region.hicc0 = container.opcode[SFZSink::kOpcodeHiCC0];
    region.locc32 = container.opcode[SFZSink::kOpcodeLoCC32];
    region.hicc32 = container.opcode[SFZSink::kOpcodeHiCC32];
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

SFZSink::SFZSink(const String& sfz_path)
    : NullFilter(),
      SFZHandler(),
      sfz_path_(sfz_path),
      regions_(),
      playback_units_(),
      renderer_(kPbSampleFrq, kPbBitDepth, kPbChannelCount, kPbSampleCount, kPbCacheSize, 4),
      bank_(),
      volume_(0),
      prog_num_(0),
      global_(),
      group_(),
      region_(),
      header_(kInvalidHeader),
      group_id_(0),
      regions_in_group_(-1),
      default_path_(""),
      sw_lokey_(NOTE_NUMBER_MIN),
      sw_hikey_(NOTE_NUMBER_MAX),
      sw_last_(INVALID_NOTE_NUMBER) {
}

SFZSink::~SFZSink() {
}

bool SFZSink::begin() {
    bool ret = true;
    NullFilter::begin();

    debug_printf("[%s::%s] loading sfz\n", kClassName, __func__);
#ifdef EMMC_USE
    pinMode(EMMC_EN, OUTPUT);
    digitalWrite(EMMC_EN, HIGH); //eMMC Select

    pinMode(EMMC_POWER_PIN, OUTPUT);
    digitalWrite(EMMC_POWER_PIN, HIGH);
    delay(50);
    if (!eMMC.begin())
    {
        Serial.println("eMMC can't begin");
        exit(1);
    }
#else
    SDClass sdcard;
    if (!sdcard.begin()) {
        error_printf("[%s::%s] error: cannot access sdcard\n", kClassName, __func__);
        exit(1);
     }
#endif
    File file;
    if (sfz_path_.startsWith("/")) {
        file = File(sfz_path_.c_str());
    } else {
#ifdef EMMC_USE
        file = eMMC.open(sfz_path_);
#else
        file = sdcard.open(sfz_path_);
#endif
        sfz_path_ = file.name();
    }
    if (!file) {
        error_printf("[%s::%s] error: cannot open \"%s\"\n", kClassName, __func__, sfz_path_.c_str());
        ret = false;
    } else {
        SFZParser parser;
        parser.parse(file, sfz_path_, this);
        file.close();
        debug_printf("[%s::%s] regions: %d\n", kClassName, __func__, (int)regions_.size());
#if DEBUG
        for (size_t i = 0; i < regions_.size(); i++) {
            debug_printf(
                "[%s::%s] %d/%d: \"%s\" "
                "lc=%d,hc=%d lk=%d,hk=%d,sl=%d lv=%d, hv=%d, lp=%d, hp=%d, lc0=%d, hc0=%d, lc32=%d, hc32=%d, o=%d,e=%d lm=%d,c=%d ls=%d,le=%d\n",
                kClassName, __func__,                                       // [file::class]
                (int)(i + 1), (int)regions_.size(),                         // i/N
                regions_[i].sample.c_str(),                                 // sample
                regions_[i].lochan, regions_[i].hichan,                     // lochan,hichan
                regions_[i].lokey, regions_[i].hikey, regions_[i].sw_last,  // lokey,hikey,sw_last
                regions_[i].lovel, regions_[i].hivel,                       // lovel,hivel
                regions_[i].loprog, regions_[i].hiprog,                     // loprog,hiprog
                regions_[i].locc0, regions_[i].hicc0,                       // locc0,hicc0
                regions_[i].locc32, regions_[i].hicc32,                     // locc32,hicc32
                regions_[i].offset, regions_[i].end,                        // offset,end
                (int)regions_[i].loop_mode, regions_[i].count,              // loop_mode,count
                regions_[i].loop_start, regions_[i].loop_end);              // loop_start,loop_end
        }
#endif
    }

    debug_printf("[%s::%s] start playback\n", kClassName, __func__);
    renderer_.begin();

    return ret;
}

void SFZSink::update() {
    NullFilter::update();
    for (auto& e : playback_units_) {
        continuePlayback(&e, kLoadFrames);
    }
}

bool SFZSink::isAvailable(int param_id) {
    trace_printf("[%s::%s] (%d)\n", kClassName, __func__, param_id);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        return true;
    } else if (param_id == SFZSink::PARAMID_SW_LAST) {
        return true;
    } else if (param_id == SFZSink::PARAMID_SW_LOKEY) {
        return true;
    } else if (param_id == SFZSink::PARAMID_SW_HIKEY) {
        return true;
    }
    return NullFilter::isAvailable(param_id);
}

intptr_t SFZSink::getParam(int param_id) {
    trace_printf("[%s::%s] (%d)\n", kClassName, __func__, param_id);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        return volume_;
    } else if (param_id == SFZSink::PARAMID_SW_LAST) {
        return sw_last_;
    } else if (param_id == SFZSink::PARAMID_SW_LOKEY) {
        return sw_lokey_;
    } else if (param_id == SFZSink::PARAMID_SW_HIKEY) {
        return sw_hikey_;
    }
    return NullFilter::getParam(param_id);
}

bool SFZSink::setParam(int param_id, intptr_t value) {
    trace_printf("[%s::%s] (%d, %d)\n", kClassName, __func__, param_id, (int)value);
    if (param_id == Filter::PARAMID_OUTPUT_LEVEL) {
        volume_ = constrain(value, -1020, 120);
        renderer_.setVolume(volume_, 0, 0);
        return true;
    } else if (param_id == SFZSink::PARAMID_SW_LAST) {
        sw_last_ = (uint8_t)value;
        return true;
    }

    return NullFilter::setParam(param_id, value);
}

bool SFZSink::sendNoteOff(uint8_t note, uint8_t /*velocity*/, uint8_t channel) {
    debug_printf("[%s::%s] (%d, %d, %d)\n", kClassName, __func__, note, velocity, channel);
    for (auto& e : playback_units_) {
        if (e.render_ch == kDeallocatedChannel) {
            continue;
        }
        if (e.channel != channel) {
            continue;
        }
        if (e.region->loop_mode == kOneShot) {
            continue;
        }
        if (e.note == note) {
            stopPlayback(&e);
            break;
        }
    }
    return true;
}

bool SFZSink::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    debug_printf("[%s::%s] (%d, %d, %d)\n", kClassName, __func__, note, velocity, channel);

    if (note < NOTE_NUMBER_MIN || NOTE_NUMBER_MAX < note) {
        return false;
    }
    if (velocity == 0) {
        return sendNoteOff(note, velocity, channel);
    }

    if (sw_lokey_ <= note && note <= sw_hikey_) {
        sw_last_ = note;
    }

    for (const auto& e : playback_units_) {
        if (e.channel == channel && e.region->loop_mode == kOneShot) {
            debug_printf("[%s::%s] playing one_shot\n", kClassName, __func__);
            return true;
        }
    }

    Region* region = nullptr;
    for (auto& e : regions_) {
        if (e.sw_last != INVALID_NOTE_NUMBER && e.sw_last != sw_last_) {
            continue;
        }
        if (channel < e.lochan || e.hichan < channel) {
            continue;
        }
        if (note < e.lokey || e.hikey < note) {
            continue;
        }
        if (velocity < e.lovel || e.hivel < velocity) {
            continue;
        }
        if (bank_.msb < e.locc0 || e.hicc0 < bank_.msb) {
            continue;
        }
        if (bank_.lsb < e.locc32 || e.hicc32 < bank_.lsb) {
            continue;
        }
        if (prog_num_ < e.loprog || e.hiprog < prog_num_) {
            continue;
        }
        region = &e;
        break;
    }
    if (region == nullptr) {
        error_printf("[%s::%s] no match region for note=%d,channel=%d\n", kClassName, __func__, note, channel);
        return false;
    }

    startPlayback(note, velocity, channel, region);

    return true;
}

bool SFZSink::sendControlChange(uint8_t ctrl_num, uint8_t value, uint8_t channel) {
    if (ctrl_num == 0x00) {
        bank_.msb = value;
    } else if (ctrl_num == 0x20) {
        bank_.lsb = value;
    } else if (0x7B <= ctrl_num && ctrl_num <= 0x7F) {
        debug_printf("[%s::%s] All Note Off\n", kClassName, __func__);
        for (auto& e : playback_units_) {
            if (e.channel == channel) {
                stopPlayback(&e);
            }
        }
    }

    return true;
}

bool SFZSink::sendProgramChange(uint8_t prog_num, uint8_t /*channel*/) {
    debug_printf("[%s::%s] bank=%d, prog=%d\n", kClassName, __func__,    //
                 ((bank_.msb & 0x7F) << 7) | ((bank_.lsb & 0x7F) << 0),  // bank
                 prog_num                                                // prog
    );
    prog_num_ = prog_num;
    return true;
}

size_t SFZSink::getNumberOfRegions() {
    return regions_.size();
}

const SFZSink::Region* SFZSink::getRegion(size_t id) {
    if (id < regions_.size()) {
        return &regions_[id];
    }
    return nullptr;
}

uint8_t SFZSink::getSwLoKey() {
    return sw_lokey_;
}

uint8_t SFZSink::getSwHiKey() {
    return sw_hikey_;
}

uint8_t SFZSink::getSwLast() {
    return sw_last_;
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
        global_.opcode[kOpcodeLochan] = 1;
        global_.opcode[kOpcodeHichan] = 16;
        global_.opcode[kOpcodeLokey] = NOTE_NUMBER_MIN;
        global_.opcode[kOpcodeHikey] = NOTE_NUMBER_MAX;
        global_.opcode[kOpcodeLovel] = 0;
        global_.opcode[kOpcodeHivel] = 127;
        global_.opcode[kOpcodeLorand] = 0;  // TODO: set 0 by Qm.n (Q notation)
        global_.opcode[kOpcodeHirand] = 1;  // TODO: set 1 by Qm.n (Q notation)
        global_.opcode[kOpcodeSeqLength] = 1;
        global_.opcode[kOpcodeSeqPosition] = 1;
        global_.opcode[kOpcodeSwLokey] = NOTE_NUMBER_MIN;
        global_.opcode[kOpcodeSwHikey] = NOTE_NUMBER_MAX;
        global_.opcode[kOpcodeSwLast] = INVALID_NOTE_NUMBER;
        global_.opcode[kOpcodeGroup] = 0;
        global_.opcode[kOpcodeOffBy] = 0;
        global_.opcode[kOpcodeOffset] = 0;
        global_.opcode[kOpcodeEnd] = 0;
        global_.opcode[kOpcodeCount] = 1;
        global_.opcode[kOpcodeLoopMode] = kNoLoop;
        global_.opcode[kOpcodeLoopStart] = 0;
        global_.opcode[kOpcodeLoopEnd] = 0;
        global_.opcode[kOpcodeSwDefault] = INVALID_NOTE_NUMBER;
        global_.opcode[kOpcodeLoProg] = 0;
        global_.opcode[kOpcodeHiProg] = 127;
        global_.opcode[kOpcodeLoCC0] = 0;
        global_.opcode[kOpcodeHiCC0] = 127;
        global_.opcode[kOpcodeLoCC32] = 0;
        global_.opcode[kOpcodeHiCC32] = 127;
    }
    group_ = global_;
    region_ = group_;

    default_path_ = "";
    sw_lokey_ = NOTE_NUMBER_MIN;
    sw_hikey_ = NOTE_NUMBER_MAX;
    sw_last_ = INVALID_NOTE_NUMBER;
    header_ = kInvalidHeader;
    group_id_ = 0;
    regions_in_group_ = -1;
}

void SFZSink::endSfz() {
    trace_printf("[%s::%s] ()\n", kClassName, __func__);
}

void SFZSink::startHeader(const String& header) {
    trace_printf("[%s::%s] (\"%s\")\n", kClassName, __func__, header.c_str());

    // manage group that has no region
    header_ = convertHeaderToEnum(header);
    if (header_ != kRegion) {
        if (regions_in_group_ == 0) {
            if (region_.is_valid) {
                regions_.push_back(buildRegion(group_));
            }
        }
    }

    // start header
    if (header_ == kGlobal) {
        region_ = global_;
        regions_in_group_ = -1;
    } else if (header_ == kGroup) {
        region_ = group_;
        region_.group_id = ++group_id_;
        regions_in_group_ = 0;
    } else if (header_ == kRegion) {
        region_ = group_;
        if (regions_in_group_ >= 0) {
            regions_in_group_++;
        }
    } else {
        regions_in_group_ = -1;
    }
}

void SFZSink::endHeader(const String& /*header*/) {
    // trace_printf("[%s::%s] (\"%s\")\n", kClassName, __func__, header.c_str());

    // end header
    if (header_ == kGlobal) {
        global_ = region_;
        group_ = region_;
    } else if (header_ == kGroup) {
        group_ = region_;
    } else if (header_ == kRegion) {
        if (region_.is_valid) {
            regions_.push_back(buildRegion(region_));
        }
    }

    header_ = kInvalidHeader;
}

void SFZSink::opcode(const String& opcode, const String& value) {
    trace_printf("[%s::%s] (\"%s\", \"%s\")\n", kClassName, __func__, opcode.c_str(), value.c_str());
    // clang-format off
    const  OpcodeSpec opcode_spec[] = {
    //   opcode_str      opcode_enum         min              max              parser
        {"sample",       kOpcodeSample,      0,               0,               nullptr      },
        {"lochan",       kOpcodeLochan,      1,               16,              parseUint32  },
        {"hichan",       kOpcodeHichan,      1,               16,              parseUint32  },
        {"hikey",        kOpcodeHikey,       NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"lokey",        kOpcodeLokey,       NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"key",          kOpcodeHikey,       NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"key",          kOpcodeLokey,       NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"lovel",        kOpcodeLovel,       0,               127,             parseUint32  },
        {"hivel",        kOpcodeHivel,       0,               127,             parseUint32  },
        {"lorand",       kOpcodeLorand,      0x00000000,      0x00010000,      parseQ16     }, //< not supported
        {"hirand",       kOpcodeHirand,      0x00000000,      0x00010000,      parseQ16     }, //< not supported
        {"seq_length",   kOpcodeSeqLength,   1,               100,             parseUint32  }, //< not supported
        {"seq_position", kOpcodeSeqPosition, 1,               100,             parseUint32  }, //< not supported
        {"group",        kOpcodeGroup,       0,               UINT32_MAX,      parseUint32  }, //< not supported
        {"off_by",       kOpcodeOffBy,       0,               UINT32_MAX,      parseUint32  }, //< not supported
        {"offset",       kOpcodeOffset,      0,               UINT32_MAX,      parseUint32  },
        {"end",          kOpcodeEnd,         0,               UINT32_MAX,      parseUint32  },
        {"count",        kOpcodeCount,       0,               UINT32_MAX,      parseUint32  },
        {"loop_mode",    kOpcodeLoopMode,    kNoLoop,         kLoopSustain,    parseLoopmode},
        {"loop_start",   kOpcodeLoopStart,   0,               UINT32_MAX,      parseUint32  },
        {"loop_end",     kOpcodeLoopEnd,     0,               UINT32_MAX,      parseUint32  },
        {"loopmode",     kOpcodeLoopMode,    kNoLoop,         kLoopSustain,    parseLoopmode},
        {"loopstart",    kOpcodeLoopStart,   0,               UINT32_MAX,      parseUint32  },
        {"loopend",      kOpcodeLoopEnd,     0,               UINT32_MAX,      parseUint32  },
        {"sw_last",      kOpcodeSwLast,      NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"sw_lokey",     kOpcodeSwLokey,     NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"sw_hikey",     kOpcodeSwHikey,     NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"default_path", kOpcodeDefaultPath, 0,               0,               nullptr      },
        {"sw_default",   kOpcodeSwDefault,   NOTE_NUMBER_MIN, NOTE_NUMBER_MAX, parseNotename},
        {"loprog",       kOpcodeLoProg,      0,               127,             parseUint32  },
        {"hiprog",       kOpcodeHiProg,      0,               127,             parseUint32  },
        {"locc0",        kOpcodeLoCC0,       0,               127,             parseUint32  },
        {"hicc0",        kOpcodeHiCC0,       0,               127,             parseUint32  },
        {"locc32",       kOpcodeLoCC32,      0,               127,             parseUint32  },
        {"hicc32",       kOpcodeHiCC32,      0,               127,             parseUint32  }
    };
    // clang-format on

    // find opcode spec
    for (size_t i = 0; i < sizeof(opcode_spec) / sizeof(opcode_spec[0]); i++) {
        const OpcodeSpec* spec = &opcode_spec[i];
        if (opcode != opcode_spec[i].opcode_str) {
            continue;
        }

        if (spec->opcode_enum == kOpcodeSample) {
            String sfz_dir = getFolderPath(sfz_path_);
            String sample_prefix = joinPath(sfz_dir, default_path_);
            String sample_path = sample_prefix + value;
            sample_path.replace("\\", "/");
            region_.sample = normalizePath(sample_path);
            if (!Storage.exists(region_.sample)) {
                error_printf("[%s::%s] no such file \"%s\"\n", kClassName, __func__, region_.sample.c_str());
                region_.is_valid = false;
            }
        } else if (spec->opcode_enum == kOpcodeDefaultPath) {
            if (header_ == kControl) {
                default_path_ = value;
            }
        } else {
            if (spec->opcode_enum == kOpcodeEnd && value == "-1") {
                region_.silence = true;
                continue;
            }
            uint32_t int_value = 0;
            if (!spec->parser(value, &int_value)) {
                error_printf("[%s::%s] parse error '%s=%s'\n", kClassName, __func__, opcode.c_str(), value.c_str());
                region_.is_valid = false;
                continue;
            }
            if (int_value < spec->min || spec->max < int_value) {
                error_printf("[%s::%s] out of range '%s=%s'\n", kClassName, __func__, opcode.c_str(), value.c_str());
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

SFZSink::PlaybackUnit* SFZSink::startPlayback(uint8_t note, uint8_t /*velocity*/, uint8_t channel, Region* region) {
    trace_printf("[%s::%s] (%d,%d,%d,%p))\n", kClassName, __func__, note, velocity, channel, region);
    PlaybackUnit* unit = nullptr;
    if (region == nullptr) {
        error_printf("[%s::%s] error: region is null\n", kClassName, __func__);
        return unit;
    }
    for (auto& e : playback_units_) {
        if (e.render_ch == kDeallocatedChannel) {
            unit = &e;
            break;
        }
    }
    if (unit == nullptr) {
        playback_units_.push_back(PlaybackUnit());
        PlaybackUnit& e = playback_units_.back();
        unit = &e;
        unit->render_ch = kDeallocatedChannel;
    }
    unit->file = File(region->sample.c_str());
    if (unit->file) {
        unit->file.seek(region->offset);
        unit->note = note;
        unit->channel = channel;
        int render_channel = renderer_.allocateChannel();
        unit->render_ch = (render_channel < 0) ? kUnallocatedChannel : render_channel;
        unit->region = region;
        unit->loop = 0;

        if (unit->render_ch == kUnallocatedChannel) {
            error_printf("[%s::%s] cannot allocate channel\n", kClassName, __func__);
        } else {
            continuePlayback(unit, kPreloadFrames);
        }
    } else {
        error_printf("[%s::%s] error: file open error \"%s\"\n", kClassName, __func__, region->sample.c_str());
    }
    return unit;
}

void SFZSink::continuePlayback(PlaybackUnit* unit, int frames) {
    if (unit == nullptr) {
        return;
    }
    if (unit->render_ch < 0) {
        return;
    }
    for (int i = 0; i < frames; i++) {
        // end of pcm
        if (unit->region->loop_mode == kNoLoop) {
            if (unit->file.position() >= unit->region->end) {
                debug_printf("[%s::%s] no_loop end\n", kClassName, __func__);
                stopPlayback(unit);
                break;
            }
        } else {
            if (unit->file.position() >= unit->region->loop_end) {
                unit->loop++;
                unit->file.seek(unit->region->loop_start);
            }
        }

        // one_shot
        if (unit->region->loop_mode == kOneShot) {
            if (unit->loop >= unit->region->count) {
                debug_printf("[%s::%s] one_shot end\n", kClassName, __func__);
                stopPlayback(unit);
                break;
            }
        }

        // output PCM
        if (renderer_.getWritableSize(unit->render_ch) < kPbBlockSize) {
            break;
        }
        trace_printf("[%s::%s] %d %d,%d\n", kClassName, __func__, unit->render_ch, (int)renderer_.getReadableSize(unit->render_ch),
                     (int)renderer_.getWritableSize(unit->render_ch));
        size_t read_size = unit->region->loop_end - unit->file.position();
        read_size = (read_size < kPbBlockSize) ? read_size : kPbBlockSize;
        uint8_t buffer[read_size];
        unit->file.read(buffer, read_size);
        renderer_.write(unit->render_ch, buffer, read_size);
    }
}

void SFZSink::stopPlayback(PlaybackUnit* unit) {
    if (unit == nullptr) {
        return;
    }
    renderer_.deallocateChannel(unit->render_ch);
    unit->render_ch = kDeallocatedChannel;
    unit->file.close();
}

#endif  // ARDUINO_ARCH_SPRESENSE
