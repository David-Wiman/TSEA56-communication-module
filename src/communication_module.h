#include <cstdint>
#include <chrono>
#include <thread>
#include <atomic>

#include "connection_data.h"
#include "raspi_common.h"

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
    CommunicationModule();

    CommunicationModule(const CommunicationModule&) = delete;
    CommunicationModule operator=(const CommunicationModule&) = delete;

    void read_sensor_data(sensor_data_t &sensor_data);
    void read_steer_data(steer_data_t &steer_data);
    void write_manual_instruction(uint16_t throttle, int16_t steering);
    void write_auto_instruction(reference_t ref, uint16_t speed, int16_t lateral_position);
    void write_regulation_constants(
            int steeringKP, int steeringKD,
            int speedKP, int speedKI,
            int turnKD, int angleOffset);
    void write_regulation_constants(ParameterConfiguration const& params);
    void throttle(unsigned max_fps);

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

#endif  // COMMUNICATOIN_MODULE_H
