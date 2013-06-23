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
extern int mymove;
extern int mik, mjk, uik, ujk, mk, uk;  /* piece captured */

extern void eval(int color);

void examboard(int color)
/* examine pieces */
  {
   int i, j, n;


/* find liberty of each piece */
   eval(color);

/* initialize piece captured */
   if (color == mymove)
     {
      mik = -1;
      mjk = -1;
    }
   else
     {
      uik = -1;
      ujk = -1;
    }
   n = 0; /* The number of captures this move for Ko purposes */

/* remove all piece of zero liberty */
   for (i = 0; i < 19; i++)
     for (j = 0; j < 19; j++)
       if ((p[i][j] == color) && (l[i][j] == 0))
	 {
	  p[i][j] = EMPTY;
/* record piece captured */
	  if (color == mymove)
	    {
	     mik = i;
	     mjk = j;
	     ++mk;
	   }
	  else
	    {
	     uik = i;
	     ujk = j;
	     ++uk;
	   }
	  ++n;  /* increment number of captures on this move */
	}
/* reset to -1 if more than one stone captured since  no Ko possible */
   if (color == mymove && n > 1)
     {
       mik = -1;   
       mjk = -1;
     }
   else if ( n > 1 )
     {
       uik = -1;
       ujk = -1;
     }
}  /* end examboard */

