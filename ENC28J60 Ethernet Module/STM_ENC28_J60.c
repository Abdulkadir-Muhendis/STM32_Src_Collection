#include "STM_ENC28_J60.h"

//variables
extern SPI_HandleTypeDef hspi1;
static uint8_t Enc28_Bank;
uint8_t dataWatch8;
uint16_t dataWatch16;

uint8_t ENC28_readOp(uint8_t oper, uint8_t addr)
{
	uint8_t spiData[2];
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	spiData[0] = (oper| (addr & ADDR_MASK));
	HAL_SPI_Transmit(&hspi1, spiData, 1, 100);
	if(addr & 0x80)
	{
		//HAL_SPI_Transmit(&hspi1, spiData, 1, 100);
		HAL_SPI_Receive(&hspi1, &spiData[1], 1, 100);
	}
	HAL_SPI_Receive(&hspi1, &spiData[1], 1, 100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	
	return spiData[1];
}
void ENC28_writeOp(uint8_t oper, uint8_t addr, uint8_t data)
{
	uint8_t spiData[2];
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	spiData[0] = (oper| (addr & ADDR_MASK)); //((oper<<5)&0xE0)|(addr & ADDR_MASK);
	spiData[1] = data;
	HAL_SPI_Transmit(&hspi1, spiData, 2, 100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
}
uint8_t ENC28_readReg8(uint8_t addr)
{
	ENC28_setBank(addr);
	return ENC28_readOp(ENC28_READ_CTRL_REG, addr);
}

void ENC28_writeReg8(uint8_t addr, uint8_t data)
{
	ENC28_setBank(addr);
	ENC28_writeOp(ENC28_WRITE_CTRL_REG, addr, data);
}

uint16_t ENC28_readReg16( uint8_t addr)
{
	return ENC28_readReg8(addr) + (ENC28_readReg8(addr+1) << 8);
}
void ENC28_writeReg16(uint8_t addrL, uint16_t data)
{
	ENC28_writeReg8(addrL, data);
	ENC28_writeReg8(addrL+1, data >> 8);
}

void ENC28_setBank(uint8_t addr)
{
	if ((addr & BANK_MASK) != Enc28_Bank) 
	{
		ENC28_writeOp(ENC28_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
		Enc28_Bank = addr & BANK_MASK;
    ENC28_writeOp(ENC28_BIT_FIELD_SET, ECON1, Enc28_Bank>>5);
	}
}

void ENC28_writePhy(uint8_t addr, uint16_t data)
{
	ENC28_writeReg8(MIREGADR, addr);
	ENC28_writeReg8(MIWR, data);
	while (ENC28_readReg8(MISTAT) & MISTAT_BUSY);
}

uint16_t ENC28_readPhy(uint8_t addr)
{
	ENC28_writeReg8(MIREGADR, addr);							// pass the PHY address to the MII
	ENC28_writeReg8(MICMD, MICMD_MIIRD);					// Enable Read bit
	while (ENC28_readReg8(MISTAT) & MISTAT_BUSY);	// Poll for end of reading
	ENC28_writeReg8(MICMD, 0x00);									// Disable MII Read
	return ENC28_readReg8(MIRD) + (ENC28_readReg8(MIRD+1) << 8);
}

void ENC28_Init(void)
{
	uint8_t spiData[2];
	// (1): Disable the chip CS pin
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
	// (2): Perform soft reset to the ENC28J60 module
	ENC28_writeOp(ENC28_SOFT_RESET, 0, ENC28_SOFT_RESET);
	HAL_Delay(2);
	// (3): Wait untill Clock is ready
	while(!ENC28_readOp(ENC28_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY);
	// (4): Initialise RX and TX buffer size
	ENC28_writeReg16(ERXST, RXSTART_INIT);
	ENC28_writeReg16(ERXND, RXSTOP_INIT);

	ENC28_writeReg16(ETXST, TXSTART_INIT);
  ENC28_writeReg16(ETXND, TXSTOP_INIT);
	
	ENC28_writeReg16(ERXRDPT, RXSTART_INIT);
	ENC28_writeReg16(ERXWRPT, RXSTART_INIT);
	
	dataWatch16 = ENC28_readReg16(ERXND);
	
	// (5): Reviece buffer filters
	ENC28_writeReg8(ERXFCON, ERXFCON_UCEN|ERXFCON_ANDOR|ERXFCON_CRCEN);
	dataWatch8 = ENC28_readReg8(ERXFCON);
	// (6): MAC Control Register 1
	ENC28_writeReg8(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS|MACON1_PASSALL);
	dataWatch8 = ENC28_readReg8(ERXFCON);
	// (7): MAC Control Register 3
	ENC28_writeOp(ENC28_BIT_FIELD_SET, MACON3,MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
  // (8): NON/Back to back gap
	ENC28_writeReg16(MAIPG, 0x0C12);  // NonBackToBack gap
	ENC28_writeReg8(MABBIPG, 0x12);  // BackToBack gap
	// (9): Set Maximum framelenght
	ENC28_writeReg16(MAMXFL, MAX_FRAMELEN);	// Set Maximum frame length (any packet bigger will be discarded)
	// (10): Set the MAC address of the device
	ENC28_writeReg8(MAADR1, MAC_1);
	ENC28_writeReg8(MAADR2, MAC_2);
	ENC28_writeReg8(MAADR3, MAC_3);
	ENC28_writeReg8(MAADR4, MAC_4);
	ENC28_writeReg8(MAADR5, MAC_5);
	ENC28_writeReg8(MAADR6, MAC_6);
	
	dataWatch8 = ENC28_readReg8(MAADR1);
	dataWatch8 = ENC28_readReg8(MAADR2);
	dataWatch8 = ENC28_readReg8(MAADR3);
	dataWatch8 = ENC28_readReg8(MAADR4);
	dataWatch8 = ENC28_readReg8(MAADR5);
	dataWatch8 = ENC28_readReg8(MAADR6);
	
	if(dataWatch8==MAC_6)HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	
	//**********Advanced Initialisations************//
	// (1): Initialise PHY layer registers
	ENC28_writePhy(PHLCON, PHLCON_LED);
	ENC28_writePhy(PHCON2, PHCON2_HDLDIS);
	// (2): Enable Rx interrupt line
	ENC28_setBank(ECON1);
	ENC28_writeOp(ENC28_BIT_FIELD_SET, ECON1, ECON1_RXEN);
//	ENC28_writeOp(ENC28_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
//	ENC28_writeOp(ENC28_BIT_FIELD_SET, EIR, EIR_PKTIF);
}

void ENC28_packetSend(uint16_t len, uint8_t* dataBuf)
{
	uint8_t retry = 0;
	
	while(1)
	{
		ENC28_writeOp(ENC28_BIT_FIELD_SET, ECON1, ECON1_TXRST);
    ENC28_writeOp(ENC28_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		ENC28_writeOp(ENC28_BIT_FIELD_CLR, EIR, EIR_TXERIF|EIR_TXIF);
		
		// prepare new transmission 
		if(retry == 0)
		{
			ENC28_writeReg16(EWRPT, TXSTART_INIT);
			ENC28_writeReg16(ETXND, TXSTART_INIT+len);
			ENC28_writeOp(ENC28_WRITE_BUF_MEM, 0, 0);  //line 485 enc28j60.cpp
			ENC28_writeBuf(len, dataBuf);
		}
		
		// initiate transmission
		ENC28_writeOp(ENC28_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
		uint16_t count = 0;
		while ((ENC28_readReg8(EIR) & (EIR_TXIF | EIR_TXERIF)) == 0 && ++count < 1000U);
		if (!(ENC28_readReg8(EIR) & EIR_TXERIF) && count < 1000U) 
		{
       // no error; start new transmission
       break;
    }
		// cancel previous transmission if stuck
    ENC28_writeOp(ENC28_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
		break;
	}
}

void ENC28_writeBuf(uint16_t len, uint8_t* data)
{
	uint8_t spiData[2];
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	spiData[0] = ENC28_WRITE_BUF_MEM;
	HAL_SPI_Transmit(&hspi1, spiData, 1, 100);
	HAL_SPI_Transmit(&hspi1, data, len, 100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
}