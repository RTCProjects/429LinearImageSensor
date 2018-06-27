/*----------------------------------------------------------------------------------------------------*/
/**
  * @file    tcd1304.c Модуль для работы Toshiba TCD1304AP
  * @brief
**/
/*----------------------------------------------------------------------------------------------------*/
#include "tcd1304ap.h"
#include "bsp_adc.h"
#include "bsp_usb.h"
#include "bsp_timers.h"
#include "process.h"

static uint16_t	icgTick = 0;

TIM_HandleTypeDef    Tim3PWMHandle;
TIM_OC_InitTypeDef 	 sConfigPWM;

static tTCDADCData	tcdADCData;
static tTCDADCData	adcTCDPack[2];
/*
uint16_t		sendCopyBuf1[TCD1304_DATA_SIZE];
uint16_t		sendCopyBuf2[TCD1304_DATA_SIZE];
uint16_t		sendCopyBuf3[TCD1304_DATA_SIZE];
uint16_t		sendCopyBuf4[TCD1304_DATA_SIZE];
*/
osThreadId 			sensorTaskHandle;
xQueueHandle 		sensorADC1DataQueue;
uint16_t 			ulADCSampleCounter;

void	TCD1304_Task(void const *argument);
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Инициализация TCD1304AP
  * @param	None
  * @reval	None
  */
void	TCD1304_Init()
{
	ulADCSampleCounter = 0;

	sensorADC1DataQueue = xQueueCreate(TCD_QUEUE_SIZE, sizeof(tTCDADCData));

	osThreadDef(sensorTask, TCD1304_Task, osPriorityAboveNormal, 0, configMINIMAL_STACK_SIZE + 0x400);
	sensorTaskHandle = osThreadCreate(osThread(sensorTask), NULL);
	/*
	 * Входная частота APB1 - 84Mhz
	 *
	 */

	Tim3PWMHandle.Instance = TIM3;

	Tim3PWMHandle.Init.Prescaler     = 4;
	Tim3PWMHandle.Init.Period        = 200 - 1;
	Tim3PWMHandle.Init.ClockDivision = 0;
	Tim3PWMHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;

	if(HAL_TIM_PWM_Init(&Tim3PWMHandle) != HAL_OK){
		Error_Handler();
	}

	sConfigPWM.OCMode     = TIM_OCMODE_PWM1;
	sConfigPWM.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigPWM.OCFastMode = TIM_OCFAST_DISABLE;

	sConfigPWM.Pulse = 100;

	if(HAL_TIM_PWM_ConfigChannel(&Tim3PWMHandle, &sConfigPWM, TIM_CHANNEL_1) != HAL_OK){
		  Error_Handler();
		}
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Запуск генерации ШИМ
  * @reval 	None
  */
void	TCD1304_Start()
{
	icgTick = 0;
	TIM3->CNT = 0;

	if(HAL_TIM_PWM_Start_IT(&Tim3PWMHandle, TIM_CHANNEL_1) != HAL_OK)
		Error_Handler();
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Остановка генерации ШИМ
  * @reval 	None
  */
void	TCD1304_Stop()
{
	if(HAL_TIM_PWM_Stop_IT(&Tim3PWMHandle, TIM_CHANNEL_1) != HAL_OK)
		Error_Handler();
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Основной поток обработки данных с линейки
  * @reval 	None
  */
void	TCD1304_Task(void const * argument)
{
		portBASE_TYPE			 xStatus;

		while(1)
		{
			osDelay(100);
			xStatus=xQueueReceive(sensorADC1DataQueue, &tcdADCData, portMAX_DELAY);
			if (xStatus == pdPASS) {

				if(tcdADCData.Adc == 1){
					uint16_t	*pADCData1 = tcdADCData.Data;
					uint16_t	*pADCData2 = tcdADCData.Data + 256;

					//memcpy(sendCopyBuf1,pADCData1,sizeof(uint16_t) * 256);
					//memcpy(sendCopyBuf2,pADCData2,sizeof(uint16_t) * 256);
					Process_Update(pADCData1,0);
					Process_Update(pADCData2,1);
				}
				if(tcdADCData.Adc == 2){
					uint16_t	*pADCData3 = tcdADCData.Data;
					uint16_t	*pADCData4 = tcdADCData.Data + 256;

					Process_Update(pADCData3,2);
					Process_Update(pADCData4,3);
					//memcpy(sendCopyBuf3,pADCData3,sizeof(uint16_t) * 256);
					//memcpy(sendCopyBuf4,pADCData4,sizeof(uint16_t) * 256);
				}
			}
		}
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	обработчик DMA от ADC
  * @param  adcValue: указатель на структуру данных с ADC
  * @reval 	None
  */
void	TCD1304_DataCallback(tTCDADCData	*adcValue)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;

	xHigherPriorityTaskWoken = pdFALSE;

		xQueueSendToBackFromISR(sensorADC1DataQueue, adcValue, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken != pdFALSE) {
    // Макрос, выполняющий переключение контекста.
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  }

}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	обработчик DMA от ADC
  * @param  htim : TIM handle
  * @reval 	None
  */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *hTim)
{
	//прерывание от каждого периода ШИМ
	if(hTim->Instance == TIM3)
	{
		//Стробируем строку, защелкивая ICG пин линейки
		if(icgTick==TCD1304_DELTA_TIME ){
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		if(icgTick>TCD1304_DELTA_TIME){
			BSP_TIM4ADC_Start();//по окончанию защелкивания - производим запуск выборки сигнала
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
			icgTick = 0;
		}
		icgTick++;
	}

}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	обработчик прерывания от TimerADCSample.
  * @note	Производим захват 256 значений из аналогово сигнала. Длительность сигнала - 9.131 mS. Длительность выборки - 9.131 / 256 = 35.66 uS
  * @reval 	None
  */
void TCD1304_SampleTimerCallback()
{
	if(ulADCSampleCounter<TCD1304_DATA_SIZE)
	{
		uint16_t	*adcBuf_1 = BSP_GetADCBuf1Value();
		uint16_t	*adcBuf_2 = BSP_GetADCBuf2Value();

		adcTCDPack[0].Adc = 1;
		adcTCDPack[0].Data[ulADCSampleCounter] = TCD1304_ADC_MAX_VALUE - adcBuf_1[0];
		adcTCDPack[0].Data[ulADCSampleCounter + TCD1304_DATA_SIZE] = TCD1304_ADC_MAX_VALUE - adcBuf_1[1];

		adcTCDPack[1].Adc = 2;
		adcTCDPack[1].Data[ulADCSampleCounter] = 4096 - adcBuf_2[0];
		adcTCDPack[1].Data[ulADCSampleCounter + TCD1304_DATA_SIZE] = TCD1304_ADC_MAX_VALUE - adcBuf_2[1];

		ulADCSampleCounter++;
	}
	else
	{
		BSP_TIM4ADC_Stop();

		TCD1304_DataCallback(&adcTCDPack[0]);
		TCD1304_DataCallback(&adcTCDPack[1]);

		ulADCSampleCounter = 0;
	}
}
/*----------------------------------------------------------------------------------------------------*/
