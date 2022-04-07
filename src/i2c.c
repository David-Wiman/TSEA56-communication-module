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

void i2c_close() {
    close(file_i2c);
}

void i2c_set_slave_addr(int slave_addr) {
    if (ioctl(file_i2c, I2C_SLAVE, slave_addr) < 0) {
        printf("Failed to set slave address\n");
        // ERROR HANDLING; you can check errno to see what went wrong
        return;
    }
}

int i2c_read(uint16_t *message_names, uint16_t *messages) {
    uint8_t buffer8[32];
    int code = read(file_i2c, buffer8, 1);
    int len = 0;
    if (code == 1) {
        len = buffer8[0];
        printf("Slave wants to transmit %d bytes\n", len);
        if (len > 16)
            printf("Warning: Slave wants to transmit too many bytes.\n");
    } else {
        printf("Failed to get message length (code %d)\n", code);
        return -1;
    }

    code = read(file_i2c, buffer8, len);
    if (code == -1) {
        // ERROR HANDLING: i2c transaction failed
        printf("Failed to read from the i2c bus.\n");
    } else if (code == len) {
        int j = 0;
        for (int i=0; i<code; i+=2) {
            uint16_t message = (uint16_t)(buffer8[i]<<8) | buffer8[i+1];
            if ((message & 0xfff0) == 0xfff0) {
                message_names[j] = message;
            } else {
                messages[j++] = message;
            }
        }
        return j;
    } else {
        printf("Failed to read from i2c bus. ");
        printf("Only read %d of %d bytes.\n", code, len);
    }
    return code;
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
    int writen_bytes = write(file_i2c, buffer8, len*2);
    if (writen_bytes != len*2) {
        // ERROR HANDLING: i2c transaction failed
        if (writen_bytes != -1) {
            printf("Error while writing on i2c bus: Only %d/%d bytes writen\n", 
                    writen_bytes, len*2);
        } else {
            printf("Error while writing on i2c bus (code -1)\n");
        }
    }
}

