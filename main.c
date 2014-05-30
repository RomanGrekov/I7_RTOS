#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hd44780/hd44780.h"

static void prvSetupHardware( void );
static void prvLedBlink1( void *pvParameters );
static void prvLedBlink2( void *pvParameters );
void vApplicationTickHook( void );

int main(void)
{


    prvSetupHardware();

    xTaskCreate(prvLedBlink1,(signed char*)"LED1",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(prvLedBlink2,(signed char*)"LED2",configMINIMAL_STACK_SIZE,
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

void prvLedBlink2( void *pvParameters )
{
    GPIO_SetBits(GPIOB,GPIO_Pin_1);

	lcd_init();
	lcd_clrscr();
	lcd_prints("FreeRTOS!-v1.0.0\0");
    while(1);
}

void vApplicationTickHook( void )
{
	unsigned char a=0;
}

