/*----------------------------------------------------------------------------------------------------*/
/**
  * @file    bsp_adc.c BSP Модуль для работы с ADC
  * @brief
**/
/*----------------------------------------------------------------------------------------------------*/
#include "bsp_adc.h"
#include "tcd1304ap.h"

ADC_HandleTypeDef    	ADC1Handle;
ADC_ChannelConfTypeDef 	ADC1Config;

ADC_HandleTypeDef    	ADC2Handle;
ADC_ChannelConfTypeDef 	ADC2Config;

uint16_t	adcBuf_1[2];
uint16_t	adcBuf_2[2];

//tTCDADCData	adcTCDPack[2];
//uint16_t	adcTCDPackInd_1 = 0;
//uint16_t	adcTCDPackInd_2 = 0;

/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Инициализация АЦП
  * @param	Mode: режим АЦП
  * @param  Adc: номер АЦП
  * @reval	None
  */
void	BSP_ADC_Init(eADCInitMode	Mode,uint8_t	Adc)
{
	switch(Mode)
	{
		case ADC_INIT_DMA_CONTCONV:
		{
			if(Adc == 1)
			{
				ADC1Handle.Instance          = ADC1;
				ADC1Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
				ADC1Handle.Init.Resolution = ADC_RESOLUTION_12B;
				ADC1Handle.Init.ScanConvMode = ENABLE;
				ADC1Handle.Init.ContinuousConvMode = ENABLE;
				ADC1Handle.Init.DiscontinuousConvMode = DISABLE;
				ADC1Handle.Init.NbrOfDiscConversion = 0;
				ADC1Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
				ADC1Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
				ADC1Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
				ADC1Handle.Init.NbrOfConversion = 2;
				ADC1Handle.Init.DMAContinuousRequests = ENABLE;
				ADC1Handle.Init.EOCSelection = DISABLE;

				if(HAL_ADC_Init(&ADC1Handle) != HAL_OK){
					Error_Handler();
				}

				 /*##-2- Configure ADC regular channel ######################################*/
				ADC1Config.Channel = ADC_CHANNEL_5;
				ADC1Config.Rank = 1;
				ADC1Config.SamplingTime = ADC_SAMPLETIME_112CYCLES;
				ADC1Config.Offset = 0;

				if(HAL_ADC_ConfigChannel(&ADC1Handle, &ADC1Config) != HAL_OK){
					Error_Handler();
				}

				ADC1Config.Channel = ADC_CHANNEL_7;
				ADC1Config.Rank = 2;
				ADC1Config.SamplingTime = ADC_SAMPLETIME_112CYCLES;
				ADC1Config.Offset = 0;

				if(HAL_ADC_ConfigChannel(&ADC1Handle, &ADC1Config) != HAL_OK){
					Error_Handler();
				}

				if(HAL_ADC_Start_DMA(&ADC1Handle, (uint32_t*)adcBuf_1, 2) != HAL_OK){
					Error_Handler();
				}
			}
			if(Adc == 2)
			{
				ADC2Handle.Instance          = ADC2;
				ADC2Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
				ADC2Handle.Init.Resolution = ADC_RESOLUTION_12B;
				ADC2Handle.Init.ScanConvMode = ENABLE;
				ADC2Handle.Init.ContinuousConvMode = ENABLE;
				ADC2Handle.Init.DiscontinuousConvMode = DISABLE;
				ADC2Handle.Init.NbrOfDiscConversion = 0;
				ADC2Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
				ADC2Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
				ADC2Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
				ADC2Handle.Init.NbrOfConversion = 2;
				ADC2Handle.Init.DMAContinuousRequests = ENABLE;
				ADC2Handle.Init.EOCSelection = DISABLE;

				if(HAL_ADC_Init(&ADC2Handle) != HAL_OK){
					Error_Handler();
				}

				 /*##-2- Configure ADC regular channel ######################################*/
				ADC2Config.Channel = ADC_CHANNEL_2;
				ADC2Config.Rank = 1;
				ADC2Config.SamplingTime = ADC_SAMPLETIME_112CYCLES;
				ADC2Config.Offset = 0;

				if(HAL_ADC_ConfigChannel(&ADC2Handle, &ADC2Config) != HAL_OK){
					Error_Handler();
				}

				ADC2Config.Channel = ADC_CHANNEL_3;
				ADC2Config.Rank = 2;
				ADC2Config.SamplingTime = ADC_SAMPLETIME_112CYCLES;
				ADC2Config.Offset = 0;

				if(HAL_ADC_ConfigChannel(&ADC2Handle, &ADC2Config) != HAL_OK){
					Error_Handler();
				}

				if(HAL_ADC_Start_DMA(&ADC2Handle, (uint32_t*)adcBuf_2, 2) != HAL_OK){
					Error_Handler();
				}
			}
		}break;
	}
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Запуск преобразования
  * @reval	None
  */
void	BSP_ADC_Start()
{
	if(HAL_ADC_Start_DMA(&ADC1Handle, (uint32_t*)adcBuf_1, 2) != HAL_OK)
		Error_Handler();

	if(HAL_ADC_Start_DMA(&ADC2Handle, (uint32_t*)adcBuf_2, 2) != HAL_OK)
		Error_Handler();

}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Остановка преобразования
  * @param  Adc: номер останавливаемого модуля АЦП
  * @reval	None
  */
void	BSP_ADC_Stop(uint8_t Adc)
{
	if(Adc == 1){
		if(HAL_ADC_Stop_DMA(&ADC1Handle)!= HAL_OK)
			Error_Handler();
	}
	if(Adc == 2){
		if(HAL_ADC_Stop_DMA(&ADC2Handle)!= HAL_OK)
			Error_Handler();
	}

}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Callback функция окончания преобразования АЦП
  * @param  AdcHandle: указатель на АЦП блок
  * @reval	None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
	/*if(AdcHandle->Instance == ADC1)
	{

		adcTCDPack[0].Adc = 1;
		adcTCDPack[0].Data[adcTCDPackInd_1] = adcBuf_1[0];
		adcTCDPack[0].Data[adcTCDPackInd_1+TCD1304_DATA_SIZE] = 4096 - adcBuf_1[1];
		adcTCDPackInd_1++;

		if(adcTCDPackInd_1>=TCD1304_DATA_SIZE)
		{
			adcTCDPackInd_1 = 0;
			BSP_ADC_Stop(1);
			TCD1304_DataCallback(&adcTCDPack[0]);
		}

	}
	if(AdcHandle->Instance == ADC2)
	{
		adcTCDPack[1].Adc = 2;
		adcTCDPack[1].Data[adcTCDPackInd_2] = adcBuf_2[0];
		adcTCDPack[1].Data[adcTCDPackInd_2+TCD1304_DATA_SIZE] = adcBuf_2[1];
		adcTCDPackInd_2++;

		if(adcTCDPackInd_2>=TCD1304_DATA_SIZE)
		{
			adcTCDPackInd_2 = 0;
			BSP_ADC_Stop(2);
			TCD1304_DataCallback(&adcTCDPack[1]);
		}
	}*/
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Получить значение из DMA буфера ADC1
  * @reval	None
  */
uint16_t	*BSP_GetADCBuf1Value()
{
	return adcBuf_1;
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Получить значение из DMA буфера ADC2
  * @reval	None
  */
uint16_t	*BSP_GetADCBuf2Value()
{
	return adcBuf_2;
}
/*----------------------------------------------------------------------------------------------------*/
