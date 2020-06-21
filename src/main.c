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
//#include "headers/uphill.h"
//#include "headers/gyro.h"

int main() {
	// initialisaties
	USART_init(); // usart.c
	MOTOR_init(); // motor.c
	ENCODER_init(); // encoder.c
	// starten sensoren
	// sensoren.c
	TWI_init();
	initGyro(0xD6, 0xD7);
	initAccelero(0x36, 0x37);
	

	// init knopje A
	DDRB &= ~(1 << PORTB3); // Portb3 input
	PORTB |= (1 << PORTB3);// | (1 << PORTB0);	// pullup portb3
	PCICR |= 1 << PCIE0;	// interrupt aanzetten
	PCMSK0 |= 1 << PCINT3;	// ^^
	sei();

	// stelt ddr op lampje in
	DDRC = 1 << PORTC7;
	
	// main loop
	while(1) {
		// checkt of knop A ingedrukt is en daarom 10cm moet rijden
		test10cm(0); // code in motor.c
		// kijkt welke toetsenbord input uitgevoerd moet worden
		checkInput(); // code in usart.c
		
		// knippert het lampje om te tonen of de robot nog werkt.
		PORTC ^= 1 << PORTC7;
	}

	return 0;
}
