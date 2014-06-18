#ifndef AT_PARSER
#define AT_PARSER

#include "globs.h"

enum{
	NOT_FOUND,
	FOUND
};

struct found_template{
	unsigned char found;
	unsigned char shift;
};

typedef struct{
	unsigned char response[AT_RESPONSE_SIZE];
	unsigned char size;
} at_response;

unsigned char USARTCheckData(unsigned char symb, at_response * res);

#endif
