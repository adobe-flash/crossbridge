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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */

//
// The Termprocessor Kimwitu++
//
// Copyright (C) 1991 University of Twente, Dept TIOS.
// Copyright (C) 1998-2003 Humboldt-University at Berlin, Institute of Informatics
// All rights reserved.
//
// Kimwitu++ is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Kimwitu++ is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Kimwitu++; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//


//
// kimwy.y
//
static char kimwy_yAccesSid[] = "@(#)$Id: kimwy.cc 12720 2004-04-06 20:25:22Z criswell $";

void yyerror_1(const char *s, int state_no);
#define yyerror(s) yyerror_1(s, yystate)

#ifdef BISON_RECOVER
  /* Note that setting YYDEBUG is required even if you do not want to
   * set the yydebug variable because we use the symbol strings in
   * 'yytname' for initialising costs and also for error messages.
   */
#define YYDEBUG             1
#define yyinsertcost(token) (token[0]=='\''?1:strlen(token))
#define yydeletecost(token) (token[0]=='\''?1:strlen(token))
#endif /* BISON_RECOVER */

/////////////////////////////////////////////////////////////////////////////
//
// Name conventions: tokens are in CAPITALS
// nonterminals are in lowercase
// operators are capitalized
//

// For windows (NT at least) we need to redefine alloca
#if defined(_WIN32) && ! defined (__GNUC__)
# define alloca _alloca
# include <malloc.h>
#endif

#include "k.h"
#include "parse.h"
#include "occur.h"
#include "util.h"	// for the v_add_to routines
#include "rk.h"		// for the patternchains::rewrite routine
#include "yystype.h"	// for YYSTYPE
#include "error.h"

#include <stdlib.h>
#include <string.h>
#include <stack>


namespace { // local kimwy.y vars
static ID pl_phylum;
static withexpressions pl_withvariables;
static std::stack<withexpressions> pl_withvariablesstack;
static std::stack<idCexpressions> pl_idCexpressionsstack;
static std::stack<fileline> pl_filelinestack;
static std::stack<int> pl_nooperatorsstack;
static int non_default_outmostpattern = 0;
}

#define dollar_mark_set() (!pl_withvariables->eq(Nilwithexpressions()))
#define do_MainC() do { if ( dollar_mark_set() ) do_NORMAL(); else do_C(); } while(0)
extern void do_NORMAL();
extern void do_CEXPR();
extern void do_CEXPRDQ();
extern void do_CEXPRSQ();
extern void do_C();
extern int yylex();


#define pf_setfileline(x) do { (x)->file = pg_filename; \
		   (x)->line = pg_lineno; \
		 } while(0)
#define pf_pushfileline() pl_filelinestack.push(FileLine(pg_filename, pg_lineno))
#define pf_setstacktopfileline(x) do { (x)->file = pl_filelinestack.top()->file; \
		       (x)->line = pl_filelinestack.top()->line; \
		     } while(0)
#define pf_topfileline() pl_filelinestack.top()
#define pf_popfileline() pl_filelinestack.pop()

#define pf_setwithvariable(x) pl_withvariables = Conswithexpressions(WEVariable(x), pl_withvariables)
#define pf_resetwithvariable() pl_withvariables = Nilwithexpressions()

#define pf_push_no_operators() pl_nooperatorsstack.push(non_default_outmostpattern)
#define pf_pop_no_operators() do{non_default_outmostpattern = pl_nooperatorsstack.top();pl_nooperatorsstack.pop();}while(0)

/* XXX - why is there no need to restore pl_withvariables on pop? MvL */
#define pf_pushwithvariable() pl_withvariablesstack.push(pl_withvariables)
#define pf_topwithvariable() pl_withvariablesstack.top()
#define pf_popwithvariable() pl_withvariablesstack.pop()

#define pf_pushidCexpressions(x) pl_idCexpressionsstack.push(x)
#define pf_topidCexpressions() pl_idCexpressionsstack.top()
#define pf_popidCexpressions() pl_idCexpressionsstack.pop()



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   771

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  66
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  158
/* YYNRULES -- Number of rules. */
#define YYNRULES  377
/* YYNRULES -- Number of states. */
#define YYNSTATES  611

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   299

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      55,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,    56,     2,    63,     2,    62,    57,
      52,    53,    64,     2,    47,     2,    65,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    45,    46,
      60,    54,    61,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    58,     2,    59,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    48,    51,    49,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    10,    13,    14,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      43,    45,    49,    50,    58,    59,    63,    68,    71,    75,
      76,    79,    81,    83,    87,    89,    93,    97,    98,    99,
     106,   110,   111,   114,   119,   120,   125,   128,   129,   132,
     137,   138,   139,   143,   144,   147,   149,   151,   153,   157,
     161,   165,   169,   172,   175,   178,   181,   182,   185,   187,
     189,   191,   193,   194,   197,   199,   201,   202,   205,   207,
     209,   211,   215,   216,   217,   222,   224,   226,   227,   232,
     233,   234,   240,   241,   242,   248,   250,   252,   254,   255,
     256,   262,   263,   264,   268,   270,   272,   274,   276,   277,
     282,   283,   288,   290,   291,   295,   296,   297,   298,   309,
     310,   311,   321,   322,   323,   324,   332,   333,   335,   336,
     342,   346,   347,   349,   351,   354,   355,   361,   363,   366,
     372,   374,   376,   378,   382,   384,   388,   389,   392,   394,
     398,   401,   403,   407,   409,   412,   416,   420,   423,   429,
     433,   436,   439,   440,   441,   442,   449,   451,   456,   458,
     460,   464,   465,   470,   472,   473,   475,   477,   481,   483,
     486,   490,   494,   496,   501,   508,   515,   519,   523,   524,
     529,   530,   535,   537,   538,   540,   542,   546,   548,   551,
     555,   559,   561,   564,   568,   569,   575,   580,   582,   585,
     591,   596,   602,   607,   610,   611,   614,   615,   618,   619,
     620,   628,   632,   635,   640,   641,   642,   650,   652,   654,
     657,   660,   665,   667,   670,   673,   676,   677,   680,   683,
     687,   691,   693,   697,   700,   705,   709,   715,   718,   722,
     726,   730,   732,   736,   739,   744,   748,   754,   757,   761,
     765,   769,   771,   774,   778,   781,   785,   789,   798,   806,
     813,   819,   825,   826,   829,   831,   835,   840,   841,   844,
     845,   848,   852,   854,   857,   860,   864,   866,   869,   872,
     876,   878,   880,   882,   884,   886,   888,   890,   891,   893,
     895,   897,   899,   901,   903,   906,   909,   913,   915,   918,
     921,   925,   927,   930,   933,   937,   940,   944,   946,   950,
     955,   960,   965,   966,   971,   977,   983,   985,   989,   991,
     994,   995,   997,   999,  1002,  1005,  1009,  1011,  1014,  1015,
    1017,  1019,  1022,  1026,  1028,  1032,  1036,  1040,  1042,  1044,
    1047,  1051,  1056,  1060,  1065,  1069,  1071,  1072,  1075,  1077,
    1080,  1082,  1083,  1088,  1093,  1094,  1096,  1100,  1102,  1105,
    1109,  1113,  1117,  1121,  1123,  1126,  1130,  1133
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      70,     0,    -1,     9,    -1,     3,    -1,     5,    -1,    -1,
      71,    72,    -1,    -1,    72,    73,    -1,    76,    -1,   136,
      -1,   183,    -1,   132,    -1,   162,    -1,   177,    -1,   179,
      -1,   181,    -1,   222,    -1,    74,    -1,    44,    68,    45,
      75,    46,    -1,    68,    -1,    75,    47,    68,    -1,    -1,
      68,    78,    45,    77,    79,    85,    46,    -1,    -1,    48,
      68,    49,    -1,    48,    50,    68,    49,    -1,    48,     1,
      -1,    48,    50,     1,    -1,    -1,    13,    68,    -1,    80,
      -1,    81,    -1,    80,    51,    81,    -1,     1,    -1,    80,
      51,     1,    -1,    80,     1,    81,    -1,    -1,    -1,    68,
      52,    82,    84,    83,    53,    -1,    68,    52,     1,    -1,
      -1,    84,    68,    -1,    84,     3,    45,    68,    -1,    -1,
      48,    86,   131,    49,    -1,    48,     1,    -1,    -1,    86,
      87,    -1,    68,    68,    88,    46,    -1,    -1,    -1,    54,
      89,    90,    -1,    -1,    90,    91,    -1,     8,    -1,    67,
      -1,    55,    -1,    56,    95,    56,    -1,    57,    97,    57,
      -1,    52,    92,    53,    -1,    58,    92,    59,    -1,    56,
       1,    -1,    57,     1,    -1,    52,     1,    -1,    58,     1,
      -1,    -1,    92,    93,    -1,    91,    -1,    47,    -1,    56,
      -1,     1,    -1,    -1,    95,    96,    -1,     8,    -1,    55,
      -1,    -1,    97,    98,    -1,     8,    -1,    55,    -1,   100,
      -1,    99,    47,   100,    -1,    -1,    -1,    68,   101,    90,
     102,    -1,    90,    -1,   103,    -1,    -1,   104,    47,   105,
     103,    -1,    -1,    -1,    48,   107,   112,   108,    49,    -1,
      -1,    -1,    48,   110,   112,   111,    49,    -1,   116,    -1,
     160,    -1,     1,    -1,    -1,    -1,    48,   114,   116,   115,
      49,    -1,    -1,    -1,   116,   117,   118,    -1,     7,    -1,
      67,    -1,    55,    -1,     6,    -1,    -1,    56,   119,    95,
      56,    -1,    -1,    57,   120,    97,    57,    -1,   113,    -1,
      -1,    18,   121,   125,    -1,    -1,    -1,    -1,    17,   122,
      52,   123,   104,   124,    53,    48,   160,    49,    -1,    -1,
      -1,    52,   142,    46,   126,    99,    53,   109,   127,   128,
      -1,    -1,    -1,    -1,    19,   129,    52,   142,   130,    53,
     109,    -1,    -1,   113,    -1,    -1,    11,   134,   133,    12,
      10,    -1,    11,     1,    10,    -1,    -1,   135,    -1,     3,
      -1,   135,     3,    -1,    -1,   146,    14,   137,   138,   140,
      -1,   139,    -1,   138,   139,    -1,    60,   166,    45,   155,
      61,    -1,    46,    -1,     1,    -1,   142,    -1,   141,    47,
     142,    -1,   143,    -1,   142,    62,   143,    -1,    -1,   144,
     145,    -1,   147,    -1,    52,   141,    53,    -1,    63,    68,
      -1,   147,    -1,   146,    47,   147,    -1,     1,    -1,   146,
       1,    -1,   146,    47,     1,    -1,   146,     1,   147,    -1,
      68,   148,    -1,    68,    52,   153,    53,   148,    -1,    68,
      54,   147,    -1,    64,   148,    -1,    20,   148,    -1,    -1,
      -1,    -1,    15,    52,   149,    90,   150,    53,    -1,    68,
      -1,    68,    52,   153,    53,    -1,    64,    -1,    20,    -1,
      68,    54,   151,    -1,    -1,    56,   152,    95,    56,    -1,
      69,    -1,    -1,   154,    -1,   151,    -1,   154,    47,   151,
      -1,     1,    -1,   154,     1,    -1,   154,    47,     1,    -1,
     154,     1,   151,    -1,    68,    -1,    68,    52,   158,    53,
      -1,   155,    14,    68,    52,   158,    53,    -1,   155,    65,
      68,    52,   158,    53,    -1,   155,    14,    68,    -1,   155,
      65,    68,    -1,    -1,    57,   156,    97,    57,    -1,    -1,
      56,   157,    95,    56,    -1,    69,    -1,    -1,   159,    -1,
     155,    -1,   159,    47,   155,    -1,     1,    -1,   159,     1,
      -1,   159,    47,     1,    -1,   159,     1,   155,    -1,   161,
      -1,   160,   161,    -1,   141,    45,   113,    -1,    -1,   146,
      14,   163,   164,    46,    -1,   146,    14,     1,    46,    -1,
     165,    -1,   164,   165,    -1,    58,   166,    45,   167,    59,
      -1,    58,   166,    45,     1,    -1,    58,     1,    45,   167,
      59,    -1,    58,     1,    45,     1,    -1,    58,     1,    -1,
      -1,   166,    68,    -1,    -1,   167,   168,    -1,    -1,    -1,
     220,    56,   169,    95,    94,   170,   176,    -1,   220,   173,
     176,    -1,   220,   113,    -1,   220,    21,   167,    22,    -1,
      -1,    -1,    26,    68,    68,   171,    90,   172,    46,    -1,
      68,    -1,    67,    -1,    68,   174,    -1,    67,   174,    -1,
      52,    68,    53,    68,    -1,   175,    -1,   174,   175,    -1,
      14,    68,    -1,    14,     1,    -1,    -1,    45,    68,    -1,
      45,     1,    -1,    25,   178,    46,    -1,    25,     1,    46,
      -1,    68,    -1,    52,    68,    53,    -1,   178,    68,    -1,
     178,    52,    68,    53,    -1,   178,    47,    68,    -1,   178,
      47,    52,    68,    53,    -1,   178,     1,    -1,   178,    47,
       1,    -1,    23,   180,    46,    -1,    23,     1,    46,    -1,
      68,    -1,    52,    68,    53,    -1,   180,    68,    -1,   180,
      52,    68,    53,    -1,   180,    47,    68,    -1,   180,    47,
      52,    68,    53,    -1,   180,     1,    -1,   180,    47,     1,
      -1,    27,   182,    46,    -1,    27,     1,    46,    -1,    68,
      -1,   182,    68,    -1,   182,    47,    68,    -1,   182,     1,
      -1,   182,    47,     1,    -1,   189,   196,   218,    -1,    68,
      38,    39,    68,    52,    53,   206,   218,    -1,    40,    68,
      52,   209,    53,   184,   218,    -1,    41,   193,    68,    52,
      53,   218,    -1,    42,   190,   198,   188,    46,    -1,    43,
     190,   198,   188,    46,    -1,    -1,    45,   185,    -1,   186,
      -1,   185,    47,   186,    -1,    68,    52,   215,    53,    -1,
      -1,    54,   215,    -1,    -1,    54,   215,    -1,    52,   215,
      53,    -1,   194,    -1,   191,   194,    -1,   195,   194,    -1,
     191,   195,   194,    -1,   194,    -1,   192,   194,    -1,   195,
     194,    -1,   192,   195,   194,    -1,    29,    -1,    30,    -1,
      16,    -1,    31,    -1,    32,    -1,    36,    -1,    16,    -1,
      -1,    36,    -1,    68,    -1,    33,    -1,    34,    -1,    35,
      -1,   203,    -1,   207,   203,    -1,    62,   203,    -1,   207,
      62,   203,    -1,   200,    -1,   207,   200,    -1,    62,   200,
      -1,   207,    62,   200,    -1,   201,    -1,   207,   201,    -1,
      62,   201,    -1,   207,    62,   201,    -1,   207,   200,    -1,
     207,    62,   200,    -1,   217,    -1,    52,   199,    53,    -1,
     200,    58,   214,    59,    -1,   200,    52,   209,    53,    -1,
      68,    38,    68,   202,    -1,    -1,   202,    58,   215,    59,
      -1,   204,    52,   209,    53,   206,    -1,   204,    52,     1,
      53,   206,    -1,   205,    -1,   204,    38,   205,    -1,    68,
      -1,    39,     4,    -1,    -1,    33,    -1,    64,    -1,    64,
     208,    -1,    64,   207,    -1,    64,   208,   207,    -1,   195,
      -1,   208,   195,    -1,    -1,    37,    -1,   210,    -1,   210,
      37,    -1,   210,    47,    37,    -1,   211,    -1,   210,    47,
     211,    -1,   189,   197,   187,    -1,   189,   212,   187,    -1,
     207,    -1,   213,    -1,   207,   213,    -1,    52,   212,    53,
      -1,   213,    58,   214,    59,    -1,    58,   214,    59,    -1,
     213,    52,   209,    53,    -1,    52,   209,    53,    -1,   215,
      -1,    -1,   216,    90,    -1,    68,    -1,    63,    68,    -1,
     106,    -1,    -1,     1,    46,   219,   218,    -1,    60,   221,
      61,    45,    -1,    -1,    68,    -1,   221,    47,    68,    -1,
       1,    -1,   221,     1,    -1,   221,    47,     1,    -1,   221,
       1,    68,    -1,    28,   223,    46,    -1,    28,     1,    46,
      -1,    68,    -1,   223,    68,    -1,   223,    47,    68,    -1,
     223,     1,    -1,   223,    47,     1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   286,   286,   291,   296,   308,   308,   316,   318,   322,
     324,   330,   332,   334,   336,   338,   340,   342,   344,   349,
     354,   356,   362,   361,   369,   370,   372,   374,   376,   382,
     383,   388,   393,   395,   397,   399,   401,   406,   406,   406,
     409,   415,   416,   418,   425,   426,   428,   434,   435,   440,
     446,   448,   447,   460,   461,   466,   468,   470,   472,   474,
     476,   478,   480,   482,   484,   486,   492,   493,   498,   500,
     505,   506,   511,   512,   517,   519,   525,   526,   531,   533,
     538,   540,   546,   548,   545,   553,   558,   560,   560,   572,
     574,   571,   581,   583,   580,   589,   591,   598,   604,   606,
     603,   613,   615,   614,   621,   623,   625,   627,   630,   629,
     635,   634,   640,   644,   643,   649,   651,   653,   648,   662,
     664,   662,   696,   698,   699,   697,   730,   731,   736,   736,
     742,   748,   751,   756,   765,   778,   777,   785,   787,   792,
     804,   805,   809,   811,   816,   818,   823,   823,   829,   831,
     833,   838,   840,   842,   844,   846,   848,   853,   856,   859,
     862,   865,   872,   873,   873,   873,   878,   880,   882,   884,
     886,   889,   888,   893,   899,   900,   905,   907,   909,   911,
     913,   915,   920,   923,   927,   930,   933,   936,   940,   939,
     946,   945,   951,   958,   959,   964,   966,   968,   970,   972,
     974,   979,   981,   986,   992,   991,   996,  1001,  1003,  1008,
    1018,  1020,  1022,  1024,  1030,  1031,  1037,  1038,  1044,  1046,
    1043,  1053,  1055,  1057,  1060,  1060,  1059,  1066,  1068,  1070,
    1072,  1077,  1082,  1084,  1089,  1091,  1097,  1098,  1100,  1105,
    1107,  1112,  1116,  1120,  1124,  1128,  1132,  1136,  1138,  1143,
    1145,  1150,  1154,  1158,  1162,  1166,  1170,  1174,  1176,  1181,
    1183,  1188,  1192,  1196,  1200,  1202,  1207,  1217,  1237,  1257,
    1277,  1284,  1295,  1296,  1301,  1303,  1308,  1314,  1316,  1322,
    1324,  1327,  1338,  1342,  1348,  1354,  1366,  1370,  1376,  1382,
    1394,  1396,  1398,  1400,  1402,  1404,  1409,  1415,  1416,  1425,
    1430,  1432,  1434,  1439,  1441,  1443,  1445,  1450,  1452,  1454,
    1456,  1461,  1463,  1465,  1467,  1472,  1474,  1479,  1481,  1483,
    1485,  1490,  1496,  1497,  1502,  1508,  1518,  1520,  1525,  1527,
    1537,  1538,  1543,  1545,  1547,  1549,  1554,  1556,  1562,  1563,
    1565,  1567,  1569,  1574,  1576,  1581,  1583,  1588,  1590,  1592,
    1597,  1599,  1601,  1603,  1605,  1613,  1617,  1617,  1624,  1626,
    1631,  1633,  1633,  1637,  1640,  1644,  1646,  1648,  1650,  1652,
    1654,  1659,  1661,  1666,  1668,  1670,  1672,  1674
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_ID", "T_OPNAME", "T_INT", "T_CNEWLINES", 
  "T_CLINE", "T_CEXPRESSION", "T_DOLLARVAR", "T_INCLUDEEND", 
  "T_INCLUDESTART", "T_ENDOFINCLUDESTART", "T_LIST", "T_ARROW", 
  "T_PROVIDED", "T_STATIC", "T_WITH", "T_FOREACH", "T_FOREACH_AFTER", 
  "T_DEFAULT", "T_UNPBLOCKSTART", "T_UNPBLOCKEND", "T_PERCENTRVIEW", 
  "T_PERCENTRVIEWVAR", "T_PERCENTUVIEW", "T_PERCENTUVIEWVAR", 
  "T_PERCENTSTORAGECLASS", "T_PERCENTLANGUAGE", "T_AUTO", "T_REGISTER", 
  "T_EXTERN", "T_TYPEDEF", "T_CONST", "T_VOLATILE", "T_UNSIGNED", 
  "T_VIRTUAL", "T_DOTDOTDOT", "T_COLONCOLON", "T_OPERATOR", 
  "T_CONSTRUCTOR", "T_DESTRUCTOR", "T_MEMBER", "T_ATTR", "T_BASECLASS", 
  "':'", "';'", "','", "'{'", "'}'", "'!'", "'|'", "'('", "')'", "'='", 
  "'\\n'", "'\"'", "'''", "'['", "']'", "'<'", "'>'", "'&'", "'$'", "'*'", 
  "'.'", "$accept", "dollarvar", "id", "integer", "specification", "@1", 
  "declarations", "declaration", "baseclassdeclaration", "baseclass_list", 
  "phylumdeclaration", "@2", "storageoption", "productionblock", 
  "alternatives", "alternative", "@3", "@4", "arguments", "Ccode_option", 
  "attributes_option", "attribute", "attribute_init_option", "@5", 
  "Cexpression", "Cexpression_elem", "Cexpression_inner", 
  "Cexpression_elem_inner", "quote_or_error", "CexpressionDQ", 
  "CexpressionDQ_elem", "CexpressionSQ", "CexpressionSQ_elem", 
  "idCexpressions", "idCexpression", "@6", "@7", "withCexpression", 
  "withCexpressions", "@8", "MainCbody", "@9", "@10", "MainCbodyinC", 
  "@11", "@12", "MainCBodycontinuation", "Cbody", "@13", "@14", "Ctext", 
  "@15", "Ctext_elem", "@16", "@17", "@18", "@19", "@20", "@21", 
  "foreach_continuation", "@22", "@23", "foreach_end_continuation", "@24", 
  "@25", "init_option", "includedeclaration", "@26", 
  "includefiles_option", "includefiles", "rwdeclaration", "@27", 
  "rwclauses", "rwclause", "semi_or_error", "patternchains", 
  "patternchain", "patternchainitem_lineinfo", "@28", "patternchainitem", 
  "outmostpatterns", "outmostpattern", "provideds", "@29", "@30", 
  "pattern", "@31", "patternsoption", "patterns", "term", "@32", "@33", 
  "termsoption", "terms", "withcases", "withcase", "unparsedeclaration", 
  "@34", "unparseclauses", "unparseclause", "useviewnames", 
  "unparseitems", "unparseitem", "@35", "@36", "@37", "@38", "unpsubterm", 
  "unpattributes", "unpattribute", "viewnameoption", "uviewdeclaration", 
  "defuviewnames", "rviewdeclaration", "defrviewnames", 
  "storageclassdeclaration", "defstorageclasses", 
  "ac_function_definition", "ac_opt_base_init_list", "ac_base_init_list", 
  "ac_base_init", "ac_default_arg_init", "ac_opt_member_init", 
  "ac_declaration_specifiers", "ac_member_declaration_specifiers", 
  "ac_storage_class_specifier", "ac_member_storage_class_specifier", 
  "opt_virtual", "ac_type_specifier", "ac_type_qualifier", 
  "ac_fn_declarator", "ac_declarator", "ac_member_declarator", 
  "ac_pointer_declarator", "ac_direct_declarator", 
  "ac_direct_member_declarator", "ac_constant_expression_list", 
  "ac_direct_fn_declarator", "ac_qualifier", "ac_fn_declarator_id", 
  "ac_opt_const", "ac_pointer", "ac_type_qualifier_list", 
  "ac_parameter_type_list", "ac_parameter_list", 
  "ac_parameter_declaration", "ac_abstract_declarator", 
  "ac_direct_abstract_declarator", "ac_constant_expression_option", 
  "ac_constant_expression", "@39", "ac_identifier", 
  "ac_compound_statement", "@40", "languageoption", "languageoptions", 
  "languagedeclaration", "deflanguagenames", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,    58,    59,    44,   123,   125,
      33,   124,    40,    41,    61,    10,    34,    39,    91,    93,
      60,    62,    38,    36,    42,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    66,    67,    68,    69,    71,    70,    72,    72,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    74,
      75,    75,    77,    76,    78,    78,    78,    78,    78,    79,
      79,    79,    80,    80,    80,    80,    80,    82,    83,    81,
      81,    84,    84,    84,    85,    85,    85,    86,    86,    87,
      88,    89,    88,    90,    90,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    92,    92,    93,    93,
      94,    94,    95,    95,    96,    96,    97,    97,    98,    98,
      99,    99,   101,   102,   100,   103,   104,   105,   104,   107,
     108,   106,   110,   111,   109,   112,   112,   112,   114,   115,
     113,   116,   117,   116,   118,   118,   118,   118,   119,   118,
     120,   118,   118,   121,   118,   122,   123,   124,   118,   126,
     127,   125,   128,   129,   130,   128,   131,   131,   133,   132,
     132,   134,   134,   135,   135,   137,   136,   138,   138,   139,
     140,   140,   141,   141,   142,   142,   144,   143,   145,   145,
     145,   146,   146,   146,   146,   146,   146,   147,   147,   147,
     147,   147,   148,   149,   150,   148,   151,   151,   151,   151,
     151,   152,   151,   151,   153,   153,   154,   154,   154,   154,
     154,   154,   155,   155,   155,   155,   155,   155,   156,   155,
     157,   155,   155,   158,   158,   159,   159,   159,   159,   159,
     159,   160,   160,   161,   163,   162,   162,   164,   164,   165,
     165,   165,   165,   165,   166,   166,   167,   167,   169,   170,
     168,   168,   168,   168,   171,   172,   168,   173,   173,   173,
     173,   173,   174,   174,   175,   175,   176,   176,   176,   177,
     177,   178,   178,   178,   178,   178,   178,   178,   178,   179,
     179,   180,   180,   180,   180,   180,   180,   180,   180,   181,
     181,   182,   182,   182,   182,   182,   183,   183,   183,   183,
     183,   183,   184,   184,   185,   185,   186,   187,   187,   188,
     188,   188,   189,   189,   189,   189,   190,   190,   190,   190,
     191,   191,   191,   191,   191,   191,   192,   193,   193,   194,
     195,   195,   195,   196,   196,   196,   196,   197,   197,   197,
     197,   198,   198,   198,   198,   199,   199,   200,   200,   200,
     200,   201,   202,   202,   203,   203,   204,   204,   205,   205,
     206,   206,   207,   207,   207,   207,   208,   208,   209,   209,
     209,   209,   209,   210,   210,   211,   211,   212,   212,   212,
     213,   213,   213,   213,   213,   214,   216,   215,   217,   217,
     218,   219,   218,   220,   220,   221,   221,   221,   221,   221,
     221,   222,   222,   223,   223,   223,   223,   223
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     1,     0,     2,     0,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       1,     3,     0,     7,     0,     3,     4,     2,     3,     0,
       2,     1,     1,     3,     1,     3,     3,     0,     0,     6,
       3,     0,     2,     4,     0,     4,     2,     0,     2,     4,
       0,     0,     3,     0,     2,     1,     1,     1,     3,     3,
       3,     3,     2,     2,     2,     2,     0,     2,     1,     1,
       1,     1,     0,     2,     1,     1,     0,     2,     1,     1,
       1,     3,     0,     0,     4,     1,     1,     0,     4,     0,
       0,     5,     0,     0,     5,     1,     1,     1,     0,     0,
       5,     0,     0,     3,     1,     1,     1,     1,     0,     4,
       0,     4,     1,     0,     3,     0,     0,     0,    10,     0,
       0,     9,     0,     0,     0,     7,     0,     1,     0,     5,
       3,     0,     1,     1,     2,     0,     5,     1,     2,     5,
       1,     1,     1,     3,     1,     3,     0,     2,     1,     3,
       2,     1,     3,     1,     2,     3,     3,     2,     5,     3,
       2,     2,     0,     0,     0,     6,     1,     4,     1,     1,
       3,     0,     4,     1,     0,     1,     1,     3,     1,     2,
       3,     3,     1,     4,     6,     6,     3,     3,     0,     4,
       0,     4,     1,     0,     1,     1,     3,     1,     2,     3,
       3,     1,     2,     3,     0,     5,     4,     1,     2,     5,
       4,     5,     4,     2,     0,     2,     0,     2,     0,     0,
       7,     3,     2,     4,     0,     0,     7,     1,     1,     2,
       2,     4,     1,     2,     2,     2,     0,     2,     2,     3,
       3,     1,     3,     2,     4,     3,     5,     2,     3,     3,
       3,     1,     3,     2,     4,     3,     5,     2,     3,     3,
       3,     1,     2,     3,     2,     3,     3,     8,     7,     6,
       5,     5,     0,     2,     1,     3,     4,     0,     2,     0,
       2,     3,     1,     2,     2,     3,     1,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     1,     2,     2,
       3,     1,     2,     2,     3,     2,     3,     1,     3,     4,
       4,     4,     0,     4,     5,     5,     1,     3,     1,     2,
       0,     1,     1,     2,     2,     3,     1,     2,     0,     1,
       1,     2,     3,     1,     3,     3,     3,     1,     1,     2,
       3,     4,     3,     4,     3,     1,     0,     2,     1,     2,
       1,     0,     4,     4,     0,     1,     3,     1,     2,     3,
       3,     3,     3,     1,     2,     3,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       5,     0,     7,     1,     0,   153,     3,     0,   292,   162,
       0,     0,     0,     0,   290,   291,   293,   294,   300,   301,
     302,   295,     0,   297,     0,     0,     0,   162,   299,     8,
      18,     9,    12,    10,     0,   151,    13,    14,    15,    16,
      11,     0,     0,   282,     0,    17,     0,   133,   128,   132,
       0,   161,     0,     0,   251,     0,     0,     0,   241,     0,
       0,   261,     0,     0,   373,     0,     0,   298,     0,   296,
     299,     0,     0,   286,     0,     0,     0,   160,     0,     0,
       0,     0,     0,   157,   154,     0,     0,     0,     0,   332,
     328,     0,   303,     0,   326,     0,   283,     0,   284,   130,
       0,   134,   163,   250,     0,   257,   249,     0,     0,   253,
     240,     0,   247,   239,     0,     0,   243,   260,   264,   259,
       0,   262,   372,   376,   371,     0,   374,   338,     0,     0,
       0,   279,   311,     0,   287,     0,   288,   279,     0,     0,
      27,     0,     0,   178,     4,   169,   171,   168,   166,   173,
     176,     0,     0,   162,   159,    22,   156,     0,     0,     0,
     155,   152,   329,   305,   336,   334,   333,     0,    89,   360,
     266,     0,     0,     0,   304,   285,     0,    53,   252,   258,
       0,   255,     0,   242,   248,     0,   245,     0,   265,   263,
     377,   375,   339,     0,     0,   340,   343,     0,   313,     0,
     356,   356,     0,     0,   312,   289,     0,    20,     0,     0,
      28,     0,    25,    72,     0,     0,   162,   179,     0,     0,
     206,   214,     0,   137,     0,     0,   207,   337,   335,   361,
       0,   327,     0,     0,   306,   129,   164,     0,   254,     0,
     244,   338,   356,     0,     0,   358,   277,   307,   347,   277,
     348,   317,   272,   341,     0,     0,   322,     0,    53,   280,
     270,   314,   271,    19,     0,     0,    26,     0,     0,   170,
     158,   181,   180,   177,    34,     0,     0,    44,     0,    32,
       0,   141,   140,   138,   136,   213,     0,   205,   208,     0,
      97,    90,   102,     0,   142,   144,     0,   146,   201,   330,
     330,    55,     2,     0,    57,     0,     0,     0,    56,    54,
       0,   256,   246,   338,     0,   347,     0,     0,     0,   355,
       0,   309,   359,   356,   345,   338,   356,     0,   308,   349,
     346,   338,   356,     0,     0,   342,   344,   269,   321,   281,
     357,    21,   330,    74,    75,   172,    73,   167,    30,     0,
       0,     0,     0,     0,     0,   215,     0,     0,   362,     0,
       0,     0,   146,   146,   146,     0,   147,   148,   202,   331,
     325,   324,    64,     0,    62,     0,    63,     0,    65,     0,
     165,   347,   318,     0,   315,   354,   350,   352,     0,   278,
       0,     0,   310,     0,     0,     0,   273,   274,   268,   356,
       0,    40,    41,    46,   126,    23,    36,    35,    33,   190,
     188,   182,   192,     0,   212,   364,   210,   364,    91,   107,
     104,   115,   113,    98,   106,   108,   110,   105,   112,   103,
     203,   143,   145,     0,   150,    69,    60,    68,    67,    58,
      78,    79,    59,    77,    61,   316,   320,   319,   353,   351,
     356,     0,     0,   267,    38,     0,    48,   127,     0,    72,
      76,     0,     0,   139,     0,     0,   211,     0,   217,     0,
     209,     0,     0,   101,    72,    76,   149,     0,   275,   323,
       3,    42,     0,    50,    45,     0,     0,   197,   195,     0,
       0,   186,   187,     0,   367,   365,     0,   216,     0,   218,
     228,   227,   222,   236,   116,   146,   114,   102,     0,     0,
     276,     0,    39,    51,     0,   191,   189,   183,   198,     0,
       0,     0,   224,   368,     0,     0,   364,     0,    72,     0,
     230,   232,   229,     0,   221,    53,     0,     0,   109,   111,
      43,    53,    49,   200,   199,   196,     0,     0,    53,   370,
     369,   366,   363,   223,     0,     0,   235,   234,   233,   238,
     237,    85,    86,   117,   119,   100,    52,   184,   185,   225,
     231,    71,    70,   219,    87,     0,     0,     0,   236,    53,
       0,    82,     0,    80,   226,   220,    88,   146,    53,     0,
       0,   146,    83,    81,    92,   120,   118,    84,     0,   122,
      93,   123,   121,     0,     0,    94,   146,   124,     0,     0,
     125
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,   308,    70,   412,     1,     2,     4,    29,    30,   208,
      31,   219,    82,   277,   278,   279,   402,   482,   454,   351,
     404,   456,   514,   541,   561,   309,   373,   438,   573,   267,
     346,   377,   443,   582,   583,   588,   597,   562,   563,   579,
     169,   230,   359,   595,   598,   603,   291,   428,   473,   537,
     292,   360,   429,   474,   475,   472,   471,   535,   575,   506,
     576,   599,   602,   604,   608,   458,    32,   100,    48,    49,
      33,   158,   222,   223,   284,   293,   294,   295,   296,   366,
      34,    35,    83,   177,   310,   150,   213,   151,   152,   488,
     460,   459,   489,   490,   297,   298,    36,   159,   225,   226,
     280,   415,   468,   528,   578,   548,   577,   503,   530,   531,
     534,    37,    59,    38,    55,    39,    62,    40,   334,   396,
     397,   324,   202,   193,    71,    42,    72,    68,    43,    44,
      91,   246,   131,   314,   384,   132,   338,    92,    93,    94,
     370,   133,   166,   316,   195,   196,   317,   250,   318,   319,
     258,   251,   170,   289,   469,   496,    45,    65
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -444
static const short yypact[] =
{
    -444,    46,  -444,  -444,   274,  -444,  -444,   642,  -444,    35,
     234,   329,   343,   643,  -444,  -444,  -444,  -444,  -444,  -444,
    -444,  -444,    51,   125,   566,   566,    51,    35,   452,  -444,
    -444,  -444,  -444,  -444,   494,  -444,  -444,  -444,  -444,  -444,
    -444,    76,   572,  -444,    51,  -444,   114,  -444,  -444,   198,
     156,  -444,   220,    51,  -444,   226,   242,    51,  -444,   318,
     310,  -444,   548,   314,  -444,   595,   189,  -444,    51,  -444,
    -444,    45,   572,  -444,    51,    45,   296,  -444,   338,   295,
      42,   124,   359,  -444,   124,    27,    32,   411,   245,   533,
    -444,    80,  -444,   207,  -444,   193,  -444,    51,  -444,  -444,
     407,  -444,  -444,  -444,   389,  -444,  -444,   350,    51,  -444,
    -444,   406,  -444,  -444,   351,    51,  -444,  -444,  -444,  -444,
     646,  -444,  -444,  -444,  -444,   647,  -444,   600,   397,    51,
     432,   483,  -444,    56,  -444,    51,  -444,   483,    51,    51,
    -444,   652,   428,  -444,  -444,  -444,  -444,  -444,   604,  -444,
    -444,   439,    98,   476,  -444,  -444,  -444,   450,   443,   455,
    -444,  -444,  -444,  -444,  -444,  -444,   533,   472,  -444,  -444,
    -444,   245,   523,   245,  -444,  -444,   524,  -444,  -444,  -444,
      51,  -444,   493,  -444,  -444,    51,  -444,   508,  -444,  -444,
    -444,  -444,  -444,   128,   511,   217,  -444,   530,  -444,    51,
    -444,  -444,   538,    51,  -444,  -444,   541,  -444,   215,   536,
    -444,   561,  -444,  -444,    42,    36,    35,    36,    85,   485,
    -444,  -444,    96,  -444,   162,   382,  -444,  -444,  -444,  -444,
     319,  -444,   570,   575,  -444,  -444,   569,   586,  -444,   587,
    -444,   356,  -444,   168,    51,  -444,   568,   211,   154,   568,
     275,  -444,   607,  -444,   635,    80,  -444,   628,  -444,  -444,
    -444,  -444,  -444,  -444,    51,   629,  -444,   446,   630,  -444,
    -444,  -444,  -444,  -444,  -444,    51,   632,   637,   423,  -444,
     222,  -444,  -444,  -444,  -444,   641,   225,  -444,  -444,    80,
    -444,  -444,   638,   612,   626,  -444,   130,   640,  -444,   657,
     657,  -444,  -444,   354,  -444,   257,    65,   426,  -444,  -444,
     639,  -444,  -444,   356,   644,   184,   645,   648,   634,  -444,
     627,   211,  -444,  -444,  -444,   600,  -444,   168,   211,   275,
    -444,   600,  -444,    51,    80,  -444,  -444,  -444,   636,  -444,
     569,  -444,   657,  -444,  -444,  -444,  -444,  -444,  -444,   169,
     377,   649,    51,   659,   195,  -444,   396,   420,  -444,   650,
     556,   654,  -444,  -444,  -444,    51,  -444,  -444,  -444,  -444,
    -444,  -444,  -444,   562,  -444,   454,  -444,    62,  -444,   534,
    -444,   378,  -444,   168,   211,  -444,  -444,  -444,    91,  -444,
     651,   653,   211,   655,   656,   658,   662,  -444,  -444,  -444,
      80,  -444,  -444,  -444,    75,  -444,  -444,  -444,  -444,  -444,
    -444,   661,  -444,   144,  -444,   298,  -444,   372,  -444,  -444,
    -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,
    -444,   626,  -444,   386,  -444,  -444,  -444,  -444,  -444,  -444,
    -444,  -444,  -444,  -444,  -444,   211,  -444,  -444,  -444,  -444,
    -444,    51,   663,  -444,   693,    51,  -444,  -444,   665,  -444,
    -444,   278,    51,  -444,    51,    51,  -444,   660,  -444,   484,
    -444,   664,   666,  -444,  -444,  -444,  -444,   667,  -444,  -444,
     674,  -444,   668,   669,  -444,   456,   361,  -444,    87,   671,
     203,   675,   677,    51,  -444,  -444,   196,  -444,    51,  -444,
     686,   686,  -444,   680,  -444,  -444,  -444,   679,   467,   388,
    -444,    51,  -444,  -444,   684,  -444,  -444,  -444,   195,   286,
     278,   278,  -444,    51,   672,   681,   208,   678,  -444,   673,
     686,  -444,   686,   676,  -444,  -444,    -2,   683,  -444,  -444,
    -444,  -444,  -444,    87,  -444,    87,   682,   685,  -444,  -444,
    -444,  -444,  -444,  -444,    51,   339,  -444,  -444,  -444,  -444,
    -444,   569,  -444,   670,  -444,  -444,   569,  -444,  -444,   569,
    -444,  -444,  -444,  -444,  -444,   687,    51,   688,   680,  -444,
     689,  -444,   474,  -444,  -444,  -444,  -444,  -444,  -444,    51,
     691,   692,   569,  -444,  -444,  -444,  -444,  -444,   319,   714,
    -444,  -444,  -444,   694,   690,  -444,  -444,   626,   695,   691,
    -444
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -444,  -333,    -4,   -40,  -444,  -444,  -444,  -444,  -444,  -444,
    -444,  -444,  -444,  -444,  -444,   227,  -444,  -444,  -444,  -444,
    -444,  -444,  -444,  -444,  -175,   165,   398,  -444,  -444,  -295,
    -444,  -443,  -444,  -444,   117,  -444,  -444,   132,  -444,  -444,
    -444,  -444,  -444,    94,  -444,  -444,   109,  -330,  -444,  -444,
     263,  -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,
    -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,
    -444,  -444,  -444,   522,  -444,   381,  -357,   383,  -444,  -444,
    -444,   -70,     3,  -444,  -444,   330,  -444,   535,  -444,  -335,
    -444,  -444,   -83,  -444,   160,  -296,  -444,  -444,  -444,   525,
     527,  -342,  -444,  -444,  -444,  -444,  -444,  -444,   251,   148,
     175,  -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,  -444,
     303,   506,   619,   753,   733,  -444,  -444,  -444,    88,     0,
    -444,  -444,   696,  -444,  -180,   -95,  -444,   -59,  -444,   588,
     -62,   -20,  -444,  -107,  -444,   507,   567,  -222,   276,  -197,
    -444,  -444,  -232,  -444,  -444,  -444,  -444,  -444
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -339
static const short yytable[] =
{
      28,   368,   236,   257,   259,   431,    54,    58,    61,    64,
     375,   154,    51,   247,   156,   417,   161,   486,    66,   413,
     194,    95,    76,   337,    74,    74,   329,   427,   157,   163,
      77,   430,   509,   160,   198,     6,   174,    90,   204,     6,
     149,   144,    97,   143,   564,     6,     3,   144,     6,   104,
      50,   109,     9,   111,     6,   116,   145,   358,   121,     6,
     363,   126,   145,   321,   128,   233,   376,   130,   328,   165,
     440,   130,   135,   -76,   457,   142,   148,   153,     6,     6,
     153,   167,   153,   340,    90,  -204,   272,  -135,     6,   164,
     144,    90,   146,   329,     6,  -174,    27,   281,   146,   217,
     147,   462,   398,   181,   182,   145,   147,   129,   261,    89,
     186,   187,    73,    73,   234,    87,   189,   441,   203,   442,
     -76,   191,   -76,   423,    99,   130,   389,     6,   168,   130,
      96,     6,    98,     6,   207,   209,   500,   211,    88,   502,
      89,   146,   282,   320,     9,   218,   228,   392,   536,   147,
       9,  -175,   464,   383,   244,   526,   221,     6,   462,   329,
     134,    67,   136,   285,   485,  -214,   227,    90,   453,    90,
     401,     6,   -37,   248,   149,   149,   237,   149,   149,   508,
     241,   239,   364,   543,   545,   175,   242,     6,    27,   245,
     243,   244,    89,   365,    27,   256,     6,   523,     6,   130,
     144,   101,   452,   445,   518,   463,   241,  -214,   102,   464,
     148,   148,   242,   148,   148,   276,   327,   244,   390,   270,
     320,   315,   -37,   205,   393,     6,   367,   105,     6,     6,
     553,   244,    87,   555,   465,    52,   241,     6,   371,   245,
     322,   127,   242,   524,   245,   171,   383,   244,     6,   607,
     519,   409,   410,   477,   253,   173,  -194,   525,   374,   172,
     341,   263,   264,   325,   254,   -72,   103,   354,   467,   326,
     357,   348,   106,   107,    -6,     5,   355,     6,   108,   487,
     400,     6,   355,   144,    87,     7,    53,   544,   110,     6,
       8,   144,   153,   381,     9,   368,   140,    10,     6,    11,
     388,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,   245,   -72,   -72,    22,    23,    24,    25,    26,   112,
     290,     6,  -146,   245,   465,  -101,  -101,   331,  -101,   395,
      56,  -193,     6,   332,   409,   410,  -101,  -101,    27,  -146,
     571,   138,   409,   410,    60,   141,     6,   343,   276,   276,
     411,   179,   184,     6,     6,   372,   117,   466,   467,     6,
     122,   434,   -66,   -66,   113,   114,   566,  -101,  -101,   440,
     115,  -146,     8,   569,  -101,  -101,  -101,   139,   403,   245,
     -47,    57,  -146,  -146,   245,    14,    15,    16,    17,    18,
      19,    20,    21,   192,   344,   572,   440,   414,   465,  -216,
     455,   -66,   180,   185,   155,  -216,   -66,   -66,   313,   -66,
     -66,   -66,   -66,   592,   242,   162,   441,  -216,   516,   176,
      89,   416,  -216,  -216,   352,   -47,   -47,   378,   287,  -216,
     313,   470,   467,   362,   -66,   -66,   242,   546,   547,   476,
     224,  -216,   178,   441,  -216,   539,  -216,   395,  -216,   197,
     481,   483,  -216,  -162,   343,  -216,  -216,   411,   491,   183,
     492,   493,   343,   495,   343,   501,  -162,    50,  -216,   -31,
     199,   -31,  -216,   -66,   353,   343,  -216,   212,   -66,  -216,
    -216,   -66,   -66,   -66,   -66,   -66,   274,     6,     6,   522,
      78,    50,   216,   302,   527,    84,   220,   -24,   275,  -162,
      79,   344,   345,   221,    80,   497,    81,   540,    85,   344,
     439,   344,   515,   224,   411,   411,   411,   411,   229,   549,
     551,   589,   344,   538,   232,   557,     6,   590,    80,   560,
      81,   -29,   423,   -29,   235,   200,   498,   201,   437,     8,
     499,    86,   301,   302,   437,   269,   238,   271,   273,   118,
     570,     6,    14,    15,    16,    17,    18,    19,    20,    21,
     192,   240,   419,   420,   252,   302,    18,    19,    20,     6,
     301,   302,   581,   421,   422,     6,  -338,   301,   302,   406,
     408,   435,    69,   255,   260,   581,   303,   262,   265,   304,
     305,   306,   307,   444,   119,   120,   123,    89,     6,    18,
      19,    20,   391,     6,   423,    18,    19,    20,   394,   435,
     266,   424,   425,   426,   303,   436,     8,   304,   305,   306,
     307,   303,   323,   299,   304,   305,   306,   307,   300,    14,
      15,    16,    17,    18,    19,    20,    21,   192,     6,   311,
     312,   124,   125,    46,    63,    47,     6,   188,   190,     6,
       6,     8,   333,   210,  -131,     6,   214,   361,   215,   362,
     407,   494,     6,     6,    14,    15,    16,    17,    18,    19,
      20,    21,   335,   550,   556,     6,     6,   559,   558,     6,
     558,   339,   342,   347,   349,   350,   356,   -95,   363,   -96,
     369,    89,   380,   387,   399,   405,   480,   382,   385,   418,
     529,   386,   423,   610,   446,   379,   593,   600,   448,   451,
     450,   586,   447,   461,   484,   449,   504,   574,   505,   511,
     510,   512,   479,   513,   517,   533,   552,   520,   -99,   521,
     542,   554,   565,   601,   584,   567,   507,   587,   568,   594,
     580,   596,   606,   605,   283,   433,   432,   591,   609,   268,
     288,   286,   532,   585,   478,   330,   206,    41,    75,   231,
     249,   336,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   137
};

static const short yycheck[] =
{
       4,   297,   177,   200,   201,   362,    10,    11,    12,    13,
     305,    81,     9,   193,    84,   357,    86,   460,    22,   354,
     127,    41,    26,   255,    24,    25,   248,   360,     1,    88,
      27,   361,   475,     1,   129,     3,    95,    41,   133,     3,
      80,     5,    42,     1,    46,     3,     0,     5,     3,    53,
      15,    55,    20,    57,     3,    59,    20,   289,    62,     3,
      62,    65,    20,   243,    68,   172,     1,    71,   248,    89,
       8,    75,    72,     8,   404,    79,    80,    81,     3,     3,
      84,     1,    86,   258,    88,    58,     1,    60,     3,    89,
       5,    95,    56,   315,     3,    53,    64,     1,    56,     1,
      64,    14,   334,   107,   108,    20,    64,    62,   203,    64,
     114,   115,    24,    25,   173,    39,   120,    55,    62,    57,
      55,   125,    57,    48,    10,   129,   323,     3,    48,   133,
      42,     3,    44,     3,   138,   139,   469,   141,    62,   469,
      64,    56,    46,    52,    20,    47,   166,   327,   505,    64,
      20,    53,    65,    62,    63,   497,    60,     3,    14,   381,
      72,    36,    74,     1,   459,     3,   166,   171,   400,   173,
       1,     3,     3,   193,   214,   215,   180,   217,   218,   474,
      52,   185,    52,   518,   519,    97,    58,     3,    64,   193,
      62,    63,    64,    63,    64,   199,     3,     1,     3,   203,
       5,     3,   399,   383,     1,    61,    52,    45,    52,    65,
     214,   215,    58,   217,   218,   219,    62,    63,   325,   216,
      52,   241,    53,   135,   331,     3,   296,     1,     3,     3,
      22,    63,    39,   528,    26,     1,    52,     3,   300,   243,
     244,    52,    58,    47,   248,    38,    62,    63,     3,   606,
      47,    56,    57,   450,    37,    62,    53,    61,     1,    52,
     264,    46,    47,    52,    47,     8,    46,    45,    60,    58,
      45,   275,    46,    47,     0,     1,   280,     3,    52,     1,
     342,     3,   286,     5,    39,    11,    52,     1,    46,     3,
      16,     5,   296,   313,    20,   591,     1,    23,     3,    25,
     320,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,   315,    55,    56,    40,    41,    42,    43,    44,     1,
       1,     3,     3,   327,    26,     6,     7,    52,     9,   333,
       1,    53,     3,    58,    56,    57,    17,    18,    64,    20,
       1,    45,    56,    57,     1,    50,     3,     8,   352,   353,
     354,     1,     1,     3,     3,     1,    46,    59,    60,     3,
      46,   365,     8,     9,    46,    47,   541,    48,    49,     8,
      52,    52,    16,   548,    55,    56,    57,    39,     1,   383,
       3,    52,    63,    64,   388,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    55,    56,     8,     1,    26,     3,
     404,    47,    52,    52,    45,     9,    52,    53,    52,    55,
      56,    57,    58,   588,    58,     4,    55,    21,    57,    12,
      64,     1,    26,     3,     1,    48,    49,     1,    46,     9,
      52,    59,    60,    47,     8,     9,    58,   520,   521,    53,
      58,    21,    53,    55,    48,    57,    26,   451,    52,    52,
     454,   455,    56,     1,     8,    59,    60,   461,   462,    53,
     464,   465,     8,   467,     8,   469,    14,    15,    48,    46,
      38,    48,    52,    47,    51,     8,    56,    49,    52,    59,
      60,    55,    56,    57,    58,    59,     1,     3,     3,   493,
      38,    15,    53,     9,   498,     1,    46,    45,    13,    47,
      48,    55,    56,    60,    52,    21,    54,   511,    14,    55,
      56,    55,    56,    58,   518,   519,   520,   521,    46,   523,
     524,    47,    55,    56,     1,   529,     3,    53,    52,   533,
      54,    46,    48,    48,    10,    52,    52,    54,   373,    16,
      56,    47,     8,     9,   379,   215,    53,   217,   218,     1,
     554,     3,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    53,     6,     7,    53,     9,    33,    34,    35,     3,
       8,     9,   576,    17,    18,     3,    53,     8,     9,   352,
     353,    47,    16,    53,    46,   589,    52,    46,    52,    55,
      56,    57,    58,    59,    46,    47,     1,    64,     3,    33,
      34,    35,   326,     3,    48,    33,    34,    35,   332,    47,
      49,    55,    56,    57,    52,    53,    16,    55,    56,    57,
      58,    52,    54,    53,    55,    56,    57,    58,    53,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     3,    53,
      53,    46,    47,     1,     1,     3,     3,     1,     1,     3,
       3,    16,    45,     1,    12,     3,    52,    45,    54,    47,
       1,     1,     3,     3,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     1,     1,     3,     3,     1,   530,     3,
     532,    53,    53,    53,    52,    48,    45,    49,    62,    49,
      33,    64,    53,    59,    58,    46,     3,    53,    53,    49,
      14,    53,    48,   609,    53,   307,   589,   598,    53,    47,
      52,   579,    59,    52,    49,    59,    52,    47,    52,    45,
      53,    53,    59,    54,    53,    45,    45,    52,    49,    52,
      46,    53,    49,    19,    46,    53,   473,    48,    53,    48,
      53,    49,    52,    49,   222,   364,   363,   587,    53,   214,
     225,   224,   501,   578,   451,   249,   137,     4,    25,   171,
     193,   254,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    75
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    70,    71,     0,    72,     1,     3,    11,    16,    20,
      23,    25,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    40,    41,    42,    43,    44,    64,    68,    73,
      74,    76,   132,   136,   146,   147,   162,   177,   179,   181,
     183,   189,   191,   194,   195,   222,     1,     3,   134,   135,
      15,   148,     1,    52,    68,   180,     1,    52,    68,   178,
       1,    68,   182,     1,    68,   223,    68,    36,   193,    16,
      68,   190,   192,   194,   195,   190,    68,   148,    38,    48,
      52,    54,    78,   148,     1,    14,    47,    39,    62,    64,
      68,   196,   203,   204,   205,   207,   194,   195,   194,    10,
     133,     3,    52,    46,    68,     1,    46,    47,    52,    68,
      46,    68,     1,    46,    47,    52,    68,    46,     1,    46,
      47,    68,    46,     1,    46,    47,    68,    52,    68,    62,
      68,   198,   201,   207,   194,   195,   194,   198,    45,    39,
       1,    50,    68,     1,     5,    20,    56,    64,    68,    69,
     151,   153,   154,    68,   147,    45,   147,     1,   137,   163,
       1,   147,     4,   203,   195,   207,   208,     1,    48,   106,
     218,    38,    52,    62,   203,   194,    12,   149,    53,     1,
      52,    68,    68,    53,     1,    52,    68,    68,     1,    68,
       1,    68,    37,   189,   209,   210,   211,    52,   201,    38,
      52,    54,   188,    62,   201,   194,   188,    68,    75,    68,
       1,    68,    49,   152,    52,    54,    53,     1,    47,    77,
      46,    60,   138,   139,    58,   164,   165,   195,   207,    46,
     107,   205,     1,   209,   203,    10,    90,    68,    53,    68,
      53,    52,    58,    62,    63,    68,   197,   200,   207,   212,
     213,   217,    53,    37,    47,    53,    68,   215,   216,   215,
      46,   201,    46,    46,    47,    52,    49,    95,   153,   151,
     148,   151,     1,   151,     1,    13,    68,    79,    80,    81,
     166,     1,    46,   139,   140,     1,   166,    46,   165,   219,
       1,   112,   116,   141,   142,   143,   144,   160,   161,    53,
      53,     8,     9,    52,    55,    56,    57,    58,    67,    91,
     150,    53,    53,    52,   199,   207,   209,   212,   214,   215,
      52,   200,    68,    54,   187,    52,    58,    62,   200,   213,
     187,    52,    58,    45,   184,    37,   211,   218,   202,    53,
      90,    68,    53,     8,    55,    56,    96,    53,    68,    52,
      48,    85,     1,    51,    45,    68,    45,    45,   218,   108,
     117,    45,    47,    62,    52,    63,   145,   147,   161,    33,
     206,   206,     1,    92,     1,    95,     1,    97,     1,    92,
      53,   207,    53,    62,   200,    53,    53,    59,   207,   215,
     209,   214,   200,   209,   214,    68,   185,   186,   218,    58,
     206,     1,    82,     1,    86,    46,    81,     1,    81,    56,
      57,    68,    69,   155,     1,   167,     1,   167,    49,     6,
       7,    17,    18,    48,    55,    56,    57,    67,   113,   118,
     113,   142,   143,   141,    68,    47,    53,    91,    93,    56,
       8,    55,    57,    98,    59,   200,    53,    59,    53,    59,
      52,    47,   215,   218,    84,    68,    87,   113,   131,   157,
     156,    52,    14,    61,    65,    26,    59,    60,   168,   220,
      59,   122,   121,   114,   119,   120,    53,   215,   186,    59,
       3,    68,    83,    68,    49,    95,    97,     1,   155,   158,
     159,    68,    68,    68,     1,    68,   221,    21,    52,    56,
      67,    68,   113,   173,    52,    52,   125,   116,    95,    97,
      53,    45,    53,    54,    88,    56,    57,    53,     1,    47,
      52,    52,    68,     1,    47,    61,   167,    68,   169,    14,
     174,   175,   174,    45,   176,   123,   142,   115,    56,    57,
      68,    89,    46,   155,     1,   155,   158,   158,   171,    68,
       1,    68,    45,    22,    53,    95,     1,    68,   175,     1,
      68,    90,   103,   104,    46,    49,    90,    53,    53,    90,
      68,     1,    56,    94,    47,   124,   126,   172,   170,   105,
      53,    68,    99,   100,    46,   176,   103,    48,   101,    47,
      53,   160,    90,   100,    48,   109,    49,   102,   110,   127,
     112,    19,   128,   111,   129,    49,    52,   142,   130,    53,
     109
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
    { yyval.yt_INT = Int( yyvsp[0].yt_integer ); pf_setfileline( yyval.yt_INT ); ;}
    break;

  case 3:
    { yyval.yt_ID = Id( Str( yyvsp[0].yt_casestring )); pf_setfileline( yyval.yt_ID ); ;}
    break;

  case 4:
    { yyval.yt_INT = Int( yyvsp[0].yt_integer ); pf_setfileline( yyval.yt_INT ); ;}
    break;

  case 5:
    { /* initialize variables and scanner state */
	  pl_phylum = 0;
	  pl_withvariables = Nilwithexpressions();
	  do_NORMAL();
	;}
    break;

  case 9:
    { Thephylumdeclarations = mergephylumdeclarations( yyvsp[0].yt_phylumdeclaration, Thephylumdeclarations ); ;}
    break;

  case 10:
    { Therwdeclarations = Consrwdeclarations( yyvsp[0].yt_rwdeclaration, Therwdeclarations ); ;}
    break;

  case 11:
    { Thefndeclarations = Consfndeclarations( yyvsp[0].yt_fndeclaration, Thefndeclarations ); ;}
    break;

  case 12:
    { ;}
    break;

  case 13:
    { Theunparsedeclarations = Consunparsedeclarations( yyvsp[0].yt_unparsedeclaration, Theunparsedeclarations ); ;}
    break;

  case 14:
    { ;}
    break;

  case 15:
    { ;}
    break;

  case 16:
    { ;}
    break;

  case 17:
    { Thelanguages = merge_languagenames( yyvsp[0].yt_languagenames, Thelanguages ); ;}
    break;

  case 18:
    { Thebaseclasses = Consbaseclass_declarations( yyvsp[0].yt_baseclass_decl, Thebaseclasses ); ;}
    break;

  case 19:
    { yyval.yt_baseclass_decl=BaseClassDecl( yyvsp[-3].yt_ID, yyvsp[-1].yt_baseclass_list ); ;}
    break;

  case 20:
    { yyval.yt_baseclass_list=Consbaseclass_list(yyvsp[0].yt_ID,Nilbaseclass_list()); ;}
    break;

  case 21:
    { yyval.yt_baseclass_list=Consbaseclass_list(yyvsp[0].yt_ID,yyvsp[-2].yt_baseclass_list); ;}
    break;

  case 22:
    { pl_phylum = yyvsp[-2].yt_ID; ;}
    break;

  case 23:
    { yyval.yt_phylumdeclaration = PhylumDeclaration( yyvsp[-6].yt_ID, yyvsp[-5].yt_storageoption, yyvsp[-2].yt_productionblock, yyvsp[-1].yt_Ccode_option ); v_extendoccur( pl_phylum, ITUserPhylum( yyval.yt_phylumdeclaration ) ); ;}
    break;

  case 24:
    { yyval.yt_storageoption = NoStorageOption(); ;}
    break;

  case 25:
    { yyval.yt_storageoption = PositiveStorageOption( yyvsp[-1].yt_ID ); v_extendoccur( yyvsp[-1].yt_ID, ITStorageClass() ); ;}
    break;

  case 26:
    { yyval.yt_storageoption = NegativeStorageOption( yyvsp[-1].yt_ID ); v_extendoccur( yyvsp[-1].yt_ID, ITStorageClass() ); ;}
    break;

  case 27:
    { yyval.yt_storageoption = NoStorageOption(); ;}
    break;

  case 28:
    { yyval.yt_storageoption = NoStorageOption(); ;}
    break;

  case 29:
    { yyval.yt_productionblock = Emptyproductionblock(); ;}
    break;

  case 30:
    { yyval.yt_productionblock = ListAlternatives( makeListAlternatives( pl_phylum, yyvsp[0].yt_ID ), yyvsp[0].yt_ID );
	  Theargsnumbers = insert_in_argsnumbers(0, Theargsnumbers);
	  Theargsnumbers = insert_in_argsnumbers(2, Theargsnumbers);
	;}
    break;

  case 31:
    { yyval.yt_productionblock = NonlistAlternatives( yyvsp[0].yt_alternatives ); ;}
    break;

  case 32:
    { yyval.yt_alternatives = Consalternatives( yyvsp[0].yt_alternative, Nilalternatives() ); ;}
    break;

  case 33:
    { yyerrok; yyval.yt_alternatives = Consalternatives( yyvsp[0].yt_alternative, yyvsp[-2].yt_alternatives ); ;}
    break;

  case 34:
    { yyval.yt_alternatives = Nilalternatives(); ;}
    break;

  case 35:
    { yyval.yt_alternatives = yyvsp[-2].yt_alternatives; ;}
    break;

  case 36:
    { yyerrok; yyval.yt_alternatives=Consalternatives( yyvsp[0].yt_alternative, yyvsp[-2].yt_alternatives ); ;}
    break;

  case 37:
    { pg_no_of_arguments = 0; ;}
    break;

  case 38:
    { Theargsnumbers = insert_in_argsnumbers(pg_no_of_arguments, Theargsnumbers); ;}
    break;

  case 39:
    { yyval.yt_alternative = Alternative( yyvsp[-5].yt_ID, yyvsp[-2].yt_arguments );
	  v_extendoccur( yyvsp[-5].yt_ID, ITUserOperator( yyval.yt_alternative, pl_phylum ) ); ;}
    break;

  case 40:
    { yyval.yt_alternative = Alternative( yyvsp[-2].yt_ID, Nilarguments()); ;}
    break;

  case 41:
    { yyval.yt_arguments = Nilarguments(); ;}
    break;

  case 42:
    { yyval.yt_arguments = Consarguments( yyvsp[0].yt_ID, yyvsp[-1].yt_arguments ); pg_no_of_arguments++; ;}
    break;

  case 43:
    { yyvsp[0].yt_ID->named_subphylum = yyvsp[-2].yt_casestring;
	  yyval.yt_arguments = Consarguments( yyvsp[0].yt_ID, yyvsp[-3].yt_arguments ); pg_no_of_arguments++; ;}
    break;

  case 44:
    { yyval.yt_Ccode_option = CcodeOption( Nilattributes(), NilCtexts() ); ;}
    break;

  case 45:
    { yyval.yt_Ccode_option = CcodeOption( yyvsp[-2].yt_attributes, yyvsp[-1].yt_Ctexts ); ;}
    break;

  case 46:
    { yyval.yt_Ccode_option = CcodeOption( Nilattributes(), NilCtexts() ); ;}
    break;

  case 47:
    { yyval.yt_attributes = Nilattributes(); ;}
    break;

  case 48:
    { yyval.yt_attributes = Consattributes( yyvsp[0].yt_attribute, yyvsp[-1].yt_attributes ); ;}
    break;

  case 49:
    { yyval.yt_attribute = Attribute( yyvsp[-3].yt_ID, yyvsp[-2].yt_ID, yyvsp[-1].yt_attribute_initialisation_option ); ;}
    break;

  case 50:
    { yyval.yt_attribute_initialisation_option = Noattribute_initialisation(); ;}
    break;

  case 51:
    { do_CEXPR(); ;}
    break;

  case 52:
    { do_NORMAL();
	  yyval.yt_attribute_initialisation_option = Yesattribute_initialisation( yyvsp[0].yt_Cexpression ); ;}
    break;

  case 53:
    { yyval.yt_Cexpression = NilCexpression(); pf_setfileline( yyval.yt_Cexpression ); ;}
    break;

  case 54:
    { yyval.yt_Cexpression = ConsCexpression( yyvsp[0].yt_Cexpression_elem, yyvsp[-1].yt_Cexpression ); yyval.yt_Cexpression->file = yyvsp[-1].yt_Cexpression->file; yyval.yt_Cexpression->line = yyvsp[-1].yt_Cexpression->line; ;}
    break;

  case 55:
    { yyval.yt_Cexpression_elem = CExpressionPart( yyvsp[0].yt_casestring ); ;}
    break;

  case 56:
    { yyval.yt_Cexpression_elem = CExpressionDollarvar( yyvsp[0].yt_INT ); ;}
    break;

  case 57:
    { yyval.yt_Cexpression_elem = CExpressionNl(); ;}
    break;

  case 58:
    { yyval.yt_Cexpression_elem = CExpressionDQ( yyvsp[-1].yt_CexpressionDQ ); ;}
    break;

  case 59:
    { yyval.yt_Cexpression_elem = CExpressionSQ( yyvsp[-1].yt_CexpressionSQ ); ;}
    break;

  case 60:
    { yyval.yt_Cexpression_elem = CExpressionPack( yyvsp[-1].yt_Cexpression ); ;}
    break;

  case 61:
    { yyval.yt_Cexpression_elem = CExpressionArray( yyvsp[-1].yt_Cexpression ); ;}
    break;

  case 62:
    { yyval.yt_Cexpression_elem = CExpressionPart(mkcasestring("")); ;}
    break;

  case 63:
    { yyval.yt_Cexpression_elem = CExpressionPart(mkcasestring("")); ;}
    break;

  case 64:
    { yyval.yt_Cexpression_elem = CExpressionPart(mkcasestring("")); ;}
    break;

  case 65:
    { yyval.yt_Cexpression_elem = CExpressionArray( ConsCexpression( CExpressionPart(mkcasestring("")), NilCexpression())); ;}
    break;

  case 66:
    { yyval.yt_Cexpression = NilCexpression(); pf_setfileline( yyval.yt_Cexpression ); ;}
    break;

  case 67:
    { yyval.yt_Cexpression = ConsCexpression( yyvsp[0].yt_Cexpression_elem, yyvsp[-1].yt_Cexpression ); yyval.yt_Cexpression->file = yyvsp[-1].yt_Cexpression->file; yyval.yt_Cexpression->line = yyvsp[-1].yt_Cexpression->line; ;}
    break;

  case 68:
    { yyval.yt_Cexpression_elem = yyvsp[0].yt_Cexpression_elem; ;}
    break;

  case 69:
    { yyval.yt_Cexpression_elem = CExpressionPart( mkcasestring(",") ); ;}
    break;

  case 72:
    { yyval.yt_CexpressionDQ = NilCexpressionDQ(); ;}
    break;

  case 73:
    { yyval.yt_CexpressionDQ = ConsCexpressionDQ( yyvsp[0].yt_CexpressionDQ_elem, yyvsp[-1].yt_CexpressionDQ ); ;}
    break;

  case 74:
    { yyval.yt_CexpressionDQ_elem = CExpressionDQPart( yyvsp[0].yt_casestring ); ;}
    break;

  case 75:
    { yyval.yt_CexpressionDQ_elem = CExpressionDQNl(); ;}
    break;

  case 76:
    { yyval.yt_CexpressionSQ = NilCexpressionSQ(); ;}
    break;

  case 77:
    { yyval.yt_CexpressionSQ = ConsCexpressionSQ( yyvsp[0].yt_CexpressionSQ_elem, yyvsp[-1].yt_CexpressionSQ ); ;}
    break;

  case 78:
    { yyval.yt_CexpressionSQ_elem = CExpressionSQPart( yyvsp[0].yt_casestring ); ;}
    break;

  case 79:
    { yyval.yt_CexpressionSQ_elem = CExpressionSQNl(); ;}
    break;

  case 80:
    { yyval.yt_idCexpressions = ConsidCexpressions( yyvsp[0].yt_idCexpression, NilidCexpressions() ); ;}
    break;

  case 81:
    { yyval.yt_idCexpressions = ConsidCexpressions( yyvsp[0].yt_idCexpression, yyvsp[-2].yt_idCexpressions ); ;}
    break;

  case 82:
    { do_CEXPR(); ;}
    break;

  case 83:
    { do_NORMAL(); ;}
    break;

  case 84:
    { yyval.yt_idCexpression = IdCexpression( yyvsp[-3].yt_ID, yyvsp[-1].yt_Cexpression ); ;}
    break;

  case 85:
    { yyval.yt_withexpression = WECexpression( yyvsp[0].yt_Cexpression ); pf_setstacktopfileline( yyval.yt_withexpression ); ;}
    break;

  case 86:
    { yyval.yt_withexpressions = Conswithexpressions( yyvsp[0].yt_withexpression, Nilwithexpressions() ); ;}
    break;

  case 87:
    { pf_pushfileline(); ;}
    break;

  case 88:
    { yyval.yt_withexpressions = Conswithexpressions( yyvsp[0].yt_withexpression, yyvsp[-3].yt_withexpressions );
	  pf_popfileline();  ;}
    break;

  case 89:
    { do_MainC(); pf_pushfileline(); pf_pushwithvariable(); pf_resetwithvariable(); ;}
    break;

  case 90:
    { do_NORMAL(); ;}
    break;

  case 91:
    { yyerrok; yyval.yt_Ctext = yyvsp[-2].yt_Ctext; pf_setstacktopfileline( yyval.yt_Ctext ); pf_popfileline(); pf_popwithvariable(); ;}
    break;

  case 92:
    { do_MainC(); pf_pushfileline(); pf_pushwithvariable(); pf_resetwithvariable(); ;}
    break;

  case 93:
    { do_C(); ;}
    break;

  case 94:
    { yyerrok; yyval.yt_Ctext = yyvsp[-2].yt_Ctext; pf_setstacktopfileline( yyval.yt_Ctext ); pf_popfileline(); pf_popwithvariable(); ;}
    break;

  case 95:
    { yyval.yt_Ctext = yyvsp[0].yt_Ctext; pf_setstacktopfileline( yyval.yt_Ctext ); ;}
    break;

  case 96:
    { Ctext_elem tmp = CTextWithexpression( pf_topwithvariable(), yyvsp[0].yt_withcases, NotInForeachContext() );
	  pf_setstacktopfileline( tmp );
	  yyval.yt_Ctext = ConsCtext( tmp, NilCtext() );
	  pf_setstacktopfileline( yyval.yt_Ctext );
	  if (! non_default_outmostpattern ) v_report(NonFatal( NoFileLine(), Problem1S( "can not infer type from 'default' pattern(s)" )));
	  non_default_outmostpattern = 0; ;}
    break;

  case 97:
    { yyval.yt_Ctext = NilCtext(); ;}
    break;

  case 98:
    { do_C(); pf_pushfileline(); pf_push_no_operators(); ;}
    break;

  case 99:
    { do_NORMAL(); ;}
    break;

  case 100:
    { yyval.yt_Ctext = yyvsp[-2].yt_Ctext; pf_setstacktopfileline( yyval.yt_Ctext ); pf_popfileline(); pf_pop_no_operators(); ;}
    break;

  case 101:
    { yyval.yt_Ctext = NilCtext(); pf_setfileline( yyval.yt_Ctext ); ;}
    break;

  case 102:
    { pf_pushfileline(); ;}
    break;

  case 103:
    { yyval.yt_Ctext = ConsCtext( yyvsp[0].yt_Ctext_elem, yyvsp[-2].yt_Ctext ); pf_setstacktopfileline( yyvsp[0].yt_Ctext_elem ); pf_popfileline(); yyval.yt_Ctext->file = yyvsp[-2].yt_Ctext->file; yyval.yt_Ctext->line = yyvsp[-2].yt_Ctext->line; ;}
    break;

  case 104:
    { yyval.yt_Ctext_elem = CTextLine( yyvsp[0].yt_casestring ); ;}
    break;

  case 105:
    { yyval.yt_Ctext_elem = CTextDollarVar( yyvsp[0].yt_INT ); ;}
    break;

  case 106:
    { yyval.yt_Ctext_elem = CTextNl( mkinteger(1) ); ;}
    break;

  case 107:
    { yyval.yt_Ctext_elem = CTextNl( yyvsp[0].yt_integer ); ;}
    break;

  case 108:
    { do_CEXPRDQ(); ;}
    break;

  case 109:
    { do_C();
	  yyval.yt_Ctext_elem = CTextCexpressionDQ( yyvsp[-1].yt_CexpressionDQ ); ;}
    break;

  case 110:
    { do_CEXPRSQ(); ;}
    break;

  case 111:
    { do_C();
	  yyval.yt_Ctext_elem = CTextCexpressionSQ( yyvsp[-1].yt_CexpressionSQ ); ;}
    break;

  case 112:
    { do_C();
	  yyval.yt_Ctext_elem = CTextCbody( yyvsp[0].yt_Ctext ); ;}
    break;

  case 113:
    { do_NORMAL(); ;}
    break;

  case 114:
    { do_C();
	  yyval.yt_Ctext_elem = yyvsp[0].yt_Ctext_elem; ;}
    break;

  case 115:
    { do_NORMAL(); ;}
    break;

  case 116:
    { do_CEXPR(); pf_pushfileline(); ;}
    break;

  case 117:
    { do_NORMAL(); pf_popfileline(); ;}
    break;

  case 118:
    { do_C();
	  yyval.yt_Ctext_elem = CTextWithexpression( yyvsp[-5].yt_withexpressions, yyvsp[-1].yt_withcases, NotInForeachContext() );
	  if (! non_default_outmostpattern ) v_report(NonFatal( NoFileLine(), Problem1S( "can not infer type from 'default' pattern(s)" )));
	  non_default_outmostpattern = 0; ;}
    break;

  case 119:
    { non_default_outmostpattern = 0; ;}
    break;

  case 120:
    {
	      pf_pushidCexpressions(yyvsp[-2].yt_idCexpressions);
	    ;}
    break;

  case 121:
    { patternchains pc_list = Conspatternchains(yyvsp[-7].yt_patternchain, Nilpatternchains());
	  patternchains rewritten = pc_list->rewrite(base_rview);
	  withexpressions wexpr = pf_gen_foreachwith_vars( yyvsp[-4].yt_idCexpressions );
	  Ctext_elem ctwe =
		    CTextWithexpression(
			wexpr,
			Conswithcases(
			    Withcase(
				syn_patternchains_fileline(
				    rewritten, pg_filename, pg_lineno ),
				yyvsp[-2].yt_Ctext ),
			    Nilwithcases()
			),
			InForeachContext(yyvsp[-7].yt_patternchain) );
	  Ctext ctxts0 = NilCtext();
	  Ctext ctxts1 = ConsCtext( ctwe, ctxts0 );
	  pf_setstacktopfileline( ctwe );
	  pf_setstacktopfileline( ctxts0 );
	  pf_setstacktopfileline( ctxts1 );
	  check_no_patternchaingroup_in_patternchain(
		  yyvsp[-7].yt_patternchain->file, yyvsp[-7].yt_patternchain->line, yyvsp[-7].yt_patternchain, "'foreach variable/pattern'" );
	  yyval.yt_Ctext_elem = CTextForeachexpression( yyvsp[-7].yt_patternchain, yyvsp[-4].yt_idCexpressions, wexpr, ctxts1, yyvsp[0].yt_foreach_after);
	  pf_popidCexpressions();
	;}
    break;

  case 122:
    { yyval.yt_foreach_after = NoForeachAfter(); ;}
    break;

  case 123:
    { do_NORMAL(); pf_pushfileline(); ;}
    break;

  case 124:
    { non_default_outmostpattern = 0; ;}
    break;

  case 125:
    { patternchains pc_list = Conspatternchains(yyvsp[-3].yt_patternchain, Nilpatternchains());
	  patternchains rewritten = pc_list->rewrite(base_rview);
	  withexpressions wexpr = pf_gen_foreachwith_listvars( pf_topidCexpressions() );
	  Ctext_elem ctwe =
	      CTextWithexpression(
		      wexpr,
		      Conswithcases(
			  Withcase(
			      syn_patternchains_fileline(
				  rewritten, pg_filename, pg_lineno ),
			      yyvsp[0].yt_Ctext ),
			  Nilwithcases()
				   ),
		      InForeachContext(yyvsp[-3].yt_patternchain) );
	  Ctext ctxts0 = NilCtext();
	  Ctext ctxts1 = ConsCtext( ctwe, ctxts0 );
	  pf_setstacktopfileline( ctwe );
	  pf_setstacktopfileline( ctxts0 );
	  pf_setstacktopfileline( ctxts1 );
	  check_no_patternchaingroup_or_pattern_in_patternchain(
		yyvsp[-3].yt_patternchain->file, yyvsp[-3].yt_patternchain->line, yyvsp[-3].yt_patternchain, "'foreach variable/pattern'" );
	  yyval.yt_foreach_after = ForeachAfter( yyvsp[-3].yt_patternchain, pf_topidCexpressions(), wexpr, ctxts1 );
	  pf_setstacktopfileline( yyval.yt_foreach_after );
	  pf_popfileline();
	;}
    break;

  case 126:
    { yyval.yt_Ctexts = NilCtexts(); ;}
    break;

  case 127:
    { yyval.yt_Ctexts = ConsCtexts( yyvsp[0].yt_Ctext, NilCtexts() ); ;}
    break;

  case 128:
    { pf_pushfileline(); ;}
    break;

  case 129:
    {{includedeclaration pl_includedeclaration = IncludeDeclaration( yyvsp[0].yt_casestring );
	  set_includefiles( yyvsp[-3].yt_includefiles, pl_includedeclaration );
	  pf_setstacktopfileline( pl_includedeclaration );
	  pf_popfileline();
	};}
    break;

  case 130:
    { yyval.yt_includedeclaration =IncludeDeclaration(mkcasestring("")); ;}
    break;

  case 131:
    {{ includefile pl_includefile = IncludeFile( mkcasestring(INC_CODE) );
	  yyval.yt_includefiles = Consincludefiles( pl_includefile, Nilincludefiles() );
	};}
    break;

  case 132:
    { yyval.yt_includefiles = yyvsp[0].yt_includefiles; ;}
    break;

  case 133:
    {{ includefile pl_includefile = IncludeFile( yyvsp[0].yt_casestring );
	  if ((pl_includefile->inc_type == include_file) || (pl_includefile->inc_type == include_header)) {
		yyval.yt_includefiles = Consincludefiles( pl_includefile, Nilincludefiles() );
	  } else {
	    v_report(NonFatal( NoFileLine(), ProblemSC( "unknown include redirection keyword:", yyvsp[0].yt_casestring )));
		yyval.yt_includefiles = Nilincludefiles();
	  }
	};}
    break;

  case 134:
    {{ includefile pl_includefile = IncludeFile( yyvsp[0].yt_casestring );
	  if ((pl_includefile->inc_type == include_file) || (pl_includefile->inc_type == include_header)) {
		yyval.yt_includefiles = Consincludefiles( pl_includefile, yyvsp[-1].yt_includefiles );
	  } else {
	    v_report(NonFatal( NoFileLine(), ProblemSC( "unknown include redirection keyword:", yyvsp[0].yt_casestring )));
		yyval.yt_includefiles = yyvsp[-1].yt_includefiles;
	  }
	};}
    break;

  case 135:
    { if (! non_default_outmostpattern ) v_report(NonFatal( NoFileLine(), Problem1S( "can not infer type from 'default' pattern(s)" )));
	  non_default_outmostpattern = 0; ;}
    break;

  case 136:
    { yyerrok; yyval.yt_rwdeclaration = RwDeclaration( yyvsp[-4].yt_outmostpatterns, yyvsp[-1].yt_rewriteclauses ); ;}
    break;

  case 137:
    { yyval.yt_rewriteclauses = Consrewriteclauses( yyvsp[0].yt_rewriteclause, Nilrewriteclauses() ); ;}
    break;

  case 138:
    { yyval.yt_rewriteclauses = Consrewriteclauses( yyvsp[0].yt_rewriteclause, yyvsp[-1].yt_rewriteclauses ); ;}
    break;

  case 139:
    { if (yyvsp[-3].yt_viewnames->eq(Nilviewnames())) {
	      yyval.yt_rewriteclause = RewriteClause( Consviewnames( Id( Str( mkcasestring( "base_rview" ))), yyvsp[-3].yt_viewnames ), yyvsp[-1].yt_term );
	  } else {
	      yyval.yt_rewriteclause = RewriteClause( yyvsp[-3].yt_viewnames, yyvsp[-1].yt_term );
	} ;}
    break;

  case 142:
    { yyval.yt_patternchains = Conspatternchains( yyvsp[0].yt_patternchain, Nilpatternchains() ); ;}
    break;

  case 143:
    { yyval.yt_patternchains = Conspatternchains( yyvsp[0].yt_patternchain, yyvsp[-2].yt_patternchains ); ;}
    break;

  case 144:
    { yyval.yt_patternchain = Conspatternchain( yyvsp[0].yt_patternchainitem, Nilpatternchain() ); ;}
    break;

  case 145:
    { yyval.yt_patternchain = Conspatternchain( yyvsp[0].yt_patternchainitem, yyvsp[-2].yt_patternchain ); ;}
    break;

  case 146:
    { pf_pushfileline(); ;}
    break;

  case 147:
    { yyval.yt_patternchainitem = yyvsp[0].yt_patternchainitem; pf_setstacktopfileline(yyval.yt_patternchainitem); pf_popfileline(); ;}
    break;

  case 148:
    { yyval.yt_patternchainitem = PatternchainitemOutmost( yyvsp[0].yt_outmostpattern ); ;}
    break;

  case 149:
    { yyval.yt_patternchainitem = PatternchainitemGroup( yyvsp[-1].yt_patternchains ); ;}
    break;

  case 150:
    { yyval.yt_patternchainitem = PatternchainitemDollarid( yyvsp[0].yt_ID ); pf_setwithvariable( yyvsp[0].yt_ID );  ;}
    break;

  case 151:
    { yyval.yt_outmostpatterns = Consoutmostpatterns( yyvsp[0].yt_outmostpattern, Niloutmostpatterns() ); ;}
    break;

  case 152:
    { yyerrok; yyval.yt_outmostpatterns = Consoutmostpatterns( yyvsp[0].yt_outmostpattern, yyvsp[-2].yt_outmostpatterns ); ;}
    break;

  case 153:
    { yyval.yt_outmostpatterns = Niloutmostpatterns(); ;}
    break;

  case 154:
    { yyval.yt_outmostpatterns=yyvsp[-1].yt_outmostpatterns; ;}
    break;

  case 155:
    { yyval.yt_outmostpatterns=yyvsp[-2].yt_outmostpatterns; ;}
    break;

  case 156:
    { yyerrok; yyval.yt_outmostpatterns = Consoutmostpatterns( yyvsp[0].yt_outmostpattern, yyvsp[-2].yt_outmostpatterns ); ;}
    break;

  case 157:
    { yyval.yt_outmostpattern = OPOperatorWildcard( yyvsp[-1].yt_ID, yyvsp[0].yt_Cexpression ); non_default_outmostpattern = 1;
	yyval.yt_outmostpattern->file = pg_filename; yyval.yt_outmostpattern->line = pg_lineno; ;}
    break;

  case 158:
    { yyval.yt_outmostpattern = OPOperator( yyvsp[-4].yt_ID, yyvsp[-2].yt_patterns, yyvsp[0].yt_Cexpression ); non_default_outmostpattern = 1;
	yyval.yt_outmostpattern->file = pg_filename; yyval.yt_outmostpattern->line = pg_lineno; ;}
    break;

  case 159:
    { yyval.yt_outmostpattern = OPNonLeafVariable( yyvsp[-2].yt_ID, yyvsp[0].yt_outmostpattern ); non_default_outmostpattern = 1;
	yyval.yt_outmostpattern->file = pg_filename; yyval.yt_outmostpattern->line = pg_lineno; ;}
    break;

  case 160:
    { yyval.yt_outmostpattern = OPWildcard( yyvsp[0].yt_Cexpression );
	yyval.yt_outmostpattern->file = pg_filename; yyval.yt_outmostpattern->line = pg_lineno; ;}
    break;

  case 161:
    { yyval.yt_outmostpattern = OPDefault( yyvsp[0].yt_Cexpression );
	yyval.yt_outmostpattern->file = pg_filename; yyval.yt_outmostpattern->line = pg_lineno;;}
    break;

  case 162:
    { yyval.yt_Cexpression = NilCexpression(); ;}
    break;

  case 163:
    { do_CEXPR(); ;}
    break;

  case 164:
    { do_NORMAL(); ;}
    break;

  case 165:
    { yyval.yt_Cexpression = yyvsp[-2].yt_Cexpression; ;}
    break;

  case 166:
    { yyval.yt_pattern = PVariable( yyvsp[0].yt_ID ); ;}
    break;

  case 167:
    { yyval.yt_pattern = POperator( yyvsp[-3].yt_ID, yyvsp[-1].yt_patterns ); ;}
    break;

  case 168:
    { yyval.yt_pattern = PWildcard(); ;}
    break;

  case 169:
    { yyval.yt_pattern = PWildcard(); ;}
    break;

  case 170:
    { yyval.yt_pattern = PNonLeafVariable( yyvsp[-2].yt_ID, yyvsp[0].yt_pattern ); ;}
    break;

  case 171:
    { do_CEXPRDQ(); ;}
    break;

  case 172:
    { do_NORMAL();
	  yyval.yt_pattern = PStringLiteral( yyvsp[-1].yt_CexpressionDQ ); ;}
    break;

  case 173:
    { yyval.yt_pattern = PIntLiteral( yyvsp[0].yt_INT ); ;}
    break;

  case 174:
    { yyval.yt_patterns = Nilpatterns(); ;}
    break;

  case 175:
    { yyval.yt_patterns = yyvsp[0].yt_patterns; ;}
    break;

  case 176:
    { yyval.yt_patterns = Conspatterns( yyvsp[0].yt_pattern, Nilpatterns() ); ;}
    break;

  case 177:
    { yyerrok; yyval.yt_patterns = Conspatterns( yyvsp[0].yt_pattern, yyvsp[-2].yt_patterns ); ;}
    break;

  case 178:
    { yyval.yt_patterns = Nilpatterns(); ;}
    break;

  case 179:
    { yyval.yt_patterns = yyvsp[-1].yt_patterns; ;}
    break;

  case 180:
    { yyval.yt_patterns = yyvsp[-2].yt_patterns; ;}
    break;

  case 181:
    { yyerrok; yyval.yt_patterns = Conspatterns( yyvsp[0].yt_pattern, yyvsp[-2].yt_patterns ); ;}
    break;

  case 182:
    { yyval.yt_term = TVariable( yyvsp[0].yt_ID );
		yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno; ;}
    break;

  case 183:
    { yyval.yt_term = TOperator( yyvsp[-3].yt_ID, yyvsp[-1].yt_terms );
	  yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno;
	;}
    break;

  case 184:
    { yyval.yt_term = TMethod( yyvsp[-5].yt_term, yyvsp[-3].yt_ID, yyvsp[-1].yt_terms );
	  yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno; ;}
    break;

  case 185:
    { yyval.yt_term = TMethodDot( yyvsp[-5].yt_term, yyvsp[-3].yt_ID, yyvsp[-1].yt_terms );
	  yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno; ;}
    break;

  case 186:
    { yyval.yt_term = TMemberVar( yyvsp[-2].yt_term, yyvsp[0].yt_ID );
	  yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno; ;}
    break;

  case 187:
    { yyval.yt_term = TMemberVarDot( yyvsp[-2].yt_term, yyvsp[0].yt_ID );
	  yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno; ;}
    break;

  case 188:
    { do_CEXPRSQ(); ;}
    break;

  case 189:
    { do_NORMAL();
	  yyval.yt_term = TCTerm( yyvsp[-1].yt_CexpressionSQ );
	  yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno; ;}
    break;

  case 190:
    { do_CEXPRDQ(); ;}
    break;

  case 191:
    { do_NORMAL();
	  yyval.yt_term = TStringLiteral( yyvsp[-1].yt_CexpressionDQ );
	  yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno; ;}
    break;

  case 192:
    { yyval.yt_term = TIntLiteral( yyvsp[0].yt_INT );
	yyval.yt_term->file = pg_filename; yyval.yt_term->line = pg_lineno; ;}
    break;

  case 193:
    { yyval.yt_terms = Nilterms(); ;}
    break;

  case 194:
    { yyval.yt_terms = yyvsp[0].yt_terms; ;}
    break;

  case 195:
    { yyval.yt_terms = Consterms( yyvsp[0].yt_term, Nilterms() ); ;}
    break;

  case 196:
    { yyerrok; yyval.yt_terms = Consterms( yyvsp[0].yt_term, yyvsp[-2].yt_terms ); ;}
    break;

  case 197:
    { yyval.yt_terms = Nilterms(); ;}
    break;

  case 198:
    { yyval.yt_terms = yyvsp[-1].yt_terms; ;}
    break;

  case 199:
    { yyval.yt_terms = yyvsp[-2].yt_terms; ;}
    break;

  case 200:
    { yyerrok; yyval.yt_terms = Consterms( yyvsp[0].yt_term, yyvsp[-2].yt_terms ); ;}
    break;

  case 201:
    { yyval.yt_withcases = Conswithcases( yyvsp[0].yt_withcase, Nilwithcases() ); ;}
    break;

  case 202:
    { yyval.yt_withcases = Conswithcases( yyvsp[0].yt_withcase, yyvsp[-1].yt_withcases ); ;}
    break;

  case 203:
    { yyval.yt_withcase = Withcase( syn_patternchains_fileline(yyvsp[-2].yt_patternchains->rewrite(base_rview), pg_filename, pg_lineno ), yyvsp[0].yt_Ctext ); ;}
    break;

  case 204:
    { if (! non_default_outmostpattern ) v_report(NonFatal( NoFileLine(), Problem1S( "can not infer type from 'default' pattern(s)" )));
	  non_default_outmostpattern = 0; ;}
    break;

  case 205:
    { yyval.yt_unparsedeclaration = UnparseDeclaration( yyvsp[-4].yt_outmostpatterns, yyvsp[-1].yt_unparseclauses ); ;}
    break;

  case 206:
    { yyval.yt_unparsedeclaration = UnparseDeclaration( yyvsp[-3].yt_outmostpatterns, Nilunparseclauses()); ;}
    break;

  case 207:
    { yyval.yt_unparseclauses = Consunparseclauses( yyvsp[0].yt_unparseclause, Nilunparseclauses() ); ;}
    break;

  case 208:
    { yyval.yt_unparseclauses = Consunparseclauses( yyvsp[0].yt_unparseclause, yyvsp[-1].yt_unparseclauses ); ;}
    break;

  case 209:
    { if (yyvsp[-3].yt_viewnames->eq( Nilviewnames())) {
	      yyval.yt_unparseclause = UnparseClause( Consviewnames( Id( Str( mkcasestring( "base_uview" ))), yyvsp[-3].yt_viewnames ), yyvsp[-1].yt_unparseitems );
	  } else {
	      yyval.yt_unparseclause = UnparseClause( yyvsp[-3].yt_viewnames, yyvsp[-1].yt_unparseitems );
	} inc_text_nr(); yyerrok;
	;}
    break;

  case 210:
    { yyval.yt_unparseclause = UnparseClause( yyvsp[-2].yt_viewnames, Nilunparseitems()); ;}
    break;

  case 211:
    { yyerrok; yyval.yt_unparseclause = UnparseClause( Nilviewnames(), yyvsp[-1].yt_unparseitems); ;}
    break;

  case 212:
    { yyval.yt_unparseclause = UnparseClause( Nilviewnames(), Nilunparseitems()); ;}
    break;

  case 213:
    { yyval.yt_unparseclause = UnparseClause( Nilviewnames(), Nilunparseitems()); ;}
    break;

  case 214:
    { yyval.yt_viewnames = Nilviewnames(); ;}
    break;

  case 215:
    { yyval.yt_viewnames = Consviewnames( yyvsp[0].yt_ID, yyvsp[-1].yt_viewnames ); ;}
    break;

  case 216:
    { yyval.yt_unparseitems = Nilunparseitems(); ;}
    break;

  case 217:
    { yyval.yt_unparseitems = Consunparseitems( yyvsp[0].yt_unparseitem, yyvsp[-1].yt_unparseitems ); ;}
    break;

  case 218:
    { do_CEXPRDQ(); ;}
    break;

  case 219:
    { do_NORMAL(); ;}
    break;

  case 220:
    { yyval.yt_unparseitem = UnpStr( yyvsp[-6].yt_languageoption, yyvsp[-3].yt_CexpressionDQ, yyvsp[0].yt_viewnameoption );
	  yyval.yt_unparseitem->text_nr=get_text_nr();
	  if(!yyvsp[0].yt_viewnameoption->eq(NoViewname()))
	    inc_text_nr();
	;}
    break;

  case 221:
    { yyval.yt_unparseitem = UnpSubexpr( yyvsp[-2].yt_languageoption, yyvsp[-1].yt_unpsubterm, yyvsp[0].yt_viewnameoption ); inc_text_nr(); ;}
    break;

  case 222:
    { yyval.yt_unparseitem = UnpCtext( yyvsp[-1].yt_languageoption, yyvsp[0].yt_Ctext ); inc_text_nr(); ;}
    break;

  case 223:
    { yyval.yt_unparseitem = UnpBody( yyvsp[-3].yt_languageoption, yyvsp[-1].yt_unparseitems ); inc_text_nr(); ;}
    break;

  case 224:
    { do_CEXPR(); ;}
    break;

  case 225:
    { do_NORMAL(); ;}
    break;

  case 226:
    { yyval.yt_unparseitem = UViewVarDecl( yyvsp[-5].yt_ID, yyvsp[-4].yt_ID, yyvsp[-2].yt_Cexpression ); ;}
    break;

  case 227:
    { yyval.yt_unpsubterm = UnpSubTerm( yyvsp[0].yt_ID ); ;}
    break;

  case 228:
    { yyval.yt_unpsubterm = UnpDollarvarTerm( yyvsp[0].yt_INT ); ;}
    break;

  case 229:
    { yyval.yt_unpsubterm = UnpSubAttr( yyvsp[-1].yt_ID, yyvsp[0].yt_unpattributes ); ;}
    break;

  case 230:
    { yyval.yt_unpsubterm = UnpDollarvarAttr( yyvsp[-1].yt_INT, yyvsp[0].yt_unpattributes ); ;}
    break;

  case 231:
    { yyval.yt_unpsubterm = UnpCastedVariable( yyvsp[-2].yt_ID, yyvsp[0].yt_ID ); ;}
    break;

  case 232:
    { yyval.yt_unpattributes = Consunpattributes( yyvsp[0].yt_ID, Nilunpattributes() ); ;}
    break;

  case 233:
    { yyval.yt_unpattributes = Consunpattributes( yyvsp[0].yt_ID, yyvsp[-1].yt_unpattributes ); ;}
    break;

  case 234:
    { yyval.yt_ID = yyvsp[0].yt_ID; ;}
    break;

  case 235:
    { yyval.yt_ID = Id(Str(mkcasestring("error"))); ;}
    break;

  case 236:
    { yyval.yt_viewnameoption = NoViewname(); ;}
    break;

  case 237:
    { yyval.yt_viewnameoption = YesViewname( yyvsp[0].yt_ID ); ;}
    break;

  case 238:
    { yyval.yt_viewnameoption = NoViewname(); ;}
    break;

  case 239:
    { yyerrok; yyval.yt_viewnames = yyvsp[-1].yt_viewnames; pg_uviewshavebeendefined = true; ;}
    break;

  case 240:
    { yyerrok; yyval.yt_viewnames = Nilviewnames(); ;}
    break;

  case 241:
    { yyval.yt_viewnames = Consviewnames( yyvsp[0].yt_ID, Nilviewnames());
	  v_add_to_uviewnames( yyvsp[0].yt_ID );
	  v_extendoccur( yyvsp[0].yt_ID, ITUserUView() ); ;}
    break;

  case 242:
    { yyval.yt_viewnames = Consviewnames( yyvsp[-1].yt_ID, Nilviewnames());
	  v_add_to_uviewnames_ext( yyvsp[-1].yt_ID );
	  v_extendoccur( yyvsp[-1].yt_ID, ITUserUView() ); ;}
    break;

  case 243:
    { yyval.yt_viewnames = Consviewnames( yyvsp[0].yt_ID, yyvsp[-1].yt_viewnames );
	  v_add_to_uviewnames( yyvsp[0].yt_ID );
	  v_extendoccur( yyvsp[0].yt_ID, ITUserUView() ); ;}
    break;

  case 244:
    { yyval.yt_viewnames = Consviewnames( yyvsp[-1].yt_ID, yyvsp[-3].yt_viewnames );
	  v_add_to_uviewnames_ext( yyvsp[-1].yt_ID );
	  v_extendoccur( yyvsp[-1].yt_ID, ITUserUView() ); ;}
    break;

  case 245:
    { yyval.yt_viewnames = Consviewnames( yyvsp[0].yt_ID, yyvsp[-2].yt_viewnames );
	  v_add_to_uviewnames( yyvsp[0].yt_ID );
	  v_extendoccur( yyvsp[0].yt_ID, ITUserUView() ); ;}
    break;

  case 246:
    { yyval.yt_viewnames = Consviewnames( yyvsp[-1].yt_ID, yyvsp[-4].yt_viewnames );
	  v_add_to_uviewnames_ext( yyvsp[-1].yt_ID );
	  v_extendoccur( yyvsp[-1].yt_ID, ITUserUView() ); ;}
    break;

  case 247:
    { yyval.yt_viewnames = yyvsp[-1].yt_viewnames; ;}
    break;

  case 248:
    { yyval.yt_viewnames = yyvsp[-2].yt_viewnames; ;}
    break;

  case 249:
    { yyerrok; yyval.yt_viewnames = yyvsp[-1].yt_viewnames; pg_rviewshavebeendefined = true; ;}
    break;

  case 250:
    { yyerrok; yyval.yt_viewnames = Nilviewnames(); ;}
    break;

  case 251:
    { yyval.yt_viewnames = Consviewnames( yyvsp[0].yt_ID, Nilviewnames());
	  v_add_to_rviewnames( yyvsp[0].yt_ID );
	  v_extendoccur( yyvsp[0].yt_ID, ITUserRView() ); ;}
    break;

  case 252:
    { yyval.yt_viewnames = Consviewnames( yyvsp[-1].yt_ID, Nilviewnames());
	  v_add_to_rviewnames_ext( yyvsp[-1].yt_ID );
	  v_extendoccur( yyvsp[-1].yt_ID, ITUserRView() ); ;}
    break;

  case 253:
    { yyval.yt_viewnames = Consviewnames( yyvsp[0].yt_ID, yyvsp[-1].yt_viewnames );
	  v_add_to_rviewnames( yyvsp[0].yt_ID );
	  v_extendoccur( yyvsp[0].yt_ID, ITUserRView() ); ;}
    break;

  case 254:
    { yyval.yt_viewnames = Consviewnames( yyvsp[-1].yt_ID, yyvsp[-3].yt_viewnames );
	  v_add_to_rviewnames_ext( yyvsp[-1].yt_ID );
	  v_extendoccur( yyvsp[-1].yt_ID, ITUserRView() ); ;}
    break;

  case 255:
    { yyval.yt_viewnames = Consviewnames( yyvsp[0].yt_ID, yyvsp[-2].yt_viewnames );
	  v_add_to_rviewnames( yyvsp[0].yt_ID );
	  v_extendoccur( yyvsp[0].yt_ID, ITUserRView() ); ;}
    break;

  case 256:
    { yyval.yt_viewnames = Consviewnames( yyvsp[-1].yt_ID, yyvsp[-4].yt_viewnames );
	  v_add_to_rviewnames_ext( yyvsp[-1].yt_ID );
	  v_extendoccur( yyvsp[-1].yt_ID, ITUserRView() ); ;}
    break;

  case 257:
    { yyval.yt_viewnames = yyvsp[-1].yt_viewnames; ;}
    break;

  case 258:
    { yyval.yt_viewnames = yyvsp[-2].yt_viewnames; ;}
    break;

  case 259:
    { yyerrok; yyval.yt_storageclasses = yyvsp[-1].yt_storageclasses; pg_storageclasseshavebeendefined = true; ;}
    break;

  case 260:
    { yyerrok; yyval.yt_storageclasses = Nilstorageclasses(); ;}
    break;

  case 261:
    { yyval.yt_storageclasses = Consstorageclasses( yyvsp[0].yt_ID, Nilstorageclasses());
	  v_add_to_storageclasses( yyvsp[0].yt_ID, f_emptyId() );
	  v_extendoccur( yyvsp[0].yt_ID, ITStorageClass() ); ;}
    break;

  case 262:
    { yyval.yt_storageclasses = Consstorageclasses( yyvsp[0].yt_ID, yyvsp[-1].yt_storageclasses );
	  v_add_to_storageclasses( yyvsp[0].yt_ID, f_emptyId() );
	  v_extendoccur( yyvsp[0].yt_ID, ITStorageClass() ); ;}
    break;

  case 263:
    { yyval.yt_storageclasses = Consstorageclasses( yyvsp[0].yt_ID, yyvsp[-2].yt_storageclasses );
	  v_add_to_storageclasses( yyvsp[0].yt_ID, f_emptyId() );
	  v_extendoccur( yyvsp[0].yt_ID, ITStorageClass() ); ;}
    break;

  case 264:
    { yyval.yt_storageclasses = yyvsp[-1].yt_storageclasses; ;}
    break;

  case 265:
    { yyval.yt_storageclasses = yyvsp[-2].yt_storageclasses; ;}
    break;

  case 266:
    {
	    fnclass tmp_type = f_fnclass_info(yyvsp[-2].yt_ac_declaration_specifiers, pg_filename, yyvsp[-1].yt_ac_declarator );
	    ID tmp_id = f_ID_of_fn_declarator(yyvsp[-1].yt_ac_declarator, tmp_type);
	    yyval.yt_fndeclaration = FnAcDeclaration( yyvsp[-2].yt_ac_declaration_specifiers, yyvsp[-1].yt_ac_declarator, Nilac_declaration_list(), AcNoBaseInit(), yyvsp[0].yt_Ctext, tmp_id, tmp_type );
	    pf_resetwithvariable();
	    v_defoccur( tmp_id, ITUserFunction( tmp_type ) );
	    yyval.yt_fndeclaration->file = pg_filename; yyval.yt_fndeclaration->last_line = pg_lineno;
	;}
    break;

  case 267:
    {
	    fnclass tmp_type=ConvOperatorFn();
	    ac_declarator decl= AcDeclarator(Nopointer(),AcNoRef(),
		    AcQualifiedDeclProto(
			    Nilac_class_qualifier_list(),
			    AcConvOperatorDecl(yyvsp[-7].yt_ID, yyvsp[-4].yt_ID),
			    AcParList(Nilac_parameter_list()),
			    yyvsp[-1].yt_ac_type_qualifier )
		    );
	    ID tmp_id = yyvsp[-4].yt_ID;
	    yyval.yt_fndeclaration = FnAcDeclaration(
		    Nilac_declaration_specifiers(),
		    decl,
		    Nilac_declaration_list(), AcNoBaseInit(), yyvsp[0].yt_Ctext, tmp_id, tmp_type
	    );
	    pf_resetwithvariable();
	    v_defoccur( tmp_id, ITUserFunction( tmp_type ) );
	    yyval.yt_fndeclaration->file = pg_filename; yyval.yt_fndeclaration->last_line = pg_lineno;
	;}
    break;

  case 268:
    {
		fnclass tmp_type=ConstructorFn();
		ac_declarator decl= AcDeclarator(Nopointer(),AcNoRef(),
			AcQualifiedDeclProto(
				Nilac_class_qualifier_list(),
				AcDirectDeclId(yyvsp[-5].yt_ID),
				yyvsp[-3].yt_ac_parameter_type_list,
				AcNoQualifier() )
			);
		ID tmp_id = f_ID_of_fn_declarator(decl, tmp_type);
		yyval.yt_fndeclaration = FnAcDeclaration(
			Nilac_declaration_specifiers(),
			decl,
			Nilac_declaration_list(), yyvsp[-1].yt_ac_opt_base_init_list, yyvsp[0].yt_Ctext, tmp_id, tmp_type
		);
		pf_resetwithvariable();
		v_defoccur( tmp_id, ITUserFunction( tmp_type ) );
		yyval.yt_fndeclaration->file = pg_filename; yyval.yt_fndeclaration->last_line = pg_lineno;
	;}
    break;

  case 269:
    {
		fnclass tmp_type=DestructorFn();
		ac_declarator decl= AcDeclarator(Nopointer(),AcNoRef(),
			AcQualifiedDeclProto(
				Nilac_class_qualifier_list(),
				AcDirectDeclId(yyvsp[-3].yt_ID),
				AcParList( Nilac_parameter_list() ),
				AcNoQualifier() )
			);
		ID tmp_id = f_ID_of_fn_declarator(decl, tmp_type);
		yyval.yt_fndeclaration = FnAcDeclaration(
			yyvsp[-4].yt_ac_declaration_specifiers,
			decl,
			Nilac_declaration_list(), AcNoBaseInit(), yyvsp[0].yt_Ctext, tmp_id, tmp_type
		);
		pf_resetwithvariable();
		v_defoccur( tmp_id, ITUserFunction( tmp_type ) );
		yyval.yt_fndeclaration->file = pg_filename; yyval.yt_fndeclaration->last_line = pg_lineno;
	;}
    break;

  case 270:
    {
		fnclass tmp_type = f_member_class_info(yyvsp[-3].yt_ac_declaration_specifiers, pg_filename);
		yyval.yt_fndeclaration=AcMemberDeclaration(yyvsp[-3].yt_ac_declaration_specifiers,yyvsp[-2].yt_ac_declarator,yyvsp[-1].yt_ac_constant_expression_option,tmp_type);
		yyval.yt_fndeclaration->is_attr=false;
		yyval.yt_fndeclaration->file = pg_filename; yyval.yt_fndeclaration->last_line = pg_lineno;
	;}
    break;

  case 271:
    {
		fnclass tmp_type = f_member_class_info(yyvsp[-3].yt_ac_declaration_specifiers, pg_filename);
		yyval.yt_fndeclaration=AcMemberDeclaration(yyvsp[-3].yt_ac_declaration_specifiers,yyvsp[-2].yt_ac_declarator,yyvsp[-1].yt_ac_constant_expression_option,tmp_type);
		yyval.yt_fndeclaration->is_attr=true;
		yyval.yt_fndeclaration->file = pg_filename; yyval.yt_fndeclaration->last_line = pg_lineno;
	;}
    break;

  case 272:
    { yyval.yt_ac_opt_base_init_list = AcNoBaseInit(); ;}
    break;

  case 273:
    { yyval.yt_ac_opt_base_init_list = AcYesBaseInit( yyvsp[0].yt_ac_base_init_list ); ;}
    break;

  case 274:
    { yyval.yt_ac_base_init_list = Consac_base_init_list( yyvsp[0].yt_ac_base_init, Nilac_base_init_list() ); ;}
    break;

  case 275:
    { yyval.yt_ac_base_init_list = Consac_base_init_list( yyvsp[0].yt_ac_base_init, yyvsp[-2].yt_ac_base_init_list ); ;}
    break;

  case 276:
    { yyval.yt_ac_base_init= AcBaseInit( yyvsp[-3].yt_ID, yyvsp[-1].yt_ac_constant_expression ); ;}
    break;

  case 277:
    { yyval.yt_ac_constant_expression_option=Noac_constant_expression(); ;}
    break;

  case 278:
    { yyval.yt_ac_constant_expression_option=Yesac_constant_expression(yyvsp[0].yt_ac_constant_expression); ;}
    break;

  case 279:
    { yyval.yt_ac_constant_expression_option=Noac_constant_expression(); ;}
    break;

  case 280:
    { yyval.yt_ac_constant_expression_option=Yesac_constant_expression(yyvsp[0].yt_ac_constant_expression); ;}
    break;

  case 281:
    { yyval.yt_ac_constant_expression_option=Yesac_constant_expression(yyvsp[-1].yt_ac_constant_expression); ;}
    break;

  case 282:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
		AcDeclSpecTypeSpec( yyvsp[0].yt_ac_type_specifier ),
		Nilac_declaration_specifiers() ); ;}
    break;

  case 283:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
	     AcDeclSpecTypeSpec( yyvsp[0].yt_ac_type_specifier ),
	     Consac_declaration_specifiers(
		AcDeclSpecStorageSpec( yyvsp[-1].yt_ac_storage_class_specifier ),
		Nilac_declaration_specifiers() )); ;}
    break;

  case 284:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
	     AcDeclSpecTypeSpec( yyvsp[0].yt_ac_type_specifier ),
	     Consac_declaration_specifiers(
	       AcDeclSpecTypeQual( yyvsp[-1].yt_ac_type_qualifier ),
	       Nilac_declaration_specifiers() )); ;}
    break;

  case 285:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
	     AcDeclSpecTypeSpec( yyvsp[0].yt_ac_type_specifier ),
	     Consac_declaration_specifiers(
	       AcDeclSpecTypeQual( yyvsp[-1].yt_ac_type_qualifier ),
	       Consac_declaration_specifiers(
		 AcDeclSpecStorageSpec( yyvsp[-2].yt_ac_storage_class_specifier ),
		 Nilac_declaration_specifiers() )));
	;}
    break;

  case 286:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
		AcDeclSpecTypeSpec( yyvsp[0].yt_ac_type_specifier ),
		Nilac_declaration_specifiers() ); ;}
    break;

  case 287:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
	     AcDeclSpecTypeSpec( yyvsp[0].yt_ac_type_specifier ),
	     Consac_declaration_specifiers(
		AcDeclSpecStorageSpec( yyvsp[-1].yt_ac_storage_class_specifier ),
		Nilac_declaration_specifiers() )); ;}
    break;

  case 288:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
	     AcDeclSpecTypeSpec( yyvsp[0].yt_ac_type_specifier ),
	     Consac_declaration_specifiers(
	       AcDeclSpecTypeQual( yyvsp[-1].yt_ac_type_qualifier ),
	       Nilac_declaration_specifiers() )); ;}
    break;

  case 289:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
	     AcDeclSpecTypeSpec( yyvsp[0].yt_ac_type_specifier ),
	     Consac_declaration_specifiers(
	       AcDeclSpecTypeQual( yyvsp[-1].yt_ac_type_qualifier ),
	       Consac_declaration_specifiers(
		 AcDeclSpecStorageSpec( yyvsp[-2].yt_ac_storage_class_specifier ),
		 Nilac_declaration_specifiers() )));
	;}
    break;

  case 290:
    { yyval.yt_ac_storage_class_specifier = AcAuto(); ;}
    break;

  case 291:
    { yyval.yt_ac_storage_class_specifier = AcRegister(); ;}
    break;

  case 292:
    { yyval.yt_ac_storage_class_specifier = AcStatic(); ;}
    break;

  case 293:
    { yyval.yt_ac_storage_class_specifier = AcExtern(); ;}
    break;

  case 294:
    { yyval.yt_ac_storage_class_specifier = AcTypedef(); ;}
    break;

  case 295:
    { yyval.yt_ac_storage_class_specifier = AcVirtual(); ;}
    break;

  case 296:
    { yyval.yt_ac_storage_class_specifier = AcStatic(); ;}
    break;

  case 297:
    { yyval.yt_ac_declaration_specifiers = Nilac_declaration_specifiers(); ;}
    break;

  case 298:
    { yyval.yt_ac_declaration_specifiers = Consac_declaration_specifiers(
			AcDeclSpecStorageSpec( AcVirtual() ),
			Nilac_declaration_specifiers()
		);
	;}
    break;

  case 299:
    { yyval.yt_ac_type_specifier = AcTypeSpec( yyvsp[0].yt_ID ); ;}
    break;

  case 300:
    { yyval.yt_ac_type_qualifier = AcConst(); ;}
    break;

  case 301:
    { yyval.yt_ac_type_qualifier = AcVolatile(); ;}
    break;

  case 302:
    { yyval.yt_ac_type_qualifier = AcUnsigned(); ;}
    break;

  case 303:
    { yyval.yt_ac_declarator = AcDeclarator( Nopointer(), AcNoRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 304:
    { yyval.yt_ac_declarator = AcDeclarator( Yespointer( yyvsp[-1].yt_ac_pointer ), AcNoRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 305:
    { yyval.yt_ac_declarator = AcDeclarator( Nopointer(), AcRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 306:
    { yyval.yt_ac_declarator = AcDeclarator( Yespointer( yyvsp[-2].yt_ac_pointer ), AcRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 307:
    { yyval.yt_ac_declarator = AcDeclarator(Nopointer(), AcNoRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 308:
    { yyval.yt_ac_declarator = AcDeclarator( Yespointer( yyvsp[-1].yt_ac_pointer ), AcNoRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 309:
    { yyval.yt_ac_declarator = AcDeclarator(Nopointer(), AcRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 310:
    { yyval.yt_ac_declarator = AcDeclarator( Yespointer( yyvsp[-2].yt_ac_pointer ), AcRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 311:
    { yyval.yt_ac_declarator = AcDeclarator( Nopointer(), AcNoRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 312:
    { yyval.yt_ac_declarator = AcDeclarator( Yespointer( yyvsp[-1].yt_ac_pointer ), AcNoRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 313:
    { yyval.yt_ac_declarator = AcDeclarator( Nopointer(), AcRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 314:
    { yyval.yt_ac_declarator = AcDeclarator( Yespointer( yyvsp[-2].yt_ac_pointer ), AcRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 315:
    { yyval.yt_ac_declarator = AcDeclarator( Yespointer( yyvsp[-1].yt_ac_pointer ), AcNoRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 316:
    { yyval.yt_ac_declarator = AcDeclarator( Yespointer( yyvsp[-2].yt_ac_pointer ), AcRef(), yyvsp[0].yt_ac_direct_declarator ); ;}
    break;

  case 317:
    { yyval.yt_ac_direct_declarator = AcDirectDeclId( yyvsp[0].yt_ID ); ;}
    break;

  case 318:
    { yyval.yt_ac_direct_declarator = AcDirectDeclPack( yyvsp[-1].yt_ac_declarator ); ;}
    break;

  case 319:
    { yyval.yt_ac_direct_declarator = AcDirectDeclArray( yyvsp[-3].yt_ac_direct_declarator, yyvsp[-1].yt_ac_constant_expression_option ); ;}
    break;

  case 320:
    { yyval.yt_ac_direct_declarator = AcDirectDeclProto( yyvsp[-3].yt_ac_direct_declarator, yyvsp[-1].yt_ac_parameter_type_list ); ;}
    break;

  case 321:
    { yyval.yt_ac_direct_declarator = AcMemberDecl( yyvsp[-3].yt_ID, yyvsp[-1].yt_ID, yyvsp[0].yt_ac_constant_expression_list ); ;}
    break;

  case 322:
    { yyval.yt_ac_constant_expression_list=Nilac_constant_expression_list(); ;}
    break;

  case 323:
    { yyval.yt_ac_constant_expression_list=Consac_constant_expression_list(yyvsp[-1].yt_ac_constant_expression,yyvsp[-3].yt_ac_constant_expression_list); ;}
    break;

  case 324:
    {
    ac_direct_declarator decl=NULL;
	ac_class_qualifier_list q=f_check_build_qualifier(yyvsp[-4].yt_ac_class_qualifier_help_list,decl);
	yyval.yt_ac_direct_declarator = AcQualifiedDeclProto( q, decl, yyvsp[-2].yt_ac_parameter_type_list ,yyvsp[0].yt_ac_type_qualifier );
	;}
    break;

  case 325:
    {
	ac_direct_declarator decl=NULL;
	ac_class_qualifier_list q=f_check_build_qualifier(yyvsp[-4].yt_ac_class_qualifier_help_list,decl);
	yyval.yt_ac_direct_declarator = AcQualifiedDeclProto( q, decl, AcParList( Nilac_parameter_list() ),yyvsp[0].yt_ac_type_qualifier );
        yyerrok;
    ;}
    break;

  case 326:
    { yyval.yt_ac_class_qualifier_help_list=Consac_class_qualifier_help_list(yyvsp[0].yt_ac_direct_declarator,Nilac_class_qualifier_help_list()); ;}
    break;

  case 327:
    { yyval.yt_ac_class_qualifier_help_list=Consac_class_qualifier_help_list(yyvsp[0].yt_ac_direct_declarator,yyvsp[-2].yt_ac_class_qualifier_help_list); ;}
    break;

  case 328:
    { yyval.yt_ac_direct_declarator=AcDirectDeclId(yyvsp[0].yt_ID); ;}
    break;

  case 329:
    {
	ac_operator_name op_name=AcOperatorName(yyvsp[0].yt_casestring);
	pf_setfileline( op_name );
	yyval.yt_ac_direct_declarator=AcOperatorDeclId(op_name);
    ;}
    break;

  case 330:
    { yyval.yt_ac_type_qualifier=AcNoQualifier(); ;}
    break;

  case 331:
    { yyval.yt_ac_type_qualifier=AcConst(); ;}
    break;

  case 332:
    { yyval.yt_ac_pointer = AcPointerNil( Nilac_type_qualifier_list() ); ;}
    break;

  case 333:
    { yyval.yt_ac_pointer = AcPointerNil( yyvsp[0].yt_ac_type_qualifier_list ); ;}
    break;

  case 334:
    { yyval.yt_ac_pointer = AcPointerCons( Nilac_type_qualifier_list(), yyvsp[0].yt_ac_pointer ); ;}
    break;

  case 335:
    { yyval.yt_ac_pointer = AcPointerCons( yyvsp[-1].yt_ac_type_qualifier_list, yyvsp[0].yt_ac_pointer ); ;}
    break;

  case 336:
    { yyval.yt_ac_type_qualifier_list = Consac_type_qualifier_list( yyvsp[0].yt_ac_type_qualifier, Nilac_type_qualifier_list() ); ;}
    break;

  case 337:
    { yyval.yt_ac_type_qualifier_list = Consac_type_qualifier_list( yyvsp[0].yt_ac_type_qualifier, yyvsp[-1].yt_ac_type_qualifier_list ); ;}
    break;

  case 338:
    { yyval.yt_ac_parameter_type_list = AcParList( Nilac_parameter_list() ); ;}
    break;

  case 339:
    { yyval.yt_ac_parameter_type_list = AcParList3Dot( Nilac_parameter_list() ); ;}
    break;

  case 340:
    { yyval.yt_ac_parameter_type_list = AcParList( yyvsp[0].yt_ac_parameter_list ); ;}
    break;

  case 341:
    { yyval.yt_ac_parameter_type_list = AcParList3Dot( yyvsp[-1].yt_ac_parameter_list ); ;}
    break;

  case 342:
    { yyval.yt_ac_parameter_type_list = AcParList3Dot( yyvsp[-2].yt_ac_parameter_list ); ;}
    break;

  case 343:
    { yyval.yt_ac_parameter_list = Consac_parameter_list( yyvsp[0].yt_ac_parameter_declaration, Nilac_parameter_list() ); ;}
    break;

  case 344:
    { yyval.yt_ac_parameter_list = Consac_parameter_list( yyvsp[0].yt_ac_parameter_declaration, yyvsp[-2].yt_ac_parameter_list ); ;}
    break;

  case 345:
    { yyval.yt_ac_parameter_declaration = AcParDeclDecl( yyvsp[-2].yt_ac_declaration_specifiers, yyvsp[-1].yt_ac_declarator, yyvsp[0].yt_ac_constant_expression_option ); ;}
    break;

  case 346:
    { yyval.yt_ac_parameter_declaration = AcParDeclAbsdecl( yyvsp[-2].yt_ac_declaration_specifiers, yyvsp[-1].yt_ac_abstract_declarator, yyvsp[0].yt_ac_constant_expression_option ); ;}
    break;

  case 347:
    { yyval.yt_ac_abstract_declarator = AcAbsdeclPointer( yyvsp[0].yt_ac_pointer ); ;}
    break;

  case 348:
    { yyval.yt_ac_abstract_declarator = AcAbsdeclDirdecl( Nopointer(), yyvsp[0].yt_ac_direct_abstract_declarator ); ;}
    break;

  case 349:
    { yyval.yt_ac_abstract_declarator = AcAbsdeclDirdecl( Yespointer( yyvsp[-1].yt_ac_pointer ), yyvsp[0].yt_ac_direct_abstract_declarator ); ;}
    break;

  case 350:
    { yyval.yt_ac_direct_abstract_declarator = AcDirAbsdeclPack( yyvsp[-1].yt_ac_abstract_declarator ); ;}
    break;

  case 351:
    { yyval.yt_ac_direct_abstract_declarator = AcDirAbsdeclArray( Yesac_direct_abstract_declarator( yyvsp[-3].yt_ac_direct_abstract_declarator ), yyvsp[-1].yt_ac_constant_expression_option ); ;}
    break;

  case 352:
    { yyval.yt_ac_direct_abstract_declarator = AcDirAbsdeclArray( Noac_direct_abstract_declarator(), yyvsp[-1].yt_ac_constant_expression_option ); ;}
    break;

  case 353:
    { yyval.yt_ac_direct_abstract_declarator = AcDirAbsdeclFn( Yesac_direct_abstract_declarator( yyvsp[-3].yt_ac_direct_abstract_declarator ), yyvsp[-1].yt_ac_parameter_type_list ); ;}
    break;

  case 354:
    { yyval.yt_ac_direct_abstract_declarator = AcDirAbsdeclFn( Noac_direct_abstract_declarator(), yyvsp[-1].yt_ac_parameter_type_list ); ;}
    break;

  case 355:
    { yyval.yt_ac_constant_expression_option = Yesac_constant_expression( yyvsp[0].yt_ac_constant_expression ); ;}
    break;

  case 356:
    { do_CEXPR(); ;}
    break;

  case 357:
    { do_NORMAL();
	 yyval.yt_ac_constant_expression = AcConstExpr( yyvsp[0].yt_Cexpression ); ;}
    break;

  case 358:
    { yyval.yt_ID = yyvsp[0].yt_ID; ;}
    break;

  case 359:
    { yyval.yt_ID = yyvsp[0].yt_ID; pf_setwithvariable( yyvsp[0].yt_ID ); ;}
    break;

  case 360:
    { yyval.yt_Ctext = yyvsp[0].yt_Ctext; ;}
    break;

  case 361:
    { yyerrok; ;}
    break;

  case 362:
    { yyval.yt_Ctext = yyvsp[0].yt_Ctext; ;}
    break;

  case 363:
    { yyerrok; yyval.yt_languageoption = LanguageList(yyvsp[-2].yt_languagenames); ;}
    break;

  case 364:
    { yyerrok; yyval.yt_languageoption = NoLanguagename(); ;}
    break;

  case 365:
    { yyval.yt_languagenames = Conslanguagenames( yyvsp[0].yt_ID, Nillanguagenames()); ;}
    break;

  case 366:
    { yyerrok; yyval.yt_languagenames = Conslanguagenames( yyvsp[0].yt_ID, yyvsp[-2].yt_languagenames ); ;}
    break;

  case 367:
    { yyval.yt_languagenames = Nillanguagenames(); ;}
    break;

  case 368:
    { yyval.yt_languagenames = yyvsp[-1].yt_languagenames; ;}
    break;

  case 369:
    { yyval.yt_languagenames = yyvsp[-2].yt_languagenames; ;}
    break;

  case 370:
    { yyerrok; yyval.yt_languagenames = Conslanguagenames( yyvsp[0].yt_ID, yyvsp[-2].yt_languagenames ); ;}
    break;

  case 371:
    { yyerrok; yyval.yt_languagenames = yyvsp[-1].yt_languagenames; pg_languageshavebeendefined = true; ;}
    break;

  case 372:
    { yyerrok; yyval.yt_languagenames = Nillanguagenames(); ;}
    break;

  case 373:
    { yyval.yt_languagenames = Conslanguagenames( yyvsp[0].yt_ID, Nillanguagenames()); ;}
    break;

  case 374:
    { yyval.yt_languagenames = Conslanguagenames( yyvsp[0].yt_ID, yyvsp[-1].yt_languagenames ); ;}
    break;

  case 375:
    { yyval.yt_languagenames = Conslanguagenames( yyvsp[0].yt_ID, yyvsp[-2].yt_languagenames ); ;}
    break;

  case 376:
    { yyval.yt_languagenames = yyvsp[-1].yt_languagenames; ;}
    break;

  case 377:
    { yyval.yt_languagenames = yyvsp[-2].yt_languagenames; ;}
    break;


    }

/* Line 999 of yacc.c.  */

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}




#include "errmsg.inc"

void
yyerror_1(const char *s, int state_no)
{
    extern int yychar;
    extern char yytext[];
    int rule_pos, i;

    v_report( NonFatal( PosNoFileLine(), Problem1S( s )));
    for (rule_pos = error_state[state_no]; error_rules[rule_pos][0] != 0; rule_pos++)
    {
	int rule = error_rules[rule_pos][0];
	int rhs = yyprhs[rule+1];
	int pos = error_rules[rule_pos][1];
	fprintf(stderr, "\t%s ->", yytname[yyr1[rule+1]]);
	for (i = 0; yyrhs[rhs+i] != -1; i++)
	{
	    if (i == pos) fprintf(stderr, " .");
	    fprintf(stderr, " %s", yytname[yyrhs[rhs+i]]);
	}
	if (i == pos) fprintf(stderr, " .");
	fprintf(stderr, "\n");
    }
    if (strlen(yytext) > 0)
	fprintf(stderr, "\ttoken last read was: '%s'\n", yytext);
    throw 22;
}

// vim:sts=4:ts=8:cino=g0,t0,\:0

