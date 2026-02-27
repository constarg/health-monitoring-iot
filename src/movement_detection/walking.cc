#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::detected_walking_continued_sig(const struct movement_snapshot
                                                   *movement)
{
    return (std::abs(movement->m_vsum_accel - GRAVITETIONAL_PULL)
            > WALKING_CONT_SIG_THRESHOLD)
           && (movement->m_vsum_accel < GRAVITETIONAL_PULL);
}

bool
health_monitor::is_walking(void)
{
    return this->movement_detection() == moving_state::WALKING;
}
