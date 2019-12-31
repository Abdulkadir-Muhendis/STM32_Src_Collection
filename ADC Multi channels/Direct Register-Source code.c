#include "stm32f4xx.h"
//Functions prototypes
void GPIO_Config(void);
void DMA_Config(void);
void ADC_Config(void);
void startADC_DMA(uint32_t SrcAddr, uint32_t DstAddr, uint32_t length);
//Numeric variables
uint8_t adcVal[3];

int main(void)
{
	//GPIO Analog input Config
	GPIO_Config();
	//DMA Config
	DMA_Config();
	//ADC Config
	ADC_Config();
	
	//Start ADC DMA
	
	startADC_DMA((uint32_t)&ADC1->DR, (uint32_t)adcVal, 3);
	
	while(1)
	{
		
		
	}
}

void GPIO_Config(void)
{
	//Enable GPIOA clock
	RCC->AHB1ENR |= 0x01;
	RCC->AHB1ENR |= (1UL << 3);
	
	//Set pins 0,1,2 as Analog
	GPIOA->MODER |= (3UL <<0*2) | (3UL <<1*2) | (3UL <<2*2);
	GPIOD->MODER |= (1UL <<12*2);
}

void DMA_Config(void)
{
	//DMA2 clock enable
	RCC->AHB1ENR |= (1UL << 22); 
	//Disable DMA stream to start off with
	DMA2_Stream0->CR &= ~(1UL<<0);
	while(((DMA2_Stream0->CR)&0x1) ==0x1);  //Wait until DMA is disabled
	//Clear all the settings
	DMA2_Stream0->CR &= ~((0x7FUL <<21) | (0xFUL <<16) | (0x1FFUL << 6));
	//Apply DMA settings
	DMA2_Stream0->CR &= ~(7UL << 25);  // Select Channel0 DMA
	DMA2_Stream0->CR &= ~(3UL << 6);   //Direction: Peripheral to Memory
	DMA2_Stream0->CR &= ~(1UL << 9);   //Disable Peripheral Increment
	DMA2_Stream0->CR |= (1UL << 10);   //Enable Memory increment
	DMA2_Stream0->CR &= ~(3UL << 11);  //Peripheral datasize = Byte
	DMA2_Stream0->CR &= ~(3UL << 13);  //Memory datasize = Byte
	
	DMA2_Stream0->CR |= (1UL << 8);    //Enable Circular mode
	DMA2_Stream0->CR &= ~(3UL << 16);  //Priority Level = Low
	DMA2_Stream0->FCR &= ~(1UL << 2);  //Disable FIFO
	
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	NVIC_SetPriority(DMA2_Stream0_IRQn, 0);
	
}
void ADC_Config(void)
{
	//Enable ADC1 clock
	RCC->APB2ENR |= (1UL << 8);
	// ADC basic configuration
	ADC1->CR2 &= ~((1UL << 30) | (1UL << 0));  //Disable ADC to start off
	
	ADC1->CR1 &= ~(3UL << 24); //Clear resolution field
	ADC1->CR1 |= (2UL << 24);  //Set Resolution to 8-bits
	ADC1->CR1 |= (1UL << 8);   //Enable Scan mode
	ADC1->CR1 |= (1UL << 26);   //Enable OVR
	ADC1->CR1 |= (1UL << 5);   //EOC Interrupt enable
	ADC1->CR2 &= ~(1UL << 11); //Data Align to Right
	ADC1->CR2 |= (3UL << 8);   //Enable DMA contiuous tranfser
	ADC1->CR2 |= (1UL << 2);   //Enable ADC continuous conversion
	ADC->CCR |= (3UL << 16);   //Set clock prescalar to 8
	ADC1->CR2 |= (1UL << 10);  //EOC after signle conv
	
	ADC1->SMPR2 |= (7UL << 0); //Channel0 Sampling time to 480 cycles
	ADC1->SMPR2 |= (7UL << 3); //Channel1 Sampling time to 480 cycles
	ADC1->SMPR2 |= (7UL << 6); //Channel2 Sampling time to 480 cycles
	
	ADC1->SQR1 = (0x02 << 20); //Number of conversion = 3
	ADC1->SQR3 = 0x00;        
	ADC1->SQR3 |= (0UL << 0);  //Channel0 1st
	ADC1->SQR3 |= (1UL << 5);  //Channel2 2nd
	ADC1->SQR3 |= (2UL << 10);  //Channel3 3rd
	
}

void startADC_DMA(uint32_t SrcAddr, uint32_t DstAddr, uint32_t length)
{
	ADC1->CR2 &=  ~(1UL);  //Stop ADC1
	//Clear DMB bit
	DMA2_Stream0->CR &= ~(1UL<<18);
	//Set DMA data length
	DMA2_Stream0->NDTR = length;
	//Set Source Address
	DMA2_Stream0->PAR = SrcAddr;
	//Set Destination Address
	DMA2_Stream0->M0AR = DstAddr;
	
	//Enable ADC DMA
	ADC1->CR2 |= (1UL << 8);
	//Clear all interrupt flags
	DMA2->LIFCR |= 0x7FUL;
	//Enable Transfer complete Interrupt
	DMA2_Stream0->CR |= (1UL << 4);
	//Enable Transfer Error and DMA Error interrupt
	DMA2_Stream0->CR |= (3UL << 1);
	//DMA Stream Enable
	DMA2_Stream0->CR |= (1UL << 0);

	//Clear ADC flags
	ADC2->SR = 0x3FUL;

	//Enable ADC
	ADC1->CR2 |= 2UL;
	ADC1->CR2 |=  1UL;  //Start ADC1
	ADC1->CR2 |=  (1<<30);
	//Stabilize the ADC clock
	uint32_t counter = (3UL * (SystemCoreClock / 1000000U));
	while(counter != 0U)
	{
		counter--;
	}
	GPIOD->ODR ^= (1UL << 12);
}

void DMA2_Stream0_IRQHandler(void)
{
	
}
