#include "clock.h"


void init_bad_clock_inter(void)
{
	RCC->CR |= RCC_CR_CSSON;
}

uint32_t InitRCC(void) 
{
	uint32_t StartUpCounter = 0, HSEStatus = 0;

	/* ÐšÐ¾Ð½Ñ„Ð¸Ð³ÑƒÑ€Ð°Ñ†Ñ�Ð¸  SYSCLK, HCLK, PCLK2 Ð¸ PCLK1 */
	/* Ð’ÐºÐ»ÑŽÑ‡Ð°ÐµÐ¼ HSE */
	  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	 /* Ð–Ð´ÐµÐ¼ Ð¿Ð¾ÐºÐ° HSE Ð½Ðµ Ð²Ñ‹Ñ�Ñ‚Ð°Ð²Ð¸Ñ‚ Ð±Ð¸Ñ‚ Ð³Ð¾Ñ‚Ð¾Ð²Ð½Ð¾Ñ�Ñ‚Ð¸ Ð»Ð¸Ð±Ð¾ Ð½Ðµ Ð²Ñ‹Ð¹Ð´ÐµÑ‚ Ñ‚Ð°Ð¹Ð¼Ð°ÑƒÑ‚*/
	  	do
	 	{
	 	HSEStatus = RCC->CR & RCC_CR_HSERDY;
	 	StartUpCounter++;
	  	}
	 	while( (HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));

	if ( (RCC->CR & RCC_CR_HSERDY) != RESET) 
	 	{
	 	HSEStatus = (uint32_t)0x01;
	  	}
	else
		{
	 	HSEStatus = (uint32_t)0x00;
	  	}

	  	/* Ð•Ñ�Ð»Ð¸ HSE Ð·Ð°Ð¿ÑƒÑ�Ñ‚Ð¸Ð»Ñ�Ñ� Ð½Ð¾Ñ€Ð¼Ð°Ð»ÑŒÐ½Ð¾ */
	if ( HSEStatus == (uint32_t)0x01)
		{
	 	/* Ð’ÐºÐ»ÑŽÑ‡Ð°ÐµÐ¼ Ð±ÑƒÑ„ÐµÑ€ Ð¿Ñ€ÐµÐ´Ð²Ñ‹Ð±Ð¾Ñ€ÐºÐ¸ FLASH */
	 	FLASH->ACR |= FLASH_ACR_PRFTBE;

	 	/* ÐšÐ¾Ð½Ñ„Ð¸Ð³ÑƒÑ€Ð¸Ñ€ÑƒÐµÐ¼ Flash Ð½Ð° 2 Ñ†Ð¸ÐºÐ»Ð° Ð¾Ð¶Ð¸Ð´Ð°Ð½Ð¸Ñ� 												*/
	 	/* Ð­Ñ‚Ð¾ Ð½ÑƒÐ¶Ð½Ð¾ Ð¿Ð¾Ñ‚Ð¾Ð¼Ñƒ, Ñ‡Ñ‚Ð¾ Flash Ð½Ðµ Ð¼Ð¾Ð¶ÐµÑ‚ Ñ€Ð°Ð±Ð¾Ñ‚Ð°Ñ‚ÑŒ Ð½Ð° Ð²Ñ‹Ñ�Ð¾ÐºÐ¾Ð¹ Ñ‡Ð°Ñ�Ñ‚Ð¾Ñ‚Ðµ 					*/
		/* ÐµÑ�Ð»Ð¸ Ñ�Ñ‚Ð¾ Ð½Ðµ Ñ�Ð´ÐµÐ»Ð°Ñ‚ÑŒ, Ñ‚Ð¾ Ð±ÑƒÐ´ÐµÑ‚ Ñ�Ñ‚Ñ€Ð°Ð½Ð½Ñ‹Ð¹ Ð³Ð»ÑŽÐº. ÐŸÑ€Ð¾Ñ† Ð¼Ð¾Ð¶ÐµÑ‚ Ð·Ð°Ð¿ÑƒÑ�Ñ‚Ð¸Ñ‚ÑŒÑ�Ñ�, Ð½Ð¾ Ñ‡ÐµÑ€ÐµÐ· Ð¿Ð°Ñ€Ñƒ 	*/
		/* Ñ�ÐµÐºÑƒÐ½Ð´ Ð¿Ð¾Ð²Ð¸Ñ�Ð°ÐµÑ‚ Ð±ÐµÐ· "Ð²Ð¸Ð´Ð¸Ð¼Ñ‹Ñ… Ð¿Ñ€Ð¸Ñ‡Ð¸Ð½". Ð’Ð¾Ñ‚ Ñ‚Ð°ÐºÐ¸Ðµ Ð²Ð¾Ñ‚ Ð½ÐµÐ¾Ñ‡ÐµÐ²Ð¸Ð´Ð½Ñ‹Ðµ Ð²Ð¸Ð»Ñ‹. 				*/
	 	//FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
	 	FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

	 	/* HCLK = SYSCLK 															*/
		/* AHB Prescaler = 1 														*/
	 	RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

	 	/* PCLK2 = HCLK 															*/
		/* APB2 Prescaler = 1 														*/
	 	RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

	 	/* PCLK1 = HCLK 															*/
		/* APB1 Prescaler = 2 														*/
	 	RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

	 	/* ÐšÐ¾Ð½Ñ„Ð¸Ð³ÑƒÑ€Ð¸Ñ€ÑƒÐµÐ¼ Ð¼Ð½Ð¾Ð¶Ð¸Ñ‚ÐµÐ»ÑŒ PLL configuration: PLLCLK = HSE * 6 = 72 MHz 	*/
	 	/* ÐŸÑ€Ð¸ ÑƒÑ�Ð»Ð¾Ð²Ð¸Ð¸, Ñ‡Ñ‚Ð¾ ÐºÐ²Ð°Ñ€Ñ† Ð½Ð° 12ÐœÐ“Ñ†! 										*/
	 	/* RCC_CFGR_PLLMULL9 - Ð¼Ð½Ð¾Ð¶Ð¸Ñ‚ÐµÐ»ÑŒ Ð½Ð° 6. Ð•Ñ�Ð»Ð¸ Ð½ÑƒÐ¶Ð½Ð° Ð´Ñ€ÑƒÐ³Ð°Ñ� Ñ‡Ð°Ñ�Ñ‚Ð¾Ñ‚Ð°, Ð½Ðµ 72ÐœÐ“Ñ† 	*/
	 	/* Ñ‚Ð¾ Ð²Ñ‹Ð±Ð¸Ñ€Ð°ÐµÐ¼ Ð´Ñ€ÑƒÐ³Ð¾Ð¹ Ð¼Ð½Ð¾Ð¶Ð¸Ñ‚ÐµÐ»ÑŒ. 											*/

		/* Ð¡Ð±Ñ€Ð°Ñ�Ñ‹Ð²Ð°ÐµÐ¼ Ð² Ð½ÑƒÐ»Ð¸ Ð¿Ñ€ÐµÐ¶Ð½ÐµÐµ Ð·Ð½Ð°Ñ‡ÐµÐ½Ð¸Ðµ*/
	 	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));

		/* Ð� Ñ‚ÐµÐ¿ÐµÑ€ÑŒ Ð½Ð°ÐºÐ°Ñ‚Ñ‹Ð²Ð°ÐµÐ¼ Ð½Ð¾Ð²Ð¾Ðµ 												*/
		/* RCC_CFGR_PLLSRC_HSE -- Ð²Ñ‹Ð±Ð¸Ñ€Ð°ÐµÐ¼ HSE Ð½Ð° Ð²Ñ…Ð¾Ð´ 								*/
		/* RCC_CFGR_PLLMULL6 -- Ð¼Ð½Ð¾Ð¶Ð¸Ñ‚ÐµÐ»ÑŒ 6											*/
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL6);

	 	/* Ð’Ñ�Ðµ Ð½Ð°Ñ�Ñ‚Ñ€Ð¾Ð¸Ð»Ð¸? Ð’ÐºÐ»ÑŽÑ‡Ð°ÐµÐ¼ PLL */
	 	RCC->CR |= RCC_CR_PLLON;

	    /* ÐžÐ¶Ð¸Ð´Ð°ÐµÐ¼, Ð¿Ð¾ÐºÐ° PLL Ð²Ñ‹Ñ�Ñ‚Ð°Ð²Ð¸Ñ‚ Ð±Ð¸Ñ‚ Ð³Ð¾Ñ‚Ð¾Ð²Ð½Ð¾Ñ�Ñ‚Ð¸ */
	 	while((RCC->CR & RCC_CR_PLLRDY) == 0)
			{
			// Ð–Ð´ÐµÐ¼
	 	 	}

	 	/* Ð Ð°Ð±Ð¾Ñ‚Ð°ÐµÑ‚? ÐœÐ¾Ð¶Ð½Ð¾ Ð¿ÐµÑ€ÐµÐºÐ»ÑŽÑ‡Ð°Ñ‚ÑŒ! Ð’Ñ‹Ð±Ð¸Ñ€Ð°ÐµÐ¼ PLL ÐºÐ°Ðº Ð¸Ñ�Ñ‚Ð¾Ñ‡Ð½Ð¸Ðº Ñ�Ð¸Ñ�Ñ‚ÐµÐ¼Ð½Ð¾Ð¹ Ñ‡Ð°Ñ�Ñ‚Ð¾Ñ‚Ñ‹ */
	 	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	 	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

	 	/* ÐžÐ¶Ð¸Ð´Ð°ÐµÐ¼, Ð¿Ð¾ÐºÐ° PLL Ð²Ñ‹Ð±ÐµÑ€ÐµÑ‚Ñ�Ñ� ÐºÐ°Ðº Ð¸Ñ�Ñ‚Ð¾Ñ‡Ð½Ð¸Ðº Ñ�Ð¸Ñ�Ñ‚ÐµÐ¼Ð½Ð¾Ð¹ Ñ‡Ð°Ñ�Ñ‚Ð¾Ñ‚Ñ‹ */
	  	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
			{
			// Ð–Ð´ÐµÐ¼
	 	 	}
		}
	else
		{
	 	/* Ð’Ñ�Ðµ Ð¿Ð»Ð¾Ñ…Ð¾... HSE Ð½Ðµ Ð·Ð°Ð²ÐµÐ»Ñ�Ñ�... Ð§ÐµÐ³Ð¾-Ñ‚Ð¾ Ñ� ÐºÐ²Ð°Ñ€Ñ†ÐµÐ¼ Ð¸Ð»Ð¸ ÐµÑ‰Ðµ Ñ‡Ñ‚Ð¾...
	 	Ð�Ð°Ð´Ð¾ Ð±Ñ‹ ÐºÐ°ÐºÑ‚Ð¾ Ð¾Ð±Ñ€Ð°Ð±Ð¾Ñ‚Ð°Ñ‚ÑŒ Ñ�Ñ‚Ñƒ Ð¾ÑˆÐ¸Ð±ÐºÑƒ... Ð•Ñ�Ð»Ð¸ Ð¼Ñ‹ Ð·Ð´ÐµÑ�ÑŒ, Ñ‚Ð¾ Ð¼Ñ‹ Ñ€Ð°Ð±Ð¾Ñ‚Ð°ÐµÐ¼
	 	Ð¾Ñ‚ HSI! */
		}

	return HSEStatus;
}
 
 
void NMI_Handler( void) 
{

RCC->CIR |= RCC_CIR_CSSC;
 
//Some code to handle situation
}
