#ifndef I2C_H
#define I2C_H

#define MLX90614_ADDRESS 0x5A
#define TEMPERATURE_ADDRESS 0x07

#define GP2Y0E03_ADDRESS 0x40
#define DISTANCE_ADDRESS 0x5E

uint16_t GetFoodSurfaceTemperature(void);
uint16_t GetHeightFromSensorToTarget(void);
void I2CSensorInit(void);
void ReadSensor(void);

#endif
