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

    uint16_t message_names[16];
    uint16_t messages[16];
    int len = i2c_read(message_names, messages);
    if (len > 0) {
        printf("Read %d packages:\n", len);
        for (int i=0; i<len; ++i)
            printf("\tName: 0x%x Data 0x%x\n", message_names[i], messages[i]);
    } else {
        printf("\tError\n");
    }
    i2c_close();
    return 0;
}
