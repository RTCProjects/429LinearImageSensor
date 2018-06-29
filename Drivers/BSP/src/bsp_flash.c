/*----------------------------------------------------------------------------------------------------*/
/**
  * @file           bsp_flash.c
  * @brief          ћодуль дл€ работы с FLASH пам€тью
**/
/*----------------------------------------------------------------------------------------------------*/
#include "bsp_flash.h"


static FLASH_EraseInitTypeDef EraseInitStruct;
static uint32_t BSP_Flash_GetSector(uint32_t Address);

osThreadId 		 flashTaskHandle;
xSemaphoreHandle xFlashWriteSemaphore;

void BSP_Flash_Task(void const * argument);
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  »нициализаци€ потока записи во флеш
  * @retval None
  */
void	BSP_Flash_Init()
{
	vSemaphoreCreateBinary(xFlashWriteSemaphore);


	osThreadDef(flashTask, BSP_Flash_Task, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE + 128);
	flashTaskHandle = osThreadCreate(osThread(flashTask), NULL);
}
void	BSP_Flash_StartWrite()
{
	xSemaphoreGive(xFlashWriteSemaphore);
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  Task записи во флеш
  * @retval None
  */
void BSP_Flash_Task(void const * argument)
{
	xSemaphoreTake( xFlashWriteSemaphore, portMAX_DELAY );

	while(1)
	{
		xSemaphoreTake( xFlashWriteSemaphore, portMAX_DELAY );
		BSP_Flash_WriteData();
	}
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  The application entry point.
  * @retval None
  */
void	BSP_Flash_WriteData()
{
	/*
	 * - определ€ем сектор по адресу
	 * - определ€ем размер сектора по адресу
	 * - определ€ем, помест€тс€ ли данные в сектор
	 * - стираем сектор
	 * - пишем блок данных
	 */
	 uint32_t SectorError = 0;
	 uint32_t Address = TCD_FLASH_DATA;
	 uint32_t Offset = 0;

	 HAL_FLASH_Unlock();
	 __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	                        FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

       EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	   EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	   EraseInitStruct.Sector = BSP_Flash_GetSector(TCD_FLASH_DATA);
	   EraseInitStruct.NbSectors = 1;
	   if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK){
		   Error_Handler();
	   }

	 __HAL_FLASH_DATA_CACHE_DISABLE();
	 __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

	 __HAL_FLASH_DATA_CACHE_RESET();
	 __HAL_FLASH_INSTRUCTION_CACHE_RESET();

	 __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	 __HAL_FLASH_DATA_CACHE_ENABLE();

	 for(int i = 0;i<TCD_SENSORS;i++)
	 {
		 uint16_t	*pTCDData = Process_GetSourcelData(i);
		 for(uint16_t j = 0;j<TCD1304_DATA_SIZE;j++)
		 {
			 HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Offset + Address + (j * 2),*(pTCDData + j));
		 }
		 Offset+=512;
	 }
	 HAL_FLASH_Lock();
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  ‘ункци€ чтени€ сохраненного спектра из Flash пам€ти
  * @param	channel: Ќомер спектрального канала
  * @retval ”казатель на массив спектра
  */
uint32_t	*BSP_Flash_ReadData(uint8_t	channel)
{
	switch(channel)
	{
		case TCD_CHANNEL_1: return (uint32_t*)(TCD_FLASH_DATA);
		case TCD_CHANNEL_2: return (uint32_t*)(TCD_FLASH_DATA + 512);
		case TCD_CHANNEL_3: return (uint32_t*)(TCD_FLASH_DATA + 1024);
		case TCD_CHANNEL_4: return (uint32_t*)(TCD_FLASH_DATA + 1536);
	}
	return 0;
}

/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t BSP_Flash_GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_SECTOR_11;
  }
  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_SECTOR_12;
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_SECTOR_13;
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_SECTOR_14;
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_SECTOR_15;
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_SECTOR_16;
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_SECTOR_17;
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_SECTOR_18;
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_SECTOR_19;
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_SECTOR_20;
  }
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_SECTOR_21;
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_SECTOR_22;
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_23;
  }

  return sector;
}
/*----------------------------------------------------------------------------------------------------*/

