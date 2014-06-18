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
/*
struct found_template find_template(uint8_t *resp, uint8_t *template)
{
	uint8_t t_size, r_size;
	uint8_t shift=0, cnt=0, found=0;
	struct found_template t_found;
	r_size = getSize(resp);
	t_size = getSize(template);

	t_found.found = 0;
	while((r_size - shift) >= t_size && t_size <= r_size){
		found = 1;
		for (uint32_t i=0; i < t_size; i++)
			if (resp[i+shift] != template[i]){
				found = 0;
				break;
			}
		if(found == 1){
			t_found.found = 1;
			t_found.shift = shift;
			return t_found;
		}
		shift++;
	}
	return t_found;// not found
}

uint8_t getSize(uint8_t *my_array){
	uint8_t i=0;
	while(*my_array++){
		i++;
	}
	return i;
}

uint8_t USARTFindCmd(uint8_t *template){
	//uint8_t RespHead_ = RespHead; //Save buffer parameters in case if command not found
	//uint8_t RespCount_ = RespCount;
	uint8_t addr=0;
	struct found_template t_found;
	struct usart_response *res;

	while(USARTHasResp()){
		res = USARTGetResp();
		t_found = find_template(res->resp_data , template);
		if (t_found.found) return 1; //return shift of found template in response
	}
	//RespHead = RespHead_;
	//RespCount = RespCount_;
	return 0;
}

uint8_t USARTFindCmdWithData(uint8_t *before_template, uint8_t *after_template,
		                     uint8_t *data){
	uint8_t RespHead_ = RespHead; //Save buffer parameters in case if command not found
	uint8_t RespCount_ = RespCount;
	uint8_t addr=0, d_size, t1_size;
	struct found_template t1_found, t2_found;
	struct usart_response *res;

	d_size = getSize(data);
	t1_size = getSize(before_template);

	while(USARTHasResp()){
		res = USARTGetResp();
		t1_found = find_template(res->resp_data , before_template);
		t2_found = find_template(res->resp_data , after_template);
		if(t1_found.found && t2_found.found){
			for(uint8_t i=0; i<t2_found.shift; i++){
				if(i < d_size){
					data[i] = res->resp_data[t1_found.shift+t1_size+i];
				}
			}
			return 1;
		}
	}
	RespHead = RespHead_;
	RespCount = RespCount_;
	return 0;
}
*/
