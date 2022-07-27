/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <Arduino.h>

#include <MP.h>

#include <VoiceCapture.h>

// Use CMSIS library
#define ARM_MATH_CM4
#define __FPU_PRESENT 1U
#include <arm_math.h>

#include "RingBuff.h"

// Select FFT length
//#define FFTLEN 512
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

void setup() {
    int ret = 0;

    // Initialize MP library
    ret = MP.begin();
    if (ret < 0) {
        errorLoop(2);
    }
    // receive with non-blocking
    MP.RecvTimeout(MP_RECV_POLLING);
}

void loop() {
    int ret;
    int8_t msgid;
    VoiceCapture::Capture *capture;

    uint16_t input_level = 0;

    // Receive PCM captured buffer from MainCore
    ret = MP.Recv(&msgid, &capture);
    if (ret >= 0) {
        ledOn(LED1);
        input_level = analyzeVolume((int16_t *)capture->data, CAP_SAMPLE_CNT);
        ringbuf.put((q15_t *)capture->data, CAP_SAMPLE_CNT);
    }

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
            MPLog("Frame %d (Send:%d) => freq_numer:%4d freq_denom:%4d volume:%4d (Return:%d)\n", result.id, result.capture_time, result.freq_numer,
                  result.freq_denom, result.volume, result.result_time);
        }

        MP.Send(MP_STOM_ID, &result);
    }
    ledOff(LED1);
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

void errorLoop(int num) {
    int i;

    while (1) {
        for (i = 0; i < num; i++) {
            ledOn(LED0);
            delay(300);
            ledOff(LED0);
            delay(300);
        }
        delay(1000);
    }
}
