#include "tm_stm32f4_hd44780.h"
#include "tm_stm32f4_rotary_encoder.h"
#include "tm_stm32f4_exti.h"
#include "tm_stm32f4_button.h"

#include "audio_nr.h"

#ifndef __UI_H
#define __UI_H


TM_RE_t RE1_Data;




void init_lcd_enc();
void menu_handling();
void BUTTON1_EventHandler(TM_BUTTON_PressType_t);
void BUTTON2_EventHandler(TM_BUTTON_PressType_t);
int32_t modify_menu_item(int32_t *, int32_t, int32_t);
int32_t nr_on_state();

#endif
