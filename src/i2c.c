#include <unistd.h>             // Needed for I2C port
#include <fcntl.h>              // Needed for I2C port
#include <sys/ioctl.h>          // Needed for I2C port
#include <linux/i2c-dev.h>      // Needed for I2C port

#include <stdio.h>
#include <stdint.h>

int file_i2c;

void i2c_init() {
    char *filename = (char*)"/dev/i2c-1";
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        // ERROR HANDLING: you can check errno to see what went wrong
        printf("Failed to open the i2c bus\n");
        return;
    }
}

void i2c_set_slave_addr(int slave_addr) {
    if (ioctl(file_i2c, I2C_SLAVE, slave_addr) < 0) {
        printf("Failed to set slave address\n");
        // ERROR HANDLING; you can check errno to see what went wrong
        return;
    }
}

void i2c_read(int16_t *buffer) {
    int len = 2;  // We always send 2 bytes
    if (read(file_i2c, buffer, len) != len) {
        // ERROR HANDLING: i2c transaction failed
        printf("Failed to read from the i2c bus.\n");
    } else {
        printf("Data read: %s\n", buffer);
    }
}

void i2c_write(uint16_t *buffer, int len) {
    printf("I2C Write\n");
    // Convert buffer with 16 bit data to one with 8 bit data
    uint8_t buffer8[32];
    for (int i=0; i<len; ++i) {
        buffer8[2*i] = (uint8_t)(buffer[i] >> 8);
        buffer8[2*i+1] = (uint8_t)(buffer[i] & 0x00ff);
    }
    for (int i=0; i<len*2; ++i) {
        printf("0x%x, ", buffer8[i]);
    }
    printf("\n");
    // write() returns the number of bytes actually written, if it doesn't
    // match then an error occurred (e.g. no response from the device)rite
    if (write(file_i2c, buffer8, len*2) != len*2) {
        // ERROR HANDLING: i2c transaction failed
        printf("Failed to write to the i2c bus.\n");
    }
}

