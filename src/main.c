#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
 
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
 
void USART_init(void);
unsigned char receive(void);
void writeChar(unsigned char data);
void writeString(char* Pstring);
 
char String[]="test test test";  
 
int main(void){
    USART_init();                                    //USART initialiseren
 
    while(1){        
        writeString(String);                        //Stuurt string naar terminal
        _delay_ms(5000);                            //Iedere 5 sec 
    }    
     return 0;
}
 
void USART_init(void){
	// set baud rate
	UBRR1H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR1L = (uint8_t)(BAUD_PRESCALLER);
	
	// enable receiver / transmitter
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);

	// set frame format: 8data 2stop bit
	UCSR1C = (3<<UCSZ10);
}
 
void writeChar(unsigned char data){
     while(!(UCSR1A & (1<<UDRE1))) {
        UDR1 = data;
    }
 
}
 
void writeString(char* Pstring){
    while(*Pstring != 0x00){
        writeChar(*Pstring);
        Pstring++;
    }
}
