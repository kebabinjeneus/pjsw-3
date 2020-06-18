#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

// eigen headers
#include "headers/usart.h"
#include "headers/motor.h"

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

int input = 0;

void motorenAanzetten();

ISR(USART1_RX_vect){
    char henk = UDR1;
    switch(henk){
        case 'q':
            input = 0;
            break;
        case 'w':
            input = 1;
            break;
        case 's':
            input = 2;
            break;
        case 'a':
            input = 3;
            break;
        case 'd':
            input = 4;
            break;
    }
}

void motorenAanzetten() {
	switch(input) {
		case 0: setMotorSpeeds(0, 0);
			break;
		case 1: setMotorSpeeds(100, 100);
			break;
		case 2: setMotorSpeeds(-100, -100);
			break;
		case 3: setMotorSpeeds(-50, 50);
			break;
		case 4: setMotorSpeeds(50, -50);
	}
}

/*ISR(USART1_RX_vect) {

	char henk = UDR1;
	switch(henk) {
		case 'q': 
			  setMotorSpeeds(0, 0);
			  UDR1 = henk;
			  break;
		case 'w':
			  setMotorSpeeds(100, 100);
			  UDR1 = henk;
			  break;
		case 'a': 
			  setMotorSpeeds(-50, 50);
			  UDR1 = henk;
			  break;
		case 's': 
			  setMotorSpeeds(-100, -100);
			  UDR1 = henk;
			  break;
		case 'd': 
			  setMotorSpeeds(50, -50);
			  UDR1 = henk;
			  break;
		default:
			  DDRC = 0b00000010;
			  PORTC = (1<<PORTC7);
	}
}*/
 
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
