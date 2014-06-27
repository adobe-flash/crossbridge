#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "re.h"
/*
extern unsigned char *strcpy(), *strncpy(), *strcat();
extern int strlen();
*/
#include <string.h>
#define CHAR	unsigned char
#define MAXPAT 128
#define MAXPATT 256
#define MAXDELIM 8            /* Max size of a delimiter pattern */
#define SHORTREG 15
#define MAXREG   30
#define MAXNAME  256
#define Max_Pats 12    /* max num of patterns */
#define Max_Keys 12    /* max num of keywords */
#define Max_Psize 128  /* max size of a pattern counting all the characters */
#define Max_Keyword 31 /* the max size of a keyword */
#define WORD 32        /* the size of a word */
#define MaxError 8     /* the max number of errors allowed */
#define MaxRerror 4    /* the max number of erros for regular expression */
#define MaxDelimit 16   /* the max raw length of a user defined delimiter */
#define BlockSize  49152
#define Max_record 49152
#define SIZE 16384       /* BlockSIze in sgrep */
#define MAXLINE   1024  /* maxline in sgrep */
#define Maxline   1024
#define RBLOCK    8192
#define RMAXLINE  1024
#define MaxNext   66000
#define ON 1
#define OFF 0
#define Compl 1
#define Maxresult 10000
#define MaxCan 2500
#define MAXSYM 256 /* ASCII */
#define WORDB     241    /* -w option */
#define LPARENT   242    /* ( */
#define RPARENT   243    /* ) */
#define LRANGE    244    /* [ */
#define RRANGE    245    /* ] */
#define LANGLE    246    /* < */
#define RANGLE    247    /* > */
#define NOTSYM    248    /* ^ */
#define WILDCD    249    /* wildcard */
#define ORSYM     250   /* | */
#define ORPAT     251   /* , */
#define ANDPAT    252   /* ; */
#define STAR      253   /* closure */
#define HYPHEN    237   /* - */
#define NOCARE    238   /* . */
#define NNLINE    239   /* special symbol for newline in begin of pattern*/
                       /* matches '\n' and NNLINE */

