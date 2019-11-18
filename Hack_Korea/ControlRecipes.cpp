#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "ControlRecipes.h"

static eSYSTEM_CURRENT_COOK_STATUS StepOfCurrentCooking = eSYSTEM_CUR_COOK_OFF;
static uint8_t MaxStepOfCurrentCooking = 0;
static uint8_t WhatstheRecipeIndexOfCurrentCooking = 0;

void SetStepOfCurrentCooking(eSYSTEM_CURRENT_COOK_STATUS status)
{
   StepOfCurrentCooking = status;
}
eSYSTEM_CURRENT_COOK_STATUS GetStepOfCurrentCooking(void)
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
void SetWhatstheRecipeIndexOfCurrentCooking(uint8_t recipe)
{
   WhatstheRecipeIndexOfCurrentCooking = recipe;
}


