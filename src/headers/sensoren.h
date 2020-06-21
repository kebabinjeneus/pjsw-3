#ifndef SENSOREN_H
#define SENSOREN_H

// uphil & gyro
void TWI_init();

// uphill vereisten
void initAccelero(uint8_t A_adresSLAW, uint8_t A_adresSLAR);
uint8_t readAcceleroVars(uint8_t adresSLAW, uint8_t adresSLAR);
void printAcceleroValues(); //debug
void faceUphill();

// gyro vereisten
void initGyro(uint8_t G_adresSLAW, uint8_t G_adresSLAR);
uint8_t readGyroVars(uint8_t adresSLAW, uint8_t adresSLAR);
void printGyroValues(); //debug
void gyro();

#endif
