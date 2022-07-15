/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "ToneFilter.h"

#include <Arduino.h>

#ifdef DEBUG
#define debugPrintf printf
#else   // DEBUG
// clang-format off
#define debugPrintf(...) do {} while (0)
// clang-format on
#endif  // DEBUG

uint8_t ToneFilter::tone2Note(uint8_t note) {
    int pitch = note % PITCH_NUM;
    int diff = 0;
    switch (tone_) {
        case MUSIC_KEY_C_MAJOR:
        case MUSIC_KEY_A_MINOR:
            break;
        case MUSIC_KEY_D_MAJOR:
        case MUSIC_KEY_B_MINOR:
            if (pitch == 0 || pitch == 5) {
                debugPrintf("C->C#, F->F#\n");
                diff = 1;
            }
            break;
        case MUSIC_KEY_E_MAJOR:
        case MUSIC_KEY_CS_MINOR:
            if (pitch == 0 || pitch == 2 || pitch == 5 || pitch == 7) {
                debugPrintf("C->C#, D->D#, F->F#, G->G#\n");
                diff = 1;
            }
            break;
        case MUSIC_KEY_F_MAJOR:
        case MUSIC_KEY_D_MINOR:
            if (pitch == 11) {
                debugPrintf("B->Bb\n");
                diff = -1;
            }
            break;
        case MUSIC_KEY_G_MAJOR:
        case MUSIC_KEY_E_MINOR:
            if (pitch == 5) {
                debugPrintf("F->F#\n");
                diff = 1;
            }
            break;
        case MUSIC_KEY_A_MAJOR:
        case MUSIC_KEY_FS_MINOR:
            if (pitch == 0 || pitch == 5 || pitch == 7) {
                debugPrintf("C->C#, F->F#, G->G#\n");
                diff = 1;
            }
            break;
        case MUSIC_KEY_B_MAJOR:
        case MUSIC_KEY_GS_MINOR:
            if (pitch == 0 || pitch == 2 || pitch == 5 || pitch == 7 || pitch == 9) {
                debugPrintf("C->C#, D->D#, F->F#, G->G#, A->A#\n");
                diff = 1;
            }
            break;
        case MUSIC_KEY_Eb_MAJOR:
        case MUSIC_KEY_C_MINOR:
            if (pitch == 4 || pitch == 9 || pitch == 11) {
                debugPrintf("CMinor (E->Eb, A->Ab, B->Bb)\n");
                diff = -1;
            }
            break;
        case MUSIC_KEY_Ab_MAJOR:
        case MUSIC_KEY_F_MINOR:
            if (pitch == 2 || pitch == 4 || pitch == 9 || pitch == 11) {
                debugPrintf("D->Db, E->Eb, A->Ab, B->Bb\n");
                diff = -1;
            }
            break;
        case MUSIC_KEY_Bb_MAJOR:
        case MUSIC_KEY_G_MINOR:
            if (pitch == 4 || pitch == 11) {
                debugPrintf("E->Eb, B->Bb\n");
                diff = -1;
            }
            break;
        case MUSIC_KEY_Db_MAJOR:
        case MUSIC_KEY_Bb_MINOR:
            if (pitch == 2 || pitch == 4 || pitch == 7 || pitch == 9 || pitch == 11) {
                debugPrintf("D->Db, E->Eb, G->Gb, A->Ab, B->Bb\n");
                diff = -1;
            }
            break;
        case MUSIC_KEY_Gb_MAJOR:
        case MUSIC_KEY_Eb_MINOR:
            if (pitch == 0 || pitch == 2 || pitch == 4 || pitch == 7 || pitch == 9 || pitch == 11) {
                debugPrintf("C->Cb, D->Db, E->Eb, G->Gb, A->Ab, B->Bb\n");
                diff = -1;
            }
            break;
        default:
            break;
    }
    return note + diff;
}

ToneFilter::ToneFilter(Filter& filter) : ToneFilter(MUSIC_KEY_C_MAJOR, filter) {
}

ToneFilter::ToneFilter(int new_tone, Filter& filter) : BaseFilter(filter), tone_(new_tone) {
    setTone(tone_);
}

bool ToneFilter::isAvailable(int param_id) {
    if (param_id == ToneFilter::PARAMID_TONE) {
        return true;
    } else {
        return BaseFilter::isAvailable(param_id);
    }
}

void ToneFilter::setTone(int new_tone) {
    tone_ = constrain(new_tone, 0, MUSIC_KEY_NUM);
}

int ToneFilter::getTone() {
    return tone_;
}

bool ToneFilter::setParam(int param_id, intptr_t value) {
    if (param_id == ToneFilter::PARAMID_TONE) {
        setTone(value);
        return true;
    } else {
        return BaseFilter::setParam(param_id, value);
    }
}

intptr_t ToneFilter::getParam(int param_id) {
    if (param_id == ToneFilter::PARAMID_TONE) {
        return getTone();
    } else {
        return BaseFilter::getParam(param_id);
    }
}

bool ToneFilter::sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
    return BaseFilter::sendNoteOn(tone2Note(note), velocity, channel);
}

#endif  // ARDUINO_ARCH_SPRESENSE
