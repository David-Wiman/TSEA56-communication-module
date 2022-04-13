#include <cstdint>

#include "drivedata.h"

#ifndef COMMUNICATION_MODULE_H
#define COMMUNICATION_MODULE_H

class CommunicationModule {
public:
    CommunicationModule() {}

    static int get_sensor_data(sensor_data_t &sensor_data);
    static void send_manual_instruction(uint16_t throttle, uint16_t steering);
};

#endif  // COMMUNICATOIN_MODULE_H
