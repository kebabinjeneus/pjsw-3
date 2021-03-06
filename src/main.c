#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// project includes
#include "headers/usart.h"
#include "headers/motor.h"
#include "headers/encoder.h"
#include "headers/sensoren.h"

int main() {
	// initialisaties
	USART_init(); // usart.c
	MOTOR_init(); // motor.c
	ENCODER_init(); // encoder.c
	// i2c | sensoren.c
	TWI_init();
	initGyro(0b11010110, 0b11010111);
	
	// init knopjes
	// A & C
	DDRB &= ~(1 << PORTB3);	// Portb3 & Portb0 input
	PORTB |= (1 << PORTB3);	// pullup portb3
	PCICR |= 1 << PCIE0;	// interrupt aanzetten
	PCMSK0 |= 1 << PCINT3;	// ^^
	sei();
	
	// lampje aanzetten na initialisatie
	// als indicatie dat de robot klaar is voor gebruik.
	DDRC = 1 << PORTC7;
	PORTC = 1 << PORTC7;
	
	// main loop
	while(1) {
		// checkt of knop A ingedrukt is en daarom 10cm moet rijden
		test10cm(0); // motor.c
		// kijkt welke toetsenbord input uitgevoerd moet worden
		checkInput(); // usart.c
	}

	return 0;
}
