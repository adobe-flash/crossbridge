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

#define BLACK 2

extern unsigned char p[19][19];

void sethand(int i)
/* set up handicap pieces */
{
 if (i > 0)
   {
    p[3][3] = BLACK;
    if (i > 1)
      {
       p[15][15] = BLACK;
       if (i > 2)
	 {
	  p[3][15] = BLACK;
	  if (i > 3)
	    {
	     p[15][3] = BLACK;
	     if (i == 5)
		p[9][9] = BLACK;
	     else
		if (i > 5)
		  {
		   p[9][15] = BLACK;
		   p[9][3] = BLACK;
		   if (i == 7)
		      p[9][9] = BLACK;
		   else
		      if (i > 7)
			{
			 p[15][9] = BLACK;
			 p[3][9] = BLACK;
			 if (i > 8)
			 p[9][9] = BLACK;
 			 if (i > 9)
 			   {p[2][2] = 2;
 			    if (i > 10)
 			      {p[16][16] = 2;
 			       if (i > 11)
 				 {p[2][16] = 2;
 				  if (i > 12)
 				    {p[16][2] = 2;
 				     if (i > 13)
 				       {p[6][6] = 2;
 					if (i > 14)
 					  {p[12][12] = 2;
 					   if (i > 15)
 					     {p[6][12] = 2;
 					      if (i > 16)
 						p[12][6] = 2;
 					    }
 					 }
 				      }
 				   }
 				}
 			     }
 			  }
		       }
		 }
	   }
	}
     }
  }
}  /* end sethand */

