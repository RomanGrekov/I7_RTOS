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
#include "sim900/sim900.h"
#include "kb_driver/keyboard_driver.h"
#include "menu/menu.h"

static void SetupHardware( void );
static void prvLedBlink1( void *pvParameters );
static void prvShowTechInfo( void *pvParameters );
static void prvInitall( void *pvParameters );
static void prvProcessMenu(void *pvParameters);

void USART2QueueSendString(uint8_t *data);

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
	InitSim900Port();
	init_keyboard();
	InitMenu();

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

void prvShowAtResponse(void *pvParameters){
	at_template templates[]={
			{8, "+CLIP: \""}
	};
	data_in_resp result={13, {'0'}};

	while(1){
		if(find_data_in_resp(&templates[0], &result) == FOUND){
			put_to_lcd_queue(result.data);
		}
		vTaskDelay(100);
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

void prvProcessMenu(void *pvParameters){
	button *btn;
	while(1){
		if(button_exists()){
			btn = get_btn();
			ProcessMenu(btn->button, btn->duration);
		}
	}
}


void prvInitall( void *pvParameters )
{
	portBASE_TYPE xStatus;
	uint8_t results=0;


	xUsart2TxMutex = xSemaphoreCreateMutex();
	if (xUsart2TxMutex != NULL) {
		results++;
	}
	xQueueUsart2Tx = xQueueCreate(USART2_TX_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueUsart2Tx != NULL) results++;

	xStatus = xTaskCreate(prvUsart2Transmitter,(signed char*)"USART2_transmitter",configMINIMAL_STACK_SIZE,
	        	NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("\n", INFO_LEVEL);
		log("Mutex - Usart 2 AT resource created\n", INFO_LEVEL);
		log("Queue - Usart 2 TX created\n", INFO_LEVEL);
		log("Task - Usart 2 sender created\n", INFO_LEVEL);
		results++;
	}

	xLcdMutex = xSemaphoreCreateMutex();
	if (xLcdMutex != NULL) {
		log("Mutex - LCD resource created\n", INFO_LEVEL);
		results++;
	}

	xQueueLCD = xQueueCreate(LCD_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueLCD != NULL){
		log("Queue - Lcd created\n", INFO_LEVEL);
		results++;
	}

	xQueueUsart1Tx = xQueueCreate(USART1_TX_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueUsart2Tx != NULL){
		log("Queue - Usart 1 TX created\n", INFO_LEVEL);
		results++;
	}

	xUsart1TxMutex = xSemaphoreCreateMutex();
		if (xUsart1TxMutex != NULL) {
			log("Mutex - Usart 1 AT resource created\n", INFO_LEVEL);
			results++;
		}

	xStatus = xTaskCreate(prvUsart1Transmitter,(signed char*)"USART1_transmitter",configMINIMAL_STACK_SIZE,
		        NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - Usart 1 sender created\n", INFO_LEVEL);
		results++;
	}

	xQueueUsart1Rx = xQueueCreate(USART1_RX_QUEUE_SIZE, sizeof(unsigned char));
	if (xQueueUsart1Rx != NULL) {
		log("Queue - USART 1 RX created\n", INFO_LEVEL);
		results++;
	}

	xAtResponseMutex = xSemaphoreCreateMutex();
	if (xAtResponseMutex != NULL) {
		log("Mutex - At response resource created\n", INFO_LEVEL);
		results++;
	}

	xQueueAtResponse = xQueueCreate(AT_RESPONSE_QUEUE_SIZE, sizeof(at_response));
	if (xQueueAtResponse != NULL) {
		log("Queue - AT response created\n", INFO_LEVEL);
		results++;
	}

	xStatus = xTaskCreate(prvUsart_1_RX_Handler,(signed char*)"USARThandler",configMINIMAL_STACK_SIZE,
	        NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - Usart 1 receiver to Lcd created\n", INFO_LEVEL);
		results++;
	}

	xStatus = xTaskCreate(prvLedBlink1,(signed char*)"LED1",configMINIMAL_STACK_SIZE,
	        NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - LED 1 created\n", INFO_LEVEL);
		results++;
	}

	xStatus = xTaskCreate(prvLcdShow,(signed char*)"LcdShow",configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - Lcd show created\n", INFO_LEVEL);
		results++;
	}

	xStatus = xTaskCreate(prvShowTechInfo,(signed char*)"TechInfo",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - tech info created\n", INFO_LEVEL);
		results++;
	}

	xStatus = xTaskCreate(prvShowAtResponse,(signed char*)"ShowAtResponse",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - show at response created\n", INFO_LEVEL);
		results++;
	}

	xQueueButtons = xQueueCreate(BUTTONS_BUF_SIZE, sizeof(button));
	if (xQueueButtons != NULL){
		log("Queue - Buttons created\n", INFO_LEVEL);
		results++;
	}

	xButtonsMutex = xSemaphoreCreateMutex();
		if (xButtonsMutex != NULL) {
			log("Mutex - Buttons resource created\n", INFO_LEVEL);
			results++;
		}

	xStatus = xTaskCreate(prvCheckButtons,(signed char*)"CheckButtons",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - check buttons created\n", INFO_LEVEL);
		results++;
	}

	xStatus = xTaskCreate(prvProcessMenu,(signed char*)"Process Menu",configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, NULL);
	if(xStatus == pdPASS){
		log("Task - process menu created\n", INFO_LEVEL);
		results++;
	}

	if(SimInit() == MODEM_TEST_PASS){
		log("Modem - test passed\n", INFO_LEVEL);
		results++;
	}

	if(results == 21) log("Initialization successful!!!", INFO_LEVEL);

    vTaskDelete(NULL);
}

