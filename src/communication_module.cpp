#include "communication_module.h"

#define MAX_INT 65535
extern "C" {
    #include "i2c.h"
    #include "i2c_common.h"
}

#include <iostream>

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
        uint16_t left_driving_distance{0};
        uint16_t right_driving_distance{0};
        uint16_t left_speed{MAX_INT};
        uint16_t right_speed{MAX_INT};
        bool found_obstacle_distance{false};
        for (int i=0; i<len; ++i) {
            switch (message_names[i]) {
                case SENSOR_OBSTACLE_DISTANCE:
                    sensor_data.obstacle_distance = messages[i];
                    found_obstacle_distance = true;
                    cout << "Read obstacle distance: "
                         << sensor_data.obstacle_distance << endl;
                    break;
                case SENSOR_LEFT_DRIVING_DISTANCE:
                    left_driving_distance = messages[i];
                    cout << "Read left driving distance: "
                         << messages[i] << endl;
                    break;
                case SENSOR_RIGHT_DRIVING_DISTANCE:
                    right_driving_distance = messages[i];
                    cout << "Read right driving distance: "
                         << messages[i] << endl;
                    break;
                case SENSOR_LEFT_SPEED:
                    left_speed = messages[i];
                    cout << "Read left speed: "
                         << messages[i] << endl;
                    break;
                case SENSOR_RIGHT_SPEED:
                    right_speed = messages[i];
                    cout << "Read right speed: "
                         << messages[i] << endl;
                    break;
                default:
                    cout << "Warning: Unexpected value read from sensor module "
                         << "value = " << messages[i] << endl;
                    break;
            }
        }

        // Calculate means
        if (left_driving_distance && right_driving_distance) {
            sensor_data.driving_distance = (left_driving_distance + right_driving_distance) / 2;
        } else if (left_driving_distance) {
            sensor_data.driving_distance = left_driving_distance;
        } else if (right_driving_distance) {
            sensor_data.driving_distance = right_driving_distance;
        } else {
            cout << "Warning: No driving distance recieved" << endl;
        }
        if (left_speed != MAX_INT && right_speed != MAX_INT) {
            sensor_data.speed = (left_speed + right_speed) / 2;
        } else if (left_speed != MAX_INT) {
            sensor_data.speed = left_speed;
        } else if (right_speed != MAX_INT) {
            sensor_data.speed = right_speed;
        } else {
            cout << "Warning: No speed recieved" << endl;
        }
        if (!found_obstacle_distance) {
            cout << "Warning: No obstacle distance recieved" << endl;
        }

        // TODO should this be 1 if missing some data?
        return 0;
    } else if (len == 0) {
        cout << "Warning: Slave has no new data." << endl;
        return 1;
    } else {
        cout << "I2C Error: " << len << endl;
        return -1;
    }

}

