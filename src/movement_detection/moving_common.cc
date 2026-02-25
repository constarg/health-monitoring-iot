#include "moving_common.h"

#include "health_monitor.h"

using namespace iot_health_mon;

namespace iot_health_mon {

bool
walking_running_common(const struct movement_snapshot *movement,
                       int low_threshold, int high_threshold)
{
    int16_t x_before_acceleration;
    int16_t x_after_acceleration;

    int y_gyroscope;

    int16_t x_diff;

    bool is_moving_on_x;
    bool forms_angle_with_y;

    x_before_acceleration = movement->m_dimmention_accel_before;
    x_after_acceleration  = movement->m_dimmention_accel_after;

    y_gyroscope = std::abs(movement->m_dimmention_gyro);

    x_diff = (int16_t)std::abs((int16_t)(x_after_acceleration
                                         - x_before_acceleration));

    /* Check if the user forms an angle relative to the y-axis. */
    is_moving_on_x = x_diff > low_threshold && x_diff < high_threshold;

    forms_angle_with_y = y_gyroscope > MOVING_GYRO_THRESHOLD;

    /* Check if the is walking towards the x-axis. */
    return is_moving_on_x && forms_angle_with_y;
}

}  // namespace iot_health_mon

enum moving_state
health_monitor::movement_detection(void)
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

    struct movement_snapshot movement;

    /* Fetch all the RAW values. */
    x_prev_raw = this->accelerometer_records_[0][0];
    y_gyro_raw = this->gyroscope_records_[0][1];

    /* Convert the RAW values to the respective physical units. */
    y_gyro_value        = GYRO_RAW_TO_DPS(y_gyro_raw);
    x_prev_acceleration = (int16_t)ACCEL_RAW_TO_MG(x_prev_raw);

    /* Determine whether the user is walking, based on the recorded values. */
    for (int sample = 1; sample < SENSORS_RECORD_DEPTH; sample++) {
        /* Fetch the currently examined accelerometer value for the X-axis. */
        x_curr_raw          = this->accelerometer_records_[sample][0];
        x_curr_acceleration = (int16_t)ACCEL_RAW_TO_MG(x_curr_raw);

        movement.m_dimmention_gyro         = (int)y_gyro_value;
        movement.m_dimmention_accel_before = x_prev_acceleration;
        movement.m_dimmention_accel_after  = x_curr_acceleration;

        if (health_monitor::probably_walking(&movement)) {
            return moving_state::WALKING;
        }

        if (health_monitor::probably_running(&movement)) {
            return moving_state::RUNNING;
        }

        x_prev_acceleration = x_curr_acceleration;
        y_gyro_raw          = this->gyroscope_records_[sample][1];
        y_gyro_value        = GYRO_RAW_TO_DPS(y_gyro_raw);
    }

    movement.m_dimmention_gyro         = (int)y_gyro_value;
    movement.m_dimmention_accel_before = x_prev_acceleration;
    movement.m_dimmention_accel_after  = x_curr_acceleration;

    if (health_monitor::probably_stationary(&movement)) {
        return moving_state::STATIONARY;
    }

    return moving_state::UNKNOWN;
}
