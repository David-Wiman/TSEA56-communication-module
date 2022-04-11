#include <iostream>
#include "i2c_common.h"

extern "C" {
    #include "i2c.h"
}

using namespace std;

int main() {
    cout << "MAIN!" << endl;
    i2c_init();
    i2c_set_slave_addr(0x51);

    int16_t gas = 10;
    int16_t steer = -2;
    int16_t steering_KP = 1200;
    int16_t steering_KD = 1400;
    int16_t speed_KP = 213;

    uint16_t buffer[] = {
        STEERING_MANUAL_GAS, package_signed(gas),
        STEERING_MANUAL_ANG, package_signed(steer),
        STEERING_STEERING_KP, package_signed(steering_KP),
        STEERING_STEERING_KD, package_signed(steering_KD),
        STEERING_SPEED_KP, package_signed(speed_KP)
    };
    uint16_t message_names[16];
    uint16_t messages[16];
    i2c_write(buffer, 10);
    i2c_close();
    return 0;
}
