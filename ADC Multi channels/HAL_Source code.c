#include "stm32f4xx_hal.h"

//Functions prototypes
void GPIO_Config(void);
void DMA_Config(ADC_HandleTypeDef* hadc);
void ADC_Config(void);

//HAndle typedefs
DMA_HandleTypeDef myDMA2_handle;
ADC_HandleTypeDef myADC_handle;

//Numeric variables
uint8_t adcVal[3];

int main(void)
{
	//HAL Initialisation
	HAL_Init();
	//GPIO Analog input Config
	GPIO_Config();
	//DMA Config
	DMA_Config(&myADC_handle);
	//ADC Config
	ADC_Config();
	
	//Start ADC DMA
	HAL_ADC_Start_DMA(&myADC_handle, (uint32_t *)adcVal, 3);
	
	while(1)
	{	
	}
}

void GPIO_Config(void)
{
	//Peripheral clock enable (Port A)
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	//Initialise Pin0, Pin1, Pin2 on port A as Analog
	GPIO_InitTypeDef myAnalogGPIO;
	myAnalogGPIO.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
	myAnalogGPIO.Mode = GPIO_MODE_ANALOG;
	myAnalogGPIO.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &myAnalogGPIO);
}

void DMA_Config(ADC_HandleTypeDef* hadc)
{
	__HAL_RCC_DMA2_CLK_ENABLE();
	
	myDMA2_handle.Instance = DMA2_Stream0;
	myDMA2_handle.Init.Channel = DMA_CHANNEL_0;
	myDMA2_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
	myDMA2_handle.Init.PeriphInc = DMA_PINC_DISABLE;
	myDMA2_handle.Init.MemInc = DMA_MINC_ENABLE;
	myDMA2_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	myDMA2_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	myDMA2_handle.Init.Mode = DMA_CIRCULAR;
	myDMA2_handle.Init.Priority = DMA_PRIORITY_LOW;
	myDMA2_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	HAL_DMA_Init(&myDMA2_handle);
	
	//Link this DMA to ADC1
	__HAL_LINKDMA(hadc,DMA_Handle,myDMA2_handle);
	
	//Enable DMA2 transfer interrupt
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	
}
void ADC_Config(void)
{
	//Enable ADC1 clock
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	//Configure ADC normal
	myADC_handle.Instance = ADC1;
  myADC_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  myADC_handle.Init.Resolution = ADC_RESOLUTION_8B;
  myADC_handle.Init.ScanConvMode = ENABLE;
  myADC_handle.Init.ContinuousConvMode = ENABLE;
  myADC_handle.Init.DiscontinuousConvMode = DISABLE;
  myADC_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  myADC_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  myADC_handle.Init.NbrOfConversion = 3;
  myADC_handle.Init.DMAContinuousRequests = ENABLE;
  myADC_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	HAL_ADC_Init(&myADC_handle);
	
	//ADC channel settings
	ADC_ChannelConfTypeDef ADC_ChConfg;
	
	//ADC Channel0
  ADC_ChConfg.Channel = ADC_CHANNEL_0;
  ADC_ChConfg.Rank = 1;
  ADC_ChConfg.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  HAL_ADC_ConfigChannel(&myADC_handle, &ADC_ChConfg);

  //ADC Channel1
  ADC_ChConfg.Channel = ADC_CHANNEL_1;
  ADC_ChConfg.Rank = 2;
  ADC_ChConfg.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  HAL_ADC_ConfigChannel(&myADC_handle, &ADC_ChConfg);
	
	//ADC Channel2
  ADC_ChConfg.Channel = ADC_CHANNEL_2;
  ADC_ChConfg.Rank = 3;
  ADC_ChConfg.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  HAL_ADC_ConfigChannel(&myADC_handle, &ADC_ChConfg);

}
