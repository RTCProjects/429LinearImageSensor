/*
 * protocol.h
 *
 *  Created on: 23 мая 2018 г.
 *      Author: O_o
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdlib.h>

#define LIN_DATA_SENSOR1_REQUEST	0x81
#define LIN_DATA_SENSOR1_RESPONSE	0x01
#define LIN_DATA_SENSOR2_REQUEST	0x82
#define LIN_DATA_SENSOR2_RESPONSE	0x02
#define LIN_DATA_SENSOR3_REQUEST	0x83
#define LIN_DATA_SENSOR3_RESPONSE	0x03
#define LIN_DATA_SENSOR4_REQUEST	0x84
#define LIN_DATA_SENSOR4_RESPONSE	0x04
#define LIN_DATA_SENSOR5_REQUEST	0x85
#define LIN_DATA_SENSOR5_RESPONSE	0x05

#define PWM_DATA_VALUE_CMD		0x80

#define LIN_DATA_PING_REQUEST	0x86
#define LIN_DATA_PING_RESPONSE	0x06

uint8_t	*Protocol_GetPacketFromStream(uint8_t *pDataBuf,int sDataBuf, int	*pDataBufIndex, uint8_t	InputByte, int *packSize);
void	Protocol_Init(void);
void 	Protocol_SendLinearSensorDataPack(uint8_t	linearNumber,uint8_t dataType,uint8_t packId,uint8_t	*pData,uint8_t	Len);
void 	Protocol_SendLinearSensorData(uint8_t	linearNumber,uint8_t dataType,uint8_t	*pData);
void 	Protocol_RxPackageAnalysis(uint8_t	*pPackage);

void 	Protocol_SendPingResponse(void);
#endif /* INC_PROTOCOL_H_ */
