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

static void prvSetupHardware( void );
static void prvLedBlink1( void *pvParameters );
static void prvLcdShow( void *pvParameters );
static void prvPutSymb( void *pvParameters );
static void prvInitall( void *pvParameters );
static void prvUsartHandler(void *pvParameters);
void vApplicationTickHook( void );

xQueueHandle xQueueLCD;
xSemaphoreHandle xBinarySemaphore;

int main(void)
{
	InitRCC();
	init_bad_clock_inter();
    prvSetupHardware();
	InitUSART(9600, 108000000);
	usart_interrupt_init();

    xQueueLCD = xQueueCreate(32, sizeof(unsigned char));
    if (xQueueLCD != NULL) {
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

void prvPutSymb( void *pvParameters ){
	unsigned char symb[32];
	unsigned char *ps;
	portBASE_TYPE xStatus;

	strncpy(symb, "Hello world!!!                 \0", 32);
	ps = &symb;
	while(*ps){
		xStatus = xQueueSendToBack(xQueueLCD, ps, 0);
		if (xStatus != pdPASS) {

		}
		else{
			ps++;
		}
	}
	vTaskDelete(NULL);
}

void prvInitall( void *pvParameters )
{
	lcd_init();

    xTaskCreate(prvLcdShow,(signed char*)"LcdShow",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(prvPutSymb,(signed char*)"PutSymb",configMINIMAL_STACK_SIZE,
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
		a = read_byte();
		xQueueSendFromISR(xQueueLCD, &a, xHigherPriorityTaskWoken);

		xHigherPriorityTaskWoken = pdFALSE;
		xStatus = xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
		if(xStatus == pdTRUE){

		}
		if( xHigherPriorityTaskWoken == pdTRUE ){
			taskYIELD();
		}
	}
}

static void prvUsartHandler(void *pvParameters) {
	portBASE_TYPE xStatus;

	for (;;) {
		GPIOB->ODR ^= GPIO_ODR_ODR1;
		vTaskDelay(1000);
		xStatus = xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		if(xStatus == pdTRUE){

		}
	}
}

