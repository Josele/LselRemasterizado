/*
	LSEL 2014 i2c ref example
	@date March 2014
	@author Jose Martin


*/


#define I2C_ADDRESS 0x22 // Address must be different for each thing

#define trainIDmask 0b11000011

#include <avr/interrupt.h>
#include <util/delay.h>
#include "softuart.h"
#include "usiTwiSlave.h"
#include <stdbool.h>

static volatile uint8_t buffer = 0;
static volatile bool newData = false;



// A callback triggered when the i2c master attempts to read from a register.
uint8_t i2cReadFromRegister(uint8_t reg)
{
	switch (reg)
	{
		case 0: 
			if (newData) {
				newData = false;
				return buffer;
			}
				
			return trainIDmask; // Buffers the byte received from TX before sending it
			break;
			
		case 1:
			if (newData) {
				return 0x01;
			}
			else
			{
				return 0x00;
			}
		default:
			return 0xff;
	}
}

// A callback triggered when the i2c master attempts to write to a register.
void i2cWriteToRegister(uint8_t reg, uint8_t value)
{
	switch (reg)
	{
	}
}
	
int main(void)
{
	// Initialize software UART - remove if not needed
	softuart_init();
	
	// Initialize I2C module
	usiTwiSlaveInit(I2C_ADDRESS, i2cReadFromRegister, i2cWriteToRegister);
	
	sei();
	
	// Main loop, receive bytes all the time
	for (;;) {
		if (softuart_kbhit() != 0){	// Checks if a new byte is received from TX and stores it
			uint8_t rxed = softuart_getchar();  
			if ((rxed & trainIDmask) == 0b00000000 )
			{
				buffer = rxed;
				newData = true;
			}
		}	
	}
	return 0; /* never reached */
}
