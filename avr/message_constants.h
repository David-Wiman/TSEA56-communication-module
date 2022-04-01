#ifndef STEERING_CONSTANTS_H
#define STEERING_CONSTANTS_H

struct steering_struct {
    const short int MANUAL_GAS = 0xfff0 | 0;
    const short int MANUAL_ANG = 0xfff0 | 1;
    const short int CUR_VEL    = 0xfff0 | 2;
    const short int REF_VEL    = 0xfff0 | 3;
    const short int CUR_LAT    = 0xfff0 | 4;
    const short int REF_LAT    = 0xfff0 | 5;
    const short int CUR_ANG    = 0xfff0 | 6;
} STEERING;

#endif  // STEERING_CONSTANTS_H
