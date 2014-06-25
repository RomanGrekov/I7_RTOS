#ifndef SIM_SEND_CMD_H
#define SIM_SEND_CMD_H
#include "stm32f10x.h"
#include <string.h>
#include "globs.h"
#include "menu.h"


struct command{
	uint8_t cmd[20];
	uint16_t timeout;
};

#define MAKE_COMMAND(name, cmd, amount) \
	command name = {(uint8_t)amount, { cmd }}\

void send_test_cmds(void);
#endif
