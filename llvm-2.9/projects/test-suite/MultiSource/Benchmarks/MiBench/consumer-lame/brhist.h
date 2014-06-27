#ifdef BRHIST
#ifndef BRHIST_H_INCLUDED
#define BRHIST_H_INCLUDED

extern int disp_brhist;

#include "lame.h"
void brhist_init(lame_global_flags *gfp,int br_min, int br_max);
void brhist_add_count(void);
void brhist_disp(void);
void brhist_disp_total(lame_global_flags *gfp);
extern long brhist_temp[15];
#endif

#endif
