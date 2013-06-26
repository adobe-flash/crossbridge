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

#define  SUN  68000

#include <stdio.h>

void showinst(void)
/* show program instructions */
{
 printf("XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOX");
 printf("OXOXOXOXOXOX\n");
 printf("O                                                                  ");
 printf("           O\n");
 printf("X                           GNU GO (Previously Hugo)               ");
 printf("           X\n");
 printf("O                           the game of Go (Wei-Chi)               ");
 printf("           O\n");
 printf("X                                                                  ");
 printf("           X\n");
 printf("O                            version 1.1    3-1-89                 ");
 printf("           O\n");
 printf("X              Copyright (C) 1989 Free Software Foundation, Inc.   ");
 printf("           X\n");
 printf("O                              Author: Man L. Li                   ");
 printf("           O\n");
 printf("X         GNU GO comes with ABSOLUTELY NO WARRANTY; see COPYING for");
 printf("           X\n");
 printf("O         detail.   This is free software, and you are welcome to  ");
 printf("           O\n");
 printf("X         redistribute it; see COPYING for copying conditions.     ");
 printf("           X\n");
 printf("O                                                                  ");
 printf("           O\n");

#ifdef SUN

 printf("X              Please report all bugs, modifications, suggestions  ");
 printf("           X\n");
 printf("O                         to manli@cs.uh.edu  (Internet)           ");
 printf("           O\n");

#endif

 printf("X                                                                  ");
 printf("           X\n");
 printf("OXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO");
 printf("XOXOXOXOXOXO\n");
 printf("\n\n\n\n\n\n\n\nPress return to continue");
 getchar(); 
 printf("\n\nTo play this game first select number of handicap pieces (0 to");
 printf(" 17) for the\nblack side.  Next choose your color (black or white).");
 printf("  To place your piece,\nenter your move as coordinate on the board");
 printf(" in column and row.  The column\nis from 'A' to 'T'(excluding 'I').");
 printf("  The row is from 1 to 19.\n\nTo pass your move enter 'pass' for");
 printf(" your turn.  After both you and the computer\npassed the game will");
 printf(" end.  To save the board and exit enter 'save'.  The game\nwill");
 printf(" continue the next time you start the program.  To stop the game in");
 printf(" the\nmiddle of play enter 'stop' for your move.  You will be");
 printf(" asked whether you want\nto count the result of the game.  If you");
 printf(" answer 'y' then you need to remove the\nremaining dead pieces and");
 printf(" fill up neutral turf on the board as instructed.\nFinally, the");
 printf(" computer will count all pieces for both side and show the result.\n\n");
}  /* end showinst */
