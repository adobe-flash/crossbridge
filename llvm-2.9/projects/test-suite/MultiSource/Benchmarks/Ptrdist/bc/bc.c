
/* bc.y: The grammar for a POSIX compatable bc processor with some
         extensions to the language. */

/*  This file is part of bc written for MINIX.
    Copyright (C) 1991, 1992 Free Software Foundation, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to
    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

    You may contact the author by:
       e-mail:  phil@cs.wwu.edu
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062
       
*************************************************************************/


#include <stdio.h>
#include "bcdefs.h"
#include "global.h"
#include "proto.h"

typedef struct  {
	char * s_value;
	char	  c_value;
	int	  i_value;
	arg_list * a_value;
       } YYSTYPE;
# define NEWLINE 257
# define AND 258
# define OR 259
# define NOT 260
# define STRING 261
# define NAME 262
# define NUMBER 263
# define MUL_OP 264
# define ASSIGN_OP 265
# define REL_OP 266
# define INCR_DECR 267
# define Define 268
# define Break 269
# define Quit 270
# define Length 271
# define Return 272
# define For 273
# define If 274
# define While 275
# define Sqrt 276
# define Else 277
# define Scale 278
# define Ibase 279
# define Obase 280
# define Auto 281
# define Read 282
# define Warranty 283
# define Halt 284
# define Last 285
# define Continue 286
# define Print 287
# define Limits 288
# define UNARY_MINUS 289
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256


short yyexca[] ={
-1, 1,
	0, -1,
	257, 6,
	59, 6,
	-2, 0,
	};
# define YYNPROD 98
# define YYLAST 706
short yyact[]={

  27,  65, 149, 145,  48,  25,  57,  45,  58,  46,
 105, 135,  41, 125,  86,  86,  38,  66,  34,  35,
 141, 109,  53,  54,  36,   6, 134,  11, 159,  88,
 104,  48, 150,  27, 136,  62,  62,  62,  25, 142,
 126,  63, 151, 127, 125, 118, 115, 162, 139, 124,
   8, 117,  59,  60, 108,  61,  40, 102,  48,  84,
  82,  72,  71,  70,  69,  68,  27,  67,  51,  49,
  23,  25,  75,  76,  77,  78,  79,  81,  74,  83,
  87,  73,  92,  21,  91, 163, 140, 119,  63, 153,
  96,  98, 152,  89,  56,  99, 100, 101,  27, 129,
  64, 106, 107,  25,  52, 144,  40, 128, 164, 113,
 110, 155, 112, 137,  50,   4,  21,   3,   2,  95,
 116,  94, 148, 103,  80,   1,   0,  27,   0,   0,
   0,   0,  25,   0,   0,   0,   0, 114,   0,   0,
   0,   0,   0,  91,   0, 130, 131,  98,   0,  21,
   0,   0,   0,   0,   0,   0,  27,   0,   0, 143,
   0,  25,   0,   0,   0, 110,  27,   0,  98,   0,
   0,  25,   0,   0,  47,   0, 157, 160,   0, 138,
 154,  21,   0, 110,   0, 113,   0,   0,   0,   0,
 146,   0,  27,   0,   0,   0,   0,  25,   0,   0,
   0,   0,  27, 156,   0,   0,   0,  25,   0, 147,
  21,   0,  85, 161,  37, 165,  55,   0,   0, 133,
  24,  12,  28,  26,   0,   0, 158,  29,  47,  13,
  15,  30,  17,  18,  19,  20,  31,   0,  32,  34,
  35,   0,  33,   9,  16,  36,  14,  22,  10,   5,
   0,   0,  27,  24,  12,  28,  26,  25,   0,   0,
  29,   7,  13,  15,  30,  17,  18,  19,  20,  31,
   0,  32,  34,  35,   0,  33,   9,  16,  36,  14,
  22,  10,  55,   0,   0,  27,  24,  12,  28,  26,
  25,   0,   0,  29,   0,  13,  15,  30,  17,  18,
  19,  20,  31,   0,  32,  34,  35,   0,  33,   9,
  16,  36,  14,  22,  10,  39,   0,   0,  24,  12,
  28,  26,   0,   0,   0,  29,   0,  13,  15,  30,
  17,  18,  19,  20,  31,   0,  32,  34,  35,   0,
  33,   9,  16,  36,  14,  22,  10,  24,  12,  28,
  26,   0,   0,   0,  29,   0,  13,  15,  30,  17,
  18,  19,  20,  31,   0,  32,  34,  35,   0,  33,
   9,  16,  36,  14,  22,  10,  24,   0,  28,  26,
   0,   0,   0,  29,   0,   0,  24,  30,  28,  26,
   0,   0,  31,  29,  32,  34,  35,  30,  33,   0,
   0,  36,  31,   0,  32,  34,  35,  45,  33,  46,
   0,  36,  24,  90,  28,  26, 123,   0,  45,  29,
  46,   0,  24,  30,  28,  26,   0,   0,  31,  29,
  32,  34,  35,  30,  33,   0,   0,  36,  31,   0,
  32,  34,  35,   0,  33,   0, 122,  36,  45, 121,
  46,  45,  45,  46,  46,   0,   0, 120,  48,   0,
 111,   0,  45,   0,  46,   0,  45,   0,  46,  48,
   0,   0,  24,   0, 132,  26,  93,   0,  45,  29,
  46,   0,  45,  30,  46,   0,   0,   0,  31,   0,
  32,  34,  35,   0,  33,   0,   0,  36,   0,  48,
   0,   0,  48,  48,   0,  24,   0,  97,  26,   0,
   0,   0,  29,  48,   0,   0,  30,  48,   0,   0,
   0,  31,   0,  32,  34,  35,   0,  33,   0,  48,
  36,   0,   0,  48,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  42,  43,   0,   0,   0,   0,  47,   0,
  44,   0,   0,  42,  43,   0,   0,   0,   0,  47,
   0,  44,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  42,  43,   0,  42,  43,   0,  47,
   0,  44,  47,  47,  44,  44,   0,  42,  43,   0,
   0,  42,  43,  47,   0,  44,   0,  47,   0,  44,
   0,   0,   0,  42,  43,   0,   0,  42,   0,  47,
   0,  44,   0,  47,   0,  44 };
short yypact[]={

-1000,  -7,-1000, -43,-1000,  58,-1000,-250,-1000,-1000,
-1000, 423,-1000,-1000,-1000,-1000,-1000,  29,-1000,  28,
-1000,  26,-1000,-259, 162, 162,-1000, 162,  -3,-261,
  27,  25,  24,  23,-1000,-1000,-1000,-1000,  26,-1000,
-1000,  21,-1000,-1000, 162, 162, 162, 162, 162, 162,
  20, 162,  19, -45,-1000,  87, 152,-1000,-1000, 409,
-1000, 435, 245, 162,-1000, -50,-1000, 162, 162, 162,
  16,-1000,-252, 162, 162, -36, -90, -90, -63, -63,
  13, 423, 162, 419, 162,  26,  87,-1000,-1000,   2,
-1000, 423, 162,-1000,  10,   1, 423,  -4, 364, 408,
 405, 375,-1000,   8,   0, -51, 409, 439,-1000, -16,
 423,-1000, 423,-1000,-1000, 152, -36,-1000, 212, 126,
-1000,-1000,-1000,-1000, -97,-251, -59,-1000,  87,   7,
-1000, 423,  -5,-1000,-237, -52,-1000, 162,-274,  87,
 116,-279, -61, -17,-1000,-1000,-1000,-1000,-1000,-252,
-1000,-1000,  87,  26, -31, 162,-1000, -44,-1000,-1000,
   6, -40,-1000,-1000,  87,-1000 };
short yypgo[]={

   0, 125,  27, 124,  70,  21, 123, 122,  30, 121,
 119, 118, 117,  22,  50, 115,  23, 114, 113, 111,
 108, 107, 105, 104,  99,  94,  29,  93,  92,  89,
  82,  81,  78 };
short yyr1[]={

   0,   1,   1,  11,  11,  11,  12,  12,  12,  12,
  13,  13,  13,  13,  13,  13,  16,  16,  14,  14,
  14,  14,  14,  14,  14,  14,  14,  14,  17,  18,
  19,  20,  14,  21,  14,  23,  24,  14,  14,  25,
  14,  26,  26,  27,  27,  22,  28,  22,  29,  15,
   6,   6,   7,   7,   7,   8,   8,   8,   8,   9,
   9,  10,  10,  10,  10,   5,   5,   3,   3,  30,
   2,  31,   2,  32,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   4,   4,   4,   4,   4,   4 };
short yyr2[]={

   0,   0,   2,   2,   1,   2,   0,   1,   3,   2,
   0,   1,   2,   3,   2,   3,   1,   2,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   4,   0,   0,
   0,   0,  13,   0,   7,   0,   0,   7,   3,   0,
   3,   1,   3,   1,   1,   0,   0,   3,   0,  12,
   0,   1,   0,   3,   3,   1,   3,   3,   5,   0,
   1,   1,   3,   3,   5,   0,   1,   0,   1,   0,
   4,   0,   4,   0,   4,   2,   3,   3,   3,   3,
   3,   2,   1,   1,   3,   4,   2,   2,   4,   4,
   4,   3,   1,   4,   1,   1,   1,   1 };
short yychk[]={

-1000,  -1, -11, -12, -15, 256, -16, 268, -14, 283,
 288,  -2, 261, 269, 286, 270, 284, 272, 273, 274,
 275, 123, 287,  -4, 260,  45, 263,  40, 262, 267,
 271, 276, 278, 282, 279, 280, 285, 257,  59, 257,
 -14, 262, 258, 259, 266,  43,  45, 264,  94,  40,
 -17,  40, -23, -13, -16, 256, -25, 265, 267,  -2,
  -2,  -2,  40,  91,  -4, 262, 278,  40,  40,  40,
  40, -16,  40, -31, -32,  -2,  -2,  -2,  -2,  -2,
  -3,  -2,  40,  -2,  40, 257,  59, 125, -26, -27,
 261,  -2, -30,  41,  -9, -10,  -2, 262,  -2,  -2,
  -2,  -2,  41,  -6,  -8, 262,  -2,  -2,  41,  -5,
  -2,  41,  -2, -16, -14,  44,  -2,  41,  44,  91,
  93,  41,  41,  41,  41,  44,  91,  59, -21, -24,
 -26,  -2, 262,  93, 123, 262,  93, -18, -14,  41,
  91, 257,  91,  -5, -22, 277, -14,  93,  -7, 281,
  93,  59, -28, -29,  -8, -19, -14, -13, 257,  59,
  -5, 257,  41, 125, -20, -14 };
short yydef[]={

   1,  -2,   2,   0,   4,   0,   7,   0,  16,  18,
  19,  20,  21,  22,  23,  24,  25,  26,  28,   0,
  35,  10,  39,  82,   0,   0,  83,   0,  92,   0,
   0,   0,  96,   0,  94,  95,  97,   3,   9,   5,
  17,   0,  71,  73,   0,   0,   0,   0,   0,  67,
   0,   0,   0,   0,  11,   0,   0,  69,  87,  75,
  81,   0,  59,   0,  86,  92,  96,   0,   0,   0,
   0,   8,  50,   0,   0,  76,  77,  78,  79,  80,
   0,  68,  65,   0,   0,  12,  14,  38,  40,  41,
  43,  44,   0,  84,   0,  60,  61,  92,   0,   0,
   0,   0,  91,   0,  51,  55,  72,  74,  27,   0,
  66,  33,  36,  13,  15,   0,  70,  85,   0,   0,
  93,  88,  89,  90,   0,   0,   0,  29,   0,   0,
  42,  63,  92,  62,   0,  57,  56,  65,  45,   0,
   0,  52,   0,   0,  34,  46,  37,  64,  48,   0,
  58,  30,   0,  10,   0,  65,  47,   0,  53,  54,
   0,  12,  31,  49,   0,  32 };
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif

#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 1:
{
			      yyval.i_value = 0;
			      if (interactive)
				{
				  printf ("%s\n", BC_VERSION);
				  welcome ();
				}
			    } break;
case 3:
{ run_code (); } break;
case 4:
{ run_code (); } break;
case 5:
{
			      yyerrok;
			      init_gen ();
			    } break;
case 6:
{ yyval.i_value = 0; } break;
case 10:
{ yyval.i_value = 0; } break;
case 17:
{ yyval.i_value = yypvt[-0].i_value; } break;
case 18:
{ warranty (("")); } break;
case 19:
{ limits (); } break;
case 20:
{
			      if (yypvt[-0].i_value & 2)
				warn ("comparison in expression");
			      if (yypvt[-0].i_value & 1)
				generate (("W"));
			      else 
				generate (("p"));
			    } break;
case 21:
{
			      yyval.i_value = 0;
			      generate (("w"));
			      generate (yypvt[-0].s_value);
			      free(yypvt[-0].s_value);
			    } break;
case 22:
{
			      if (break_label == 0)
				yyerror ("Break outside a for/while");
			      else
				{
				  sprintf (genstr, "J%1d:", break_label);
				  generate ((genstr));
				}
			    } break;
case 23:
{
			      warn ("Continue statement");
			      if (continue_label == 0)
				yyerror ("Continue outside a for");
			      else
				{
				  sprintf (genstr, "J%1d:", continue_label);
				  generate ((genstr));
				}
			    } break;
case 24:
{ exit (0); } break;
case 25:
{ generate (("h")); } break;
case 26:
{ generate (("0R")); } break;
case 27:
{ generate (("R")); } break;
case 28:
{
			      yypvt[-0].i_value = break_label; 
			      break_label = next_label++;
			    } break;
case 29:
{
			      if (yypvt[-1].i_value > 1)
				warn ("Comparison in first for expression");
			      yypvt[-1].i_value = next_label++;
			      if (yypvt[-1].i_value < 0)
				sprintf (genstr, "N%1d:", yypvt[-1].i_value);
			      else
				sprintf (genstr, "pN%1d:", yypvt[-1].i_value);
			      generate ((genstr));
			    } break;
case 30:
{
			      if (yypvt[-1].i_value < 0) generate (("1"));
			      yypvt[-1].i_value = next_label++;
			      sprintf (genstr, "B%1d:J%1d:", yypvt[-1].i_value, break_label);
			      generate ((genstr));
			      yyval.i_value = continue_label;
			      continue_label = next_label++;
			      sprintf (genstr, "N%1d:", continue_label);
			      generate ((genstr));
			    } break;
case 31:
{
			      if (yypvt[-1].i_value > 1)
				warn ("Comparison in third for expression");
			      if (yypvt[-1].i_value < 0)
				sprintf (genstr, "J%1d:N%1d:", yypvt[-7].i_value, yypvt[-4].i_value);
			      else
				sprintf (genstr, "pJ%1d:N%1d:", yypvt[-7].i_value, yypvt[-4].i_value);
			      generate ((genstr));
			    } break;
case 32:
{
			      sprintf (genstr, "J%1d:N%1d:",
				       continue_label, break_label);
			      generate ((genstr));
			      break_label = yypvt[-12].i_value;
			      continue_label = yypvt[-4].i_value;
			    } break;
case 33:
{
			      yypvt[-1].i_value = if_label;
			      if_label = next_label++;
			      sprintf (genstr, "Z%1d:", if_label);
			      generate ((genstr));
			    } break;
case 34:
{
			      sprintf (genstr, "N%1d:", if_label); 
			      generate ((genstr));
			      if_label = yypvt[-4].i_value;
			    } break;
case 35:
{
			      yypvt[-0].i_value = next_label++;
			      sprintf (genstr, "N%1d:", yypvt[-0].i_value);
			      generate ((genstr));
			    } break;
case 36:
{
			      yypvt[-0].i_value = break_label; 
			      break_label = next_label++;
			      sprintf (genstr, "Z%1d:", break_label);
			      generate ((genstr));
			    } break;
case 37:
{
			      sprintf (genstr, "J%1d:N%1d:", yypvt[-6].i_value, break_label);
			      generate ((genstr));
			      break_label = yypvt[-3].i_value;
			    } break;
case 38:
{ yyval.i_value = 0; } break;
case 39:
{  warn ("print statement"); } break;
case 43:
{
			      generate (("O"));
			      generate (yypvt[-0].s_value);
			      free(yypvt[-0].s_value);
			    } break;
case 44:
{ generate (("P")); } break;
case 46:
{
			      warn ("else clause in if statement");
			      yypvt[-0].i_value = next_label++;
			      sprintf (genstr, "J%d:N%1d:", yypvt[-0].i_value, if_label); 
			      generate ((genstr));
			      if_label = yypvt[-0].i_value;
			    } break;
case 48:
{
			      /* Check auto list against parameter list? */
			      check_params (yypvt[-4].a_value,yypvt[-0].a_value);
			      sprintf (genstr, "F%d,%s.%s[", lookup(yypvt[-6].s_value,FUNCT), 
				       arg_str (yypvt[-4].a_value,TRUE), arg_str (yypvt[-0].a_value,TRUE));
			      generate ((genstr));
			      free_args (yypvt[-4].a_value);
			      free_args (yypvt[-0].a_value);
			      yypvt[-7].i_value = next_label;
			      next_label = 0;
			    } break;
case 49:
{
			      generate (("0R]"));
			      next_label = yypvt[-11].i_value;
			    } break;
case 50:
{ yyval.a_value = NULL; } break;
case 52:
{ yyval.a_value = NULL; } break;
case 53:
{ yyval.a_value = yypvt[-1].a_value; } break;
case 54:
{ yyval.a_value = yypvt[-1].a_value; } break;
case 55:
{ yyval.a_value = nextarg (NULL, lookup (yypvt[-0].s_value,SIMPLE)); } break;
case 56:
{ yyval.a_value = nextarg (NULL, lookup (yypvt[-2].s_value,ARRAY)); } break;
case 57:
{ yyval.a_value = nextarg (yypvt[-2].a_value, lookup (yypvt[-0].s_value,SIMPLE)); } break;
case 58:
{ yyval.a_value = nextarg (yypvt[-4].a_value, lookup (yypvt[-2].s_value,ARRAY)); } break;
case 59:
{ yyval.a_value = NULL; } break;
case 61:
{
			      if (yypvt[-0].i_value > 1) warn ("comparison in argument");
			      yyval.a_value = nextarg (NULL,0);
			    } break;
case 62:
{
			      sprintf (genstr, "K%d:", -lookup (yypvt[-2].s_value,ARRAY));
			      generate ((genstr));
			      yyval.a_value = nextarg (NULL,1);
			    } break;
case 63:
{
			      if (yypvt[-0].i_value > 1) warn ("comparison in argument");
			      yyval.a_value = nextarg (yypvt[-2].a_value,0);
			    } break;
case 64:
{
			      sprintf (genstr, "K%d:", -lookup (yypvt[-2].s_value,ARRAY));
			      generate ((genstr));
			      yyval.a_value = nextarg (yypvt[-4].a_value,1);
			    } break;
case 65:
{
			      yyval.i_value = -1;
			      warn ("Missing expression in for statement");
			    } break;
case 67:
{
			      yyval.i_value = 0;
			      generate (("0"));
			    } break;
case 68:
{
			      if (yypvt[-0].i_value > 1)
				warn ("comparison in return expresion");
			    } break;
case 69:
{
			      if (yypvt[-0].c_value != '=')
				{
				  if (yypvt[-1].i_value < 0)
				    sprintf (genstr, "DL%d:", -yypvt[-1].i_value);
				  else
				    sprintf (genstr, "l%d:", yypvt[-1].i_value);
				  generate ((genstr));
				}
			    } break;
case 70:
{
			      if (yypvt[-0].i_value > 1) warn("comparison in assignment");
			      if (yypvt[-2].c_value != '=')
				{
				  sprintf (genstr, "%c", yypvt[-2].c_value);
				  generate ((genstr));
				}
			      if (yypvt[-3].i_value < 0)
				sprintf (genstr, "S%d:", -yypvt[-3].i_value);
			      else
				sprintf (genstr, "s%d:", yypvt[-3].i_value);
			      generate ((genstr));
			      yyval.i_value = 0;
			    } break;
case 71:
{
			      warn("&& operator");
			      yypvt[-0].i_value = next_label++;
			      sprintf (genstr, "DZ%d:p", yypvt[-0].i_value);
			      generate ((genstr));
			    } break;
case 72:
{
			      sprintf (genstr, "DZ%d:p1N%d:", yypvt[-2].i_value, yypvt[-2].i_value);
			      generate ((genstr));
			      yyval.i_value = yypvt[-3].i_value | yypvt[-0].i_value;
			    } break;
case 73:
{
			      warn("|| operator");
			      yypvt[-0].i_value = next_label++;
			      sprintf (genstr, "B%d:", yypvt[-0].i_value);
			      generate ((genstr));
			    } break;
case 74:
{
			      int tmplab;
			      tmplab = next_label++;
			      sprintf (genstr, "B%d:0J%d:N%d:1N%d:",
				       yypvt[-2].i_value, tmplab, yypvt[-2].i_value, tmplab);
			      generate ((genstr));
			      yyval.i_value = yypvt[-3].i_value | yypvt[-0].i_value;
			    } break;
case 75:
{
			      yyval.i_value = yypvt[-0].i_value;
			      warn("! operator");
			      generate (("!"));
			    } break;
case 76:
{
			      yyval.i_value = 3;
			      switch (*(yypvt[-1].s_value))
				{
				case '=':
				  generate (("="));
				  break;

				case '!':
				  generate (("#"));
				  break;

				case '<':
				  if (yypvt[-1].s_value[1] == '=')
				    generate (("{"));
				  else
				    generate (("<"));
				  break;

				case '>':
				  if (yypvt[-1].s_value[1] == '=')
				    generate (("}"));
				  else
				    generate ((">"));
				  break;
				}
			    } break;
case 77:
{
			      generate (("+"));
			      yyval.i_value = yypvt[-2].i_value | yypvt[-0].i_value;
			    } break;
case 78:
{
			      generate (("-"));
			      yyval.i_value = yypvt[-2].i_value | yypvt[-0].i_value;
			    } break;
case 79:
{
			      genstr[0] = yypvt[-1].c_value;
			      genstr[1] = 0;
			      generate ((genstr));
			      yyval.i_value = yypvt[-2].i_value | yypvt[-0].i_value;
			    } break;
case 80:
{
			      generate (("^"));
			      yyval.i_value = yypvt[-2].i_value | yypvt[-0].i_value;
			    } break;
case 81:
{
			      generate (("n"));
			      yyval.i_value = yypvt[-0].i_value;
			    } break;
case 82:
{
			      yyval.i_value = 1;
			      if (yypvt[-0].i_value < 0)
				sprintf (genstr, "L%d:", -yypvt[-0].i_value);
			      else
				sprintf (genstr, "l%d:", yypvt[-0].i_value);
			      generate ((genstr));
			    } break;
case 83:
{
			      int length = strlen((yypvt[-0].s_value));
			      yyval.i_value = 1;
			      if (length == 1 && *yypvt[-0].s_value == '0')
				generate (("0"));
			      else if (length == 1 && *yypvt[-0].s_value == '1')
				generate (("1"));
			      else
				{
				  generate (("K"));
				  generate (yypvt[-0].s_value);
				  generate ((":"));
				}
			      free(yypvt[-0].s_value);
			    } break;
case 84:
{ yyval.i_value = yypvt[-1].i_value | 1; } break;
case 85:
{
			      yyval.i_value = 1;
			      if (yypvt[-1].a_value != NULL)
				{ 
				  sprintf (genstr, "C%d,%s:",
					   lookup (yypvt[-3].s_value,FUNCT),
					   arg_str (yypvt[-1].a_value,FALSE));
				  free_args (yypvt[-1].a_value);
				}
			      else
				{
				  sprintf (genstr, "C%d:", lookup (yypvt[-3].s_value,FUNCT));
				}
			      generate ((genstr));
			    } break;
case 86:
{
			      yyval.i_value = 1;
			      if (yypvt[-0].i_value < 0)
				{
				  if (yypvt[-1].c_value == '+')
				    sprintf (genstr, "DA%d:L%d:", -yypvt[-0].i_value, -yypvt[-0].i_value);
				  else
				    sprintf (genstr, "DM%d:L%d:", -yypvt[-0].i_value, -yypvt[-0].i_value);
				}
			      else
				{
				  if (yypvt[-1].c_value == '+')
				    sprintf (genstr, "i%d:l%d:", yypvt[-0].i_value, yypvt[-0].i_value);
				  else
				    sprintf (genstr, "d%d:l%d:", yypvt[-0].i_value, yypvt[-0].i_value);
				}
			      generate ((genstr));
			    } break;
case 87:
{
			      yyval.i_value = 1;
			      if (yypvt[-1].i_value < 0)
				{
				  sprintf (genstr, "DL%d:x", -yypvt[-1].i_value);
				  generate ((genstr)); 
				  if (yypvt[-0].c_value == '+')
				    sprintf (genstr, "A%d:", -yypvt[-1].i_value);
				  else
				      sprintf (genstr, "M%d:", -yypvt[-1].i_value);
				}
			      else
				{
				  sprintf (genstr, "l%d:", yypvt[-1].i_value);
				  generate ((genstr));
				  if (yypvt[-0].c_value == '+')
				    sprintf (genstr, "i%d:", yypvt[-1].i_value);
				  else
				    sprintf (genstr, "d%d:", yypvt[-1].i_value);
				}
			      generate ((genstr));
			    } break;
case 88:
{ generate (("cL")); yyval.i_value = 1;} break;
case 89:
{ generate (("cR")); yyval.i_value = 1;} break;
case 90:
{ generate (("cS")); yyval.i_value = 1;} break;
case 91:
{
			      warn ("read function");
			      generate (("cI")); yyval.i_value = 1;
			    } break;
case 92:
{ yyval.i_value = lookup(yypvt[-0].s_value,SIMPLE); } break;
case 93:
{
			      if (yypvt[-1].i_value > 1) warn("comparison in subscript");
			      yyval.i_value = lookup(yypvt[-3].s_value,ARRAY);
			    } break;
case 94:
{ yyval.i_value = 0; } break;
case 95:
{ yyval.i_value = 1; } break;
case 96:
{ yyval.i_value = 2; } break;
case 97:
{ yyval.i_value = 3; } break; 
		}
		goto yystack;  /* stack new state and value */

	}
