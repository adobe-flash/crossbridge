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

extern unsigned char p[19][19], ml[19][19];
extern int lib;

void count(int i, int j, int color)
/* count liberty of color piece at i, j */
{
/* set current piece as marked */
 ml[i][j] = EMPTY;

/* check North neighbor */
 if (i != EMPTY)
   {
    if ((p[i - 1][j] == EMPTY) && ml[i - 1][j])
      {
       ++lib;
       ml[i - 1][j] = EMPTY;
     }
    else
       if ((p[i - 1][j] == color) && ml[i - 1][j])
	  count(i - 1, j, color);
  }
/* check South neighbor */
 if (i != 18)
   {
    if ((p[i + 1][j] == EMPTY) && ml[i + 1][j])
      {
       ++lib;
       ml[i + 1][j] = EMPTY;
     }
    else
       if ((p[i + 1][j] == color) && ml[i + 1][j])
	  count(i + 1, j, color);
  }
/* check West neighbor */
 if (j != EMPTY)
   {
    if ((p[i][j - 1] == EMPTY) && ml[i][j - 1])
      {
       ++lib;
       ml[i][j - 1] = EMPTY;
     }
    else
       if ((p[i][j - 1] == color) && ml[i][j - 1])
	  count(i, j - 1, color);
  }
/* check East neighbor */
 if (j != 18)
   {
    if ((p[i][j + 1] == EMPTY) && ml[i][j + 1])
      {
       ++lib;
       ml[i][j + 1] = EMPTY;
     }
    else
       if ((p[i][j + 1] == color) && ml[i][j + 1])
	  count(i, j + 1, color);
  }
}  /* end count */
