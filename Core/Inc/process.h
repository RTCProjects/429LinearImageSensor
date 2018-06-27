#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <string.h>
#include <math.h>

#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define TCD_CHANNEL_1		0x00
#define TCD_CHANNEL_2		0x01
#define TCD_CHANNEL_3		0x02
#define TCD_CHANNEL_4		0x03

#define TCD_SENSORS			4
#define QUERY_SIZE			4
#define ARRAY_SIZE			256
#define BACKGROUND_SIZE		30

typedef struct
{
	uint32_t	ulBackgroundArray[ARRAY_SIZE];			//данные фона
	uint16_t	ulData[ARRAY_SIZE];						//текущие усредненные данные
	uint16_t	ulOpticalData[ARRAY_SIZE];				//данные оптической плотности
	uint16_t	ulQueryData[QUERY_SIZE][ARRAY_SIZE];	//очередь текущих данных
	uint8_t		ulQueryIndex;							//индекс очереди
	uint8_t		ulBackgroundIndex;						//индекс накопления фонового спектра
}tDeviceStruct;

void		Process_Init(void);
void		Process_BackgroundUpdate(tDeviceStruct	*pDevice,uint16_t *pBackgroundData);
void		Process_Update(uint16_t	*pData,uint8_t	ulIndex);
void		Process_OpticalDensityCalculate(tDeviceStruct	*pDevice);
uint16_t	*Process_GetSourcelData(uint8_t	channel);
uint16_t	*Process_GetOpticalData(uint8_t	channel);

#endif
