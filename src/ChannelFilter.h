/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file ChannelFilter.h
 */
#ifndef CHANNEL_FILTER_H_
#define CHANNEL_FILTER_H_

#include "YuruInstrumentFilter.h"

/**
 * @brief @~japanese 前段からの演奏指示をチャンネル番号でフィルタリングして後段に伝える楽器部品です。
 */
class ChannelFilter : public BaseFilter {
public:
    enum ParamId {  // MAGIC CHAR = 'C'
        /**
         * @brief [set] @~japanese 指定したチャンネル番号を有効にします。
         */
        PARAMID_ENABLE_CHANNEL = ('C' << 8),
        /**
         * @brief [set] @~japanese 指定したチャンネル番号を無効にします。
         *
         */
        PARAMID_DISABLE_CHANNEL,
        /**
         * @brief [get,set] @~japanese 有効チャンネルのビットマスクです。
         * @details @~japanese ビットマスクのビット0が1のときチャンネル番号1が有効、ビット15が1のときチャンネル番号16が有効です。
         */
        PARAMID_CHANNEL_MASK
    };

    /**
     * @brief @~japanese ChannelFilter オブジェクトを生成します。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    ChannelFilter(Filter& filter);

    /**
     * @brief @~japanese ChannelFilter オブジェクトを生成します。
     * @param[in] channel @~japanese 有効チャンネルのビットマスクを指定します。
     * @param[in] filter @~japanese 後ろにつなげる Filter オブジェクトを指定します。
     */
    ChannelFilter(uint16_t channel, Filter& filter);

    ~ChannelFilter();

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;

    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;

private:
    uint16_t channel_flags_;
};

#endif  // CHANNEL_FILTER_H_
