
// some functions to access data stored in our virtual eeprom
// DL2FW 03/2018

#include "stm32f4_vrom.h"
#include "audio_nr.h"
#include "eeprom_access.h"
#include "stdio.h"
#include "tm_stm32f4_hd44780.h"


const uint16_t FW_Version  = 10;  //starting with 10;

int16_t load_settings()

{

  NoiseredPara temp_settings;
  int res;
  char buf[15];

    res = vrom_read(7,0,sizeof(temp_settings),&temp_settings); // just trying ROM ID 7
    //sprintf(buf, "read_res:%3d", (int)(res));  // print the result to the lcd - here I get always "-6"
    //TM_HD44780_Puts(0, 3,buf);

  if (temp_settings.Version != NR3.Version)     //global Variable, should be altered, when changing size of EEPROM content
						//just to make sure, that data is compatible
    return -1;  				// incompatible EEPROM Data, load defaults!!!

  else
    {
      memcpy(&NR3, &temp_settings, sizeof(temp_settings)); // could be done directly, might need a 2nd checking in between?
    }

  return 0;

}

int16_t write_settings()

{
  int res;
  char buf[15];

res = vrom_write(7,0,sizeof(NR3),&NR3); // here I get "20" after the 2nd write attempt the first result gives "0"
					//  but the data is already after the 1st write in the EEPROM
//sprintf(buf, "write_res:%3d", (int)(res));
//TM_HD44780_Puts(0, 3,buf);

return 0;

}
