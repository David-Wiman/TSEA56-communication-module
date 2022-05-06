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
: cycle_time{1000/fps}, start_time{chrono::high_resolution_clock::now()} {
    Logger::log(INFO, __FILE__, "COM", "Initiating communication module");
    stringstream ss{};
    ss << "Throttle set to " << cycle_time << " ms";
    Logger::log(INFO, __FILE__, "COM", ss.str());
}

void CommunicationModule::write_manual_instruction(uint16_t throttle, int16_t steering) {
    Logger::log(DEBUG, __FILE__, "COM", "Writing manual instruction");
    uint16_t data[] = {
        STEERING_REGULATION_MODE, REGULATION_MODE_MANUAL,
        STEERING_MANUAL_GAS, package_signed(throttle),
        STEERING_MANUAL_ANG, package_signed(steering)
    };
    i2c_set_slave_addr(STEERING_MODULE_SLAVE_ADDRESS);
    int len = i2c_write(data, sizeof(data)/sizeof(uint16_t));
    if (len == -1) {
        Logger::log(ERROR, __FILE__, "Write", "Failed to write manual instruction");
    } else {
        Logger::log(DEBUG, __FILE__, "Write", "Succesfully wrote manual instruction");
    }
}


void CommunicationModule::write_auto_instruction(
        reference_t ref, uint16_t speed, int16_t lat) {
    Logger::log(DEBUG, __FILE__, "COM", "Writing auto instruction");
    uint16_t data[] = {
        STEERING_CUR_VEL, package_signed(speed),
        STEERING_REF_VEL, package_signed(ref.speed),
        STEERING_CUR_ANG, package_signed(ref.angle),
        STEERING_CUR_LAT, package_signed(lat),
        STEERING_REGULATION_MODE, package_signed(ref.regulation_mode)
    };
    i2c_set_slave_addr(STEERING_MODULE_SLAVE_ADDRESS);
    int len = i2c_write(data, sizeof(data)/sizeof(uint16_t));
    if (len == -1) {
        Logger::log(ERROR, __FILE__, "Write", "Failed to write auto instruction");
    } else {
        Logger::log(DEBUG, __FILE__, "Write", "Succesfully wrote auto instruction");
    }
}

void CommunicationModule::write_regulation_constants(
        int steeringKP, int steeringKD, int speedKP,
        int speedKI, int turnKP, int turnKD) {
    Logger::log(DEBUG, __FILE__, "COM", "Writing regulation constants");
    uint16_t data[] = {
        STEERING_STEERING_KP, package_signed(steeringKP),
        STEERING_STEERING_KD, package_signed(steeringKD),
        STEERING_SPEED_KP, package_signed(speedKP),
        STEERING_SPEED_KI, package_signed(speedKI),
        STEERING_TURN_KP, package_signed(turnKP),
        STEERING_TURN_KD, package_signed(turnKD)
    };
    i2c_set_slave_addr(STEERING_MODULE_SLAVE_ADDRESS);
    int len = i2c_write(data, sizeof(data)/sizeof(uint16_t));
    if (len == -1) {
        Logger::log(ERROR, __FILE__, "Write", "Failed to write regulation constants");
    } else {
        Logger::log(DEBUG, __FILE__, "Write", "Succesfully wrote regulation constats");
    }
}

void CommunicationModule::write_regulation_constants(ParameterConfiguration const& params) {
    write_regulation_constants(
            params.steering_kp, params.steering_kd,
            params.speed_kp, params.speed_ki,
            params.turn_kp, params.turn_kd);
}

void CommunicationModule::read_sensor_data(sensor_data_t &sensor_data) {
    i2c_set_slave_addr(SENSOR_MODULE_SLAVE_ADDRESS);
    uint16_t message_names[16];
    uint16_t messages[16];
    Logger::log(DEBUG, __FILE__, "I2C", "Read sensor data");
    int len = i2c_read(message_names, messages);

    if (len > 0) {
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

    } else if (len == 0) {
        Logger::log(WARNING, __FILE__, "I2C", "Sensor module has no new data.");
    } else if (len == -2) {
        Logger::log(ERROR, __FILE__, "I2C", "Failed to get message length from sensor module");
    } else {
        Logger::log(ERROR, __FILE__, "I2C", "Can't read from sensor module");
    }
}

void CommunicationModule::read_steer_data(steer_data_t &steer_data) {
    i2c_set_slave_addr(STEERING_MODULE_SLAVE_ADDRESS);
    uint16_t message_names[16];
    uint16_t messages[16];
    Logger::log(DEBUG, __FILE__, "I2C", "Read steer data");
    int len = i2c_read(message_names, messages);

    if (len > 0) {
        bool found_gas{false};
        bool found_steer_angle{false};
        for (int i=0; i<len; ++i) {
            switch (message_names[i]) {
                case STEERING_RETURN_GAS:
                    steer_data.gas = messages[i];
                    found_gas = true;
                    break;
                case STEERING_RETURN_ANG:
                    steer_data.steer_angle = restore_signed(messages[i]);
                    found_steer_angle = true;
                    break;
                default:
                    Logger::log(WARNING, __FILE__, "I2C", "Unexpected value read from steering module ");
                    Logger::log(WARNING, __FILE__, "I2C", messages[i]);
                    break;
            }
        }

        // Check if data recieved
        if (!found_gas) {
            Logger::log(WARNING, __FILE__, "I2C", "No gas recieved");
        }
        if (!found_steer_angle) {
            Logger::log(WARNING, __FILE__, "I2C", "No steer angle recieved");
        }

        // Log steer data
        stringstream ss;
        ss << "Steer data:"
           << "\n\tgas: " << steer_data.gas
           << "\n\tsteer_angle: " << steer_data.steer_angle;
        Logger::log(DEBUG, __FILE__, "I2C", ss.str());

        // TODO should this be 1 if missing some data?
    } else if (len == 0) {
        Logger::log(WARNING, __FILE__, "I2C", "Steering module has no new data.");
    } else if (len == -2) {
        Logger::log(ERROR, __FILE__, "I2C", "Failed to get message length from steering module");
    } else {
        Logger::log(ERROR, __FILE__, "I2C", "Can't read from steering module");
    }
}

void CommunicationModule::throttle(bool do_throttle) {
    const auto now = chrono::high_resolution_clock::now();
    double t_delta = chrono::duration<double, std::milli>(now-start_time).count();
    stringstream ss{};
    ss << "Program cycle took " << t_delta << " ms";
    Logger::log(DEBUG, __FILE__, "COM", ss.str());
    ss.str("");
    if (do_throttle && (t_delta < cycle_time)) {
        ss << "Sleeping for " << cycle_time - t_delta << " ms";
        Logger::log(DEBUG, __FILE__, "COM", ss.str());
        int ms = static_cast<int>(cycle_time - t_delta);
        this_thread::sleep_for(chrono::milliseconds(ms));
    } else {
        Logger::log(WARNING, __FILE__, "COM", "Program cycle time exceeded");
    }
    start_time = chrono::high_resolution_clock::now();
}

