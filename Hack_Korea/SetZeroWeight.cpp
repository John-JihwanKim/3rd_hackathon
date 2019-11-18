#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "SetZeroWeight.h"

static uint8_t ZeroStatus = 0;

void SetZeroWeight(uint8_t status)
{
   Serial.print("From Induction = ");
   ZeroStatus = status;
}

uint8_t GetZeroWeight(void)
{
   return(ZeroStatus);
}
