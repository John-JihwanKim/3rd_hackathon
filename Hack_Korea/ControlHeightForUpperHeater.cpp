#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "ControlHeightForUpperHeater.h"

static uint8_t TargetHeightForUpperHeater = 0;
static uint8_t CurrentHeightForUpperHeater = 0;

void SetCurrentHeightForUpperHeater(uint8_t level)
{
   CurrentHeightForUpperHeater = level;
}

uint8_t GetCurrentHeightForUpperHeater(void)
{
   return(CurrentHeightForUpperHeater);
}

void SetTargetHeightForUpperHeater(uint8_t level)
{
   TargetHeightForUpperHeater = level;
}

uint8_t GetTargetHeightForUpperHeater(void)
{
   return(TargetHeightForUpperHeater);
}

