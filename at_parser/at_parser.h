#ifndef AT_PARSER
#define AT_PARSER

#include "globs.h"

enum{
	NOT_FOUND,
	FOUND
};

typedef struct{
	unsigned char found;
	unsigned char shift;
}found_template;

typedef struct{
	unsigned char size;
	unsigned char response[AT_RESPONSE_SIZE];
} at_response;

typedef struct{
	unsigned char size;
	unsigned char *template;
} at_template;

typedef struct{
	unsigned char size;
	unsigned char data[MAX_DATA_SIZE_IN_RESPONSE];
}data_in_resp;

unsigned char USARTCheckData(unsigned char symb, at_response * res);
found_template find_template(uint8_t *resp, uint8_t resp_size, uint8_t *template, uint8_t templ_size);
found_template find_template_in_response(at_response *response, at_template *template);
uint8_t get_data_from_response(at_response *response, at_template *template, data_in_resp *data);
uint8_t find_data_in_resp(at_template *template, data_in_resp *data);
uint8_t find_answer(at_template *template);
void flush_answers(void);

#endif
