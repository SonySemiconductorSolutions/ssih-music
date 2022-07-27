/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef TONE_FILTER_H_
#define TONE_FILTER_H_

#include "YuruInstrumentFilter.h"

class ToneFilter : public BaseFilter {
public:
    enum ParamId {                  // MAGIC CHAR = 'T'
        PARAMID_TONE = ('T' << 8),  //<
    };

    enum MusicKey {
        MUSIC_KEY_C_MAJOR = 0,
        MUSIC_KEY_Db_MAJOR,
        MUSIC_KEY_D_MAJOR,
        MUSIC_KEY_Eb_MAJOR,
        MUSIC_KEY_E_MAJOR,
        MUSIC_KEY_F_MAJOR,
        MUSIC_KEY_Gb_MAJOR,
        MUSIC_KEY_G_MAJOR,
        MUSIC_KEY_Ab_MAJOR,
        MUSIC_KEY_A_MAJOR,
        MUSIC_KEY_Bb_MAJOR,
        MUSIC_KEY_B_MAJOR,
        MUSIC_KEY_C_MINOR,
        MUSIC_KEY_CS_MINOR,
        MUSIC_KEY_D_MINOR,
        MUSIC_KEY_Eb_MINOR,
        MUSIC_KEY_E_MINOR,
        MUSIC_KEY_F_MINOR,
        MUSIC_KEY_FS_MINOR,
        MUSIC_KEY_G_MINOR,
        MUSIC_KEY_GS_MINOR,
        MUSIC_KEY_A_MINOR,
        MUSIC_KEY_Bb_MINOR,
        MUSIC_KEY_B_MINOR,
        MUSIC_KEY_NUM
    };

    ToneFilter(Filter& filter);

    ToneFilter(int new_tone, Filter& filter);

    void setTone(int new_tone);

    int getTone();

    bool isAvailable(int param_id) override;

    intptr_t getParam(int param_id) override;

    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

private:
    int tone_;  //< value of MusicKey

    uint8_t tone2Note(uint8_t note);
};

#endif  // TONE_FILTER_H_
