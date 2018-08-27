
#include "arm_math.h"

#ifndef _AUDIO_NR_H
#define _AUDIO_NR_H

#define NR_FFT_SIZE 512
#define NR_FFT_L_2 (NR_FFT_SIZE * 2)


typedef struct {
  int32_t Version;
  int32_t alpha_int;//0..100
  int32_t asnr_int; //0..30
  int32_t power_threshold_int;  //0..100
  int32_t width_int; // 2..50


} NoiseredPara;

NoiseredPara NR3;

void spectral_noise_reduction_3 (short*);

#endif //ifndef __AUDIO_NR_H
