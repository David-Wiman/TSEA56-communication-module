#ifndef AVR_I2C_H
#define AVR_I2C_H

#include <stdbool.h>

/* Global bool which is set when new data is available. */
volatile bool i2c_new_data;

void I2C_init(uint8_t slave_address);

/* Pack data for sending over i2c. Next time the master asks for data it will 
 * be read. */
void I2C_pack(uint16_t *message_names, uint16_t *messages, int len);

/* Unpack I2C from buffer.
 *
 * message_names and messages are arrays with length 16.
 * After calling I2C_unpack these arrays will be filled with the data from the
 * i2c buss.
 *
 * Return the number of messages (1-16) */
int I2C_unpack(uint16_t *message_names, uint16_t *messages);

#endif  // AVR_I2C_H
