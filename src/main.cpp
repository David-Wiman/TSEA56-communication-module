#include <iostream>
#include "i2c_common.h"

extern "C" {
    #include "i2c.h"
}

using namespace std;

int main() {
    cout << "MAIN!" << endl;
    i2c_init();
    i2c_set_slave_addr(0x50);

    cout << "Labels: " << STEERING_MANUAL_GAS << " " << STEERING_MANUAL_ANG << endl;

    int16_t gas = 10;
    int16_t steer = -2;
    uint16_t buffer[] = {
        STEERING_MANUAL_GAS, package_signed(gas),
        STEERING_MANUAL_ANG, package_signed(steer)
    };
    int len = 4;
    i2c_write(buffer, len);
    return 0;
}
