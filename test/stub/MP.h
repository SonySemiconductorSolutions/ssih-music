/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_MP_H_
#define DUMMY_MP_H_

#include <stdint.h>

#define MP_RECV_POLLING (0xFFFFFFFF)

#define CONFIG_CXD56_CPUFIFO_NBUFFERS (8)

class MPClass {
public:
    MPClass(void);
    ~MPClass(void);
    int begin(int subid);
    int end(int subid);
    int Send(int8_t msgid, void *msgaddr, int subid);
    int Recv(int8_t *msgid, void *msgaddr, int subid);
    void RecvTimeout(uint32_t timeout);
    void GetMemoryInfo(int &usedMem, int &freeMem, int &largestFreeMem);
};

extern MPClass MP;

#endif  // DUMMY_MP_H_
