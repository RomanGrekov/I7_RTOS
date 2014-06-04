#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hd44780/hd44780.h"
#include "clock/clock.h"

static void prvSetupHardware( void );
static void prvLedBlink1( void *pvParameters );
static void prvLcdShow( void *pvParameters );
static void prvPutSymb( void *pvParameters );
static void prvInitall( void *pvParameters );
void vApplicationTickHook( void );

xQueueHandle xQueue;

int main(void)
{
	InitRCC();
	init_bad_clock_inter();
    prvSetupHardware();

    xQueue = xQueueCreate(16, sizeof(unsigned char));
    if (xQueue != NULL) {

    }

    xTaskCreate(prvInitall,(signed char*)"Initall",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(prvLedBlink1,(signed char*)"LED1",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);


    /* Start the scheduler. */
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

    while(1){
    	xStatus = xQueueReceive(xQueue, &symb, portMAX_DELAY);
    	if (xStatus == pdPASS) {
    		lcd_putc(symb);
    	}
    }
}

void prvPutSymb( void *pvParameters ){
	unsigned char symb=31;
	portBASE_TYPE xStatus;

	while(1){
		xStatus = xQueueSendToBack(xQueue, &symb, 0);
		if (xStatus != pdPASS) {

		}
		else{
			vTaskDelay(2000);
			if(symb < 127) symb++;
			else symb = 31;
		}
	}
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

