#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "avr_i2c.h"

// master writes: Slave Receive mode (SR)
// master reads: Slave Transmit mode (ST)

#define I2C_SR_START 0x60  // Own SLA+W has been received; ACK has been returned
#define I2C_SR_DATA 0x80  // Previously addressed with own SLA+W; data has been received; ACK has been returned
#define I2C_SR_DATA_NACK 0x88  // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
#define I2C_SR_STOP 0xA0  // A STOP condition or repeated START condition has been received while still addressed as Slave

volatile uint8_t i2c_in_buffer[32];
volatile int i2c_in_ptr = 0;

void I2C_init() {
    TWAR = 0x50 << 1;  // Set slave address
    // Start Slave Listening: Clear TWINT Flag, Enable ACK, Enable TWI, TWI Interrupt Enable
    TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);

	// Default initial values
    TWDR = 0x00;
	i2c_new_data = false;
}

int I2C_unpack(uint16_t *message_names, uint16_t *messages) {
    // The arrays must be long enough for all possible message types
    // Use length 16 to be safe

    int msg_idx = 0;

    // Translate the 8 bit packages to 16 bit data, and put them in the place for
    for (int i=0; i<i2c_in_ptr/2; i++) {
        uint16_t packet = (uint16_t)(i2c_in_buffer[2*i] << 8) | i2c_in_buffer[2*i+1];
        printf(packet);
        if ((packet & 0xfff0) == 0xfff0) {
            // Message name
            message_names[msg_idx] = packet;
        } else {
            // Message
            messages[msg_idx++] = packet;
        }
    }
    i2c_in_ptr = 0;
    return msg_idx;
}

ISR(TWI_vect) {
    cli();
    uint8_t status_code;
    status_code = TWSR & 0xf8;
    switch (status_code) {
        case I2C_SR_START:
            // SLA+W has been received (slave is now reading)
            TWCR |= (1<<TWINT) | (1<<TWEA) | (1<<TWIE);
            break;
        case I2C_SR_DATA:
            // Data package received. Add to buffer
            i2c_in_buffer[i2c_in_ptr++] = TWDR;
            TWCR |= (1<<TWINT) | (1<<TWEA) | (1<<TWIE);
            break;
        case I2C_SR_STOP:
            // Last byte received
            i2c_new_data = true;
            TWCR |= (1<<TWINT) | (1<<TWIE);
            break;
        default:
            printf("Unrecognised status code\n");
            break;
    }

    sei();
}
