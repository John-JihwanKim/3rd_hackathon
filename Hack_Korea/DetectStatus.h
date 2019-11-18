#ifndef DETECTSTATUS_H
#define DETECTSTATUS_H

typedef enum 
{
   eDETECT_NONE = 0,
   eDETECT_SHRIMP,
   eDETECT_BEEF,
   eDETECT_VEGETABLE,
} eDETECT_STATUS;

void SetLeftFrontStatus(eDETECT_STATUS status);
eDETECT_STATUS GetLeftFrontStatus(void);

void SetRightFrontStatus(eDETECT_STATUS status);
eDETECT_STATUS GetRightFrontStatus(void);

void SetLeftRearStatus(eDETECT_STATUS status);
eDETECT_STATUS GetLeftRearStatus(void);

void SetRightRearStatus(eDETECT_STATUS status);
eDETECT_STATUS GetRightRearStatus(void);

#endif
