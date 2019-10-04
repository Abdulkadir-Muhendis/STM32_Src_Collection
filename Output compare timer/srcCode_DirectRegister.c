#include "stm32f4xx.h"

void GPIO_Config(void);
void TIM_Config(void);

int main(void)
{
	//configurations
	GPIO_Config();
	TIM_Config();
	//Start Timer2
	TIM4->CR1 |= 1UL;
	
	while(1)
	{
		
	}
}

void GPIO_Config(void)
{
	//GPIOD clock enable
	RCC->AHB1ENR |= (1UL << 3);
	//Set GPIOD pins 12 - 15 to alternate mode
	GPIOD->MODER &= ~((3UL << 12*2) | (3UL << 13*2) | (3UL << 14*2) | (3UL << 15*2));
	GPIOD->MODER |= (2UL << 12*2) | (2UL << 13*2) | (2UL << 14*2) | (2UL << 15*2);
	//Set alternate function to AF2 (TIM4)
	GPIOD->AFR[1] |= (2UL << 4*4) | (2UL << 5*4) | (2UL << 6*4) | (2UL << 7*4);
	//Set speed to medium
	GPIOD->OSPEEDR |= (1UL << 12*2) | (1UL << 13*2) | (1UL << 14*2) | (1UL << 15*2);
}
void TIM_Config(void)
{
	// Basic timer configuration
	RCC->APB1ENR |= (1UL << 2);  	//Enable TIM4 clock
	TIM4->CR1 &= ~(0x0010);    		//Set the mode to Count up 
	TIM4->PSC = 16000-1;					//Set the Prescalar
	TIM4->ARR = 400; 						//Set period (Auto reload) to 400
	TIM4->SR &= ~(0x0001);				//Clear Update interrupt flag
	// TIM Clock source config 
	TIM4->SMCR &= ~(0x7F);        //Reset register parameters
	TIM4->SMCR &= ~(0xFFUL << 8);
	TIM4->SMCR &= ~(0x7UL);    		//Clock source to internal
	//Master hardware config
	TIM4->SMCR &= ~(1UL << 7); 		//DISABLE MasterSlave mode
	TIM4->CR2 &= ~(7UL << 4);			//TRGO RESET
	//Output Compare configuration
	TIM4->CCER &= ~(1UL);					//Disable channel 1
	TIM4->CCER &= ~(1UL << 4);		//Disable channel 2
	TIM4->CCER &= ~(1UL << 8);		//Disable channel 3
	TIM4->CCER &= ~(1UL << 12);		//Disable channel 4
	
	//Channel 1 config
	TIM4->CCMR1 &= ~((3UL) | (7UL << 4));  //Reset both OutputCompare mode and CaptureCompare
	TIM4->CCMR1 |= (3UL << 4);    //Set OutputCompare mode to Toggle
	TIM4->CCMR1 &= ~(3UL << 0);   //CC to output
	TIM4->CCER &= ~(1UL << 1);   	//Output Compare polarity to active high
	TIM4->CCER |= (1UL << 0);   	//Capture compare output enable
	//Set the CC register value (i.e. Pulse)
	TIM4->CCR1 = 100;
	
	//Channel 2 config
	TIM4->CCMR1 &= ~((3UL<<8) | (7UL << (4+8)));  //Reset both OutputCompare mode and CaptureCompare
	TIM4->CCMR1 |= (3UL << (4+8));    //Set OutputCompare mode to Toggle
	TIM4->CCMR1 &= ~(3UL << 8);   //CC to output
	TIM4->CCER &= ~(1UL << (1+4));   	//Output Compare polarity to active high
	TIM4->CCER |= (1UL << 4);   	//Capture compare output enable
	//Set the CC register value (i.e. Pulse)
	TIM4->CCR2 = 200;
	
	//Channel 3 config
	TIM4->CCMR2 &= ~((3UL) | (7UL << 4));  //Reset both OutputCompare mode and CaptureCompare
	TIM4->CCMR2 |= (3UL << 4);    //Set OutputCompare mode to Toggle
	TIM4->CCMR2 &= ~(3UL << 0);   //CC to output
	TIM4->CCER &= ~(1UL << 9);   	//Output Compare polarity to active high
	TIM4->CCER |= (1UL << 8);   	//Capture compare output enable
	//Set the CC register value (i.e. Pulse)
	TIM4->CCR3 = 300;
	
	//Channel 4 config
	TIM4->CCMR2 &= ~((3UL<<8) | (7UL << (4+8)));  //Reset both OutputCompare mode and CaptureCompare
	TIM4->CCMR2 |= (3UL << (4+8));    //Set OutputCompare mode to Toggle
	TIM4->CCMR2 &= ~(3UL << 8);   //CC to output
	TIM4->CCER &= ~(1UL << 13);   	//Output Compare polarity to active high
	TIM4->CCER |= (1UL << 12);   	//Capture compare output enable
	//Set the CC register value (i.e. Pulse)
	TIM4->CCR4 = 400;
}