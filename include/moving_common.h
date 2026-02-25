#ifndef MOVING_COMMON_H
#define MOVING_COMMON_H

#include <stdint.h>

/* WALKING_LOW_THRESHOLD Defines a threshold from which we can
   safely assume the user walks. */
#define WALKING_LOW_THRESHOLD  (10)  // 10 mg/(s^2)
/* WALKING_HIGH_THRESHOLD Defines a threshold after which
   we can safely assume that the user no longer walks (maybe run). */
#define WALKING_HIGH_THRESHOLD (45)  // 60 mg/(s^2)
/* RUNNING_LOW_THRESHOLD Defines a threshold after which we can
   safely assume that the user is running. */
#define RUNNING_LOW_THRESHOLD  (50)
/* RUNNING_HIGH_THRESHOLD Define a threshold after which we can
   safely assume that the user is no longer running
   (maybe moving using a bike or a car). */
#define RUNNING_HIGH_THRESHOLD (150)

/* MOVING_GYRO_THRESHOLD Defines a threshold from which we can
   safely assume the user is moving. */
#define MOVING_GYRO_THRESHOLD (2000)  // 2000 DPS.

/* ACCEL_RAW_TO_MG Converts the RAW value retrieved
   from the accelerometer sensor into Mili G Unit. */
#define ACCEL_RAW_TO_MG(RAW) ((RAW) * 0.061f)
/* GYRO_RAW_TO_DPS Converts the RAW value retrieved
   from the Gyroscope sensor into Mili Degrees Per Second Unit. */
#define GYRO_RAW_TO_DPS(RAW) ((RAW) * 0.070f)

namespace iot_health_mon {

enum moving_state
{
    STATIONARY,
    RUNNING,
    WALKING,
    UNKNOWN
};

struct movement_snapshot
{
    int16_t m_dimmention_accel_before;
    int16_t m_dimmention_accel_after;
    int     m_dimmention_gyro;
};

extern bool walking_running_common(const struct movement_snapshot *movement,
                                   int low_threshold, int high_threshold);

}  // namespace iot_health_mon

#endif
