#include "stm32f10x.h"

void InitUSART(uint16_t boudrate,  uint32_t f_cpu);
void send_to_usart(uint8_t data);
uint8_t read_byte(void);
void send_string(uint8_t * data);
void usart_interrupt_init(void);
