#ifndef INC_TCD1304AP_H_
#define INC_TCD1304AP_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define TCD1304_ADC_MAX_VALUE	4096
#define TCD1304_DELTA_TIME		1920
#define TCD1304_DATA_SIZE		256
#define TCD_QUEUE_SIZE			2

typedef struct
{
	uint8_t		Adc;
	uint16_t	Data[TCD1304_DATA_SIZE*2];
}tTCDADCData;

void	TCD1304_Init(void);
void	TCD1304_DataCallback(tTCDADCData	*adcValue);
void	TCD1304_Start(void);
void	TCD1304_Stop(void);

void	TCD1304_SampleTimerCallback();
#endif /* INC_TCD1304AP_H_ */
