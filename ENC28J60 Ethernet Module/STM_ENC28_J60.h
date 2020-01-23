#include "stm32f4xx_hal.h"
// Operations Defines
#define ENC28_READ_CTRL_REG	 	 0x00
#define ENC28_WRITE_CTRL_REG   0x40 //0x02
#define ENC28_BIT_FIELD_CLR		 0xA0 //0x05
#define ENC28_BIT_FIELD_SET		 0x80 //0x04
#define ENC28_SOFT_RESET		   0xFF //0x07

#define ENC28_READ_BUF_MEM		 0x3A //0x01
#define ENC28_WRITE_BUF_MEM		 0x7A //0x03

// Masks and some constants
#define ADDR_MASK									0x1F
#define BANK_MASK									0x60  //0110 0000
#define RXSTART_INIT							0x0000
#define RXSTOP_INIT								0x0BFF
#define TXSTART_INIT							0x0C00
#define TXSTOP_INIT								0x11FF
#define MAX_FRAMELEN							1500
#define MISTAT_BUSY								0x01

// Bank0 - control registers addresses
#define ERDPT											0x00
#define EWRPT											0x02
#define ETXST											0x04
#define ETXND											0x06
#define ERXST											0x08
#define ERXND											0x0A
#define ERXRDPT										0x0C
#define ERXWRPT										0x0E

// Bank1 - control registers addresses
#define ERXFCON										0x18|0x20
#define EPMM0											0x08|0x20 
#define EPMCS											0x10|0x20
#define EPKTCNT										0x19|0x20

// Bank2 - control registers addresses
#define MACON1										0x00|0x40
#define MACON2										0x01|0x40
#define MACON3										0x02|0x40
#define MAIPG										  0x06|0x40|0x80
#define MABBIPG									  0x04|0x40|0x80
#define MAMXFL									  0x0A|0x40|0x80
#define MIREGADR									0x14|0x40|0x80
#define MIWR					    				0x16|0x40|0x80
#define MICMD					    				0x12|0x40|0x80
#define MIRD					    				0x18|0x40|0x80

// Bank3 - control registers addresses
#define MAADR1									  0x04|0x60|0x80
#define MAADR2									  0x05|0x60|0x80
#define MAADR3									  0x02|0x60|0x80
#define MAADR4									  0x03|0x60|0x80
#define MAADR5									  0x00|0x60|0x80
#define MAADR6									  0x01|0x60|0x80
#define MISTAT									  0x0A|0x60|0x80
#define EREVID									  0x12|0x60

// Common registers
#define EIE												0x1B
#define EIR												0x1C
#define ESTAT											0x1D
#define ECON1											0x1F
#define ECON2											0x1E

// BitField Defines
#define ECON1_BSEL0								0x01
#define ECON1_BSEL1								0x02
#define ESTAT_CLKRDY 							0x01
#define ECON2_PKTDEC							0x40
#define ECON2_AUTOINC							0x80
#define ECON1_RXEN								0x04
#define ECON1_TXRST								0x80
#define ECON1_TXRTS								0x08
#define ERXFCON_UCEN							0x80
#define ERXFCON_CRCEN							0x20
#define ERXFCON_PMEN							0x10
#define ERXFCON_BCEN							0x01 
#define ERXFCON_ANDOR							0x40
#define MACON1_MARXEN							0x01
#define MACON1_TXPAUS							0x08
#define MACON1_RXPAUS							0x04
#define MACON1_PASSALL						0x02
#define MACON3_PADCFG0						0x20
#define MACON3_TXCRCEN						0x10
#define MACON3_FRMLNEN						0x02
#define EIE_INTIE									0x80 
#define EIE_PKTIE									0x40
#define EIR_TXERIF								0x02
#define EIR_PKTIF 								0x40
#define EIR_TXIF									0x08
#define MICMD_MIIRD								0x01

//my MAC address  0x74,0x69,0x69,0x2D,0x30,0x36
#define MAC_1					0x36
#define MAC_2					0x30
#define MAC_3					0x2D
#define MAC_4					0x69
#define MAC_5					0x69
#define MAC_6					0x74

// PHY layer
#define PHCON2_HDLDIS							0x0100
#define PHLCON_LED								0x0122

#define PHCON2										0x10
#define PHSTAT2										0x11
#define PHLCON										0x14

//Functions prototypes
uint8_t ENC28_readOp(uint8_t oper, uint8_t addr);
void ENC28_writeOp(uint8_t oper, uint8_t addr, uint8_t data);

uint8_t ENC28_readReg8( uint8_t addr);
void ENC28_writeReg8(uint8_t addr, uint8_t data);

uint16_t ENC28_readReg16( uint8_t addr);
void ENC28_writeReg16(uint8_t addrL, uint16_t data);

void ENC28_setBank(uint8_t addr);

void ENC28_Init(void);

void ENC28_writePhy(uint8_t addr, uint16_t data);
uint16_t ENC28_readPhy(uint8_t addr);

void ENC28_packetSend(uint16_t len, uint8_t* dataBuf);
void ENC28_writeBuf(uint16_t len, uint8_t* data);
