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
//void SENSOREN_init();
//void sensor_update();
#include "headers/uphill.h"
#include "headers/gyro.h"

int main() {
	// initialisaties
	USART_init();
	MOTOR_init();
	ENCODER_init();
	//initGyro(0b11010110, 0b11010111);
	
	// init knopjes
	// A & C
	DDRB &= ~(1 << PORTB3) && ~(1 << PORTB0);	// Portb3 & Portb0 input
	PORTB |= (1 << PORTB3) | (1 << PORTB0);	// pullup portb3
	PCICR |= 1 << PCIE0;	// interrupt aanzetten
	PCMSK0 |= 1 << PCINT3;	// ^^
	sei();
	// B
	DDRD &= ~(1 << PORTD5);
	PORTD |= (1 << PORTD5);

	// main loop
	while(1) {
		test10cm(0); // test of de robot 10cm moet rijden	
		motorenAanzetten(); // test of motoren aanmoeten volgens toetsenbord
		//printGyroValues();
		//readGyroValues();
	}

	return 0;
}
