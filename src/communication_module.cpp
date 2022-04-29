#include "communication_module.h"
#include "log.h"

extern "C" {
    #include "i2c.h"
    #include "i2c_common.h"
}

#include <sstream>

#define MAX_INT 65535

using namespace std;

CommunicationModule::CommunicationModule(int fps)
: cycle_time{1/fps}, start_time{chrono::high_resolution_clock::now()} {
    Logger::log(INFO, __FILE__, "COM", "Initiating communication module");
}

void CommunicationModule::send_manual_instruction(uint16_t throttle, uint16_t steering) {
    i2c_set_slave_addr(STEERING_MODULE_SLAVE_ADDRESS);
    uint16_t buffer[] = {
        STEERING_MANUAL_GAS, throttle,
        STEERING_MANUAL_ANG, steering
    };
    i2c_write(buffer, 4);
}

void CommunicationModule::send_auto_instruction(
        reference_t ref, int speed, int lat) {
    i2c_set_slave_addr(STEERING_MODULE_SLAVE_ADDRESS);
    uint16_t buffer[] = {
        STEERING_CUR_VEL, package_signed(speed),
        STEERING_REF_VEL, package_signed(ref.speed),
        STEERING_CUR_ANG, package_signed(ref.angle),
        STEERING_CUR_LAT, package_signed(lat),
        STEERING_REGULATION_MODE, package_signed(ref.regulation_mode)
    };
    i2c_write(buffer, sizeof(buffer)/sizeof(uint16_t));
}

void CommunicationModule::send_regulation_constants(
        int steeringKP, int steeringKD, int speedKP,
        int speedKI, int turnKP, int turnKD) {
    i2c_set_slave_addr(STEERING_MODULE_SLAVE_ADDRESS);
    uint16_t buffer[] = {
        STEERING_STEERING_KP, package_signed(steeringKP),
        STEERING_STEERING_KD, package_signed(steeringKD),
        STEERING_SPEED_KP, package_signed(speedKP),
        STEERING_SPEED_KI, package_signed(speedKI),
        STEERING_TURN_KP, package_signed(turnKP),
        STEERING_TURN_KD, package_signed(turnKD)
    };
    i2c_write(buffer, sizeof(buffer)/sizeof(uint16_t));
}

int CommunicationModule::get_sensor_data(sensor_data_t &sensor_data) {
    i2c_set_slave_addr(SENSOR_MODULE_SLAVE_ADDRESS);
    uint16_t message_names[16];
    uint16_t messages[16];
    int len = i2c_read(message_names, messages);
    if (len > 0) {
        Logger::log(DEBUG, __FILE__, "I2C", "Read sensor data");
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
                    break;
                case SENSOR_LEFT_DRIVING_DISTANCE:
                    left_driving_distance = messages[i];
                    break;
                case SENSOR_RIGHT_DRIVING_DISTANCE:
                    right_driving_distance = messages[i];
                    break;
                case SENSOR_LEFT_SPEED:
                    left_speed = messages[i];
                    break;
                case SENSOR_RIGHT_SPEED:
                    right_speed = messages[i];
                    break;
                default:
                    Logger::log(WARNING, __FILE__, "I2C", "Unexpected value read from sensor module ");
                    Logger::log(WARNING, __FILE__, "I2C", messages[i]);
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
            Logger::log(WARNING, __FILE__, "I2C", "No driving distance recieved");
        }
        if ((left_speed != MAX_INT) && (right_speed != MAX_INT)) {
            sensor_data.speed = (left_speed + right_speed) / 2;
        } else if (left_speed != MAX_INT) {
            sensor_data.speed = left_speed;
        } else if (right_speed != MAX_INT) {
            sensor_data.speed = right_speed;
        } else {
            Logger::log(WARNING, __FILE__, "I2C", "No speed recieved");
        }
        if (!found_obstacle_distance) {
            Logger::log(WARNING, __FILE__, "I2C", "No obstacle distance recieved");
        }

        // Log sensor data
        stringstream ss;
        ss << "Sensor data:"
           << "\n\tobstacle distance: " << sensor_data.obstacle_distance
           << "\n\tdriving distance: " << sensor_data.driving_distance
           << "\n\tspeed: " << sensor_data.speed;
        Logger::log(DEBUG, __FILE__, "I2C", ss.str());

        // TODO should this be 1 if missing some data?
        return 0;
    } else if (len == 0) {
        Logger::log(WARNING, __FILE__, "I2C", "Sensor module has no new data.");
        return 1;
    } else if (len == -2) {
        Logger::log(ERROR, __FILE__, "I2C", "Failed to get message length from sensor module");
    } else {
        Logger::log(ERROR, __FILE__, "I2C", "Can't read from sensor module");
        return -1;
    }
    return 0;
}

void CommunicationModule::throttle() {
    const auto now = chrono::high_resolution_clock::now();
    double t_delta = chrono::duration<double, std::milli>(now-start_time).count();
    double cycle_time{200};
    stringstream ss{};
    ss << "Program cycle took " << t_delta << " ms";
    Logger::log(DEBUG, __FILE__, "COM", ss.str());
    ss.str("");
    if (t_delta < cycle_time) {
        ss << "Sleeping for " << cycle_time - t_delta << " ms";
        Logger::log(DEBUG, __FILE__, "COM", ss.str());
        int ms = static_cast<int>(cycle_time - t_delta);
        this_thread::sleep_for(chrono::milliseconds(ms));
    } else {
        Logger::log(WARNING, __FILE__, "COM", "Program cycle time exceeded");
    }
    start_time = chrono::high_resolution_clock::now();
}

