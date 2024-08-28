/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

/**
 * @file YuruInstrumentConfig.h
 */
#ifndef YURU_INSTRUMENT_CONFIG_H_
#define YURU_INSTRUMENT_CONFIG_H_

#include <vector>

#include <Arduino.h>

#include "YuruInstrumentFilter.h"

#define EMMC_USE

/**
 * @brief @~japanese Filter のパラメータ設定を行うコンフィグファイルを実現するクラスです。
 */
class YuruInstrumentConfig {
public:
    enum ErrorCode { kNoError = 0, kErrPerm, kErrNoEnt, kErrAccess, kErrExist, kErrNoDev, kErrNotDir, kErrIsDir, kErrInval };
    enum ParamType { kIntegerValue, kIntegerValueReadOnly, kStringValue, kStringValueReadOnly };
    struct ParamSpec {
        String name;
        int id;
        ParamType type;
    };

    /**
     * @brief @~japanese YuruInstrumentConfig オブジェクトを生成します。
     * @param[in] filter Filter object
     */
    YuruInstrumentConfig(Filter &filter);

    /**
     * @brief @~japanese YuruInstrumentConsole オブジェクトを破棄します。
     */
    ~YuruInstrumentConfig();

    /**
     * @brief @~japanese 関連付けられている Filter オブジェクトを取得します。
     * @return Filter object
     */
    Filter *getFilter();

    /**
     * @brief @~japanese YuruConfigConfig オブジェクトが扱えるパラメータのリストをコンソールに出力します。
     * @retval 0 no error
     * @retval otherwise error
     */
    int printParamList();

    /**
     * @brief @~japanese 指定したパラメータが設定可能かどうかをコンソールに出力します。
     * @param[in] param_str parameter ID or parameter name
     * @retval 0 no error
     * @retval otherwise error
     */
    int printAvailable(const char *param_str);

    /**
     * @brief @~japanese 指定したパラメータの値をコンソールに出力します。
     * @param[in] param_str parameter ID or parameter name
     * @retval 0 no error
     * @retval otherwise error
     */
    int printParam(const char *param_str);

    /**
     * @brief @~japanese パラメータIDにパラメータ名を設定します。
     * @param[in] name parameter name
     * @param[in] id parameter ID
     * @param[in] readonly read only flag
     * @retval 0 no error
     * @retval otherwise error
     */
    int registerParam(const char *name, int id, bool readonly = false);

    /**
     * @brief @~japanese パラメータIDにパラメータ名を設定します。このパラメータには数値ではなく文字列を扱います。
     * @param[in] name parameter name
     * @param[in] id parameter ID
     * @param[in] readonly read only flag
     * @retval 0 no error
     * @retval otherwise error
     */
    int registerStringParam(const char *name, int id, bool readonly = false);

    /**
     * @brief @~japanese パラメータを設定します。
     * @param[in] param_str parameter ID or parameter name
     * @param[in] value_str value string
     * @retval 0 no error
     * @retval otherwise error
     */
    int setParam(const char *param_str, const char *value_str);

    /**
     * @brief @~japanese コンフィグファイルからパラメータを読み込んで Filter オブジェクトに設定します。
     * @param[in] path config file path
     * @retval 0 no error
     * @retval otherwise error
     */
    int loadFromFile(const char *path);

    /**
     * @brief @~japanese Filter オブジェクトに設定されているパラメータをコンフィグファイルに書き出します。
     * @param[in] path config file path
     * @retval 0 no error
     * @retval otherwise error
     */
    int saveToFile(const char *path);

private:
    Filter *filter_;
    std::vector<ParamSpec> param_specs_;
};

#endif  // YURU_INSTRUMENT_CONFIG_H_
