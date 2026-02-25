#include <cstdio>

#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::probably_falling(const struct movement_snapshot *movement)
{
    int32_t gyroscope;  /* Holds the sum of X, Y and Z AXIS for the
                           Gyroscope. */
    bool    is_falling; /* Indicates whether the user is falling. */

    gyroscope = std::abs(movement->m_vsum_gyro);

    is_falling = gyroscope > FALLING_GYRO_THRESHOLD;
    return is_falling;
}

bool
health_monitor::is_falling(void)
{
    return this->movement_detection() == moving_state::FALLING;
}
