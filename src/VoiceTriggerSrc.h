/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef VOICE_TRIGGER_H_
#define VOICE_TRIGGER_H_

#include "YuruInstrumentFilter.h"
#include "VoiceCapture.h"

class VoiceTriggerSrc : public VoiceCapture {
public:
    VoiceTriggerSrc(Filter& filter);

protected:
    void onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) override;

private:
    bool is_playing_;
    int correct_count_;
    uint8_t play_state_;

    bool correctSound();
    int activeVoice(unsigned int freq, unsigned int volume);
};

#endif  // VOICE_TRIGGER_H_
