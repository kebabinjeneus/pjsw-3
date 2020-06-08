#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <util/delay.h>

// project includes
#include "headers/usart.h"

int main() {
	// initialisaties
	USART_init();

	// main loop
	while(1) {
		writeString("test");
		_delay_ms(100);
	}
	return 0;
}
