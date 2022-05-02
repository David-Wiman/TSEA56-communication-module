#include <cstdint>
#include <chrono>
#include <thread>
#include <atomic>

#include "drivedata.h"
#include "raspi_common.h"
#include "threaded_queue.h"

#ifndef COMMUNICATION_MODULE_H
#define COMMUNICATION_MODULE_H

namespace drive_mode {
    enum DriveMode {manual, semi_auto, full_auto};
}

typedef struct i2c_out_item {
    uint8_t slave_address;
    std::vector<uint16_t> *data;
} i2c_out_t;

class CommunicationModule {
public:
    CommunicationModule(int fps, double sensor_read_wait, double steering_read_wait);
    ~CommunicationModule();

    CommunicationModule(const CommunicationModule&) = delete;
    CommunicationModule operator=(const CommunicationModule&) = delete;

    void update_sensor_data(sensor_data_t &sensor_data);
    void update_steer_data(steer_data_t &steer_data);
    void enqueue_manual_instruction(uint16_t throttle, int16_t steering);
    void enqueue_auto_instruction(reference_t ref, uint16_t speed, int16_t lateral_position);
    void enqueue_regulation_constants(
            int steering_KP, int steeringKD,
            int speedKP, int speedKI,
            int turnKP, int turnKD);
    void throttle();

private:
    void i2c_manager();
    void read_sensor_data();
    void read_steer_data();
    int cycle_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    ThreadedQueue<i2c_out_t> i2c_out_buffer{};
    std::atomic<bool> i2c_bus_running;
    std::thread *i2c_manager_thread = nullptr;

    double sensor_read_wait;
    double steering_read_wait;
    sensor_data_t sensor_data_buffer{};
    std::mutex sensor_data_mtx{};
    steer_data_t steer_data_buffer{};
    std::mutex steer_data_mtx{};
};

#endif  // COMMUNICATOIN_MODULE_H
