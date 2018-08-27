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
  uint8_t level1[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111
  };

  uint8_t level2[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111
  };
  uint8_t level3[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111
  };
  uint8_t level4[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  uint8_t level5[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  uint8_t level6[8] = {
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  uint8_t level7[8] = {
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  uint8_t level8[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };



   TM_HD44780_Clear();
   TM_HD44780_Init(16, 2);

   TM_HD44780_CreateChar(0,level1);
   TM_HD44780_CreateChar(1,level2);
   TM_HD44780_CreateChar(2,level3);
   TM_HD44780_CreateChar(3,level4);
   TM_HD44780_CreateChar(4,level5);
   TM_HD44780_CreateChar(5,level6);
   TM_HD44780_CreateChar(6,level7);
   TM_HD44780_CreateChar(7,level8);


   TM_HD44780_Puts(0, 0, " Noisereduction ");


   TM_RE_Init(&RE1_Data, GPIOD, GPIO_PIN_8, GPIOD, GPIO_PIN_9);
   TM_RE_SetMode(&RE1_Data, TM_RE_Mode_One);

   TM_BUTTON_Init(GPIOD, GPIO_PIN_10, 0, BUTTON1_EventHandler); //Encoder Button
   TM_BUTTON_Init(GPIOA, GPIO_PIN_0, 1, BUTTON2_EventHandler); //Blue Button to switch on NR

}

void BUTTON1_EventHandler(TM_BUTTON_PressType_t type) {
    /* Check button */

  if (menu_pos != 6) //don't activate select when in spectrum window!
    {
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

  char buf[20];
  static int16_t was_in = -1;

  TM_BUTTON_Update(); //checks for Encoder-Button condition

  if (selected == 0)
    {
      TM_RE_Get(&RE1_Data);
      if (RE1_Data.Diff > 0)
	{

	  sprintf(buf, "                ");
	  TM_HD44780_Puts(0, 0,buf);
	  TM_HD44780_Puts(0, 1,buf);

	  menu_pos++;
	  if (menu_pos > 6) menu_pos = 6;
	}
      else if (RE1_Data.Diff < 0)
	{
	  //sprintf(buf, "                ");
	  //TM_HD44780_Puts(0, 0,buf);
	  //TM_HD44780_Puts(0, 1,buf);

	  menu_pos--;
	  if (menu_pos < 0) menu_pos = 0;
	}
    }


  switch (menu_pos){

    case 0:  // change nr_alpha

        changed_item = modify_menu_item(&NR3.alpha_int,100,0);
	if ((trigger_select == 1) || changed_item || was_in != 0)
	  {
	    sprintf(buf, "                ");
	    TM_HD44780_Puts(0, 0,buf);
	    TM_HD44780_Puts(0, 1,buf);
	    sprintf(buf, "Red. Level: %3d", (int)(NR3.alpha_int));
	    TM_HD44780_Puts(0, 1,buf);
	  }
	was_in = 0;
	changed_item=0;
    break;

    case 1:  // change asnr


        changed_item = modify_menu_item(&NR3.asnr_int, 30,0);
	if ((trigger_select == 1) || changed_item || was_in != 1)
	  {
	    sprintf(buf, "                ");
	    TM_HD44780_Puts(0, 0,buf);
	    TM_HD44780_Puts(0, 1,buf);
	    sprintf(buf, "asnr      : %3d", (int)(NR3.asnr_int));
	    TM_HD44780_Puts(0, 1,buf);
	  }
	was_in = 1;
	changed_item=0;
    break;


    case 2:  // change power_threshold

        changed_item = modify_menu_item(&NR3.power_threshold_int, 99,10);

	if ((trigger_select == 1) || changed_item || was_in != 2)
	  {
	    sprintf(buf, "                ");
	    TM_HD44780_Puts(0, 0,buf);
	    TM_HD44780_Puts(0, 1,buf);
	    sprintf(buf, "pwrthresh.: %3d", (int)(NR3.power_threshold_int));
	    TM_HD44780_Puts(0, 1,buf);
	  }
	was_in = 2;
	changed_item=0;

	break;


    case 3:  // change width

	changed_item = modify_menu_item(&NR3.width_int, 50,2);

	if ((trigger_select == 1) || changed_item || was_in != 3)
	  {
	    sprintf(buf, "                ");
	    TM_HD44780_Puts(0, 0,buf);
	    TM_HD44780_Puts(0, 1,buf);
	    sprintf(buf, "width:      %3d", (int)(NR3.width_int));
	    TM_HD44780_Puts(0, 1,buf);
	  }
	was_in = 3;
	changed_item=0;
            break;

    case 4:  // store to EEPROM

      changed_item = modify_menu_item(&write_confirmed,1,0);


    	if ((trigger_select == 1) || changed_item || was_in != 4)
    	  {
    	    sprintf(buf, "                ");
	    TM_HD44780_Puts(0, 0,buf);
	    TM_HD44780_Puts(0, 1,buf);
    	    sprintf(buf, "save->eeprom  %1d", (int)(write_confirmed));
    	    TM_HD44780_Puts(0, 1,buf);
    	  }
    	was_in = 4;
    	changed_item=0;
	    break;

    case 5:  // switch NR ON OFF

        changed_item = modify_menu_item(&NR_enabled,1,0);


      	if ((trigger_select == 1) || changed_item || was_in != 5)
      	  {
	    sprintf(buf, "                ");
	    TM_HD44780_Puts(0, 0,buf);
	    TM_HD44780_Puts(0, 1,buf);
      	    sprintf(buf, "NR ON/OFF     %1d", (int)(NR_enabled));
      	    TM_HD44780_Puts(0, 1,buf);
      	  }
      	was_in = 5;
      	changed_item=0;
      	break;

    case 6:  // Display Spektrum and HK's

	  selected = 0;  //avoid that we get stuck here by pressing the knob!!!
	  if (was_in != 6)
	    {
	      sprintf(buf, "                ");
	      TM_HD44780_Puts(0, 0,buf);
	      TM_HD44780_Puts(0, 1,buf);
	      sprintf(buf, "                ");
	      TM_HD44780_Puts(0, 0,buf);
	      TM_HD44780_Puts(0, 1,buf);
	      was_in = 6;
	      changed_item=0;
	    }

          break;


  }


}

int32_t  modify_menu_item(int32_t * item, int32_t maxval, int32_t minval)
{
  int32_t changed, count_buf;
  count_buf=*item;
  changed = 0;
	if (trigger_select == 1)
	  {
	    RE1_Data.Absolute = *item; //on first pass set to current value
	    RE1_Data.RE_Count = *item;
	    count_buf = *item;
	    //TM_HD44780_Puts(0, 2,"                ");
	    trigger_select = 0;  // we have been here!
	  }
	if (selected == 1)
	  {
	    TM_RE_Get(&RE1_Data);
	    if (RE1_Data.Rotation != TM_RE_Rotate_Nothing)
	      {
		changed = 1;
		count_buf = RE1_Data.RE_Count;
		if (count_buf > maxval)  {
		  count_buf=maxval;
		  RE1_Data.RE_Count = maxval;
		  RE1_Data.Absolute = maxval;
		  RE1_Data.Rotation = TM_RE_Rotate_Nothing;
		}

		if (count_buf < minval)  {
		  count_buf = minval;
		  RE1_Data.RE_Count = minval;
		  RE1_Data.Absolute = minval;
		  RE1_Data.Rotation = TM_RE_Rotate_Nothing;
		}
	      }
	      else changed = 0;

	    *item = count_buf;
	  }

	return changed;
}

int16_t get_menu_pos()
{
  return menu_pos;
}
