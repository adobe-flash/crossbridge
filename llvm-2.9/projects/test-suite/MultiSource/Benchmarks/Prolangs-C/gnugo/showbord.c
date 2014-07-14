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
#define WHITE 1

extern unsigned char p[19][19];
extern int mymove, umove;
extern int mk, uk;  /* piece captured */

void showboard(void)
/* show go board */
{
   int i, j, ii;

/* p = 0 for empty ,p = 1 for white piece, p = 2 for black piece */
   printf("   A B C D E F G H J K L M N O P Q R S T\n");
/* row 19 to 17 */
   for (i = 0; i < 3; i++)
     {
      ii = 19 - i;
      printf("%2d",ii);

      for (j = 0; j < 19; j++)
	if (p[i][j] == EMPTY)
	   printf(" -");
	else if (p[i][j] == WHITE)
		printf(" O");
	     else printf(" X");

      printf("%2d",ii);
      printf("\n");
     }
/* row 16 */
   printf("16");

   for (j = 0; j < 3; j++)
     if (p[3][j] == EMPTY)
	printf(" -");
     else if (p[3][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[3][3] == 0)
      printf(" +");
   else if (p[3][3] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 4; j < 9; j++)
     if (p[3][j] == EMPTY)
	printf(" -");
     else if (p[3][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[3][9] == EMPTY)
      printf(" +");
   else if (p[3][9] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 10; j < 15; j++)
     if (p[3][j] == EMPTY)
	printf(" -");
     else if (p[3][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[3][15] == EMPTY)
      printf(" +");
   else if (p[3][15] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 16; j < 19; j++)
     if (p[3][j] == EMPTY)
	printf(" -");
     else if (p[3][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   printf("16");
   if (umove == 1)
      printf("     Your color: White O\n");
   else
      if (umove == 2)
	 printf("     Your color: Black X\n");
      else
	 printf("\n");
/* row 15 to 11 */
   for (i = 4; i < 9; i++)
     {
      ii = 19 - i;
      printf("%2d",ii);

      for (j = 0; j < 19; j++)
	if (p[i][j] == EMPTY)
	   printf(" -");
	else if (p[i][j] == WHITE)
		printf(" O");
	     else printf(" X");

      printf("%2d",ii);
      if (i == 4)
	{
	 if (mymove == 1)
	    printf("     My color:   White O\n");
	 else
	    if (mymove == 2)
	       printf("     My color:   Black X\n");
	    else
	       printf("\n");
       }
      else
	 if (i != 8)
	    printf("\n");
	 else
	    printf("     You have captured %d pieces\n", mk);
     }
/* row 10 */
   printf("10");

   for (j = 0; j < 3; j++)
     if (p[9][j] == EMPTY)
	printf(" -");
     else if (p[9][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[9][3] == EMPTY)
      printf(" +");
   else if (p[9][3] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 4; j < 9; j++)
     if (p[9][j] == EMPTY)
	printf(" -");
     else if (p[9][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[9][9] == EMPTY)
      printf(" +");
   else if (p[9][9] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 10; j < 15; j++)
     if (p[9][j] == EMPTY)
	printf(" -");
     else if (p[9][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[9][15] == EMPTY)
      printf(" +");
   else if (p[9][15] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 16; j < 19; j++)
     if (p[9][j] == EMPTY)
	printf(" -");
     else if (p[9][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   printf("10");
   printf("     I have captured %d pieces\n", uk);
/* row 9 to 5 */
   for (i = 10; i < 15; i++)
     {
      ii = 19 - i;
      printf("%2d",ii);

      for (j = 0; j < 19; j++)
	if (p[i][j] == EMPTY)
	   printf(" -");
	else if (p[i][j] == WHITE)
		printf(" O");
	     else printf(" X");

      printf("%2d",ii);
      printf("\n");
     }
/* row 4 */
   printf(" 4");

   for (j = 0; j < 3; j++)
     if (p[15][j] == EMPTY)
	printf(" -");
     else if (p[15][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[15][3] == EMPTY)
      printf(" +");
   else if (p[15][3] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 4; j < 9; j++)
     if (p[15][j] == EMPTY)
	printf(" -");
     else if (p[15][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[15][9] == EMPTY)
      printf(" +");
   else if (p[15][9] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 10; j < 15; j++)
     if (p[15][j] == EMPTY)
	printf(" -");
     else if (p[15][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   if (p[15][15] == EMPTY)
      printf(" +");
   else if (p[15][15] == WHITE)
	   printf(" O");
	else printf(" X");

   for (j = 16; j < 19; j++)
     if (p[15][j] == EMPTY)
	printf(" -");
     else if (p[15][j] == WHITE)
	     printf(" O");
	  else printf(" X");

   printf(" 4");
   printf("\n");
/* row 3 to 1 */
   for (i = 16; i < 19; i++)
     {
      ii = 19 - i;
      printf("%2d",ii);

      for (j = 0; j < 19; j++)
	if (p[i][j] == EMPTY)
	   printf(" -");
	else if (p[i][j] == WHITE)
		printf(" O");
	     else printf(" X");

      printf("%2d",ii);
      printf("\n");
     }
   printf("   A B C D E F G H J K L M N O P Q R S T\n\n");
 }  /* end showboard */
