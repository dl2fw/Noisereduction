
#include "arm_math.h"

#ifndef _AUDIO_NR_H
#define _AUDIO_NR_H

#define NR_FFT_SIZE 512  // vorher bei internal 16ks: 512
#define NR_FFT_L_2 (NR_FFT_SIZE * 2)


typedef struct {
  int32_t Version;
  int32_t alpha_int;//0..100
  int32_t asnr_int; //0..30
  int32_t power_threshold_int;  //0..100
  int32_t width_int; // 2..50
  int32_t NR_enabled;
  int32_t NB_enabled;
  int32_t ka1;
  int32_t ka2;
  int32_t det_access;
  int32_t a_corr;
} NoiseredPara;

NoiseredPara NR3;

void spectral_noise_reduction_3 (short*);
//void alt_noise_blanking(float*,int, int, float* );
//void det(int32_t, int32_t, float32_t*, int32_t*, int32_t, int32_t);
//void median (int32_t, float32_t*, float32_t*);
//void lpc_calc(float32_t*,int32_t,int32_t, float32_t*);
#endif //ifndef __AUDIO_NR_H

float32_t e1xb (float32_t);
