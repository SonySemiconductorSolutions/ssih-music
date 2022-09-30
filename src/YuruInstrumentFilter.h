/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef YURUINSTRUMENT_FILTER_H_
#define YURUINSTRUMENT_FILTER_H_

#include <stdint.h>
#include <stddef.h>

static const uint8_t INVALID_NOTE_NUMBER = 0xFF;
static const uint8_t NOTE_NUMBER_MIN = 0x00;
static const uint8_t NOTE_NUMBER_MAX = 0x7F;
static const uint8_t DEFAULT_VELOCITY = 64;
static const uint8_t DEFAULT_CHANNEL = 0;
static const int PITCH_NUM = 12;

/// Interface of YuruInstument component
class Filter {
public:
    enum ParamId {            //
        PARAMID_OUTPUT_LEVEL  //<
    };

    virtual bool begin() = 0;
    virtual void update() = 0;
    virtual bool isAvailable(int param_id) = 0;
    virtual intptr_t getParam(int param_id) = 0;
    virtual bool setParam(int param_id, intptr_t value) = 0;
    virtual bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) = 0;
    virtual bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) = 0;
};

/// Null of YuruInstrument component
class NullFilter : public Filter {
public:
    NullFilter();
    ~NullFilter();
    bool begin() override;
    void update() override;
    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
};

/// Base class of YuruInstrument component
class BaseFilter : public Filter {
protected:
    Filter* next_filter_;

public:
    BaseFilter(Filter& filter);
    ~BaseFilter();
    bool begin() override;
    void update() override;
    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;
    bool sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) override;
    bool sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) override;
};

#endif  // YURUINSTRUMENT_FILTER_H_
