/*
                GNU GO - the game of Go (Wei-Chi)
                Version 1.1   last revised 3-1-89
           Copyright (C) Free Software Foundation, Inc.
                      written by Man L. Li
                      modified by Wayne Iba
                    documented by Bob Webber
*/
/*
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation - version 1.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License in file COPYING for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Please report any bug/fix, modification, suggestion to

mail address:   Man L. Li
                Dept. of Computer Science
                University of Houston
                4800 Calhoun Road
                Houston, TX 77004

e-mail address: manli@cs.uh.edu         (Internet)
                coscgbn@uhvax1.bitnet   (BITNET)
                70070,404               (CompuServe)



*/

#include <stdio.h>

extern unsigned char p[19][19], l[19][19];
extern int mymove;

extern void initmark(void);
extern int findnextmove(int m, int n, int *i, int *j, int *val, int minlib);

int findsaver(int *i, int *j, int *val)
/* find move if any pieces is threaten */
  {
   int m, n, minlib;
   int ti, tj, tval;

   *i = -1;   *j = -1;	 *val = -1;
   for (minlib = 1; minlib < 4; minlib++)
      {
/* count piece with minimum liberty */
       for (m = 0; m < 19; m++)
	 for (n = 0; n < 19; n++)
	   if ((p[m][n] == mymove) && (l[m][n] == minlib))
/* find move to save pieces */
	     {
	      initmark();
	      if (findnextmove(m, n, &ti, &tj, &tval, minlib) && (tval > *val))
		{
		 *val = tval;
		 *i = ti;
		 *j = tj;
	       }
	     }
     }
    if (*val > 0)   /* find move */
       return 1;
    else	    /* move not found */
       return 0;
 }  /* findsaver */
