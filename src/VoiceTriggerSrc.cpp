/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "VoiceTriggerSrc.h"

#include <MP.h>

#include "VoiceCapture.h"

//送信ノート
const int kSendDummyNote = 60;
//補正用定数
const int kStartupSoundlessCount = 5;
const int kSoundlessCount = 2;
const int kLowVolumeThreshold = 150;
const int kHighVolumeThreshold = 10000;

//無音か音ありか
enum CorrectSound {
    kStartupSound = 0,
    kActiveSound,
    kSoundless,
};

VoiceTriggerSrc::VoiceTriggerSrc(Filter& filter) : VoiceCapture(filter), is_playing_(false), correct_count_(0), play_state_(kStartupSound) {
}

bool VoiceTriggerSrc::correctSound() {
    bool result = false;
    if (correct_count_ <= kSoundlessCount) {  //無音補正を行う
        result = true;
        correct_count_++;
    }
    return result;
}

int VoiceTriggerSrc::activeVoice(unsigned int freq, unsigned int volume) {
    if (freq <= 0 || (volume < kLowVolumeThreshold || kHighVolumeThreshold < volume)) {  //無音
        if (!correctSound()) {                                                           //無音補正処理
            return kSoundless;
        }
    }
    return kActiveSound;
}

void VoiceTriggerSrc::onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) {
    if (play_state_ == kStartupSound) {
        if (correct_count_ > kStartupSoundlessCount) {  //起動直後の無音補正を行う
            play_state_ = kSoundless;
            correct_count_ = 0;
        } else {
            sendNoteOff(kSendDummyNote, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
            correct_count_++;
        }
    } else {
        play_state_ = activeVoice(freq_numer / freq_denom, volume);
        if (play_state_ == kActiveSound) {
            if (!is_playing_) {
                sendNoteOn(kSendDummyNote, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                is_playing_ = true;
            }
        } else {
            if (is_playing_) {
                sendNoteOff(kSendDummyNote, DEFAULT_VELOCITY, DEFAULT_CHANNEL);
                is_playing_ = false;
            }
        }
    }

    BaseFilter::update();
}

#endif  // ARDUINO_ARCH_SPRESENSE
