#include "health_monitor.h"

using namespace iot_health_mon;

bool
health_monitor::being_hunt_by_a_cat(void) const
{
    char sosBuff[] = "Send HELP.";

    this->hospital_direct_line_.write(&sosBuff, sizeof(sosBuff));
    return true;
}
