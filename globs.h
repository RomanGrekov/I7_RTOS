#ifndef GLOBS_H
#define GLOBS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "USART/usart.h"

enum {
	ERROR_LEVEL,
	INFO_LEVEL,
	DEBUG_LEVEL
};
#define DEBUG_LEVEL DEBUG_LEVEL

#define LCD_QUEUE_SIZE 32
#define USART1_RX_QUEUE_SIZE 32
#define USART1_TX_QUEUE_SIZE 32
#define USART2_TX_QUEUE_SIZE 32
#define AT_RESPONSE_QUEUE_SIZE 10
#define AT_RESPONSE_SIZE 50
#define MAX_DATA_SIZE_IN_RESPONSE 50
#define BUTTONS_BUF_SIZE 5

xQueueHandle xQueueUsart1Rx;
xQueueHandle xQueueUsart1Tx;
xQueueHandle xQueueUsart2Tx;
xQueueHandle xQueueLCD;
xQueueHandle xQueueAtResponse;
xQueueHandle xQueueButtons;
xSemaphoreHandle xUsart2TxMutex;
xSemaphoreHandle xLcdMutex;
xSemaphoreHandle xUsart1TxMutex;
xSemaphoreHandle xAtResponseMutex;
xSemaphoreHandle xButtonsMutex;


void log(uint8_t *data, uint8_t level);

#endif
