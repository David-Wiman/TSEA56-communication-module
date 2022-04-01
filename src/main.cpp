#include <iostream>
#include "message_constants.h"

extern "C" {
    #include "i2c.h"
}

using namespace std;

int main() {
    cout << "MAIN!" << endl;
    i2c_init();
    i2c_set_slave_addr(0x50);

    cout << "Labels: " << STEERING.MANUAL_GAS << " " << STEERING.MANUAL_ANG << endl;

    uint16_t buffer[] = {STEERING.MANUAL_GAS, 0x111, STEERING.MANUAL_ANG, -2 & 0x7fff};
    int len = 4;
    i2c_write(buffer, len);
    return 0;
}
