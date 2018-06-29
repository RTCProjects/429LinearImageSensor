#include "process.h"
/*----------------------------------------------------------------------------------------------------*/

static tDeviceStruct	Devices[TCD_SENSORS] CCM_SRAM;
/*----------------------------------------------------------------------------------------------------*/
/*
 *
 */
void	Process_Init()
{
	memset(Devices,0,sizeof(tDeviceStruct) * 4);
}
/*----------------------------------------------------------------------------------------------------*/
/*
 *
 */
void	Process_BackgroundUpdate(tDeviceStruct	*pDevice,uint16_t *pBackgroundData)
{
	if(!pDevice || !pBackgroundData)
		return;

	if(pDevice->ulBackgroundIndex < BACKGROUND_SIZE)
	{
		for(int i = 0;i<TCD1304_DATA_SIZE;i++)
			pDevice->ulBackgroundArray[i] += pBackgroundData[i];

		pDevice->ulBackgroundIndex++;
		if(pDevice->ulBackgroundIndex == BACKGROUND_SIZE){
			for(int i = 0;i<TCD1304_DATA_SIZE;i++){
				pDevice->ulBackgroundArray[i] /= BACKGROUND_SIZE;
			}
		}
	}
}
/*----------------------------------------------------------------------------------------------------*/
/*
 *
 */
void	Process_Update(uint16_t	*pData,uint8_t	ulIndex)
{
	if(ulIndex > TCD_SENSORS)
		return;
	if(!pData)
		return;

	tDeviceStruct	*pDevice = &Devices[ulIndex];

	Process_BackgroundUpdate(pDevice,pData);

	if(pDevice->ulQueryIndex < QUERY_SIZE)
	{
		memcpy(pDevice->ulQueryData[pDevice->ulQueryIndex],pData,sizeof(uint16_t) * TCD1304_DATA_SIZE);
		pDevice->ulQueryIndex++;
	}
	else
	{
		memcpy(&pDevice->ulQueryData[0],&pDevice->ulQueryData[1],sizeof(uint16_t) * TCD1304_DATA_SIZE * (QUERY_SIZE - 1));
		memcpy(&pDevice->ulQueryData[(QUERY_SIZE - 1)],pData,sizeof(uint16_t) * TCD1304_DATA_SIZE);

		uint16_t	ulData[TCD1304_DATA_SIZE];
		memset(ulData,0,sizeof(uint16_t) * TCD1304_DATA_SIZE);
		//memset(pDevice->ulData,0,sizeof(uint16_t) * TCD1304_DATA_SIZE);

		for(int i = 0;i<QUERY_SIZE;i++){
			for(int j = 0;j<TCD1304_DATA_SIZE;j++){
				ulData[j]+=pDevice->ulQueryData[i][j];
			}
		}
		for(int i = 0;i<TCD1304_DATA_SIZE;i++)
			ulData[i] = ulData[i] / QUERY_SIZE;

		taskENTER_CRITICAL();
		memcpy(pDevice->ulData,ulData,sizeof(uint16_t) * TCD1304_DATA_SIZE);
		taskEXIT_CRITICAL();

		//оптическую плотность считаем только после накопления фона
		if(pDevice->ulBackgroundIndex==BACKGROUND_SIZE)
			Process_OpticalDensityCalculate(pDevice);
	}
}
/*----------------------------------------------------------------------------------------------------*/
/*
 *
 */
void	Process_OpticalDensityCalculate(tDeviceStruct	*pDevice)
{
	for(int i = 0;i<TCD1304_DATA_SIZE;i++)
	{
		float fLogValue = log10((float)pDevice->ulBackgroundArray[i] * (float)TIM2->CCR3 / (float)pDevice->ulData[i]);
		pDevice->ulOpticalData[i] = (uint16_t)(fLogValue * TIM2->CCR2);
	}
}
/*----------------------------------------------------------------------------------------------------*/
/*
 *
 */
uint16_t	*Process_GetSourcelData(uint8_t	channel)
{
	if(channel < TCD_SENSORS)
		return Devices[channel].ulData;
	else
		return 0;
}
/*----------------------------------------------------------------------------------------------------*/
uint16_t	*Process_GetOpticalData(uint8_t	channel)
{
	if(channel < TCD_SENSORS)
		return Devices[channel].ulOpticalData;
	else
		return 0;
}
/*----------------------------------------------------------------------------------------------------*/
