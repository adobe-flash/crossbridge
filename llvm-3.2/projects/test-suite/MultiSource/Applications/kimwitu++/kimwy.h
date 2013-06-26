/* A Bison parser, made by GNU Bison 1.875a.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_ID = 258,
     T_OPNAME = 259,
     T_INT = 260,
     T_CNEWLINES = 261,
     T_CLINE = 262,
     T_CEXPRESSION = 263,
     T_DOLLARVAR = 264,
     T_INCLUDEEND = 265,
     T_INCLUDESTART = 266,
     T_ENDOFINCLUDESTART = 267,
     T_LIST = 268,
     T_ARROW = 269,
     T_PROVIDED = 270,
     T_STATIC = 271,
     T_WITH = 272,
     T_FOREACH = 273,
     T_FOREACH_AFTER = 274,
     T_DEFAULT = 275,
     T_UNPBLOCKSTART = 276,
     T_UNPBLOCKEND = 277,
     T_PERCENTRVIEW = 278,
     T_PERCENTRVIEWVAR = 279,
     T_PERCENTUVIEW = 280,
     T_PERCENTUVIEWVAR = 281,
     T_PERCENTSTORAGECLASS = 282,
     T_PERCENTLANGUAGE = 283,
     T_AUTO = 284,
     T_REGISTER = 285,
     T_EXTERN = 286,
     T_TYPEDEF = 287,
     T_CONST = 288,
     T_VOLATILE = 289,
     T_UNSIGNED = 290,
     T_VIRTUAL = 291,
     T_DOTDOTDOT = 292,
     T_COLONCOLON = 293,
     T_OPERATOR = 294,
     T_CONSTRUCTOR = 295,
     T_DESTRUCTOR = 296,
     T_MEMBER = 297,
     T_ATTR = 298,
     T_BASECLASS = 299
   };
#endif
#define T_ID 258
#define T_OPNAME 259
#define T_INT 260
#define T_CNEWLINES 261
#define T_CLINE 262
#define T_CEXPRESSION 263
#define T_DOLLARVAR 264
#define T_INCLUDEEND 265
#define T_INCLUDESTART 266
#define T_ENDOFINCLUDESTART 267
#define T_LIST 268
#define T_ARROW 269
#define T_PROVIDED 270
#define T_STATIC 271
#define T_WITH 272
#define T_FOREACH 273
#define T_FOREACH_AFTER 274
#define T_DEFAULT 275
#define T_UNPBLOCKSTART 276
#define T_UNPBLOCKEND 277
#define T_PERCENTRVIEW 278
#define T_PERCENTRVIEWVAR 279
#define T_PERCENTUVIEW 280
#define T_PERCENTUVIEWVAR 281
#define T_PERCENTSTORAGECLASS 282
#define T_PERCENTLANGUAGE 283
#define T_AUTO 284
#define T_REGISTER 285
#define T_EXTERN 286
#define T_TYPEDEF 287
#define T_CONST 288
#define T_VOLATILE 289
#define T_UNSIGNED 290
#define T_VIRTUAL 291
#define T_DOTDOTDOT 292
#define T_COLONCOLON 293
#define T_OPERATOR 294
#define T_CONSTRUCTOR 295
#define T_DESTRUCTOR 296
#define T_MEMBER 297
#define T_ATTR 298
#define T_BASECLASS 299




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



