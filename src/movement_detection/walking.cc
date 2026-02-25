#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::probably_walking(const struct movement_snapshot *movement)
{
    return walking_running_common(movement, WALKING_LOW_THRESHOLD,
                                  WALKING_HIGH_THRESHOLD);
}

bool
health_monitor::is_walking(void)
{
    return this->movement_detection() == moving_state::WALKING;
}
