#ifndef SENSOREN_H
#define SENSOREN_H

// nodig in main
//void SENSOREN_init();
//void sensor_update();

// uphil & gyro
//void TWI_init();

// uphill vereisten
//void initAccelero(uint8_t A_adresSLAW, uint8_t A_adresSLAR);
//uint8_t readAcceleroVars(uint8_t adresSLAW, uint8_t adresSLAR);

// gyro vereisten
void initGyro(uint8_t G_adresSLAW, uint8_t G_adresSLAR);
uint8_t readGyroVars(uint8_t adresSLAW, uint8_t adresSLAR);
int32_t getZTotalTurn();
void printGyroValues();
void readGyroValues();

#endif
