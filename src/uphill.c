#define Accelero_adres_SLAW 0b00111010
#define Accelero_adres_SLAR 0b00111011

#include <avr/io.h>

#include "headers/motor.h"
#include "headers/sensoren.h"
#include "headers/encoder.h"
//#include "headers/usart.h"

int32_t xTotalTurn;
int32_t yTotalTurn;
int32_t zTotalTurn;

int xa, ya, za;

int isUphill = 0;

const int16_t maxSpeed = 70;

int16_t constr(int speed, int maxMin, int maxPlus){
  if(speed < maxMin){
    speed = maxMin;
  } else if(speed > maxPlus){
    speed = maxPlus;
  }
  return speed;
}

void uphill() {
 TWI_init();
  //WriteString("start calibrating \n\r");
  initAccelero(Accelero_adres_SLAW, Accelero_adres_SLAR);
  //WriteString("end calibrating \n\r");

  DDRC = (1 << PORTC7);
  PORTC = (1 << PORTC7);
  //WriteString("Metingen: \n\r");

 readAcceleroVars(Accelero_adres_SLAW, Accelero_adres_SLAR);
  //WriteString("Accelero \n\r");
  //WriteString("X: \t"); WriteString(xa);WriteString("\n\r");
  //WriteString("Y: \t"); WriteString(ya);WriteString("\n\r");
  //WriteString("Z: \t"); WriteString(za);WriteString("\n\r");
  


 if (isUphill && xa > 1000 && ya < 500 && ya > -500) {
    setMotorSpeeds(0, 0);
    isUphill = 0;
  }


  if(~PIND & (1 << PORTD5)) {
    isUphill = 1;
    int32_t magnitudeSquared = (int32_t)xa * xa + (int32_t)ya * ya;
               
    // Use the encoders to see how much we should drive forward.
    // If the robot rolls downhill, the encoder counts will become
    // negative, resulting in a positive forwardSpeed to counteract
    // the rolling.
    int16_t forwardSpeed = -(getEncoderLeft() + getEncoderRight());
    forwardSpeed = constr(forwardSpeed, -maxSpeed, maxSpeed);

    // See if we are actually on an incline.
    //  32768 * sin(5 deg) = 2856
    int16_t turnSpeed;
  
    if(magnitudeSquared > (int32_t)2856*2856){
        //Geel signaal LED:
        PORTC = (1<<PORTC7);
  
      // We are on an incline of more than 5 degrees, so
      // try to face uphill using a feedback algorithm.
      turnSpeed = ya / 16;
    }
    
    else{
      PORTC &= ~(1<<PORTC7);
      turnSpeed = 0;
    }

    int16_t leftSpeed = forwardSpeed - turnSpeed;
    int16_t rightSpeed = forwardSpeed + turnSpeed;
  
    // Constrain the speeds to be between -maxSpeed and maxSpeed.
    leftSpeed = constr(leftSpeed, -maxSpeed, maxSpeed);
    rightSpeed = constr(rightSpeed, -maxSpeed, maxSpeed);
  
    setMotorSpeeds(leftSpeed, rightSpeed);
  
  
    //delay(100);
  }
  //delay(200); 
}
