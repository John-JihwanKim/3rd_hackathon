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

static void EveryOneSecondLoop(void)
{
   uint16_t remainedTime = GetRemainedTime();
   // SendDataToBLEFrequently();
   PrintInformationForDebugging();

   if(remainedTime > 0)
   {
      SetRemainedTime(remainedTime-1);
   }
   if (GetRemainedTime() == 0 && GetStepOfCurrentCooking() == eSYSTEM_CUR_COOK_STARTED_1)
   {
      SetRemainedTime(420);
      // Second cycle - heater
      SetCurrentInductionLevel(0);
      SetCurrentACLoads(7);
      SetTargetHeightForUpperHeater(5);
      SetStepOfCurrentCooking(eSYSTEM_CUR_COOK_STARTED_2);
   }
   else if(GetRemainedTime() == 0 && GetStepOfCurrentCooking() == eSYSTEM_CUR_COOK_STARTED_2)
   {
      Serial.println("@@@@@@@@ Cook Finished @@@@@@@");
      // TODO : handle time out and finish cook
      SetCurrentInductionLevel(0);
      SetCurrentACLoads(0);
      SetTargetHeightForUpperHeater(0);
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

   I2CSensorInit();
   // SetRemainedTime(100); // for the test
   OneSecondInterval.start(1000, AsyncDelay::MILLIS);
   TwoHundredmSecondInterval.start(200, AsyncDelay::MILLIS);
   ThreeHundredmSecondInterval.start(300, AsyncDelay::MILLIS);
}

void loop()
{
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
}
