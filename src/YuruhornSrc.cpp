/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "YuruhornSrc.h"

#include <Arduino.h>

#include <MP.h>

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

#define UP_THRSH_PERCENT (80)      //< 80%
#define DOWN_THRSH_PERCENT (20)    //< 20%
#define NORMAL_THRSH_PERCENT (50)  //< 50%

static const struct YuruhornSrc::FreqNotePair freq2note_all[] = {
    {77, -1},                                                                                //< sentinel
    {82, 0},       {87, 1},       {92, 2},      {97, 3},      {103, 4},      {109, 5},       //< C-1 to B-1
    {116, 6},      {122, 7},      {130, 8},     {138, 9},     {146, 10},     {154, 11},      //<
    {164, 12},     {173, 13},     {184, 14},    {194, 15},    {206, 16},     {218, 17},      //< C0 to B0
    {231, 18},     {245, 19},     {260, 20},    {275, 21},    {291, 22},     {309, 23},      //<
    {327, 24},     {346, 25},     {367, 26},    {389, 27},    {412, 28},     {437, 29},      //< C1 to B1
    {462, 30},     {490, 31},     {519, 32},    {550, 33},    {583, 34},     {617, 35},      //<
    {654, 36},     {693, 37},     {734, 38},    {778, 39},    {824, 40},     {873, 41},      //< C2 to B2
    {925, 42},     {980, 43},     {1038, 44},   {1100, 45},   {1165, 46},    {1235, 47},     //<
    {1308, 48},    {1386, 49},    {1468, 50},   {1556, 51},   {1648, 52},    {1746, 53},     //< C3 to B3
    {1850, 54},    {1960, 55},    {2077, 56},   {2200, 57},   {2331, 58},    {2469, 59},     //<
    {2616, 60},    {2772, 61},    {2937, 62},   {3111, 63},   {3296, 64},    {3492, 65},     //< C4 to B4
    {3700, 66},    {3920, 67},    {4153, 68},   {4400, 69},   {4662, 70},    {4939, 71},     //<
    {5233, 72},    {5544, 73},    {5873, 74},   {6223, 75},   {6593, 76},    {6985, 77},     //< C5 to B5
    {7400, 78},    {7840, 79},    {8306, 80},   {8800, 81},   {9323, 82},    {9878, 83},     //<
    {10465, 84},   {11087, 85},   {11747, 86},  {12445, 87},  {13185, 88},   {13969, 89},    //< C6 to B6
    {14800, 90},   {15680, 91},   {16612, 92},  {17600, 93},  {18647, 94},   {19755, 95},    //<
    {20930, 96},   {22175, 97},   {23493, 98},  {24890, 99},  {26370, 100},  {27938, 101},   //< C7 to B7
    {29600, 102},  {31360, 103},  {33224, 104}, {35200, 105}, {37293, 106},  {39511, 107},   //<
    {41860, 108},  {44349, 109},  {46986, 110}, {49780, 111}, {52740, 112},  {55877, 113},   //< C8 to B8
    {59199, 114},  {62719, 115},  {66449, 116}, {70400, 117}, {74586, 118},  {79021, 119},   //<
    {83720, 120},  {88698, 121},  {93973, 122}, {99561, 123}, {105481, 124}, {111753, 125},  //< C9 to G9
    {118398, 126}, {125439, 127},                                                            //<
    {132898, 128}};                                                                          //< sentinel

static const int kDefaultActiveThresh = 300;
static const int kDefaultCorrectTime = 5;
static const int kDefaultSuppressTime = 3;
static const int kDefaultKeepTime = 3;

YuruhornSrc::NoteHistory::NoteHistory() {
    index_ = 0;
    for (int i = 0; i < HISTORY_LEN; i++) {
        history_[i] = INVALID_NOTE_NUMBER;
    }
    for (int i = 0; i < PITCH_NUM; i++) {
        pitch_[i] = 0;
    }
    for (int i = 0; i < EXP_NUM; i++) {
        exp_[i] = 0;
    }
}

void YuruhornSrc::NoteHistory::update(uint8_t note) {
    uint8_t oldest_note = history_[index_];
    uint8_t newest_note = history_[index_] = note;
    index_ = (index_ + 1) % HISTORY_LEN;

    if (oldest_note <= NOTE_NUMBER_MAX) {
        pitch_[oldest_note % PITCH_NUM]--;
        exp_[oldest_note / PITCH_NUM]--;
    }
    if (newest_note <= NOTE_NUMBER_MAX) {
        pitch_[newest_note % PITCH_NUM]++;
        exp_[newest_note / PITCH_NUM]++;
    }
}
uint8_t YuruhornSrc::NoteHistory::lookup(uint8_t note) {
    int pitch = note % PITCH_NUM;
    if (pitch_[pitch] == HISTORY_LEN) {
        // if all history has same pitch
        int exp = note / PITCH_NUM;
        for (int i = 0; i < EXP_NUM; i++) {
            // adopt majority exp
            if (exp_[i] > exp_[exp]) {
                exp = i;
            }
        }
        return exp * PITCH_NUM + pitch;
    }
    return note;
}

int YuruhornSrc::note2index(uint8_t note) {
    int ret = -1;

    for (size_t i = 0; i < freq2note_.size(); i++) {
        if (freq2note_[i].note == note) {
            ret = i;
            break;
        }
    }

    return ret;
}

uint8_t YuruhornSrc::decideNote(uint8_t note, uint16_t vol) {
    uint8_t ret = note;

    // if input volume is enough, keep making the sound
    if (note == INVALID_NOTE_NUMBER && vol >= active_thres_) {
        if (invalid_counter_ < extend_frames_) {
            invalid_counter_++;
            ret = playing_note_;
        }
    } else {
        invalid_counter_ = 0;
    }

    if (vol < active_thres_) {
        active_counter_ = 0;
        if (silent_counter_ < keep_frames_) {
            ret = playing_note_;
        }
        silent_counter_++;
    } else {
        if (active_counter_ < suppress_frames_) {
            ret = INVALID_NOTE_NUMBER;
        }
        active_counter_++;
        silent_counter_ = 0;
    }
    return ret;
}

YuruhornSrc::YuruhornSrc(Filter& filter)
    : VoiceCapture(filter),
      active_thres_(kDefaultActiveThresh),
      performance_button_enabled_(false),
      playing_note_(INVALID_NOTE_NUMBER),
      extend_frames_(kDefaultCorrectTime),
      suppress_frames_(kDefaultSuppressTime),
      keep_frames_(kDefaultKeepTime),
      invalid_counter_(0),
      active_counter_(0),
      silent_counter_(0),
      prev_note_(INVALID_NOTE_NUMBER),
      prev_btn_(false),
      history_(),
      playing_scale_(SCALE_ALL),
      max_playing_note_(NOTE_NUMBER_MAX),
      min_playing_note_(NOTE_NUMBER_MIN),
      input_level_(0),
      monitor_enabled_(false) {
    setPlayingKey(playing_scale_);
}

void YuruhornSrc::setPlayingNote(uint8_t new_playing_note) {
    playing_note_ = new_playing_note;
}

// Yuruhorn Function
void YuruhornSrc::setActiveThres(int new_active_thres) {
    active_thres_ = new_active_thres;
    return;
}

int YuruhornSrc::getActiveThres() {
    return active_thres_;
}

uint8_t YuruhornSrc::getNote(uint32_t freq, uint8_t prev) {
    uint32_t thresh;
    uint8_t note = INVALID_NOTE_NUMBER;

    // skip process when invalid freq
    if (freq <= 0) {
        return note;
    }

    int prev_idx = note2index(prev);
    debug_printf("prev = %d, prev_index = %d, input_Freq = %d", prev, prev_idx, freq);

    size_t i = 1;
    for (i = 1; i < freq2note_.size(); i++) {
        // take a wide frequency range of previous note
        if (prev < 0) {
            thresh = freq2note_[i - 1].freq + (freq2note_[i].freq - freq2note_[i - 1].freq) * NORMAL_THRSH_PERCENT / 100;
        } else if (freq > freq2note_[prev_idx].freq) {
            thresh = freq2note_[i - 1].freq + (freq2note_[i].freq - freq2note_[i - 1].freq) * UP_THRSH_PERCENT / 100;
        } else {
            thresh = freq2note_[i - 1].freq + (freq2note_[i].freq - freq2note_[i - 1].freq) * DOWN_THRSH_PERCENT / 100;
        }

        if (freq < thresh) {
            break;
        }
    }
    note = freq2note_[i - 1].note;
    debug_printf(", thresh=%d", thresh);
    if (note <= freq2note_[0].note || freq2note_[freq2note_.size() - 1].note <= note) {
        // first and last elements are invalid
        note = INVALID_NOTE_NUMBER;
    }

    history_.update(note);
    note = history_.lookup(note);

    debug_printf(", return note_num = %d\n", note);
    return note;
}

bool YuruhornSrc::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
#ifdef ENABLE_PROFILE
    digitalWrite(PIN_D17, HIGH);
#endif
    setPlayingNote(note);
    return BaseFilter::sendNoteOn(note, velocity, channel);
}

bool YuruhornSrc::sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
    setPlayingNote(INVALID_NOTE_NUMBER);
    return BaseFilter::sendNoteOff(note, velocity, channel);
}

void YuruhornSrc::setPlayingKey(int playing_key) {
    freq2note_.clear();
    bool first_push = true;

    for (size_t i = 1; i < sizeof(freq2note_all) / sizeof(freq2note_all[0]); i++) {
        int note = freq2note_all[i].note;
        if (playing_key & (1U << (note % PITCH_NUM))) {
            if (min_playing_note_ <= note && note <= max_playing_note_) {
                if (first_push) {
                    // push end mark
                    freq2note_.push_back(freq2note_all[i - 1]);
                    first_push = false;
                }
                freq2note_.push_back(freq2note_all[i]);
            }
        }
    }

    if (freq2note_.size() > 0) {
        // push end mark
        freq2note_.push_back(freq2note_all[freq2note_.back().note + 2]);
    }
    return;
}

bool YuruhornSrc::setParam(int param_id, intptr_t value) {
    if (param_id == YuruhornSrc::PARAMID_ACTIVE_LEVEL) {
        setActiveThres(value);
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE) {
        performance_button_enabled_ = value;
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_MAX_NOTE) {
        max_playing_note_ = value;
        setPlayingKey(playing_scale_);
    } else if (param_id == YuruhornSrc::PARAMID_MIN_NOTE) {
        min_playing_note_ = value;
        setPlayingKey(playing_scale_);
    } else if (param_id == YuruhornSrc::PARAMID_SCALE) {
        playing_scale_ = value;
        setPlayingKey(value);
    } else if (param_id == YuruhornSrc::PARAMID_CORRECT_FRAMES) {
        extend_frames_ = value;
    } else if (param_id == YuruhornSrc::PARAMID_SUPPRESS_FRAMES) {
        suppress_frames_ = value;
    } else if (param_id == YuruhornSrc::PARAMID_KEEP_FRAMES) {
        keep_frames_ = value;
    } else if (param_id == YuruhornSrc::PARAMID_MONITOR_ENABLE) {
        if (value) {
            printf("millis in_freq out_freq volume threshold\n");
        }
        monitor_enabled_ = value;
    } else {
        return VoiceCapture::setParam(param_id, value);
    }
    return true;
}

intptr_t YuruhornSrc::getParam(int param_id) {
    if (param_id == YuruhornSrc::PARAMID_ACTIVE_LEVEL) {
        return getActiveThres();
    } else if (param_id == YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE) {
        return performance_button_enabled_;
    } else if (param_id == YuruhornSrc::PARAMID_MAX_NOTE) {
        return max_playing_note_;
    } else if (param_id == YuruhornSrc::PARAMID_MIN_NOTE) {
        return min_playing_note_;
    } else if (param_id == YuruhornSrc::PARAMID_SCALE) {
        return playing_scale_;
    } else if (param_id == YuruhornSrc::PARAMID_CORRECT_FRAMES) {
        return extend_frames_;
    } else if (param_id == YuruhornSrc::PARAMID_SUPPRESS_FRAMES) {
        return suppress_frames_;
    } else if (param_id == YuruhornSrc::PARAMID_KEEP_FRAMES) {
        return keep_frames_;
    } else if (param_id == YuruhornSrc::PARAMID_VOLUME_METER) {
        return input_level_;
    } else if (param_id == YuruhornSrc::PARAMID_MONITOR_ENABLE) {
        return monitor_enabled_;
    } else {
        return VoiceCapture::getParam(param_id);
    }
}

int YuruhornSrc::getResult(int8_t* rcvid, VoiceCapture::Result** result, int subid) {
    int ret;

    ret = MP.Recv(rcvid, result, subid);
    return ret;
}

bool YuruhornSrc::isAvailable(int param_id) {
    if (param_id == YuruhornSrc::PARAMID_ACTIVE_LEVEL) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_PLAY_BUTTON_ENABLE) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_MAX_NOTE) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_MIN_NOTE) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_SCALE) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_CORRECT_FRAMES) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_SUPPRESS_FRAMES) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_KEEP_FRAMES) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_VOLUME_METER) {
        return true;
    } else if (param_id == YuruhornSrc::PARAMID_MONITOR_ENABLE) {
        return true;
    } else {
        return VoiceCapture::isAvailable(param_id);
    }
}

void YuruhornSrc::onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) {
    bool btn;
    uint8_t note;

    input_level_ = volume;
    digitalWrite(LED2, HIGH);

    trace_printf("freq:%d, volume:%d\n", freq_numer * 10 / freq_denom, volume);
    note = getNote(freq_numer * 10 / freq_denom, prev_note_);
    note = decideNote(note, volume);
    if (monitor_enabled_) {
        int note_freq = 0;
        if (note != INVALID_NOTE_NUMBER) {
            for (const auto& e : freq2note_) {
                if (e.note == note) {
                    note_freq = e.freq;
                }
            }
            note_freq = freq2note_[note2index(note)].freq;
        }
        printf("%d %d %d %d %d\n",                   //
               (int)(millis() % 1000),               //
               (int)(freq_numer * 10 / freq_denom),  //
               (int)note_freq,                       //
               (int)(volume / 2),                    //
               (int)(active_thres_ / 2)              //
        );                                           //
    }

    if (performance_button_enabled_) {
        btn = (digitalRead(PIN_D14) == LOW);
    } else {
        btn = true;
    }

    // Frontend's output frequency is 16kHz,
    // but pitch_detection is 8kHz as default,
    // peakFs needs to be multiply with 2.
    if (!btn) {
        note = INVALID_NOTE_NUMBER;
    }
    if (note != prev_note_) {
        if (prev_note_ != INVALID_NOTE_NUMBER) {
            sendNoteOff(prev_note_, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
        if (note != INVALID_NOTE_NUMBER) {
            sendNoteOn(note, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
        }
        prev_note_ = note;
    }
    prev_btn_ = btn;

    digitalWrite(LED2, LOW);
}

#endif  // ARDUINO_ARCH_SPRESENSE
