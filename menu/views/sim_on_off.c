#include "sim_on_off.h"
#include "sim900/sim900.h"
#include "kb_driver/keyboard_driver.h"

void turn_on_off(void){
	button *btn;
	uint8_t status, retcode, btn_val;

	status = SimGetStatus();
	show_status(status);

	do{
		if (status != SimGetStatus()){
			show_status(SimGetStatus());
		}

		btn = get_btn();
		btn_val = btn->button;

		switch (btn_val){
		case '2':
			changeMenu(MENU_THIS);
			break;
		case '1':
			SwitchSim900(SWITCH_ON, 5);
			break;
		case '3':
			SwitchSim900(SWITCH_OFF, 5);
			break;
		case '*':
			retcode = SimInit();
			if(retcode == MODEM_TEST_PASS)  put_to_lcd_queue("Modem test pass. 1-ON, 2-OFF, 3-Status");
			if(retcode == MODEM_TEST_FAIL)  put_to_lcd_queue("Modem test fail. 1-ON, 2-OFF, 3-Status");
		}

	}while (btn != '2');
}

void show_status(uint8_t status){
	if(status == STATUS_ON)	put_to_lcd_queue("Modem is ON. 1-ON, 2-OFF, 3-Status");
	if(status == STATUS_OFF)	put_to_lcd_queue("Modem is OFF. 1-ON, 2-OFF, 3-Status");
}
