/*----------------------------------------------------------------------------------------------------*/
/**
  * @file    bsp_timers.c Модуль для работы с таймерами
  * @brief
**/
/*----------------------------------------------------------------------------------------------------*/
#include "bsp_timers.h"

TIM_HandleTypeDef    	Tim2PWMHandle;	//таймер генерации ШИМ для линеек
TIM_HandleTypeDef		Tim4ADCHandle;	//таймер захвата сигнала CCD линеек
TIM_OC_InitTypeDef 	 	sConfigPWM;
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Инициализация ШИМ таймеров
  * @param	None
  * @reval	None
  */
void	BSP_TIM2PWM_Init()
{
	Tim2PWMHandle.Instance = TIM2;

	Tim2PWMHandle.Init.Prescaler     = 8;
	Tim2PWMHandle.Init.Period        = 0x4000 - 1;
	Tim2PWMHandle.Init.ClockDivision = 0;
	Tim2PWMHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;

	if(HAL_TIM_PWM_Init(&Tim2PWMHandle) != HAL_OK){
			Error_Handler();
	}

	sConfigPWM.OCMode     = TIM_OCMODE_PWM1;
	sConfigPWM.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigPWM.OCFastMode = TIM_OCFAST_DISABLE;

	sConfigPWM.Pulse = 0x64;
	if(HAL_TIM_PWM_ConfigChannel(&Tim2PWMHandle, &sConfigPWM, TIM_CHANNEL_1) != HAL_OK){
			  Error_Handler();
	}

	sConfigPWM.Pulse = 1000;
	if(HAL_TIM_PWM_ConfigChannel(&Tim2PWMHandle, &sConfigPWM, TIM_CHANNEL_2) != HAL_OK){
				  Error_Handler();
	}

	sConfigPWM.Pulse = 3000;
	if(HAL_TIM_PWM_ConfigChannel(&Tim2PWMHandle, &sConfigPWM, TIM_CHANNEL_3) != HAL_OK){
					  Error_Handler();
	}

	BSP_TIM2PWM_Start();

}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Запуск TIM2 PWM
  * @param	None
  * @reval	None
  */
void	BSP_TIM2PWM_Start()
{
	if(HAL_TIM_PWM_Start(&Tim2PWMHandle, TIM_CHANNEL_1) != HAL_OK)
			Error_Handler();
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Остановка TIM2 PWM
  * @param	None
  * @reval	None
  */
void 	BSP_TIM2PWM_Stop()
{
	if(HAL_TIM_PWM_Stop(&Tim2PWMHandle, TIM_CHANNEL_1) != HAL_OK)
		Error_Handler();
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Инициализация таймера выборки сигнала с линейки
  * @reval 	None
  */
void	BSP_TIM4ADC_Init()
{
	Tim4ADCHandle.Instance = TIM4;

	Tim4ADCHandle.Init.Prescaler     = 0;
	Tim4ADCHandle.Init.Period        = 2995;
	Tim4ADCHandle.Init.ClockDivision = 0;
	Tim4ADCHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;

	if(HAL_TIM_Base_Init(&Tim4ADCHandle) != HAL_OK){
		Error_Handler();
	}

	if(HAL_TIM_Base_Start_IT(&Tim4ADCHandle) != HAL_OK){
		Error_Handler();
	}
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Запуск таймера
  * @reval 	None
  */
void	BSP_TIM4ADC_Start()
{
	if(HAL_TIM_Base_Start_IT(&Tim4ADCHandle) != HAL_OK){
		Error_Handler();
	}
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Остановка таймера
  * @reval 	None
  */
void	BSP_TIM4ADC_Stop()
{
	if(HAL_TIM_Base_Stop_IT(&Tim4ADCHandle) != HAL_OK){
		Error_Handler();
	}
}
/*----------------------------------------------------------------------------------------------------*/
