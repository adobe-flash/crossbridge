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
extern int mymove, umove;
extern int mk, uk;  /* piece captured */

extern int getij(char move[], int *i, int *j);
extern void showboard(void);
extern int findcolor(int i, int j);

void endgame(void)
/* count pieces and announce the winner */
{
 char an[10];
 int i, j, mtot, utot, cont;

 printf("\nTo count score, we need the following steps:\n");
 printf("First, I need you to remove all dead pieces on the board.\n");
 printf("Second, I need you to fill in neutral territories with ");
 printf("pieces.\n");
 printf("Last, I will fill in all pieces and anounce the winner.\n");

/* remove dead pieces */
 printf("\nFirst, you should enter the dead pieces (blank and white) to");
 printf(" be removed.  Enter\n");
 printf(" 'stop' when you have finished.\n");

 cont = 1;
 do {
     printf("Dead piece? ");
     scanf("%s", an);
     if (strcmp(an, "stop"))
       {
	getij(an, &i, &j);
	if (p[i][j] == mymove)
	  {
	   p[i][j] = EMPTY;
	   mk++;
	 }
	else
	   if (p[i][j] == umove)
	     {
	      p[i][j] = EMPTY;
	      uk++;
	    }
	showboard();
      }
    else
       cont = 0;
   }
 while (cont);

/* fill in neutral */
 printf("Next, you need to fill in pieces (black and white) in all neutral");
 printf(" territories.\n");
 printf("Enter your and my pieces alternately and enter 'stop' when finish\n");
 cont = 1;

 do {
     printf("Your piece? ");
     scanf("%s", an);
     if (strcmp(an, "stop"))
       {
	getij(an, &i, &j);
	p[i][j] = umove;
	printf("My piece? ");
	scanf("%s", an);
	getij(an, &i, &j);
	p[i][j] = mymove;
	showboard();
      }
     else
	cont = 0;
   }
  while (cont);

/* set empty to side they belong to */
  for (i = 0; i < 19; i++)
     for (j = 0; j < 19; j++)
	if (p[i][j] == EMPTY)
	   p[i][j] = findcolor(i, j);

/* count total */
  mtot = 0;  utot = 0;
  for (i = 0; i < 19; i++)
     for (j = 0; j < 19; j++)
	if (p[i][j] == mymove)
	  ++mtot;
	else
	   if (p[i][j] == umove)
	     ++utot;

  showboard();
  printf("Your total number of pieces %d\n", utot);
  printf("My total number of pieces %d\n", mtot);

}  /* end endgame */

