#include "globs.h"

void log(uint8_t *data, uint8_t level){
	if(level <= DEBUG_LEVEL){
		USART2QueueSendString(data);
	}
}
