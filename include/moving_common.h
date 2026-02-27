#ifndef MOVING_COMMON_H
#define MOVING_COMMON_H

#include <stdint.h>

/* MOVING_STARTED_SIG_THRESHOLD Defines the threshold (in mg),
   from which we can safely assume that the user started moving.
   The "moving" can be anything, not walking, just moving. The
   clasification as walking or running is another signature. */
#define MOVING_STARTED_SIG_THRESHOLD (5)

/* WALKING_CONT_SIG_THRESHOLD Defines the threshold from which
   we can safely assume that the user started walking, depending
   on whether the user has started moving in previous record. */
#define WALKING_CONT_SIG_THRESHOLD (5)

/* RUNNING_CONT_SIG_THRESHOLD Defines the threshold from which
   we can safely assume that the user started running, depending
   on whether the user has started moving in previous record. */
#define RUNNING_CONT_SIG_THRESHOLD (50)

/* RUNNING_GYRO_SIG_THRESHOLD Defines the threshold of the
   gyroscope from which we can safely assume that the user is
   running. */
#define RUNNING_GYRO_SIG_THRESHOLD (10000)

/* JUMPING_CONT_SIG_THRESHOLD Defines the threshold from which
   we can safely assume that the user started jumping, depending
   on whether the user was in free fall state in previous record. */
#define JUMPING_CONT_SIG_THRESHOLD (30)

/* FALLING_CONT_SIG_THRESHOLD Defines the threshold from which
   we can safely assume that the user is falling, depending on
   whether the user as in free fall state in previous record. */
#define FALLING_CONT_SIG_THRESHOLD (30)

/* FALLING_GYRO_SIG_THRESHOLD Defines the gyroscope threshold
   from which we can safely assume that the user is falling. This
   is shown as a very quick acceleration towards one of the axis. */
#define FALLING_GYRO_SIG_THRESHOLD (15000)

/* GRAVETETIONAL_PULL Defines the always present pulling from the
   earth against the accelerator. */
#define GRAVITETIONAL_PULL (60)

/* FREE_FALL_STARTED_SIG_THRESHOLD Defines the threshold from which
   we can safely assume that the user is entered a Free Fall state.
   This state is present during both FALLING and JUMPING. The
   characteristic is that the acceleration to the earth temporarly
   is reaching near zero gravetetional pull. I assume that, if the
   user reached less than FREE_ALL_STARTED_SIG_THRESHOLD MG/s^2,
   then is probably in free fall state. */
#define FREE_FALL_STARTED_SIG_THRESHOLD (30)

/* ACCEL_RAW_TO_MG Converts the RAW value retrieved
   from the accelerometer sensor into Mili G Unit. */
#define ACCEL_RAW_TO_MG(RAW) ((RAW) * 0.061f)
/* GYRO_RAW_TO_DPS Converts the RAW value retrieved
   from the Gyroscope sensor into Mili Degrees Per Second Unit. */
#define GYRO_RAW_TO_DPS(RAW) ((RAW) * 0.070f)

namespace iot_health_mon {

/* moving_state holds the known moves. */
enum moving_state
{
    STATIONARY = 0b000,
    RUNNING    = 0b001,
    WALKING    = 0b010,
    JUMPING    = 0b011,
    HIKING     = 0b100,
    FALLING    = 0b101,
    UNKNOWN    = 0b111
};

/**
 * movemenet_snapshot Represent a snap shot of the sampling process.
 * This snap shot is used to determine if the user is active or not.
 * Typically it contains the vector sum of the prevously recorded
 * sample and the sample that came after that.
 */
struct movement_snapshot
{
    int32_t m_vsum_accel; /* Represents the vector sum of
                             the accelerators sample
                             value, recorded after the previous. */
    int     m_vsum_gyro;  /* Represents the vector sum of
                             the gyroscope, for the matching sample
                             as the accelerometers. */
};

}  // namespace iot_health_mon

#endif
