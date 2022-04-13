#include "communication_module.h"
extern "C" {
    #include "i2c.h"
    #include "i2c_common.h"
}

using namespace std;

void CommunicationModule::send_manual_instruction(uint16_t throttle, uint16_t steering) {
    i2c_set_slave_addr(STEERING_MODULE_SLAVE_ADDRESS);
    uint16_t buffer[] = {
        STEERING_MANUAL_GAS, package_signed(throttle),
        STEERING_MANUAL_ANG, package_signed(steering)
    };
    i2c_write(buffer, 4);
}

int CommunicationModule::get_sensor_data(sensor_data_t &sensor_data) {
    i2c_set_slave_addr(SENSOR_MODULE_SLAVE_ADDRESS);
    uint16_t message_names[16];
    uint16_t messages[16];
    int len = i2c_read(message_names, messages);
    if (len > 0) {
        cout << "Read " << len << " packages" << endl;
        for (int i=0; i<len; ++i) {
            switch (message_names[i]) {
                case SENSOR_OBSTACLE_DISTANCE:
                    sensor_data.obstacle_distance = messages[i];
                    cout << "Read obstacle distance: "
                         << sensor_data.obstacle_distance << endl;
                    break;
                case SENSOR_DRIVING_DISTANCE:
                    sensor_data.driving_distance = messages[i];
                    cout << "Read driving distance: "
                         << sensor_data.driving_distance << endl;
                    break;
                case SENSOR_SPEED:
                    sensor_data.speed = messages[i];
                    cout << "Read speed: "
                         << sensor_data.speed << endl;
                    break;
                default:
                    cout << "Warning: Unexpected value read from sensor module "
                         << "value = " << messages[i] << endl;
                    break;
            }
        }
        return 0;
    } else if (len == 0) {
        cout << "Warning: Slave has no new data." << endl;
        return 1;
    } else {
        cout << "I2C Error: " << len << endl;
        return -1;
    }

}

