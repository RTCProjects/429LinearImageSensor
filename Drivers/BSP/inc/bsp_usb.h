#ifndef _BSP_USB
#define _BSP_USB

#define USB_RX_BUF_SIZE			64
#define USB_TX_QUEUE_LEN		16
#define USB_RX_QUEUE_LEN		64
#define USB_MAX_TX_PACKAGE		512

#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

typedef struct
{
	uint8_t		*pData;
	uint16_t	Len;
}tUSBTxPackage;

void	BSP_Usb_Init(void);
void 	BSP_Usb_ReceiveCallback(uint8_t* pBuf, uint32_t *pLen);
void 	BSP_Usb_SendPackage(uint8_t	*pData,uint16_t	Len);
void 	BSP_Usb_SendString(char	*String);

#endif
