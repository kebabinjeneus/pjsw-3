#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <util/delay.h>

// project includes
#include "headers/usart.h"
#include "headers/motor.h"

int main() {
	// initialisaties
	USART_init();
	MOTOR_init();

	// main loop
	while(1) {
		writeString("vooruit\n");
		setMotorSpeeds(100, 100);
		_delay_ms(3000);
		writeString("stop\n");
		setMotorSpeeds(0, 0);
		_delay_ms(1000);
		writeString("achteruit\n");
		setMotorSpeeds(-100,-100);
		_delay_ms(3000);
		writeString("stop\n");
		setMotorSpeeds(0, 0);
		_delay_ms(1000);

	}

	return 0;
}
