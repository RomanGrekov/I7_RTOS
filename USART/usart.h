#ifndef USART_H
#define USART_H

#include "stm32f10x.h"
#include "globs.h"

void USART1Init(uint16_t boudrate, uint32_t f_cpu);
void USART2Init(uint16_t boudrate, uint32_t f_cpu);
void USART1WriteByte(uint8_t data);
void USART2WriteByte(uint8_t data);
void USART1InterrInit(void);
void USART2InterrInit(void);
uint8_t USART1ReadByte(void);
uint8_t USART2ReadByte(void);
void USART1SendString(uint8_t *data);
void USART2SendString(uint8_t *data);


#endif
