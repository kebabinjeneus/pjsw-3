#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

// eigen headers
#include "headers/usart.h"
#include "headers/motor.h"
#include "headers/encoder.h"
#include "headers/sensoren.h"

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

int input = -1;

// usart interrupt
ISR(USART1_RX_vect){
	// stel het nummer van de input in op basis van de ingedrukte toets.
	switch(UDR1){
		case 'q': // stop met rijden
			input = 0;
			break;
		case 'w': // rijd vooruit
			input = 1;
			resetEncoderValues(); // nodig voor rechtdoor rijden
			break;
		case 's': // rijd achteruit
			input = 2;
			resetEncoderValues(); // nodig voor rechtdoor rijden
			break;
		case 'a': // draai naar links
			input = 3;
			break;
		case 'd': // draai naar rechts
			input = 4;
			break;
		case 'g': // gyro mode: robot draait terug naar aanroep positie
			input = 5;
			break;
		case 'p': // print debug info sensoren
			input = 99;
			break;
	}	
}

void checkInput() {
	// voert buiten de interrupt de acties uit die voor de input van het toetsenbord staan.
	switch(input) {
		case -1: break; // verander niks
		case 0: setMotorSpeeds(0, 0);
			input = -1;
			break;
		case 1: setMotorCorrectie(100, 100);
			break;
		case 2: setMotorCorrectie(-100, -100);
			break;
		case 3: setMotorSpeeds(-100, 100);
			break;
		case 4: setMotorSpeeds(100, -100);
			break;
		case 5: gyro();
			break;
		case 99: // debug //
			setMotorSpeeds(0, 0);
			printGyroValues();
			input = -1;
			break;
	}
}

void USART_init(){
	// set baud rate
	UBRR1H = (unsigned char)(BAUD_PRESCALLER>>8);
	UBRR1L = (unsigned char)(BAUD_PRESCALLER);
	
	// enable receiver / transmitter
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1) | (1<<TXCIE1);

	// set frame format: 8data 1stop bit
	UCSR1C = (3<<UCSZ10);
}

// wacht tot volgend character gezonden kan worden 
// en stop het volgende character in het zendregister
void writeChar(char data) {
	while(!(UCSR1A & (1<<UDRE1)));
        UDR1 = data;
}
 
// stuurt string per character naar writeChar
void writeString(char* Pstring)i {
	while(*Pstring != 0x00){
		writeChar(*Pstring);
		Pstring++;
	}
}

// zet int om naar String
void writeInt(int i) { 
	char buffer[8];
	itoa(i,buffer,10);
	writeString(buffer);
} 
