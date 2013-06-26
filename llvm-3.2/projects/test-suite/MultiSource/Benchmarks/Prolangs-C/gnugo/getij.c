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

int getij(char move[], int *i, int *j)
/* convert string input to i, j coordinate */
{
 int k;

 if ((move[0] >= 65) && (move[0] <= 72))
    *j = move[0] - 65;
 else
    if ((move[0] >= 74) && (move[0] <= 84))
       *j = move[0] - 66;
    else
       if ((move[0] >= 97) && (move[0] <= 104))
	  *j = move[0] - 97;
       else
	  if ((move[0] >= 106) && (move[0] <= 116))
	     *j = move[0] - 98;
	  else
	     return 0;
 k = move[1] - 48;
 if (move[2]) k = k * 10 + move[2] - 48;
 *i = 19 - k;
 if ((*i >= 0) && (*i <= 18))
    return 1;
 else
    return 0;
}  /* end getij */
