/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_STREAM_H_
#define DUMMY_STREAM_H_

#include <stdio.h>

#include "Arduino.h"
#include "Print.h"

class Stream : public Print {
public:
    Stream(void);
    virtual int available(void) = 0;
    virtual void flush(void) = 0;
    virtual int read(void) = 0;
    String readStringUntil(char terminator);
    void setTimeout(unsigned long timeout);

protected:
    unsigned long timeout_;
};

#endif  // DUMMY_STREAM_H_
