#include "UARTForInduction.h"
#include "UARTForBLE.h"
#include "AsyncDelay.h"
#include "I2C.h"
#include "ReadTemperatureADC.h"
#include "SystemStatus.h"
#include "ControlCookingTime.h"
#include "ControlRecipes.h"


/* Timer base on 1-millisecond */
static AsyncDelay OneSecondInterval;
static AsyncDelay ThreeHundredmSecondInterval;
static AsyncDelay TwoHundredmSecondInterval;

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
   SendDataToBLEFrequently();
   PrintInformationForDebugging();

   if(remainedTime > 0)
   {
      SetRemainedTime(remainedTime-1);
   }
   if(GetRemainedTime() == 0 && GetStepOfCurrentCooking() == eSYSTEM_CUR_COOK_STARTED)
   {
      // TODO : handle time out and finish cook
      SetStepOfCurrentCooking(eSYSTEM_CUR_COOK_OFF);
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
