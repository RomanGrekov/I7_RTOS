#include "add_phone.h"
#include "kb_driver/keyboard_driver.h"
#include "text_editor/text_editor.h"
#include "hd44780/hd44780.h"

void add_phone(void){
	button *btn;
	uint8_t btn_val;

	keyboard my_kb =
			{CURSOR_BLINK,
			12,
			3,
			{{'0', 0 , 0 , 0 , 0 ,' '},
			{'1','_', 0 , 0 , 0 , 0 },
			{'2','a','b','c', 0 , 0 },
			{'3','d','e','f', 0 , 0 },
			{'4','g','h','i', 0 , 0 },
			{'5','j','k','l', 0 , 0 },
			{'6','m','n','o', 0 , 0 },
			{'7','p','q','r','s', 0 },
			{'8','t','u','v', 0 , 0 },
			{'9','w','x','y','z', 0 },
			{'<', 0 , 0 , 0 , 0 ,'#'},
			{'*','+', 0 , 0 , 0 ,'^'}},
			"Type text",
			"012345678"};

	text_editor_init(&my_kb);
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
	text_editor_close();
}
