#define F_CPU 16000000

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
#define ERROR 0
#define SUCCES 1
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

int x, y, z;
int xOff, yOff, zOff;
int32_t xTotalTurn = 0;
int32_t yTotalTurn = 0;
int32_t zTotalTurn = 0;

uint8_t TWI_statusCodeCheck(uint8_t st);
void calibrate();

void USART_init(){
	// set baud rate
	UBRR1H = (unsigned char)(BAUD_PRESCALLER>>8);
	UBRR1L = (unsigned char)(BAUD_PRESCALLER);

	// enable receiver / transmitter
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1) | (1<<TXCIE1);

	// set frame format: 8data 2stop bit
	//UCSR1C = (1<<USBS1) | (3<<UCSZ10);	// geen idee of dit beter is
	UCSR1C = (3<<UCSZ10);			// maar dit werkt

}

void writeChar(char data){
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}

void writeString(char* Pstring){
	while(*Pstring != 0x00){
		writeChar(*Pstring);
		Pstring++;
	}
}

void writeInt(int i) {                            //zet int om naar String
	char buffer[8];
	itoa(i,buffer,10);
	writeString(buffer);
}

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

	x = (int16_t)(xh << 8 | xl);
	y = (int16_t)(yh << 8 | yl);
	z = (int16_t)(zh << 8 | zl);

	x -= xOff;
	y -= yOff;
	z -= zOff;

	return SUCCES;
}

void write(uint16_t adresSLAW, uint8_t adres_register, uint8_t data){
	TWI_init();

	TWI_start();
	TWI_statusCodeCheck(0x08);

	TWI_write(adresSLAW);
	TWI_statusCodeCheck(0x18);

	TWI_write(adres_register);
	TWI_statusCodeCheck(0x28);

	TWI_write(data);
	TWI_statusCodeCheck(0x28);

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
	calibrate();
}

uint8_t TWI_statusCodeCheck(uint8_t st) {
	if(TWI_GetStatus() != st){
		writeString("Fout!");
		Error();
		return ERROR;
	} else	return 1;
}

void calibrate() {
	int32_t xTot = 0;
	int32_t yTot = 0;
	int32_t zTot = 0;

	for (uint16_t i = 0; i < 1024; i++)
	{
		// Wait for new data to be available, then read it.
		readGyroVars(Gyro_adres_SLAW, Gyro_adres_SLAR);
		_delay_ms(5);

		// Add the Z axis reading to the total.
		xTot += x;
		yTot += y;
		zTot += z;
	}
	xOff = xTot / 1024;
	yOff = yTot / 1024;
	zOff = zTot / 1024;
}

int main(void) {
	USART_init();
	TWI_init();
	initGyro(Gyro_adres_SLAW, Gyro_adres_SLAR);
	DDRC = (1<<PORTC7);
	PORTC = (1<<PORTC7);

	while(1){
		PORTC = (1<<PORTC7);
		// Gyro meter:
		readGyroVars(Gyro_adres_SLAW, Gyro_adres_SLAR);
		xTotalTurn += x;
		yTotalTurn += y;
		zTotalTurn += z;
		_delay_ms(10);
		writeString("Gyro:");
		writeString("\n\rX:"); writeInt(x); writeString("\tX totaal:"); writeInt(xTotalTurn);
		writeString("\n\rY:"); writeInt(y); writeString("\tY totaal:"); writeInt(yTotalTurn);
		writeString("\n\rZ:"); writeInt(z); writeString("\tZ totaal:"); writeInt(zTotalTurn);
		writeString("\n");
		_delay_ms(100);
	}

	return 1;
}

