#ifndef IOT_MSC_APPLICATION_H
#define IOT_MSC_APPLICATION_H

#include <mbed.h>
#include <stdint.h>

namespace iot_health_mon {

#define ACCELEROMETER_DEPTH (10)
#define PRESUREMETER_DEPTH  (10)
#define GYROSCOPE_DEPTH     (3)

#define HOSPITAL_BAUD_RATE (9600)
#define HOSPITAL_BAUD_BITS (8)
#define HOSPITAL_STOP_BIT  (1)

class health_monitor {
   private:
    int16_t accelerometer_records_[ACCELEROMETER_DEPTH][3];
    float   presuremeter_records_[PRESUREMETER_DEPTH];
    float   gyroscope_records_[GYROSCOPE_DEPTH][3];

    uint8_t sampling_rate_sec_;

    mbed::BufferedSerial hospital_direct_line_;

   public:
    health_monitor(void);
    ~health_monitor(void);

    void read_sensors(void);

    uint8_t get_sampling_rate(void) const;
    void    set_sampling_rate(uint8_t rate);

    void send_to_hospital(void) const;
    void falling_alert(void) const;

    bool is_staying(void) const;
    bool is_walking(void) const;
    bool is_running(void) const;
    bool is_jumping(void) const;
    bool is_stretching(void) const;
    bool is_hiking(void) const;
    bool is_falling(void) const;
};

}  // namespace iot_health_mon

#endif
