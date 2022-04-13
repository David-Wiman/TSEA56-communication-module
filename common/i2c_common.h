#include "stdint.h"

#ifndef COMMON_H
#define COMMON_H

/* Package signed value before sending it on the 12c bus.
 *
 * Data is lost if num uses more than 14 bits. */
inline uint16_t package_signed(int16_t num) {
    return num & 0x7fff;
}

/* Restore the signed form of an integer sent over the i2c bus. */
inline int16_t restore_signed(uint16_t num) {
    return (int16_t)(((num & 0x4000) << 1) | num);
}

#define STEERING_MODULE_SLAVE_ADDRESS 0x51
#define SENSOR_MODULE_SLAVE_ADDRESS   0x52

#define STEERING_MANUAL_GAS       0xfff0
#define STEERING_MANUAL_ANG       0xfff1
#define STEERING_CUR_VEL          0xfff2
#define STEERING_REF_VEL          0xfff3
#define STEERING_CUR_LAT          0xfff4
#define STEERING_REF_LAT          0xfff5
#define STEERING_CUR_ANG_LEFT     0xfff6
#define STEERING_CUR_ANG_RIGHT    0xfff7
#define STEERING_STEERING_KP      0xfff8
#define STEERING_STEERING_KD      0xfff9
#define STEERING_SPEED_KP         0xfffA
#define STEERING_TURN_KP          0xfffB
#define STEERING_TURN_KD          0xfffC
#define STEERING_REGULATION_MODE  0xfffD

#define SENSOR_OBSTACLE_DISTANCE 0xfff0
#define SENSOR_DRIVING_DISTANCE  0xfff1
#define SENSOR_SPEED             0xfff2

#endif  // COMMON_H
