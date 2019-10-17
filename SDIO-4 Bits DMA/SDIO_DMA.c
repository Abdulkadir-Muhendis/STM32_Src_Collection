uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
{
	HAL_SD_StateTypeDef state_return;
	HAL_SD_CardStateTypeDef sd_card_state_return;
	uint32_t timeout = 0;
	/* Read block(s) in DMA transfer mode */
	if (HAL_SD_ReadBlocks_DMA(&hsd, (uint8_t *) pData, ReadAddr, NumOfBlocks)
	!= HAL_OK) { return MSD_ERROR; }
	timeout = 0;
	do
	{
		state_return = HAL_SD_GetState(&hsd);
		timeout++;
	} while ((HAL_SD_STATE_BUSY == state_return) && (SD_DATATIMEOUT > timeout));
	if (HAL_SD_STATE_READY != state_return) { return MSD_ERROR; }
	timeout = 0;
	do
	{
		sd_card_state_return = HAL_SD_GetCardState(&hsd);
		timeout++;
	} while ((HAL_SD_CARD_TRANSFER != sd_card_state_return) && (SD_DATATIMEOUT > timeout));
	if ((SD_DATATIMEOUT <= timeout)) { return HAL_TIMEOUT; }
	return MSD_OK;
}

//====================================

uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
{
	uint8_t sd_state = MSD_OK;
	HAL_SD_StateTypeDef state_return;
	HAL_SD_CardStateTypeDef sd_card_state_return;
	uint32_t timeout = 0;
	if (HAL_SD_WriteBlocks_DMA(&hsd, (uint8_t *) pData, WriteAddr, NumOfBlocks)
	!= HAL_OK) { return MSD_ERROR; }
	timeout = 0;
	do
	{
	state_return = HAL_SD_GetState(&hsd);
	timeout++;
	} while ((HAL_SD_STATE_BUSY == state_return) && (SD_DATATIMEOUT > timeout));
	if (HAL_SD_STATE_READY != state_return) { return MSD_ERROR; }
	timeout = 0;
	do
	{
	sd_card_state_return = HAL_SD_GetCardState(&hsd);
	timeout++;
	} while ((HAL_SD_CARD_TRANSFER != sd_card_state_return) && (SD_DATATIMEOUT > timeout));
	if ((SD_DATATIMEOUT <= timeout)) { return HAL_TIMEOUT; }
	return MSD_OK;
}

//====================================

DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
	if (BSP_SD_ReadBlocks_DMA((uint32_t*) buff, (uint32_t) (sector),
	count) != MSD_OK)
	{
	return RES_ERROR;
	}
	return RES_OK;
}

//====================================

DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
	if (BSP_SD_WriteBlocks_DMA((uint32_t*) buff, (uint32_t) (sector),
	count) != MSD_OK)
	{
	return RES_ERROR;
	}
	return RES_OK;
}
