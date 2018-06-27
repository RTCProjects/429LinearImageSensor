/*----------------------------------------------------------------------------------------------------*/
/**
  * @file           main.c
  * @brief          Main program body
**/ 
/*----------------------------------------------------------------------------------------------------*/
#include <math.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#include "bsp_usb.h"
#include "bsp_adc.h"
#include "bsp_timers.h"
#include "bsp_flash.h"
#include "tcd1304ap.h"

#include "protocol.h"
#include "process.h"


//__IO uint8_t	pCCMBuffer[100] CCM_SRAM;

const uint8_t	spiArray[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};	//SPI
const uint8_t	rstArray[] = {0x40,0x00,0x00,0x00,0x00,0x95};	//SPI
const uint8_t	verArray[] = {0x48,0x00,0x00,0x01,0xAA,0x87};	//SPI

TIM_HandleTypeDef	htim7;

osThreadId defaultTaskHandle;
osThreadId defaultTaskHandle2;

void systemClock_Config(void);
void portClkInit(void);

void mainTask(void const * argument);
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  The application entry point.
  * @retval None
  */
int main(void)
{
  HAL_Init();

  systemClock_Config();
  portClkInit();

  osThreadDef(defaultTask, mainTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE + 0x100);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
	
  osKernelStart();	
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief 	System Clock Configuration
  * @retval None
  */
void systemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_5);

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}
/*----------------------------------------------------------------------------------------------------*/
/**
	* @brief 		Подключение IO(A,C,D,H) к APB
	* @param 		None
	* @reval		None	
	*/
void portClkInit(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
}
/*----------------------------------------------------------------------------------------------------*/
/** 
	* @brief 			Главный поток программы.
	* @param		 	argument: параметры потока FreeRTOS 
	* @reval			None
	*/

/*----------------------------------------------------------------------------------------------------*/

extern uint16_t sendCopyBuf1[];
extern uint16_t sendCopyBuf2[];
extern uint16_t sendCopyBuf3[];
extern uint16_t sendCopyBuf4[];
/*
#define QUERY_SIZE			4
#define ARRAY_SIZE			192
#define BACKGROUND_SIZE		30

static uint16_t 	 uQueryData[QUERY_SIZE][ARRAY_SIZE];
uint16_t	 uSendData[ARRAY_SIZE];
uint16_t	 uSendScaleData[ARRAY_SIZE];
uint32_t	 uSumData[ARRAY_SIZE];
uint8_t	 	 uQueryIndex = 0;

uint8_t		 uBackgroundReady = 0;
uint32_t 	 uBackgroundArray[ARRAY_SIZE];
*/

void mainTask(void const * argument)
{
	//Инициализация CDC_USB стека
	BSP_Usb_Init();

	BSP_TIM4ADC_Init();
	//Иницализация двух АЦП модулей
	BSP_ADC_Init(ADC_INIT_DMA_CONTCONV,1);
	BSP_ADC_Init(ADC_INIT_DMA_CONTCONV,2);
	//запуск ADC DMA для непрерываного преобразования
	BSP_ADC_Start();
	//Настройка ШИМ таймера для тактирования CCD линеек
	BSP_TIM2PWM_Init();
	//настройка и запуск драйвера CCD
	TCD1304_Init();
	TCD1304_Start();
	//Main loop
	Process_Init();

	//memset(uBackgroundArray,0,sizeof(uint32_t) * ARRAY_SIZE);

	for(;;)
	{
		//Process_Update(sendCopyBuf1,0);

		Protocol_SendLinearSensorData(0x01,0,(uint8_t*)Process_GetChannelData(0));
		Protocol_SendLinearSensorData(0x02,0,(uint8_t*)Process_GetChannelData(1));
		/*Protocol_SendLinearSensorData(0x02,0,(uint8_t*)sendCopyBuf2);
		Protocol_SendLinearSensorData(0x03,0,(uint8_t*)sendCopyBuf3);
		Protocol_SendLinearSensorData(0x04,0,(uint8_t*)sendCopyBuf4);*/

						osDelay(100);
	}
/*
	for(;;)
	{
		if(uQueryIndex < QUERY_SIZE)
		{
			memcpy(uQueryData[uQueryIndex],&sendCopyBuf2,sizeof(uint16_t) * ARRAY_SIZE);
			uQueryIndex++;


		}
		else
		{
			memcpy(&uQueryData[0],&uQueryData[1],sizeof(uint16_t) * ARRAY_SIZE * (QUERY_SIZE - 1));
			memcpy(&uQueryData[(QUERY_SIZE - 1)],&sendCopyBuf2,sizeof(uint16_t) * ARRAY_SIZE);

			memset(uSumData,0,sizeof(uint32_t) * ARRAY_SIZE);

			for(int i = 0;i<QUERY_SIZE;i++){
				for(int j = 0;j<ARRAY_SIZE;j++){
					uSumData[j]+=uQueryData[i][j];
				}
			}
			for(int i = 0;i<ARRAY_SIZE;i++)
				uSendData[i] = uSumData[i] / QUERY_SIZE;

			if(uBackgroundReady < BACKGROUND_SIZE)
			{
				for(int i = 0;i<ARRAY_SIZE;i++)
					uBackgroundArray[i] += uSendData[i];

				uBackgroundReady++;
				if(uBackgroundReady == BACKGROUND_SIZE)
				{
					for(int i = 0;i<ARRAY_SIZE;i++)
					{
						uBackgroundArray[i] /= BACKGROUND_SIZE;
						uBackgroundArray[i] = uBackgroundArray[i];
					}
				}

			}
			else
			{
				Protocol_SendLinearSensorData(0x01,0,(uint8_t*)sendCopyBuf1);
				Protocol_SendLinearSensorData(0x02,0,(uint8_t*)sendCopyBuf2);
				Protocol_SendLinearSensorData(0x03,0,(uint8_t*)sendCopyBuf3);
				Protocol_SendLinearSensorData(0x04,0,(uint8_t*)sendCopyBuf4);

				//for(int i = 0;i<ARRAY_SIZE;i++)
				//{
				//	__IO float fLogValue = log10((float)uBackgroundArray[i] * (float)TIM2->CCR1 / (float)uSendData[i]);
				//	uSendScaleData[i] = (uint16_t)(fLogValue * TIM2->CCR2);
				//}
				//Protocol_SendLinearSensorData(0x01,0,(uint8_t*)uSendScaleData);

			}


			//Protocol_SendLinearSensorData(0x01,0,(uint8_t*)uSendData);
		}

		osDelay(100);
	}*/
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
volatile uint32_t ulHighFrequencyTimerTicks = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  if(htim->Instance == TIM7){
	  ulHighFrequencyTimerTicks++;
  }
	if(htim->Instance == TIM4)
	{
		TCD1304_SampleTimerCallback();
	}
}
/*
 *
 */
uint32_t	GetRunTimeStatsValue()
{
	return ulHighFrequencyTimerTicks;
}
/*
 *
 */
void SetupRunTimeStatsTimer()
{
	RCC_ClkInitTypeDef    clkconfig;
	  uint32_t              uwTimclock = 0;
	  uint32_t              uwPrescalerValue = 0;
	  uint32_t              pFLatency;

	  /*Configure the TIM1 IRQ priority */
	  HAL_NVIC_SetPriority(TIM7_IRQn, 1 ,0);
	  HAL_NVIC_EnableIRQ(TIM7_IRQn);

	  /* Enable TIM1 clock */
	  __HAL_RCC_TIM7_CLK_ENABLE();

	  /* Get clock configuration */
	  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

	  /* Compute TIM1 clock */
	  uwTimclock = 2*HAL_RCC_GetPCLK2Freq();

	  /* Compute the prescaler value to have TIM1 counter clock equal to 10MHz */
	  uwPrescalerValue = (uint32_t) ((uwTimclock / 10000000) - 1);

	  /* Initialize TIM1 */
	  htim7.Instance = TIM7;

	  /* Initialize TIMx peripheral as follow:
	  + Period = [(TIM1CLK/1000) - 1]. to have a (1/1000) s time base.
	  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
	  + ClockDivision = 0
	  + Counter direction = Up
	  */
	  htim7.Init.Period = (10000000 / 1000) - 1;
	  htim7.Init.Prescaler = uwPrescalerValue;
	  htim7.Init.ClockDivision = 0;
	  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	  if(HAL_TIM_Base_Init(&htim7) == HAL_OK)
	  {
	    /* Start the TIM time Base generation in interrupt mode */
	    HAL_TIM_Base_Start_IT(&htim7);
	  }
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  while(1){
		
  }
}
/*----------------------------------------------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  
}
/*----------------------------------------------------------------------------------------------------*/
#endif /* USE_FULL_ASSERT */
