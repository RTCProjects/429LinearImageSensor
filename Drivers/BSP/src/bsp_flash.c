/*----------------------------------------------------------------------------------------------------*/
/**
  * @file           bsp_flash.c
  * @brief          Модуль для работы с FLASH памятью
**/
/*----------------------------------------------------------------------------------------------------*/
#include "bsp_flash.h"


static FLASH_EraseInitTypeDef EraseInitStruct;
static uint32_t BSP_Flash_GetSector(uint32_t Address);
static uint32_t BSP_Flash_GetSectorSize(uint32_t Sector);

osThreadId 		 flashTaskHandle;
xSemaphoreHandle xFlashWriteSemaphore;
tSaveData		 SaveData USER_FLASH;

void BSP_Flash_Task(void const * argument);

/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  Инициализация потока записи во флеш
  * @retval None
  */
void	BSP_Flash_Init()
{
	memset(&SaveData,0,sizeof(tSaveData));
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
	while(1)
	{
		BSP_Flash_WriteBlock((uint8_t*)Process_GetSourcelData(0),TCD1304_DATA_SIZE * sizeof(uint16_t));
		xSemaphoreTake( xFlashWriteSemaphore, portMAX_DELAY );
	}
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  The application entry point.
  * @retval None
  */
void	BSP_Flash_WriteBlock(uint8_t	*pData,uint16_t	Size)
{
	/*
	 * - определяем сектор по адресу
	 * - определяем размер сектора по адресу
	 * - определяем, поместятся ли данные в сектор
	 * - стираем сектор
	 * - пишем блок данных
	 */
	 uint32_t SectorError = 0;
	 uint32_t Address = 0x80FC000;
	 uint16_t AddressIndex = 0;

	 HAL_FLASH_Unlock();
	 __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	                        FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

       EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	   EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	   EraseInitStruct.Sector = FLASH_SECTOR_11;
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


	 while(AddressIndex < 512){
		 HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address + AddressIndex,*(pData + AddressIndex));
		 AddressIndex+=2;
	 }
	 HAL_FLASH_Lock();
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  The application entry point.
  * @retval None
  */
void	BSP_Flash_ReadBlock(uint8_t	*pData,uint16_t	Size)
{

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
/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
static uint32_t BSP_Flash_GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;
  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) ||\
     (Sector == FLASH_SECTOR_3) || (Sector == FLASH_SECTOR_12) || (Sector == FLASH_SECTOR_13) ||\
     (Sector == FLASH_SECTOR_14) || (Sector == FLASH_SECTOR_15))
  {
    sectorsize = 16 * 1024;
  }
  else if((Sector == FLASH_SECTOR_4) || (Sector == FLASH_SECTOR_16))
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }
  return sectorsize;
}
/*----------------------------------------------------------------------------------------------------*/
