#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stdio.h"
#include "string.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "hd44780/hd44780.h"
#include "clock/clock.h"
#include "common/common_funcs.h"
#include "USART/usart.h"
#include "globs.h"

static void SetupHardware( void );
static void prvLedBlink1( void *pvParameters );
static void prvLcdShow( void *pvParameters );
static void prvShowTechInfo( void *pvParameters );
static void prvInitall( void *pvParameters );
static void prvUsartHandler(void *pvParameters);
static void prvUsart2Transmitter (void *pvParameters);
void USART2QueueSendString(uint8_t *data);
void vApplicationTickHook( void );

int main(void)
{
    xTaskCreate(prvInitall,(signed char*)"Initall",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

    while(1);
}

void SetupHardware()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void prvLedBlink1( void *pvParameters )
{
	while(1){
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
		vTaskDelay(1000);
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		vTaskDelay(1000);
	}
 }

void prvLcdShow( void *pvParameters )
{
    unsigned char symb;
    portBASE_TYPE xStatus;

    unsigned char buffer_cnt = 0;

    while(1){
    	xStatus = xQueueReceive(xQueueLCD, &symb, portMAX_DELAY);
    	if (xStatus == pdPASS) {
        	if (buffer_cnt == 32){
        		lcd_clrscr();
        		buffer_cnt = 0;
        	}
        	if(buffer_cnt == 16){
        		lcd_goto(2,0);
        	}
    		lcd_putc(symb);
    	}
    	buffer_cnt++;
    }
}

void prvShowTechInfo( void *pvParameters ){
	unsigned char symb[32];
	portBASE_TYPE xStatus;

	strncpy(symb, "Free heap: ", 11);
	itoa(xPortGetFreeHeapSize(), 10, symb+11);
	put_to_lcd_queue(symb);

	vTaskDelete(NULL);
}


void prvInitall( void *pvParameters )
{
	//InitRCC();
	//init_bad_clock_inter();
    SetupHardware();
	USART1Init(9600, 108000000);
	USART1InterrInit();
	USART2Init(9600, 108000000);
	//USART2InterrInit();
	lcd_init();

	xQueueUsart2Tx = xQueueCreate(USART2_TX_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueUsart2Tx != NULL) {
	}
	xTaskCreate(prvUsart2Transmitter,(signed char*)"USART2_transmitter",configMINIMAL_STACK_SIZE,
	        	NULL, tskIDLE_PRIORITY + 1, NULL);

	xQueueLCD = xQueueCreate(LCD_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueLCD != NULL) {
	}
	xQueueUsart1Rx = xQueueCreate(USART1_RX_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueUsart1Rx != NULL) {
	}

	xTaskCreate(prvUsartHandler,(signed char*)"USARThandler",configMINIMAL_STACK_SIZE,
	        	NULL, tskIDLE_PRIORITY + 1, NULL);


	xTaskCreate(prvLedBlink1,(signed char*)"LED1",configMINIMAL_STACK_SIZE,
	            NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(prvLcdShow,(signed char*)"LcdShow",configMINIMAL_STACK_SIZE,
            	NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(prvShowTechInfo,(signed char*)"TechInfo",configMINIMAL_STACK_SIZE,
            	NULL, tskIDLE_PRIORITY + 1, NULL);


	//USART2QueueSendString("Test USART 2");
    USART2WriteByte('a');
    vTaskDelete(NULL);
}


static void prvUsartHandler(void *pvParameters) {
	portBASE_TYPE xStatus;
	uint8_t a;
	for (;;) {
		xStatus = xQueueReceive(xQueueUsart1Rx, &a, portMAX_DELAY);
		if (xStatus == pdPASS){
			xQueueSendToBack(xQueueLCD, &a, 100);
		}
	}
}

void USART2QueueSendString(uint8_t *data){
	portBASE_TYPE xStatus;

	while(*data){
		xStatus = xQueueSend(xQueueUsart2Tx, data, 100);
		if (xStatus == pdPASS){
			data++;
		}
	}
}

static void prvUsart2Transmitter(void *pvParameters) {
	portBASE_TYPE xStatus;
	uint8_t a;
	for (;;) {
		xStatus = xQueueReceive(xQueueUsart2Tx, &a, portMAX_DELAY);
		if (xStatus == pdPASS){
			USART2WriteByte(a);
		}
	}
}

void vApplicationTickHook( void )
{
	unsigned char a=0;
}
