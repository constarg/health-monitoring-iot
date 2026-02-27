#include "health_monitor.h"

#include <cstdio>
#include <cstring>

#include "PinNameAliases.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_tsensor.h"

using namespace iot_health_mon;

#define HOSPITAL_MESSAGE_BYTES (5)

namespace {

uint8_t hospital_buff[HOSPITAL_MESSAGE_BYTES];

}

health_monitor::health_monitor(void) : hospital_direct_line_(USBTX, USBRX)
{
    /* Configure the UART communication channel. */
    this->hospital_direct_line_.set_baud(HOSPITAL_BAUD_RATE);
    this->hospital_direct_line_.set_format(HOSPITAL_BAUD_BITS,
                                           BufferedSerial::None,
                                           HOSPITAL_STOP_BIT);

    /* Initialize all the required sensors. */
    BSP_TSENSOR_Init();  /* Thermal sensor. */
    BSP_PSENSOR_Init();  /* Presure sensor. */
    BSP_HSENSOR_Init();  /* Humidity sensor. */
    BSP_ACCELERO_Init(); /* Accelerometer sensor. */
    BSP_GYRO_Init();     /* Gyroscope sensor. */
}

health_monitor::~health_monitor()
{
    BSP_ACCELERO_DeInit();
    BSP_GYRO_DeInit();
}

void
health_monitor::read_sensors(void)
{
    int16_t accelerometer_sample[3]; /* Holds the currently READ sample from
                                     the accelerometer. */
    float   gyroscope_sample[3];     /* Holds the currently READ sample from
                                     the gyroscope. */
    float   presuremeter_sample;     /* Holds the currently READ sample from
                                        the presuremeter. */

    for (int sample = 0; sample < SENSORS_RECORD_DEPTH; sample++) {
        /* Get a sample from each sensor. */
        BSP_ACCELERO_AccGetXYZ(accelerometer_sample);
        BSP_GYRO_GetXYZ(gyroscope_sample);
        presuremeter_sample = BSP_PSENSOR_ReadPressure();

        /* Store the newly read samples in the respective indexes. */
        (void)memcpy(this->accelerometer_records_[sample],
                     accelerometer_sample, sizeof(accelerometer_sample));
        (void)memcpy(this->gyroscope_records_[sample], gyroscope_sample,
                     sizeof(gyroscope_sample));

        this->presuremeter_records_[sample] = presuremeter_sample;
    }

    /* Read Temperature and Humidity values, to calculate the comfort level. */
    this->last_temp_     = BSP_TSENSOR_ReadTemp();
    this->last_humidity_ = BSP_HSENSOR_ReadHumidity();
}

uint8_t
health_monitor::get_sampling_rate(void) const
{
    return this->sampling_rate_ms_;
}

void
health_monitor::set_sampling_rate(uint8_t rate)
{
    this->sampling_rate_ms_ = rate;
}

enum moving_state
health_monitor::get_active_move(void) const
{
    return this->active_move_;
}

void
health_monitor::set_active_move(enum moving_state move)
{
    this->active_move_ = move;
}

void
health_monitor::send_to_hospital(void) const
{
}
