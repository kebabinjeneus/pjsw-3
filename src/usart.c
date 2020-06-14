#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

// eigen headers
#include "headers/usart.h"

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)


ISR(USART1_RX_vect) {
	int henk = 5;
	henk = UDR1;
	UDR1 = henk;
}
 
void USART_init(){
	// set baud rate
	UBRR1H = (unsigned char)(BAUD_PRESCALLER>>8);
	UBRR1L = (unsigned char)(BAUD_PRESCALLER);
	
	// enable receiver / transmitter
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);

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
