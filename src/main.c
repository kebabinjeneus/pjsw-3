#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <util/delay.h>

// project includes
#include "headers/usart.h"
#include "headers/motor.h"
#include "headers/encoder.h"


int main() {
	// initialisaties
	USART_init();
	MOTOR_init();
	ENCODER_init();

	// main loop
	while(1) {
		writeInt(test);
	}

	return 0;
}
