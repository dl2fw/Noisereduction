
#include "audio_nr.h"
#include "stm32f4_adc.h"
#include "stm32f4_dac.h"

//#include "stm32f4_vrom.h

#include "sm1000_leds_switches.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stdlib.h>

#include "arm_math.h"
#include "tm_stm32f4_hd44780.h"
#include "tm_stm32f4_rotary_encoder.h"
#include "tm_stm32f4_exti.h"

 TM_RE_t RE1_Data;


//void spectral_noise_reduction_3 (short*);

int main(void) {

    int            n_samples_16k;
    int32_t	   nr_al = 0;

    /* Outgoing sample counter */

    /* init all the drivers for various peripherals */

    sm1000_leds_switches_init();

    /* Enable CRC clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, 1);

    /* Set up ADCs/DACs */


    dac_open(4 * NR_FFT_SIZE);
    adc_open(ADC_FS_16KHZ, 4 * NR_FFT_SIZE);


	n_samples_16k = NR_FFT_SIZE;

    short          adc16k[n_samples_16k];


    /* put outputs into a known state */

    led_pwr(1); led_ptt(0); led_rt(0); led_err(0); not_cptt(1);

    /* clear filter memories */

   //float32_t f_samples[n_samples_16k];

   int nr_active=0;
   int button_count = 0;

   char buf[15];

   TM_HD44780_Clear();
   TM_HD44780_Init(20, 4);
   TM_HD44780_Puts(0, 0, "   Noisereduction ");

   TM_RE_Init(&RE1_Data, GPIOC, GPIO_PIN_14, GPIOC, GPIO_PIN_15);
   TM_RE_SetMode(&RE1_Data, TM_RE_Mode_One);

    while(1) {

                /* ADC2 is the NF input (PA2), DAC1 is the NF Output (PA4) */

                if (adc2_read(adc16k, n_samples_16k) == 0) {

                    GPIOE->ODR = (1 << 3);

					led_ptt(1);

                    /* clipping indicator */

                    led_err(0);
                    for (int i=0; i<n_samples_16k; i++) {
                        if (abs(adc16k[i]) > 28000)
                            led_err(1);
                    }

					if (nr_active == 1) spectral_noise_reduction_3(adc16k, nr_al); //takes a short, internal casting to float and back to short



                    dac1_write(adc16k, n_samples_16k);

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

                    led_rt(nr_active);
					led_ptt(0);
                   // led_ptt(1); led_rt(0); led_err(0); not_cptt(0);
                    GPIOE->ODR &= ~(1 << 3);

                    TM_RE_Get(&RE1_Data);
                    if (RE1_Data.RE_Count > 100)  {
                	RE1_Data.RE_Count = 100;
                	RE1_Data.Absolute = 100;
                    }

                    if (RE1_Data.RE_Count < 0)  {
                      	RE1_Data.RE_Count = 0;
                      	RE1_Data.Absolute = 0;
                    }
		    nr_al=(int32_t)RE1_Data.Absolute;

                    sprintf(buf, "Reduction Level:%3d", RE1_Data.Absolute);

                    TM_HD44780_Puts(0, 2,buf);

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
