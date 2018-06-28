/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

extern void _Error_Handler(char *, int);
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */

void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}
/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;

  if(htim->Instance == TIM2)
  {
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_TIM2_CLK_ENABLE();
	  //инициализация ШИМ GPIO
	  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	  GPIO_InitStruct.Pin = GPIO_PIN_15;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  }
  if(htim->Instance == TIM3)
  {
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_TIM3_CLK_ENABLE();
	  //инициализация SH
	  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;

	  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	  //инициализация ICG
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	  GPIO_InitStruct.Pin = GPIO_PIN_2;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);


	  HAL_NVIC_SetPriority(TIM3_IRQn, 6, 0);
	  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	  if(htim->Instance == TIM4)
	  {
		  __HAL_RCC_TIM4_CLK_ENABLE();
		  __HAL_RCC_GPIOB_CLK_ENABLE();
		  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		  GPIO_InitStruct.Pull = GPIO_PULLUP;
		  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

		  GPIO_InitStruct.Pin = GPIO_PIN_1;
		  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


		  HAL_NVIC_SetPriority(TIM4_IRQn, 10, 0);
		  HAL_NVIC_EnableIRQ(TIM4_IRQn);
	  }
}
/**
  * @brief ADC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;
  static DMA_HandleTypeDef  hdma_adc;
  static DMA_HandleTypeDef  hdma_adc2;


  if(hadc->Instance == ADC1)
  {
	  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	  /* Enable GPIO clock */
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  /* ADC1 Periph clock enable */
	  __HAL_RCC_ADC1_CLK_ENABLE();
	  /* Enable DMA2 clock */
	  __HAL_RCC_DMA2_CLK_ENABLE();

	  /*##-2- Configure peripheral GPIO ##########################################*/
	  /* ADC3 Channel8 GPIO pin configuration */
	  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
	  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  HAL_NVIC_SetPriority(ADC_IRQn, 9, 0);
	  HAL_NVIC_EnableIRQ(ADC_IRQn);
	  /*##-3- Configure the DMA streams ##########################################*/
	  /* Set the parameters to be configured */
	  hdma_adc.Instance = DMA2_Stream0;

	  hdma_adc.Init.Channel  = DMA_CHANNEL_0;
	  hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
	  hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
	  hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
	  hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	  hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	  hdma_adc.Init.Mode = DMA_CIRCULAR;
	  hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
	  hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	  hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
	  hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
	  hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE;

	  HAL_DMA_Init(&hdma_adc);

	  /* Associate the initialized DMA handle to the the ADC handle */
	  __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);

	  /*##-4- Configure the NVIC for DMA #########################################*/
	  /* NVIC configuration for DMA transfer complete interrupt */
	  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 10, 0);
	  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  }
  if(hadc->Instance == ADC2)
  {
	  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	 /* Enable GPIO clock */
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	 /* ADC1 Periph clock enable */
	 __HAL_RCC_ADC2_CLK_ENABLE();
	 /* Enable DMA2 clock */
	 __HAL_RCC_DMA2_CLK_ENABLE();

	 /*##-2- Configure peripheral GPIO ##########################################*/
	 	  /* ADC3 Channel8 GPIO pin configuration */
	 GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	 GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	 HAL_NVIC_SetPriority(ADC_IRQn, 9, 0);
	 HAL_NVIC_EnableIRQ(ADC_IRQn);
	 	  /*##-3- Configure the DMA streams ##########################################*/
	 	  /* Set the parameters to be configured */
	 hdma_adc2.Instance = DMA2_Stream2;

	 hdma_adc2.Init.Channel  = DMA_CHANNEL_1;
	 hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
	 hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
	 hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
	 hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	 hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	 hdma_adc2.Init.Mode = DMA_CIRCULAR;
	 hdma_adc2.Init.Priority = DMA_PRIORITY_HIGH;
	 hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	 hdma_adc2.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
	 hdma_adc2.Init.MemBurst = DMA_MBURST_SINGLE;
	 hdma_adc2.Init.PeriphBurst = DMA_PBURST_SINGLE;

	 HAL_DMA_Init(&hdma_adc2);

	 	  /* Associate the initialized DMA handle to the the ADC handle */
	 __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc2);
	 	  /*##-4- Configure the NVIC for DMA #########################################*/
	 	  /* NVIC configuration for DMA transfer complete interrupt */
	 HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 10, 0);
	 HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
