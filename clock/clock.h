#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

uint32_t InitRCC(void);
void init_bad_clock_inter(void);

void NMI_Handler(void);
