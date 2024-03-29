#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "UARTForInduction.h"
#include "ControlWeight.h"
#include "ControlInduction.h"
#include "ControlACLoads.h"
#include "ControlRecipes.h"
#include "ControlHeightForUpperHeater.h"
#include "SystemStatus.h"
#include "SetZeroWeight.h"
#include "ReadTemperatureADC.h"
#include "I2C.h"
#include "UARTForBLE.h"

static uint8_t TXToInduction[eIND_TX_ETX + 1] = {0};
static uint8_t RXFromInduction[eIND_RX_ETX + 1] = {0};

uint16_t CalcultationCRC(uint8_t *buf, uint8_t len)
{
	uint8_t index1 = 0;
	uint8_t index2 = 0;
	uint16_t ret_crc = 0;
	uint16_t crc_polynomial = 0x5105;
	uint16_t buf_mask = 0x8000;

	for(index1=0; index1<len; index1++){
		ret_crc = ret_crc^buf[index1]<<8;

		for(index2=0; index2<8; index2++){
			if(ret_crc & buf_mask){
				ret_crc = ret_crc<<1^crc_polynomial;
			}
			else{
				ret_crc = ret_crc<<1;
			}
		}
		ret_crc &=0xffff;
	}
	return ret_crc;
}

static void SendingTXBufferToThunderInduction(void)
{
   Serial.print("To Induction = ");
   for(unsigned char i = 0; i <= eIND_TX_ETX; i++)
   {
      while(!(UCSR2A & (1<<UDRE2))){};
      UDR2 = TXToInduction[i];
      Serial.print(TXToInduction[i], HEX);
      Serial.print(" ");
   }
   Serial.println("");
}

static void LogDataForThunderInduction(void)
{
   Serial.print("InductionStatus:InductionLevel:HeightForHeater:zeroweight = ");
   Serial.print(GetInductionSystemStatus());
   Serial.print(":");
   Serial.print(GetCurrentInductionLevel());
   Serial.print(":");
   Serial.print(GetTargetHeightForUpperHeater());
   Serial.print(":");
   Serial.println(GetZeroWeight());
}

static void SetTXDataForThunderInduction(void)
{
   LogDataForThunderInduction();
   uint16_t crc = 0;
   
   TXToInduction[eIND_TX_STX] = STX;
   TXToInduction[eIND_TX_UARTLENGTH] = TXUART_LENGTH_TO_INDUCTION;
   TXToInduction[eIND_TX_UISTATUS_SYSTEM] = (uint8_t)GetUISystemStatus();
   TXToInduction[eIND_TX_MAINSTATUS_SYSTEM] = (uint8_t)GetInductionSystemStatus();
   TXToInduction[eIND_TX_INDUCTIONLEVEL] = GetCurrentInductionLevel();
   TXToInduction[eIND_TX_ACLOADS] = GetCurrentACLoads();
   TXToInduction[eIND_TX_HEIGHT_UPPERHEATER] = GetTargetHeightForUpperHeater();
   TXToInduction[eIND_TX_SETZERO_WEIGHT] = GetZeroWeight();
   TXToInduction[eIND_TX_TEMP0] = 0;
   TXToInduction[eIND_TX_TEMP1] = 0;
   TXToInduction[eIND_TX_TEMP2] = 0;
   TXToInduction[eIND_TX_TEMP3] = 0;
   TXToInduction[eIND_TX_TEMP4] = 0;

   crc = CalcultationCRC(&TXToInduction[eIND_TX_STX], eIND_TX_TEMP4 + 1);
   TXToInduction[eIND_TX_CRC0] = (uint8_t)(crc >> 8);
   TXToInduction[eIND_TX_CRC1] = (uint8_t)crc;
   TXToInduction[eIND_TX_ETX] = ETX;
   if(GetUISystemStatus() == eSYSTEM_COOKDONE)
   {
      SetUISystemStatus(eSYSTEM_STANDBY);
   }
}

static void ProcessingForRecievedRXFromThunderInduction(void)
{
   uint16_t recievedcrc = RXFromInduction[eIND_RX_CRC0];
   int16_t weight = 0;
   
   recievedcrc = (recievedcrc << 8) | RXFromInduction[eIND_RX_CRC1];
   if((recievedcrc != CalcultationCRC(&RXFromInduction[eIND_RX_STX], eIND_RX_TEMP4 + 1))
      || (RXUART_LENGTH_FROM_INDUCTION != RXFromInduction[eIND_RX_UARTLENGTH]))
   {
      return;
   }

   SetInductionSystemStatus(RXFromInduction[eIND_RX_MAINSTATUS_SYSTEAM]);
   SetCurrentHeightForUpperHeater(RXFromInduction[eIND_RX_HEIGHT_UPPERHEATER]);

   Serial.print("@@@@@ InductionStatus: = ");
   Serial.println(GetInductionSystemStatus());

   switch (GetInductionSystemStatus())
   {
   case eSYSTEM_ON:
      SetUISystemStatus(eSYSTEM_INITING);
      break;
   case eSYSTEM_INITING:
      SetUISystemStatus(eSYSTEM_INITING);
      break;

   case eSYSTEM_INITED:
      SetUISystemStatus(eSYSTEM_INITED);
      SendDataToThunderInductionFrequently();
      SetUISystemStatus(eSYSTEM_STANDBY);
      SendDataToThunderInductionFrequently();
      break;
   case eSYSTEM_ERROR:
      Serial.println("@@@@@ ERROR @@@ ");
      SetUISystemStatus(eSYSTEM_ERROR);
      SendDataToThunderInductionFrequently();
      SetUISystemStatus(eSYSTEM_STANDBY);
      SendDataToThunderInductionFrequently();
      SetCurrentInductionLevel(0);
      SetCurrentACLoads(0);
      SetTargetHeightForUpperHeater(0);
      SetStepOfCurrentCooking(eSYSTEM_CUR_COOK_OFF);
      break;
   }

   weight = RXFromInduction[eIND_RX_VALUE_WEIGHT0];
   Serial.print("1 : ");
   Serial.println(RXFromInduction[eIND_RX_VALUE_WEIGHT0]);
   weight <<= 8;
   weight |= RXFromInduction[eIND_RX_VALUE_WEIGHT1];
   Serial.print("2 : ");
   Serial.println(RXFromInduction[eIND_RX_VALUE_WEIGHT1]);
   Serial.print("weight : ");
   Serial.println(weight);
   SetCurrentWeight(weight);

   // @@@   TODO : Read weight and configure..  @@@
   if(weight > 900)
   {
      // START DETECTING
      SetWhatstheRecipeIndexOfCurrentCooking(1);
      SendDataToBLEFrequently();
   }
   // else if (100 < weight < 200)
   // {
   //    SetZeroWeight(1);
   //    SendDataToThunderInductionFrequently();
   //    SetZeroWeight(0);
   // }
}

void SendDataToThunderInductionFrequently(void)
{
   SetTXDataForThunderInduction();
   SendingTXBufferToThunderInduction();
}

ISR(USART2_RX_vect)//This is for Thunder Indcution
{
   uint8_t readbyte = UDR2;   
   static uint8_t readbyte_count = 0;

   switch(readbyte_count)
   {
      case 0:
         if(readbyte == STX)
         {
            RXFromInduction[eIND_RX_STX] = readbyte;
            readbyte_count++;
         }
         else
         {
            readbyte_count = 0;
         }
         break;
      case eIND_RX_UARTLENGTH:
         if(readbyte == RXUART_LENGTH_FROM_INDUCTION)
         {
            RXFromInduction[eIND_RX_UARTLENGTH] = readbyte;
            readbyte_count++;
         }
         else
         {
            readbyte_count = 0;
         }
         break;
      default:
         if(RXFromInduction[eIND_RX_UARTLENGTH] >= readbyte_count)//stx, etx
         {
            RXFromInduction[readbyte_count] = readbyte;
            readbyte_count++;
            
            if((RXFromInduction[eIND_RX_UARTLENGTH]) < readbyte_count)
            {
               if(RXFromInduction[RXFromInduction[eIND_RX_UARTLENGTH]] == ETX)
               {
                  ProcessingForRecievedRXFromThunderInduction();
                  Serial.print("From Induction = ");
                  for(uint8_t i = 0; i <= RXFromInduction[eIND_RX_UARTLENGTH]; i++)
                  {
                     Serial.print(RXFromInduction[i], HEX);
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

void InitUARTForThunderInduction(void)
{
   pinMode(UART_PORT_RX_FOR_INDUCTION,INPUT_PULLUP);
   pinMode(UART_PORT_TX_FOR_INDUCTION,OUTPUT);

   noInterrupts();
   UBRR2 = UBRR_VALUE; // set baud rate
   UCSR2A = 0x00; //Clear send and receive flags
   UCSR2B |= (1 << TXEN2); //enable TX
	UCSR2B |= (1 << RXEN2); //enable RX
	UCSR2B |= (1 << RXCIE2); //RX complete interrupt
	UCSR2C |= (1 << UCSZ20) | (1 << UCSZ21); // no parity, 1 stop bit, 8-bit data

   interrupts();
}

void InitUARTForDebugging(void)
{
   Serial.begin(115200);
   Serial.println("Welcom UART testing Mode");
}

void PrintInformationForDebugging(void)
{
   Serial.print("Height:Tfood:Tbottom:Weight = ");
   Serial.print(GetHeightFromSensorToTarget());

   Serial.print(":");
   Serial.print(GetFoodSurfaceTemperature());

   Serial.print(":");
   Serial.print(GetTemperatureForPotBottom());

   Serial.print(":");
   Serial.println(GetCurrentWeight());
   
}

