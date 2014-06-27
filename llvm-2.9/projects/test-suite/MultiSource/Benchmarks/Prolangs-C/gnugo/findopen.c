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

extern unsigned char p[19][19], ma[19][19];
extern int mik, mjk;  /* piece captured */

int findopen(int m, int n, int i[], int j[], int color, int minlib, int *ct)
/* find all open spaces i, j from m, n */
{
/* mark this one */
 ma[m][n] = 1;

/* check North neighbor */
 if (m != 0)
   {
    if ((p[m - 1][n] == EMPTY) && (((m - 1) != mik) || (n != mjk)))
      {
       i[*ct] = m - 1;
       j[*ct] = n;
       ++*ct;
       if (*ct == minlib) return 1;
     }
    else
      if ((p[m - 1][n] == color) && !ma[m - 1][n])
	 if (findopen(m - 1, n, i, j, color, minlib, ct) && (*ct == minlib))
	    return 1;
  }

/* check South neighbor */
 if (m != 18)
   {
    if ((p[m + 1][n] == EMPTY) && (((m + 1) != mik) || (n != mjk)))
      {
       i[*ct] = m + 1;
       j[*ct] = n;
       ++*ct;
       if (*ct == minlib) return 1;
     }
    else
      if ((p[m + 1][n] == color) && !ma[m + 1][n])
	 if (findopen(m + 1, n, i, j, color, minlib, ct) && (*ct == minlib))
	    return 1;
  }

/* check West neighbor */
 if (n != 0)
   {
    if ((p[m][n - 1] == EMPTY) && ((m != mik) || ((n - 1) != mjk)))
      {
       i[*ct] = m;
       j[*ct] = n - 1;
       ++*ct;
       if (*ct == minlib) return 1;
     }
    else
      if ((p[m][n - 1] == color) && !ma[m][n - 1])
	 if (findopen(m, n - 1, i, j, color, minlib, ct) && (*ct == minlib))
	    return 1;
  }

/* check East neighbor */
 if (n != 18)
   {
    if ((p[m][n + 1] == EMPTY) && ((m != mik) || ((n + 1) != mjk)))
      {
       i[*ct] = m;
       j[*ct] = n + 1;
       ++*ct;
       if (*ct == minlib) return 1;
     }
    else
      if ((p[m][n + 1] == color) && !ma[m][n + 1])
	 if (findopen(m, n + 1, i, j, color, minlib, ct) && (*ct == minlib))
	    return 1;
  }

/* fail to find open space */
 return 0;
}  /* end findopen */
