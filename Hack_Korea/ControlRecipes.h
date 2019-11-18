#ifndef CONTROLRECIPES_H
#define CONTROLRECIPES_H

typedef enum 
{
   eSYSTEM_CUR_COOK_OFF = 0,
   eSYSTEM_CUR_COOK_STARTED,
   eSYSTEM_CUR_COOK_DONE
} eSYSTEM_CURRENT_COOK_STATUS;

void SetStepOfCurrentCooking(eSYSTEM_CURRENT_COOK_STATUS status);
eSYSTEM_CURRENT_COOK_STATUS GetStepOfCurrentCooking(void);
uint8_t GetMaxStepOfCurrentCooking(void);
uint8_t GetWhatstheRecipeIndexOfCurrentCooking(void);
void SetWhatstheRecipeIndexOfCurrentCooking(uint8_t recipe);

#endif
