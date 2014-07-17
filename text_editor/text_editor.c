#include "text_editor.h"
#include "kb_driver/keyboard_driver.h"
#include "timers.h"
#include "FreeRTOS.h"

uint8_t current_symbol;

keyboard *got_kb;
/*
void display_symbol(uint8_t btn, uint8_t duration, uint8_t press_counter);
uint8_t is_service_symbol(uint8_t symb);
void acept_btn(uint8_t btn, uint8_t duration, uint8_t press_cnt);
void timer_ready(void);
void make_service(uint8_t symbol);
void delete_timer(void);
void reset_timer(void);
void lcd_show_btn(uint8_t btn, uint8_t duration, uint8_t pressed_cnt);
void clean_flags(void);
uint8_t is_max_response(void);
void response_push(uint8_t symbol);
void response_rm_char(void);
void lcd_init_resp(uint8_t *old_response);
void lcd_init_first(void);
uint8_t get_symbol(uint8_t btn, uint8_t duration, uint8_t pressed_cnt);
uint8_t has_variants(uint8_t btn);
uint8_t get_line(uint8_t btn);
uint8_t get_vars_amount(uint8_t btn);
uint8_t typing(button *button_obj);
uint8_t alphabet_pull(uint8_t line, uint8_t element);
void response_init(uint8_t *response, uint8_t size);

enum {
	ExitContinue=0,
	ExitMaxResp,
	ExitOk,
	ExitDiscard
};
uint8_t exit_status=ExitContinue;

uint8_t press_counter=0;
uint8_t resp_ptr;
uint8_t btn_old=0;
uint8_t duration_old=0;
uint8_t timer_id=0;
uint8_t response[max_resp_size];

EditorConf EdConf;

uint8_t typing(button *button_obj){
	uint8_t btn, duration;

	btn = button_obj->button;
	duration = button_obj->duration;

	if(btn){ //if real button pressed
		reset_timer();
		if(btn_old == btn){
			if(press_counter == get_vars_amount(btn)-1)press_counter=0;
			else press_counter++; //If button pressed not first time, increment counter
		}
		else{
			if(btn_old != 0){// If already was pressed button
				delete_timer();
				acept_btn(btn_old, duration_old, press_counter);
			}
			if(btn_old == 0){// Button was pressed first time. all before were committed
				//USART2_PutChar('A');
				btn_old = btn;
				duration_old = duration;
				uint8_t symbol = get_symbol(btn_old, duration_old, press_counter);
				if(! is_service_symbol(symbol)) //If text symbol give it some time
				{
					if(get_vars_amount(btn) < 2){//If button doesn't have variants
						acept_btn(btn, duration, press_counter);
						//cursor_shift(LEFT);
					}
					else timer_id = Slow_Timer_Add(tm_Once, TIME_AFTER_PRESS, timer_ready);
				}
				else acept_btn(btn, duration, press_counter); //if service btn, process it rapidly
			}
		}
		display_symbol(btn, duration, press_counter);
	}
	return exit_status;
}

void delete_timer(void){
	if(timer_id != 0){
		Slow_Timer_Delete(timer_id);
		timer_id=0;
	}
}

void reset_timer(void){
	if(timer_id != 0){
		Slow_Timer_Modify(timer_id, tm_Not_Change, 1, 0);
	}
}

void timer_ready(void){
	acept_btn(btn_old, duration_old, press_counter);
	timer_id = 0;
	return;
}

void display_symbol(uint8_t btn, uint8_t duration, uint8_t press_counter){
	uint8_t symbol;
	if(is_max_response())return;
	symbol = get_symbol(btn, duration, press_counter);
	if(! is_service_symbol(symbol))lcd_show_btn(btn, duration, press_counter); //if not service btn
}

void acept_btn(uint8_t btn, uint8_t duration, uint8_t press_cnt){
	uint8_t symbol;
	symbol = get_symbol(btn, duration, press_counter);

	if(is_service_symbol(symbol)){
		make_service(symbol);
        clean_flags();
		return;
	}

	if(is_max_response()){
		if(EdConf.do_exit_on_max_resp){
			response_push(symbol);
            turn_off_cursor();
            exit_status=ExitMaxResp;
		}
        response_rm_char();
        response_push(symbol);
        clean_flags();
	}
	else{
        response_push(symbol);
        cursor_shift(RIGHT);
        clean_flags();
	}
}

void clean_flags(void){
	btn_old = 0;
	duration_old=0;
	press_counter = 0;
}

void make_service(uint8_t symbol){
	if(symbol == EdConf.clean_char_symb){
        if(resp_ptr > 0){
        	cursor_shift(LEFT);
        	lcd_putc(' ');
        	cursor_shift(LEFT);
        	if(btn_old && !is_service_symbol(symbol))return; //if btn not pushed in response yet
        	response_rm_char();
        }
        if(resp_ptr >= disp_line_length){
            lcd_init_first();
            for(uint8_t i=(resp_ptr-disp_line_length); i<resp_ptr; i++){
                lcd_putc(response[i]);
            }
        }
        exit_status=ExitContinue;
	}
	if(symbol == EdConf.exit_symb_ok){
        turn_off_cursor();
        response_push('\0');
        exit_status=ExitOk;
	}
	if(symbol == EdConf.exit_symb_discard){
        turn_off_cursor();
        response_init(EdConf.old_response, EdConf.resp_size);
        exit_status=ExitDiscard;
	}
}

uint8_t get_symbol(uint8_t btn, uint8_t duration, uint8_t pressed_cnt){
	uint8_t line_num;
	uint8_t el_num;
	uint8_t symbol;

	line_num = get_line(btn); //Get line from alphabet

	el_num = pressed_cnt;
	if(duration == '1'){ //If it was long button press than take last element
		el_num = (EdConf.x_size-1);
	}

	symbol = alphabet_pull(line_num,el_num);
	if(symbol == 0)el_num=0; //If there is no char for such pressed btn
	return symbol;
}

void lcd_show_btn(uint8_t btn, uint8_t duration, uint8_t pressed_cnt){
    if(resp_ptr >= disp_line_length){
    	lcd_init_first();
    	for(uint8_t i=(resp_ptr-disp_line_length+1); i<resp_ptr; i++){
    		lcd_putc(response[i]);
    	}
            //shift_display(LEFT);
    }
	if(EdConf.mask) lcd_putc(EdConf.mask); //If mask set than show mask instesd of symbol
	else lcd_putc(get_symbol(btn, duration, pressed_cnt));
	cursor_shift(LEFT);
}

uint8_t is_service_symbol(uint8_t symb){
	if(symb == EdConf.exit_symb_ok)return 1;
	if(symb == EdConf.exit_symb_discard)return 1;
	if(symb == EdConf.clean_char_symb)return 1;
	return 0;
}

uint8_t has_variants(uint8_t btn){
	uint8_t line_num;
	uint8_t chars_amount=0;

	line_num = get_line(btn);
	for(uint8_t i=1; i<(EdConf.x_size-1); i++){// -1 because last symbol is long press
		if(alphabet_pull(line_num,i) != 0){
			return 1;
		}
	}
	return 0;
}

uint8_t get_line(uint8_t btn){
	uint8_t line_num;

	switch(btn){
	case '#':
		line_num = btn - 25;
		break;
	case '*':
		line_num = btn - 31;
		break;
	default:
		line_num = btn - 48;
		break;
	}
	return line_num;
}

uint8_t get_vars_amount(uint8_t btn){
	uint8_t line;
	uint8_t cnt=0;

	line = get_line(btn);
	for(uint8_t i=0; i<EdConf.x_size; i++){
		if(alphabet_pull(line,i) != 0)cnt++;
		else return cnt;
	}
	return cnt;
}

void init_editor(EditorConf config){
    press_counter=0;
    resp_ptr=0;
    btn_old=0;
    duration_old=0;
    timer_id=0;
    exit_status=ExitContinue;
	EdConf = config;

	lcd_init_first();
	turn_on_cursor();

	response_init(EdConf.old_response, EdConf.resp_size);
	lcd_init_resp(EdConf.old_response);
}

uint8_t alphabet_pull(uint8_t line, uint8_t element){
	return (EdConf.alphabet[line * EdConf.x_size + element]);
}

void response_init(uint8_t *old_response, uint8_t size){
	uint8_t symb;
	resp_ptr=0;
	for(uint8_t i=0; i<max_resp_size; i++)response[i]=0;
	//Show already exist response
	for(uint8_t i=0;i<size;i++){
		symb = old_response[i];
		if(symb != '\0'){
			response_push(symb);
		}
		else break;
	}
}

void lcd_init_resp(uint8_t *old_response){
	while(*old_response){
        lcd_putc(*old_response++);
	}
}

void lcd_init_first(void){
    lcd_clrscr();
    LCDPrintS(EdConf.top_line);
    lcd_goto(2, 0);
}

void response_push(uint8_t symbol){
	if(resp_ptr < max_resp_size){
		response[resp_ptr] = symbol;
		resp_ptr++;
		response[resp_ptr] = '\0';
	}
}

void response_rm_char(void){
	uint8_t old_one;
	old_one = response[resp_ptr-1];////////////////////////////////////////////////////
	if(resp_ptr > 0){
		response[resp_ptr] = '\0';
		resp_ptr--;
		response[resp_ptr] = '\0';
	}
}

uint8_t is_max_response(void){
	if(resp_ptr >= EdConf.resp_size)return 1;
	return 0;
}

uint8_t *get_text(void){
	return response;
}
*/
void text_editor_init(keyboard *init_struct){
	got_kb = init_struct;
	lcd_clrscr();
	lcd_prints(init_struct->label);
	lcd_goto(2,0);
	lcd_prints(init_struct->init_text);
	turn_on_cursor(init_struct->blinking);

	vSemaphoreCreateBinary(xKeyApprovedSemaphore);
	if (xKeyApprovedSemaphore == NULL) log("Can't create binary key semaphore", ERROR_LEVEL);
	xSemaphoreTake(xBinaryKeySemaphore, portMAX_DELAY);// Clear semaphore for the first time

	vSemaphoreCreateBinary(xSymbolChangedSemaphore);
	if (xSymbolChangedSemaphore == NULL) log("Can't create binary key semaphore", ERROR_LEVEL);
	xSemaphoreTake(xSymbolChangedSemaphore, portMAX_DELAY);// Clear semaphore for the first time

	xCurrentSymbolMutex = xSemaphoreCreateMutex();
	if (xCurrentSymbolMutex == NULL) log("Can't create current symbol mutex", ERROR_LEVEL);

}

void text_editor_close(void){
	turn_off_cursor();
	lcd_clrscr();
}

void key_controller(uint8_t key, uint8_t duration){
	enum{
		NEW_KEY,
		SAME_KEY,
		DIFF_KEY
	};
	static uint8_t old_key=0;
	static uint8_t state=NEW_KEY;
	xTimerHandle xBtnTimer;

	if(duration == LONG_PRESS){
		return get_btn(key, 1, duration);
	}

	switch(state){
	case NEW_KEY:
		if(xSemaphoreTake(xCurrentSymbolMutex, 10/portTICK_RATE_MS) == pdTRUE){
			xSemaphoreGive(xSymbolChangedSemaphore);
			current_symbol = get_btn(key, 1, SHORT_PRESS);
			xSemaphoreGive(xCurrentSymbolMutex);
		}
		xBtnTimer = xTimerCreate("BtnTimer", 1000/portTICK_RATE_MS, pdFALSE, 1, SymbolApproved);
		if(xBtnTimer == NULL)log("Timer for button isn't created", ERROR_LEVEL);
		if(xTimerStart(xBtnTimer, 10) == pdFAIL)log("Timer for button can't be started", ERROR_LEVEL);
	break;
	}
}

uint8_t read_symbol(void){
	if(xSemaphoreTake(xKeyApprovedSemaphore, 10) == pdPASS){
		if(xSemaphoreTake(xCurrentSymbolMutex, 10) == pdPASS){
				xSemaphoreGive(xCurrentSymbolMutex);
				return current_symbol;
		}
	}
	return 0;
}

uint8_t read_tmp_symbol(void){
	if(xSemaphoreTake(xSymbolChangedSemaphore, 10) == pdPASS){
		if(xSemaphoreTake(xCurrentSymbolMutex, 10) == pdPASS){
			xSemaphoreGive(xCurrentSymbolMutex);
			return current_symbol;
		}
	}
	return 0;
}

void SymbolApproved(xTimerHandle xTimer){
	xSemaphoreGive(xKeyApprovedSemaphore);
}

uint8_t get_line(uint8_t btn){
	switch(btn){
	case '#':
		return btn - 25;
	case '*':
		return btn - 31;
	default:
		return btn - 48;
	}
}

uint8_t pull_key(uint8_t line, uint8_t position){
	return got_kb->alphabet[line][position];
}

uint8_t get_btn(uint8_t btn, uint8_t pressed, uint8_t duration){
	uint8_t line;
	uint8_t btn;
	uint8_t pos;
	uint8_t variants;
	uint8_t ost;

	line = get_line(btn);

	//If button was pressed for a long time
	if(duration == LONG_PRESS)return pull_key(line, got_kb->max_variants_size-1);

	variants = get_vars(line);
	if(pressed <= variants)return pull_key(line, pressed-1);

	ost = pressed % variants;
	if(ost == 0)return pull_key(line, variants-1);
	return pull_key(line, ost-1);

}

uint8_t get_vars(uint8_t line){
	uint8_t i;
	//when we get variants we shouldn't count last symbol for long press
	for(i=0; i < got_kb->max_variants_size-1; i++){
		if(got_kb->alphabet[line][i] == 0)return i;
	}
	return i+1;
}
