/* mbed Microcontroller Library
 * Copyright (c) 2026 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdio>

#include "health_monitor.h"
#include "mbed_thread.h"

int
main()
{
    iot_health_mon::health_monitor test;

    while (1) {
        test.read_sensors();
        if (test.is_walking()) { printf("WALKING\n"); }

        if (test.is_running()) { printf("RUNNING\n"); }
        if (test.is_stationary()) { printf("Stationary\n"); }

        thread_sleep_for(500);
    }

    return 0;
}
