#ifndef STEERING_CONSTANTS_H
#define STEERING_CONSTANTS_H

struct steering_struct {
    const short int MANUAL_GAS;
    const short int MANUAL_ANG;
    const short int CUR_VEL;
    const short int REF_VEL;
    const short int CUR_LAT;
    const short int REF_LAT;
    const short int CUR_ANG;
} STEERING = {0xfff0, 0xfff1, 0xfff2, 0xfff3, 0xfff4, 0xfff5, 0xfff6};

#endif  // STEERING_CONSTANTS_H
