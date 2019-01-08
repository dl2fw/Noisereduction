#include "tm_stm32f4_hd44780.h"
#include "tm_stm32f4_rotary_encoder.h"
#include "tm_stm32f4_exti.h"
#include "tm_stm32f4_button.h"

#include "audio_nr.h"

#ifndef _UI_H
#define _UI_H


TM_RE_t RE1_Data;

int32_t write_confirmed;
//int32_t NR_enabled;



int16_t selected; // is 1 as long as an item is selected to change

int32_t pulse_position[100];//only used for NB test
int32_t pulse_length[100];//only used for NB test
int32_t stop_feedback;//only used for NB test



void init_lcd_enc();
void menu_handling();
void BUTTON1_EventHandler(TM_BUTTON_PressType_t);
void BUTTON2_EventHandler(TM_BUTTON_PressType_t);
int32_t modify_menu_item(int32_t *, int32_t, int32_t);
int32_t nr_on_state();

int16_t get_menu_pos();

#endif
