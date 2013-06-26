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

extern unsigned char p[19][19];

int findcolor(int i, int j)
/* find color for empty piece */
{
 int k, color1, color2;

/* check North neighbor */
 color1 = 0;
 k = i;
 do --k;
 while ((p[k][j] == EMPTY) && (k > 0));
 color1 = p[k][j];

/* check South neighbor */
 color2 = 0;
 k = i;
 do k++;
 while ((p[k][j] == EMPTY) && (k < 18));
 color2 = p[k][j];

 if (color1)
   {
    if ((color1 == color2) || (color2 == 0))
       return color1;
    else
       return 0;  /* cannot determine */
  }
 else
    if (color2)
       return color2;
    else  /* both zero */
       {
/* check West neighbor */
	color1 = 0;
	k = j;
	do --k;
	while ((p[i][k] == EMPTY) && (k > 0));
	color1 = p[i][k];

/* check East neighbor */
	color2 = 0;
	k = j;
	do k++;
	while ((p[i][k] == EMPTY) && (k < 18));
	color2 = p[i][k];

	if (color1)
	  {
	   if ((color1 == color2) || (color2 == 0))
	      return color1;
	   else
	      return 0;  /* cannot determine */
	 }
	else
	   if (color2)
	      return color2;
	   else
	      return 0;
      }
}  /* end findcolor */

