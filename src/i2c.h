#ifndef I2C_H
#define I2C_H

void i2c_init();
void i2c_set_slave_addr(int slave_addr);
void i2c_read();
// Buffer can be max 16 long
void i2c_write(uint16_t *buffer, int len);

#endif  // I2C_H
