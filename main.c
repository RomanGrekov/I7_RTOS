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

#define LCD_QUEUE_SIZE 32

static void prvSetupHardware( void );
static void prvLedBlink1( void *pvParameters );
static void prvLcdShow( void *pvParameters );
static void prvShowTechInfo( void *pvParameters );
static void prvInitall( void *pvParameters );
static void prvUsartHandler(void *pvParameters);
void vApplicationTickHook( void );

portBASE_TYPE put_to_lcd_queue(uint8_t *p);

xQueueHandle xQueueLCD;
xQueueHandle xQueueUsartRx;
xSemaphoreHandle xBinarySemaphore;

int main(void)
{
	InitRCC();
	init_bad_clock_inter();
    prvSetupHardware();
	InitUSART(9600, 108000000);
	usart_interrupt_init();

    xQueueLCD = xQueueCreate(LCD_QUEUE_SIZE, sizeof(unsigned char));
    if (xQueueLCD != NULL) {
    }
    xQueueUsartRx = xQueueCreate(32, sizeof(unsigned char));
    if (xQueueUsartRx != NULL) {
    }

    vSemaphoreCreateBinary(xBinarySemaphore);
    if (xBinarySemaphore != NULL) {

        xTaskCreate(prvUsartHandler,(signed char*)"USARThandler",configMINIMAL_STACK_SIZE,
        		NULL, tskIDLE_PRIORITY + 2, NULL);
    }

    xTaskCreate(prvInitall,(signed char*)"Initall",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(prvLedBlink1,(signed char*)"LED1",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);


    vTaskStartScheduler();

    while(1);
}

void prvSetupHardware()
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

portBASE_TYPE put_to_lcd_queue(uint8_t *p){
	portBASE_TYPE xStatus;
	uint8_t i=0, a=' ';

	while(*p){
		xStatus = xQueueSendToBack(xQueueLCD, p, 10);
		p++;
		if (xStatus == pdPASS) {
			i++;
		}
		else return xStatus;
	}
	while(i < LCD_QUEUE_SIZE){
		xStatus = xQueueSendToBack(xQueueLCD, &a, 10);
		if (xStatus == pdPASS) {
			i++;
		}
		else return xStatus;
	}
	return pdPASS;
}

void prvInitall( void *pvParameters )
{
	uint8_t s[7];

	lcd_init();

    xTaskCreate(prvLcdShow,(signed char*)"LcdShow",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(prvShowTechInfo,(signed char*)"TechInfo",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskDelete(NULL);
}

void vApplicationTickHook( void )
{
	unsigned char a=0;
}

void USART1_IRQHandler(void){
	static portBASE_TYPE xHigherPriorityTaskWoken, xStatus;
	uint8_t a;
	if(USART1->SR & USART_SR_RXNE){
		xHigherPriorityTaskWoken = pdFALSE;
		a = read_byte();
		xQueueSendFromISR(xQueueUsartRx, &a, xHigherPriorityTaskWoken);
		if(xStatus == pdTRUE){
			GPIOB->ODR ^= GPIO_ODR_ODR1;
		}
		if( xHigherPriorityTaskWoken == pdTRUE ){
			taskYIELD();
		}
	}
}

static void prvUsartHandler(void *pvParameters) {
	portBASE_TYPE xStatus;
	uint8_t a;
	for (;;) {
		xStatus = xQueueReceive(xQueueUsartRx, &a, portMAX_DELAY);
		if (xStatus == pdPASS){
			xQueueSendToBack(xQueueLCD, &a, 100);
		}
	}
}

