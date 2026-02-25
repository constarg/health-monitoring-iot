#ifndef MOVING_COMMON_H
#define MOVING_COMMON_H

#include <stdint.h>

/* WALKING_LOW_THRESHOLD Defines a threshold from which we can
   safely assume the user walks. */
#define WALKING_LOW_THRESHOLD  (15)  // 15 mg/(s^2)
/* WALKING_HIGH_THRESHOLD Defines a threshold after which
   we can safely assume that the user no longer walks (maybe run). */
#define WALKING_HIGH_THRESHOLD (35)  // 35 mg/(s^2)
/* RUNNING_LOW_THRESHOLD Defines a threshold after which we can
   safely assume that the user is running. */
#define RUNNING_LOW_THRESHOLD  (35)
/* RUNNING_HIGH_THRESHOLD Define a threshold after which we can
   safely assume that the user is no longer running
   (maybe moving using a bike or a car). */
#define RUNNING_HIGH_THRESHOLD (120)

/* MOVING_GYRO_THRESHOLD Defines a threshold from which we can
   safely assume the user is moving. */
#define MOVING_GYRO_THRESHOLD (5000)  // 2000 DPS.

/* FALLING_GYRO_THRESHOLD Defines a threshold from which we can
   safely assume that the user is falling down. */
#define FALLING_GYRO_THRESHOLD (50000)

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
    HIKING,
    FALLING,
    UNKNOWN
};

/**
 * movemenet_snapshot Represent a snap shot of the sampling process.
 * This snap shot is used to determine if the user is active or not.
 * Typically it contains the vector sum of the prevously recorded
 * sample and the sample that came after that.
 */
struct movement_snapshot
{
    int32_t m_vsum_accel_before; /* Rerpesents the vector sum of
                                    the accelerators previous sample
                                    value. */
    int32_t m_vsum_accel_after;  /* Represents the vector sum of
                                    the accelerators sample
                                    value, recorded after the previous. */
    int     m_vsum_gyro;         /* Represents the vector sum of
                                    the gyroscope, for the matching sample
                                    as the accelerometers. */
};

/**
 * walking_running_common Is used to detect both the walking activity and
 * the running activity. The differenciation between walking and running
 * is determined by the threshold passed on this routine (namely low_threshold
 * and high_threshold).
 *
 * @param movement A pointer to the currently examined movement.
 * @param low_threshold An integer representing a lower bound.
 * @param high_threshold An integer representing a high bound.
 * @return True if movement were between the lower & higher bound, False,
 * otherwise.
 */
extern bool walking_running_common(const struct movement_snapshot *movement,
                                   int low_threshold, int high_threshold);

}  // namespace iot_health_mon

#endif
