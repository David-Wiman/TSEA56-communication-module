#include <iostream>

extern "C" {
    #include "i2c.h"
}

using namespace std;

int main() {
    cout << "MAIN!" << endl;
    i2c_init();
    i2c_set_slave_addr(0x50);

    uint16_t buffer[] = {0x0123, 0x4567, 0x89ab, 0xcdef};
    int len = 4;
    i2c_write(buffer, len);
    return 0;
}
