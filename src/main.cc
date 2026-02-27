/* mbed Microcontroller Library
 * Copyright (c) 2026 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdio>

#include "health_monitor.h"
#include "mbed_thread.h"
#include "moving_common.h"

/* ACTIVE_MOVE_ITERATIONS specifies the number of
   iterations in which the last detected move will
   be kept, unless an other move, except the STATIONARY
   appeared. */
#define MAX_ACTIVE_MOVE_ITERATIONS (10)

int
main()
{
    iot_health_mon::health_monitor *monitor; /* The monitor system. */

    enum iot_health_mon::moving_state prev_move; /* The previously detected
                                                    move. */
    enum iot_health_mon::moving_state curr_move; /* The currently detected
                                                    move. */
    int move_iterations; /* The number of iteration the current move is
                            active. */

    bool change_move; /* Indicates whether the monitor system should
                                change the currently active move. */

    monitor         = new iot_health_mon::health_monitor();
    move_iterations = 0;

    change_move = false;
    prev_move   = iot_health_mon::STATIONARY;
    curr_move   = iot_health_mon::STATIONARY;

    while (true) {
        monitor->read_sensors();

        if (monitor->is_falling()) {
            monitor->set_active_move(iot_health_mon::FALLING);
        }

        if (monitor->is_running()) {
            monitor->set_active_move(iot_health_mon::RUNNING);
        }

        if (monitor->is_walking()) {
            monitor->set_active_move(iot_health_mon::WALKING);
        }

        if (monitor->is_jumping()) {
            monitor->set_active_move(iot_health_mon::JUMPING);
        }

        /* If the move is NON stationary, then apply directly the
           move as active move BUT, if the move is stationary,
           this move shoudn't overide the existing active move,
           except in the case where the move is expired. */
        curr_move = monitor->get_active_move();
        if (monitor->is_stationary()) {
            curr_move = iot_health_mon::STATIONARY;
        }

        /* The currently active move should change ONLY if the current
           active move reached the maximum alive iterations, or if there
           is another move that is NOT the stationary move. */
        change_move = MAX_ACTIVE_MOVE_ITERATIONS == move_iterations
                      || (prev_move != curr_move
                          && curr_move != iot_health_mon::STATIONARY);

        if (change_move) {
            monitor->set_active_move(curr_move);
            prev_move = curr_move;
        }

        /* Reset the alive time of the move, if the same
           move was detected. */
        if (curr_move == prev_move) { move_iterations = 0; }

        move_iterations++;

        /* Change the previous value, ONLY if the previous
           value is expired OR is another move that is NOT
           the stationary. */
        prev_move = (curr_move == iot_health_mon::STATIONARY
                     && move_iterations != MAX_ACTIVE_MOVE_ITERATIONS)
                        ? prev_move
                        : curr_move;

        // thread_sleep_for(monitor->get_sampling_rate());

        switch (prev_move) {
            case iot_health_mon::STATIONARY:
                printf("STATIONARY\n");
                break;
            case iot_health_mon::WALKING:
                printf("WALKING\n");
                break;
            case iot_health_mon::RUNNING:
                printf("RUNNING\n");
                break;
            case iot_health_mon::JUMPING:
                printf("JUMPING\n");
                break;
            case iot_health_mon::FALLING:
                printf("FALLING\n");
                break;
            default:
                break;
        }
    }

    delete monitor;
    return EXIT_SUCCESS;
}
