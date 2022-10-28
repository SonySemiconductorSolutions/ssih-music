/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_MEMORY_UTIL_H_
#define DUMMY_MEMORY_UTIL_H_

#include "Arduino.h"

#define MSGQ_AUD_MGR (1)

#define SECTION_NO0 (0)
#define SECTION_NO1 (1)

#define MEM_LAYOUT_RECORDINGPLAYER (2)

typedef int MsgType;
typedef int MsgQueId;

typedef int PoolId;

int initMemoryPools(void);

int createStaticPools(uint8_t layout_no);

class MemHandle {
public:
    class Chunk {
    public:
        void* addr_;
        size_t size_;
        int ref_;
    };
    Chunk* chunk_;

    MemHandle(void);
    MemHandle(const MemHandle& rhs);
    ~MemHandle(void);
    MemHandle& operator=(const MemHandle& rhs);

    err_t allocSeg(PoolId id, size_t size_for_check);
    err_t freeSeg(void);
    void* getVa(void) const;
    void* getPa(void) const;
};

const PoolId NullPoolId = 0;
const PoolId S0_REND_PCM_BUF_POOL = 1;

#endif  // DUMMY_MEMORY_UTIL_H_
