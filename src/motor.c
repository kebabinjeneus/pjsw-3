#define F_CPU 16000000

#include <util/delay.h>
#include <avr/io.h>

#define lmot 16 //digital
#define rmot 15 //digital
#define lsp 10  //analog/pwm
#define rsp 9   //analog/pwm


void init() {
  //Motors LOW is naar voren, HIGH is naar achter


  // Timer 1 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz

  DDRB |= (1 << DDB1) | (1 << DDB2) | (1 << DDB5) | (1 << DDB6); //pb1 en pb2 zijn outputs

  PORTB &= ~( (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB5) | (1 << PORTB6));

  TCCR1A = 0b10100000;
  TCCR1B = 0b00010001;
  ICR1 = 400;
  OCR1A = 0;
  OCR1B = 0;
}

int main() {
	init();
  _delay_ms(100);
  OCR1A = 200;
  OCR1B = 200;

  PORTB |= (1 << PORTB1) | (1 << PORTB2);
  
  _delay_ms(100000);
}
