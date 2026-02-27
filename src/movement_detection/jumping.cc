#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::detected_jumping_continued_sig(const struct movement_snapshot
                                                   *movement)
{
    return (std::abs(movement->m_vsum_accel - GRAVITETIONAL_PULL)
            > JUMPING_CONT_SIG_THRESHOLD)
           && (movement->m_vsum_accel > GRAVITETIONAL_PULL);
}

bool
health_monitor::is_jumping(void)
{
    return this->movement_detection() == moving_state::JUMPING;
}
