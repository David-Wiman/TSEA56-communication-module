#include "i2c_common.h"

#include <iostream>
#include <chrono>
#include <thread>

extern "C" {
    #include "i2c.h"
}

using namespace std;

int main() {
    cout << "MAIN!" << endl;
    i2c_init();
    i2c_set_slave_addr(0x50);

    int tests{1000};
    int sucessfull_tests{0};
    int delay_ms = 10;
    int max_consecutive_fails{0};
    int consecutive_fails{0};
    for (int n=0; n<tests; ++n) {
        uint16_t message_names[16];
        uint16_t messages[16];
        int len = i2c_read(message_names, messages);
        if (len > 0) {
            printf("Read %d packages:\n", len);
            for (int i=0; i<len; ++i)
                printf("\tName: 0x%x Data 0x%x\n", message_names[i], messages[i]);
            sucessfull_tests++;
            consecutive_fails = 0;
	} else if (len == 0) {
		cout << "Warning: Slave has no new data." << endl;
        } else {
            max_consecutive_fails = max(max_consecutive_fails, ++consecutive_fails);
	    cout << "I2C Error: " << len << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
    i2c_close();
    cout << "\n" << sucessfull_tests << "/" << tests
         << " successful reads (@" << 1/(delay_ms/1000.0) << " Hz)"
         << " (at most " << max_consecutive_fails << " fails in a row)" << endl;
    return 0;
}
