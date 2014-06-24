#include "keyboard_driver.h"

const uint8_t templates[] = "123456789*0#"; //Знаки на клавиатуре

void init_keyboard(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Clock port
	GPIOA->CRH 	&= ~(GPIO_CRH_MODE8_0 | GPIO_CRH_MODE11_0 | GPIO_CRH_MODE12_0);//Ножки 8,11,12 - входы,
	GPIOA->CRH 	&= ~(GPIO_CRH_MODE8_1 | GPIO_CRH_MODE11_1 | GPIO_CRH_MODE12_1);//
	GPIOA->CRH	&= ~(GPIO_CRH_CNF8_0  | GPIO_CRH_CNF11_0  | GPIO_CRH_CNF12_0); //подтяжка к +
	GPIOA->CRH |=   (GPIO_CRH_CNF8_1  | GPIO_CRH_CNF11_1  | GPIO_CRH_CNF12_1); //
	GPIOA->ODR |=   (GPIO_ODR_ODR8    | GPIO_ODR_ODR11    | GPIO_ODR_ODR12);   //подтяжка к +
	
	GPIOA->CRL 	|=  (GPIO_CRL_MODE4_0 | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE7_0);// 50MHz
	GPIOA->CRL 	|=  (GPIO_CRL_MODE4_1 | GPIO_CRL_MODE5_1 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1);//
	GPIOA->CRL	&= ~(GPIO_CRL_CNF4_0  | GPIO_CRL_CNF5_0  | GPIO_CRL_CNF6_0  | GPIO_CRL_CNF7_0);// Ножки 3,4,5,6 - выходы
	GPIOA->CRL	&= ~(GPIO_CRL_CNF4_1  | GPIO_CRL_CNF5_1  | GPIO_CRL_CNF6_1  | GPIO_CRL_CNF7_1);//
}

void kb_strobe(void)
{
	uint16_t cols_[] = cols;
	uint16_t rows_[] = rows;
	uint8_t col=0;
	uint8_t i=0;
	uint8_t row=0;
	static uint8_t check_buttons[cols_num][rows_num];

	for (col=0; col<cols_num; col++)//Перебираем все строки
	{
		for (i=0; i<cols_num; i++)//Формируем маску
		{
			if (i == col){KB_PORT->ODR &= ~cols_[i];}
			else{ KB_PORT->ODR |= cols_[i];}
		}

		for (row=0; row<rows_num; row++)//Опрашиваем построчно
		{
			if ((KB_PORT->IDR & rows_[row]) == 0)
			{
				check_buttons[col][row]++;
			}
			else{
				if (check_buttons[col][row] >= trashhold1 &&
						check_buttons[col][row] <= trashhold2){
					add_button(col, row, '0');//0 means short press
				}
				if(check_buttons[col][row] > trashhold2){
					add_button(col, row, '1');//1 means long press
				}

				check_buttons[col][row]=0;
			}
		}
	}
}

void add_button(uint8_t col, uint8_t row, uint8_t duration){
	button btn;

	if(xSemaphoreTake(xButtonsMutex, portMAX_DELAY) == pdTRUE){
		btn.button = templates[col*rows_num+row];
		btn.duration = duration;
		xQueueSend(xQueueButtons, &btn, portMAX_DELAY);
		xSemaphoreGive(xButtonsMutex);
	}
}

uint8_t button_exists(void){
	if(uxQueueMessagesWaiting(xQueueButtons) != 0){
		return BTN_EXISTS;
	}
	return BTN_NOT_EXISTS;
}

button* get_btn(void){
	button btn;
	btn.button = 0;
	btn.duration = 0;

	if(uxQueueMessagesWaiting(xQueueButtons) != 0){
		xQueueReceive(xQueueButtons, &btn, 100/portTICK_RATE_MS);
	}

	return &btn;
}

uint8_t get_btn_simple(void){
	return get_btn()->button;
}

void prvCheckButtons( void *pvParameters ){
	while(1){
		kb_strobe();
	}
}

