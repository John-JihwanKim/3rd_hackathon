#include <SoftwareSerial.h>

#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "DetectStatus.h"

static eDETECT_STATUS LeftFrontStatus = eDETECT_NONE;
static eDETECT_STATUS RightFrontStatus = eDETECT_NONE;
static eDETECT_STATUS LeftRearStatus = eDETECT_NONE;
static eDETECT_STATUS RightRearStatus = eDETECT_NONE;

void SetLeftFrontStatus(eDETECT_STATUS status)
{
   LeftFrontStatus = status;
}

eDETECT_STATUS GetLeftFrontStatus(void)
{
   return(LeftFrontStatus);
}

void SetRightFrontStatus(eDETECT_STATUS status)
{
   RightFrontStatus = status;
   
}
eDETECT_STATUS GetRightFrontStatus(void)
{
   return(RightFrontStatus);
}
void SetLeftRearStatus(eDETECT_STATUS status)
{
   LeftRearStatus = status;
}
eDETECT_STATUS GetLeftRearStatus(void)
{
   return(LeftRearStatus);
}
void SetRightRearStatus(eDETECT_STATUS status)
{
   RightRearStatus = status;
}
eDETECT_STATUS GetRightRearStatus(void)
{
   return(RightRearStatus);
}
