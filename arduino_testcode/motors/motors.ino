void setup() {
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
  if (speed > 400)  // Max 
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
void setSpeeds(int leftSpeed, int rightSpeed)
{
  setLeftSpeed(leftSpeed);
  setRightSpeed(rightSpeed);
}

void loop() {
  delay(500);
  setSpeeds(100, -100);
  delay(1000);
  setSpeeds(100, 0);
  delay(1000);
  setSpeeds(0, 0);
  delay(1000000);
}
