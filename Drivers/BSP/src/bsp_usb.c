/*----------------------------------------------------------------------------------------------------*/
/**
  * @file    bsp_usb.c ������ ��� ������ � USB
  * @brief   
**/
/*----------------------------------------------------------------------------------------------------*/
#include "bsp_usb.h"
#include "cmsis_os.h"
#include "protocol.h"
#include <string.h>

osThreadId 		usbTaskTxHandle;
osThreadId 		usbTaskRxHandle;

xQueueHandle 	usbTxDataQueue;	
xQueueHandle	usbRxDataQueue;

void	BSP_Usb_TxTask(void const * argument);
void	BSP_Usb_RxTask(void const * argument);

static	uint8_t	usbRxBuf[USB_RX_BUF_SIZE];
int	UsbBufInd = 0;

/*----------------------------------------------------------------------------------------------------*/
/**
	* @brief	������������� BSP Usb
	* @param	None
	* @reval	None
	*/
void	BSP_Usb_Init()
{
	UsbBufInd= 0;

	usbTxDataQueue = xQueueCreate(USB_TX_QUEUE_LEN, sizeof(tUSBTxPackage));
	usbRxDataQueue = xQueueCreate(USB_RX_QUEUE_LEN, sizeof(uint8_t));
	
	vQueueAddToRegistry(usbTxDataQueue,"usbTxDataQueue");

	osThreadDef(usbTxTask, BSP_Usb_TxTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE + 0x100);
 	usbTaskTxHandle = osThreadCreate(osThread(usbTxTask), NULL);
	
	osThreadDef(usbRxTask, BSP_Usb_RxTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE + 0x100);
	usbTaskRxHandle = osThreadCreate(osThread(usbRxTask), NULL);
	
	MX_USB_DEVICE_Init();
}
/*----------------------------------------------------------------------------------------------------*/
/**
	* @brief	����� �������� ���� �� ������� � CDC USB
	* @param	argument: ��������� ������ FreeRTOS 
	* @reval	None
	*/
void	BSP_Usb_TxTask(void const * argument)
{
	static tUSBTxPackage	txMsg;
	
	while(1)
	{
		portBASE_TYPE xStatus;
		xStatus=xQueueReceive(usbTxDataQueue, &txMsg, portMAX_DELAY);
			if (xStatus == pdPASS){
				//int	pPackSize = 0;
				//uint8_t *pPackage = protocol_CreatePackageForOutputStream(txMsg.Event,txMsg.Status,txMsg.DataSize,txMsg.pData,&pPackSize);
				//if(pPackage)
				
					//taskENTER_CRITICAL();
					CDC_Transmit_FS(txMsg.pData,txMsg.Len);
					vPortFree(txMsg.pData);
					//taskEXIT_CRITICAL();
					osDelay(1);
					
					//vPortFree(pPackage);
				}
	}	
}
/*----------------------------------------------------------------------------------------------------*/
/**
	* @brief	����� ������ ���� �� ������� � CDC USB
	* @param	argument: ��������� ������ FreeRTOS 
	* @reval	None
	*/
void	BSP_Usb_RxTask(void const * argument)
{
	static uint8_t	receiveByte = 0;

	int 		sPackage;
	uint8_t 	*pPackage = 0;

	while(1)
	{
		portBASE_TYPE xStatus;
		xStatus=xQueueReceive(usbRxDataQueue, &receiveByte, portMAX_DELAY);

			if (xStatus == pdPASS){
				pPackage = Protocol_GetPacketFromStream(usbRxBuf,USB_RX_BUF_SIZE,&UsbBufInd,receiveByte,&sPackage);
					if(pPackage){
						UsbBufInd = 0;
						Protocol_RxPackageAnalysis(pPackage);
					}
			}
	}
}
/*----------------------------------------------------------------------------------------------------*/
/**
	* @brief	Callback ������� ��������� ����������� �� CDC USB ������ ����
	* @param	pBuf: ��������� �� ����� �������� ������
	* @param	pLen: ��������� �� ���������� �������� ���� � �����
	* @reval 	None
	*/
void BSP_Usb_ReceiveCallback(uint8_t* Buf, uint32_t *Len)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	for(int i = 0;i<*Len;i++)
	{
		uint8_t DataReceive = *(Buf + i*sizeof(int8_t));
		xQueueSendToBackFromISR(usbRxDataQueue, &DataReceive, &xHigherPriorityTaskWoken);
	}
	if (xHigherPriorityTaskWoken != pdFALSE) {
	   portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	 }
}
/*----------------------------------------------------------------------------------------------------*/
/**
	* @brief	���������� ������ ���� � ������� FreeRTOS
	* @param	pData: ��������� �� ����� ������������ ������
	* @param	Len: ������ ������������� ������
	* @reval 	None
	*/
void BSP_Usb_SendPackage(uint8_t	*pData,uint16_t	Len)
{
	static tUSBTxPackage	txMsg;
		
	txMsg.pData = pData;
	txMsg.Len = Len;
	
		xQueueSendToBack(usbTxDataQueue,&txMsg,0);
}
/*----------------------------------------------------------------------------------------------------*/
/**
	* @brief	�������� ������ � ������� �������� �� USB
	* @param	String: ��������� �� ����� ������������ ������
	* @reval 	None
	*/
void BSP_Usb_SendString(char	*String)
{
	int Len = strlen(String);
	
	if(Len > 0 && Len < USB_MAX_TX_PACKAGE)
		BSP_Usb_SendPackage((uint8_t*)String,Len);
}
/*----------------------------------------------------------------------------------------------------*/

