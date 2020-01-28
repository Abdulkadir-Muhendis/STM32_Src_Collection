#include "stm32f4xx_hal.h"

void GPIO_Config(void);
void I2C_Config(void);
#define mpu9265Address	0xD0

I2C_HandleTypeDef myI2Chandle;
//I2C variables 
uint8_t i2cBuf[8];
int16_t ax,ay,az;
float Xaccel,Yaccel,Zaccel;
int main(void)
{
	//HAL Initialise
	HAL_Init();
	//Config functions
	GPIO_Config();
	I2C_Config();
	//1. Scan the I2C addresses
	for(uint8_t i=0; i<255; i++)
	{
		if(HAL_I2C_IsDeviceReady(&myI2Chandle, i, 1, 10) == HAL_OK)
		{
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
			break;
		}
	}
	//2. I2C Write example
	//a) Set accelerometer range (reg28)
	i2cBuf[0] = 28;			//Register address: Accelerometer config 1
	i2cBuf[1] = 0x08;		//Data to write, +-8g range
	HAL_I2C_Master_Transmit(&myI2Chandle, mpu9265Address, i2cBuf, 2, 10);
	
	//3. I2C Read example
	//Request to read from a register (reg28)
	i2cBuf[0] = 28;			//Register address: Accelerometer config 1
	HAL_I2C_Master_Transmit(&myI2Chandle, mpu9265Address, i2cBuf, 1, 10);
	//Read data
	i2cBuf[1] = 0x00;
	HAL_I2C_Master_Receive(&myI2Chandle, mpu9265Address|0x01, &i2cBuf[1], 1, 10);
	
	while(1)
	{
		//4. Read accelerometer data
		//Request to read from a register
		i2cBuf[0] = 0x3B;			//Register address: X_axis H
		HAL_I2C_Master_Transmit(&myI2Chandle, mpu9265Address, i2cBuf, 1, 10);
		//Read data
		i2cBuf[1] = 0x00;
		HAL_I2C_Master_Receive(&myI2Chandle, mpu9265Address|0x01, &i2cBuf[1], 6, 10);
		
		ax = -(i2cBuf[1]<<8 | i2cBuf[2]);
		ay = -(i2cBuf[3]<<8 | i2cBuf[4]);
		az = 	(i2cBuf[5]<<8 | i2cBuf[6]);

		Xaccel = ax/8192.0;
		Yaccel = ay/8192.0;
		Zaccel = az/8192.0;
		HAL_Delay(50);
	}
}

void GPIO_Config(void)
{
	//Enable Ports clocks
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	//Init tyoeDef
	GPIO_InitTypeDef myPinInit;
	//LED pins config
	myPinInit.Pin = GPIO_PIN_12;
	myPinInit.Mode = GPIO_MODE_OUTPUT_PP;
	myPinInit.Pull = GPIO_NOPULL;
	myPinInit.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &myPinInit);
	//I2C pins config
	myPinInit.Pin = GPIO_PIN_6 |GPIO_PIN_7;
	myPinInit.Mode = GPIO_MODE_AF_OD;
	myPinInit.Pull = GPIO_PULLUP;
	myPinInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	myPinInit.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &myPinInit);
	//Systick interrupt enable for HAL_Delay function
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void I2C_Config(void)
{
	//Enable I2C peripheral clock
	__HAL_RCC_I2C1_CLK_ENABLE();
	
	myI2Chandle.Instance = I2C1;
	myI2Chandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	myI2Chandle.Init.ClockSpeed = 100000;
	myI2Chandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	myI2Chandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	myI2Chandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	myI2Chandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	myI2Chandle.Init.OwnAddress1 = 0;
	myI2Chandle.Init.OwnAddress2 = 0;
	HAL_I2C_Init(&myI2Chandle);
}
void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}
