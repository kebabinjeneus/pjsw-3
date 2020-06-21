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

ISR(USART1_RX_vect){
    switch(UDR1){
        case 'q':
            input = 0;
            break;
        case 'w':
            input = 1;
	    resetEncoderValues();
            break;
        case 's':
            input = 2;
	    resetEncoderValues();
            break;
        case 'a':
            input = 3;
            break;
        case 'd':
            input = 4;
            break;
	case 'g':
	    input = 5;
	    break;
	case 'u':
	    input = 6;
	    break;
	case 'p':
	    input = 99;
	    break;
    }	
}

void checkInput() {
	switch(input) {
		case -1: break;
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
		case 5:
			gyro();
			break;
		case 6:
			faceUphill();
			break;
		case 99:
			printAcceleroValues();
			//printGyroValues();
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

	// set frame format: 8data 2stop bit
	//UCSR1C = (1<<USBS1) | (3<<UCSZ10);	// geen idee of dit beter is
	UCSR1C = (3<<UCSZ10);			// maar dit werkt
}
 
void writeChar(char data){
	while(!(UCSR1A & (1<<UDRE1)));
        UDR1 = data;
}
 
void writeString(char* Pstring){
	while(*Pstring != 0x00){
		writeChar(*Pstring);
		Pstring++;
	}
}

void writeInt(int i) {                            //zet int om naar String
	char buffer[8];
	itoa(i,buffer,10);
	writeString(buffer);
} 
