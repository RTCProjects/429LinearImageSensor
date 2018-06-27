#ifndef BSP_INC_BSP_ADC_H_
#define BSP_INC_BSP_ADC_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#define ADCs	2

typedef enum
{
	ADC_INIT_DMA_CONTCONV	= 0x01
}eADCInitMode;

void		BSP_ADC_Init(eADCInitMode	,uint8_t	);
uint16_t 	BSP_ADC_GetValue(void);
void		BSP_ADC_Start(void);
void		BSP_ADC_Stop(uint8_t);

uint16_t	*BSP_GetADCBuf1Value(void);
uint16_t	*BSP_GetADCBuf2Value(void);

#endif /* BSP_INC_BSP_ADC_H_ */
