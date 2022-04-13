#include "communication_module.h"
#include "i2c_common.h"
extern "C" {
    #include "i2c.h"
}

void CommunicationModule::send_manual_instruction(uint16_t throttle, uint16_t steering) {
    i2c_set_slave_addr(0x51);
    uint16_t buffer[] = {
        STEERING_MANUAL_GAS, package_signed(throttle),
        STEERING_MANUAL_ANG, package_signed(steering)
    };
    i2c_write(buffer, 4);
}

