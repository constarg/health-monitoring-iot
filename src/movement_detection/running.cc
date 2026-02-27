#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::is_running(void)
{
    return this->movement_detection() == moving_state::RUNNING;
}
