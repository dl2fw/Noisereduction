

#include "arm_math.h"
#include "samp_rate_conv.h"




void fdmdv_8_to_16_short(short out16k[], short in8k[], int n)
{
    int32_t i,k,l;
    float32_t acc;



    for(i=0; i<n; i++) {
        acc = 0.0;
        for(k=0,l=0; k < OS_TAPS_16K; k+=RATE_OS,l++)
            acc += fdmdv_os_filter[k]*(float)in8k[i-l];
        out16k[i*RATE_OS] = RATE_OS*acc;

        acc = 0.0;
        for(k=1,l=0; k<OS_TAPS_16K; k+=RATE_OS,l++)
            acc += fdmdv_os_filter[k]*(float)in8k[i-l];
        out16k[i*RATE_OS+1] = RATE_OS*acc;
    }

    /* update filter memory */

    for(i=-(OS_TAPS_8K); i<0; i++)
	in8k[i] = in8k[i + n];

}



void fdmdv_16_to_8_short(short out8k[], short in16k[], int n)
{
    float32_t acc;
    int32_t i,j,k;

    for(i=0, k=0; k<n; i+=RATE_OS, k++) {
	acc = 0.0;
	for(j=0; j<OS_TAPS_16K; j++)
	    acc += fdmdv_os_filter[j]*(float)in16k[i-j];
        out8k[k] = acc;
    }

    /* update filter memory */

    for(i=-OS_TAPS_16K; i<0; i++)
	in16k[i] = in16k[i + n*RATE_OS];
}
