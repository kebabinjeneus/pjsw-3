#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// project includes
#include "headers/usart.h"
#include "headers/motor.h"
#include "headers/encoder.h"

void rijd10cm();

ISR(PCINT3_vect) {
	writeString("hoi");
	if(~PINB & (1 << PORTB3)) {
		rijd10cm();
	}
	writeString("werken voor je zakgeld");
}

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


	//setMotorSpeeds(50, 0);

	// main loop
	while(1) {
		//writeInt(getEncoderLeft());
		//writeString(" | ");
		//_delay_ms(100);
	}

	return 0;
}

void rijd10cm() {
	int start = getEncoderLeft();
	setMotorSpeeds(400, 400);
	while(getEncoderLeft() < (start+983));
	setMotorSpeeds(0, 0);
}

