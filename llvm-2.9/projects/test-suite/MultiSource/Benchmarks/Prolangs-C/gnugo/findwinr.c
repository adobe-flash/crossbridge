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

#define EMPTY 0

extern unsigned char p[19][19], l[19][19];
extern int mymove, umove;
extern int lib;

extern void initmark(void);
extern int findopen(int m, int n, int i[], int j[], int color, int minlib, int *ct);
extern void countlib(int m, int n, int color);

int findwinner(int *i, int *j, int *val)
/* find opponent piece to capture or attack */
{
 int m, n, ti[3], tj[3], tval, ct, u, v, lib1;

 *i = -1;   *j = -1;   *val = -1;

/* find opponent with liberty less than four */
 for (m = 0; m < 19; m++)
   for (n = 0; n < 19; n++)
     if ((p[m][n] == umove) && (l[m][n] < 4))
       {
	ct = 0;
	initmark();
	if (findopen(m, n, ti, tj, umove, l[m][n], &ct))
	  {
	   if (l[m][n] == 1)
	     {
	      if (*val < 120)
		{
		 *val = 120;
		 *i = ti[0];
		 *j = tj[0];
	       }
	    }
	   else
	     for (u = 0; u < l[m][n]; u++)
	       for (v = 0; v < l[m][n]; v++)
		  if (u != v)
		    {
		     lib = 0;
		     countlib(ti[u], tj[u], mymove);
		     if (lib > 0) /* valid move */
		       {
                        lib1 = lib;
			p[ti[u]][tj[u]] = mymove;
		    /* look ahead opponent move */
			lib = 0;
			countlib(ti[v], tj[v], umove);
			if ((lib1 == 1) && (lib > 0))
                          tval = 0;
                        else
                          tval = 120 - 20 * lib;
			if (*val < tval)
			  {
			   *val = tval;
			   *i = ti[u];
			   *j = tj[u];
			 }
			p[ti[u]][tj[u]] = EMPTY;
		      }
		   }
	 }
      }
 if (*val > 0)	/* find move */
    return 1;
 else  /* fail to find winner */
    return 0;
}  /* end findwinner */
