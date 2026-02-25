#ifndef IOT_MSC_APPLICATION_H
#define IOT_MSC_APPLICATION_H

#include <mbed.h>
#include <stdint.h>

#include "moving_common.h"

namespace iot_health_mon {

#define SENSORS_RECORD_DEPTH (4)

#define HOSPITAL_BAUD_RATE (9600)
#define HOSPITAL_BAUD_BITS (8)
#define HOSPITAL_STOP_BIT  (1)

class health_monitor {
   private:
    int16_t accelerometer_records_[SENSORS_RECORD_DEPTH][3];
    float   presuremeter_records_[SENSORS_RECORD_DEPTH];
    float   gyroscope_records_[SENSORS_RECORD_DEPTH][3];

    uint8_t sampling_rate_sec_;

    mbed::BufferedSerial hospital_direct_line_;

    static bool probably_running(const struct movement_snapshot *movement);
    static bool probably_walking(const struct movement_snapshot *movement);
    static bool probably_falling(const struct movement_snapshot *movement);

    static bool probably_stationary(const struct movement_snapshot *movement);

    enum moving_state movement_detection(void);

   public:
    health_monitor(void);
    ~health_monitor(void);

    void read_sensors(void);

    uint8_t get_sampling_rate(void) const;
    void    set_sampling_rate(uint8_t rate);

    void send_to_hospital(void) const;
    void falling_alert(void) const;

    bool is_walking(void);
    bool is_running(void);
    bool is_hiking(void) const;
    bool is_falling(void);
    bool is_stationary(void);

    bool being_hunt_by_a_cat(void) const;
};

}  // namespace iot_health_mon

#endif
