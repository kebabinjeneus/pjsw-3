#ifndef SENSOREN_H
#define SENSOREN_H

// gyro
void initGyro(uint8_t G_adresSLAW, uint8_t G_adresSLAR);
uint8_t readGyroVars(uint8_t adresSLAW, uint8_t adresSLAR);
int32_t getZTotalTurn();
void printGyroValues();
void TWI_init();
void gyro();

#endif
