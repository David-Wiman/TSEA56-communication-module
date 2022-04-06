#ifndef I2C_H
#define I2C_H

void i2c_init();
void i2c_close();
void i2c_set_slave_addr(int slave_addr);
int i2c_read(uint16_t *message_names, uint16_t *messages);
// Buffer can be max 16 long
void i2c_write(uint16_t *buffer, int len);

#endif  // I2C_H
