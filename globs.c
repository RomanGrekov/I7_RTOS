#include "globs.h"

void log(uint8_t *data){
	if(Debug == 1){
		USART2QueueSendString(data);
	}
}
