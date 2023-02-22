/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef VOICE_CAPTURE_H_
#define VOICE_CAPTURE_H_

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)

#include <pthread.h>

#include <FrontEnd.h>
#include <File.h>

#include "YuruInstrumentFilter.h"

#endif  // ARDUINO_ARCH_SPRESENSE

#define SUB_CORE_ID (1)

#define MSGID_INIT (110)
#define MSGID_INIT_DONE (113)
#define MSGID_SEND_CAPTURE (114)
#define MSGID_SEND_RESULT (115)

// pitch detector parameter
static const int PITDT_SAMPLE_FRQ = 8000;
static const int PITDT_BYTE_WIDTH = 2;
static const int PITDT_CHNL_CNT = 1;
static const int PITDT_SAMPLE_CNT = 256;

// capture parameter
static const int CAP_SAMPLE_FRQ = 16000;
static const int CAP_BYTE_WIDTH = PITDT_BYTE_WIDTH;
static const int CAP_CHNL_CNT = PITDT_CHNL_CNT;
static const int CAP_SAMPLE_CNT = PITDT_SAMPLE_CNT;
static const int CAP_BLK_SIZE = CAP_SAMPLE_CNT * CAP_BYTE_WIDTH * CAP_CHNL_CNT;

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)
class VoiceCapture : public BaseFilter {
#else
class VoiceCapture {
#endif  // ARDUINO_ARCH_SPRESENSE
public:
    struct Capture {
        unsigned int id;
        void* data;
        size_t size;
        int sample_size;
        unsigned int channel;

        unsigned int fs;

        unsigned long capture_time;
        unsigned char reserved;
    };

    struct Result {
        unsigned int id;
        unsigned int freq_numer;
        unsigned int freq_denom;
        unsigned int volume;
        unsigned long capture_time;
        unsigned long result_time;
    };

#if defined(ARDUINO_ARCH_SPRESENSE) && !defined(SUBCORE)
    enum ParamId {                      // MAGIC CHAR = 'V'
        PARAMID_MIC_GAIN = ('V' << 8),  //<
        PARAMID_INPUT_LEVEL,            //<
        PARAMID_RECORDING_TIME,         //<
        PARAMID_RECORDING               //<
    };

    class Recorder {
    public:
        enum State { RECORDER_STATE_INIT, RECORDER_STATE_RECORDING, RECORDER_STATE_FULL };
        Recorder();
        ~Recorder();

        bool start();
        bool end();
        void writeFrame(void* buffer);
        void flush();

        bool isRecording();
        int getRecordingTime();
        bool setRecordingTime(int value);

    private:
        int state_;
        File file_;
        uint8_t* buffer_;
        int frame_index_;
        int recording_time_;
        int seconds_;
    };

    VoiceCapture(Filter& filter);
    ~VoiceCapture();

    bool begin() override;
    void update() override;

    bool isAvailable(int param_id) override;
    intptr_t getParam(int param_id) override;
    bool setParam(int param_id, intptr_t value) override;
#if 0
    bool onMicFrontend(AsMicFrontendEvent evtype, uint32_t result, uint32_t sub_result);
#endif
    void onFrontendDone(AsPcmDataParam param);

protected:
    virtual void onCapture(unsigned int freq_numer, unsigned int freq_denom, unsigned int volume) = 0;

private:
    int gain_;
    int input_level_;
    Result* result_;

    size_t capture_frames_;
    size_t send_frames_;
    size_t receive_frames_;

    Recorder recorder_;
#endif  // ARDUINO_ARCH_SPRESENSE
};

#endif  // VOICE_CAPTURE_H_
