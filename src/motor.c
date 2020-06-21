#define F_CPU 16000000UL

// libraries
#include <avr/io.h>
#include <util/delay.h>

// project includes
#include "headers/motor.h"
#include "headers/encoder.h"
#include "headers/usart.h"

// paar definities enzo
#define pgv 1                // Proportional gain value
#define straightfactor 0.9975         // pas deze waarde aan om de robot recht te doen rijden.

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

// stel snelheid voor linker motor in
// speed is een nummer tussen -400 en 400
void setLeftSpeed(int speed) {
  int reverse = 0;
  if (speed < 0) {
    speed = -speed;	// maak snelheid positief
    reverse = 1;	// behoud richting
  }
  if (speed > 400)	// Max PWM dutycycle
    speed = 400;

  OCR1B = speed;	// stel snelheid in

  if (reverse)		// stel richting in
    PORTB |= (1 << PORTB2);
  else
    PORTB &= ~(1 << PORTB2);
}

// stel snelheid voor rechter motor in
// speed is een nummer tussen -400 en 400
void setRightSpeed(int speed) {
  int reverse = 0;
  if (speed < 0) {
    speed = -speed;	// Maak snelheid positief
    reverse = 1;	// behoud richting
  }
  
  if (speed > 400)	// Max PWM dutycycle
    speed = 400;

  OCR1A = speed;	// stel snelheid in

  if (reverse)		// stel richting in
    PORTB |= (1 << PORTB1);
  else
    PORTB &= ~(1 << PORTB1);
}

// stel de snelheid in voor beide motoren
void setMotorSpeeds(int leftSpeed, int rightSpeed)
{
  setLeftSpeed(leftSpeed);
  setRightSpeed(rightSpeed);
}

// laat de zumo 10 cm vooruit rijden
void rijd10cm() {
	_delay_ms(200); // tijd om het knopje los te laten voor de robot wegrijd
	int start = getEncoderRight(); // startwaarde encoder
	setMotorSpeeds(200, 200); // vooruit rijden
	// wacht zolang de encoder niet de waarde voor 10cm bereikt heeft
	while(getEncoderRight() < (start+492));
	setMotorSpeeds(0, 0); // zet motoren uit.
}

// wordt aangeroepen met waarde 1 als knop A door de interrupt wordt gedetecteerd
// wordt ook standaard aangeroepen door de main loop met waarde 0
// dit laatste gebeurt om te checken of knop A ingedrukt is.
void test10cm(int set) {
	static int flag = 0;
	if(set) {
		flag = 1;
	} else if(flag) {
		rijd10cm();
		flag = 0;
	}
}

// functie om in een rechte lijn te rijden.
void setMotorCorrectie(int snelheidL, int snelheidR) {
	int correctie, afwijking; // correctie factor = pgv * afwijking 

	// encoderwaardes inlezen
	int aantalL = getEncoderLeft();
	int aantalR = getEncoderRight();

	// logica om motorsnelheden aan te passen op afwijking in encoders.
	afwijking = aantalL - straightfactor * aantalR;
	correctie = pgv * afwijking;
	snelheidR += correctie;
	setMotorSpeeds(snelheidL, snelheidR);
}
