#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "UARTForInduction.h"
#include "UARTForBLE.h"
#include "I2C.h"
#include "ControlWeight.h"
#include "ControlInduction.h"
#include "ControlACLoads.h"
#include "ControlHeightForUpperHeater.h"
#include "ReadTemperatureADC.h"
#include "SystemStatus.h"
#include "ControlCookingTime.h"
#include "ControlRecipes.h"
#include "SetZeroWeight.h"

static uint8_t TXToBLE[eBLE_TX_ETX + 1] = {0};
static uint8_t RXFromBLE[eBLE_RX_ETX + 1] = {0};

static void StartCook(void)
{
   SetStepOfCurrentCooking(eSYSTEM_CUR_COOK_STARTED_1);
   SetUISystemStatus(eSYSTEM_STARTED);
   SetRemainedTime(210);
   // First cycle - induction
   SetCurrentInductionLevel(10);
   SetCurrentACLoads(0);
   SetTargetHeightForUpperHeater(5);
}

static void SendingTXBufferToBLE(void)
{
   Serial.print("To BLE = ");
   for(unsigned char i = 0; i <= eBLE_TX_ETX; i++)
   {
      while(!(UCSR3A & (1<<UDRE3))){};
      UDR3 = TXToBLE[i];
      Serial.print(TXToBLE[i], HEX);
      Serial.print(" ");
   }
   Serial.println("");
}

static void SetTXDataForBLE(void)
{
   uint16_t crc = 0;
   uint16_t foodsurfacetemp = GetFoodSurfaceTemperature();
   int16_t weight = GetCurrentWeight();
   uint16_t potbottomtemp = GetTemperatureForPotBottom();
   uint16_t remainedtime = GetRemainedTime();
   
   TXToBLE[eBLE_TX_STX] = STX;
   TXToBLE[eBLE_TX_UARTLENGTH] = TXUART_LENGTH_TO_BLE;
   TXToBLE[eBLE_TX_RESPONSE] = 0;
   TXToBLE[eBLE_TX_UISTATUS_SYSTEM] = (uint8_t)GetUISystemStatus();
   TXToBLE[eBLE_TX_INDUCTIONLEVEL] = GetCurrentInductionLevel();
   TXToBLE[eBLE_TX_ACLOADS] = GetCurrentACLoads();
   TXToBLE[eBLE_TX_HEIGHT_UPPERHEATER] = GetCurrentHeightForUpperHeater();
   TXToBLE[eBLE_TX_WEIGHT0] = (uint8_t)(weight >> 8);
   TXToBLE[eBLE_TX_WEIGHT1] = (uint8_t)weight;
   TXToBLE[eBLE_TX_POT_TEMPERATURE0] = (uint8_t)(potbottomtemp >> 8);
   TXToBLE[eBLE_TX_POT_TEMPERATURE1] = (uint8_t)potbottomtemp;
   TXToBLE[eBLE_TX_FOOD_TEMPERATURE0] = (uint8_t)(foodsurfacetemp >> 8);
   TXToBLE[eBLE_TX_FOOD_TEMPERATURE1] = (uint8_t)foodsurfacetemp;
   TXToBLE[eBLE_TX_HEIGHT_FROMSENSOR_TOTARGET] = (uint8_t)GetHeightFromSensorToTarget();
   TXToBLE[eBLE_TX_REMAINED_TIME0] = (uint8_t)(remainedtime >> 8);
   TXToBLE[eBLE_TX_REMAINED_TIME1] = (uint8_t)remainedtime;
   TXToBLE[eBLE_TX_DETECTED_FOOD_LR] = 0;
   TXToBLE[eBLE_TX_DETECTED_FOOD_RR] = 0;
   TXToBLE[eBLE_TX_DETECTED_FOOD_LF] = 0;
   TXToBLE[eBLE_TX_DETECTED_FOOD_RF] = 0;
   TXToBLE[eBLE_TX_NUMBER_OF_STEPS_CURRENT_COOKING] = GetStepOfCurrentCooking(); // Current cooking status
   TXToBLE[eBLE_TX_MAX_STEPS_CURRENT_COOKING] = GetMaxStepOfCurrentCooking();
   TXToBLE[eBLE_TX_WHATS_THE_RECIPE_INDEX_CURRENT_COOKING] = GetWhatstheRecipeIndexOfCurrentCooking();

   crc = CalcultationCRC(&TXToBLE[eBLE_TX_STX], TXUART_LENGTH_TO_BLE - 3);
   TXToBLE[eBLE_TX_CRC0] = (uint8_t)(crc >> 8);
   TXToBLE[eBLE_TX_CRC1] = (uint8_t)crc;
   TXToBLE[eBLE_TX_ETX] = ETX;
}

static void ProcessingForRecievedRXFromBLE(void)
{
   uint16_t recievedcrc = RXFromBLE[eBLE_RX_CRC0];

   Serial.println("BLE RX Recieved");
   recievedcrc = (recievedcrc << 8) | RXFromBLE[eBLE_RX_CRC1];
   if((recievedcrc != CalcultationCRC(&RXFromBLE[eBLE_RX_STX], RXUART_LENGTH_FROM_BLE - 3))
      || (RXUART_LENGTH_FROM_BLE != RXFromBLE[eBLE_RX_UARTLENGTH]))
   {
      return;
   }
   //eBLE_RX_CONNECTION_STATUS,
   SetUISystemStatus(RXFromBLE[eBLE_RX_UISTATUS_SYSTEM]);
   SetCurrentInductionLevel(RXFromBLE[eBLE_RX_INDUCTIONLEVEL]);
   SetCurrentACLoads(RXFromBLE[eBLE_RX_ACLOADS]);
   SetTargetHeightForUpperHeater(RXFromBLE[eBLE_RX_HEIGHT_UPPERHEATER]);
   //eBLE_RX_REQUEST_COOKING_TIME0,
   //eBLE_RX_REQUEST_COOKING_TIME1,
   //eBLE_RX_NUMBER_OF_STEPS_FOR_USERCOOKING,
   //eBLE_RX_MAX_STEPS_FOR_USERCOOKING,
   //eBLE_RX_WHATS_THE_RECIPE_INDEX_FOR_USERCOOKING,

   SetTXDataForBLE();
   TXToBLE[eBLE_TX_RESPONSE] = 1;
   SendingTXBufferToBLE();

   // Set Zero weight test
   // SetZeroWeight(RXFromBLE[eBLE_RX_WHATS_THE_RECIPE_INDEX_FOR_USERCOOKING]);
   // SendDataToThunderInductionFrequently();
   // SetZeroWeight(0);
   // SendDataToThunderInductionFrequently();


   // TEST : Start cook test
   // if(GetCurrentInductionLevel() != 0 || GetCurrentACLoads() != 0)
   // {
   //    // #### START COOK ??  #######
   //    // TODO : Check if started and set system started
   //    // AND start timer
   //    // countTimer(5);
   //    // SetRemainedTime();
   //    SetStepOfCurrentCooking(eSYSTEM_CUR_COOK_STARTED);
   //    SetUISystemStatus(eSYSTEM_STARTED);
   // }
   // else
   // {
   //    SetUISystemStatus(eSYSTEM_STANDBY);
   // }
   
   SetWhatstheRecipeIndexOfCurrentCooking(RXFromBLE[eBLE_RX_WHATS_THE_RECIPE_INDEX_FOR_USERCOOKING]);
   if(GetWhatstheRecipeIndexOfCurrentCooking() == 2)
   {
      // START
      Serial.println("### Start Recived!! ###");
      SetWhatstheRecipeIndexOfCurrentCooking(1);
      SendDataToBLEFrequently();
      SetWhatstheRecipeIndexOfCurrentCooking(0);
      
      StartCook();
   }
}

ISR(USART3_RX_vect)//This is from Thunder BLE
{
   uint8_t readbyte = UDR3;   
   static uint8_t readbyte_count = 0;

   switch(readbyte_count)
   {
      case 0:
         if(readbyte == STX)
         {
            RXFromBLE[eBLE_RX_STX] = readbyte;
            readbyte_count++;
         }
         else
         {
            readbyte_count = 0;
         }
         break;
      case eBLE_RX_UARTLENGTH:
         if(readbyte == RXUART_LENGTH_FROM_BLE)
         {
            RXFromBLE[eBLE_RX_UARTLENGTH] = readbyte;
            readbyte_count++;
         }
         else
         {
            readbyte_count = 0;
         }
         break;
      default:
         if(RXFromBLE[eBLE_RX_UARTLENGTH] >= readbyte_count)//stx, etx
         {
            RXFromBLE[readbyte_count] = readbyte;
            readbyte_count++;
            
            if((RXFromBLE[eBLE_RX_UARTLENGTH]) < readbyte_count)
            {
               if(RXFromBLE[RXFromBLE[eBLE_RX_UARTLENGTH]] == ETX)
               {
                  ProcessingForRecievedRXFromBLE();
                  Serial.print("From BLE = ");
                  for(uint8_t i = 0; i <= RXFromBLE[eBLE_RX_UARTLENGTH]; i++)
                  {
                     Serial.print(RXFromBLE[i], HEX);
                     Serial.print(" ");
                  }
                  Serial.println("");
               }
               readbyte_count = 0;
            }
         }
         break;
   }
}

void SendDataToBLEFrequently(void)
{
   SetTXDataForBLE();
   TXToBLE[eBLE_TX_RESPONSE] = 0;
   SendingTXBufferToBLE();
}

void InitUARTForBLE(void)
{
   pinMode(UART_PORT_RX_FOR_BLE,INPUT_PULLUP);
   pinMode(UART_PORT_TX_FOR_BLE,OUTPUT);

   noInterrupts();
   UBRR3 = UBRR_VALUE; // set baud rate
   UCSR3A = 0x00; //Clear send and receive flags
   UCSR3B |= (1 << TXEN3); //enable TX
	UCSR3B |= (1 << RXEN3); //enable RX
	UCSR3B |= (1 << RXCIE3); //RX complete interrupt
	UCSR3C |= (1 << UCSZ30) | (1 << UCSZ31); // no parity, 1 stop bit, 8-bit data

   interrupts();
}

