
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     RTFtext = 258,
     RTFstart = 259,
     RTFansi = 260,
     RTFmac = 261,
     RTFpc = 262,
     RTFpca = 263,
     RTFignore = 264,
     RTFinfo = 265,
     RTFstylesheet = 266,
     RTFfootnote = 267,
     RTFheader = 268,
     RTFfooter = 269,
     RTFpict = 270,
     RTFplain = 271,
     RTFparagraph = 272,
     RTFdefaultParagraph = 273,
     RTFrow = 274,
     RTFcell = 275,
     RTFtabulator = 276,
     RTFemdash = 277,
     RTFendash = 278,
     RTFemspace = 279,
     RTFenspace = 280,
     RTFbullet = 281,
     RTFlquote = 282,
     RTFrquote = 283,
     RTFldblquote = 284,
     RTFrdblquote = 285,
     RTFred = 286,
     RTFgreen = 287,
     RTFblue = 288,
     RTFcolorbg = 289,
     RTFcolorfg = 290,
     RTFunderlinecolor = 291,
     RTFcolortable = 292,
     RTFfont = 293,
     RTFfontSize = 294,
     RTFNeXTGraphic = 295,
     RTFNeXTGraphicWidth = 296,
     RTFNeXTGraphicHeight = 297,
     RTFNeXTHelpLink = 298,
     RTFNeXTHelpMarker = 299,
     RTFNeXTfilename = 300,
     RTFNeXTmarkername = 301,
     RTFNeXTlinkFilename = 302,
     RTFNeXTlinkMarkername = 303,
     RTFpaperWidth = 304,
     RTFpaperHeight = 305,
     RTFmarginLeft = 306,
     RTFmarginRight = 307,
     RTFmarginTop = 308,
     RTFmarginButtom = 309,
     RTFfirstLineIndent = 310,
     RTFleftIndent = 311,
     RTFrightIndent = 312,
     RTFalignCenter = 313,
     RTFalignJustified = 314,
     RTFalignLeft = 315,
     RTFalignRight = 316,
     RTFlineSpace = 317,
     RTFspaceAbove = 318,
     RTFstyle = 319,
     RTFbold = 320,
     RTFitalic = 321,
     RTFunderline = 322,
     RTFunderlineDot = 323,
     RTFunderlineDash = 324,
     RTFunderlineDashDot = 325,
     RTFunderlineDashDotDot = 326,
     RTFunderlineDouble = 327,
     RTFunderlineStop = 328,
     RTFunderlineThick = 329,
     RTFunderlineThickDot = 330,
     RTFunderlineThickDash = 331,
     RTFunderlineThickDashDot = 332,
     RTFunderlineThickDashDotDot = 333,
     RTFunderlineWord = 334,
     RTFstrikethrough = 335,
     RTFstrikethroughDouble = 336,
     RTFunichar = 337,
     RTFsubscript = 338,
     RTFsuperscript = 339,
     RTFtabstop = 340,
     RTFfcharset = 341,
     RTFfprq = 342,
     RTFcpg = 343,
     RTFOtherStatement = 344,
     RTFfontListStart = 345,
     RTFfamilyNil = 346,
     RTFfamilyRoman = 347,
     RTFfamilySwiss = 348,
     RTFfamilyModern = 349,
     RTFfamilyScript = 350,
     RTFfamilyDecor = 351,
     RTFfamilyTech = 352
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 82 "rtfGrammar.y"

	int		number;
	const char	*text;
	RTFcmd		cmd;



/* Line 1676 of yacc.c  */
#line 157 "rtfGrammar.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




