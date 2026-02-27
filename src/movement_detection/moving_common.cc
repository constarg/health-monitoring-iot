#include "moving_common.h"

#include <math.h>

#include <cstdio>

#include "health_monitor.h"

using namespace iot_health_mon;

/* VECTOR_SUM Calculates the vector SUM of a 3D component.
 *
 * @param X the value in X axis.
 * @param Y the value in Y axis.
 * @param Z the value in Z axis.
 */
#define VECTOR_SUM(X, Y, Z) sqrt(pow((X), 2) + pow((Y), 2) + pow((Z), 2))

bool
health_monitor::detected_moving_started_sig(const struct movement_snapshot
                                                *movement)
{
    return (std::abs(movement->m_vsum_accel - GRAVITETIONAL_PULL)
            > MOVING_STARTED_SIG_THRESHOLD)
           && (movement->m_vsum_accel > GRAVITETIONAL_PULL);
}

bool
health_monitor::detected_freefall_started_sig(const struct movement_snapshot
                                                  *movement)
{
    return movement->m_vsum_accel < FREE_FALL_STARTED_SIG_THRESHOLD;
}

enum moving_state
health_monitor::movement_detection(void)
{
    int16_t x_curr_acceleration; /* Holds the previous value sampled for X-axis
                                    by the sensor. */
    int16_t y_curr_acceleration; /* Holds the previous value sampled for Y-axis
                                    by the sensor. */
    int16_t z_curr_acceleration; /* Holds the previous value sampled for X-axis
                                    by the sensor. */

    int16_t x_curr_raw; /* Holds the previous RAW value of the accelerometer
                           for the X-axis. */
    int16_t y_curr_raw; /* Holds the previous RAW value of the accelerometer
                           for the Y-axis. */
    int16_t z_curr_raw; /* Holds the previous RAW value of the accelerometer
                           for the Z-axis. */

    int32_t acceleration_vector_sum; /* Holds the previous sum of all the
                                             vectors of the accelerometer. */

    float x_gyro_value; /* Holds the currently examined value of the gyroscope,
                           relative to the x-axis. */
    float x_gyro_raw;   /* Holds the currenlty examined RAW value of the
                           gyroscope,   relative to the x-axis. */
    float y_gyro_value; /* Holds the currently examined value of the gyroscope,
                           relative to the y-axis. */
    float y_gyro_raw;   /* Holds the currenlty examined RAW value of the
                           gyroscope,   relative to the y-axis. */
    float z_gyro_value; /* Holds the currently examined value of the gyroscope,
                           relative to the z-axis. */
    float z_gyro_raw;   /* Holds the currenlty examined RAW value of the
                           gyroscope,   relative to the z-axis. */

    float gyro_vector_sum; /* Holds the vector sum of the currently examined
                              state of the gyroscope. */

    struct movement_snapshot movement; /* Holds the currently examined
                                          movement. */

    bool is_moving_started_sig; /* Indicates whether the user is moving. */
    bool is_freefall_sig;       /* Indicates whether the user is at free fall
                                   state. */

    bool is_walking_continued_sig; /* Indicates whether the user is walking. */
    bool is_jumping_continued_sig; /* Indicates whether the user is jumping. */
    bool is_falling_continued_sig; /* Indicates whether the user is falling. */
    bool is_running_continued_sig; /* Indicates whether the user is running. */

    is_moving_started_sig    = false;
    is_walking_continued_sig = false;
    is_freefall_sig          = false;
    is_jumping_continued_sig = false;
    is_falling_continued_sig = false;
    is_running_continued_sig = false;

    for (int sample = 0; sample < SENSORS_RECORD_DEPTH; sample++) {
        /* Fetch all the RAW values. */
        x_curr_raw = this->accelerometer_records_[sample][0];
        y_curr_raw = this->accelerometer_records_[sample][1];
        z_curr_raw = this->accelerometer_records_[sample][2];

        x_gyro_raw = this->gyroscope_records_[sample][0];
        y_gyro_raw = this->gyroscope_records_[sample][1];
        z_gyro_raw = this->gyroscope_records_[sample][2];

        /* Convert the RAW values to the respective physical units. */
        x_gyro_value = GYRO_RAW_TO_DPS(x_gyro_raw);
        y_gyro_value = GYRO_RAW_TO_DPS(y_gyro_raw);
        z_gyro_value = GYRO_RAW_TO_DPS(z_gyro_raw);

        /* Convert the RAW values of the accelerometer to the
           physical units. */
        x_curr_acceleration = (int16_t)ACCEL_RAW_TO_MG(x_curr_raw);
        y_curr_acceleration = (int16_t)ACCEL_RAW_TO_MG(y_curr_raw);
        z_curr_acceleration = (int16_t)ACCEL_RAW_TO_MG(z_curr_raw);

        /* Sum up all the vectors of the accelerator. */
        acceleration_vector_sum = VECTOR_SUM(x_curr_acceleration,
                                             y_curr_acceleration,
                                             z_curr_acceleration);

        /* Sum up all the vectors of the gyroscope. */
        gyro_vector_sum = VECTOR_SUM(x_gyro_value, y_gyro_value, z_gyro_value);

        movement.m_vsum_accel = acceleration_vector_sum;
        movement.m_vsum_gyro  = (int)gyro_vector_sum;

        /* Find the FIRST stage of an action. The first stage, for this
           exercise at least, is either a moving state or a free fall
           state. Using the moving stage we can determine whether the user
           is walking, running, hiking and using the free fall state we can
           determine whether the use is falling of jumping. */
        if (health_monitor::detected_freefall_started_sig(&movement)) {
            is_freefall_sig          = true;
            is_moving_started_sig    = false;
            is_walking_continued_sig = false;
            is_running_continued_sig = false;
        } else if (health_monitor::detected_moving_started_sig(&movement)) {
            is_moving_started_sig = true;
        }

        /* If the user was in the free fall state and the signature
           indicated a falling state, the user is FALLING!. */
        if (is_freefall_sig && is_falling_continued_sig) {
            return moving_state::FALLING;
        }

        /* If the user was in the free fall state and the signature
           indicated a jumping state, the user is jumping. */
        if (is_freefall_sig && is_jumping_continued_sig) {
            return moving_state::JUMPING;
        }

        /* If the user was in the moving state and the signature
           indicatesd a running state, the user is running. */
        if (is_moving_started_sig && is_running_continued_sig) {
            return moving_state::RUNNING;
        }

        /* If the user was in the moving state and the signature
           indicated a walking state, the user is walking. */
        if (is_moving_started_sig && is_walking_continued_sig) {
            return moving_state::WALKING;
        }

        if (is_freefall_sig) {
            if (detected_falling_continued_sig(&movement)) {
                is_falling_continued_sig = true;
            } else if (detected_jumping_continued_sig(&movement)) {
                is_jumping_continued_sig = true;
            }

        } else if (is_moving_started_sig) {
            if (detected_running_continued_sig(&movement)) {
                is_running_continued_sig = true;
            } else if (detected_walking_continued_sig(&movement)) {
                is_walking_continued_sig = true;
                is_moving_started_sig    = false;
            }
        }
    }

    if (!is_moving_started_sig || !is_freefall_sig) {
        return moving_state::STATIONARY;
    }

    return moving_state::UNKNOWN;
}
