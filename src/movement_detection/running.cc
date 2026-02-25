#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::probably_running(const struct movement_snapshot *movement)
{
    return walking_running_common(movement, RUNNING_LOW_THRESHOLD,
                                  RUNNING_HIGH_THRESHOLD);
}

bool
health_monitor::is_running(void)
{
    return this->movement_detection() == moving_state::RUNNING;
}
