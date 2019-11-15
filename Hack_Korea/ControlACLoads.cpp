#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "ControlACLoads.h"

static uint8_t CurrentACLoads = 0;

void SetCurrentACLoads(uint8_t level)
{
   CurrentACLoads = level;
}

uint8_t GetCurrentACLoads(void)
{
   return(CurrentACLoads);
}


