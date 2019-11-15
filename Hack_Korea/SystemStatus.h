#ifndef SYSTEMPSTATUS_H
#define SYSTEMPSTATUS_H

typedef enum 
{
   eSYSTEM_ON = 0,
   eSYSTEM_INITING,
   eSYSTEM_INITED,
   eSYSTEM_STANDBY,
   eSYSTEM_STARTED,
   eSYSTEM_PAUSE,
   eSYSTEM_COOKDONE,
   eSYSTEM_OFF,
   eSYSTEM_ERROR
} eSYSTEM_STATUS;

void SetUISystemStatus(eSYSTEM_STATUS status);
eSYSTEM_STATUS GetUISystemStatus(void);
void SetInductionSystemStatus(eSYSTEM_STATUS status);
eSYSTEM_STATUS GetInductionSystemStatus(void);


#endif
