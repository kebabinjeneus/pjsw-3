#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "headers/motor.h"
#include "headers/sensoren.h"

#define Gyro_adres_SLAW 0b11010110
#define Gyro_adres_SLAR 0b11010111 // +1 voor leesbit //adres met direction bit (0 voor schrijven & 1 voor lezen)

//int32_t xTotalTurn;
//int32_t yTotalTurn;
//int32_t zTotalTurn;

//int xg, yg, zg;

void gyro() {
 //TWI_init();
  //WriteString("start calibrating \n\r");
  //initGyro(Gyro_adres_SLAW, Gyro_adres_SLAR);
  //WriteString("end calibrating \n\r");

  //xTotalTurn = 0;
  //yTotalTurn = 0;
  //zTotalTurn = 0;

  //DDRC = (1 << PORTC7);
  //PORTC = (1 << PORTC7);
  //Serial.println("Metingen:");


  readGyroVars(Gyro_adres_SLAW, Gyro_adres_SLAR);
  _delay_ms(2);

  int moveSpeed = 80;
  int32_t zTotalTurn = getZTotalTurn();

  //if(~PINB & (1 << PORTB0)) {
    if(zTotalTurn > 1200) {
      setMotorSpeeds(moveSpeed, -moveSpeed);  
    }
    else if(zTotalTurn < -1200) {
      setMotorSpeeds(-moveSpeed, moveSpeed);   
    }
  //}
  if (zTotalTurn < 1200 && zTotalTurn > -1200) {
    setMotorSpeeds(0, 0);
  }
  
  //WriteString("Gyro \n\r");
  //WriteString("X: \t"); WriteString(xg); WriteString("\t total: "); WriteString(xTotalTurn); WriteString("\n\r");
  //WriteString("Y: \t"); WriteString(yg); WriteString("\t total: "); WriteString(yTotalTurn); WriteString("\n\r");
  //WriteString("Z: \t"); WriteString(zg); WriteString("\t total: "); WriteString(zTotalTurn); WriteString("\n\r");
  //delay(100);
}
