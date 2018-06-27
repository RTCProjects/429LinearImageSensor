#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <string.h>

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "main.h"

#define TCD_SENSORS			4
#define QUERY_SIZE			4
#define ARRAY_SIZE			256
#define BACKGROUND_SIZE		30

typedef struct
{
	uint32_t	ulBackgroundArray[ARRAY_SIZE];
	uint16_t	ulData[ARRAY_SIZE];
	uint16_t	ulQueryData[QUERY_SIZE][ARRAY_SIZE];
	uint8_t		ulQueryIndex;
	uint8_t		ulBackgroundIndex;

}tDeviceStruct;

void		Process_Init(void);
uint16_t	*Process_GetChannelData(uint8_t	channel);
void		Process_BackgroundUpdate(uint16_t *pBackgroundData, uint8_t	ulIndex);
void		Process_Update(uint16_t	*pData,uint8_t	ulIndex);

#endif
