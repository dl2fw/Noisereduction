/*
 * audio_nb.h
 *
 *  Created on: 08.01.2019
 *      Author: dl2fw
 */

#include "arm_math.h"

#ifndef AUDIO_NB_H_
#define AUDIO_NB_H_



void ATAc0 (int32_t, int32_t, float32_t*, float32_t*);
void multA1TA2(float32_t*, float32_t*, int32_t, int32_t, int32_t, float32_t*);
void multXKE(float32_t*, float32_t*, int32_t, int32_t, int32_t, float32_t*);
void multAv(float32_t*, float32_t*, int32_t, int32_t, float32_t*);
void xHat(int32_t, int32_t, float32_t*, float32_t*, float32_t*);
void invf(int32_t, int32_t, float32_t*, float32_t*, float32_t*);
void det(int32_t, int32_t, float32_t*, int32_t*, int32_t, int32_t);
int scanFrame(int32_t, int32_t, float32_t, int32_t*, int32_t*, int32_t*,int32_t*, int32_t*, int32_t*, int32_t*);
void execFrame(float32_t*, int32_t, int32_t,int32_t,float32_t);
void dR (int32_t, float32_t*, float32_t*);
void trI (int32_t, float32_t*, float32_t*);
void asolve(int32_t, int32_t, float32_t*, float32_t*);
void median (int32_t, float32_t*, float32_t*);

void lpc_calc(float32_t*,int32_t,int32_t, float32_t*);






#endif /* AUDIO_NB_H_ */
