/**
 * File: jumping.cc
 *
 ***********************************************************************
 * Implementes functions for jumping detection.
 *
 * Copyright (C) 2026  Constantinos Argyriou
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Email: constarg@pm.me
 ***********************************************************************
 */
#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::detected_jumping_continued_sig_(const struct movement_snapshot
                                                    *movement)
{
    int32_t diff; /* Holds the difference of the current value of the
                 accelerometer and the gravetational pull of the earth. */

    diff = std::abs(movement->m_vsum_accel - GRAVITETIONAL_PULL);

    return (diff > JUMPING_CONT_SIG_THRESHOLD)
           && (movement->m_vsum_accel > GRAVITETIONAL_PULL);
}

bool
health_monitor::is_jumping(void)
{
    return this->movement_detection() == moving_state::JUMPING;
}
