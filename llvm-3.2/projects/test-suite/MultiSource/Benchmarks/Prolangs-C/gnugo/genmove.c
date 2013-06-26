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
#define MAXTRY 400

extern unsigned char p[19][19];
extern int mymove, umove;
extern int rd, lib, pass;

extern void eval(int color);
extern int findwinner(int *i, int *j, int *val);
extern int findsaver(int *i, int *j, int *val);
extern int findpatn(int *i, int *j, int *val);
extern void random_nasko(int *i);
extern void countlib(int m, int n, int color);
extern int fioe(int i, int j);

void genmove(int *i, int *j)
/* generate computer move */
  {
   int ti, tj, tval;
   char a;
   int ii, val;
   int try1 = 0;   /* number of try */

/* initialize move and value */
   *i = -1;  *j = -1;  val = -1;

/* re-evaluate liberty of opponent pieces */
   eval(umove);

/* find opponent piece to capture or attack */
   if (findwinner(&ti, &tj, &tval))
       if (tval > val)
	 {
	  val = tval;
	  *i = ti;
	  *j = tj;
	}

/* save any piece if threaten */
   if (findsaver(&ti, &tj, &tval))
       if (tval > val)
	 {
	  val = tval;
	  *i = ti;
	  *j = tj;
	}

/* try match local play pattern for new move */
   if (findpatn(&ti, &tj, &tval))
       if (tval > val)
	 {
	  val = tval;
	  *i = ti;
	  *j = tj;
	}

/* no urgent move then do random move */
   if (val < 0)
       do {
	   random_nasko(&rd);
	   *i = rd % 19;
/* avoid low line  and center region */
	   if ((*i < 2) || (*i > 16) || ((*i > 5) && (*i < 13)))
	     {
	      random_nasko(&rd);
	      *i = rd % 19;
	      if ((*i < 2) || (*i > 16))
		{
		 random_nasko(&rd);
		 *i = rd % 19;
	       }
	    }
	   random_nasko(&rd);
	   *j = rd % 19;
/* avoid low line and center region */
	   if ((*j < 2) || (*j > 16) || ((*j > 5) && (*j < 13)))
	     {
	      random_nasko(&rd);
	      *j = rd % 19;
	      if ((*j < 2) || (*j > 16))
		{
		 random_nasko(&rd);
		 *j = rd % 19;
	       }
	    }
	    lib = 0;
	    countlib(*i, *j, mymove);
       }
/* avoid illegal move, liberty one or suicide, fill in own eye */
       while ((++try1 < MAXTRY)
	      && ((p[*i][*j] != EMPTY) || (lib < 2) || fioe(*i, *j)));

   if (try1 >= MAXTRY)  /* computer pass */
     {
      pass++;
      printf("I pass.\n");
      *i = -1;
    }
   else   /* find valid move */
     {
      pass = 0;      
      printf("my move: ");
      if (*j < 8)
	 a = *j + 65;
      else
	 a = *j + 66;
      printf("%c", a);
      ii = 19 - *i;
      if (ii < 10)
	 printf("%1d\n", ii);
      else
	 printf("%2d\n", ii);
    }
}  /* end genmove */
