#define Gyro_adres_SLAW 0b11010110
#define Gyro_adres_SLAR 0b11010111 // +1 voor leesbit //adres met direction bit (0 voor schrijven & 1 voor lezen)
#define Accelero_adres_SLAW 0b00111010
#define Accelero_adres_SLAR 0b00111011
#define G_CTRL1 0b0100000
#define G_CTRL2 0b0100001
#define G_CTRL3 0b0100010
#define G_CTRL4 0b0100011
#define G_CTRL5 0b0100100
#define A_CTRL1 0x20
#define A_CTRL2 0x21
#define A_CTRL3 0x22
#define A_CTRL4 0x23
#define A_CTRL5 0x24
#define ERROR 0
#define SUCCES 1
#define NACK 1
#define ACK 0
#define LED_Rood 13

void TWI_init();
void TWI_start();
void TWI_write (uint8_t data);
uint8_t TWI_readACK();
uint8_t TWI_readNACK();
uint8_t TWI_GetStatus();
uint8_t read(uint8_t adresSLAW, uint8_t adresSLAR, uint8_t *data, uint8_t BoolACK);
void write(uint16_t adresSLAW, uint8_t adres_register, uint8_t data);
void Error();
void initGyro(uint8_t G_adresSLAW, uint8_t G_adresSLAR);



int xg, yg, zg;
int xa, ya, za;
int xgOff, ygOff, zgOff;
int xaOff, yaOff, zaOff;
int32_t xTotalTurn;
int32_t yTotalTurn;
int32_t zTotalTurn;

int moveSpeed = 80;


void setup() {
  Serial.begin(9600);
  TWI_init();
  Serial.println("start calibrating");
  initGyro(Gyro_adres_SLAW, Gyro_adres_SLAR);
  initAccelero(Accelero_adres_SLAW, Accelero_adres_SLAR);
  Serial.println("end calibrating");

  xTotalTurn = 0;
  yTotalTurn = 0;
  zTotalTurn = 0;

  DDRC = (1 << PORTC7);
  PORTC = (1 << PORTC7);
  Serial.println("Metingen:");


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
  readGyroVars(Gyro_adres_SLAW, Gyro_adres_SLAR);
  delay(2);
  readAcceleroVars(Accelero_adres_SLAW, Accelero_adres_SLAR);
  delay(2);

  if(zTotalTurn > 1200) {
    setSpeeds(moveSpeed, -moveSpeed);  
  }
  else if(zTotalTurn < -1200) {
    setSpeeds(-moveSpeed, moveSpeed);   
  }
  else {
    setSpeeds(0, 0);
  }
  
  
  Serial.println("Gyro");
  Serial.print("X: \t"); Serial.print(xg); Serial.print("\t total: "); Serial.println(xTotalTurn);
  Serial.print("Y: \t"); Serial.print(yg); Serial.print("\t total: "); Serial.println(yTotalTurn);
  Serial.print("Z: \t"); Serial.print(zg); Serial.print("\t total: "); Serial.println(zTotalTurn);


  
  Serial.println("Accelero");
  Serial.print("X: \t"); Serial.println(xa);
  Serial.print("Y: \t"); Serial.println(ya);
  Serial.print("Z: \t"); Serial.println(za);
//  delay(100);
}

uint8_t readGyroVars(uint8_t adresSLAW, uint8_t adresSLAR) {
  uint8_t xl, xh, yl, yh, zl, zh;
  TWI_init();

  TWI_start();
  TWI_statusCodeCheck(0x08);

  TWI_write(adresSLAW);
  TWI_statusCodeCheck(0x18);

  TWI_write(0b10101000);
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


uint8_t readAcceleroVars(uint8_t adresSLAW, uint8_t adresSLAR) {
  uint8_t xl, xh, yl, yh, zl, zh;
  TWI_init();

  TWI_start();
  TWI_statusCodeCheck(0x08);

  TWI_write(adresSLAW);
  TWI_statusCodeCheck(0x18);

  TWI_write(0b10101000);
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

  xa = (int16_t)(xh << 8 | xl);
  ya = (int16_t)(yh << 8 | yl);
  za = (int16_t)(zh << 8 | zl);


  xa -= xaOff;
  ya -= yaOff;
  za -= zaOff;

  return SUCCES;
}


void write(uint16_t adresSLAW, uint8_t adres_register, uint8_t data) {
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

  return SUCCES;
}

// alle hulpfuncties
void TWI_init() {
  TWSR = (0 << TWPS1) | (0 << TWPS0);
  TWBR = ((160000000 / 400000) - 16) / (2 * 1);
  TWCR = (1 << TWEN);
}

void TWI_start() {
  TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA);
  while (!(TWCR & (1 << TWINT)));
}

void TWI_stop() {
  TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
}

void TWI_write (uint8_t data) {
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
}
uint8_t TWI_readACK() {
  TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
  while (!(TWCR & (1 << TWINT)));
  return TWDR;
}

uint8_t TWI_readNACK() {
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  return TWDR;
}

uint8_t TWI_GetStatus() {
  uint8_t status;
  status = TWSR & 0xF8;
  return status;
}

void Error() {
  TWI_stop();
}

void initGyro(uint8_t G_adresSLAW, uint8_t G_adresSLAR) {
  uint8_t waarde_CTRL1 = 0x6F;            //CTRL1 = (1<<DR1)|(1<<BW1)|(1<<BW0)|(1<<PD)|(!<<Zen)|(1<<Yen)|(1<<Xen);
  write(G_adresSLAW, G_CTRL1, waarde_CTRL1);
  delay(10);
  uint8_t waarde_CTRL4 = 0x20;
  write(G_adresSLAW, G_CTRL4, waarde_CTRL4);
  delay(10);
  calibr('G');
}

void initAccelero(uint8_t A_adresSLAW, uint8_t A_adresSLAR) {
  uint8_t waarde_CTRL1 = 0b01010111;
  write(A_adresSLAW, A_CTRL1, waarde_CTRL1);
  delay(10);
  uint8_t waarde_CTRL2 = 0x00;
  write(A_adresSLAW, A_CTRL2, waarde_CTRL2);
  delay(10);
  calibr('A');
}

uint8_t TWI_statusCodeCheck(uint8_t st) {
  if (TWI_GetStatus() != st) {
    Serial.print(st + " Fout! \t"); Serial.print("TWSR: \t"); Serial.print(TWSR); Serial.println();
    Error();
    return ERROR;
  }
}

void calibr(char sensor) {
  int32_t xTot = 0;
  int32_t yTot = 0;
  int32_t zTot = 0;
  if (sensor == 'G') {
    for (uint16_t i = 0; i < 1024; i++) {
      readGyroVars(Gyro_adres_SLAW, Gyro_adres_SLAR);
      delay(2);
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
      delay(2);
      xTot += xa;
      yTot += ya;
      zTot += za;
    }
    xaOff = xTot / 1024;
    yaOff = yTot / 1024;
    zaOff = zTot / 1024;
  }
  return;
}
