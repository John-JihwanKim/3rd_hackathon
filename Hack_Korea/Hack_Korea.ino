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


// Serial for Camera
// RX, TX
// SoftwareSerial cameraSerial(19, 18);

static void EveryTwoHundredmSecondLoop(void)
{
   SendDataToThunderInductionFrequently();
}

static void EveryThreeHundredmSecondLoop(void)
{
   ReadSensor();
   CalulationTemperatureForPotBottom();
}

// static void InitUARTForCamera(void)
// {
//    cameraSerial.begin(115200);
// }

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
      SetCookRemainedTime(420);
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
   // InitUARTForCamera();
   // Serial.println("Initialize All UARTs");

   I2CSensorInit();
   // SetRemainedTime(100); // for the test
   OneSecondInterval.start(1000, AsyncDelay::MILLIS);
   TwoHundredmSecondInterval.start(200, AsyncDelay::MILLIS);
   ThreeHundredmSecondInterval.start(300, AsyncDelay::MILLIS);
}

void loop()
{
   // static char rxBuffer[4];
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
   // if(cameraSerial.available())
   // {
   //    Serial.print("Camera Detection Data Start");

   //    for (unsigned char i = 0; i < 4; i++)
   //    {
   //       rxBuffer[i] = Serial.read();
   //       Serial.print(rxBuffer[i]);
   //    }

   //    Serial.print("Camera Detection Data End");
   // }
}
