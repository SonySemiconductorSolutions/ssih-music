/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file VoiceTriggerSrc.h
 */
#ifndef VOICE_TRIGGER_SRC_H_
#define VOICE_TRIGGER_SRC_H_

#include "YuruInstrumentFilter.h"
#include "VoiceCapture.h"

/**
 * @brief @~japanese 音声が入力されたらノート・オン、終了したらSndNoteOffを発行する楽器機能です。
 */
class VoiceTriggerSrc : public VoiceCapture {
public:
    /**
     * @brief @~japanese VoiceTriggerSrc オブジェクトを生成します。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    VoiceTriggerSrc(Filter& filter);

    /**
     * @brief @~japanese VoiceTriggerSrc オブジェクトを生成します。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     * @param[in] channel channel (1 to 16)
     */
    VoiceTriggerSrc(Filter& filter, uint8_t channel);

protected:
    void onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) override;

private:
    bool is_playing_;
    int correct_count_;
    uint8_t play_state_;
    uint8_t channel_;

    bool correctSound();
    int activeVoice(unsigned int freq, unsigned int volume);
};

#endif  // VOICE_TRIGGER_SRC_H_
