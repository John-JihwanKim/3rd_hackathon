#include "UARTForInduction.h"
#include "UARTForBLE.h"
#include "AsyncDelay.h"
#include "I2C.h"
#include "ReadTemperatureADC.h"
#include "SystemStatus.h"
#include "ControlCookingTime.h"
#include "ControlRecipes.h"
#include "ControlInduction.h"
#include "ControlACLoads.h"
#include "ControlHeightForUpperHeater.h"
#include "SetZeroWeight.h"
#include "DetectStatus.h"
#include <stdlib.h>

/* Timer base on 1-millisecond */
static AsyncDelay OneSecondInterval;
static AsyncDelay ThreeHundredmSecondInterval;
static AsyncDelay TwoHundredmSecondInterval;
// static 


static void EveryTwoHundredmSecondLoop(void)
{
   SendDataToThunderInductionFrequently();
}

static void EveryThreeHundredmSecondLoop(void)
{
   ReadSensor();
   CalulationTemperatureForPotBottom();
}

static void InitUARTForCamera(void)
{
   Serial1.begin(115200);
}

static void EveryOneSecondLoop(void)
{
   uint16_t remainedTime = GetRemainedTime();
   uint16_t cookRemainedTime = GetCookRemainedTime();
   // SendDataToBLEFrequently();
   PrintInformationForDebugging();

   if(cookRemainedTime > 0)
   {
      SetRemainedTime(remainedTime-1);
      SetCookRemainedTime(cookRemainedTime-1);
   }

   if (GetCookRemainedTime() == 0 && GetStepOfCurrentCooking() == eSYSTEM_CUR_COOK_STARTED_1)
   {
      SetCookRemainedTime(180); // 3m
      // SetCookRemainedTime(20);
      // Second cycle - heater
      SetCurrentInductionLevel(0);
      SetCurrentACLoads(7);
      SetTargetHeightForUpperHeater(5);
      SetStepOfCurrentCooking(eSYSTEM_CUR_COOK_STARTED_2);
   }
   else if(GetCookRemainedTime() == 0 && GetStepOfCurrentCooking() == eSYSTEM_CUR_COOK_STARTED_2)
   {
      Serial.println("@@@@@@@@ Cook Finished @@@@@@@");
      // TODO : handle time out and finish cook
      SetCurrentInductionLevel(0);
      SetCurrentACLoads(0);
      SetTargetHeightForUpperHeater(0);
      SetWhatstheRecipeIndexOfCurrentCooking(3);
      SetStepOfCurrentCooking(eSYSTEM_CUR_COOK_OFF);
      SetUISystemStatus(eSYSTEM_COOKDONE);
   }
   SendDataToBLEFrequently();
}

static void initializeThunder(void)
{
   SetUISystemStatus(eSYSTEM_ON);
}

void setup()
{
   InitUARTForDebugging();
   InitUARTForThunderInduction();
   InitUARTForBLE();
   initializeThunder();
   SetZeroWeight(1);
   SendDataToThunderInductionFrequently();
   SetZeroWeight(0);

   InitUARTForCamera();
   // Serial.println("Initialize All UARTs");

   I2CSensorInit();
   SetRemainedTime(300); // for the test
   OneSecondInterval.start(1000, AsyncDelay::MILLIS);
   TwoHundredmSecondInterval.start(200, AsyncDelay::MILLIS);
   ThreeHundredmSecondInterval.start(300, AsyncDelay::MILLIS);
}

void loop()
{
   static char rxBuffer[4];
   if(TwoHundredmSecondInterval.isExpired())
   {
      EveryTwoHundredmSecondLoop();
      TwoHundredmSecondInterval.repeat();
   }
   
   if(ThreeHundredmSecondInterval.isExpired())
   {
      EveryThreeHundredmSecondLoop();
      ThreeHundredmSecondInterval.repeat();
   }

   if(OneSecondInterval.isExpired())
   {
      EveryOneSecondLoop();
      OneSecondInterval.repeat();
   }
   if(Serial1.available())
   {
      Serial.print("Camera Detection Data Start");

      for (unsigned char i = 0; i < 4; i++)
      {
         rxBuffer[i] = Serial1.read();
         uint32_t myInt = rxBuffer[i];
         Serial.println(myInt);
         //UL UR BL BR
         switch(i)
         {
         case 1:
            SetLeftRearStatus((eDETECT_STATUS)myInt);
            break;
         case 2:
            SetRightRearStatus((eDETECT_STATUS)myInt);
            break;
         case 3:
            SetLeftFrontStatus((eDETECT_STATUS)myInt);
            break;
         case 4:
            SetRightFrontStatus((eDETECT_STATUS)myInt);
            break;
         }
      }

      Serial.print("Camera Detection Data End");
   }
}
