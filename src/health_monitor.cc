#include "health_monitor.h"

#include <cstring>

#include "PinNameAliases.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_tsensor.h"

using namespace iot_health_mon;

namespace {

uint8_t g_next_accel_index;
uint8_t g_next_gyro_index;
uint8_t g_next_presure_index;

}  // namespace

health_monitor::health_monitor(void) : hospital_direct_line_(USBTX, USBRX)
{
    /* Configure the UART communication channel. */
    this->hospital_direct_line_.set_baud(HOSPITAL_BAUD_RATE);
    this->hospital_direct_line_.set_format(HOSPITAL_BAUD_BITS,
                                           BufferedSerial::None,
                                           HOSPITAL_STOP_BIT);

    /* Initialize all the required sensors. */
    BSP_TSENSOR_Init();
    BSP_PSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_ACCELERO_Init();
    BSP_GYRO_Init();
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

    /* Calculate the index in which the next sample should be stored.
       The index should not exceed the number of available spots. */
    g_next_accel_index   = (g_next_accel_index + 1) % SENSORS_RECORD_DEPTH;
    g_next_gyro_index    = (g_next_gyro_index + 1) % SENSORS_RECORD_DEPTH;
    g_next_presure_index = (g_next_presure_index + 1) % SENSORS_RECORD_DEPTH;

    /* Get a sample from each sensor. */
    BSP_ACCELERO_AccGetXYZ(accelerometer_sample);
    BSP_GYRO_GetXYZ(gyroscope_sample);
    presuremeter_sample = BSP_PSENSOR_ReadPressure();

    /* Store the newly read samples in the respective indexes. */
    (void)memcpy(this->accelerometer_records_[g_next_accel_index],
                 accelerometer_sample, sizeof(accelerometer_sample));
    (void)memcpy(this->gyroscope_records_[g_next_gyro_index], gyroscope_sample,
                 sizeof(gyroscope_sample));

    this->presuremeter_records_[g_next_presure_index] = presuremeter_sample;
}

uint8_t
health_monitor::get_sampling_rate(void) const
{
    return this->sampling_rate_sec_;
}

void
health_monitor::set_sampling_rate(uint8_t rate)
{
    this->sampling_rate_sec_ = rate;
}

void
health_monitor::send_to_hospital(void) const
{
    // TODO: This function should send all the sensor data algong with
    // some flags to the hospital.
    //
    // TODO: It should convert any floating measurement into uint8, and
    // after that to write the array of data into the UART.
}

void
health_monitor::falling_alert(void) const
{
    // TODO: This function should call the send_to_hospital method,
    // with an extra flag that incicates that it was falling.
}
