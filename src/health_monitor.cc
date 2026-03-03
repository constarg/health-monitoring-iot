#include "health_monitor.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#include "PinNameAliases.h"
#include "crypto/aes.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_tsensor.h"

using namespace iot_health_mon;

/* HOSPITAL_MESSAGE_BYTE Defines the number of bytes to
   send to the hospital line. */
#define HOSPITAL_MESSAGE_BYTES (5)

/* MOVE_TWO_DIGITS_ON_INT Moves the two first fraction
   digits of a number to the integer part. */
#define MOVE_TWO_DEGITS_ON_INT (100)

/* ENABLE_LOW_POWER Enables sensor's lower-power mode,
   if passed on the respective routine. */
#define ENABLE_LOW_POWER (1)

/* DISABLE_LOW_POWER Disables sensor's lower-power mode,
   if passed on the respective routine. */
#define DISABLE_LOW_POWER (0)

/* POWER_SAVING_SAMPLING_RATE Defines the sampling rate
   at which the monitoring system should work if the
   power saving feature is enabled. */
#define POWER_SAVING_SAMPLING_RATE (200)

/* STATUS_BIT Indicates the bit where the status bit
   is placed. */
#define STATUS_BIT (0x3)

namespace {

/* g_hospital_buff Contains the data to be sent on hospital. */
uint8_t g_hospital_buff[HOSPITAL_MESSAGE_BYTES] = {0};
/* g_hospital_cipher Contains the encrypted version of g_hospital_buff. */
uint8_t g_hospital_cipher[AES_KEY_LENGTH]       = {0};

// NOLINTBEGIN(readability-magic-numbers)
uint8_t g_hospital_aes_key[AES_KEY_LENGTH] = {
    0x5c, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
// NOLINTEND(readability-magic-numbers)

}  // namespace

health_monitor::health_monitor(void) : hospital_direct_line_(USBTX, USBRX)
{
    /* Configure the UART communication channel. */
    this->hospital_direct_line_.set_baud(HOSPITAL_BAUD_RATE);
    this->hospital_direct_line_.set_format(HOSPITAL_BAUD_BITS,
                                           BufferedSerial::None,
                                           HOSPITAL_STOP_BIT);

    this->sampling_rate_ms_       = 0;
    this->power_saving_is_active_ = false;

    /* Define the cryptographic key used. */
    (void)memcpy(this->crypto_key_, g_hospital_aes_key, AES_KEY_LENGTH);

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
health_monitor::enable_power_saving(void)
{
    BSP_ACCELERO_LowPower(ENABLE_LOW_POWER);
    BSP_GYRO_LowPower(ENABLE_LOW_POWER);

    this->sampling_rate_ms_ = POWER_SAVING_SAMPLING_RATE;
}

void
health_monitor::disable_power_saving(void)
{
    BSP_ACCELERO_LowPower(DISABLE_LOW_POWER);
    BSP_GYRO_LowPower(DISABLE_LOW_POWER);

    this->sampling_rate_ms_ = 0;
}

bool
health_monitor::is_power_saving_on(void) const
{
    return this->power_saving_is_active_;
}

void
health_monitor::send_to_hospital(void)
{
    uint8_t int_temp;  /* Holds the integer part of temperature. */
    uint8_t int_humid; /* Holds the integer part of Humidity. */

    uint8_t int_fraction_temp;  /* Holds the fraction part of the
                                  temperature. */
    uint8_t int_fraction_humid; /* Holds the fraction part of the
                                  humidity. */

    float fraction_temp;  /* Holds the fraction part of the
                             temperature. */
    float fraction_humid; /* Holds the frection part of the
                             humidity. */

    /* Clear up the previous configuration. */
    (void)memset(g_hospital_buff, 0x0, HOSPITAL_MESSAGE_BYTES);

    /* Extract the fraction part out of temperature and humidity
       samples. */
    int_temp  = (uint8_t)this->last_temp_;
    int_humid = (uint8_t)this->last_humidity_;

    fraction_temp  = this->last_temp_ - (float)int_temp;
    fraction_humid = this->last_humidity_ - (float)int_humid;

    int_fraction_temp  = (uint8_t)(fraction_temp * MOVE_TWO_DEGITS_ON_INT);
    int_fraction_humid = (uint8_t)(fraction_humid * MOVE_TWO_DEGITS_ON_INT);

    /* Configure temperature and humidity. */
    g_hospital_buff[0] = int_temp;
    g_hospital_buff[1] = int_fraction_temp;
    g_hospital_buff[2] = int_humid;
    g_hospital_buff[3] = int_fraction_humid;

    /*
       Configure additional informations.
       The g_hospital_buff should contain the status
       of the monitoring system. This should contain
       the following:
            - The currently active move.
            - The monitor state (power-saving, full)
        These information will be encoded in the last
        byte as follows:
            BIT[2] downto BIT[0] = MOVE
            BIT[3]               = state (power-saving, full)
        See moving_state enumaration, moving_comoon.h, to see
        the encoded values of each move.
    */
    g_hospital_buff[4] |= this->active_move_;
    g_hospital_buff[4] |= ((uint8_t)this->power_saving_is_active_)
                          << STATUS_BIT;

    // printf("%d\n", g_hospital_buff[4]);

    /* Encrypt the data to be sent to the hospital. */
    AES_ECB_encrypt(g_hospital_buff, this->crypto_key_, g_hospital_cipher,
                    HOSPITAL_MESSAGE_BYTES);

    /* Send the data into the UART channel. */
    this->hospital_direct_line_.write(g_hospital_cipher, AES_KEY_LENGTH);
}
