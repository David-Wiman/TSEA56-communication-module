#include <iostream>

extern "C" {
    #include "i2c.h"
    void i2c_test();
}

//extern "C" void i2c_init();

using namespace std;

int main() {
    cout << "MAIN!" << endl;
    i2c_test();
    return 0;
}
