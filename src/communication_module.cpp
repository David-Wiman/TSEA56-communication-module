#include "communication_module.h"
#include "log.h"

extern "C" {
    #include "i2c.h"
    #include "i2c_common.h"
}

#include <sstream>

#define MAX_INT 65535

using namespace std;

CommunicationModule::CommunicationModule(int fps, double sensor_read_wait, double steering_read_wait)
: cycle_time{1/fps}, start_time{chrono::high_resolution_clock::now()},
  i2c_bus_running{true}, sensor_read_wait{sensor_read_wait}, steering_read_wait{steering_read_wait} {
    Logger::log(INFO, __FILE__, "COM", "Initiating communication module");
    i2c_manager_thread = new thread(&CommunicationModule::i2c_manager, this);
}

CommunicationModule::~CommunicationModule() {
    i2c_bus_running.store(false);
    i2c_manager_thread->join();
    delete i2c_manager_thread;
}

void CommunicationModule::enqueue_manual_instruction(uint16_t throttle, int16_t steering) {
    Logger::log(DEBUG, __FILE__, "COM", "Enqueueing manual instruction");
    auto data = new vector<uint16_t>{
        STEERING_MANUAL_GAS, package_signed(throttle),
        STEERING_MANUAL_ANG, package_signed(steering)
    };
    i2c_out_t packet{STEERING_MODULE_SLAVE_ADDRESS, data};
    i2c_out_buffer.enqueue(packet);
}

void CommunicationModule::enqueue_auto_instruction(
        reference_t ref, uint16_t speed, int16_t lat) {
    Logger::log(DEBUG, __FILE__, "COM", "Enqueueing auto instruction");
    auto data = new vector<uint16_t>{
        STEERING_CUR_VEL, package_signed(speed),
        STEERING_REF_VEL, package_signed(ref.speed),
        STEERING_CUR_ANG, package_signed(ref.angle),
        STEERING_CUR_LAT, package_signed(lat),
        STEERING_REGULATION_MODE, package_signed(ref.regulation_mode)
    };
    i2c_out_t packet{STEERING_MODULE_SLAVE_ADDRESS, data};
    i2c_out_buffer.enqueue(packet);
}

void CommunicationModule::enqueue_regulation_constants(
        int steeringKP, int steeringKD, int speedKP,
        int speedKI, int turnKP, int turnKD) {
    Logger::log(DEBUG, __FILE__, "COM", "Enqueueing regulation constants");
    auto data = new vector<uint16_t>{
        STEERING_STEERING_KP, package_signed(steeringKP),
        STEERING_STEERING_KD, package_signed(steeringKD),
        STEERING_SPEED_KP, package_signed(speedKP),
        STEERING_SPEED_KI, package_signed(speedKI),
        STEERING_TURN_KP, package_signed(turnKP),
        STEERING_TURN_KD, package_signed(turnKD)
    };
    i2c_out_t packet{STEERING_MODULE_SLAVE_ADDRESS, data};
    i2c_out_buffer.enqueue(packet);
}

void CommunicationModule::i2c_manager() {
    Logger::log(INFO, __FILE__, "I2C", "I2C manager initiated");

    auto sensor_read_time = chrono::high_resolution_clock::now();
    auto steering_read_time = chrono::high_resolution_clock::now();
    while (i2c_bus_running.load()) {

        size_t i2c_out_waiting = i2c_out_buffer.size();
        if (i2c_out_waiting > 2) {
            stringstream ss;
            ss << i2c_out_waiting << " transactions waiting in out buffer";
            Logger::log(WARNING, __FILE__, "I2C", ss.str());
        }

        auto now = chrono::high_resolution_clock::now();
        double since_sensor_read = chrono::duration<double, std::milli>(now-sensor_read_time).count();
        double since_steering_read = chrono::duration<double, std::milli>(now-steering_read_time).count();
        if (since_sensor_read >= sensor_read_wait) {
            sensor_read_time = chrono::high_resolution_clock::now();
            read_sensor_data();
        } else if (since_steering_read >= steering_read_wait) {
            steering_read_time = chrono::high_resolution_clock::now();
            // TODO
        } else if (!i2c_out_buffer.empty()) {
            // Write on the bus

            i2c_out_t packet = i2c_out_buffer.dequeue();
            i2c_set_slave_addr(packet.slave_address);

            // This extracts the data as a pointer to the vector
            vector<uint16_t> *vec = packet.data;
            // This extracts a pointer to the first value, aka a C array
            uint16_t *buffer = &((*vec)[0]);
            int len = i2c_write(buffer, vec->size());
            if (len == -1) {
                stringstream ss;
                ss << "Failed to write to slave " << packet.slave_address;
                Logger::log(ERROR, __FILE__, "COM", ss.str());
            } else {
                stringstream ss;
                ss << "Wrote " << len << " bytes to slave " << packet.slave_address;
                Logger::log(DEBUG, __FILE__, "COM", ss.str());
            }
            delete vec;
        }
        // Wait a while to increase stability
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    Logger::log(INFO, __FILE__, "COM", "Closing I2C manager");

    // Clear all remaining data
    while (!i2c_out_buffer.empty()) {
        i2c_out_t packet = i2c_out_buffer.dequeue();
        delete packet.data;
    }
}

void CommunicationModule::update_sensor_data(sensor_data_t &sensor_data) {
    lock_guard<mutex> lk(sensor_data_mtx);
    sensor_data = sensor_data_buffer;
}

void CommunicationModule::read_sensor_data() {
    i2c_set_slave_addr(SENSOR_MODULE_SLAVE_ADDRESS);
    uint16_t message_names[16];
    uint16_t messages[16];
    Logger::log(DEBUG, __FILE__, "I2C", "Read sensor data");
    int len = i2c_read(message_names, messages);

    lock_guard<mutex> lk(sensor_data_mtx);

    if (len > 0) {
        uint16_t left_driving_distance{0};
        uint16_t right_driving_distance{0};
        uint16_t left_speed{MAX_INT};
        uint16_t right_speed{MAX_INT};
        bool found_obstacle_distance{false};
        for (int i=0; i<len; ++i) {
            switch (message_names[i]) {
                case SENSOR_OBSTACLE_DISTANCE:
                    sensor_data_buffer.obstacle_distance = messages[i];
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
            sensor_data_buffer.driving_distance = (left_driving_distance + right_driving_distance) / 2;
        } else if (left_driving_distance) {
            sensor_data_buffer.driving_distance = left_driving_distance;
        } else if (right_driving_distance) {
            sensor_data_buffer.driving_distance = right_driving_distance;
        } else {
            Logger::log(WARNING, __FILE__, "I2C", "No driving distance recieved");
        }
        if ((left_speed != MAX_INT) && (right_speed != MAX_INT)) {
            sensor_data_buffer.speed = (left_speed + right_speed) / 2;
        } else if (left_speed != MAX_INT) {
            sensor_data_buffer.speed = left_speed;
        } else if (right_speed != MAX_INT) {
            sensor_data_buffer.speed = right_speed;
        } else {
            Logger::log(WARNING, __FILE__, "I2C", "No speed recieved");
        }
        if (!found_obstacle_distance) {
            Logger::log(WARNING, __FILE__, "I2C", "No obstacle distance recieved");
        }

        // Log sensor data
        stringstream ss;
        ss << "Sensor data:"
           << "\n\tobstacle distance: " << sensor_data_buffer.obstacle_distance
           << "\n\tdriving distance: " << sensor_data_buffer.driving_distance
           << "\n\tspeed: " << sensor_data_buffer.speed;
        Logger::log(DEBUG, __FILE__, "I2C", ss.str());

    } else if (len == 0) {
        Logger::log(WARNING, __FILE__, "I2C", "Sensor module has no new data.");
    } else if (len == -2) {
        Logger::log(ERROR, __FILE__, "I2C", "Failed to get message length from sensor module");
    } else {
        Logger::log(ERROR, __FILE__, "I2C", "Can't read from sensor module");
    }
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

