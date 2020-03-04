#include "usb_serial.h"
void usb_sendSerial_string(uint8_t * data)
{
	uint8_t ret;

  	ret = CDC_Transmit_FS(data, sizeof(data));
  	if(ret != HAL_OK)
  	{
  		Error_Handler();
  	}
}
