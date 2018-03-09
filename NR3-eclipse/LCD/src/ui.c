#include "ui.h"
#include "tm_stm32f4_hd44780.h"
#include "tm_stm32f4_rotary_encoder.h"
#include "tm_stm32f4_exti.h"
#include "stdio.h"


static int16_t menu_pos=0; // keeps the actual menu position

static int16_t trigger_select = 0;

static int32_t nr_on = 0;
static int32_t changed_item = 0;
//static int32_t write_confirmed = 0;
//static int16_t selected = 0; // is 1 as long as an item is selected to change
// sets up the encoder, encoder button  and LCD for our user interface

void init_lcd_enc()
{

   TM_HD44780_Clear();
   TM_HD44780_Init(20, 4);
   TM_HD44780_Puts(0, 0, "   Noisereduction ");

   TM_RE_Init(&RE1_Data, GPIOC, GPIO_PIN_14, GPIOC, GPIO_PIN_15);
   TM_RE_SetMode(&RE1_Data, TM_RE_Mode_One);

   TM_BUTTON_Init(GPIOC, GPIO_PIN_13, 0, BUTTON1_EventHandler); //Encoder Button
   TM_BUTTON_Init(GPIOA, GPIO_PIN_0, 1, BUTTON2_EventHandler); //Blue Button to switch on NR

}

void BUTTON1_EventHandler(TM_BUTTON_PressType_t type) {
    /* Check button */
    if (type == TM_BUTTON_PressType_OnPressed) {
//	TM_HD44780_Puts(0, 3,"pressed");
    } else if (type == TM_BUTTON_PressType_Normal) {
	if (selected == 0)  //toggle between item is selected and not selected
	  {
	    selected = 1;
	    trigger_select = 1;

	    TM_HD44780_CursorOn();
	    TM_HD44780_BlinkOn();
	  }
	else
	  {
	    selected = 0;
	    TM_HD44780_CursorOff();
	    TM_HD44780_BlinkOff();

	  }
    }
	else {
	    //	TM_HD44780_Puts(0, 3,"long pressed");
	}
}

void BUTTON2_EventHandler(TM_BUTTON_PressType_t type) {
    /* Check button */
    if (type == TM_BUTTON_PressType_OnPressed) {
	//TM_HD44780_Puts(0, 3,"pressed");
    } else if (type == TM_BUTTON_PressType_Normal) {

	if (nr_on == 0) nr_on = 1;
	else nr_on = 0;

    }
	else {
	    //	TM_HD44780_Puts(0, 3,"long pressed");
	}
}

int32_t nr_on_state()
{
  TM_BUTTON_Update();
  return nr_on;
}




void menu_handling()
{

  char buf[15];

  TM_BUTTON_Update(); //checks for Encoder-Button condition

  if (selected == 0)
    {
      TM_RE_Get(&RE1_Data);
      if (RE1_Data.Diff > 0)
	{
	  menu_pos++;
	  if (menu_pos > 4) menu_pos = 4;
	}
      else if (RE1_Data.Diff < 0)
	{
	  menu_pos--;
	  if (menu_pos < 0) menu_pos = 0;
	}
    }


  switch (menu_pos){

    case 0:  // change nr_alpha

        changed_item = modify_menu_item(&NR3.alpha_int,100,0);
	if ((trigger_select == 1) || changed_item)
	  {
	    sprintf(buf, "Red. Level  : %3d", (int)(NR3.alpha_int));
	    TM_HD44780_Puts(0, 2,buf);
	  }
    break;

    case 1:  // change asnr

        changed_item = modify_menu_item(&NR3.asnr_int, 30,0);
	if ((trigger_select == 1) || changed_item)
	  {
	    sprintf(buf, "asnr        : %3d", (int)(NR3.asnr_int));
	    TM_HD44780_Puts(0, 2,buf);
	  }
    break;


    case 2:  // change power_threshold

        changed_item = modify_menu_item(&NR3.power_threshold_int, 99,10);

	if ((trigger_select == 1) || changed_item)
	  {
	    sprintf(buf, "pwr thresh.:  %3d", (int)(NR3.power_threshold_int));
	    TM_HD44780_Puts(0, 2,buf);
	  }

	break;


    case 3:  // change width

	changed_item = modify_menu_item(&NR3.width_int, 50,2);

	if ((trigger_select == 1) || changed_item)
	  {
	    sprintf(buf, "width:        %3d", (int)(NR3.width_int));
	    TM_HD44780_Puts(0, 2,buf);
	  }

            break;

    case 4:  // store to EEPROM

      changed_item = modify_menu_item(&write_confirmed,1,0);


    	if ((trigger_select == 1) || changed_item)
    	  {
    	    sprintf(buf, "save to eeprom  %1d", (int)(write_confirmed));
    	    TM_HD44780_Puts(0, 2,buf);
    	  }

	    break;

  }


}

int32_t  modify_menu_item(int32_t * item, int32_t maxval, int32_t minval)
{
  int32_t changed;
	if (trigger_select == 1)
	  {
	    RE1_Data.Absolute = *item; //on first pass set to current value
	    RE1_Data.RE_Count = *item;
	    TM_HD44780_Puts(0, 2,"                ");
	    trigger_select = 0;  // we have been here!
	  }
	if (selected == 1)
	  {
	    TM_RE_Get(&RE1_Data);
	    if (RE1_Data.Rotation != TM_RE_Rotate_Nothing) changed = 1;
	      else changed = 0;
	    if (RE1_Data.RE_Count > maxval)  {
	    RE1_Data.RE_Count = maxval;
	    RE1_Data.Absolute = maxval;
	    RE1_Data.Rotation = TM_RE_Rotate_Nothing;
	    }

	    if (RE1_Data.RE_Count < minval)  {
	    RE1_Data.RE_Count = minval;
	    RE1_Data.Absolute = minval;
	    RE1_Data.Rotation = TM_RE_Rotate_Nothing;
	    }
	    *item=(int32_t)RE1_Data.Absolute;
	  }

	return changed;
}

