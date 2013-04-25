
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         GSRTFparse
#define yylex           GSRTFlex
#define yyerror         GSRTFerror
#define yylval          GSRTFlval
#define yychar          GSRTFchar
#define yydebug         GSRTFdebug
#define yynerrs         GSRTFnerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 36 "rtfGrammar.y"


/*
  The overall plan is to make this grammer universal in usage.
  Intrested buddies can implement plain C functions to consume what
  the grammer is producing. this way the rtf-grammer-tree can be
  converted to what is needed: GNUstep attributed strings, tex files,
  ...
  
  The plan is laid out by defining a set of C functions which cover
  all what is needed to mangle rtf information (it is NeXT centric
  however and may even lack some features).  Be aware that some
  functions are called at specific times when some information may or
  may not be available. The first argument of all functions is a
  context, which is asked to be maintained by the consumer at
  whichever purpose seems appropriate.  This context must be passed to
  the parser by issuing 'value = GSRTFparse(ctxt, lctxt);' in the
  first place.
*/

#import <AppKit/AppKit.h>
#include <stdlib.h>
#include <string.h>
#include "rtfScanner.h"

/*	this context is passed to the interface functions	*/
typedef void	*GSRTFctxt;
// Two parameters are not supported by some bison versions. The declaration of 
// yyparse in the .c file must be corrected to be able to compile it.
/*#define YYPARSE_PARAM	ctxt, void *lctxt*/
#define YYLEX_PARAM		lctxt
/*#undef YYLSP_NEEDED*/
#define CTXT            ctxt

#define	YYERROR_VERBOSE
#define YYDEBUG 0

#include "RTFConsumerFunctions.h"
/*int GSRTFlex (YYSTYPE *lvalp, RTFscannerCtxt *lctxt); */
int GSRTFlex(void *lvalp, void *lctxt);



/* Line 189 of yacc.c  */
#line 125 "rtfGrammar.tab.m"

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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
#line 82 "rtfGrammar.y"

	int		number;
	const char	*text;
	RTFcmd		cmd;



/* Line 214 of yacc.c  */
#line 266 "rtfGrammar.tab.m"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 278 "rtfGrammar.tab.m"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1300

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  100
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  116
/* YYNRULES -- Number of states.  */
#define YYNSTATES  168

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   352

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,    98,     2,    99,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,    13,    15,    17,    19,    21,
      23,    24,    27,    30,    33,    36,    39,    42,    43,    49,
      50,    56,    57,    63,    64,    70,    71,    77,    78,    84,
      85,    91,    92,    98,   102,   104,   106,   108,   110,   112,
     114,   116,   118,   120,   122,   124,   126,   128,   130,   132,
     134,   136,   138,   140,   142,   144,   146,   148,   150,   152,
     154,   156,   158,   160,   162,   164,   166,   168,   170,   172,
     174,   176,   178,   180,   182,   184,   186,   188,   190,   192,
     194,   196,   197,   199,   201,   203,   204,   205,   215,   216,
     217,   230,   231,   232,   241,   246,   247,   250,   255,   262,
     267,   268,   271,   274,   277,   280,   282,   284,   286,   288,
     290,   292,   294,   299,   300,   303,   308
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     101,     0,    -1,    -1,    -1,    98,   102,     4,   104,   105,
     103,    99,    -1,     5,    -1,     6,    -1,     7,    -1,     8,
      -1,    89,    -1,    -1,   105,   126,    -1,   105,   131,    -1,
     105,   115,    -1,   105,     3,    -1,   105,   106,    -1,   105,
       1,    -1,    -1,    98,   107,   105,   116,    99,    -1,    -1,
      98,   108,     9,   105,    99,    -1,    -1,    98,   109,    10,
     105,    99,    -1,    -1,    98,   110,    11,   105,    99,    -1,
      -1,    98,   111,    12,   105,    99,    -1,    -1,    98,   112,
      13,   105,    99,    -1,    -1,    98,   113,    14,   105,    99,
      -1,    -1,    98,   114,    15,   105,    99,    -1,    98,     1,
      99,    -1,    38,    -1,    39,    -1,    49,    -1,    50,    -1,
      51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,    -1,
      56,    -1,    57,    -1,    85,    -1,    58,    -1,    59,    -1,
      60,    -1,    61,    -1,    63,    -1,    62,    -1,    18,    -1,
      64,    -1,    34,    -1,    35,    -1,    36,    -1,    83,    -1,
      84,    -1,    65,    -1,    66,    -1,    67,    -1,    68,    -1,
      69,    -1,    70,    -1,    71,    -1,    72,    -1,    73,    -1,
      74,    -1,    75,    -1,    76,    -1,    77,    -1,    78,    -1,
      79,    -1,    80,    -1,    81,    -1,    82,    -1,    16,    -1,
      17,    -1,    19,    -1,    89,    -1,    -1,   117,    -1,   120,
      -1,   123,    -1,    -1,    -1,    98,    40,     3,    41,    42,
      99,   118,   105,   119,    -1,    -1,    -1,    98,    43,    46,
       3,    47,     3,    48,     3,    99,   121,   105,   122,    -1,
      -1,    -1,    98,    44,    46,     3,    99,   124,   105,   125,
      -1,    98,    90,   127,    99,    -1,    -1,   127,   128,    -1,
     127,    98,   128,    99,    -1,   127,    98,   128,   106,     3,
      99,    -1,    38,   130,   129,     3,    -1,    -1,   129,    86,
      -1,   129,    87,    -1,   129,    88,    -1,   129,   106,    -1,
      91,    -1,    92,    -1,    93,    -1,    94,    -1,    95,    -1,
      96,    -1,    97,    -1,    98,    37,   132,    99,    -1,    -1,
     132,   133,    -1,    31,    32,    33,     3,    -1,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   195,   195,   195,   195,   198,   199,   200,   201,   203,
     206,   207,   208,   209,   210,   211,   212,   215,   215,   216,
     216,   217,   217,   218,   218,   219,   219,   220,   220,   221,
     221,   222,   222,   223,   231,   238,   245,   252,   259,   266,
     273,   280,   287,   294,   301,   308,   315,   316,   317,   318,
     319,   326,   327,   328,   329,   336,   343,   350,   357,   364,
     371,   378,   385,   392,   399,   406,   413,   420,   421,   428,
     435,   442,   449,   456,   463,   469,   470,   471,   472,   473,
     474,   478,   479,   480,   481,   493,   493,   493,   508,   508,
     508,   522,   522,   522,   531,   534,   535,   536,   537,   543,
     547,   548,   549,   550,   551,   556,   557,   558,   559,   560,
     561,   562,   570,   573,   574,   578,   583
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "RTFtext", "RTFstart", "RTFansi",
  "RTFmac", "RTFpc", "RTFpca", "RTFignore", "RTFinfo", "RTFstylesheet",
  "RTFfootnote", "RTFheader", "RTFfooter", "RTFpict", "RTFplain",
  "RTFparagraph", "RTFdefaultParagraph", "RTFrow", "RTFcell",
  "RTFtabulator", "RTFemdash", "RTFendash", "RTFemspace", "RTFenspace",
  "RTFbullet", "RTFlquote", "RTFrquote", "RTFldblquote", "RTFrdblquote",
  "RTFred", "RTFgreen", "RTFblue", "RTFcolorbg", "RTFcolorfg",
  "RTFunderlinecolor", "RTFcolortable", "RTFfont", "RTFfontSize",
  "RTFNeXTGraphic", "RTFNeXTGraphicWidth", "RTFNeXTGraphicHeight",
  "RTFNeXTHelpLink", "RTFNeXTHelpMarker", "RTFNeXTfilename",
  "RTFNeXTmarkername", "RTFNeXTlinkFilename", "RTFNeXTlinkMarkername",
  "RTFpaperWidth", "RTFpaperHeight", "RTFmarginLeft", "RTFmarginRight",
  "RTFmarginTop", "RTFmarginButtom", "RTFfirstLineIndent", "RTFleftIndent",
  "RTFrightIndent", "RTFalignCenter", "RTFalignJustified", "RTFalignLeft",
  "RTFalignRight", "RTFlineSpace", "RTFspaceAbove", "RTFstyle", "RTFbold",
  "RTFitalic", "RTFunderline", "RTFunderlineDot", "RTFunderlineDash",
  "RTFunderlineDashDot", "RTFunderlineDashDotDot", "RTFunderlineDouble",
  "RTFunderlineStop", "RTFunderlineThick", "RTFunderlineThickDot",
  "RTFunderlineThickDash", "RTFunderlineThickDashDot",
  "RTFunderlineThickDashDotDot", "RTFunderlineWord", "RTFstrikethrough",
  "RTFstrikethroughDouble", "RTFunichar", "RTFsubscript", "RTFsuperscript",
  "RTFtabstop", "RTFfcharset", "RTFfprq", "RTFcpg", "RTFOtherStatement",
  "RTFfontListStart", "RTFfamilyNil", "RTFfamilyRoman", "RTFfamilySwiss",
  "RTFfamilyModern", "RTFfamilyScript", "RTFfamilyDecor", "RTFfamilyTech",
  "'{'", "'}'", "$accept", "rtfFile", "$@1", "$@2", "rtfCharset",
  "rtfIngredients", "rtfBlock", "$@3", "$@4", "$@5", "$@6", "$@7", "$@8",
  "$@9", "$@10", "rtfStatement", "rtfNeXTstuff", "rtfNeXTGraphic", "$@11",
  "$@12", "rtfNeXTHelpLink", "$@13", "$@14", "rtfNeXTHelpMarker", "$@15",
  "$@16", "rtfFontList", "rtfFonts", "rtfFontStatement", "rtfFontAttrs",
  "rtfFontFamily", "rtfColorDef", "rtfColors", "rtfColorStatement", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   100,   102,   103,   101,   104,   104,   104,   104,   104,
     105,   105,   105,   105,   105,   105,   105,   107,   106,   108,
     106,   109,   106,   110,   106,   111,   106,   112,   106,   113,
     106,   114,   106,   106,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   116,   116,   116,   116,   118,   119,   117,   121,   122,
     120,   124,   125,   123,   126,   127,   127,   127,   127,   128,
     129,   129,   129,   129,   129,   130,   130,   130,   130,   130,
     130,   130,   131,   132,   132,   133,   133
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     7,     1,     1,     1,     1,     1,
       0,     2,     2,     2,     2,     2,     2,     0,     5,     0,
       5,     0,     5,     0,     5,     0,     5,     0,     5,     0,
       5,     0,     5,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     1,     1,     1,     0,     0,     9,     0,     0,
      12,     0,     0,     8,     4,     0,     2,     4,     6,     4,
       0,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     4,     0,     2,     4,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     2,     0,     0,     1,     0,     5,     6,     7,     8,
       9,    10,     0,    16,    14,    77,    78,    52,    79,    54,
      55,    56,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    46,    47,    48,    49,    51,    50,    53,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    57,    58,
      45,    80,     0,     0,    15,    13,    11,    12,     0,   113,
      95,    10,     0,     0,     0,     0,     0,     0,     0,     4,
      33,     0,     0,     0,    10,    10,    10,    10,    10,    10,
      10,   116,     0,   112,   114,     0,     0,    94,    96,     0,
       0,    82,    83,    84,     0,     0,     0,     0,     0,     0,
       0,     0,   105,   106,   107,   108,   109,   110,   111,   100,
       0,     0,     0,     0,    18,    20,    22,    24,    26,    28,
      30,    32,     0,     0,     0,    97,     0,     0,     0,     0,
     115,    99,   101,   102,   103,   104,     0,     0,     0,     0,
      98,     0,     0,    91,    85,     0,    10,    10,     0,     0,
       0,     0,    93,    87,    88,    10,     0,    90
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    63,    11,    12,    64,    71,    72,    73,
      74,    75,    76,    77,    78,    65,   100,   101,   157,   163,
     102,   165,   167,   103,   156,   162,    66,    82,    98,   133,
     119,    67,    81,    94
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -114
static const yytype_int16 yypact[] =
{
     -97,  -114,    23,    20,  -114,    -2,  -114,  -114,  -114,  -114,
    -114,  -114,   266,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,    90,   -73,  -114,  -114,  -114,  -114,   -71,  -114,
    -114,  -114,    21,    19,    29,    32,    18,    27,    17,  -114,
    -114,    16,   -13,   351,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,    13,  -114,  -114,   113,     8,  -114,  -114,    -1,
      -9,  -114,  -114,  -114,   436,   521,   606,   691,   776,   861,
     946,    59,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
     -77,    91,    49,    50,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,   110,    24,   181,  -114,   111,    75,   115,   116,
    -114,  -114,  -114,  -114,  -114,  -114,    22,    78,    76,    38,
    -114,    39,   173,  -114,  -114,   129,  -114,  -114,   175,  1031,
    1116,    82,  -114,  -114,  -114,  -114,  1201,  -114
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -114,  -114,  -114,  -114,  -114,    46,  -113,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,    87,  -114,
    -114,  -114,  -114,  -114
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -93
static const yytype_int16 yytable[] =
{
      68,     1,   -17,     6,     7,     8,     9,   136,   -19,   -21,
     -23,   -25,   -27,   -29,   -31,   -17,   -17,   -17,   -17,    91,
     145,   134,   135,     4,     5,    95,    79,   141,    80,    85,
      84,    88,    90,   -17,   -17,   -17,    69,   -17,   -17,   121,
      86,    89,   122,   123,    87,   111,    95,    92,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,    96,    97,    10,   -17,    70,
     124,    68,   132,   -17,   137,   138,   139,   -17,   -17,   -19,
     -21,   -23,   -25,   -27,   -29,   -31,   -17,   -17,   -17,   -17,
     142,   143,   144,   140,   146,    93,   147,    83,   148,   149,
     151,   150,   134,   152,   -17,   -17,   -17,    69,   -17,   -17,
     104,   105,   106,   107,   108,   109,   110,   153,   154,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   155,   158,   161,   -17,
      70,   164,    68,   120,   -17,     0,     0,     0,   -17,   -17,
     -19,   -21,   -23,   -25,   -27,   -29,   -31,   -17,   -17,   -17,
     -17,     0,   159,   160,   112,   113,   114,   115,   116,   117,
     118,   166,     0,     0,     0,   -17,   -17,   -17,     0,   -17,
     -17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,    13,     0,    14,
     -17,     0,     0,     0,     0,     0,     0,     0,     0,   -17,
     -17,     0,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    13,     0,    14,    61,     0,     0,     0,     0,
       0,     0,     0,     0,    62,    -3,     0,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    13,     0,    14,
      61,     0,     0,     0,     0,     0,     0,     0,     0,    99,
     -81,     0,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    13,     0,    14,    61,     0,     0,     0,     0,
       0,     0,     0,     0,    62,   125,     0,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    13,     0,    14,
      61,     0,     0,     0,     0,     0,     0,     0,     0,    62,
     126,     0,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    13,     0,    14,    61,     0,     0,     0,     0,
       0,     0,     0,     0,    62,   127,     0,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    13,     0,    14,
      61,     0,     0,     0,     0,     0,     0,     0,     0,    62,
     128,     0,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    13,     0,    14,    61,     0,     0,     0,     0,
       0,     0,     0,     0,    62,   129,     0,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    13,     0,    14,
      61,     0,     0,     0,     0,     0,     0,     0,     0,    62,
     130,     0,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    13,     0,    14,    61,     0,     0,     0,     0,
       0,     0,     0,     0,    62,   131,     0,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    13,     0,    14,
      61,     0,     0,     0,     0,     0,     0,     0,     0,    62,
     -92,     0,    15,    16,    17,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    20,    21,     0,    22,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    13,     0,    14,    61,     0,     0,     0,     0,
       0,     0,     0,     0,    62,   -86,     0,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,     0,     0,     0,
      61,     0,     0,     0,     0,     0,     0,     0,     0,    62,
     -89
};

static const yytype_int16 yycheck[] =
{
       1,    98,     3,     5,     6,     7,     8,   120,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,     3,
     133,    98,    99,     0,     4,    38,    99,     3,    99,    10,
       9,    13,    15,    34,    35,    36,    37,    38,    39,    40,
      11,    14,    43,    44,    12,    32,    38,    31,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    98,    99,    89,    89,    90,
      99,     1,    33,     3,     3,    46,    46,    98,    99,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      86,    87,    88,     3,     3,    99,    41,    71,     3,     3,
      42,    99,    98,    47,    34,    35,    36,    37,    38,    39,
      84,    85,    86,    87,    88,    89,    90,    99,    99,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,     3,    48,     3,    89,
      90,    99,     1,    96,     3,    -1,    -1,    -1,    98,    99,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    -1,   156,   157,    91,    92,    93,    94,    95,    96,
      97,   165,    -1,    -1,    -1,    34,    35,    36,    -1,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,     1,    -1,     3,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      99,    -1,    16,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     1,    -1,     3,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,    -1,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    -1,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,     1,    -1,     3,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      99,    -1,    16,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     1,    -1,     3,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,    -1,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    -1,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,     1,    -1,     3,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      99,    -1,    16,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     1,    -1,     3,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,    -1,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    -1,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,     1,    -1,     3,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      99,    -1,    16,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     1,    -1,     3,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,    -1,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    -1,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,     1,    -1,     3,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      99,    -1,    16,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     1,    -1,     3,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,    -1,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    -1,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,     1,    -1,     3,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      99,    -1,    16,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    35,    36,    -1,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     1,    -1,     3,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,    -1,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    -1,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      99
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    98,   101,   102,     0,     4,     5,     6,     7,     8,
      89,   104,   105,     1,     3,    16,    17,    18,    19,    34,
      35,    36,    38,    39,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    89,    98,   103,   106,   115,   126,   131,     1,    37,
      90,   107,   108,   109,   110,   111,   112,   113,   114,    99,
      99,   132,   127,   105,     9,    10,    11,    12,    13,    14,
      15,     3,    31,    99,   133,    38,    98,    99,   128,    98,
     116,   117,   120,   123,   105,   105,   105,   105,   105,   105,
     105,    32,    91,    92,    93,    94,    95,    96,    97,   130,
     128,    40,    43,    44,    99,    99,    99,    99,    99,    99,
      99,    99,    33,   129,    98,    99,   106,     3,    46,    46,
       3,     3,    86,    87,    88,   106,     3,    41,     3,     3,
      99,    42,    47,    99,    99,     3,   124,   118,    48,   105,
     105,     3,   125,   119,    99,   121,   105,   122
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (ctxt, lctxt, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, ctxt, lctxt); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *ctxt, void *lctxt)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, ctxt, lctxt)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    void *ctxt;
    void *lctxt;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (ctxt);
  YYUSE (lctxt);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *ctxt, void *lctxt)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, ctxt, lctxt)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    void *ctxt;
    void *lctxt;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, ctxt, lctxt);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, void *ctxt, void *lctxt)
#else
static void
yy_reduce_print (yyvsp, yyrule, ctxt, lctxt)
    YYSTYPE *yyvsp;
    int yyrule;
    void *ctxt;
    void *lctxt;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , ctxt, lctxt);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, ctxt, lctxt); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *ctxt, void *lctxt)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, ctxt, lctxt)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    void *ctxt;
    void *lctxt;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (ctxt);
  YYUSE (lctxt);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void *ctxt, void *lctxt);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *ctxt, void *lctxt)
#else
int
yyparse (ctxt, lctxt)
    void *ctxt;
    void *lctxt;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

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
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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

/* Line 1455 of yacc.c  */
#line 195 "rtfGrammar.y"
    { GSRTFstart(CTXT); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 195 "rtfGrammar.y"
    { GSRTFstop(CTXT); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 198 "rtfGrammar.y"
    { (yyval.number) = 1; ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 199 "rtfGrammar.y"
    { (yyval.number) = 2; ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 200 "rtfGrammar.y"
    { (yyval.number) = 3; ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 201 "rtfGrammar.y"
    { (yyval.number) = 4; ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 203 "rtfGrammar.y"
    { (yyval.number) = 1; free((void*)(yyvsp[(1) - (1)].cmd).name); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 210 "rtfGrammar.y"
    { GSRTFmangleText(CTXT, (yyvsp[(2) - (2)].text)); free((void *)(yyvsp[(2) - (2)].text)); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 215 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, NO); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 215 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, NO); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 216 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 216 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 217 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 217 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 218 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 218 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 219 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 219 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 220 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 220 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 221 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 221 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 222 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 222 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 231 "rtfGrammar.y"
    { int font;
		    
						  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      font = 0;
						  else
						      font = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFfontNumber(CTXT, font); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 238 "rtfGrammar.y"
    { int size;

						  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      size = 24;
						  else
						      size = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFfontSize(CTXT, size); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 245 "rtfGrammar.y"
    { int width; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      width = 12240;
						  else
						      width = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFpaperWidth(CTXT, width);;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 252 "rtfGrammar.y"
    { int height; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      height = 15840;
						  else
						      height = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFpaperHeight(CTXT, height);;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 259 "rtfGrammar.y"
    { int margin; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      margin = 1800;
						  else
						      margin = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFmarginLeft(CTXT, margin);;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 266 "rtfGrammar.y"
    { int margin; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      margin = 1800;
						  else
						      margin = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFmarginRight(CTXT, margin); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 273 "rtfGrammar.y"
    { int margin; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      margin = 1440;
						  else
						      margin = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFmarginTop(CTXT, margin); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 280 "rtfGrammar.y"
    { int margin; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      margin = 1440;
						  else
						      margin = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFmarginButtom(CTXT, margin); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 287 "rtfGrammar.y"
    { int indent; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      indent = 0;
						  else
						      indent = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFfirstLineIndent(CTXT, indent); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 294 "rtfGrammar.y"
    { int indent; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      indent = 0;
						  else
						      indent = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFleftIndent(CTXT, indent);;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 301 "rtfGrammar.y"
    { int indent; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      indent = 0;
						  else
						      indent = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFrightIndent(CTXT, indent);;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 308 "rtfGrammar.y"
    { int location; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      location = 0;
						  else
						      location = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFtabstop(CTXT, location);;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 315 "rtfGrammar.y"
    { GSRTFalignCenter(CTXT); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 316 "rtfGrammar.y"
    { GSRTFalignJustified(CTXT); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 317 "rtfGrammar.y"
    { GSRTFalignLeft(CTXT); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 318 "rtfGrammar.y"
    { GSRTFalignRight(CTXT); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 319 "rtfGrammar.y"
    { int space; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      space = 0;
						  else
						      space = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFspaceAbove(CTXT, space); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 326 "rtfGrammar.y"
    { GSRTFlineSpace(CTXT, (yyvsp[(1) - (1)].cmd).parameter); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 327 "rtfGrammar.y"
    { GSRTFdefaultParagraph(CTXT); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 328 "rtfGrammar.y"
    { GSRTFstyle(CTXT, (yyvsp[(1) - (1)].cmd).parameter); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 329 "rtfGrammar.y"
    { int color; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      color = 0;
						  else
						      color = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFcolorbg(CTXT, color); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 336 "rtfGrammar.y"
    { int color; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      color = 0;
						  else
						      color = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFcolorfg(CTXT, color); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 343 "rtfGrammar.y"
    { int color; 
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      color = 0;
						  else
						      color = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFunderlinecolor(CTXT, color); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 350 "rtfGrammar.y"
    { int script;
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      script = 6;
						  else
						      script = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFsubscript(CTXT, script); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 357 "rtfGrammar.y"
    { int script;
		
		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty)
						      script = 6;
						  else
						      script = (yyvsp[(1) - (1)].cmd).parameter;
						  GSRTFsuperscript(CTXT, script); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 364 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFbold(CTXT, on); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 371 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFitalic(CTXT, on); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 378 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleSingle | NSUnderlinePatternSolid); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 385 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleSingle | NSUnderlinePatternDot); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 392 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleSingle | NSUnderlinePatternDash); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 399 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleSingle | NSUnderlinePatternDashDot); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 406 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleSingle | NSUnderlinePatternDashDotDot); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 413 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleDouble | NSUnderlinePatternSolid); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 420 "rtfGrammar.y"
    { GSRTFunderline(CTXT, NO, NSUnderlineStyleNone); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 421 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleThick | NSUnderlinePatternSolid); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 428 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleThick | NSUnderlinePatternDot); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 435 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleThick | NSUnderlinePatternDash); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 442 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleThick | NSUnderlinePatternDashDot); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 449 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleThick | NSUnderlinePatternDashDotDot); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 456 "rtfGrammar.y"
    { BOOL on;

		                                  if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
						      on = YES;
						  else
						      on = NO;
						  GSRTFunderline(CTXT, on, NSUnderlineStyleSingle | NSUnderlinePatternSolid | NSUnderlineByWordMask); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 463 "rtfGrammar.y"
    {   NSInteger style;
   if ((yyvsp[(1) - (1)].cmd).isEmpty || (yyvsp[(1) - (1)].cmd).parameter)
     style = NSUnderlineStyleSingle | NSUnderlinePatternSolid;
   else
     style = NSUnderlineStyleNone;
   GSRTFstrikethrough(CTXT, style); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 469 "rtfGrammar.y"
    { GSRTFstrikethrough(CTXT, NSUnderlineStyleDouble | NSUnderlinePatternSolid); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 470 "rtfGrammar.y"
    { GSRTFunicode(CTXT, (yyvsp[(1) - (1)].cmd).parameter); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 471 "rtfGrammar.y"
    { GSRTFdefaultCharacterStyle(CTXT); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 472 "rtfGrammar.y"
    { GSRTFparagraph(CTXT); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 473 "rtfGrammar.y"
    { GSRTFparagraph(CTXT); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 474 "rtfGrammar.y"
    { GSRTFgenericRTFcommand(CTXT, (yyvsp[(1) - (1)].cmd)); 
		                                  free((void*)(yyvsp[(1) - (1)].cmd).name); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 493 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 493 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 494 "rtfGrammar.y"
    {
			GSRTFNeXTGraphic (CTXT, (yyvsp[(3) - (9)].text), (yyvsp[(4) - (9)].cmd).parameter, (yyvsp[(5) - (9)].cmd).parameter);
		;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 508 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 508 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 509 "rtfGrammar.y"
    {
			GSRTFNeXTHelpLink (CTXT, (yyvsp[(2) - (12)].cmd).parameter, (yyvsp[(4) - (12)].text), (yyvsp[(6) - (12)].text), (yyvsp[(8) - (12)].text));
		;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 522 "rtfGrammar.y"
    { GSRTFopenBlock(CTXT, YES); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 522 "rtfGrammar.y"
    { GSRTFcloseBlock(CTXT, YES); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 523 "rtfGrammar.y"
    {
			GSRTFNeXTHelpMarker (CTXT, (yyvsp[(2) - (8)].cmd).parameter, (yyvsp[(4) - (8)].text));
		;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 538 "rtfGrammar.y"
    { free((void *)(yyvsp[(5) - (6)].text));;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 543 "rtfGrammar.y"
    { GSRTFregisterFont(CTXT, (yyvsp[(4) - (4)].text), (yyvsp[(2) - (4)].number), (yyvsp[(1) - (4)].cmd).parameter);
                                                          free((void *)(yyvsp[(4) - (4)].text)); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 556 "rtfGrammar.y"
    { (yyval.number) = RTFfamilyNil - RTFfamilyNil; ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 557 "rtfGrammar.y"
    { (yyval.number) = RTFfamilyRoman - RTFfamilyNil; ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 558 "rtfGrammar.y"
    { (yyval.number) = RTFfamilySwiss - RTFfamilyNil; ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 559 "rtfGrammar.y"
    { (yyval.number) = RTFfamilyModern - RTFfamilyNil; ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 560 "rtfGrammar.y"
    { (yyval.number) = RTFfamilyScript - RTFfamilyNil; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 561 "rtfGrammar.y"
    { (yyval.number) = RTFfamilyDecor - RTFfamilyNil; ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 562 "rtfGrammar.y"
    { (yyval.number) = RTFfamilyTech - RTFfamilyNil; ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 579 "rtfGrammar.y"
    { 
		       GSRTFaddColor(CTXT, (yyvsp[(1) - (4)].cmd).parameter, (yyvsp[(2) - (4)].cmd).parameter, (yyvsp[(3) - (4)].cmd).parameter);
		       free((void *)(yyvsp[(4) - (4)].text));
		     ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 584 "rtfGrammar.y"
    { 
		       GSRTFaddDefaultColor(CTXT);
		       free((void *)(yyvsp[(1) - (1)].text));
		     ;}
    break;



/* Line 1455 of yacc.c  */
#line 2750 "rtfGrammar.tab.m"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (ctxt, lctxt, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (ctxt, lctxt, yymsg);
	  }
	else
	  {
	    yyerror (ctxt, lctxt, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, ctxt, lctxt);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, ctxt, lctxt);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (ctxt, lctxt, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, ctxt, lctxt);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, ctxt, lctxt);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 596 "rtfGrammar.y"


/*	some C code here	*/


