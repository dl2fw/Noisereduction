#include "stm32f4xx.h"
#include "audio_nr.h"
#include "stm32f4_adc.h"
#include "stm32f4_dac.h"

//#include "stm32f4_vrom.h

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
#include "ui.h"




//void spectral_noise_reduction_3 (short*);




int main(void) {


    NR3.power_threshold_int = 75;
    NR3.alpha_int = 95;
    NR3.asnr_int = 30;
    NR3.width_int = 15;



    int            n_samples_16k;



    start_codec_ugly();

    sm1000_leds_switches_init();

    /* Enable CRC clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

    /* Set up ADCs/DACs */

    dac_open(4 * NR_FFT_SIZE);
    adc_open(ADC_FS_16KHZ, 4 * NR_FFT_SIZE);


    n_samples_16k = NR_FFT_SIZE;

    short  adc16k[n_samples_16k];


    /* put outputs into a known state */

    led_pwr(1); led_ptt(0); led_rt(0); led_err(0); not_cptt(1);


   int32_t nr_active=0;



  // int button_count = 0;

   init_lcd_enc();

    while(1) {

                /* ADC2 is the NF input (PA2), DAC1 is the NF Output (PA4) */

                if (adc2_read(adc16k, n_samples_16k) == 0) {

                    GPIOE->ODR = (1 << 3);  //HW test output

                    led_ptt(1);

                    /* clipping indicator */

                    led_err(0);
                    for (int i=0; i<n_samples_16k; i++) {
                        if (abs(adc16k[i]) > 28000)
                            led_err(1);
                    }

                    nr_active = nr_on_state();
//                    nr_active = 1;
		    if (nr_active == 1) spectral_noise_reduction_3(adc16k); //takes a short, internal casting to float and back to short

                    dac1_write(adc16k, n_samples_16k);
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
