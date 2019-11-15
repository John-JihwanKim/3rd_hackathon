#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "ControlRecipes.h"

static uint8_t StepOfCurrentCooking = 0;
static uint8_t MaxStepOfCurrentCooking = 0;
static uint8_t WhatstheRecipeIndexOfCurrentCooking = 0;

uint8_t GetStepOfCurrentCooking(void)
{
   return(StepOfCurrentCooking);
}
uint8_t GetMaxStepOfCurrentCooking(void)
{
   return(MaxStepOfCurrentCooking);
}
uint8_t GetWhatstheRecipeIndexOfCurrentCooking(void)
{
   return(WhatstheRecipeIndexOfCurrentCooking);
}


