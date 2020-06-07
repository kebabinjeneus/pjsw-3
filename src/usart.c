#define F_CPU 16000000UL

// libraries
#include <avr/io.h>

// eigen headers
#include "headers/usart.h"

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
 
 
/*
void USART_init(void);
void writeChar(char data);
void writeString(char* Pstring);

// testcode origineel usart gebruik, main kan met pensioen.
int main(void){
    USART_init();                                    //USART initialiseren
 
    while(1){        
        writeString("ik wil dood");                        //Stuurt string naar terminal
        _delay_ms(100);                            //Iedere 100 milisec 
    }    
    return 0;
}
*/
 
void USART_init(){
	// set baud rate
	UBRR1H = (unsigned char)(BAUD_PRESCALLER>>8);
	UBRR1L = (unsigned char)(BAUD_PRESCALLER);
	
	// enable receiver / transmitter
	UCSR1B = (1<<RXEN1) | (1<<TXEN1);

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
