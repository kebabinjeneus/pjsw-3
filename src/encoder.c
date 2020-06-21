#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <avr/interrupt.h>

// project include
#include "headers/encoder.h"
#include "headers/motor.h"

static volatile int lastLeftA;
static volatile int lastRightA;

static volatile int32_t leftCounter = 0;
static volatile int32_t rightCounter = 0;

//PE6 = right encoder xor = int6 interrupt
//PF0 = right encoder b

//PB4 = left encoder xor = PCint4 interrupt
//PE2 = left encoder b

//linker interrupt & knop A interrupt
ISR(PCINT0_vect) {
	// check of knop A is ingedrukt of dat het de encoder is
	if(~PINB & (1 << PORTB3)) {
		// set flag zodat bij volgende main loop iteratie rijd10cm uitgevoerd wordt.
		test10cm(1);
	}
	
	// niet knop A dus encoder interrupt:
	else {
		int channelB = PINE & (1 << PINE2);			//uitlezen pine2
		int channelA = !(PINB & (1 << PINB4)) != !channelB;	//XOR geeft signaal van channelA

		// logica voor het uitlezen van de encoder
		if (channelA != lastLeftA) {
			if (channelB == channelA) {
				leftCounter --;
			} else {
				leftCounter ++;
			}
		} else if (channelB == channelA) { // we snappen nog niet helemaal de reden dat dit nodig is voor de linker encoder.
			leftCounter ++;
		} else {
			leftCounter --;
		}

		lastLeftA = channelA;

	}
}

//rechter interrupt
ISR(INT6_vect) {
	int channelB = PINF & (1 << PINF0);				// uitlezen pinf0
	int channelA = !(PINE & (1 << PINE6)) != !channelB;		// xor geeft signaal van channelA

	// logica voor het uitlezen van de encoder
	if (channelA != lastRightA) {
		if (channelB == channelA) {
			rightCounter--;
		} else {
			rightCounter++;
		}
	}
	lastRightA = channelA;
}

void initLeftInterrupt() {
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT4);
}

void initRightInterrupt() {
  EICRB |= (1 << ISC60);				
  EIMSK |= (1 << INT6);
}

void ENCODER_init() {
  //set as inputs
  DDRB &= ~(1 << DDB4);
  DDRE &= ~((1 << DDE2) | (1 << DDE6));
  DDRF &= ~(1 << DDF0);

  //set pull-up
  PORTB |= (1 << PORTB4);
  PORTE |= (1 << PORTE2) | (1 << PORTE6);
  PORTF |= (1 << PORTF0);

  // initialiseer interrupts
  initLeftInterrupt();
  initRightInterrupt();

  sei();

  // lees initiele encoder waardes uit:
  lastLeftA = !(PINB & (1 << PINB4)) != !(PINE & (1 << PINE2));
  lastRightA = !(PINE & (1 << PINE6)) != !(PINF & (1 << PINF0));
}

// De volgende twee functies heten get, maar zijn niet cpp code
// deze naam komt uit de gewoonte om java te schrijven
//
// Het enige dat deze functies doen is de globale variabele van dit bestand doorgeven
// dit komt doordat het doorgeven van globale variabelen naar andere bestanden niet echt wilde lukken.
int getEncoderLeft() {	return leftCounter; }

int getEncoderRight() {	return rightCounter; }

// stelt de encoder tellers in op 0
void resetEncoderValues() { 
	leftCounter = 0; 
	rightCounter = 0; 
}
