/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation
 */

/**
 * @file TimeKeeper.h
 */
#ifndef TIME_KEEPER_H_
#define TIME_KEEPER_H_

#include <stdint.h>

/**
 * @brief @~japanese スタンダードMIDIファイルの時刻計算を行います。
 */
class TimeKeeper {
public:
    /**
     * @brief @~japanese TimeKeeper オブジェクトを生成します。
     * @details @~japanese division, Tempo, 現在時刻(ミリ秒), 楽譜内時間(ミリ秒), MIDI Time Codeの相互変換をします。
     */
    TimeKeeper();

    /**
     * @brief @~japanese 内部の時刻情報をリセットします。
     * @param[in] division @~japanese スタンダードMIDIファイルで定義されるdivision値
     * @param[in] tempo @~japanese 1拍あたりの時間(マイクロ秒)
     */
    void reset(uint16_t division, uint32_t tempo);

    /**
     * @brief @~japanese division値を取得します。
     * @return @~japanese division値
     */
    uint16_t getDivision();

    /**
     * @brief @~japanese division値を設定します。
     * @param[in] division @~japanese division値
     */
    void setDivision(uint16_t division);

    /**
     * @brief @~japanese 1拍あたりの時間(マイクロ秒)を取得します。
     * @return @~japanese 1拍あたりの時間(マイクロ秒)
     */
    uint32_t getTempo();

    /**
     * @brief @~japanese 1拍あたりの時間(マイクロ秒)を設定します。
     * @param[in] @~japanese 1拍あたりの時間(マイクロ秒)
     */
    void setTempo(uint32_t tempo);

    /**
     * @brief @~japanese 楽譜の先頭から現在までの累積時間(tick)を取得する。
     * @return Cumulative tick time
     */
    uint32_t getTotalTick();

    /**
     * @brief @~japanese 時間(tick)を歩進させます。
     * @param[in] delta_time
     */
    void forward(uint32_t delta_time);

    /**
     * @brief @~japanese 実時間タイマーを開始します。
     */
    void startSmfTimer();

    /**
     * @brief @~japanese 実時間タイマーを一時停止します。MIDIメッセージの待ち時間の残りを退避します。
     */
    void stopSmfTimer();

    /**
     * @brief @~japanese 実時間タイマーを再開します。MIDIメッセージの待ち時間を再設定します。
     */
    void continueSmfTimer();

    /**
     * @brief @~japanese MIDIメッセージの待ち時間を設定します。
     * @param[in] duration @~japanese 待ち時間(ミリ秒)
     */
    void setSmfDuration(unsigned long duration);

    /**
     * @brief @~japanese 実時間を更新します。
     */
    void setCurrentTime();

    /**
     * @brief @~japanese タイマーを設定します。指定した時刻が歩進すると TimeKeeper::isScheduledTime() が true を返すようになります。
     * @param[in] delta_time
     */
    void setScheduleTime(uint32_t delta_time);

    /**
     * @brief @~japanese 指定した時間(ミリ秒)が次のMIDIイベントの発火時刻より前かを判定します。
     * @param[in] target_ms target time(ms)
     * @retval true Before
     * @retval false After
     */
    bool isBeforeScheduledMs(unsigned long target_ms);

    /**
     * @brief @~japanese 次のMIDIイベントの発火時刻を指定した時間(ミリ秒)に更新します。
     * @param target_ms target time(ms)
     */
    void rescheduleTime(unsigned long target_ms);

    /**
     * @brief @~japanese 次のMIDIイベントの発火時刻を迎えたかを判定します。
     * @retval true Reached
     * @retval false Unreached
     */
    bool isScheduledTime();

    /**
     * @brief @~japanese 時間(tick)をミリ秒に変換します。
     * @param[in] delta_time Tick time(tick)
     * @return time(ms)
     */
    unsigned long calculateDurationMs(uint32_t delta_time);

    /**
     * @brief @~japanese 楽譜の先頭から指定時間(tick)までの累積時間(ミリ秒)を計算する。
     * @param[in] delta_time Tick time(tick)
     * @return time(ms)
     */
    unsigned long calculateCurrentMs(uint32_t delta_time);

    /**
     * @brief @~japanese MIDIビートをtickに変換します。
     * @param[in] beats MIDI beat
     * @return Tick time
     */
    uint32_t midiBeatToTick(uint16_t beats);

    /**
     * @brief @~japanese tickを実時間に変換します。
     * @param[in] division @~japanese division値
     * @param[in] tempo @~japanese 1拍あたりの時間(マイクロ秒)
     * @param[in] delta_time @~japanese Tick time(tick)
     * @return @~japanese 実時間(ミリ秒)
     */
    unsigned long deltaTimeToMs(uint16_t division, uint32_t tempo, uint32_t delta_time);

    /**
     * @brief @~japanese MIDI TimeCodeを実時間に変換します。
     * @param[in] hr @~japanese 時
     * @param[in] mn @~japanese 分
     * @param[in] sc @~japanese 秒
     * @param[in] fr @~japanese フレーム
     * @return @~japanese 実時間(ミリ秒)
     */
    unsigned long mtcToMs(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr);

    /**
     * @brief @~japanese MIDI TimeCodeをフレーム数に変換する。
     * @param[in] hr @~japanese 時
     * @param[in] mn @~japanese 分
     * @param[in] sc @~japanese 秒
     * @param[in] fr @~japanese フレーム
     * @return @~japanese フレーム数
     */
    unsigned long mtcToFrames(uint8_t hr, uint8_t mn, uint8_t sc, uint8_t fr);

private:
    uint16_t division_;
    uint32_t tempo_;
    uint32_t total_tick_;
    uint32_t reference_tick_;
    unsigned long reference_ms_;

    uint32_t prev_delta_time_;
    unsigned long current_time_;
    unsigned long start_time_;
    unsigned long prev_time_;
    unsigned long schedule_time_;
    unsigned long duration_;
};

#endif  // TIME_KEEPER_H_
