#include "at_parser.h"

unsigned char USARTCheckData(unsigned char symb, at_response * res){
	enum{
		clear = 0,
		d_found,
		d_a_found,
		symb_found,
		sec_d_found,
		sec_d_a_found
	};
	static uint8_t state = clear, i=0;

	switch (symb){
		case 0:
			break;
		case 0x0d:
			if(state == clear) state = d_found;
			if(state == symb_found) state = sec_d_found;
			break;
		case 0x0a:
			if(state == d_found) state = d_a_found;
			if(state == sec_d_found)
				{
					res->response[i] = 0;
					res->size = i;
					state = clear;
					i = 0;
					return FOUND;
				}
			if(state != d_a_found && state != sec_d_a_found) state = clear;
			break;
		default:
			if (state == d_a_found) state = symb_found;
			if (state == symb_found){
				res->response[i] = symb;
				if(i < AT_RESPONSE_SIZE-1)i++;
			}
			if(state != symb_found) state = clear;
			break;
	}
	return NOT_FOUND;
}

found_template find_template(uint8_t *resp, uint8_t resp_size, uint8_t *template, uint8_t templ_size)
{
	uint8_t shift=0, cnt=0, found=0, i;
	found_template t_found;

	t_found.found = NOT_FOUND;
	while((resp_size - shift) >= templ_size && templ_size <= resp_size){
		found = 1;
		for (i=0; i < templ_size; i++)
			if (resp[i+shift] != template[i]){
				found = 0;
				break;
			}
		if(found == 1){
			t_found.found = FOUND;
			t_found.shift = shift;
			return t_found;
		}
		shift++;
	}
	return t_found;// not found
}

found_template find_template_in_response(at_response *response, at_template *template){
	return find_template(&response->response, response->size, template->template, template->size);
}

uint8_t get_data_from_response(at_response *response, at_template *template, data_in_resp *data){
	found_template t_result;

	t_result = find_template_in_response(response, template);
	if (t_result.found == FOUND){
		strncpy(data->data, response->response+t_result.shift+template->size, data->size);
		data->data[data->size]=0;
		return FOUND;
	}

	return NOT_FOUND;
}

uint8_t find_data_in_resp(at_template *template, data_in_resp *data){
	unsigned portBASE_TYPE elements_in_queue = uxQueueMessagesWaiting(xQueueAtResponse);
	unsigned portBASE_TYPE element=0;
	portBASE_TYPE xStatus=pdPASS;
	at_response response;
	found_template t_result;

	if(xSemaphoreTake(xAtResponseMutex, 100/portTICK_RATE_MS) == pdTRUE){
		while(xStatus == pdPASS && element < elements_in_queue){
			xStatus = xQueueReceive(xQueueAtResponse, &response, 100/portTICK_RATE_MS);
			if (xStatus == pdPASS){
				if(get_data_from_response(&response, template, data) == FOUND)return FOUND;
				else{
					xQueueSend(xQueueAtResponse, &response, 0);
					element++;
				}
			}
		}
		xSemaphoreGive(xAtResponseMutex);
	}
	return NOT_FOUND;
}

uint8_t find_answer(at_template *template){
	unsigned portBASE_TYPE elements_in_queue = uxQueueMessagesWaiting(xQueueAtResponse);
	unsigned portBASE_TYPE element=0;
	portBASE_TYPE xStatus=pdPASS;
	at_response response;
	found_template t_result;

	if(xSemaphoreTake(xAtResponseMutex, 100/portTICK_RATE_MS) == pdTRUE){
		while(xStatus == pdPASS && element < elements_in_queue){
			xStatus = xQueueReceive(xQueueAtResponse, &response, 0);
			if (xStatus == pdPASS){
				t_result = find_template_in_response(&response, template);
				if (t_result.found == FOUND){
					xSemaphoreGive(xAtResponseMutex);
					return FOUND;
				}
				else{
					xQueueSend(xQueueAtResponse, &response, 0);
					element++;
				}
			}
		}
		xSemaphoreGive(xAtResponseMutex);
	}
	return NOT_FOUND;
}
