
// some functions to access data stored in our virtual eeprom
// DL2FW 03/2018

#include "stm32f4_vrom.h"
#include "audio_nr.h"
#include "eeprom_access.h"
#include "stdio.h"


const uint16_t FW_Version  = 10;  //starting with 10;

int16_t load_settings()

{



  NoiseredPara temp_settings;


  int res;

  res = vrom_read(0,0,sizeof(temp_settings),&temp_settings);

  if (temp_settings.Version != NR3.Version)     //global Variable, should be altered, when changing size of EEPROM content
						//just to make sure, that data is compatible
    return -1;  				// incompatible EEPROM Data, load defaults!!!

  else
    {

      memcpy(&NR3, &temp_settings, sizeof(NR3)); // could be done directly, might need a 2nd checking in between?
    }

  return 0;

}

int16_t write_settings()

{
  int res;

NoiseredPara temp_settings;//just a test here to see, if the data made it to the EEPROM

  res = vrom_write(0,0,sizeof(NR3),&NR3);

  //just a test here to see, if the data made it to the EEPROM

  res = vrom_read(0,0,sizeof(temp_settings),&temp_settings);


return 0;

}
