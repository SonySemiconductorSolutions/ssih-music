/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef YURUHORN_SRC_H_
#define YURUHORN_SRC_H_

#include <vector>

#include "YuruInstrumentFilter.h"
#include "VoiceCapture.h"

class YuruhornSrc : public VoiceCapture {
public:
    enum ParamId {                          // MAGIC CHAR = 'Y'
        PARAMID_ACTIVE_LEVEL = ('Y' << 8),  //<
        PARAMID_PLAY_BUTTON_ENABLE,         //<
        PARAMID_SCALE,                      //<
        PARAMID_MIN_NOTE,                   //<
        PARAMID_MAX_NOTE,                   //<
        PARAMID_CORRECT_FRAMES,             //<
        PARAMID_SUPPRESS_FRAMES,            //<
        PARAMID_KEEP_FRAMES,                //<
        PARAMID_VOLUME_METER,               //<
        PARAMID_MONITOR_ENABLE              //<
    };

    enum Scale {
        //                  B A G FE D C
        SCALE_CHROMATIC = 0b111111111111,  //< chromatic scale (4095)
        SCALE_C_MAJOR = 0b101010110101,    //< C-major scale (2741)
        SCALE_Db_MAJOR = 0b010101101011,   //< Db-major scale (1387)
        SCALE_D_MAJOR = 0b101011010110,    //< D-major scale (2774)
        SCALE_Eb_MAJOR = 0b010110101101,   //< Eb-major scale (1453)
        SCALE_E_MAJOR = 0b101101011010,    //< E-major scale (2906)
        SCALE_F_MAJOR = 0b011010110101,    //< F-major scale (1717)
        SCALE_Gb_MAJOR = 0b110101101010,   //< Gb-major scale (3434)
        SCALE_G_MAJOR = 0b101011010101,    //< G-major scale (2773)
        SCALE_Ab_MAJOR = 0b010110101011,   //< Ab-major scale (1451)
        SCALE_A_MAJOR = 0b101101010110,    //< A-major scale (2902)
        SCALE_Bb_MAJOR = 0b011010101101,   //< Bb-major scale (1709)
        SCALE_B_MAJOR = 0b110101011010,    //< B-major scale (3418)
        SCALE_ALL = SCALE_CHROMATIC
    };

    struct FreqNotePair {
        uint32_t freq;
        int note;
    };

    class NoteHistory {
    public:
        NoteHistory();
        void update(uint8_t note);
        uint8_t lookup(uint8_t note);

    private:
        static const int EXP_NUM = (NOTE_NUMBER_MAX + PITCH_NUM - 1) / PITCH_NUM;
        static const int HISTORY_LEN = 3;
        size_t index_;
        uint8_t history_[HISTORY_LEN];
        int pitch_[PITCH_NUM];
        int exp_[EXP_NUM];
    };

    YuruhornSrc(Filter& filter);

    void setPlayingNote(uint8_t new_playing_note);
    int getActiveThres();
    void setActiveThres(int new_active_thres);
    uint8_t getNote(uint32_t freq, uint8_t prev);
    int getResult(int8_t* rcvid, VoiceCapture::Result** result, int subid);
    void setPlayingKey(int playing_key);

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

protected:
    void onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) override;

private:
    int active_thres_;
    bool performance_button_enabled_;
    uint8_t playing_note_;
    int extend_frames_;
    int suppress_frames_;
    int keep_frames_;
    int invalid_counter_;
    int active_counter_;
    int silent_counter_;
    uint8_t prev_note_;
    bool prev_btn_;
    NoteHistory history_;
    std::vector<FreqNotePair> freq2note_;
    int playing_scale_;
    int max_playing_note_;
    int min_playing_note_;
    int input_level_;
    bool monitor_enabled_;

    int note2index(uint8_t note);
    uint8_t decideNote(uint8_t note, uint16_t vol);
};

#endif  // YURUHORN_SRC_H_
