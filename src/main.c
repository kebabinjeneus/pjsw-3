#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// project includes
#include "headers/usart.h"
#include "headers/motor.h"
#include "headers/encoder.h"

int main() {
	// initialisaties
	USART_init();
	MOTOR_init();
	ENCODER_init();
	
	// init knopjes
	// A
	DDRB &= ~(1 << PORTB3);	// Portb3 input
	PORTB |= 1 << PORTB3;	// pullup portb3
	PCICR |= 1 << PCIE0;	// interrupt aanzetten
	PCMSK0 |= 1 << PCINT3;	// ^^
	sei();

	// main loop
	while(1) {
		test10cm(0); // test of de robot 10cm moet rijden
		
		//writeInt(getEncoderLeft());
		//writeString(";");
		//writeInt(getEncoderRight());
		//writeString(" | ");
	}

	return 0;
}

