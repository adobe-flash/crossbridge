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

extern unsigned char p[19][19];
extern int mymove;

int fioe(int i, int j)
{
/* check top edge */
 if (i == 0)
   {
    if ((j == 0) && ((p[1][0] == mymove) && (p[0][1] == mymove))) return 1;
    if ((j == 18) && ((p[1][18] == mymove) && (p[0][17] == mymove))) return 1;
    if ((p[1][j] == mymove) &&
	((p[0][j - 1] == mymove) && (p[0][j + 1] == mymove))) return 1;
    else
       return 0;
  }
/* check bottom edge */
 if (i == 18)
   {
    if ((j == 0) && ((p[17][0] == mymove) && (p[18][1] == mymove))) return 1;
    if ((j == 18) && ((p[17][18] == mymove) && (p[18][17] == mymove))) return 1;
    if ((p[17][j] == mymove) &&
	((p[18][j - 1] == mymove) && (p[18][j + 1] == mymove)))
       return 1;
    else
       return 0;
  }
/* check left edge */
 if (j == 0)
    if ((p[i][1] == mymove) &&
	((p[i - 1] [0] == mymove) && (p[i + 1][0] == mymove)))
       return 1;
    else
       return 0;
/* check right edge */
 if (j == 18)
    if ((p[i][17] == mymove) &&
	((p[i - 1] [18] == mymove) && (p[i + 1][18] == mymove)))
       return 1;
    else
       return 0;
/* check center pieces */
 if (((p[i][j - 1] == mymove) && (p[i][j + 1] == mymove)) &&
     ((p[i - 1][j] == mymove) && (p[i + 1][j] == mymove)))
    return 1;
 else
    return 0;
}  /* fioe */
