#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::probably_stationary(const struct movement_snapshot *movement)
{
    return !health_monitor::probably_walking(movement)
           && !health_monitor::probably_running(movement);
}

bool
health_monitor::is_stationary(void)
{
    return this->movement_detection() == moving_state::STATIONARY;
}
