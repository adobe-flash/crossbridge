#ifndef L3PSY_DOT_H_
#define L3PSY_DOT_H_

/* l3psy.c */
#include "l3side.h"
void L3psycho_anal( lame_global_flags *gfp,
                    short int *buffer[2], int gr , 
		    FLOAT8 *ms_ratio, 
		    FLOAT8 *ms_ratio_next, 
		    FLOAT8 *ms_ener_ratio, 
		    III_psy_ratio ratio[2][2],
		    III_psy_ratio MS_ratio[2][2],
		    FLOAT8 pe[2], FLOAT8 pe_MS[2], 
                    int blocktype_d[2]); 
#endif
