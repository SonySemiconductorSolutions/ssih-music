/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef RING_BUFF_H_
#define RING_BUFF_H_

class RingBuff {
public:
    RingBuff(int sample) {
        int size = sample * sizeof(q15_t);
        top_ = (q15_t *)malloc(size);
        bottom_ = top_ + sample;
        wptr_ = rptr_ = top_;
    };
    ~RingBuff() {
        free(top_);
    };

    int put(q15_t *buf, int sample) {
        if (sample < (bottom_ - wptr_)) {
            arm_copy_q15(buf, wptr_, sample);
            wptr_ += sample;
        } else {
            int part = bottom_ - wptr_;
            arm_copy_q15(buf, wptr_, part);
            arm_copy_q15(&buf[part], top_, sample - part);
            wptr_ = top_ + sample - part;
        }
        // printf("[%4d+ %4d] (w:0x%08x+ r:0x%08x)\n", stored(), remain(),
        // (uint32_t)wptr_, (uint32_t)rptr_);
        return sample;
    };

    int put(q15_t *buf, int sample, int chnum, int ch) {
        int i;
        if (sample < (bottom_ - wptr_)) {
            for (i = 0; i < sample; i++) {
                wptr_[i] = buf[chnum * i + ch];
            }
            wptr_ += sample;
        } else {
            int part = bottom_ - wptr_;
            for (i = 0; i < part; i++) {
                wptr_[i] = buf[chnum * i + ch];
            }
            for (i = part; i < sample; i++) {
                top_[i - part] = buf[chnum * i + ch];
            }
            wptr_ = top_ + sample - part;
        }
        // printf("[%4d+ %4d] (w:0x%08x+ r:0x%08x)\n", stored(), remain(),
        // (uint32_t)wptr_, (uint32_t)rptr_);
        return sample;
    };

    int get(float *buf, int sample, int advance = 0) {
        if (advance <= 0) {
            advance = sample;
        }
        if ((rptr_ + sample) < bottom_) {
            arm_q15_to_float(rptr_, buf, sample);
            rptr_ += advance;
        } else {
            int part = bottom_ - rptr_;
            arm_q15_to_float(rptr_, buf, part);
            arm_q15_to_float(top_, &buf[part], sample - part);
            if (rptr_ + advance >= bottom_) {
                rptr_ = top_ + (rptr_ + advance - bottom_);
            } else {
                rptr_ += advance;
            }
        }
        // printf("[%4d +%4d] (w:0x%08x +r:0x%08x)\n", stored(), remain(),
        // (uint32_t)wptr_, (uint32_t)rptr_);
        return sample;
    };

    int remain() {
        return (bottom_ - top_) - stored();
    };
    int stored() {
        return (rptr_ <= wptr_) ? wptr_ - rptr_ : (bottom_ - rptr_) + (wptr_ - top_);
    };

private:
    q15_t *top_;
    q15_t *bottom_;
    q15_t *wptr_;
    q15_t *rptr_;
};

#endif  // RING_BUFF_H_
