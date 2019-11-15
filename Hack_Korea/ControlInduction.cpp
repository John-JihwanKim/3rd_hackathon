#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "ControlInduction.h"

static uint8_t CurrentInductionLevel = 0;

void SetCurrentInductionLevel(uint8_t level)
{
   CurrentInductionLevel = level;
}

uint8_t GetCurrentInductionLevel(void)
{
   return(CurrentInductionLevel);
}

