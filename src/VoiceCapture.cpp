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

// #define DEBUG (1)

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

static const char kRecordFilePath[] = "Sound.raw";
static const int kFramePerSecond = CAP_SAMPLING_FREQ / CAP_FRAME_LENGTH;
static const int kDefaultRecordingSeconds = 60;  // 1min

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

VoiceCapture::Recorder::Recorder()
    : state_(RECORDER_STATE_INIT), file_(), buffer_(nullptr), frame_index_(0), recording_time_(kDefaultRecordingSeconds), seconds_(0) {
}

VoiceCapture::Recorder::~Recorder() {
    end();
    if (file_) {
        file_.close();
    }
    if (buffer_) {
        delete[] buffer_;
    }
}

bool VoiceCapture::Recorder::start() {
    if (state_ != RECORDER_STATE_INIT) {
        error_printf("[%s::%s]: already recoding.\n", kClassName, __func__);
        return false;
    }

    if (file_) {
        file_.close();
    }
    if (buffer_ == nullptr) {
        buffer_ = new uint8_t[CAP_BLOCK_SIZE * kFramePerSecond];
    }

    SDClass sdcard;
    if (!sdcard.begin()) {
        error_printf("[%s::%s]: SD begin error.\n", kClassName, __func__);
        return false;
    }
    if (sdcard.exists(kRecordFilePath)) {
        sdcard.remove(kRecordFilePath);
    }
    file_ = sdcard.open(kRecordFilePath, FILE_WRITE);
    if (!file_) {
        error_printf("[%s::%s] error: open error '%s'.\n", kClassName, __func__, kRecordFilePath);
        return false;
    }

    frame_index_ = 0;
    seconds_ = 0;
    state_ = RECORDER_STATE_RECORDING;
    ledOn(LED3);

    return true;
}

bool VoiceCapture::Recorder::end() {
    if (state_ == RECORDER_STATE_INIT) {
        return true;
    }

    if (state_ == RECORDER_STATE_RECORDING) {
        file_.write(buffer_, CAP_BLOCK_SIZE * frame_index_);
    } else if (state_ == RECORDER_STATE_FULL) {
        flush();
    }
    if (file_) {
        file_.close();
    }
    state_ = RECORDER_STATE_INIT;
    ledOff(LED3);

    return true;
}

void VoiceCapture::Recorder::writeFrame(void* buffer) {
    if (state_ == RECORDER_STATE_RECORDING) {
        memcpy(&buffer_[CAP_BLOCK_SIZE * frame_index_], buffer, CAP_BLOCK_SIZE);
        frame_index_++;
        if (kFramePerSecond <= frame_index_) {
            state_ = RECORDER_STATE_FULL;
        }
    }
}

void VoiceCapture::Recorder::flush() {
    if (state_ == RECORDER_STATE_FULL) {
        file_.write(buffer_, CAP_BLOCK_SIZE * frame_index_);
        frame_index_ = 0;
        seconds_++;
        if (seconds_ < recording_time_) {
            state_ = RECORDER_STATE_RECORDING;
        } else {
            file_.close();
            state_ = RECORDER_STATE_INIT;
            ledOff(LED3);
        }
    }
}

bool VoiceCapture::Recorder::isRecording() {
    return (state_ != RECORDER_STATE_INIT);
}

int VoiceCapture::Recorder::getRecordingTime() {
    return recording_time_;
}

bool VoiceCapture::Recorder::setRecordingTime(int value) {
    if (0 < value) {
        recording_time_ = value;
        return true;
    }
    return false;
}

VoiceCapture::VoiceCapture(Filter& filter)
    : BaseFilter(filter),
      gain_(kDefaultGain),
      input_level_(kDefaultInputlevel),
      result_(nullptr),
      capture_frames_(0),
      send_frames_(0),
      receive_frames_(0),
      recorder_() {
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

    debug_printf("[%s::%s] BaseFilter::begin\n", kClassName, __func__);
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

    // setup SubCore
    debug_printf("[%s::%s] setup SubCore\n", kClassName, __func__);
    MP.RecvTimeout(MP_RECV_BLOCKING);
    MP.Send(MSGID_INIT, nullptr, SUB_CORE_ID);
    while (true) {
        int8_t rcvid = -1;
        uint32_t msgdata = 0;
        MP.Recv(&rcvid, &msgdata, SUB_CORE_ID);
        debug_printf("[%s::%s]: receive %d, %d\n", kClassName, __func__, rcvid, msgdata);
        if (rcvid == MSGID_INIT_DONE) {
            debug_printf("[%s::%s]: MSGID_INIT_DONE\n", kClassName, __func__);
            break;
        }
    }
    MP.RecvTimeout(MP_RECV_POLLING);

    // initialize FrontEnd (Audio Input)
    AsDataDest callback = {0};
    callback.cb = frontendDoneCallback;

    debug_printf("[%s::%s] FrontEnd::begin\n", kClassName, __func__);
    err = g_frontend->begin();
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.begin => %d\n", kClassName, __func__, err);
        return false;
    }

    debug_printf("[%s::%s] FrontEnd::activate\n", kClassName, __func__);
    err = g_frontend->activate();
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.activate => %d\n", kClassName, __func__, err);
        return false;
    }

#if MIC_SAMPLING_FREQ == CAP_SAMPLING_FREQ
    // through input
    debug_printf("[%s::%s] FrontEnd::init\n", kClassName, __func__);
    err = g_frontend->init(AS_CHANNEL_MONO, AS_BITLENGTH_16, MIC_FRAME_LENGTH, AsDataPathCallback, callback);
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.init => %d\n", kClassName, __func__, err);
        return false;
    }
#else   // MIC_SAMPLING_FREQ == CAP_SAMPLING_FREQ
    // enable SRC
    debug_printf("[%s::%s] AS_InitMicFrontend\n", kClassName, __func__);
    AsInitMicFrontendParam frontend_init;
    frontend_init.channel_number = AS_CHANNEL_MONO;
    frontend_init.bit_length = AS_BITLENGTH_16;
    frontend_init.samples_per_frame = MIC_FRAME_LENGTH;
    frontend_init.preproc_type = AsMicFrontendPreProcSrc;
    strncpy(frontend_init.dsp_path, "/mnt/sd0/BIN/SRC", sizeof(frontend_init.dsp_path));
    frontend_init.data_path = AsDataPathCallback;
    frontend_init.dest = callback;
    frontend_init.out_fs = CAP_SAMPLING_FREQ;
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
#endif  // MIC_SAMPLING_FREQ == CAP_SAMPLING_FREQ

    debug_printf("[%s::%s] FrontEnd::setMicGain\n", kClassName, __func__);
    err = g_frontend->setMicGain(gain_);
    if (err != FRONTEND_ECODE_OK) {
        error_printf("[%s::%s] error: failed FrontEnd.setMicGain => %d\n", kClassName, __func__, err);
        return false;
    }

    debug_printf("[%s::%s] FrontEnd::start\n", kClassName, __func__);
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
    VoiceCapture::Result analyze_result = {0, 0, 0, 0, 0, 0};

    recorder_.flush();

    int8_t rcvid = -1;
    VoiceCapture::Result* result = nullptr;
    int ret = MP.Recv(&rcvid, &result, SUB_CORE_ID);
    if (ret >= 0) {
        if (rcvid == MSGID_SEND_RESULT) {
            receive_frames_++;
            if (result) {
                analyze_result = *result;
                has_result = true;
            }
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
    } else if (param_id == VoiceCapture::PARAMID_RECORDING_TIME) {
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
    } else if (param_id == VoiceCapture::PARAMID_RECORDING_TIME) {
        return recorder_.getRecordingTime();
    } else if (param_id == VoiceCapture::PARAMID_RECORDING) {
        return recorder_.isRecording();
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
    } else if (param_id == VoiceCapture::PARAMID_RECORDING_TIME) {
        return recorder_.setRecordingTime((int)value);
    } else if (param_id == VoiceCapture::PARAMID_RECORDING) {
        bool prev = recorder_.isRecording();
        bool recording = value ? true : false;
        if (recording != prev) {
            if (recording) {
                recorder_.start();
            } else {
                recorder_.end();
            }
        }
        return true;
    }
    return BaseFilter::setParam(param_id, value);
}

#if 0
bool RecodeVoiceCapture::onMicFrontend(AsMicFrontendEvent evtype, uint32_t result, uint32_t sub_result) {
    return true;
}
#endif

void VoiceCapture::onFrontendDone(AsPcmDataParam param) {
    static uint8_t buffer[CAP_BLOCK_SIZE];  //< referenced from SubCore
    static VoiceCapture::Capture capture;   //< referenced from SubCore

    ledOn(LED0);

    if (param.size > 0) {
        memcpy(buffer, param.mh.getPa(), param.size);
        capture.data = buffer;
        capture.data = param.mh.getPa();

        recorder_.writeFrame(&buffer);

        capture.sample_size = CAP_BYTE_WIDTH;
        capture.channel = CAP_CHANNELS;
        capture.fs = CAP_SAMPLING_FREQ;
        capture.size = param.size;

        if (CAP_BYTE_WIDTH == 2) {
            if (input_level_ != 100) {
                int16_t* data = (int16_t*)capture.data;
                int n = CAP_FRAME_LENGTH / sizeof(data[0]);
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
            MP.Send(MSGID_SEND_CAPTURE, &capture, SUB_CORE_ID);
            send_frames_++;
        }
    }

    ledOff(LED0);
}

#endif  // ARDUINO_ARCH_SPRESENSE
