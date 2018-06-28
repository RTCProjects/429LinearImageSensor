#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <string.h>
#include <math.h>

#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "tcd1304ap.h"

#define TCD_CHANNEL_1		0x00
#define TCD_CHANNEL_2		0x01
#define TCD_CHANNEL_3		0x02
#define TCD_CHANNEL_4		0x03

#define TCD_SENSORS			4
#define QUERY_SIZE			4
#define BACKGROUND_SIZE		30

typedef struct
{
	uint32_t	ulBackgroundArray[TCD1304_DATA_SIZE];			//данные фона
	uint16_t	ulData[TCD1304_DATA_SIZE];						//текущие усредненные данные
	uint16_t	ulOpticalData[TCD1304_DATA_SIZE];				//данные оптической плотности
	uint16_t	ulQueryData[QUERY_SIZE][TCD1304_DATA_SIZE];		//очередь текущих данных
	uint8_t		ulQueryIndex;									//индекс очереди
	uint8_t		ulBackgroundIndex;								//индекс накопления фонового спектра
}tDeviceStruct;

void		Process_Init(void);
void		Process_BackgroundUpdate(tDeviceStruct	*pDevice,uint16_t *pBackgroundData);
void		Process_Update(uint16_t	*pData,uint8_t	ulIndex);
void		Process_OpticalDensityCalculate(tDeviceStruct	*pDevice);
uint16_t	*Process_GetSourcelData(uint8_t	channel);
uint16_t	*Process_GetOpticalData(uint8_t	channel);

#endif
