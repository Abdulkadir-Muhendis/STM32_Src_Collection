#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "usart.h"

#define RECEIVED_DATA_SIZE 16

extern "C"{
	void USART6_IRQHandler(void);
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
}

void UsartProcessData(void);
void UsartDataParse();

unsigned char usartReceivedData[RECEIVED_DATA_SIZE];
unsigned char usartGetData;
float Temperature = 0;
volatile float Angular = 0;
float angularReaded=0;
unsigned char Index = 0;
unsigned char Direction = 0;
float Speed=0;

int main(){
	SystemClockConfigForStm32f415rg();
	MX_USART6_UART_Init();
	HAL_UART_Receive_IT(&huart6, &usartGetData, 1);
	while(1){
		UsartProcessData();
			}
}

void USART6_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart6);
}
int dataIndex = 0;
volatile bool DataOk = false;
volatile bool SOM = false;
volatile bool EOM = false;
unsigned char usartBuffer[RECEIVED_DATA_SIZE];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UsartDataParse();
	HAL_UART_Receive_IT(&huart6, &usartGetData, 1);
	UNUSED(huart);
}

void UsartProcessData()
{
	if(DataOk)
	{
		DataOk = false;
		memcpy(&angularReaded,&usartReceivedData[1],sizeof(float));
		Angular=angularReaded;
		memcpy(&Temperature,&usartReceivedData[5],sizeof(float));
		memcpy(&Speed,&usartReceivedData[9],sizeof(float));
		Direction = usartReceivedData[13];
		Index = usartReceivedData[14];
	}
}

void UsartDataParse()
{
	if(usartGetData == '$' && !SOM) // SOM is the start of message
	{
		SOM = true;
		dataIndex = 0;
	}
	if(SOM && !EOM && dataIndex < RECEIVED_DATA_SIZE) // EOM is the end of the message 
	{
		usartBuffer[dataIndex] = usartGetData;
		dataIndex++;
	}
	if(usartGetData == '#' && !EOM && SOM && dataIndex == RECEIVED_DATA_SIZE)
	{
		EOM = true;
		dataIndex = 0;
	}
	else if(usartGetData != '#' && !EOM && SOM && dataIndex == RECEIVED_DATA_SIZE)
	{
		SOM = false;
		EOM = true;
		dataIndex = 0;		
	}
	if(SOM && EOM)
	{
		SOM = false;
		EOM = false;
		for(int i=0; i<RECEIVED_DATA_SIZE; i++)
		{
			usartReceivedData[i] = usartBuffer[i];
			usartBuffer[i] = 0;
		}
		DataOk = true;
	}
}
