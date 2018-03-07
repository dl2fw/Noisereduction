
#include "audio_nr.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "arm_common_tables.h"


void spectral_noise_reduction_3 (short* in_buffer)
{
////////////////////////////////////////////////////////////////////////////////////////

// Frank DD4WH & Michael DL2FW, November 2017
// NOISE REDUCTION BASED ON SPECTRAL SUBTRACTION
// following Romanin et al. 2009 on the basis of Ephraim & Malah 1984 and Hu et al. 2001
// detailed technical description of the implemented algorithm
// can be found in our WIKI
// https://github.com/df8oe/UHSDR/wiki/Noise-reduction
//
// half-overlapping input buffers (= overlap 50%)
// Hann window on 128 or 256 samples
// FFT128 - inverse FFT128 or FFT256 / iFFT256
// overlap-add

//const float32_t SQRT_von_Hann_256[256] = {0,0.01231966,0.024637449,0.036951499,0.049259941,0.061560906,0.073852527,0.086132939,0.098400278,0.110652682,0.122888291,0.135105247,0.147301698,0.159475791,0.171625679,0.183749518,0.195845467,0.207911691,0.219946358,0.231947641,0.24391372,0.255842778,0.267733003,0.279582593,0.291389747,0.303152674,0.314869589,0.326538713,0.338158275,0.349726511,0.361241666,0.372701992,0.384105749,0.395451207,0.406736643,0.417960345,0.429120609,0.440215741,0.451244057,0.462203884,0.473093557,0.483911424,0.494655843,0.505325184,0.515917826,0.526432163,0.536866598,0.547219547,0.557489439,0.567674716,0.577773831,0.587785252,0.597707459,0.607538946,0.617278221,0.626923806,0.636474236,0.645928062,0.65528385,0.664540179,0.673695644,0.682748855,0.691698439,0.700543038,0.709281308,0.717911923,0.726433574,0.734844967,0.743144825,0.75133189,0.759404917,0.767362681,0.775203976,0.78292761,0.790532412,0.798017227,0.805380919,0.812622371,0.819740483,0.826734175,0.833602385,0.840344072,0.846958211,0.853443799,0.859799851,0.866025404,0.872119511,0.878081248,0.88390971,0.889604013,0.895163291,0.900586702,0.905873422,0.911022649,0.916033601,0.920905518,0.92563766,0.930229309,0.934679767,0.938988361,0.943154434,0.947177357,0.951056516,0.954791325,0.958381215,0.961825643,0.965124085,0.968276041,0.971281032,0.974138602,0.976848318,0.979409768,0.981822563,0.984086337,0.986200747,0.988165472,0.989980213,0.991644696,0.993158666,0.994521895,0.995734176,0.996795325,0.99770518,0.998463604,0.999070481,0.99952572,0.99982925,0.999981027,0.999981027,0.99982925,0.99952572,0.999070481,0.998463604,0.99770518,0.996795325,0.995734176,0.994521895,0.993158666,0.991644696,0.989980213,0.988165472,0.986200747,0.984086337,0.981822563,0.979409768,0.976848318,0.974138602,0.971281032,0.968276041,0.965124085,0.961825643,0.958381215,0.954791325,0.951056516,0.947177357,0.943154434,0.938988361,0.934679767,0.930229309,0.92563766,0.920905518,0.916033601,0.911022649,0.905873422,0.900586702,0.895163291,0.889604013,0.88390971,0.878081248,0.872119511,0.866025404,0.859799851,0.853443799,0.846958211,0.840344072,0.833602385,0.826734175,0.819740483,0.812622371,0.805380919,0.798017227,0.790532412,0.78292761,0.775203976,0.767362681,0.759404917,0.75133189,0.743144825,0.734844967,0.726433574,0.717911923,0.709281308,0.700543038,0.691698439,0.682748855,0.673695644,0.664540179,0.65528385,0.645928062,0.636474236,0.626923806,0.617278221,0.607538946,0.597707459,0.587785252,0.577773831,0.567674716,0.557489439,0.547219547,0.536866598,0.526432163,0.515917826,0.505325184,0.494655843,0.483911424,0.473093557,0.462203884,0.451244057,0.440215741,0.429120609,0.417960345,0.406736643,0.395451207,0.384105749,0.372701992,0.361241666,0.349726511,0.338158275,0.326538713,0.314869589,0.303152674,0.291389747,0.279582593,0.267733003,0.255842778,0.24391372,0.231947641,0.219946358,0.207911691,0.195845467,0.183749518,0.171625679,0.159475791,0.147301698,0.135105247,0.122888291,0.110652682,0.098400278,0.086132939,0.073852527,0.061560906,0.049259941,0.036951499,0.024637449,0.01231966,0};

float32_t SQRT_von_Hann[NR_FFT_L_2];

static uint8_t NR_init_counter = 0;


//const float32_t tinc = 0.00533333; // frame time 5.3333ms
//const float32_t tax=0.071;	// noise output smoothing time constant - absolut value in seconds
//const float32_t tap=0.152;	// speech prob smoothing time constant  - absolut value in seconds
const float32_t psthr=0.99;	// threshold for smoothed speech probability [0.99]
const float32_t pnsaf=0.01;	// noise probability safety value [0.01]
const float32_t psini=0.5;	// initial speech probability [0.5]
const float32_t pspri=0.5;	// prior speech probability [0.5]

       float32_t nr_alpha = 0.995;


static float32_t 	last_iFFT_result [NR_FFT_L_2 / 2];
static float32_t 	last_sample_buffer_L [NR_FFT_L_2 / 2];
static float32_t 	Hk[NR_FFT_L_2 / 2]; // gain factors
       float32_t 	FFT_buffer[NR_FFT_L_2 * 2];
static float32_t 	Nest[NR_FFT_L_2 / 2]; // noise estimates for the current and the last FFT frame
static float32_t 	SNR_prio[NR_FFT_L_2 / 2];
static float32_t 	SNR_post[NR_FFT_L_2 / 2];
static float32_t 	Hk_old[NR_FFT_L_2 / 2];
static float32_t 	X[NR_FFT_L_2 / 2][2]; // magnitudes of the current and the last FFT bins

       float32_t	ax;
       float32_t	ap;
       float32_t	xih1;
       float32_t	xih1r;
       float32_t	pfac;

       float32_t	snr_prio_min;

       int16_t		NN;// for musical noise reduction
       int16_t		width = 15;// for musical noise reduction
       float32_t 	pre_power;// for musical noise reduction
       float32_t	post_power;// for musical noise reduction
       float32_t	power_ratio; // for musical noise reduction
       float32_t	power_threshold = (float32_t)NR3.power_threshold_int / 100.0;
       //int16_t						power_threshold_int;
       int16_t 		asnr = 30;

	static int16_t  	nr_first_time =1;





// for 12ksps and FFT128
//NR2.ax = 0.9276; 		//expf(-tinc / tax);
//NR2.ap = 0.9655; 		//expf(-tinc / tap);

// for 6ksps and FFT256
//ax = 0.7405;
//ap = 0.8691;

// for 16ksps and FFT1024
ax = 0.63720;
ap = 0.81020;



//NR2.xih1 = 31.62; 		//powf(10, (float32_t)NR2.asnr / 10.0);


asnr =	NR3.asnr_int;

xih1 = powf(10, (float32_t)asnr / 10.0);
xih1r = 1.0 / (1.0 + xih1) - 1.0;
pfac= (1.0 / pspri - 1.0) * (1.0 + xih1);
snr_prio_min = 0.001; 			//powf(10, - (float32_t)NR2.snr_prio_min_int / 10.0);  //range should be down to -30dB min

static float32_t pslp[NR_FFT_SIZE];
static float32_t xt[NR_FFT_SIZE];
float32_t xtr;
float32_t ph1y[NR_FFT_SIZE];





    if(nr_first_time == 1)
    { // TODO: properly initialize all the variables

	//NR3.power_threshold_int = 75;
	//NR3.alpha_int = 95;
	//NR3.asnr_int = 30;
	//NR3.width_int = 15;



		for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++)
			{
				  last_sample_buffer_L[bindx] = 0.0;
				  Hk[bindx] = 1.0;

				  Hk_old[bindx] = 1.0; // old gain or xu in development mode
				  Nest[bindx] = 0.0;

				  pslp[bindx] = 0.5;

			}

     //Calculating the FFT Window
		for(int bindx = 0; bindx < NR_FFT_L_2; bindx++)
			SQRT_von_Hann[bindx] = sqrtf(0.50*(1-cosf(2*3.1415926535*(float)bindx/(float)(NR_FFT_L_2 -1))));

        nr_first_time = 2; // we need to do some more a bit later down
    }

    for(int k = 0; k < 1; k++) // 1 for 256er FFT
    {
    // NR_FFT_buffer is 256 floats big
    // interleaved r, i, r, i . . .
    // fill first half of FFT_buffer with last events audio samples
          for(int i = 0; i < NR_FFT_L_2 / 2; i++)
          {
            FFT_buffer[i * 2] = last_sample_buffer_L[i]; // real
            FFT_buffer[i * 2 + 1] = 0.0; // imaginary
          }
    // copy recent samples to last_sample_buffer for next time!
          for(int i = 0; i < NR_FFT_L_2  / 2; i++)
          {
             last_sample_buffer_L [i] = (float32_t)in_buffer[i + k * (NR_FFT_L_2 / 2)];
          }
    // now fill recent audio samples into second half of FFT_buffer
          for(int i = 0; i < NR_FFT_L_2 / 2; i++)
          {
              FFT_buffer[NR_FFT_L_2 + i * 2] = (float32_t)in_buffer[i+ k * (NR_FFT_L_2 / 2)]; // real
              FFT_buffer[NR_FFT_L_2 + i * 2 + 1] = 0.0;
          }
    /////////////////////////////////7
    // WINDOWING


        	  for (int idx = 0; idx < NR_FFT_L_2; idx++)
              {
        	  	  FFT_buffer[idx * 2] *= SQRT_von_Hann[idx];
              }
              //arm_cfft_f32(&arm_cfft_sR_f32_len2048, FFT_buffer, 0, 1);
			 //arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_buffer, 0, 1);
			 arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_buffer, 0, 1);
    // NR_FFT
    // calculation is performed in-place the FFT_buffer [re, im, re, im, re, im . . .]



	  for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++)
		{
		  //here we need squared magnitude
			X[bindx][0] = (FFT_buffer[bindx * 2] * FFT_buffer[bindx * 2] + FFT_buffer[bindx * 2 + 1] * FFT_buffer[bindx * 2 + 1]);
		}

      if(nr_first_time == 2)
      {
 		  for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++)
		  {
			  Nest[bindx] = Nest[bindx] + 0.05* X[bindx][0];// we do it 20 times to average over 20 frames for app. 100ms only on NR_on/bandswitch/modeswitch,...
			  xt[bindx] = psini * Nest[bindx];
		  }
		  NR_init_counter++;
		  if (NR_init_counter > 19)//average over 20 frames for app. 100ms
		  {
			  NR_init_counter = 0;
			  nr_first_time = 3;  // now we did all the necessary initialization to actually start the noise reduction
		  }
      }
     if (nr_first_time == 3)
     {

	 nr_alpha = (float32_t)(NR3.alpha_int)/1000.0 + 0.899f;
	 power_threshold = (float32_t)(NR3.power_threshold_int) / 100.0;
	 asnr =	NR3.asnr_int;
	 width = NR3.width_int;




 //new noise estimate MMSE based!!!

		for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++)// 1. Step of NR - calculate the SNR's
    	{
		      ph1y[bindx] = 1.0 / (1.0 + pfac * expf(xih1r * X[bindx][0]/xt[bindx]));
		      pslp[bindx] = ap * pslp[bindx] + (1.0 - ap) * ph1y[bindx];
		      //ph1y[bindx] = fmin(ph1y[bindx], 1.0 - pnsaf * (pslp[bindx] > psthr)); //?????

		      if (pslp[bindx] > psthr)
		      {
		    	  ph1y[bindx] = 1.0 - pnsaf;
		      }
		      else
		      {
		    	  ph1y[bindx] = fmin(ph1y[bindx] , 1.0);
		      }
		      xtr = (1.0 - ph1y[bindx]) * X[bindx][0] + ph1y[bindx] * xt[bindx];
		      xt[bindx] = ax * xt[bindx] + (1.0 - ax) * xtr;
        }


	  	  for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++)// 1. Step of NR - calculate the SNR's
			 {
			   SNR_post[bindx] = fmax(fmin(X[bindx][0] / xt[bindx],1000.0), snr_prio_min); // limited to +30 /-15 dB, might be still too much of reduction, let's try it?

			   SNR_prio[bindx] = fmax(nr_alpha * Hk_old[bindx] + (1.0f - nr_alpha) * fmax(SNR_post[bindx] - 1.0f, 0.0f), 0.0f);
			 }


  	  // 4    calculate v = SNRprio(n, bin[i]) / (SNRprio(n, bin[i]) + 1) * SNRpost(n, bin[i]) (eq. 12 of Schmitt et al. 2002, eq. 9 of Romanin et al. 2009)
     //		   and calculate the HK's

		for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++)// maybe we should limit this to the signal containing bins (filtering!!)
		{
			  float32_t v = SNR_prio[bindx] * SNR_post[bindx] / (1.0 + SNR_prio[bindx]);

			  Hk[bindx] = fmax(1.0 / SNR_post[bindx] * sqrtf((0.7212 * v + v * v)),0.001); //limit HK's to 0.001'

			  Hk_old[bindx] = SNR_post[bindx] * Hk[bindx] * Hk[bindx]; //


			  /*if(!(ts.dsp_active & DSP_NR_ENABLE)) // if NR is not enabled (but notch is enabled !)
			  {
				  NR.Hk[bindx] = 1.0;
			  } */
		}
		// musical noise "artefact" reduction by dynamic averaging - depending on SNR ratio
		pre_power = 0.0;
		post_power = 0.0;
		for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++)
		{
		    pre_power += X[bindx][0];
		    post_power += Hk[bindx] * Hk[bindx]  * X[bindx][0];
		}

		power_ratio = post_power / pre_power;
		if (power_ratio > power_threshold)
		  {
		    power_ratio = 1.0;
		    NN = 1;
		  }
		else
		  {
		    NN = 1 + 2 * (int)(0.5 + width * (1.0 - power_ratio / power_threshold));
		  }

		for(int bindx = NN/2; bindx < NR_FFT_L_2 / 2 - NN/2; bindx++)
		  {
		    Nest[bindx] = 0.0;
		    for(int m = bindx - NN/2; m <= bindx + NN/2;m++)
		      {
			Nest[bindx] += Hk[m];
		      }
		    Nest[bindx] /= (float32_t)NN;
		  }

		// and now the edges - only going NN steps forward and taking the average
		// lower edge
		for(int bindx = 0; bindx < NN/2; bindx++)
		  {
		    Nest[bindx] = 0.0;
		    for(int m = bindx; m < (bindx + NN);m++)
		      {
			Nest[bindx] += Hk[m];
		      }
		    Nest[bindx] /= (float32_t)NN;
		 }

		// upper edge - only going NN steps backward and taking the average
		for(int bindx = NR_FFT_L_2 / 2 - NN; bindx < NR_FFT_L_2 /2 ; bindx++)
		  {
		    Nest[bindx] = 0.0;
		    for(int m = bindx; m > (bindx - NN); m--)
		      {
			Nest[bindx] += Hk[m];
		      }
		    Nest[bindx] /= (float32_t)NN;
		 }

		// end of edge treatment

		for(int bindx = NN/2; bindx < NR_FFT_L_2 / 2 - NN/2; bindx++)
		  {
		    Hk[bindx] = Nest[bindx];
		  }
// end of musical noise reduction
	}	//end of "if ts.nr_first_time == 3"


        // FINAL SPECTRAL WEIGHTING: Multiply current FFT results with NR_FFT_buffer for 64 bins with the 64 bin-specific gain factors
              // only do this for the bins inside the filter passband
              // if you do this for all the bins, you will get distorted audio: plopping !
              //              for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++) // plopping !!!!
                for(int bindx = 0; bindx < NR_FFT_L_2 / 2; bindx++) // no plopping
              {
                  FFT_buffer[bindx * 2] = 	FFT_buffer [bindx * 2] * Hk[bindx]; // real part
                  FFT_buffer[bindx * 2 + 1] = FFT_buffer [bindx * 2 + 1] * Hk[bindx]; // imag part
                  FFT_buffer[NR_FFT_L_2 * 2 - bindx * 2 - 2] = FFT_buffer[NR_FFT_L_2 * 2 - bindx * 2 - 2] * Hk[bindx]; // real part conjugate symmetric
                  FFT_buffer[NR_FFT_L_2 * 2 - bindx * 2 - 1] = FFT_buffer[NR_FFT_L_2 * 2 - bindx * 2 - 1] * Hk[bindx]; // imag part conjugate symmetric

              }

         /*****************************************************************
         * NOISE REDUCTION CODE ENDS HERE
         *****************************************************************/
// NR_iFFT
// & Window on exit!
          //arm_cfft_f32(&arm_cfft_sR_f32_len2048, FFT_buffer, 1, 1);
    	  //arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_buffer, 1, 1);
    	  arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_buffer, 1, 1);
    	  for (int idx = 0; idx < NR_FFT_L_2; idx++)
          {
    	  	  FFT_buffer[idx * 2] *= SQRT_von_Hann[idx];
          }

    // do the overlap & add
          for(int i = 0; i < NR_FFT_L_2 / 2; i++)
          { // take real part of first half of current iFFT result and add to 2nd half of last iFFT_result
        	  //              NR_output_audio_buffer[i + k * (NR_FFT_L_2 / 2)] = NR_FFT_buffer[i * 2] + NR_last_iFFT_result[i];
        	  in_buffer[i + k * (NR_FFT_L_2 / 2)] = (short)(FFT_buffer[i * 2] + last_iFFT_result[i]);
          }
          for(int i = 0; i < NR_FFT_L_2 / 2; i++)
          {
              last_iFFT_result[i] = FFT_buffer[NR_FFT_L_2 + i * 2];
          }
       // end of "for" loop which repeats the FFT_iFFT_chain two times !!!
    }

}

