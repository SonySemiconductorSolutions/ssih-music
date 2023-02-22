/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <MP.h>

#include <VoiceCapture.h>

// Use CMSIS library
#define ARM_MATH_CM4
#define __FPU_PRESENT 1U
#include <arm_math.h>

#include "RingBuff.h"

// Select FFT length
// #define FFTLEN 512
#define FFTLEN 1024

// Ring buffer
#define INPUT_BUFFER (1024 * 4)
RingBuff ringbuf(INPUT_BUFFER);

// Allocate the larger heap size than default
USER_HEAP_SIZE(64 * 1024);

// Temporary buffer
float pSrc[FFTLEN];
float pDst[FFTLEN];
float tmpBuf[FFTLEN];

// Analysis parameters
const int SILENT_LEVEL = 100;
const int VOICE_VOLUME = 800;
const int HUMAN_VOICE_FREQ_MIN = 80;
const int HUMAN_VOICE_FREQ_MAX = 1100;

enum { NO_ERROR = 0, ERROR_MP_BEGIN = 2, ERROR_MP_SEND, ERROR_LIB_INIT, ERROR_LIB_SET };

void errorStop(int num) {
    const int BLINK_TIME_MS = 300;
    const int INTERVAL_MS = 1000;
    while (true) {
        for (int i = 0; i < num; i++) {
            ledOn(LED1);
            delay(BLINK_TIME_MS);
            ledOff(LED1);
            delay(BLINK_TIME_MS);
        }
        delay(INTERVAL_MS);
    }
}

uint16_t analyzeVolume(const int16_t *input, int length) {
    uint32_t sum = 0;
    int count = 0;

    for (int i = 0; i < length; i++) {
        if (input[i] < -SILENT_LEVEL) {
            sum = sum - input[i];
            count++;
        } else if (input[i] > SILENT_LEVEL) {
            sum = sum + input[i];
            count++;
        }
    }

    if (count > 0) {
        return (uint16_t)(sum / count);
    } else {
        return 0;
    }
}

void setup() {
    int ret = 0;

    ret = MP.begin();
    if (ret < 0) {
        MPLog("error: MP.begin => %d\n", ret);
        errorStop(ERROR_MP_BEGIN);
    }
    MP.RecvTimeout(MP_RECV_BLOCKING);

    while (true) {
        int8_t rcvid = -1;
        uint32_t msgdata = 0;
        ret = MP.Recv(&rcvid, &msgdata);
        if (rcvid == MSGID_INIT) {
            ret = MP.Send(MSGID_INIT_DONE, nullptr);
            if (ret < 0) {
                MPLog("MP.Send = %d\n", ret);
                errorStop(ERROR_MP_SEND);
            }
            break;
        }
    }

    MPLog("setup done\n");
}

void loop() {
    uint16_t input_level = 0;

    int8_t rcvid = -1;
    VoiceCapture::Capture *capture = nullptr;
    int ret = 0;

    ret = MP.Recv(&rcvid, &capture);
    if (ret < 0) {
        MPLog("MP.Recv = %d\n", ret);
        return;
    }
    if (rcvid != MSGID_SEND_CAPTURE) {
        MPLog("received = %d\n", rcvid);
        return;
    }
    if (capture == nullptr) {
        MPLog("received invalid data\n", rcvid);
        return;
    }

    ledOn(LED1);
    input_level = analyzeVolume((int16_t *)capture->data, CAP_SAMPLE_CNT);
    ringbuf.put((q15_t *)capture->data, CAP_SAMPLE_CNT);

    while (ringbuf.stored() >= FFTLEN) {
        float peak = fftProcessing();
        static VoiceCapture::Result result = {0};

        result.id = capture->id;
        result.freq_numer = peak * capture->fs;
        result.freq_denom = CAP_SAMPLE_FRQ;

        if (VOICE_VOLUME < input_level) {
            result.volume = input_level;
        } else {
            result.freq_numer = 0.0f;
            result.volume = 0;
        }

        result.capture_time = capture->capture_time;
        result.result_time = millis();

        if (capture->reserved == 1) {
            MPLog("frame=%d, capture_time=%d, freq=%d/%d, volume=%d, result_time=%d\n",  //
                  result.id,                                                             // frame
                  result.capture_time,                                                   // capture_time
                  result.freq_numer, result.freq_denom,                                  // freq (numer/denom)
                  result.volume,                                                         // volume
                  result.result_time);                                                   // result_time
        }

        MP.Send(MSGID_SEND_RESULT, &result);
    }
    ledOff(LED1);
}

float fftProcessing() {
    int i;
    float peakFs = 0.0f;

    // Read from the ring buffer
    ringbuf.get(pSrc, FFTLEN, CAP_SAMPLE_CNT);

    // Calculate FFT
    fft(pSrc, pDst, FFTLEN);

    // Peak
    peakFs = getPeakFrequency(pDst, FFTLEN);

    if (peakFs < HUMAN_VOICE_FREQ_MIN || HUMAN_VOICE_FREQ_MAX < peakFs) {
        peakFs = 0;
    }
    // printf("peakFs:%8.3f\n", peakFs);
    return peakFs;
}

void fft(float *pSrc, float *pDst, int fftLen) {
    arm_rfft_fast_instance_f32 S;

#if (FFTLEN == 512)
    arm_rfft_512_fast_init_f32(&S);
#elif (FFTLEN == 1024)
    arm_rfft_1024_fast_init_f32(&S);
#endif

    // calculation
    arm_rfft_fast_f32(&S, pSrc, tmpBuf, 0);

    arm_cmplx_mag_f32(&tmpBuf[2], &pDst[1], fftLen / 2 - 1);
    pDst[0] = tmpBuf[0];
    pDst[fftLen / 2] = tmpBuf[1];
}

float getPeakFrequency(float *pData, int fftLen) {
    float g_fs = (float)CAP_SAMPLE_FRQ;
    uint32_t index;
    float maxValue;
    float delta;
    float peakFs;

    arm_max_f32(pData, fftLen / 2, &maxValue, &index);

    delta = 0.5 * (pData[index - 1] - pData[index + 1]) / (pData[index - 1] + pData[index + 1] - (2.0f * pData[index]));
    peakFs = (index + delta) * g_fs / (fftLen - 1);

    return peakFs;
}
