#include "stm32f4xx_hal.h"

void GPIO_Config(void);
void TIM_Config(void);

TIM_HandleTypeDef tim2Handle;
uint32_t counterVal;

int main(void)
{
	//HAL Initialisation
	HAL_Init();
	//Config functions
	GPIO_Config();
	TIM_Config();
	
	//Start Input Capture TIM2-Channel1
	HAL_TIM_IC_Start_IT(&tim2Handle, TIM_CHANNEL_1);
	
	while(1)
	{
		counterVal = __HAL_TIM_GetCounter(&tim2Handle);
	}
}

void GPIO_Config(void)
{
	//Enable port A clock
	__HAL_RCC_GPIOA_CLK_ENABLE();
	//Init TypeDef to configure GPIO pins
	GPIO_InitTypeDef myGPIO_Init;
	//basic configurations
	myGPIO_Init.Pin = GPIO_PIN_0;							//PA0
	myGPIO_Init.Mode = GPIO_MODE_AF_PP;				//Alternate function to link to a timer
	myGPIO_Init.Pull = GPIO_NOPULL;
	myGPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;
	myGPIO_Init.Alternate = GPIO_AF1_TIM2;		//Connect pin to TIM2 (This is on AF1)
	//Implement the configurations
	HAL_GPIO_Init(GPIOA, &myGPIO_Init);
	
}

void TIM_Config(void)
{
	//Enable TIM2 clock
	__HAL_RCC_TIM2_CLK_ENABLE();
	
	//TIM2 Base initialisation
	tim2Handle.Instance = TIM2;
	tim2Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim2Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim2Handle.Init.Prescaler = 16000;
  tim2Handle.Init.Period = 100000;
  HAL_TIM_Base_Init(&tim2Handle);
	
	//Clock source configuration
	TIM_ClockConfigTypeDef clkSrcConfig;
	clkSrcConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&tim2Handle, &clkSrcConfig);
	
	//Initialise Input capture
	HAL_TIM_IC_Init(&tim2Handle);
	
	//Master config
	TIM_MasterConfigTypeDef MstrConfig;
	MstrConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  MstrConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&tim2Handle, &MstrConfig);
	
	//Input capture Configuration
	TIM_IC_InitTypeDef IC_config;
	
	IC_config.ICFilter = 15;
	IC_config.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  IC_config.ICPrescaler = TIM_ICPSC_DIV1;
	IC_config.ICSelection = TIM_ICSELECTION_DIRECTTI;
	HAL_TIM_IC_ConfigChannel(&tim2Handle, &IC_config, TIM_CHANNEL_1);
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
	
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

uint32_t inputCaptureVal;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		inputCaptureVal = __HAL_TIM_GetCounter(htim);
		__HAL_TIM_SetCounter(htim, 0);
	}
	
}

void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&tim2Handle);
}


void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

