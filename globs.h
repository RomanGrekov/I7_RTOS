#ifndef GLOBS_H
#define GLOBS_H

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "USART/usart.h"

#define Debug 1

#define LCD_QUEUE_SIZE 32
#define USART1_RX_QUEUE_SIZE 32
#define USART1_TX_QUEUE_SIZE 32
#define USART2_TX_QUEUE_SIZE 32
#define AT_RESPONSE_QUEUE_SIZE 5
#define AT_RESPONSE_SIZE 50

xQueueHandle xQueueUsart1Rx;
xQueueHandle xQueueUsart1Tx;
xQueueHandle xQueueUsart2Tx;
xQueueHandle xQueueLCD;
xQueueHandle xQueueAtResponse;

void log(uint8_t *data);

#endif
