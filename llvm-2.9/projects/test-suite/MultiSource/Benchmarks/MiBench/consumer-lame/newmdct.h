#ifndef MDCT_DOT_H
#define MDCT_DOT_H
void mdct_sub48(lame_global_flags *gfp,short *w0, short *w1,
	      FLOAT8 mdct_freq[2][2][576],
	      III_side_info_t *l3_side);
#endif
