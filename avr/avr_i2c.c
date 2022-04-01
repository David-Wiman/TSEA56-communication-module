#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>

// master writes: Slave Receive mode (SR)
// master reads: Slave Transmit mode (ST)

#define I2C_SR_START 0x60  // Own SLA+W has been received; ACK has been returned
#define TEMP 0x68  // Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
#define TEMP 0x70  // General call address has been received; ACK has been returned
#define TEMP 0x78  // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
#define I2C_SR_DATA 0x80  // Previously addressed with own SLA+W; data has been received; ACK has been returned
#define I2C_SR_DATA_NACK 0x88  // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
#define TEMP 0x90  // Previously addressed with general call; data has been re- ceived; ACK has been returned
#define TEMP 0x98  // Previously addressed with general call; data has been received; NOT ACK has been returne
#define I2C_SR_STOP 0xA0  // A STOP condition or repeated START condition has been received while still addressed as Slave

void I2C_init() {
	TWAR = 0x50 << 1;  // Set slave address
	//TWCR = (1 << TWEA) | (1 << TWEN) | (1<<TWIE);
	// Start Slave Listening: Clear TWINT Flag, Enable ACK, Enable TWI, TWI Interrupt Enable
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);

	TWDR = 0x00;  // Default Initial Value
}

ISR(TWI_vect) {
	cli();
	uint8_t status_code, newTWCR;
	status_code = TWSR & 0xf8;
	switch (status_code) {
		case I2C_SR_START:
		TWCR |= (1<<TWINT) | (1<<TWEA) | (1<<TWIE);
		break;
		case I2C_SR_DATA:
		TWCR |= (1<<TWINT) | (1<<TWEA) | (1<<TWIE);
		break;
		case I2C_SR_STOP:
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
	while (1) {
		x++;
	}
}
