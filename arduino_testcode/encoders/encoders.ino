//PE6 = right encoder xor = int6 interrupt
//PF0 = right encoder b

//PB4 = left encoder xor = PCint4 interrupt
//PE2 = left encoder b
#include <stdbool.h>
#include <FastGPIO.h>
static volatile bool lastLeftA;
static volatile bool lastRightA;

static volatile int32_t leftCounter = 0;
static volatile int32_t rightCounter = 0;

//left interrupt
ISR(PCINT0_vect) {
  bool channelB = PINE & (1 << PINE2);
  bool channelA = (PINB & (1 << PINB4)) ^ channelB;

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
//  bool channelB = FastGPIO::Pin<23>::isInputHigh();
//  bool channelA = FastGPIO::Pin<7>::isInputHigh() ^ channelB;
  bool channelB = PINF & (1 << PINF0);
  bool channelA = (PINE & (1 << PINE6)) ^ channelB;

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

void setup() {
  Serial.begin(9600);
  
  //set as inputs
  DDRB &= ~(1 << DDB4);
  DDRE &= ~((1 << DDE2) | (1 << DDE6));
  DDRF &= ~(1 << DDF0);

  //set pull-up
  PORTB |= (1 << PORTB4);
  PORTE |= (1 << PORTE2) | (1 << PORTE6);
  PORTF |= (1 << PORTF0);

  initLeftISR();
  initRightISR();

  sei();
  initLeftVars();
  initRightVars();
  
}

void loop() {
  Serial.print(leftCounter); Serial.print("\t\t"); Serial.println(rightCounter);
}

void initLeftISR() {
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT4);
}

void initRightISR() {
  EICRB |= (1 << ISC60);
  EIMSK |= (1 << INT6);
}

void initLeftVars() {
  lastLeftA = (PINB & (1 << PINB4)) ^ (PINE & (1 << PINE2));
}

void initRightVars() {
  lastRightA = (PINE & (1 << PINE6)) ^ (PINF & (1 << PINF0));
}
