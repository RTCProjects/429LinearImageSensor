/*----------------------------------------------------------------------------------------------------*/
/**
  * @file    protocol.c Модуль реализации протокола обмена данными
  * @brief
**/
/*----------------------------------------------------------------------------------------------------*/
#include "protocol.h"
#include "bsp_usb.h"
#include <string.h>
/*----------------------------------------------------------------------------------------------------*/
extern uint16_t	sendCopyBuf1[];
extern uint16_t	sendCopyBuf2[];
extern uint16_t	sendCopyBuf3[];
extern uint16_t	sendCopyBuf4[];
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Функция инициализации
  * @reval	None
  */
void Protocol_Init()
{

}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Функция создания пакета данных
  * @param  Cmd: Индекс команды
  * @param  *pData: Указатель на блок данных
  * @param  Len: Длина блока данных
  * @param  *pPackSize: Указатель на размер созданного пакета данных
  * @reval	указатель на созданный пакет данных
  */
uint8_t *Protocol_CreatePacket(uint8_t	Cmd,uint8_t *pData,uint8_t Len,uint8_t *pPackSize)
{
	uint8_t	bufferSize = (uint8_t)(5 + Len);
	uint8_t	*pBuf = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * bufferSize);

	uint16_t	ks = 0,i = 0;

	if(pBuf)
	{
		pBuf[0] = 0xAA;
		pBuf[1] = bufferSize;
		pBuf[2] = Cmd;
		memcpy((pBuf + 3),pData,Len);

			for(i = 0;i<bufferSize - 2;i++)
				ks+=pBuf[i];

		memcpy((pBuf + (bufferSize - 2)),&ks,2);

		*pPackSize = bufferSize;
	}
	return pBuf;
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Функция получения пакета данных из потока
  * @param  *pDataBuf: Указатель на приемный буфер
  * @param  sDataBuf: Размер приемного буфера
  * @param  *pDataBufIndex: Указатель на текущий индекс приемного буфера
  * @param  *packSize: Размер полученного пакета данных
  * @reval	указатель на полученный пакет данных
  */
uint8_t	*Protocol_GetPacketFromStream(uint8_t *pDataBuf,int sDataBuf, int	*pDataBufIndex, uint8_t	InputByte, int *packSize)
{
	int index;
	int i,ii;
	int ks = 0,pKS = 0;
	uint16_t len;

	uint8_t *pD;
	int iD = 0;

	if (sDataBuf<=0)  return 0;
		index = *pDataBufIndex;

	 // удаляем все до байта синхронизации
  if ((index>0) && (pDataBuf[0]!=0xAA)) {
    ii=0;
    while ((ii<index) && (pDataBuf[ii]!=0xAA))  ii++;
    memcpy(&pDataBuf[0],&pDataBuf[ii],index-ii);
    index=index-ii;
  }
	//проверяем переполнение
	if(index >= sDataBuf)
	{
		 memcpy(&pDataBuf[0],&pDataBuf[1],sDataBuf-1);
			index=sDataBuf-1;

		 if ((index>0) && (pDataBuf[0]!=0xAA)) {
      ii=0;
      while ((ii<index) && (pDataBuf[ii]!=0xAA))  ii++;
      memcpy(&pDataBuf[0],&pDataBuf[ii],index-ii);
      index=index-ii;
    }
	}




	pDataBuf[index]=InputByte;
		index++;
			*pDataBufIndex=index;

	pD=(uint8_t* )pDataBuf;
	iD=index;

	while (1) {
		 if (iD<5)  break;

			if(pD[0] == (uint8_t)0xAA){
				len = 	(uint8_t)pD[1];

				if(iD>=len)
				{
					if(len>0){
					//pKS = (pD[len-2] << 8) | pD[len-1];
					pKS = (pD[len-1] << 8) | pD[len-2];

					for(i = 0;i<len-2;i++){
						ks+=pD[i];	//посчитали КС
						}

						if(pKS == ks)
						{
							if(packSize)	*packSize = (len - 5);

								return (pD);
						}
					}
				}

			}
			pD++;
			iD--;
	}
	return 0;

}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Функция создания пакета данных линейки
  * @param  linearNumber: Номер линейки
  * @param  packId: Индекс пакета линейки
  * @param  *pData: Указатель на данные линейки
  * @param  Len: Размер пакета данных линейки
  * @reval	None
  */
void Protocol_SendLinearSensorDataPack(uint8_t	linearNumber,uint8_t dataType,uint8_t packId,uint8_t	*pData,uint8_t	Len)
{
	uint8_t		packSize = 0;
	uint8_t		*pModPackage = pvPortMalloc(Len + 2);

	if(pModPackage)
	{
		pModPackage[0] = dataType;
		pModPackage[1] = packId;
		memcpy(pModPackage + 2,pData,sizeof(uint8_t)*Len);

		uint8_t		*pPackage = Protocol_CreatePacket(linearNumber,pModPackage,Len + 2,&packSize);
		if(pPackage){
			vPortFree(pModPackage);
			BSP_Usb_SendPackage(pPackage,packSize);
			}
	}
}
/*----------------------------------------------------------------------------------------------------*/
/**
  * @brief	Функция создания последовательности пакетов данных линейки
  * @param  linearNumber: Номер линейки
  * @param  *pData: Указатель на данные линейки
  * @reval	None
  */
void Protocol_SendLinearSensorData(uint8_t	linearNumber,uint8_t	dataType,uint8_t	*pData)
{
	Protocol_SendLinearSensorDataPack(linearNumber,dataType,0x01,(uint8_t*)pData,128);
	Protocol_SendLinearSensorDataPack(linearNumber,dataType,0x02,(uint8_t*)pData+128,128);
	Protocol_SendLinearSensorDataPack(linearNumber,dataType,0x03,(uint8_t*)pData+256,128);
	Protocol_SendLinearSensorDataPack(linearNumber,dataType,0x04,(uint8_t*)pData+384,128);
}

void Protocol_SendPingResponse()
{
	uint8_t		packSize = 0;
	uint8_t		*pPackage = Protocol_CreatePacket(LIN_DATA_PING_RESPONSE,0,0,&packSize);

	if(pPackage){
				BSP_Usb_SendPackage(pPackage,packSize);
	}
}
/*----------------------------------------------------------------------------------------------------*/
/**
	* @brief	Фукнция анализа принятого пакета
	* @param	pPackage: указатель на принятый пакет данных
	* @reval 	None
	*/
void Protocol_RxPackageAnalysis(uint8_t	*pPackage)
{
	uint8_t	Code = pPackage[2];
	uint8_t	dataType = 0;

	uint16_t	*pLinear1 = sendCopyBuf1;
	uint16_t	*pLinear2 = sendCopyBuf2;
	uint16_t	*pLinear3 = sendCopyBuf3;
	uint16_t	*pLinear4 = sendCopyBuf4;

	switch(Code)
	{
		case PWM_DATA_VALUE_CMD:
		{
			uint8_t		PWMIndex = pPackage[3];
			uint16_t	PWMValue = (uint16_t)(pPackage[5] << 8 | pPackage[4]);

 			switch(PWMIndex)
			{
				case 0x01:	TIM2->CCR1 = PWMValue; 	break;
				case 0x02:	TIM2->CCR2 = PWMValue; 	break;
				case 0x03:	TIM2->CCR3 = PWMValue;	break;
				case 0x04:	TIM2->CCR4 = PWMValue;	break;
			}

		}break;

		case LIN_DATA_PING_REQUEST:
		{
			Protocol_SendPingResponse();
		}break;

		case LIN_DATA_SENSOR1_REQUEST:
		{
			dataType = pPackage[3];
			//Protocol_SendLinearSensorData(LIN_DATA_SENSOR1_RESPONSE,dataType,(uint8_t*)pLinear1);
		}break;

		case LIN_DATA_SENSOR2_REQUEST:
		{
			dataType = pPackage[3];
			//Protocol_SendLinearSensorData(LIN_DATA_SENSOR2_RESPONSE,dataType,(uint8_t*)pLinear2);
		}break;

		case LIN_DATA_SENSOR3_REQUEST:
		{
			dataType = pPackage[3];
			//Protocol_SendLinearSensorData(LIN_DATA_SENSOR3_RESPONSE,dataType,(uint8_t*)pLinear3);
		}break;

		case LIN_DATA_SENSOR4_REQUEST:
		{
			dataType = pPackage[3];
			//Protocol_SendLinearSensorData(LIN_DATA_SENSOR4_RESPONSE,dataType,(uint8_t*)pLinear4);
		}break;
/*
		case LIN_DATA_SENSOR5_REQUEST:
		{
			dataType = pPackage[3];
			Protocol_SendLinearSensorData(LIN_DATA_SENSOR5_RESPONSE,(uint8_t*)pLinear1);
		}break;*/
	}
}
/*----------------------------------------------------------------------------------------------------*/



