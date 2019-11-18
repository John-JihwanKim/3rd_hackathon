#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "DetectStatus.h"

static eDETECT_STATUS DetectStatus = eDETECT_NONE;

void SetDetectStatus(eDETECT_STATUS status)
{
   DetectStatus = status;
}

eDETECT_STATUS GetDetectStatus(void)
{
   return(DetectStatus);
}
