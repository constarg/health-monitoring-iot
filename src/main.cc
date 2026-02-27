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
    iot_health_mon::health_monitor *test;

    test = new iot_health_mon::health_monitor();

    while (1) {
        test->read_sensors();

        if (test->is_falling()) { printf("Falling!\n"); }
        if (test->is_running()) { printf("RUNNING\n"); }
        if (test->is_walking()) { printf("WALKING\n"); }

        if (test->is_jumping()) { printf("JUMPING\n"); }
        //
        if (test->is_stationary()) { printf("Stationary\n"); }
    }

    delete test;
    return 0;
}
