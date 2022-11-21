/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include "VoiceCapture.h"

#include <Arduino.h>

#include <File.h>
#include <FrontEnd.h>
#include <MemoryUtil.h>
#include <MP.h>
#include <SDHCI.h>

//#define DEBUG (1)

// clang-format off
#define nop(...) do {} while (0)
// clang-format on
#if defined(DEBUG)
#define trace_printf nop
#define debug_printf printf
#define error_printf printf
#else
#define trace_printf nop
#define debug_printf nop
#define error_printf printf
#endif  // if defined(DEBUG)

static const char kClassName[] = "VoiceCapture";

static const int kGainMin = 0;
static const int kGainMax = 210;
static const int kDefaultGain = 0;

static const int kInputlevelMin = 0;
static const int kInputlevelMax = 200;
static const int kDefaultInputlevel = 100;

static FrontEnd* g_frontend = FrontEnd::getInstance();
static VoiceCapture* g_voice_capture = nullptr;
String g_record_path = "Sound0.raw";

#if 0
static bool micFrontendCallback(AsMicFrontendEvent evtype, uint32_t result, uint32_t sub_result) {
    if (g_voice_capture) {
        return g_voice_capture->onMicFrontend(evtype, result, sub_result);
    }
    return true;
}
#endif

static void frontendDoneCallback(AsPcmDataParam param) {
    if (g_voice_capture) {
        g_voice_capture->onFrontendDone(param);
    }
}

VoiceCapture::VoiceCapture(Filter& filter)
    : BaseFilter(filter),
      gain_(kDefaultGain),
      input_level_(kDefaultInputlevel),
      result_(nullptr),
      capture_frames_(0),
      send_frames_(0),
      receive_frames_(0),
      is_recording_(false),
      dump_buffer_(nullptr),
      wp_(0),
      rp_(0) {
}

VoiceCapture::~VoiceCapture() {
    setParam(VoiceCapture::PARAMID_RECORDING, false);
    if (g_voice_capture == this) {
        g_voice_capture = nullptr;
    }
}

bool VoiceCapture::begin() {
    bool ok = true;
    int ret = 0;
    err_t err = FRONTEND_ECODE_OK;

    g_voice_capture = this;

    ok = BaseFilter::begin();
    if (!ok) {
        error_printf("[%s::%s] error: failed BaseFilter::begin => %d\n", kClassName, __func__, ok);
        return false;
    }

    // initialize MP
    debug_printf("[%s::%s] init MP\n", kClassName, __func__);
    ret = MP.begin(SUB_CORE_ID);
    if (ret < 0) {
        error_printf("[%s::%s] error: failed MP.begin => %d\n", kClassName, __func__, ret);
        return false;
    }
    MP.RecvTimeout(MP_RECV_POLLING);

    // initialize FrontEnd (Audio Input)
    AsDataDest callback = {0};
    callback.cb = frontendDoneCallback;

    debug_printf("[%s::%s] init capture\n", kClassName, __func__);

    err = g_frontend->begin();
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.begin => %d\n", kClassName, __func__, err);
        return false;
    }

    err = g_frontend->activate();
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.activate => %d\n", kClassName, __func__, err);
        return false;
    }

#if 0
    err = g_frontend->init(AS_CHANNEL_MONO, AS_BITLENGTH_16,
                           CAP_SAMPLE_CNT * 3,  // H/W-in:48KHz SRC-out:16KHz
                           AsDataPathCallback, callback, AsMicFrontendPreProcSrc, "/mnt/sd0/BIN/SRC");
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.init => %d\n", kClassName, __func__, err);
        return false;
    }
#else
    AsInitMicFrontendParam frontend_init;
    frontend_init.channel_number = AS_CHANNEL_MONO;
    frontend_init.bit_length = AS_BITLENGTH_16;
    frontend_init.samples_per_frame = CAP_SAMPLE_CNT * 3;
    frontend_init.preproc_type = AsMicFrontendPreProcSrc;
    strncpy(frontend_init.dsp_path, "/mnt/sd0/BIN/SRC", sizeof(frontend_init.dsp_path));
    frontend_init.data_path = AsDataPathCallback;
    frontend_init.dest = callback;
    frontend_init.out_fs = AS_SAMPLINGRATE_16000;
    ok = AS_InitMicFrontend(&frontend_init);
    if (!ok) {
        error_printf("error: failed AS_InitMicFrontend => %d\n", ok);
        return false;
    }
    AudioObjReply reply;
    ok = AS_ReceiveObjectReply(MSGQ_AUD_MGR, &reply);
    if (!ok) {
        error_printf("error: failed AS_ReceiveObjectReply => %d\n", ok);
        return false;
    }
#endif

    debug_printf("[%s::%s] start capture.\n", kClassName, __func__);

    err = g_frontend->setMicGain(gain_);
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.setMicGain => %d\n", kClassName, __func__, err);
        return false;
    }

    err = g_frontend->start();
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.start => %d\n", kClassName, __func__, err);
        return false;
    }

    debug_printf("[%s::%s] VoiceCapture: ready\n", kClassName, __func__);

    return true;
}

void VoiceCapture::update() {
    bool has_result = false;
    VoiceCapture::Result analyze_result = {0};

    int8_t rcvid = -1;
    VoiceCapture::Result* result = nullptr;
    int ret = MP.Recv(&rcvid, &result, SUB_CORE_ID);
    if (ret >= 0) {
        receive_frames_++;
        if (result) {
            analyze_result = *result;
            has_result = true;
        }
    } else if (result_) {
        analyze_result = *result_;
        has_result = true;
    }
    result_ = nullptr;
    if (has_result) {
        debug_printf("[%s::%s] send time:%d, result time:%d\n", kClassName, __func__, (int)analyze_result.capture_time, (int)analyze_result.result_time);
        debug_printf("[%s::%s] frame num:%d\n", kClassName, __func__, analyze_result.id);
        onCapture(analyze_result.freq_numer, analyze_result.freq_denom, analyze_result.volume);
    }
    BaseFilter::update();
}

bool VoiceCapture::isAvailable(int param_id) {
    if (param_id == VoiceCapture::PARAMID_MIC_GAIN) {
        return true;
    } else if (param_id == VoiceCapture::PARAMID_INPUT_LEVEL) {
        return true;
    } else if (param_id == VoiceCapture::PARAMID_RECORDING) {
        return true;
    }
    return BaseFilter::isAvailable(param_id);
}

intptr_t VoiceCapture::getParam(int param_id) {
    if (param_id == VoiceCapture::PARAMID_MIC_GAIN) {
        return gain_;
    } else if (param_id == VoiceCapture::PARAMID_INPUT_LEVEL) {
        return input_level_;
    } else if (param_id == VoiceCapture::PARAMID_RECORDING) {
        return is_recording_;
    }
    return BaseFilter::getParam(param_id);
}

bool VoiceCapture::setParam(int param_id, intptr_t value) {
    if (param_id == VoiceCapture::PARAMID_MIC_GAIN) {
        int prev = gain_;
        gain_ = constrain(value, kGainMin, kGainMax);
        if (gain_ != prev) {
            err_t err = FRONTEND_ECODE_OK;
            err = g_frontend->setMicGain(gain_);
            if (err != FRONTEND_ECODE_OK) {
                error_printf("[%s::%s] error: failed FrontEnd.setMicGain %d\n", kClassName, __func__, err);
            }
            return (err == FRONTEND_ECODE_OK);
        }
        return true;
    } else if (param_id == VoiceCapture::PARAMID_INPUT_LEVEL) {
        input_level_ = constrain(value, kInputlevelMin, kInputlevelMax);
        return true;
    } else if (param_id == VoiceCapture::PARAMID_RECORDING) {
        bool prev = is_recording_;
        is_recording_ = value ? true : false;
        if (is_recording_ != prev) {
            if (is_recording_) {
                dump_buffer_ = new uint8_t[CAP_BLK_SIZE];
                wp_ = 0;
                rp_ = 0;
            } else {
                if (dump_buffer_) {
                    delete[] dump_buffer_;
                }
                dump_buffer_ = nullptr;
                wp_ = 0;
                rp_ = 0;
            }
        }
        return true;
    }
    return BaseFilter::setParam(param_id, value);
}

#if 0
bool VoiceCapture::onMicFrontend(AsMicFrontendEvent evtype, uint32_t result, uint32_t sub_result) {
    return true;
}
#endif

void VoiceCapture::onFrontendDone(AsPcmDataParam param) {
    static uint8_t buffer[CAP_BLK_SIZE];   //< referenced from SubCore
    static VoiceCapture::Capture capture;  //< referenced from SubCore

    ledOn(LED0);

    if (param.size == CAP_BLK_SIZE) {
        memcpy(buffer, param.mh.getPa(), param.size);
        capture.data = buffer;
        capture.data = param.mh.getPa();

        capture.sample_size = 2;
        capture.channel = 1;
        capture.fs = CAP_SAMPLE_FRQ;
        capture.size = param.size;

        if (CAP_BYTE_WIDTH == 2) {
            if (input_level_ != 100) {
                int16_t* data = (int16_t*)capture.data;
                int n = CAP_SAMPLE_CNT / sizeof(data[0]);
                float ratio = (float)input_level_ / 100.0;
                for (int i = 0; i < n; i++) {
                    data[i] *= ratio;
                }
            }
        }
        capture.capture_time = millis();
        capture.id = capture_frames_++;
        capture.reserved = 0;

        if (send_frames_ - receive_frames_ < CONFIG_CXD56_CPUFIFO_NBUFFERS) {
            MP.Send(MP_MTOS_ID, &capture, SUB_CORE_ID);
            send_frames_++;
        }

        if (is_recording_ && dump_buffer_ && wp_ == 0) {
            memcpy(dump_buffer_, buffer, CAP_BLK_SIZE);
            wp_ = param.size;
            rp_ = 0;
        }
        if (rp_ < wp_) {
            printf("Capture Data:");
            for (int i = 0; i < 32; i++) {
                if (rp_ < wp_) {
                    printf(" %02X", dump_buffer_[rp_++]);
                }
            }
            printf("\n");
            if (rp_ >= wp_) {
                setParam(PARAMID_RECORDING, false);
            }
        }
    }

    ledOff(LED0);
}

#endif  // ARDUINO_ARCH_SPRESENSE
