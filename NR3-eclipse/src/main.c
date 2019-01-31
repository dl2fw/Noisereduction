#include "stm32f4xx.h"
#include "audio_nr.h"
#include "samp_rate_conv.h"
#include "stm32f4_adc.h"
#include "stm32f4_dac.h"

#include "stm32f4_vrom.h"

#include "sm1000_leds_switches.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stdlib.h>
#include <stdio.h>


#include "arm_math.h"
#include "tm_stm32f4_hd44780.h"
#include "tm_stm32f4_rotary_encoder.h"
#include "tm_stm32f4_exti.h"
#include "codec.h"
#include "stm32f4_discovery_audio_codec.h"
#include "tm_stm32f4_delay.h"
#include "ui.h"
#include "eeprom_access.h"







int main(void) {


  //try to read presets from our virtual EEPROM
  NR3.Version = 10;
  write_confirmed = 0;

  selected = 0;

  stop_feedback = 0;

  init_lcd_enc();


  int16_t result = load_settings();

  if (result < 0)  // failed to find settings in EEPROM, load default values
    {
      TM_HD44780_Puts(0, 1,"reading default");

      NR3.power_threshold_int = 75;
      NR3.alpha_int = 95;
      NR3.asnr_int = 30;
      NR3.width_int = 15;
      NR3.NR_enabled = 0;
      NR3.NB_enabled = 0;
      NR3.ka1 = 8;
      NR3.ka2 = 20;
      NR3.det_access = 0;
      NR3.a_corr = 10;
    }
  else
    TM_HD44780_Puts(0, 1,"use EEPROM data");


    int            n_samples_16K;
    int            n_samples_8K;


    start_codec_ugly();

    sm1000_leds_switches_init();

    /* Enable CRC clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

    /* Set up ADCs/DACs */

    //dac_open(4 * NR_FFT_SIZE);
    //adc_open(ADC_FS_16KHZ, 4 * NR_FFT_SIZE);


    dac_open(8 * NR_FFT_SIZE);   //von 4 auf 8 geändert aufgrund Halbierung NR_FFT_SIZE
    adc_open(ADC_FS_16KHZ, 8 * NR_FFT_SIZE);//     "


    n_samples_16K = 2 * NR_FFT_SIZE;  // von 1 auf 2 erhöht. Grund s.o.
    n_samples_8K = NR_FFT_SIZE;
    short  adc16k[OS_TAPS_16K + n_samples_16K];
    short  adc8k[n_samples_8K];
    short  dac16k[n_samples_16K];
    short  dac8k[OS_TAPS_8K + n_samples_8K];

    for (int p=0; p < OS_TAPS_16K; p++)   // filter taps löschen
	adc16k[p] = 0.0;
    for (int p=0; p < OS_TAPS_8K; p++)
    	dac8k[p] = 0.0;

    /* put outputs into a known state */

    led_pwr(1); led_ptt(0); led_rt(0); led_err(0); not_cptt(1);


   int32_t nr_active=0;



  // int button_count = 0;

   //init_lcd_enc();

    while(1) {

                /* ADC2 is the NF input (PA2), DAC1 is the NF Output (PA4) */

                if (adc2_read(&adc16k[OS_TAPS_16K], n_samples_16K) == 0) {

                    GPIOE->ODR = (1 << 3);  //HW test output

                    led_ptt(1);

                    /* clipping indicator */

                    led_err(0);
                    for (int i=0; i<n_samples_16K; i++) {
                        if (abs(adc16k[OS_TAPS_16K + i]) > 28000)
                            led_err(1);
                    }

                    fdmdv_16_to_8_short(adc8k,&adc16k[OS_TAPS_16K],n_samples_8K);//convert to internal 8ksamples / sec


  //                  nr_active = nr_on_state();
                   if (NR3.NR_enabled == 1) nr_active = 1;
		     else nr_active = 0;

		   // if (nr_active == 1) spectral_noise_reduction_3(adc16k); //takes a short, internal casting to float and back to short

		    spectral_noise_reduction_3(adc8k);

		    for (int p = 0; p < n_samples_8K;p++)
		      dac8k[OS_TAPS_8K + p] = adc8k[p];

		    fdmdv_8_to_16_short(dac16k, &dac8k[OS_TAPS_8K], n_samples_8K);//convert back to 16ks/sec for the external world


                    dac1_write(dac16k, n_samples_16K);
/*
		    if (nr_on()==1)
		    {
			button_count++;
			if (button_count>10)
			{
				if (nr_active == 1) nr_active = 0;
				else nr_active = 1;
				button_count=0;
			}
		    }
*/
                    led_rt(nr_active);
                    led_ptt(0);

                    GPIOE->ODR &= ~(1 << 3);

                    menu_handling();

                    if ((write_confirmed == 1) && (selected == 0))
                      {

			write_settings();
			TM_HD44780_Puts(0, 1,"*** WRITING ***");
			write_confirmed = 0;
			TM_HD44780_Puts(0, 0,"*** finished ***");
                      }
                }

    } /* while(1) ... */
}

void TM_EXTI_Handler(uint16_t GPIO_Pin) {
    /* Check RE pin 1 */
    if (GPIO_Pin == RE1_Data.GPIO_PIN_A) {
        /* Process data */
        TM_RE_Process(&RE1_Data);
    }

}
