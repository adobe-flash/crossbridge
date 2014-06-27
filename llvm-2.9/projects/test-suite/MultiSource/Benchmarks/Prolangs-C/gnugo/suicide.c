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
extern int uik, ujk;  /* piece captured */

extern void countlib(int m, int n, int color);
extern void eval(int color);

int suicide(int i, int j)
/* check for suicide move of opponent at p[i][j] */
{
 int m, n, k;

/* check liberty of new move */
 lib = 0;
 countlib(i, j, umove);
 if (lib == 0)
/* new move is suicide then check if kill my pieces and Ko possibility */
   {
/* assume alive */
    p[i][j] = umove;

/* check my pieces */
    eval(mymove);
    k = 0;

    for (m = 0; m < 19; m++)
      for (n = 0; n < 19; n++)
/* count pieces will be killed */
	if ((p[m][n] == mymove) && !l[m][n]) ++k;

    if ((k == 0) || (k == 1 && ((i == uik) && (j == ujk))))
/* either no effect on my pieces or an illegal Ko take back */
      {
       p[i][j] = EMPTY;   /* restore to open */
       return 1;
      }
    else
/* good move */
      return 0;
   }
 else
/* valid move */
   return 0;
}  /* end suicide */
