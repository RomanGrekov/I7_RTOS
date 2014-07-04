#ifndef MENU_H
#define MENU_H
#include "stm32f10x.h"
#include <string.h>

#include "views/sim_on_off/sim_on_off.h"
#include "views/add_phone/add_phone.h"

typedef struct{
	void       *Next;
	void       *Previous;
	void       *Parent;
	void       *Child;
	void	   (*handler)();
	uint8_t     View;
	const char  Text[16];
	void       *This;
} menuItem;

#define MAKE_MENU(Name, Next, Previous, Parent, Child, handler, View, Text) \
    extern menuItem Next;     \
	extern menuItem Previous; \
	extern menuItem Parent;   \
	extern menuItem Child;  \
	menuItem Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Child, (void*)&handler, (uint8_t)View, { Text }, (void*)&Name}

enum {
    MENU_NEXT=1,
    MENU_PREVIOUS,
    MENU_CHILD,
    MENU_PARENT,
    MENU_THIS,
};

void InitMenu(void);
void menuChange(menuItem* NewMenu);
void changeMenu(uint8_t el_num);
uint8_t* GetCurMenuName(void);
void ProcessMenu(uint8_t btn, uint8_t duaration);
uint8_t GetCurMenuView(void);
void NULL_HANDLER(void);
void callView(void);
uint8_t is_in_menu(void);

#endif
