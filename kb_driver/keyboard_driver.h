#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H
#include "stm32f10x.h"
#include "globs.h"

typedef struct {
	uint8_t button;
	uint8_t duration;
} button;

enum{
	BTN_NOT_EXISTS,
	BTN_EXISTS
};

enum{
	SHORT_PRESS,
	LONG_PRESS
};


#define KB_PORT GPIOA

#define cols_num 4 //Строки
#define rows_num 3 //Колонки
#define buttons_em cols_num*rows_num //Общее кол-во кнопок
#define trashhold1 10 //Нижний уровень стробирования
#define trashhold2 60 //Верхний уровень стробирования

#define cols {GPIO_ODR_ODR4,GPIO_ODR_ODR5,GPIO_ODR_ODR6,GPIO_ODR_ODR7}; //Пины строк
#define rows {GPIO_IDR_IDR8,GPIO_IDR_IDR11,GPIO_IDR_IDR12}; //Пины колонок

void init_keyboard(void);
void kb_strobe(void);
void add_button(uint8_t col, uint8_t row, uint8_t duration);
button* get_btn(void);
uint8_t get_btn_simple(void);
void prvCheckButtons( void *pvParameters );
uint8_t button_exists(void);

#endif
