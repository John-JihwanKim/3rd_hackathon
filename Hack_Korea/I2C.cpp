#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include <Wire.h>
#include "I2C.h"

uint16_t FoodSurfaceTemperature = 0;
uint16_t HeightFromSensorToTarget = 0;

void I2CSensorInit(void)
{
   Wire.begin(); 
}

uint16_t GetFoodSurfaceTemperature(void)
{
   return(FoodSurfaceTemperature);
}

uint16_t GetHeightFromSensorToTarget(void)
{
   return(HeightFromSensorToTarget);
}

void ReadSensor(void)
{
   static uint8_t device_address = GP2Y0E03_ADDRESS;
   static uint8_t data_address = DISTANCE_ADDRESS;
   static uint8_t numberofdata = 2;
   byte data[3];
   uint16_t newtemperature = 0;
   
   Wire.beginTransmission(device_address); // start transmission to device 
   Wire.write(data_address); // sends register address to read from
   if(GP2Y0E03_ADDRESS == device_address)
   {
      Wire.endTransmission(); // end transmission
      Wire.requestFrom(device_address, numberofdata);// send data n-bytes read
      data[0] = Wire.read();          
      data[1] = Wire.read();
      HeightFromSensorToTarget = ((data[0] * 16 + data[1]) / 16) / 4 ;
      device_address = MLX90614_ADDRESS;
      data_address = TEMPERATURE_ADDRESS;
      numberofdata = 3;
   }
   else
   {
      Wire.endTransmission(0); // end transmission
      Wire.requestFrom(device_address, numberofdata);// send data n-bytes read
      data[0] = Wire.read();          
      data[1] = Wire.read();
      data[2] = Wire.read();

      newtemperature = data[0];       // receive DATA
      newtemperature |= data[1] << 8; // receive DATA
      newtemperature *= 2;// /100
      newtemperature -= 27315; // 100
      newtemperature /= 100;
      
      FoodSurfaceTemperature = (FoodSurfaceTemperature * 8) + (newtemperature * 2);
      FoodSurfaceTemperature /= 10;

      device_address = GP2Y0E03_ADDRESS;
      data_address = DISTANCE_ADDRESS;
      numberofdata = 2;
   }
}

