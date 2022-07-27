/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_SDHCI_H_
#define DUMMY_SDHCI_H_

#include <stdio.h>

#include "Arduino.h"

#include "File.h"
#include "Storage.h"

class SDClass : public StorageClass {
public:
    SDClass(void);
    boolean begin(uint8_t dummy = 0);
};

#endif  // DUMMY_SDHCI_H_
