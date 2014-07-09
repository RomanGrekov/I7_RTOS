//------------------------------------------------------
/* File:       Library for HD44780 compatible displays  */
/* Version:	   v1.1  						 			*/
/* Language	   ANSI C			   		  	 			*/
/* Author:     GrAnd/www.MakeSystem.net		 			*/
/* Tested on:  AVR		  			 	 	 		 	*/
/* License:	   GNU LGPLv2.1		 		 	 			*/
//------------------------------------------------------
/* Copyright (C)2012 GrAnd. All right reserved 			*/
//------------------------------------------------------

/*
  [hd44780.h - Library for HD44780 compatible displays]
  [Copyright (C)2012 GrAnd. All right reserved]

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Contact information :
						mail@makesystem.net
						http://makesystem.net/?page_id=2
*/

#include "hd44780.h"
#include "FreeRTOS.h"

gpio_init(void){

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //Clock port
    GPIOB->CRH      &= ~(GPIO_CRH_CNF10 |
    					 GPIO_CRH_CNF11 |
    					 GPIO_CRH_CNF12 |
    					 GPIO_CRH_CNF13 |
    					 GPIO_CRH_CNF14 |
    					 GPIO_CRH_CNF15);
    GPIOB->CRH		|= (GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1 |
    			        GPIO_CRH_MODE11_0 | GPIO_CRH_MODE11_1 |
    			        GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 |
    			        GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1 |
    			        GPIO_CRH_MODE14_0 | GPIO_CRH_MODE14_1 |
    			        GPIO_CRH_MODE15_0 |  GPIO_CRH_MODE15_1);
}

//-------------------------------
// LOW LEVEL FUNCTIONS
//-------------------------------
static void DELAY(int32u_t us);
static void LCD_STROBE(int32u_t u);
static void HIGHBITS(int8u_t data);
static void LOWBITS(int8u_t data);

//-------------------------------
/* DELAY FUNCTION */
//-------------------------------
static void DELAY(volatile int32u_t us)
{
	int32u_t t=0;
	if(us >= 1000)us=us/1000;
	if(us >= 100)us=us/100;
	if(us >= 10)us=us/10;
	vTaskDelay(us);
}

static void DELAY_native(volatile int32u_t us)
{
	volatile int32u_t n, alfa, i;
	n = us * (configCPU_CLOCK_HZ / 1000000);
	i=0;

	for(alfa=0;alfa<n;alfa++)
	{
		i++;
	}
}

//-------------------------------
/* INITIATE TRANSFER OF DATA/COMMAND TO LCD */
//-------------------------------
static void LCD_STROBE(uint32_t us)
{
 ENABLE(LCD_WIRE,E);
 DELAY(us);
 DISABLE(LCD_WIRE,E); // Enter
 DELAY(us);
}

static void LCD_STROBE_native(uint32_t us)
{
 ENABLE(LCD_WIRE,E);
 DELAY_native(us);
 DISABLE(LCD_WIRE,E); // Enter
 DELAY_native(us);
}

//-------------------------------
/* PUT HIGH BITS */
//-------------------------------
static void HIGHBITS(int8u_t data)
{
 if(data & 0x80) ENABLE(LCD_WIRE,D7); else DISABLE(LCD_WIRE,D7);
 if(data & 0x40) ENABLE(LCD_WIRE,D6); else DISABLE(LCD_WIRE,D6);
 if(data & 0x20) ENABLE(LCD_WIRE,D5); else DISABLE(LCD_WIRE,D5);
 if(data & 0x10) ENABLE(LCD_WIRE,D4); else DISABLE(LCD_WIRE,D4);
}

//-------------------------------
/* PUT LOW BITS */
//-------------------------------
static void LOWBITS(int8u_t data)
{
 if(data & 0x08) ENABLE(LCD_WIRE,D7); else DISABLE(LCD_WIRE,D7);
 if(data & 0x04) ENABLE(LCD_WIRE,D6); else DISABLE(LCD_WIRE,D6);
 if(data & 0x02) ENABLE(LCD_WIRE,D5); else DISABLE(LCD_WIRE,D5);
 if(data & 0x01) ENABLE(LCD_WIRE,D4); else DISABLE(LCD_WIRE,D4);
}

//-------------------------------
/* PUT DATA/COMMAND TO LCD */
//-------------------------------
void lcd_cmd(int8u_t data)
{/* LCD ELEMENTARY COMMAND */
 HIGHBITS(data);
 LCD_STROBE(20);
 LOWBITS(data);
 LCD_STROBE(20); // busy delay
}

void lcd_cmd_native(int8u_t data)
{/* LCD ELEMENTARY COMMAND */
 HIGHBITS(data);
 LCD_STROBE_native(20);
 LOWBITS(data);
 LCD_STROBE_native(20); // busy delay
}

//-------------------------------
/* LCD CLEAR SCREEN */
//-------------------------------
void lcd_clrscr(void)
{
 lcd_cmd(0x01); // clear screen
 DELAY(200);
}

//-------------------------------
/* LCD RETURN CURSOR */
//-------------------------------
void lcd_return(void)
{
 lcd_cmd(0x02); // return cursor
}

//-------------------------------
/* GO TO SPECIFIED MEMORY ADDRESS */
//-------------------------------
void lcd_goto(int8u_t line, int8u_t address)
{/* GO TO SPECIFIED ADDRESS */
 switch(line)
 {
  case     1: lcd_cmd(0x80|address); break;
  case     2: lcd_cmd(0xC0|address); break;
  case CGRAM: lcd_cmd(0x40|address); break; // CGRAM address
 }
}

//-------------------------------
/* WRITE A SINGLE CHARACTER 
   TO SPECIFIED MEMORY */
//-------------------------------
void lcd_putc(int8u_t data)
{/* WRITE A CHARACTER TO LCD */
 ENABLE(LCD_WIRE,RS);
 lcd_cmd(data);
 DISABLE(LCD_WIRE,RS);
}

//-------------------------------
/* ERASE A SINGLE CHARACTER 
   FROM DISPLAY */
//-------------------------------
void lcd_backspace(void)
{/* ERASE LEFT CHAR */
 lcd_cmd(0x10); // �������� ������ �� ���� ������� �����
 lcd_putc(' '); // �������, ����� ���� ���������� ������������� ������
 lcd_cmd(0x10); // �������� ������ �� ���� ������� �����
}

//-------------------------------
/* SCROLL DISPLAY 
   TO SPECIFIED DIRECTION */
//-------------------------------
void lcd_scroll(int8u_t direction)
{
 switch(direction)
 {
  case RIGHT : lcd_cmd(0x1C); break; // scroll display to right
  case LEFT  : lcd_cmd(0x18); break; // scroll display to left
 }
}

//-------------------------------
/* SHIFT CURSOR 
   TO SPECIFIED DIRECTION */
//-------------------------------
void cursor_shift(int8u_t direction)
{
 switch(direction)
 {
  case RIGHT : lcd_cmd(0x14); break; // shift cursor to right
  case LEFT  : lcd_cmd(0x10); break; // shift cursor to left
 }
}

//-------------------------------
/* DISPLAY A INTEGER NUMER */
//-------------------------------
void lcd_itostr(int32s_t value)
{/* DISPLAY A INTEGER NUMER: +/- 2147483647 */
 int32s_t i;
 if(value<0)
 {
  lcd_putc('-');
  value=-value;
 }
 for(i=1;(value/i)>9;i*=10);
 lcd_putc(value/i+'0');
 i/=10;
 while(i)
 {
  lcd_putc((value%(i*10))/i+'0');
  i/=10;
 }
}

//-------------------------------
/* DISPLAY A 4-DIGIT INTEGER NUMER */
//-------------------------------
void lcd_numTOstr(int16u_t value, int8u_t nDigit)
{/* DISPLAY n-DIGIT INTEGER NUMER */
 switch(nDigit)
 {
  case 4: lcd_putc((value/1000)+'0');
  case 3: lcd_putc(((value/100)%10)+'0');
  case 2: lcd_putc(((value/10)%10)+'0');
  case 1: lcd_putc((value%10)+'0');
 }
}

//-------------------------------
/* CONFIGURE 4-BIT DISPLAY INTERFACE */
//-------------------------------
void lcd_config(int8u_t param)
{/* CONFIGURE THE DISPLAY */
 HIGHBITS(param); // 4-bit, two lines, 5x8 pixel
  LCD_STROBE_native(9); // change 8-bit interface to 4-bit interface
  LCD_STROBE_native(9); // init 4-bit interface
 LOWBITS(param);
  LCD_STROBE_native(40);
}

//-------------------------------
/* INITIALIZE ENTIRE DISPLAY */
//-------------------------------
void lcd_init(void)
{
 DELAY_native(15000);
 gpio_init();
 lcd_cmd_native(0x30); // 1, return home cursor
 lcd_cmd_native(0x30); // 1, return home cursor
 lcd_cmd_native(0x30); // 1, return home cursor
 lcd_cmd_native(0x30); // 1, return home cursor
 lcd_cmd_native(0x30); // 1, return home cursor

 lcd_config(DEFAULT_DISPLAY_CONFIG); // 1, Data Lenght, Number of lines, character font
 lcd_cmd_native(DEFAULT_DISPLAY_CONTROL); // 1, lcd, cursor, blink
 lcd_cmd_native(DEFAULT_ENTRY_MODE); // 1,increment/decrement,display shift on/off
 lcd_cmd_native(0x01); // clear display
 lcd_cmd_native(0x02); // 1, return home cursor
#if (USE_PROGRESS_BAR)
 lcd_readybar();
#endif
}

void turn_on_cursor(void){
 lcd_cmd(DISPLAY_CTRL_DispOn_BlkOff_CrsOn); // 1, lcd, cursor, blink
}
void turn_off_cursor(void){
 lcd_cmd(DISPLAY_CTRL_DispOn_BlkOff_CrsOff); // 1, lcd, cursor, blink
}

void lcd_prints(const int8u_t *p)
{
	while(*p)
	{
		lcd_putc(*p++);
	}
}

void shift_display(uint8_t direction)
{
	if(direction == '>'){
		lcd_cmd(0x1E);
	};
	if(direction == '<'){
		lcd_cmd(0x18);
	};
}

portBASE_TYPE put_to_lcd_queue(uint8_t *p){
	portBASE_TYPE xStatus;
	uint8_t i, a;

	if(xSemaphoreTake(xLcdMutex, portMAX_DELAY) == pdPASS){
		for(i=0; i<LCD_QUEUE_SIZE; i++){
			if(*p){
				a = *p;
				p++;
			}
			else a=' ';

			if(xQueueSendToBack(xQueueLCD, &a, 0) != pdPASS){
				xSemaphoreGive(xLcdMutex);
				return xStatus;
			}
		}
		xSemaphoreGive(xLcdMutex);
	}
	else return pdFAIL;
	return pdPASS;
}

void prvLcdShow( void *pvParameters )
{
    uint8_t symb, buffer_cnt=0, el_in_queue=0;
    portBASE_TYPE xStatus;

    while(1){
    	el_in_queue = uxQueueMessagesWaiting(xQueueLCD);
    	if(el_in_queue > 0){
    		if(xSemaphoreTake(xLcdMutex, portMAX_DELAY) == pdPASS){
            	while(el_in_queue > 0){
            		if(xQueueReceive(xQueueLCD, &symb, 0) == pdPASS){
            			if (buffer_cnt == 32){
							lcd_clrscr();
							buffer_cnt = 0;
						}
						if(buffer_cnt == 16){
							lcd_goto(2,0);
						}
						lcd_putc(symb);
						buffer_cnt++;
						el_in_queue--;
            		}
            	}
            	xSemaphoreGive(xLcdMutex);
    		}
    	}
    }
}

//-------------------------------
/* END OF FILE */
//-------------------------------
