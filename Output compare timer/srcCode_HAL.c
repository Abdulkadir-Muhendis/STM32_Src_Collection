#include "stm32f4xx_hal.h"

//Functions prototypes
void GPIO_Config(void);
void TIM_Config(void);
//Variables declarations
TIM_HandleTypeDef tim4Handle;

int main(void)
{
	//HAL Initialisation
	HAL_Init();
	//Configuration functions
	GPIO_Config();
	TIM_Config();
	//Start timers
	HAL_TIM_OC_Start(&tim4Handle, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&tim4Handle, TIM_CHANNEL_2);
	HAL_TIM_OC_Start(&tim4Handle, TIM_CHANNEL_3);
	HAL_TIM_OC_Start(&tim4Handle, TIM_CHANNEL_4);
	
	while(1)
	{
		
	}
}

void GPIO_Config(void)
{
	//Enable GPIOD clock
	__HAL_RCC_GPIOD_CLK_ENABLE(); 
	//GPIO Initialisation typeDef
	GPIO_InitTypeDef myLED;
	
	myLED.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	myLED.Pull = GPIO_NOPULL;
	myLED.Speed = GPIO_SPEED_FREQ_MEDIUM;
	myLED.Mode = GPIO_MODE_AF_PP;   	//Select alternate function
	myLED.Alternate = GPIO_AF2_TIM4;	//Connect Multiplexer to AF2 for TIM4
	HAL_GPIO_Init(GPIOD, &myLED);
}
void TIM_Config(void)
{
	//Enable TIM4 peripheral clock
	__HAL_RCC_TIM4_CLK_ENABLE();
	
		
  
  
	
	//1. Basic timer initialisation
	tim4Handle.Instance = TIM4;
	tim4Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim4Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim4Handle.Init.Prescaler = 16000-1;
	tim4Handle.Init.Period = 400;
	HAL_TIM_Base_Init(&tim4Handle);
	//2. Clock source configuration
	TIM_ClockConfigTypeDef ClkSrcConfig;
	ClkSrcConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&tim4Handle, &ClkSrcConfig);
	//3. OC initialisation
	HAL_TIM_OC_Init(&tim4Handle);
	//4. Master configuration
	TIM_MasterConfigTypeDef MstrConfig;
	MstrConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	MstrConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&tim4Handle, &MstrConfig);
	//5. OC channel configuration
	TIM_OC_InitTypeDef OcConfig;
	OcConfig.OCFastMode = TIM_OCFAST_DISABLE;
	OcConfig.OCMode = TIM_OCMODE_TOGGLE;
	OcConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	//Channel 1 Output compare pulse
	OcConfig.Pulse = 100;
	HAL_TIM_OC_ConfigChannel(&tim4Handle, &OcConfig, TIM_CHANNEL_1);
	//Channel 2 Output compare pulse
	OcConfig.Pulse = 200;
	HAL_TIM_OC_ConfigChannel(&tim4Handle, &OcConfig, TIM_CHANNEL_2);
	//Channel 3 Output compare pulse
	OcConfig.Pulse = 300;
	HAL_TIM_OC_ConfigChannel(&tim4Handle, &OcConfig, TIM_CHANNEL_3);
	//Channel 4 Output compare pulse
	OcConfig.Pulse = 400;
	HAL_TIM_OC_ConfigChannel(&tim4Handle, &OcConfig, TIM_CHANNEL_4);
}