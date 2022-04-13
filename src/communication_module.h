#include <cstdint>

#ifndef COMMUNICATION_MODULE_H
#define COMMUNICATION_MODULE_H

class CommunicationModule {
public:
    CommunicationModule() {}

    static void send_manual_instruction(uint16_t throttle, uint16_t steering);
};

#endif  // COMMUNICATOIN_MODULE_H
