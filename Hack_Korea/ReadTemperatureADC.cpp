#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "ReadTemperatureADC.h"

static const uint16_t Slope[7] =    {286, 132, 122, 177, 305, 606, 1479};
static const uint8_t Intercept[7] = {254,  118,  111, 130, 156,  190,  237};
static uint16_t TemperatureForPotBottom = 0;

uint16_t GetTemperatureForPotBottom(void)
{
   return(TemperatureForPotBottom);
}

void CalulationTemperatureForPotBottom(void)
{
   uint8_t tablenumber = 0;
   uint32_t newtemp = 0;

   uint16_t readadcvalue = analogRead(PORT_FOR_POTBOTTOMTEMPERATURE);

   if((986 < readadcvalue) || (21 > readadcvalue))
   {
      Serial.println("You may not connect the NTC");
   }
   else
   {
      if((986 >= readadcvalue) && (882 < readadcvalue))
      {
         tablenumber = 0;
      }
      else if((882 >= readadcvalue) && (599 < readadcvalue))
      {
         tablenumber = 1;
      }
      else if((599 >= readadcvalue) && (365 < readadcvalue))         
      {
         tablenumber = 2;
      }
      else if((365 >= readadcvalue) && (202 < readadcvalue))
      {
         tablenumber = 3;
      }
      else if((202 >= readadcvalue) && (117 < readadcvalue))
      {
         tablenumber = 4;
      }
      else if((117 >= readadcvalue) && (56 < readadcvalue))
      {
         tablenumber = 5;
      }
      else if((56 >= readadcvalue) && (21 < readadcvalue))
      {
         tablenumber = 6;
      }
   }
   newtemp = (uint32_t)((uint32_t)Intercept[tablenumber] * 1000) - (uint32_t)((uint32_t)readadcvalue * (uint32_t)Slope[tablenumber]);
   newtemp /= 1000;
   newtemp = ((uint32_t)TemperatureForPotBottom * 8) + (newtemp * 2);
   TemperatureForPotBottom = (uint16_t)(newtemp / 10);
}


