/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_FRONT_END_H_
#define DUMMY_FRONT_END_H_

#include <stddef.h>
#include <stdint.h>

#include "MemoryUtil.h"
#include "OutputMixer.h"

#define FRONTEND_ECODE_OK (0)
#define FRONTEND_ECODE_COMMAND_ERROR (1)
#define FRONTEND_ECODE_BASEBAND_ERROR (2)

typedef enum {
    AsDataPathCallback = 0,
    AsDataPathMessage,
    AsDataPathSimpleFIFO,
} AsMicFrontendDataPath;

typedef enum {
    AsMicFrontendPreProcThrough = 0,
    AsMicFrontendPreProcSrc,
    AsMicFrontendPreProcUserCustom,
    AsMicFrontendPreProcInvalid = 0xFF,
} AsMicFrontendPreProcType;

typedef void (*FrontendDoneCallback)(AsPcmDataParam param);

union AsDataDest {
    FrontendDoneCallback cb;
};

struct AsInitMicFrontendParam {
    uint8_t channel_number;
    uint8_t bit_length;
    uint16_t samples_per_frame;
    uint32_t out_fs;
    uint8_t preproc_type;
    char dsp_path[32];
    uint8_t data_path;
    AsDataDest dest;
};

bool AS_InitMicFrontend(AsInitMicFrontendParam* initparam);

class FrontEnd {
public:
    err_t begin(void);
    err_t activate(void);
    err_t init(uint8_t channel_number, uint8_t bit_length, uint32_t samples_per_frame, uint8_t data_path, AsDataDest dest, uint8_t preproc_type,
               const char* dsp_path);
    err_t start(void);
    err_t setMicGain(int16_t mic_gain);
    static FrontEnd* getInstance(void);

private:
    FrontEnd(void);
    FrontEnd(const FrontEnd& rhs);
    FrontEnd& operator=(const FrontEnd& rhs);
    ~FrontEnd(void);
};

#endif  // DUMMY_FRONT_END_H_
