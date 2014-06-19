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
#include "at_parser/at_parser.h"

static void SetupHardware( void );
static void prvLedBlink1( void *pvParameters );
static void prvLcdShow( void *pvParameters );
static void prvShowTechInfo( void *pvParameters );
static void prvInitall( void *pvParameters );

void USART2QueueSendString(uint8_t *data);

void vApplicationTickHook( void );

int main(void)
{
	InitRCC();
	init_bad_clock_inter();
    SetupHardware();
	USART1Init(9600, configCPU_CLOCK_HZ);
	USART1InterrInit();
	USART2Init(9600, configCPU_CLOCK_HZ/2);
	//USART2InterrInit();
	lcd_init();

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

void prvShowAtResponse(void *pvParameters){
	portBASE_TYPE xStatus;
	at_response response;
	found_template t_result;
	unsigned char tel_number[13];

	at_template templates[]={
			{8, "+CLIP: \""}
	};
	data_in_resp result={13, {'0'}};

	while(1){
		xStatus = xQueueReceive(xQueueAtResponse, &response, portMAX_DELAY);
		if (xStatus == pdPASS){
			/*
			t_result = find_template_in_response(&response, &templates[0]);
			if (t_result.found == FOUND){
				strncpy(tel_number, response.response+8, 13);
				put_to_lcd_queue(&tel_number);
			}
			*/
			if(get_data_from_response(&response, &templates[0], &result) == FOUND){
				put_to_lcd_queue(result.data);
			}
		}
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
	portBASE_TYPE xStatus;
	uint8_t results=0;

	xQueueUsart2Tx = xQueueCreate(USART2_TX_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueUsart2Tx != NULL) results++;

	xStatus = xTaskCreate(prvUsart2Transmitter,(signed char*)"USART2_transmitter",configMINIMAL_STACK_SIZE,
	        	NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("\n");
		log("Queue - Usart 2 TX created\n");
		log("Task - Usart 2 sender created\n");
		results++;
	}

	xQueueLCD = xQueueCreate(LCD_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueLCD != NULL){
		log("Queue - Lcd created\n");
		results++;
	}

	xQueueUsart1Rx = xQueueCreate(USART1_RX_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueUsart1Rx != NULL) {
		log("Queue - USART 1 RX created\n");
		results++;
	}

	xQueueAtResponse = xQueueCreate(AT_RESPONSE_QUEUE_SIZE, sizeof(at_response));
	if (xQueueAtResponse != NULL) {
		log("Queue - AT response created\n");
		results++;
	}

	xStatus = xTaskCreate(prvUsart_1_RX_Handler,(signed char*)"USARThandler",configMINIMAL_STACK_SIZE,
	        NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - Usart 1 receiver to Lcd created\n");
		results++;
	}

	xStatus = xTaskCreate(prvLedBlink1,(signed char*)"LED1",configMINIMAL_STACK_SIZE,
	        NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - LED 1 created\n");
		results++;
	}

	xStatus = xTaskCreate(prvLcdShow,(signed char*)"LcdShow",configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - Lcd show created\n");
		results++;
	}

	xStatus = xTaskCreate(prvShowTechInfo,(signed char*)"TechInfo",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - tech info created\n");
		results++;
	}

	xStatus = xTaskCreate(prvShowAtResponse,(signed char*)"ShowAtResponse",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - show at response created\n");
		results++;
	}

	if(results == 10) log("Initialization successful!!!");
    vTaskDelete(NULL);
}


void vApplicationTickHook( void )
{
	unsigned char a=0;
}
