#include <cstdint>
#include <chrono>
#include <thread>

#include "drivedata.h"
#include "raspi_common.h"

#ifndef COMMUNICATION_MODULE_H
#define COMMUNICATION_MODULE_H

class CommunicationModule {
public:
    CommunicationModule(int fps);

    static int get_sensor_data(sensor_data_t &sensor_data);
    static int get_steer_data(steer_data_t &steer_data);
    static void send_manual_instruction(uint16_t throttle, uint16_t steering);
    static void send_auto_instruction(reference_t ref, int speed, int lateral_position);
    static void send_regulation_constants(
            int steering_KP, int steeringKD,
            int speedKP, int speedKI,
            int turnKP, int turnKD);
    void throttle();

private:
    int cycle_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

#endif  // COMMUNICATOIN_MODULE_H
