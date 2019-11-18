#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "ControlCookingTime.h"

static uint16_t RemainedTime = 0;
static uint16_t CookTime = 0;

void SetRemainedTime(uint16_t time)
{
   RemainedTime = time;
}

uint16_t GetRemainedTime(void)
{
#ifdef DEBUG
   Serial.print("@@@@@ GetRemainedTime: = ");
   Serial.println(RemainedTime);
#endif
   return(RemainedTime);
}

void SetCookRemainedTime(uint16_t time)
{
   CookTime = time;
}

uint16_t GetCookRemainedTime(void)
{
#ifdef DEBUG
   Serial.print("@@@@@ GetCookRemainedTime: = ");
   Serial.println(CookTime);
#endif
   return(CookTime);
}

