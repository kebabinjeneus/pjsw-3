//PE6 = right encoder xor = int6 interrupt
//PF0 = right encoder b

//PB4 = left encoder xor = PCint4 interrupt
//PE2 = left encoder b
#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <avr/interrupt.h>

// project include
#include "headers/encoder.h"

static volatile int lastLeftA;
static volatile int lastRightA;

static volatile int32_t leftCounter = 0;
static volatile int32_t rightCounter = 0;

//left interrupt
ISR(PCINT0_vect) {
  int channelB = PINE & (1 << PINE2);
  int channelA = (PINB & (1 << PINB4)) ^ channelB;

  if (channelA != lastLeftA) {
    if (channelB == channelA) {
      leftCounter --;
    }
    else {
      leftCounter ++;
    }
  }
  else if (channelB == channelA) {
    leftCounter ++;
  }
  else {
    leftCounter --;
  }

  lastLeftA = channelA;
}

//right interrupt
ISR(INT6_vect) {
  int channelB = PINF & (1 << PINF0);
  int channelA = (PINE & (1 << PINE6)) ^ channelB;

  if (channelA != lastRightA) {
    if (channelB == channelA) {
      rightCounter --;
    }
    else {
      rightCounter ++;
    }
  }
  else if (channelB == channelA) {
    
    rightCounter ++;
  }
  else {
    rightCounter --;
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

void initLeftVars() {
  lastLeftA = (PINB & (1 << PINB4)) ^ (PINE & (1 << PINE2));
}

void initRightVars() {
  lastRightA = (PINE & (1 << PINE6)) ^ (PINF & (1 << PINF0));
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

  initLeftInterrupt();
  initRightInterrupt();

  sei();
  initLeftVars();
  initRightVars();
}
