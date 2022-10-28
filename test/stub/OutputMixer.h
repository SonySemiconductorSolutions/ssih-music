/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_OUTPUT_MIXER_H_
#define DUMMY_OUTPUT_MIXER_H_

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#include <vector>

#include "Arduino.h"

#include "MemoryUtil.h"
#include "arch/board/cxd56_audio.h"

#define OUTPUTMIXER_ECODE_OK (0)
#define OUTPUTMIXER_ECODE_COMMAND_ERROR (1)

#define OUTPUTMIXER_RNDCLK_NORMAL (0)
#define OUTPUTMIXER_RNDCLK_HIRESO (1)

#define AS_CHANNEL_MONO (1)
#define AS_CHANNEL_STEREO (2)
#define AS_BITLENGTH_16 (16)

typedef void (*PcmProcDoneCallback)(int32_t identifier, bool is_end);

typedef struct {
    int32_t identifier;
    PcmProcDoneCallback callback;
    MemHandle mh;
    uint32_t sample;
    uint32_t size;
    bool is_end;
    bool is_valid;
    uint8_t bit_length;
} AsPcmDataParam;

enum AsOutputMixDevice { HPOutputDevice = 0, I2SOutputDevice, A2dpSrcOutputDevice, OutputMixDeviceNum };

enum AsOutputMixerHandle {
    OutputMixer0 = 0,
    OutputMixer1,
};

enum AsOutputMixDoneCmdType {
    OutputMixActDone = 0,
    OutputMixInitDone,
    OutputMixDeactDone,
    OutputMixSetClkRcvDone,
    OutputMixInitPostDone,
    OutputMixSetPostDone,
    OutputMixDoneCmdTypeNum
};

struct AsOutputMixDoneParam {
    int handle;
    AsOutputMixDoneCmdType done_type;
    bool result;
    uint32_t ecode;
};

typedef struct {
    uint8_t handle;
    PcmProcDoneCallback callback;
    AsPcmDataParam pcm;
} AsSendDataOutputMixer;

typedef void (*OutputMixerCallback)(MsgQueId requester_dtq, MsgType msgtype, AsOutputMixDoneParam *param);

class OutputMixer {
public:
    err_t create(void);
    err_t activate(AsOutputMixerHandle handle, OutputMixerCallback omcb);
    err_t activate(AsOutputMixerHandle handle, uint8_t output_device, OutputMixerCallback omcb);
    err_t sendData(AsOutputMixerHandle handle, PcmProcDoneCallback pcmdone_cb, AsPcmDataParam pcm);
    err_t activateBaseband(void);
    err_t setVolume(int master, int player0, int player1);
    bool setRenderingClkMode(uint8_t clk_mode);

    static OutputMixer *getInstance(void);

    void setOutputHandler(void *(*handler)(void *arg, AsSendDataOutputMixer *data), void *arg);
    void flush(int n = -1);
    void clear();

private:
    std::vector<AsSendDataOutputMixer> queue_;
    void *(*handler_)(void *arg, AsSendDataOutputMixer *data);
    void *handler_arg_;

private:
    OutputMixer(void);
    OutputMixer(const OutputMixer &);
    OutputMixer &operator=(const OutputMixer &);
    ~OutputMixer(void);
};

#endif  // DUMMY_OUTPUT_MIXER_H_
