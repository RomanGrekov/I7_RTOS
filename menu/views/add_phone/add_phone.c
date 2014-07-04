#include "add_phone.h"
#include "kb_driver/keyboard_driver.h"

void add_phone(void){
	button *btn;
	uint8_t btn_val;

	do{
		btn = get_btn();
		btn_val = btn->button;
/*
		switch (btn_val){
		case '2':

			break;
		case '1':

			break;
		case '3':

			break;
		case '*':

		}
*/
	}while (btn_val != '2');
}
