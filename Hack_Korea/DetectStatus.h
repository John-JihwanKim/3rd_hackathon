#ifndef DETECTSTATUS_H
#define DETECTSTATUS_H

typedef enum 
{
   eDETECT_NONE = 0,
   eDETECT_BEEF,
   eDETECT_ASPARAGUS,
   eDETECT_BEEF_ASPARAGUS_KIT,
   eDETECT_BEEF_SHRIMP_ASPARAGUS_KIT,
} eDETECT_STATUS;

void SetDetectStatus(eDETECT_STATUS status);
eDETECT_STATUS GetDetectStatus(void);

#endif
