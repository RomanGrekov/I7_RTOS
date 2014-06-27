#include "sim900.h"

void InitSim900Port(void){
    RCC->APB2ENR |= CLOCK_REG; //Clock port

    MANAGE_PORT->SET_PWR_REG &= ~(PWR_CNF_REG); //Init PWR_KEY
    MANAGE_PORT->SET_PWR_REG |= (PWR_MODE_REG0);// max freq 10 MHz
    MANAGE_PORT->SET_PWR_REG &= ~(PWR_MODE_REG1);//

    MANAGE_PORT->SET_STATUS_REG |= STATUS_CNF_REG0;// Init STATUS. Input
    MANAGE_PORT->SET_STATUS_REG &= ~STATUS_CNF_REG1;// Init STATUS. Input
    MANAGE_PORT->SET_STATUS_REG &= ~(STATUS_MODE_REG0 | STATUS_MODE_REG1);
}

uint8_t SwitchSim900(uint8_t state, uint8_t retries){
	uint8_t r = retries;
	if (state == SWITCH_ON){
		while ((MANAGE_PORT->CHECK_REG & STATUS_STATE) == 0 && r > 0){ //while status != 1
			MANAGE_PORT->SET_REG |= PWR_KEY; //turn on pwr key
			vTaskDelay(1000/portTICK_RATE_MS);
			MANAGE_PORT->SET_REG &= ~PWR_KEY; //turn off pwr key
			vTaskDelay(2000/portTICK_RATE_MS);
			r--;
		}
		if(MANAGE_PORT->CHECK_REG & STATUS_STATE == 0) return pdFAIL;
	}
	if(state == SWITCH_OFF){
		while ((MANAGE_PORT->CHECK_REG & STATUS_STATE) != 0 && r > 0){ //while status != 0
			MANAGE_PORT->SET_REG |= PWR_KEY; //turn on pwr key
			vTaskDelay(1000/portTICK_RATE_MS);
			MANAGE_PORT->SET_REG &= ~PWR_KEY; //turn off pwr key
			vTaskDelay(2000/portTICK_RATE_MS);
			r--;
		}
		if(MANAGE_PORT->CHECK_REG & STATUS_STATE != 0) return pdFAIL;
	}
	return pdPASS;
}

uint8_t SimGetStatus(void){
	if ((MANAGE_PORT->CHECK_REG & STATUS_STATE) == 0) return STATUS_OFF;
	else return STATUS_ON;
}

uint8_t SimInit(void){
	at_template ok_t= {2, "OK"};
	at_template cfun_t = {8, "+CFUN: 1"};

	log("Start modem test...\n", DEBUG);
	log("Send string: at\n", DEBUG);
	flush_answers();
	USART1QueueSendString("at\r\n");
	vTaskDelay(500/portTICK_RATE_MS);
	if(find_answer(&ok_t) == NOT_FOUND){
		log("OK response not found!!!\n", DEBUG);
		return MODEM_TEST_FAIL;
	}
	log("OK response found!\n", DEBUG);

	log("Send string: at+cfun?\n", DEBUG);
	USART1QueueSendString("at+cfun?\r\n");
	vTaskDelay(500/portTICK_RATE_MS);
	if(find_answer(&cfun_t) == NOT_FOUND){
		log("CFUN response not found!!!\n", DEBUG);
		return MODEM_TEST_FAIL;
	}
	log("CFUN response found!\n", DEBUG);
	log("Finish modem test...\n", DEBUG);

	flush_answers();
	return MODEM_TEST_PASS;
}
