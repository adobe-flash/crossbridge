/* { dg-do compile } */
/* { dg-options "-O2" } */
void foo(void *pdst, short i, double *sp, 
	short xsize, short dorder, short strided) 
{ 
  unsigned short *dp = (unsigned short*)pdst + i*strided; 
  short j, k; 
  if (strided == xsize) 
    { 
      for (j = 0; j < (xsize << dorder); j++) 
	{ 
	  double x = sp[2*j]; 
	  if (x >= (32767 * 2 + 1)) 
	    x = (32767 * 2 + 1); 
	  if (x <= 0) x
	     = 0; 
	  dp[j] = (short) x; 
	} 
    } 
  else 
    { 
      for (k = 0; k < (1 << dorder); k++) 
	{ 
	  for (j = 0; j < xsize; j++)
	    { 
	      double x = sp[2*j]; 
	      if (x >= (32767 * 2 + 1)) 
		x = (32767 * 2 + 1); 
	      if (x <= 0) 
		x = 0; 
	      dp[j] = (short) x; 
	    } 
	  sp += 2*xsize; 
	  dp += strided; 
	} 
    } 
}
