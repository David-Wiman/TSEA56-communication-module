#ifndef AVR_I2C_H
#define AVR_I2C_H

#include <stdbool.h>

volatile bool i2c_new_data;

void I2C_init();

/* Unpack I2C from buffer.
 *
 * message_names and messages are arrays with length 16.
 * After calling I2C_unpack these arrays will be filled with the data from the
 * i2c buss.
 *
 * Return the number of messages (1-16)
 */
int I2C_unpack(uint16_t *message_names, uint16_t *messages);

#endif  // AVR_I2C_H
