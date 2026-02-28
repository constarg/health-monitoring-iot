#ifndef IOT_MSC_APPLICATION_H
#define IOT_MSC_APPLICATION_H

#include <mbed.h>
#include <stdint.h>

#include "moving_common.h"

namespace iot_health_mon {

/* Defines the number of samples to fetch from
   the sensors to detect a move. */
#define SENSORS_RECORD_DEPTH (600)

/* HOSPITAL_BAUD_RATE Represents the baud rate of the UART
   used to communicate with the Matlab module. */
#define HOSPITAL_BAUD_RATE (9600)
/* HOSPITAL_BAUD_BITS Specifies the number of bits for the
   UART. */
#define HOSPITAL_BAUD_BITS (8)

#define HOSPITAL_STOP_BIT (1)

#define AES_KEY_LENGTH (4)

class health_monitor {
   private:
    /* accelerometer_records_ Holds the next SENSOR_RECORD_DEPTH
       samples of the accelerometer sensor. */
    int16_t accelerometer_records_[SENSORS_RECORD_DEPTH][3];
    /* presuremeter_records_ Holds the next SENSOR_RECORD_DEPTH
       samples of the presure meter sensor. */
    float   presuremeter_records_[SENSORS_RECORD_DEPTH];
    /* gyroscope_records_ Holds the next SENSOR_RECORD_DEPTH
       samples of the gyroscope sensor.*/
    float   gyroscope_records_[SENSORS_RECORD_DEPTH][3];

    /* last_temp_ Holds the last temperature sample. */
    float last_temp_;
    /* last_humidity_ Holds the last humidity sample. */
    float last_humidity_;

    /* sampling_rate_ms_ Holds the sampling rate of the board. It
       can be reduced for power saving purposes. */
    uint8_t sampling_rate_ms_;

    /* active_move_ Holds the currently active move of the user. */
    enum moving_state active_move_;

    /* power_saving_is_active_ Indicates whether the power saving
       feature is active or not. */
    bool power_saving_is_active_;

    /* crypto_key Holds the cryptographic key used to encrypt the
       traffict between the hospital and the end-user. */
    uint8_t crypto_key_[AES_KEY_LENGTH];

    /* hospital_direct_line_ Represents a logical line to the hospital,
       Matlab in out scenario. Data will be sent to this line, through
       UART. */
    mbed::BufferedSerial hospital_direct_line_;

    /**
     * detect_moving_started_sig Detects the signature of a movement that
     * is staring to be visible, .e.g. when the user starts walking there
     * is small spike which goes against gravity.
     *
     * @param movement A pointer to the currently examined sampled movement.
     * @return True if a signature is detected, indicating a move, otherwise
     * false.
     */
    static bool
    detected_moving_started_sig(const struct movement_snapshot *movement);

    /**
     * detect_walking_continued_sig Detects the signature of 'walking' action.
     * The monitoring system will search for this signature only after a
     * moving signature is detected in previous records, detected by
     * detect_moving_started_sig.
     *
     * @param movement A pointer to the currently examined sampled movement.
     * @return True if a signature is detected, indicating that the user
     * walks, otherwise false.
     */
    static bool
    detected_walking_continued_sig(const struct movement_snapshot *movement);

    /**
     * detected_running_continued_sig Detects the signature of 'running'
     * action. The monitoring system will search for this signature only after
     * a moving signature is detected in previous records, detected by
     * detect_moving_started_sig.
     *
     * @param movement A pointer to the currently examined sampled movement.
     * @return True if a signature is detected, indicating that the user
     * is running, otherwise false.
     */
    static bool
    detected_running_continued_sig(const struct movement_snapshot *movement);

    /**
     * detected_freefall_started_sig Detects the signature of a free fall.
     * This signature is quite useful to detect a falling scenario and a
     * jumping exercise scenario. It essentially detects whether an object
     * reaches near 0g situations, which implies that the object is in free
     * fall state. The system will proiritize any movement that came after
     * a free fall, as it may be a fall.
     *
     * @param movement A pointer to the currently examined sampled movement.
     * @return True if a signature is detected, indicating that the user
     * is currently in free fall state.
     */
    static bool
    detected_freefall_started_sig(const struct movement_snapshot *movement);

    /**
     * detected_jumping_continued_sig Detects the signature of a 'jump'.
     * The monitoring system will search for this signature only after
     * the user enters a free fall state.
     *
     * @param movement A pointer to the currently examined sampled movement.
     * @return True if a signature is detected, indicating that the user
     * is currently jumping, otherwise false.
     */
    static bool
    detected_jumping_continued_sig(const struct movement_snapshot *movement);

    /**
     * detected_falling_continued_sig Detects the signature of a 'fall'.
     * The monitoring system will search for this signature only after
     * the user enters a free fall state. This action takes priority over
     * all of the avaiable movements.
     *
     * @param movement A pointer to the currently examiend sampled movement.
     * @return True if a signature is detected, indicating that the user
     * experience a fall, otherwise false.
     */
    static bool
    detected_falling_continued_sig(const struct movement_snapshot *movement);

    /**
     * movement_detection Detects the currently active move.
     * See 'enum moving_state';
     *
     * @return The currently active move.
     */
    enum moving_state movement_detection(void);

   public:
    health_monitor(void);
    ~health_monitor(void);

    /**
     * read_sensors Feteches a SENSOR_RECORD_DEPTH samples
     * from all the sensors of interest.
     */
    void read_sensors(void);

    /**
     * get_sampling_rate Get's the current sampling rate.
     *
     * @return The current sampling rate in (ms).
     */
    uint8_t get_sampling_rate(void) const;

    /**
     * set_sampling_rate Configures the sampling rate
     * to @rate (ms).
     *
     * @param rate The sampling rate in (ms).
     */
    void set_sampling_rate(uint8_t rate);

    /**
     * get_active_move Get's the currently active
     * move of the user.
     */
    enum moving_state get_active_move(void) const;

    /**
     * set_active_move Configures the currently active
     * move of the user.
     *
     * @param move The newly configured move.
     */
    void set_active_move(enum moving_state move);

    /**
     * enable_power_saving Enables the power saving
     * feature.
     */
    void enable_power_saving(void);

    /**
     * disable_power_saving Disables the power saving
     * feature.
     */
    void disable_power_saving(void);

    /**
     * is_power_saving_on Checks whether the power saving
     * feature is enabled.
     *
     * @return True if the power saving feature is enabled,
     * otherwise false.
     */
    bool is_power_saving_on(void) const;

    /**
     * send_to_hospital Sends the petients sensor data
     * to the hospital for further process and monitoring.
     * This is done using the direct line to the hospital.
     */
    void send_to_hospital(void);

    /**
     * is_walking Determines whether the user walks.
     */
    bool is_walking(void);

    /**
     * is_running Determines whether the user is
     * running.
     */
    bool is_running(void);

    /**
     * is_jumping Determiens whether the user is
     * jumping.
     */
    bool is_jumping(void);

    /**
     * is_hiking Determines whether the user is
     * hiking.
     */
    bool is_hiking(void);

    /**
     * is_falling Determines whether the user is
     * falling.
     */
    bool is_falling(void);

    /**
     * is_stationary Determines whether the user
     * is falling.
     */
    bool is_stationary(void);
};

}  // namespace iot_health_mon

#endif
