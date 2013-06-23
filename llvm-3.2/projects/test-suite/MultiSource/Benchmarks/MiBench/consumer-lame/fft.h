#ifndef FFT_H
#define FFT_H

#include "encoder.h"

void fft_long(FLOAT x_real[BLKSIZE], int, short **);
void fft_short(FLOAT x_real[3][BLKSIZE_s], int, short **);
void init_fft(void);

#endif
