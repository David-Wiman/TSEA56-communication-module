#include <iostream>

extern "C" {
    #include "i2c.h"
}

using namespace std;

int main() {
    cout << "MAIN!" << endl;
    uint8_t buffer[32] = {0};
    i2c_init();
    i2c_set_slave_addr(0x5a);
    i2c_write();
    return 0;
}
