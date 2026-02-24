/* mbed Microcontroller Library
 * Copyright (c) 2026 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "health_monitor.h"

int
main()
{
    iot_health_mon::health_monitor test;

    while (1) { test.read_sensors(); }

    return 0;
}
