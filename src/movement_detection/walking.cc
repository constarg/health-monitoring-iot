#include "health_monitor.h"

using namespace iot_health_mon;

/* ACCEL_RAW_TO_MG Converts the RAW value retrieved
   from the accelerometer sensor into Mili G Unit. */
#define ACCEL_RAW_TO_MG(RAW) ((RAW) * 0.061f)
/* GYRO_RAW_TO_DPS Converts the RAW value retrieved
   from the Gyroscope sensor into Mili Degrees Per Second Unit. */
#define GYRO_RAW_TO_DPS(RAW) ((RAW) * 0.070f)

/* WALKING_LOW_THRESHOLD Defines a threshold from which we can
   safely assume the user walks. */
#define WALKING_LOW_THRESHOLD  (10)  // 10 mg/(s^2)
/* WALKING_HIGH_THRESHOLD Defines a threshold from after which
   we can safely assume that the user no longer walks (maybe run). */
#define WALKING_HIGH_THRESHOLD (60)  // 60 mg/(s^2)

/* WALKING_GYRO_THRESHOLD Defines a threshold from which we can
   safely assume the user is moving. */
#define WALKING_GYRO_THRESHOLD (2000)  // 2000 DPS.

bool
health_monitor::is_walking(void)
{
    int16_t x_prev_acceleration; /* Holds the previous value sampled for X-axis
                                    by the sensor. */
    int16_t x_curr_acceleration; /* Holds the current value sampled for X-axis
                                    by the sensor. */

    int16_t x_prev_raw; /* Holds the previous RAW value of the accelerometer
                           for the X-axis. */
    int16_t x_curr_raw; /* Holds the current RAW value of the accelerometer
                           for the X-axis. */

    float y_gyro_value; /* Holds the currently examined value of the gyroscope,
                           relative to the y-axis. */
    float y_gyro_raw;   /* Holds the currenlty examined RAW value of the
                           gyroscope,   relative to the y-axis. */

    int16_t x_diff; /* Holds the difference between the previous and currently
                       examined value of the acceleromter X-axis. */

    bool user_moves_on_x; /* Indicates whether the user is moving on the
                             X-axis. */
    bool user_angle_to_y; /* Indicates whether the user forms an angle
                                 with the y-axis. */

    /* Fetch all the RAW values. */
    x_prev_raw = this->accelerometer_records_[0][0];
    y_gyro_raw = this->gyroscope_records_[0][1];

    /* Convert the RAW values to the respective physical units. */
    y_gyro_value        = GYRO_RAW_TO_DPS(y_gyro_raw);
    x_prev_acceleration = (int16_t)ACCEL_RAW_TO_MG(x_prev_raw);

    /* Determine whether the user is walking, based on the recorded values. */
    for (int sample = 1; sample < ACCELEROMETER_DEPTH; sample++) {
        /* Fetch the currently examined accelerometer value for the X-axis. */
        x_curr_raw          = this->accelerometer_records_[sample][0];
        x_curr_acceleration = (int16_t)ACCEL_RAW_TO_MG(x_curr_raw);

        x_diff = std::abs((int16_t)(x_prev_acceleration
                                    - x_curr_acceleration));

        /* Check if the user moves relatively fast towards the x-axis. */
        user_moves_on_x = x_diff > WALKING_LOW_THRESHOLD
                          && x_diff < WALKING_HIGH_THRESHOLD;

        /* Check if the user forms an angle relative to the y-axis. */
        user_angle_to_y = std::abs((int)y_gyro_value) > WALKING_GYRO_THRESHOLD;

        /* Check if the user walks. */
        if (user_moves_on_x && user_angle_to_y) { return true; }

        x_prev_acceleration = x_curr_acceleration;
        y_gyro_raw          = this->gyroscope_records_[sample][1];
        y_gyro_value        = GYRO_RAW_TO_DPS(y_gyro_raw);
    }
    return false;
}
