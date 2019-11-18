#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "ControlCookingTime.h"

static uint16_t RemainedTime = 0;

void SetRemainedTime(uint16_t time)
{
   RemainedTime = time;
}

uint16_t GetRemainedTime(void)
{
   //Serial.print("@@@@@ GetRemainedTime: = ");
   // Serial.println(RemainedTime);
   return(RemainedTime);
}

