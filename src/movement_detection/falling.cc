#include <cstdio>

#include "health_monitor.h"
#include "moving_common.h"

using namespace iot_health_mon;

bool
health_monitor::detected_falling_continued_sig(const struct movement_snapshot
                                                   *movement)
{
    return false;
}

bool
health_monitor::is_falling(void)
{
    return this->movement_detection() == moving_state::FALLING;
}
