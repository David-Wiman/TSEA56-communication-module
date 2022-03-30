#ifndef I2C_H
#define I2C_H

void i2c_init();
void i2c_set_slave_addr(int slave_addr);
void i2c_read();
void i2c_write();

#endif  // I2C_H
