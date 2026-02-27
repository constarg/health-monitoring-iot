#include <cstdio>

#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::detected_falling_continued_sig(const struct movement_snapshot
                                                   *movement)
{
    int32_t diff; /* Holds the difference of the current value of the
                     accelerometer and the gravetational pull of the earth. */

    diff = std::abs(movement->m_vsum_accel - GRAVITETIONAL_PULL);

    return (diff > FALLING_CONT_SIG_THRESHOLD)
           && (movement->m_vsum_accel > GRAVITETIONAL_PULL)
           && movement->m_vsum_gyro > FALLING_GYRO_SIG_THRESHOLD;
}

bool
health_monitor::is_falling(void)
{
    return this->movement_detection() == moving_state::FALLING;
}
