#ifndef BSP_INC_BSP_TIMERS_H_
#define BSP_INC_BSP_TIMERS_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

void	BSP_TIM2PWM_Init(void);
void	BSP_TIM2PWM_Start(void);
void	BSP_TIM2PWM_Stop(void);

void	BSP_TIM4ADC_Init(void);
void	BSP_TIM4ADC_Start(void);
void	BSP_TIM4ADC_Stop(void);
#endif
