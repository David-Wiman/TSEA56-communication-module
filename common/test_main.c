#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "avr_i2c.h"
#include "i2c_common.h"

int main(void){

    // Enable global interrupts
    sei();

    I2C_init();

    uint16_t message_names[16];
    uint16_t messages[16];

    while (true) {
        if (i2c_new_data) {
            i2c_new_data = false;
            int len = I2C_unpack(message_names, messages);
            for (int i=0; i<len; ++i) {
                volatile int16_t val = restore_signed(messages[i]);
                printf(message_names[i], val);
            }
        }
    }
}
