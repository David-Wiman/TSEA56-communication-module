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

void i2c_write() {
    printf("I2C Write\n");
    uint16_t buffer[] = {0x0123, 0x4567, 0x89ab, 0xcdef};
    int len = 4;
    // Convert buffer with 16 bit data to one with 8 bit data
    uint8_t buffer8[32];
    for (int i=0; i<len; ++i) {
        buffer8[2*i] = (uint8_t)(buffer[i] >> 8);
        buffer8[2*i+1] = (uint8_t)(buffer[i] & 0x00ff);
    }
    printf("0x%x, 0x%x, 0x%x, 0x%x\n", buffer8[0], buffer8[1], buffer8[2], buffer8[3]);
    // write() returns the number of bytes actually written, if it doesn't
    // match then an error occurred (e.g. no response from the device)rite
    if (write(file_i2c, buffer8, len*2) != len*2) {
        // ERROR HANDLING: i2c transaction failed
        printf("Failed to write to the i2c bus.\n");
    }
}

void i2c_test() {

    printf("TESTING I2C\n");

    int file_i2c;
    int length;
    unsigned char buffer[60] = {0};

    //----- OPEN THE I2C BUS -----
    char *filename = (char*)"/dev/i2c-1";
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        // ERROR HANDLING: you can check errno to see what went wrong
        printf("Failed to open the i2c bus");
        return;
    }

    int addr = 0x5a;  // The I2C address of the slave
    if (ioctl(file_i2c, I2C_SLAVE, addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        // ERROR HANDLING; you can check errno to see what went wrong
        return;
    }


    //----- READ BYTES -----
    length = 4;  // Number of bytes to read
    // read() returns the number of bytes actually read, if it doesn't match
    // then an error occurred (e.g. no response from the device)
    if (read(file_i2c, buffer, length) != length) {
        // ERROR HANDLING: i2c transaction failed
        printf("Failed to read from the i2c bus.\n");
    } else {
        printf("Data read: %s\n", buffer);
    }


    //----- WRITE BYTES -----
    buffer[0] = 0x01;
    buffer[1] = 0x02;
    length = 2; // Number of bytes to w
    // write() returns the number of bytes actually written, if it doesn't
    // match then an error occurred (e.g. no response from the device)rite
    if (write(file_i2c, buffer, length) != length) {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Failed to write to the i2c bus.\n");
    }
}
