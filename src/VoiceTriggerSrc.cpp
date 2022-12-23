/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "VoiceTriggerSrc.h"

#include <MP.h>

#include "VoiceCapture.h"

// Outgoing Notes
const int kSendDummyNote = 127;
// correction constant
const int kStartupSoundlessCount = 5;
const int kSoundlessCount = 2;
const int kLowVolumeThreshold = 150;
const int kHighVolumeThreshold = 10000;

// Silence or sound
enum CorrectSound {
    kStartupSound = 0,
    kActiveSound,
    kSoundless,
};

VoiceTriggerSrc::VoiceTriggerSrc(Filter& filter) : VoiceTriggerSrc(filter, DEFAULT_CHANNEL) {
}

VoiceTriggerSrc::VoiceTriggerSrc(Filter& filter, uint8_t channel)
    : VoiceCapture(filter), is_playing_(false), correct_count_(0), play_state_(kStartupSound), channel_(channel) {
}

void VoiceTriggerSrc::onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) {
    if (play_state_ == kStartupSound) {
        if (correct_count_ > kStartupSoundlessCount) {  // Perform silent correction immediately after startup
            play_state_ = kSoundless;
            correct_count_ = 0;
        } else {
            sendNoteOff(kSendDummyNote, DEFAULT_VELOCITY, channel_);
            correct_count_++;
        }
    } else {
        play_state_ = activeVoice(freq_numer / freq_denom, volume);
        if (play_state_ == kActiveSound) {
            if (!is_playing_) {
                sendNoteOn(kSendDummyNote, DEFAULT_VELOCITY, channel_);
                is_playing_ = true;
            }
        } else {
            if (is_playing_) {
                sendNoteOff(kSendDummyNote, DEFAULT_VELOCITY, channel_);
                is_playing_ = false;
            }
        }
    }

    BaseFilter::update();
}

bool VoiceTriggerSrc::correctSound() {
    bool result = false;
    if (correct_count_ <= kSoundlessCount) {  // Perform silence correction
        result = true;
        correct_count_++;
    }
    return result;
}

int VoiceTriggerSrc::activeVoice(unsigned int freq, unsigned int volume) {
    if (freq <= 0 || (volume < kLowVolumeThreshold || kHighVolumeThreshold < volume)) {  // silence
        if (!correctSound()) {                                                           // Silence correction processing
            return kSoundless;
        }
    }
    return kActiveSound;
}

#endif  // ARDUINO_ARCH_SPRESENSE
