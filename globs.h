#ifndef GLOBS_H
#define GLOBS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "USART/usart.h"

enum {
	EMERGANCY,
	INFO,
	DEBUG
};
#define DEBUG_LEVEL DEBUG

#define LCD_QUEUE_SIZE 32
#define USART1_RX_QUEUE_SIZE 32
#define USART1_TX_QUEUE_SIZE 32
#define USART2_TX_QUEUE_SIZE 32
#define AT_RESPONSE_QUEUE_SIZE 5
#define AT_RESPONSE_SIZE 50
#define MAX_DATA_SIZE_IN_RESPONSE 50

xQueueHandle xQueueUsart1Rx;
xQueueHandle xQueueUsart1Tx;
xQueueHandle xQueueUsart2Tx;
xQueueHandle xQueueLCD;
xQueueHandle xQueueAtResponse;

void log(uint8_t *data, uint8_t level);

#endif
