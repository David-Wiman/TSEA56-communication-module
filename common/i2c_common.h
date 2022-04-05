#ifndef STEERING_CONSTANTS_H
#define STEERING_CONSTANTS_H

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

struct steering_struct {
    const short int MANUAL_GAS;
    const short int MANUAL_ANG;
    const short int CUR_VEL;
    const short int REF_VEL;
    const short int CUR_LAT;
    const short int REF_LAT;
    const short int CUR_ANG;
} STEERING = {0xfff0, 0xfff1, 0xfff2, 0xfff3, 0xfff4, 0xfff5, 0xfff6};

#endif  // STEERING_CONSTANTS_H_
