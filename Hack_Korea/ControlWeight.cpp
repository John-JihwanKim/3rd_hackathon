#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "ControlWeight.h"

static int16_t CurrentWeight = 0;

void SetCurrentWeight(int16_t weight)
{
   CurrentWeight = weight;
}

int16_t GetCurrentWeight(void)
{
   return(CurrentWeight);
}

