#include "moving_common.h"

#include "health_monitor.h"

using namespace iot_health_mon;

namespace iot_health_mon {

bool
walking_running_common(const struct movement_snapshot *movement,
                       int low_threshold, int high_threshold)
{
    int32_t before_acceleration; /* Represents the state of the accelerometer
                                    before the currently examined sample. */
    int32_t after_acceleration;  /* Represents the current state of the
                                    accelerometer. */

    int gyroscope; /* Represents the current state of the gyroscope. */

    int16_t diff; /* Holds the difference in acceleration between the
                     previous and the currently examined sample. */

    bool is_moving;   /* Indicates whether the user is moving. */
    bool forms_angle; /* Indicates whether the user forms any angle towards any
                         dimmention. */

    before_acceleration = movement->m_vsum_accel_before;
    after_acceleration  = movement->m_vsum_accel_after;

    gyroscope = std::abs(movement->m_vsum_gyro);

    diff = (int16_t)std::abs((int16_t)(after_acceleration
                                       - before_acceleration));

    /* Check if the user forms an angle relative to the y-axis. */
    is_moving = diff > low_threshold && diff < high_threshold;

    forms_angle = gyroscope > MOVING_GYRO_THRESHOLD;

    /* Check if the is walking towards the x-axis. */
    return is_moving && forms_angle;
}

}  // namespace iot_health_mon

enum moving_state
health_monitor::movement_detection(void)
{
    int16_t x_prev_acceleration; /* Holds the previous value sampled for X-axis
                                    by the sensor. */
    int16_t y_prev_acceleration; /* Holds the previous value sampled for Y-axis
                                    by the sensor. */
    int16_t z_prev_acceleration; /* Holds the previous value sampled for X-axis
                                    by the sensor. */

    int16_t x_curr_acceleration; /* Holds the current value sampled for X-axis
                                    by the sensor. */
    int16_t y_curr_acceleration; /* Holds the current value sampled for Y-axis
                                    by the sensor. */
    int16_t z_curr_acceleration; /* Holds the current value sampled for Z-axis
                                    by the sensor. */

    int16_t x_prev_raw; /* Holds the previous RAW value of the accelerometer
                           for the X-axis. */
    int16_t x_curr_raw; /* Holds the current RAW value of the accelerometer
                           for the X-axis. */
    int16_t y_prev_raw; /* Holds the previous RAW value of the accelerometer
                           for the Y-axis. */
    int16_t y_curr_raw; /* Holds the current RAW value of the accelerometer
                           for the Y-axis. */
    int16_t z_prev_raw; /* Holds the previous RAW value of the accelerometer
                           for the Z-axis. */
    int16_t z_curr_raw; /* Holds the current RAW value of the accelerometer
                           for the Z-axis. */

    int32_t prev_acceleration_vector_sum; /* Holds the previous sum of all the
                                             vectors of the accelerometer. */
    int32_t curr_acceleration_vector_sum; /* Holds the current sum of all the
                                             vectors of the accelerometer. */

    float x_gyro_value; /* Holds the currently examined value of the gyroscope,
                           relative to the y-axis. */
    float x_gyro_raw;   /* Holds the currenlty examined RAW value of the
                           gyroscope,   relative to the y-axis. */
    float y_gyro_value; /* Holds the currently examined value of the gyroscope,
                           relative to the y-axis. */
    float y_gyro_raw;   /* Holds the currenlty examined RAW value of the
                           gyroscope,   relative to the y-axis. */
    float z_gyro_value; /* Holds the currently examined value of the gyroscope,
                           relative to the y-axis. */
    float z_gyro_raw;   /* Holds the currenlty examined RAW value of the
                           gyroscope,   relative to the y-axis. */

    float gyro_vector_sum; /* Holds the vector sum of the currently examined
                              state of the gyroscope. */

    struct movement_snapshot movement; /* Holds the currently examined
                                          movement. */

    /* Fetch all the RAW values. */
    x_prev_raw = this->accelerometer_records_[0][0];
    y_prev_raw = this->accelerometer_records_[0][1];
    z_prev_raw = this->accelerometer_records_[0][2];

    x_gyro_raw = this->gyroscope_records_[0][0];
    y_gyro_raw = this->gyroscope_records_[0][1];
    z_gyro_raw = this->gyroscope_records_[0][2];

    /* Convert the RAW values to the respective physical units. */
    x_gyro_value = GYRO_RAW_TO_DPS(x_gyro_raw);
    y_gyro_value = GYRO_RAW_TO_DPS(y_gyro_raw);
    z_gyro_value = GYRO_RAW_TO_DPS(z_gyro_raw);

    x_prev_acceleration = (int16_t)ACCEL_RAW_TO_MG(x_prev_raw);
    y_prev_acceleration = (int16_t)ACCEL_RAW_TO_MG(y_prev_raw);
    z_prev_acceleration = (int16_t)ACCEL_RAW_TO_MG(z_prev_raw);

    prev_acceleration_vector_sum = x_prev_acceleration + y_prev_acceleration
                                   + z_prev_acceleration;
    gyro_vector_sum = x_gyro_value + y_gyro_value + z_gyro_value;

    /* Determine whether the user is walking, based on the recorded values. */
    for (int sample = 1; sample < SENSORS_RECORD_DEPTH; sample++) {
        x_curr_raw          = this->accelerometer_records_[sample][0];
        x_curr_acceleration = (int16_t)ACCEL_RAW_TO_MG(x_curr_raw);
        y_curr_raw          = this->accelerometer_records_[sample][1];
        y_curr_acceleration = (int16_t)ACCEL_RAW_TO_MG(y_curr_raw);
        z_curr_raw          = this->accelerometer_records_[sample][2];
        z_curr_acceleration = (int16_t)ACCEL_RAW_TO_MG(z_curr_raw);

        curr_acceleration_vector_sum = x_curr_acceleration
                                       + y_curr_acceleration
                                       + z_curr_acceleration;

        movement.m_vsum_gyro         = (int)gyro_vector_sum;
        movement.m_vsum_accel_before = prev_acceleration_vector_sum;
        movement.m_vsum_accel_after  = curr_acceleration_vector_sum;

        if (health_monitor::probably_walking(&movement)) {
            return moving_state::WALKING;
        }

        if (health_monitor::probably_running(&movement)) {
            return moving_state::RUNNING;
        }

        if (health_monitor::probably_falling(&movement)) {
            return moving_state::FALLING;
        }

        x_prev_acceleration = x_curr_acceleration;
        y_prev_acceleration = y_curr_acceleration;
        z_prev_acceleration = z_curr_acceleration;

        prev_acceleration_vector_sum = x_prev_acceleration
                                       + y_prev_acceleration
                                       + z_prev_acceleration;

        x_gyro_raw = this->gyroscope_records_[sample][0];
        y_gyro_raw = this->gyroscope_records_[sample][1];
        z_gyro_raw = this->gyroscope_records_[sample][2];

        x_gyro_value = GYRO_RAW_TO_DPS(x_gyro_raw);
        y_gyro_value = GYRO_RAW_TO_DPS(y_gyro_raw);
        z_gyro_value = GYRO_RAW_TO_DPS(z_gyro_raw);
    }

    movement.m_vsum_gyro         = (int)gyro_vector_sum;
    movement.m_vsum_accel_before = prev_acceleration_vector_sum;
    movement.m_vsum_accel_after  = curr_acceleration_vector_sum;

    if (health_monitor::probably_stationary(&movement)) {
        return moving_state::STATIONARY;
    }

    return moving_state::UNKNOWN;
}
