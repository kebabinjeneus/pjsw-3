#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define Gyro_adres_SLAW 0b11010110
#define Gyro_adres_SLAR 0b11010111 // +1 voor leesbit //adres met direction bit (0 voor schrijven & 1 voor lezen)
#define G_CTRL1 0b0100000
#define G_CTRL2 0b0100001
#define G_CTRL3 0b0100010
#define G_CTRL4 0b0100011
#define G_CTRL5 0b0100100

#define Accelero_adres_SLAW 0b00111010
#define Accelero_adres_SLAR 0b00111011
#define A_CTRL1 0x20
#define A_CTRL2 0x21
#define A_CTRL3 0x22
#define A_CTRL4 0x23
#define A_CTRL5 0x24

#define ERROR 0
#define SUCCES 1

int xg, yg, zg, xa, ya, za;
int xgOff, ygOff, zgOff, xaOff, yaOff, zaOff;
int32_t xTotalTurn = 0;
int32_t yTotalTurn = 0;
int32_t zTotalTurn = 0;

uint8_t TWI_statusCodeCheck(uint8_t st);
void calibrate(char sensor);

#include "headers/usart.h"
#include "headers/motor.h"
#include "headers/encoder.h"

void TWI_init() {
	TWSR = (0<<TWPS1) | (0<<TWPS0);
	//TWBR = ((160000000 / 400000) - 16) / (2 * 1);
	TWBR = ((F_CPU / 400000) - 16) / (2 * 1);
	TWCR = (1<<TWEN);
}

void TWI_start(){
	TWCR = (1<<TWEN) | (1<< TWINT) | (1<<TWSTA);
	while(!(TWCR&(1<<TWINT)));
}

void TWI_stop(){
	TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTO);
}

void Error(){
	TWI_stop();
}

void TWI_write (uint8_t data){
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR&(1<<TWINT)));
}
uint8_t TWI_readACK(){
	TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWEA);
	while(!(TWCR&(1<<TWINT)));
	return TWDR;
}

uint8_t TWI_readNACK(){
	TWCR = (1<<TWEN) | (1<<TWINT);
	while(!(TWCR&(1<<TWINT)));
	return TWDR;
}

uint8_t TWI_GetStatus(){
	uint8_t status;
	status = TWSR & 0xF8;
	return status;
}

uint8_t readGyroVars(uint8_t adresSLAW, uint8_t adresSLAR){
	uint8_t xl, xh, yl, yh, zl, zh;
	TWI_init();

	TWI_start();
	TWI_statusCodeCheck(0x08);

	TWI_write(adresSLAW);
	TWI_statusCodeCheck(0x18);

	TWI_write(0b10101000);		//wat is dit adres? --> adres van register waarin x y z staan
	TWI_statusCodeCheck(0x28);

	TWI_start();
	TWI_statusCodeCheck(0x10);

	TWI_write(adresSLAR);
	TWI_statusCodeCheck(0x40);

	xl = TWI_readACK();
	TWI_statusCodeCheck(0x50);

	xh = TWI_readACK();
	TWI_statusCodeCheck(0x50);

	yl = TWI_readACK();
	TWI_statusCodeCheck(0x50);

	yh = TWI_readACK();
	TWI_statusCodeCheck(0x50);

	zl = TWI_readACK();
	TWI_statusCodeCheck(0x50);

	zh = TWI_readNACK();
	TWI_statusCodeCheck(0x58);

	TWI_stop();

	xg = (int16_t)(xh << 8 | xl);
	yg = (int16_t)(yh << 8 | yl);
	zg = (int16_t)(zh << 8 | zl);

	xg -= xgOff;
	yg -= ygOff;
	zg -= zgOff;

	xTotalTurn += xg;
	yTotalTurn += yg;
	zTotalTurn += zg;
	
	return SUCCES;
}

void write(uint16_t adresSLAW, uint8_t adres_register, uint8_t data){
	TWI_init();

	TWI_start();
	TWI_statusCodeCheck(0x08);

	TWI_write(adresSLAW);
	//TWI_statusCodeCheck(0x18);

	TWI_write(adres_register);
	//TWI_statusCodeCheck(0x28);

	TWI_write(data);
	//TWI_statusCodeCheck(0x28);

	TWI_stop();
}

void initGyro(uint8_t G_adresSLAW, uint8_t G_adresSLAR){
	uint8_t CTRL1_adres = G_CTRL1;
	uint8_t waarde_CTRL1 = 0x6F;      //CTRL1 = (1<<DR1)|(1<<BW1)|(1<<BW0)|(1<<PD)|(!<<Zen)|(1<<Yen)|(1<<Xen);
	write(G_adresSLAW, CTRL1_adres, waarde_CTRL1);
	_delay_ms(10);
	uint8_t CTRL2_adres = G_CTRL2;
	uint8_t waarde_CTRL2 = 0x00;
	write(G_adresSLAW, CTRL2_adres, waarde_CTRL2);
	_delay_ms(10);
	uint8_t CTRL3_adres = G_CTRL3;
	uint8_t waarde_CTRL3 = 0x00;
	write(G_adresSLAW, CTRL3_adres, waarde_CTRL3);
	_delay_ms(10);
	uint8_t CTRL4_adres = G_CTRL4;
	uint8_t waarde_CTRL4 = 0x20;
	write(G_adresSLAW, CTRL4_adres, waarde_CTRL4);
	_delay_ms(10);
	uint8_t CTRL5_adres = G_CTRL5;
	uint8_t waarde_CTRL5 = 0x00;
	write(G_adresSLAW, CTRL5_adres, waarde_CTRL5);
	calibrate('G');
}

void initAccelero(uint8_t A_adresSLAW, uint8_t A_adresSLAR) {
	uint8_t waarde_CTRL1 = 0b01010111;
	write(A_adresSLAW, A_CTRL1, waarde_CTRL1);
	_delay_ms(10);
	uint8_t waarde_CTRL2 = 0x00;
	write(A_adresSLAW, A_CTRL2, waarde_CTRL2);
	_delay_ms(10);
	calibrate('A');
}

uint8_t readAcceleroVars(uint8_t adresSLAW, uint8_t adresSLAR) {
	uint8_t xl, xh, yl, yh, zl, zh;
	TWI_init();

	TWI_start();
	TWI_statusCodeCheck(0x08);

	TWI_write(adresSLAW);
	//TWI_statusCodeCheck(0x18);

	TWI_write(0b10101000);
	//TWI_statusCodeCheck(0x28);

	TWI_start();
	TWI_statusCodeCheck(0x10);

	TWI_write(adresSLAR);
	//TWI_statusCodeCheck(0x40);

	xl = TWI_readACK();
	//TWI_statusCodeCheck(0x50);

	xh = TWI_readACK();
	//TWI_statusCodeCheck(0x50);

	yl = TWI_readACK();
	//TWI_statusCodeCheck(0x50);

	yh = TWI_readACK();
	//TWI_statusCodeCheck(0x50);

	zl = TWI_readACK();
	//TWI_statusCodeCheck(0x50);

	zh = TWI_readNACK();
	//TWI_statusCodeCheck(0x58);

	TWI_stop();

	xa = (int16_t)(xh << 8 | xl);
	ya = (int16_t)(yh << 8 | yl);
	za = (int16_t)(zh << 8 | zl);

	xa -= xaOff;
	ya -= yaOff;
	za -= zaOff;

	return SUCCES;
}

uint8_t TWI_statusCodeCheck(uint8_t st) {
	if(TWI_GetStatus() != st){
		writeString("\n\rFout!");
		writeInt(TWSR);
		Error();
		return ERROR;
	} else	return 1;
}

void calibrate(char sensor) {
	int32_t xTot = 0;
	int32_t yTot = 0;
	int32_t zTot = 0;

	if (sensor == 'G') {
		for (uint16_t i = 0; i < 1024; i++) {
			readGyroVars(Gyro_adres_SLAW, Gyro_adres_SLAR);
			_delay_ms(5);
			xTot += xg;
			yTot += yg;
			zTot += zg;
		}
		xgOff = xTot / 1024;
		ygOff = yTot / 1024;
		zgOff = zTot / 1024;
	}
	else if (sensor == 'A') {
		for (uint16_t i = 0; i < 1024; i++) {
			readAcceleroVars(Accelero_adres_SLAW, Accelero_adres_SLAR);
			_delay_ms(5);
			xTot += xa;
			yTot += ya;
			zTot += za;
		}
		xaOff = xTot / 1024;
		yaOff = yTot / 1024;
		zaOff = zTot / 1024;
	}
}

// debug functie
void printAcceleroValues() {
	readAcceleroVars(Accelero_adres_SLAW, Accelero_adres_SLAR);
	_delay_ms(10);
	writeString("\n\rAccelero:");
	writeString("\n\rX:"); writeInt(xa);
	writeString("\tY:"); writeInt(ya);
	writeString("\tZ:"); writeInt(za);
}


// debug functie
void printGyroValues() {
	readGyroVars(Gyro_adres_SLAW, Gyro_adres_SLAR);
	_delay_ms(10);
	writeString("\n\rGyro:");
	writeString("\n\rX:"); writeInt(xg); writeString("\tX totaal:"); writeInt(xTotalTurn);
	writeString("\n\rY:"); writeInt(yg); writeString("\tY totaal:"); writeInt(yTotalTurn);
	writeString("\n\rZ:"); writeInt(zg); writeString("\tZ totaal:"); writeInt(zTotalTurn);
}

void gyro() {
	while(1) {
		readGyroVars(Gyro_adres_SLAW, Gyro_adres_SLAR);
		_delay_ms(2);
  
		int moveSpeed = 80;
		if(zTotalTurn > 2500) {
			setMotorSpeeds(moveSpeed, -moveSpeed);  
		} else if(zTotalTurn < -2500) {
			setMotorSpeeds(-moveSpeed, moveSpeed);   
		} else {
			setMotorSpeeds(0, 0);
			break;
		}
	}
}

int16_t constr(int speed, int maxMin, int maxPlus ){
	if(speed < maxMin){
		speed = maxMin;
	}
	else if(speed > maxPlus){
		speed = maxPlus;
	}
	return speed;
}

void faceUphill(){		
	while(1) {
		readAcceleroVars(Accelero_adres_SLAW, Accelero_adres_SLAR);
		writeString("\n\rgelezen;\t");
		writeInt(xa); writeString("\t"); writeInt(ya);

		const int16_t maxSpeed = 70;
	
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
			// We are on an incline of more than 5 degrees, so
			// try to face uphill using a feedback algorithm.
			turnSpeed = ya / 16;
			
		}
		else{
			turnSpeed = 0;
			break;
		}
	
		int16_t leftSpeed = forwardSpeed - turnSpeed;
		int16_t rightSpeed = forwardSpeed + turnSpeed;
	
		// Constrain the speeds to be between -maxSpeed and maxSpeed.
		leftSpeed = constr(leftSpeed, -maxSpeed, maxSpeed);
		rightSpeed = constr(rightSpeed, -maxSpeed, maxSpeed);
	
		setMotorSpeeds(leftSpeed, rightSpeed);
	}
}
