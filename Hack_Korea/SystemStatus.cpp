#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "SystemStatus.h"

static eSYSTEM_STATUS UISystemStatus = eSYSTEM_ON;
static eSYSTEM_STATUS InductionSystemStatus = eSYSTEM_ON;

void SetUISystemStatus(eSYSTEM_STATUS status)
{
   UISystemStatus = status;
}

eSYSTEM_STATUS GetUISystemStatus(void)
{
   return(UISystemStatus);
}

void SetInductionSystemStatus(eSYSTEM_STATUS status)
{
   InductionSystemStatus = status;
}

eSYSTEM_STATUS GetInductionSystemStatus(void)
{
   return(InductionSystemStatus);
}

