#include "audio_nr.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "arm_common_tables.h"
#include "tm_stm32f4_hd44780.h"
#include "ui.h"
#include "stdio.h"
#include "audio_nb.h"
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


float32_t SQRT_von_Hann[NR_FFT_L_2 / ovrlp];


//const float32_t tinc = 0.00533333; // frame time 5.3333ms

static float32_t 	last_iFFT_result [NR_FFT_SIZE];
static float32_t 	last_sample_buffer_L [NR_FFT_SIZE];
static float32_t 	Hk[NR_FFT_SIZE]; // gain factors
       float32_t 	FFT_buffer[NR_FFT_SIZE * 4];
static float32_t 	X[NR_FFT_SIZE]; // magnitudes of the current FFT bins

static 	int16_t  	nr_first_time =1;



	uint8_t 	vox_det = 1;


    if(nr_first_time == 1)
    { // TODO: properly initialize all the variables


      for(int bindx = 0; bindx < NR_FFT_SIZE; bindx++)
	{
	  last_sample_buffer_L[bindx] = 0.0;
	  Hk[bindx] = 1.0;
	}

     //Calculating the FFT Window

	float32_t corr_sum =0;
	float32_t corr_gain;

		for(int bindx = 0; bindx < NR_FFT_L_2; bindx++)
		  {
		    //SQRT_von_Hann[bindx] = sqrtf(0.50*(1-cosf(2*3.1415926535*(float)bindx/(float)(NR_FFT_L_2 -1))));
		    SQRT_von_Hann[bindx] = sqrtf(0.54-0.46*cosf(2*3.1415926535*(float)bindx/(float)(NR_FFT_L_2 -1)));
		    corr_sum += SQRT_von_Hann[bindx];

		  }
		corr_gain = corr_sum / NR_FFT_L_2;
		for(int bindx = 0; bindx < NR_FFT_L_2; bindx++)
		  SQRT_von_Hann[bindx] *= corr_gain;

		nr_first_time = 2; // we need to do some more a bit later down
    }

    for(int k = 0; k < 1; k++) // 1 for 256er FFT
    {
    // NR_FFT_buffer is 256 floats big
    // interleaved r, i, r, i . . .
    // fill first half of FFT_buffer with last events audio samples
          for(int i = 0; i < NR_FFT_SIZE; i++)
          {
            FFT_buffer[i * 2] = last_sample_buffer_L[i]; // real
            FFT_buffer[i * 2 + 1] = 0.0; // imaginary
          }
    // copy recent samples to last_sample_buffer for next time!
          for(int i = 0; i < NR_FFT_SIZE; i++)
          {
             last_sample_buffer_L [i] = (float32_t)in_buffer[i + k * (NR_FFT_SIZE)];
          }
    // now fill recent audio samples into second half of FFT_buffer
          for(int i = 0; i < NR_FFT_SIZE; i++)
          {
              FFT_buffer[NR_FFT_L_2 + i * 2] = (float32_t)in_buffer[i+ k * (NR_FFT_SIZE)]; // real
              FFT_buffer[NR_FFT_L_2 + i * 2 + 1] = 0.0;
          }
    /////////////////////////////////
       //******   Here is a good place to call our noiseblanker
       //******   as we have 2 consecutive Frames (2048 Samples) available in one buffer
       //******   the Noiseblanker would work twice on all samples and we don't have any border-problems
    //////////////////////////////

	//***************************************************
	//  if (NR3.NB_enabled) alt_noise_blanking(FFT_buffer,NR_FFT_L_2, 32,&Energy_dummy );

          if (NR3.NB_enabled)
            execFrame(FFT_buffer, 2 * NR_FFT_SIZE, 32, 1, 0.5);
	//               source, size, order der lpc, passes, pmultmin

	//******************************************************

    // WINDOWING


        	  for (int idx = 0; idx < NR_FFT_L_2; idx++)
              {
        	  	  FFT_buffer[idx * 2] *= SQRT_von_Hann[idx];
              }
              //arm_cfft_f32(&arm_cfft_sR_f32_len2048, FFT_buffer, 0, 1);
			 //arm_cfft_f32(&arm_cfft_sR_f32_len256, FFT_buffer, 0, 1);
			 arm_cfft_f32(&arm_cfft_sR_f32_len1024, FFT_buffer, 0, 1);
			// arm_cfft_f32(&arm_cfft_sR_f32_len512, FFT_buffer, 0, 1);
    // NR_FFT
    // calculation is performed in-place the FFT_buffer [re, im, re, im, re, im . . .]




	  for(int bindx = 0; bindx < NR_FFT_SIZE; bindx++)
		{
		  // calculate the squared magnitudes for all the further calculations
			X[bindx] = (FFT_buffer[bindx * 2] * FFT_buffer[bindx * 2] + FFT_buffer[bindx * 2 + 1] * FFT_buffer[bindx * 2 + 1]);
		}

	 vox_det = do_vox(X);  //vox function - vox_detect = 1 means "audio present, no PTT" to freeze the gain calculation


	if (get_menu_pos()==6)
	  {
		  show_spectrum(Hk,X);
	  }


      if(nr_first_time == 2)
      {

		  nr_first_time = 3;  // now we did all the necessary initialization to actually start the noise reduction

      }
     if ((nr_first_time == 3) && (vox_det > 0))
     {


	 gain_calc(X,Hk);
	 musical_noise_reduction(X,Hk);


	}	//end of "if ts.nr_first_time == 3"

    if (!NR3.NR_enabled) //if NR not enabled set the HK's to 1.0
      {
	for(int bindx = 0; bindx < NR_FFT_SIZE; bindx++)
	  Hk[bindx]=1.0;
      }

	    // FINAL SPECTRAL WEIGHTING: Multiply current FFT results with  bin-specific gain factors

    for(int bindx = 0; bindx < NR_FFT_SIZE; bindx++)
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
    //  arm_cfft_f32(&arm_cfft_sR_f32_len512, FFT_buffer, 1, 1);


      for (int idx = 0; idx < NR_FFT_L_2; idx++)
		{
		 FFT_buffer[idx * 2] *= SQRT_von_Hann[idx];
		}

	// do the overlap & add
      for(int i = 0; i < NR_FFT_SIZE; i++)
		{ // take real part of first half of current iFFT result and add to 2nd half of last iFFT_result
		//              NR_output_audio_buffer[i + k * (NR_FFT_L_2 / 2)] = NR_FFT_buffer[i * 2] + NR_last_iFFT_result[i];
		in_buffer[i + k * (NR_FFT_SIZE)] = (short)(FFT_buffer[i * 2] + last_iFFT_result[i]);
		}
      for(int i = 0; i < NR_FFT_SIZE; i++)
		{
	    last_iFFT_result[i] = FFT_buffer[NR_FFT_L_2 + i * 2];
		}
       // end of "for" loop which repeats the FFT_iFFT_chain two times !!!
    }

}




float32_t e1xb (float32_t x)
{
	float32_t e1, ga, r, t, t0;
	int32_t k, m;
	if (x == 0.0)
		e1 = 1.0e30;
	else if (x <= 1.0)
	{
        e1 = 1.0;
        r = 1.0;

        for (k = 1; k <= 25; k++)
		{
			r = -r * k * x / ((k + 1.0)*(k + 1.0));
			e1 = e1 + r;
			if ( fabs (r) <= fabs (e1) * 1.0e-15f )
				break;
        }

        ga = 0.5772156649015328;
        e1 = - ga - logf (x) + x * e1;
	}
      else
	{
        m = 20 + (int32_t)(80.0 / x);
        t0 = 0.0;
        for (k = m; k >= 1; k--)
			t0 = (float32_t)k / (1.0 + k / (x + t0));
        t = 1.0 / (x + t0);
        e1 = expf (- x) * t;
	}
    return e1;
}

void musical_noise_reduction(float32_t* X, float32_t* Hk)
{
	// musical noise "artefact" reduction by dynamic averaging - depending on SNR ratio

	 float32_t 	Nest[NR_FFT_SIZE]; // averaged Hk's
	 float32_t 	pre_power = 0.0;
     float32_t  post_power = 0.0;
     float32_t 	power_ratio =0.0;
	 int16_t	width = 15;// for musical noise reduction
	 float32_t	power_threshold = (float32_t)NR3.power_threshold_int / 100.0;
	 int16_t		NN;// for musical noise reduction


	 width = NR3.width_int;

	 for(int bindx = 0; bindx < NR_FFT_SIZE; bindx++)
      {
	  pre_power += X[bindx];
	  post_power += Hk[bindx] * Hk[bindx]  * X[bindx];
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

      for(int bindx = NN/2; bindx < NR_FFT_SIZE - NN/2; bindx++)
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
      for(int bindx = NR_FFT_SIZE - NN; bindx < NR_FFT_SIZE ; bindx++)
	{
	  Nest[bindx] = 0.0;
	  for(int m = bindx; m > (bindx - NN); m--)
	    {
	      Nest[bindx] += Hk[m];
	    }
	  Nest[bindx] /= (float32_t)NN;
       }

      // end of edge treatment

      for(int bindx = NN/2; bindx < NR_FFT_SIZE - NN/2; bindx++)
	{
	  Hk[bindx] = Nest[bindx];
	}

}

int16_t do_vox(float32_t* X)

 //vox function - vox_detect = 1 means "audio present, no PTT" hence freeze the gain calculation


{


 static uint8_t 	vox_was = 1;
	float32_t 	sig_power = 0.0;
	int16_t 	vox_det_int = 1;
	char 		buf[16];

	  for(int bindx = 0; bindx < NR_FFT_SIZE; bindx++)
			if (bindx > 20) sig_power += X[bindx];  //calculate the signalpower for the higher spectral bins to do a vox-function

	  if (sig_power > 1000000001)
	    {
	      vox_det_int = 1; //value is experimental, has to be adjustable?
	      if (vox_was == 0) // erase freeze message
			{
			  sprintf(buf, "                ");
			  TM_HD44780_Puts(0, 0,buf);
			}

	      vox_was = 1;
	    }
	  else
	    {
	      vox_det_int = 0;
	      sprintf(buf, "*****freeze*****");
	      TM_HD44780_Puts(0, 0,buf);

	      vox_was = 0;
	    }

	return vox_det_int;

}


void show_spectrum(float32_t* X, float32_t* Hk)
{
	float32_t spectrum[20];
	float32_t spec_part[25];
	float32_t HK_display[20];
	uint32_t  max_posi;
	uint32_t  int_spec[20];

	for (int s=0; s < 16; s++)  // but we have only 16 columns at our display
	    {
	      for (int q=0; q<11; q++) // so we take from 11 bins the maximum to display
			spec_part[q] = X[s*11 + q + 11];  // 11*16=240, almost all :-), leave out the lower 12

	      arm_max_f32(&spec_part[0],11,&spectrum[s],&max_posi); // take the maximum
	      int_spec[s] = (int)(spectrum[s] / 3200000000);  // scale it fixed - later adaptive ???
	      if (int_spec[s] > 7) int_spec[s] = 7; //limit it to 7
	      TM_HD44780_PutCustom(s,0, int_spec[s]);  //plot it to our display

	      for (int q=0; q<15; q++)   //now the same for our HK's - the reduction factors
	      		spec_part[q] = Hk[s*11 + q + 11];  // use the same variable ... misleading

	      arm_min_f32(&spec_part[0],11,&HK_display[s],&max_posi); // here we take the lowest factor
	      int_spec[s] = (int)(HK_display[s] * 7.0); // HK's are between 0 and 1.0 ??
	      if (int_spec[s] > 7) int_spec[s] = 7; //limit to 7
	      TM_HD44780_PutCustom(s,1, int_spec[s]); //plot it to our display in the 2nd row

	    }
}

void gain_calc(float32_t* X, float32_t* Hk)

{
static int16_t 		init_done = 0;
static float32_t 	pslp[NR_FFT_SIZE];
static float32_t 	xt[NR_FFT_SIZE];
	   float32_t 	xtr;
	   float32_t 	ph1y[NR_FFT_SIZE];
	   float32_t	ax;
       float32_t	ap;
       float32_t	xih1;
       float32_t	xih1r;
       float32_t	pfac;
       float32_t	snr_prio_min;
       int16_t 		asnr = 30;
static float32_t 	SNR_prio[NR_FFT_SIZE];
static float32_t 	SNR_post[NR_FFT_SIZE];
const float32_t 	tax = 0.071;	// noise output smoothing time constant - absolut value in seconds
const float32_t 	tap = 0.152;	// speech prob smoothing time constant  - absolut value in seconds
const float32_t 	psthr = 0.99;	// threshold for smoothed speech probability [0.99]
const float32_t 	pnsaf = 0.01;	// noise probability safety value [0.01]
const float32_t 	psini = 0.5;	// initial speech probability [0.5]
const float32_t 	pspri = 0.5;	// prior speech probability [0.5]
	   float32_t 	nr_alpha = 0.995;
static float32_t 	Hk_old[NR_FFT_SIZE];
       float32_t 	a_corr = 1.0;
       float32_t 	v = 1.0;
// for 12ksps and FFT1282
//NR2.ax = 0.9276; 		//expf(-tinc / tax);
//NR2.ap = 0.9655; 		//expf(-tinc / tap);
// for 6ksps and FFT256
//ax = 0.7405;
//ap = 0.8691;
// for 16ksps and FFT1024
//ax = 0.63720;
//ap = 0.81020;
//for 8ksps and FFT512 same as above (same frame rate!!)
// for 8ksps and FFT1024
ax = 0.406;
ap = 0.6564;

asnr =	NR3.asnr_int;
xih1 = powf(10, (float32_t)asnr / 10.0);
xih1r = 1.0 / (1.0 + xih1) - 1.0;
pfac= (1.0 / pspri - 1.0) * (1.0 + xih1);
snr_prio_min = 0.001; 			//powf(10, - (float32_t)NR2.snr_prio_min_int / 10.0);  //range should be down to -30dB min




	 nr_alpha = (float32_t)(NR3.alpha_int)/1000.0 + 0.899f;
	 asnr =	NR3.asnr_int;

	 ax = 0.406;
	 ap = 0.6564;

	 a_corr = (float32_t)NR3.a_corr / 10.0f;
	 ax=expf(-a_corr * NR_FFT_SIZE/8000/tax);  //tax is a timeconstant of 71ms
	 ap=expf(-a_corr * NR_FFT_SIZE/8000/tap);  //tap is a timeconstant of 152ms

	 if (init_done < 1)
	   {
	     for (int bindx = 0; bindx < NR_FFT_SIZE; bindx++)
	       {
		 Hk_old[bindx] = 1.0; // old gain or xu in development mode
		 pslp[bindx] = 0.5;
		 xt[bindx] = 0.5;
		 init_done = 1;
	       }
	   }


	for(int bindx = 0; bindx < NR_FFT_SIZE; bindx++)// 1. Step of NR - calculate the SNR's
    	{
		  ph1y[bindx] = 1.0 / (1.0 + pfac * expf(xih1r * X[bindx]/xt[bindx]));
		  pslp[bindx] = ap * pslp[bindx] + (1.0 - ap) * ph1y[bindx];

		  if (pslp[bindx] > psthr)
		  {
			  ph1y[bindx] = 1.0 - pnsaf;
		  }
		  else
		  {
			  ph1y[bindx] = fmin(ph1y[bindx] , 1.0);
		  }
		  xtr = (1.0 - ph1y[bindx]) * X[bindx] + ph1y[bindx] * xt[bindx];
		  xt[bindx] = ax * xt[bindx] + (1.0 - ax) * xtr;
        }


      for(int bindx = 0; bindx < NR_FFT_SIZE; bindx++)// 1. Step of NR - calculate the SNR's
       {
		 SNR_post[bindx] = fmax(fmin(X[bindx] / xt[bindx],1000.0), snr_prio_min); // limited to +30 /-15 dB, might be still too much of reduction, let's try it?

		 if(NR3.det_access < 5)  //switch between old (<5) and Warren's >=5
		   SNR_prio[bindx] = fmax(nr_alpha * Hk_old[bindx] + (1.0f - nr_alpha) * fmax(SNR_post[bindx] - 1.0f, 0.0f), 0.0f);
		 else
		   SNR_prio[bindx] = nr_alpha * Hk_old[bindx] + (1.0f - nr_alpha) * fmax(SNR_post[bindx] - 1.0f, 1.0e-30f); // bei Warren
  	  // 4    calculate v = SNRprio(n, bin[i]) / (SNRprio(n, bin[i]) + 1) * SNRpost(n, bin[i]) (eq. 12 of Schmitt et al. 2002, eq. 9 of Romanin et al. 2009)
	  //		   and calculate the HK's

		v = SNR_prio[bindx] * SNR_post[bindx] / (1.0 + SNR_prio[bindx]);

		if(NR3.det_access < 5)
		  {
			Hk[bindx] = fmin(fmax(1.0 / SNR_post[bindx] * sqrtf((0.7212 * v + v * v)),0.0001),10000); //limit HK's to 0.001' and calculate bei an approximation
		  }
		else //warren's approach:
		  {
			Hk[bindx] = fmin(fmax(SNR_prio[bindx] / (1 + SNR_prio[bindx]) * expf(fmin(70.0,0.5 * e1xb(v))),0.0001),10000); // calculate the  Exponential Integral most accurate
		  }
		Hk_old[bindx] = SNR_post[bindx] * Hk[bindx] * Hk[bindx]; //

       }

}


//alt noise blanking is trying to localize some impulse noise within the samples and after that
//trying to replace corrupted samples by linear predicted samples.
//therefore, first we calculate the lpc coefficients which represent the actual status of the
//speech or sound generating "instrument" (in case of speech this is an estimation of the current
//filter-function of the voice generating tract behind our lips :-) )
//after finding this function we inverse filter the actual samples by this function
//so we are eliminating the speech, but not the noise. Then we do a matched filtering an thereby detecting impulses
//After that we threshold the remaining samples by some
//level and so detecting impulse noise's positions within the current frame - if one (or more) impulses are there.
//finally some area around the impulse position will be replaced by predicted samples from both sides (forward and
//backward prediction)
//hopefully we have enough processor power left....


/*

void alt_noise_blanking(float* insamp,int Nsam, int order, float* E )  //Nsam = 128
{

#define boundary_blank 		14 // for first trials very large!!!!
#define impulse_length 		15 // has to be odd!!!! 7 / 3 should be enough
#define PL             		7 // has to be (impulse_length-1)/2 !!!!
#define MAX_PULSE_N		50
#define MAX_PULSE_LENGTH	50
#define MIN_ORDER		10
#define MIN_PULSE_LENGTH	5
//#define order         10 // lpc's order
    arm_fir_instance_f32 LPC;
    float32_t lpcs[order+1]; // we reserve one more than "order" because of a leading "1"
    float32_t reverse_lpcs[order+1]; //this takes the reversed order lpc coefficients
    float32_t firStateF32[Nsam + order];
    float32_t tempsamp[Nsam];

    //static float32_t last_frame_end[order+PL]; //this takes the last samples from the previous frame to do the prediction within the boundaries

    static int32_t frame_count=0;  //only used for the distortion insertion - can alter be deleted


    //float32_t R[order+1];  // takes the autocorrelation results
    //float32_t k,alfa;

    //float32_t any[order+1];  //some internal buffers for the levinson durben algorithm
    float32_t Rfw[impulse_length+order]; // takes the forward predicted audio restauration Nsam is much too big!!!
    float32_t Rbw[impulse_length+order]; // takes the backward predicted audio restauration
    float32_t Wfw[impulse_length],Wbw[impulse_length]; // taking linear windows for the combination of fwd and bwd

    //float32_t s;
    char buf[20];
    int32_t det_output[1024];
    int32_t imp_start[1024];
    int32_t imp_length[1024]; // much too long, has to be reduced
    int32_t was_impulse = 0;
    int32_t pulse_count = 0;


    float32_t working_buffer[1024]; //here we have to extract the real samples from our insamp array
    								      //necessary to watch for impulses as close to the frame boundaries as possible


    if (frame_count > 10)     // insert a distorting pulse
            {
		//insamp[500]=insamp[500] + 9000;
		//insamp[502]=insamp[502] + 9000;
		//insamp[504]=insamp[504] + 9000;
		///insamp[506]=insamp[506] + 9000;
		insamp[508]=insamp[508] + 900;
                insamp[510]=insamp[510] + 3000; // overlaying a short  distortion pulse +/-
                insamp[512]=insamp[512] + 3000;
                insamp[514]=insamp[514] - 1500; // overlaying a short  distortion pulse +/-
                insamp[516]=insamp[516] - 1500;
                insamp[518]=insamp[518] - 900;
                //insamp[520]=insamp[520] - 900;
                //insamp[522]=insamp[522] - 9000;
                //insamp[524]=insamp[524] - 9000;
                //insamp[526]=insamp[526] - 9000;
                //insamp[528]=insamp[528] - 9000;
                //insamp[530]=insamp[530] - 9000;

            }

        frame_count++;
        if (frame_count > 11) frame_count=0;


    //*****************************end of debug impulse generation

    for (int i=0; i< Nsam; i++) working_buffer[i] = insamp[2 * i];// extract the real samples from the complex insamp array

    lpc_calc(&working_buffer[0],Nsam,order,&lpcs[0]);

    for (int o = 0; o < order+1; o++ )             //store the reverse order coefficients separately
        reverse_lpcs[order-o]=lpcs[o];        // for the matched impulse filter

    arm_fir_init_f32(&LPC,order+1,&reverse_lpcs[0],&firStateF32[0],Nsam);// we are using the same function as used in freedv

    //arm_fir_f32(&LPC,insamp,tempsamp,Nsam); //do the inverse filtering to eliminate voice and enhance the impulses

    arm_fir_f32(&LPC,&working_buffer[0],tempsamp,Nsam); //do the inverse filtering to eliminate voice and enhance the impulses

    det(Nsam, order, tempsamp, det_output,NR3.ka1,NR3.ka2);

    for (int w=0;w<Nsam;w++)
      {
	imp_start[w]=0;
	imp_length[w]=0;
      }

    pulse_count = 0;

	for (int v = order;v < Nsam;v++)
	  {
	   if (det_output[v] == 1)
	     {
	       if (was_impulse == 0) //it is a new impulse
		 {
		   imp_start[pulse_count] = v; //store the start position
		   was_impulse = 1; //remember, that there was already an impulse
		   imp_length[pulse_count]++; //increase the length
		 }
	       else
		 if (was_impulse == 1) //the impulse is continuing
		   {
		     imp_length[pulse_count]++; // increase the length
		   }
	       }
	   else if (was_impulse == 1)
	     {
	       was_impulse = 0;// at the end of every impulse increase the pulse_counter
	       pulse_count++;
	     }
	   else
	     {
	       was_impulse = 0; // just for safety
	     }
	   if (pulse_count > (MAX_PULSE_N - 1)) pulse_count = MAX_PULSE_N - 1;
	   }
      stop_feedback = 1;


      for(int y=0;y < pulse_count;y++)  // we need those just to display them
	{
	  pulse_position[y]=(imp_start[y] + (imp_length[y] / 2) ); //center position
	  pulse_length[y]=imp_length[y];
	}


    arm_negate_f32(&lpcs[1],&lpcs[1],order);
    arm_negate_f32(&reverse_lpcs[0],&reverse_lpcs[0],order);


    for (int j=0; j < pulse_count; j++)
    {
	//set all pulses to Zero within pulseduration
	for(int z=imp_start[j];z<(imp_start[j]+imp_length[j]);z++)
	  working_buffer[z]=0.0;
	//***********************************************


	//       Impulse shouldn't be too long, and away from the boundaries
	if ((imp_length[j] < 150) &&(imp_start[j] > order) && ((imp_start[j]+imp_length[j]+order) < Nsam)) //do this only, if we are not close to the start
	  {
        for (int k = 0; k<order; k++)   // we have to copy some samples from the original signal as
        {                           // basis for the reconstructions - could be done by memcopy

            Rfw[k]=working_buffer[imp_start[j] - order + k];

        }

        for (int i = 0; i < pulse_length[j]; i++) //now we calculate the forward and backward predictions
        {
            arm_dot_prod_f32(&reverse_lpcs[0],&Rfw[i],order,&Rfw[i+order]);
        }
        for (int c=0;c<pulse_length[j];c++)// now let's correct the samples in the buffer
          //working_buffer[imp_start[j]-order+c+det_access]=Rfw[order+c];
        working_buffer[imp_start[j]+c+NR3.det_access]=Rfw[order+c];


    }
    }

    for (int i=0; i< Nsam; i++)
      {
	insamp[2 * i] = working_buffer[i]; // copy the samples back to the complex insamp array

      }


}








void lpc_calc(float32_t* wb,int32_t ns,int32_t ord, float32_t* lpcs)
{


      float32_t R[ord+1];  // takes the autocorrelation results
      float32_t k,alfa,s;

      float32_t any[ord+1];  //some internal buffers for the levinson durben algorithm
  // calculate the autocorrelation of insamp (moving by max. of #order# samples)
  for(int i=0; i < (ord+1); i++)
  {

	arm_dot_prod_f32(&wb[0],&wb[i],ns-i,&R[i]); // R is carrying the crosscorrelations
  }
  // end of autocorrelation



  //alternative levinson durben algorithm to calculate the lpc coefficients from the crosscorrelation

  R[0] = R[0] * (1.0 + 1.0e-9);

  lpcs[0] = 1;   //set lpc 0 to 1

  for (int i=1; i < ord+1; i++)
      lpcs[i]=0;                      // fill rest of array with zeros - could be done by memfill

  alfa = R[0];

  for (int m = 1; m <= ord; m++)
  {
      s = 0.0;
      for (int u = 1; u < m; u++)
          s = s + lpcs[u] * R[m-u];

      k = -(R[m] + s) / alfa;

      for (int v = 1;v < m; v++)
          any[v] = lpcs[v] + k * lpcs[m-v];

      for (int w = 1; w < m; w++)
          lpcs[w] = any[w];

      lpcs[m] = k;
      alfa = alfa * (1 - k * k);
  }

  // end of levinson durben algorithm

}


*/
