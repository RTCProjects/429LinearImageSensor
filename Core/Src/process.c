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
void	Process_BackgroundUpdate(uint16_t *pBackgroundData, uint8_t	ulIndex)
{

	if(ulIndex > TCD_SENSORS)
		return;
	if(!pBackgroundData)
		return;

	tDeviceStruct	*pDevice = &Devices[ulIndex];

	if(pDevice->ulBackgroundIndex < BACKGROUND_SIZE)
	{
		for(int i = 0;i<ARRAY_SIZE;i++)
			pDevice->ulBackgroundArray[i] += pBackgroundData[i];

		pDevice->ulBackgroundIndex++;
		if(pDevice->ulBackgroundIndex == BACKGROUND_SIZE){
			for(int i = 0;i<ARRAY_SIZE;i++){
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

	Process_BackgroundUpdate(pData,ulIndex);

	if(pDevice->ulQueryIndex < QUERY_SIZE)
	{
		memcpy(pDevice->ulQueryData[pDevice->ulQueryIndex],pData,sizeof(uint16_t) * ARRAY_SIZE);
		pDevice->ulQueryIndex++;
	}
	else
	{
		memcpy(&pDevice->ulQueryData[0],&pDevice->ulQueryData[1],sizeof(uint16_t) * ARRAY_SIZE * (QUERY_SIZE - 1));
		memcpy(&pDevice->ulQueryData[(QUERY_SIZE - 1)],pData,sizeof(uint16_t) * ARRAY_SIZE);

		memset(pDevice->ulData,0,sizeof(uint16_t) * ARRAY_SIZE);

		for(int i = 0;i<QUERY_SIZE;i++){
			for(int j = 0;j<ARRAY_SIZE;j++){
				pDevice->ulData[j]+=pDevice->ulQueryData[i][j];
			}
		}
		for(int i = 0;i<ARRAY_SIZE;i++)
			pDevice->ulData[i] = pDevice->ulData[i] / QUERY_SIZE;
	}
}
/*----------------------------------------------------------------------------------------------------*/
/*
 *
 */
uint16_t	*Process_GetChannelData(uint8_t	channel)
{
	if(channel < TCD_SENSORS)
		return Devices[channel].ulData;
	else
		return 0;
}
/*----------------------------------------------------------------------------------------------------*/
