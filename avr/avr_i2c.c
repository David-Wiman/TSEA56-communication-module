#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

// master writes: Slave Receive mode (SR)
// master reads: Slave Transmit mode (ST)

#define I2C_SR_START 0x60  // Own SLA+W has been received; ACK has been returned
//#define TEMP 0x68  // Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
//#define TEMP 0x70  // General call address has been received; ACK has been returned
//#define TEMP 0x78  // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
#define I2C_SR_DATA 0x80  // Previously addressed with own SLA+W; data has been received; ACK has been returned
#define I2C_SR_DATA_NACK 0x88  // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
//#define TEMP 0x90  // Previously addressed with general call; data has been re- ceived; ACK has been returned
//#define TEMP 0x98  // Previously addressed with general call; data has been received; NOT ACK has been returne
#define I2C_SR_STOP 0xA0  // A STOP condition or repeated START condition has been received while still addressed as Slave

volatile uint8_t I2C_IN_BUFFER[32];
volatile int I2C_IN_PTR = 0;
volatile bool i2c_new_data = false;

uint16_t message_names[16];
uint16_t messages[16];



void I2C_init() {
	TWAR = 0x50 << 1;  // Set slave address
	// Start Slave Listening: Clear TWINT Flag, Enable ACK, Enable TWI, TWI Interrupt Enable
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);

	TWDR = 0x00;  // Default Initial Value
}

void I2C_unpack(uint16_t *message_names, uint16_t *messages) {
	// The arrays must be long enough for all possible message types 
	// Use length 16 to be safe
	
	int msg_idx = 0;
	
	// Translate the 8 bit packages to 16 bit data, and put them in the place for 
	for (int i=0; i<I2C_IN_PTR/2; i++) {
		uint16_t packet = (uint16_t)(I2C_IN_BUFFER[2*i] << 8) | I2C_IN_BUFFER[2*i+1];
		printf(packet);
		if ((packet & 0xfff0) == 0xfff0) {
			// Message name
			message_names[msg_idx] = packet;
		} else {
			// Message
			messages[msg_idx++] = packet;
		}
	}
	I2C_IN_PTR = 0;
}

ISR(TWI_vect) {
	cli();
	uint8_t status_code;
	status_code = TWSR & 0xf8;
	switch (status_code) {
		case I2C_SR_START:
			TWCR |= (1<<TWINT) | (1<<TWEA) | (1<<TWIE);
			break;
		case I2C_SR_DATA:
			I2C_IN_BUFFER[I2C_IN_PTR++] = TWDR;
			TWCR |= (1<<TWINT) | (1<<TWEA) | (1<<TWIE);
			break;
		case I2C_SR_STOP:
			//i2c_new_data = true;
			I2C_unpack(message_names, messages);
			TWCR |= (1<<TWINT) | (1<<TWIE);
			break;
		case I2C_SR_DATA_NACK:
			printf("Error nack on data received\n");
			break;
		default:
			printf("Unrecognised status code\n");
			break;
	}
	
	sei();
}


/*******************************************************************************/
/* Main                                                                        */
/*******************************************************************************/

int main(void){

	// Enable global interrupts
	sei();
	
	I2C_init();

	volatile int x = 0;
	uint16_t message_names[16];
	uint16_t messages[16];
	
	while (1) {
		if (i2c_new_data) {
			i2c_new_data = false;
			I2C_unpack(message_names, messages);
		}
		x++;
	}
}
