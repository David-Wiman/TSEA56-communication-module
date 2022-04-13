#include "i2c_common.h"
#include "drivedata.h"
#include "connection.h"
#include "manualdriveinstruction.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

extern "C" {
    #include "i2c.h"
}

using namespace std;
using json = nlohmann::json;

int main() {
    cout << "Start" << endl;

    // Initiate
    i2c_init();
    Connection connection{1234};

    while (true) {

        if (connection.new_manual_instruction()) {
            ManualDriveInstruction instruction = connection.get_manual_drive_instruction();
            cout << "Recieved throttle: " << instruction.get_throttle() << endl;
            cout << "Recieved steering: " << instruction.get_steering() << endl;

            // Send on bus
            i2c_set_slave_addr(0x51);

            int16_t throttle = instruction.get_throttle();
            int16_t steering = instruction.get_steering();
            uint16_t buffer[] = {
                STEERING_MANUAL_GAS, package_signed(throttle),
                STEERING_MANUAL_ANG, package_signed(steering)
            };
            int len = 4;
            i2c_write(buffer, len);
        } else {
            //cout << "No new instruction" << endl;
        }

        this_thread::sleep_for(chrono::milliseconds(100));

        if (connection.has_lost_connection()) {
            cout << "Lost connection with user interface" << endl;
            break;
        }
    }
    return 0;
}
