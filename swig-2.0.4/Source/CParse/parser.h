
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
     ID = 258,
     HBLOCK = 259,
     POUND = 260,
     STRING = 261,
     INCLUDE = 262,
     IMPORT = 263,
     INSERT = 264,
     CHARCONST = 265,
     NUM_INT = 266,
     NUM_FLOAT = 267,
     NUM_UNSIGNED = 268,
     NUM_LONG = 269,
     NUM_ULONG = 270,
     NUM_LONGLONG = 271,
     NUM_ULONGLONG = 272,
     NUM_BOOL = 273,
     TYPEDEF = 274,
     TYPE_INT = 275,
     TYPE_UNSIGNED = 276,
     TYPE_SHORT = 277,
     TYPE_LONG = 278,
     TYPE_FLOAT = 279,
     TYPE_DOUBLE = 280,
     TYPE_CHAR = 281,
     TYPE_WCHAR = 282,
     TYPE_VOID = 283,
     TYPE_SIGNED = 284,
     TYPE_BOOL = 285,
     TYPE_COMPLEX = 286,
     TYPE_TYPEDEF = 287,
     TYPE_RAW = 288,
     TYPE_NON_ISO_INT8 = 289,
     TYPE_NON_ISO_INT16 = 290,
     TYPE_NON_ISO_INT32 = 291,
     TYPE_NON_ISO_INT64 = 292,
     LPAREN = 293,
     RPAREN = 294,
     COMMA = 295,
     SEMI = 296,
     EXTERN = 297,
     INIT = 298,
     LBRACE = 299,
     RBRACE = 300,
     PERIOD = 301,
     CONST_QUAL = 302,
     VOLATILE = 303,
     REGISTER = 304,
     STRUCT = 305,
     UNION = 306,
     EQUAL = 307,
     SIZEOF = 308,
     MODULE = 309,
     LBRACKET = 310,
     RBRACKET = 311,
     ILLEGAL = 312,
     CONSTANT = 313,
     NAME = 314,
     RENAME = 315,
     NAMEWARN = 316,
     EXTEND = 317,
     PRAGMA = 318,
     FEATURE = 319,
     VARARGS = 320,
     ENUM = 321,
     CLASS = 322,
     TYPENAME = 323,
     PRIVATE = 324,
     PUBLIC = 325,
     PROTECTED = 326,
     COLON = 327,
     STATIC = 328,
     VIRTUAL = 329,
     FRIEND = 330,
     THROW = 331,
     CATCH = 332,
     EXPLICIT = 333,
     USING = 334,
     NAMESPACE = 335,
     NATIVE = 336,
     INLINE = 337,
     TYPEMAP = 338,
     EXCEPT = 339,
     ECHO = 340,
     APPLY = 341,
     CLEAR = 342,
     SWIGTEMPLATE = 343,
     FRAGMENT = 344,
     WARN = 345,
     LESSTHAN = 346,
     GREATERTHAN = 347,
     DELETE_KW = 348,
     LESSTHANOREQUALTO = 349,
     GREATERTHANOREQUALTO = 350,
     EQUALTO = 351,
     NOTEQUALTO = 352,
     QUESTIONMARK = 353,
     TYPES = 354,
     PARMS = 355,
     NONID = 356,
     DSTAR = 357,
     DCNOT = 358,
     TEMPLATE = 359,
     OPERATOR = 360,
     COPERATOR = 361,
     PARSETYPE = 362,
     PARSEPARM = 363,
     PARSEPARMS = 364,
     CAST = 365,
     LOR = 366,
     LAND = 367,
     OR = 368,
     XOR = 369,
     AND = 370,
     RSHIFT = 371,
     LSHIFT = 372,
     MINUS = 373,
     PLUS = 374,
     MODULO = 375,
     SLASH = 376,
     STAR = 377,
     LNOT = 378,
     NOT = 379,
     UMINUS = 380,
     DCOLON = 381
   };
#endif
/* Tokens.  */
#define ID 258
#define HBLOCK 259
#define POUND 260
#define STRING 261
#define INCLUDE 262
#define IMPORT 263
#define INSERT 264
#define CHARCONST 265
#define NUM_INT 266
#define NUM_FLOAT 267
#define NUM_UNSIGNED 268
#define NUM_LONG 269
#define NUM_ULONG 270
#define NUM_LONGLONG 271
#define NUM_ULONGLONG 272
#define NUM_BOOL 273
#define TYPEDEF 274
#define TYPE_INT 275
#define TYPE_UNSIGNED 276
#define TYPE_SHORT 277
#define TYPE_LONG 278
#define TYPE_FLOAT 279
#define TYPE_DOUBLE 280
#define TYPE_CHAR 281
#define TYPE_WCHAR 282
#define TYPE_VOID 283
#define TYPE_SIGNED 284
#define TYPE_BOOL 285
#define TYPE_COMPLEX 286
#define TYPE_TYPEDEF 287
#define TYPE_RAW 288
#define TYPE_NON_ISO_INT8 289
#define TYPE_NON_ISO_INT16 290
#define TYPE_NON_ISO_INT32 291
#define TYPE_NON_ISO_INT64 292
#define LPAREN 293
#define RPAREN 294
#define COMMA 295
#define SEMI 296
#define EXTERN 297
#define INIT 298
#define LBRACE 299
#define RBRACE 300
#define PERIOD 301
#define CONST_QUAL 302
#define VOLATILE 303
#define REGISTER 304
#define STRUCT 305
#define UNION 306
#define EQUAL 307
#define SIZEOF 308
#define MODULE 309
#define LBRACKET 310
#define RBRACKET 311
#define ILLEGAL 312
#define CONSTANT 313
#define NAME 314
#define RENAME 315
#define NAMEWARN 316
#define EXTEND 317
#define PRAGMA 318
#define FEATURE 319
#define VARARGS 320
#define ENUM 321
#define CLASS 322
#define TYPENAME 323
#define PRIVATE 324
#define PUBLIC 325
#define PROTECTED 326
#define COLON 327
#define STATIC 328
#define VIRTUAL 329
#define FRIEND 330
#define THROW 331
#define CATCH 332
#define EXPLICIT 333
#define USING 334
#define NAMESPACE 335
#define NATIVE 336
#define INLINE 337
#define TYPEMAP 338
#define EXCEPT 339
#define ECHO 340
#define APPLY 341
#define CLEAR 342
#define SWIGTEMPLATE 343
#define FRAGMENT 344
#define WARN 345
#define LESSTHAN 346
#define GREATERTHAN 347
#define DELETE_KW 348
#define LESSTHANOREQUALTO 349
#define GREATERTHANOREQUALTO 350
#define EQUALTO 351
#define NOTEQUALTO 352
#define QUESTIONMARK 353
#define TYPES 354
#define PARMS 355
#define NONID 356
#define DSTAR 357
#define DCNOT 358
#define TEMPLATE 359
#define OPERATOR 360
#define COPERATOR 361
#define PARSETYPE 362
#define PARSEPARM 363
#define PARSEPARMS 364
#define CAST 365
#define LOR 366
#define LAND 367
#define OR 368
#define XOR 369
#define AND 370
#define RSHIFT 371
#define LSHIFT 372
#define MINUS 373
#define PLUS 374
#define MODULO 375
#define SLASH 376
#define STAR 377
#define LNOT 378
#define NOT 379
#define UMINUS 380
#define DCOLON 381




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 1592 "parser.y"

  char  *id;
  List  *bases;
  struct Define {
    String *val;
    String *rawval;
    int     type;
    String *qualifier;
    String *bitfield;
    Parm   *throws;
    String *throwf;
  } dtype;
  struct {
    char *type;
    String *filename;
    int   line;
  } loc;
  struct {
    char      *id;
    SwigType  *type;
    String    *defarg;
    ParmList  *parms;
    short      have_parms;
    ParmList  *throws;
    String    *throwf;
  } decl;
  Parm         *tparms;
  struct {
    String     *method;
    Hash       *kwargs;
  } tmap;
  struct {
    String     *type;
    String     *us;
  } ptype;
  SwigType     *type;
  String       *str;
  Parm         *p;
  ParmList     *pl;
  int           intvalue;
  Node         *node;



/* Line 1676 of yacc.c  */
#line 349 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


