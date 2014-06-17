#include "usart.h"

#include "FreeRTOS.h"

void USART1Init(uint16_t boudrate, uint32_t f_cpu){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Clock port A
    //============Pin 9 (TX) setup===================//
    GPIOA->CRH      &= ~(GPIO_CRH_CNF9_0); //Set pin 9 to alternative push-pull
    GPIOA->CRH      |= GPIO_CRH_CNF9_1; //
    GPIOA->CRH      |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0); // 50 MHz
    //============Pin 10 (RX) setup===================//
    GPIOA->CRH      &= ~(GPIO_CRH_CNF10_1); //Set pin 10 to input
    GPIOA->CRH      |= GPIO_CRH_CNF10_0; // without lift to positive
    GPIOA->CRH      &= ~(GPIO_CRH_MODE10_1 | GPIO_CRH_MODE10_0); // input

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable clock for USART1

    USART1->BRR = f_cpu/boudrate; //Calculate value for speed. In my case it's 0x4e2
    USART1->CR1 |= USART_CR1_UE; //Enable USART
    USART1->CR1 &= ~(USART_CR1_M); // 8 data bit
    USART1->CR1 &= ~(USART_CR1_PCE); // Parity disabled
    USART1->CR1 |=(USART_CR1_TE | USART_CR1_RE); // Enable transmit and receive

    USART1->CR2 &= ~(USART_CR2_STOP_0 | USART_CR2_STOP_1); // One stop bit
}

void USART2Init(uint16_t boudrate, uint32_t f_cpu){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Clock port A

    //============Pin 2 (TX) setup===================//
    GPIOA->CRL      &= ~(GPIO_CRL_CNF2_0); //Set pin 2 to alternative push-pull
    GPIOA->CRL      |= GPIO_CRL_CNF2_1; //
    GPIOA->CRL      |= (GPIO_CRL_MODE2_1 | GPIO_CRL_MODE2_0); // 50 MHz
    //============Pin 3 (RX) setup===================//
    GPIOA->CRL      &= ~(GPIO_CRL_CNF3_1); //Set pin 3 to input
    GPIOA->CRL      |= GPIO_CRL_CNF3_0; // without lift to positive
    GPIOA->CRL      &= ~(GPIO_CRL_MODE3_1 | GPIO_CRL_MODE3_0); // input

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enable clock for USART2

    USART2->BRR = f_cpu/boudrate; //Calculate value for speed. In my case it's 0x4e2
    USART2->CR1 |= USART_CR1_UE; //Enable USART
    USART2->CR1 &= ~(USART_CR1_M); // 8 data bit
    USART2->CR1 &= ~(USART_CR1_PCE); // Parity disabled
    USART2->CR1 |=(USART_CR1_TE | USART_CR1_RE); // Enable transmit and receive

    USART2->CR2 &= ~(USART_CR2_STOP_0 | USART_CR2_STOP_1); // One stop bit
}

void USART1WriteByte(uint8_t data) {
	while(!(USART1->SR & USART_SR_TXE)); //Waiting until bit TC in register SR not 1
	USART1->DR=data; //Send byte
}

void USART2WriteByte(uint8_t data) {
	while(!(USART2->SR & USART_SR_TXE)); //Waiting until bit TC in register SR not 1
	USART2->DR=data; //Send byte
}

void USART1InterrInit(void){
	USART1->CR1 |= USART_CR1_RXNEIE;/*!<RXNE Interrupt Enable */
	USART1->CR1 |= USART_CR1_TCIE;/*!<Transmission Complete Interrupt Enable */
	NVIC_EnableIRQ(USART1_IRQn);//Allow usart receive unterruption
	NVIC_SetPriority(USART1_IRQn, 15);
}

void USART2InterrInit(void){
	USART2->CR1 |= USART_CR1_RXNEIE;/*!<RXNE Interrupt Enable */
	USART2->CR1 |= USART_CR1_TCIE;/*!<Transmission Complete Interrupt Enable */
	NVIC_EnableIRQ(USART2_IRQn);//Allow usart receive unterruption
	NVIC_SetPriority(USART2_IRQn, 15);
}

uint8_t USART1ReadByte(void){
	if (USART1->SR & USART_SR_RXNE){
		return 	USART1->DR;
	}
	return 0;
}

uint8_t USART2ReadByte(void){
	if (USART2->SR & USART_SR_RXNE){
		return 	USART2->DR;
	}
	return 0;
}

void USART1SendString(uint8_t *data){
	while (*data){
		USART1WriteByte(*data);
		data++;
	}
}

void USART2SendString(uint8_t *data){
	while (*data){
		USART2WriteByte(*data);
		data++;
	}
}

void USART1_IRQHandler(void){
	static portBASE_TYPE xHigherPriorityTaskWoken, xStatus;
	uint8_t a;
	if(USART1->SR & USART_SR_RXNE){
		xHigherPriorityTaskWoken = pdFALSE;
		a = USART1ReadByte();
		xStatus = xQueueSendFromISR(xQueueUsart1Rx, &a, xHigherPriorityTaskWoken);
		if(xStatus == pdTRUE){
			//GPIOB->ODR ^= GPIO_ODR_ODR1;
		}
		if( xHigherPriorityTaskWoken == pdTRUE ){
			taskYIELD();
		}
	}
	if((USART1->SR & USART_SR_TC) != 0){//!<Transmission Complete
		USART1->SR &= ~USART_SR_TC; //Clear flag --^
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

void prvUsartHandler(void *pvParameters) {
	portBASE_TYPE xStatus;
	uint8_t a;
	for (;;) {
		xStatus = xQueuePeek(xQueueUsart1Rx, &a, portMAX_DELAY);
		if (xStatus == pdPASS){
			xQueueSendToBack(xQueueUsart2Tx, &a, 100);
		}
		xStatus = xQueueReceive(xQueueUsart1Rx, &a, portMAX_DELAY);
		if (xStatus == pdPASS){
			xQueueSendToBack(xQueueLCD, &a, 100);
		}
	}
}

void prvUsart2Transmitter(void *pvParameters) {
	portBASE_TYPE xStatus;
	uint8_t a;
	for (;;) {
		xStatus = xQueueReceive(xQueueUsart2Tx, &a, portMAX_DELAY);
		if (xStatus == pdPASS){
			USART2WriteByte(a);
		}
	}
}

