#define F_CPU 16000000UL

// libraries
#include <avr/io.h>

// project includes
#include "headers/motor.h"
#include "headers/encoder.h"

void MOTOR_init() {
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

// set speed for left motor; speed is a number between -400 and 400
void setLeftSpeed(int speed) {
  int reverse = 0;
  if (speed < 0) {
    speed = -speed; // make speed a positive quantity
    reverse = 1;    // preserve the direction
  }
  if (speed > 400)  // Max PWM dutycycle
    speed = 400;

  OCR1B = speed;

  if (reverse)
    PORTB |= (1 << PORTB2);
  else
    PORTB &= ~(1 << PORTB2);
}

// set speed for right motor; speed is a number between -400 and 400
void setRightSpeed(int speed) {
  int reverse = 0;
  if (speed < 0) {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;
  }
  
  if (speed > 400)  // Max PWM dutycycle
    speed = 400;

  OCR1A = speed;

  if (reverse)
    PORTB |= (1 << PORTB1);
  else
    PORTB &= ~(1 << PORTB1);
}

// set speed for both motors
void setMotorSpeeds(int leftSpeed, int rightSpeed)
{
  setLeftSpeed(leftSpeed);
  setRightSpeed(rightSpeed);
}

// laat de zumo 10 cm vooruit rijden
void rijd10cm() {
	int start = getEncoderRight();
	setMotorSpeeds(200, 200);
	while(getEncoderRight() < (start+504)); // 1:50 = 504 || 1:75 = 742 || 1:100 = 983
	setMotorSpeeds(0, 0);
}

// wordt aangeroepen met waarde 1 als knop A door de interrupt wordt gedetecteerd
// wordt ook standaard aangeroepen door de main loop met waarde 0
// dit laatste gebeurt om te checken of knop A ingedrukt is.
void test10cm(int set) {
	static int flag = 0;
	if(set) {
		flag = 1;
	}else if(flag) {
		rijd10cm();
		flag = 0;
	}
}

