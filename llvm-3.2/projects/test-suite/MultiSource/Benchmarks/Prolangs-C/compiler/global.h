
/****** global.h ********************************************************/

#include <stdio.h>
#include <ctype.h>
/*
extern char *strcpy();
extern int strcmp();
*/

#define STRMAX 999    /* size of lexemes array */
#define SYMMAX 100    /* size of symtable */

#define BSIZE  128           /* buffer size */
#define NONE   -1
#define EOS   '\0'           /* END OF STRING */

#define NUM    256           /* token for integers */
#define ID     259           /* token for identifiers */
#define DONE   260           /* token for end of file */

#define RNUM   2001          /* token for real numbers */
#define INT    2002          /* token for the string "int" */
#define FLOAT  2003          /* token for the string "float" */
#define POP    2004          /* token for poping to a labeled location */
#define PUSH_LABEL 2005      /* token for outputing push from labeled loc. */
#define ARRAY  2006          /* token for an array referance */
#define WHILE  2007          /* token for the keyword while */
#define IF     2008          /* token for the keyword if */
#define READ   2009          /* token for the keyword read */
#define WRITE  2010          /* token for the keyword write */
#define WRITELN 2011         /* token for the keyword writeln */
#define EQUAL  2012          /* token for == */
#define NEQUAL 2013          /* token for != */
#define LABEL_CODE 2014      /* emitter code to label an assemble stmt */
#define PUSH_CODE_LABEL 2015 /* emitter code to push code labels */
#define COMP   2016          /* emitter code for comp opcode */
#define BEQ    2017          /* emitter code for BEQ */ 
#define B      2018          /* emitter code for B */
#define STORE  2020          /* emitter code to output ST */
#define STH    2022          /* emitter code for STH command */
#define VAL    2023          /* emitter code for VAL command */
#define ELSE   2024          /* token for keyword else */
#define BNE    2025          /* emitter code for BNE command */
#define BEGIN  2026          /* emitter code: print special char at start prg*/
#define END    2027          /* emitter code: print special char at end prog */
#define IST    2028          /* emitter code for IST instruction */
#define ADD    2029          /* emitter code for ADD */
#define RADD   2030          /* emitter code for RADD */
#define SUB    2031          /* emitter code for SUB */
#define RSUB   2032          /* emitter code for RSUB */
#define MUL    2033          /* emitter code for MUL */
#define RMUL   2034          /* emitter code for RMUL */
#define DIV    2035          /* emitter code for DIV */
#define RDIV   2036          /* emitter code for RDIV */
#define SWAP   2037          /* emitter code for SWAP */
#define NEG    2038          /* emitter code for NEG */
#define RNEG   2039          /* emitter code for RNEG */
#define FIX    2040          /* emitter code for FIX */
#define PARAM  2041          /* code for parameters in symbol tables */
#define RETURN 2042          /* token for return keyword */
#define READF  2043          /* token for readf() */
#define READI  2044          /* token for readi() */
#define WRITEF 2045          /* token for writef() */
#define WRITEI 2046          /* token for writei() */
#define LOC_DECL 2047        /* emitter code for a local declaration */
#define GLO_DECL 2048        /* emitter code for a global declaration */
#define PUSH_LOC_VAR_ADDR 2049 /* emitter code to push the address of a
                                  local variable */
#define PUSH_GLO_VAR_ADDR 2050 /* emitter code to push the address of a
                                  global varialbe */
#define PUSH_LOC_VAR_VALUE 2051 /* emitter code to push value stored
                                  in a local variable */
#define PUSH_GLO_VAR_VALUE 2052 /* emitter code to push value stored
                                  in a global variable */
#define STHB       2053      /* emitter code for STHB */
#define LABEL_FUNC 2054      /* emitter code to insert function name in code */
#define ISTB       2055      /* emitter code for ISTB instruction */
#define STORE_RA   2056      /* emitter code to store return address */
#define SAVE_FRAMESIZE 2057  /* emitter code to save frame size */
#define PUSH_RA    2058      /* emitter code to push the return address */
#define PUSH_FRAMESIZE 2059  /* emitter code for pushing the frame size */
#define IB         2060      /* emitter code for ib */
#define DB         2061      /* emitter code for db */
#define NOTDEC     2062      /* emitter code for funtion not parsed */
#define VALB       2063      /* emitter code for VALB */

int     lookahead;           /* token code number */
int     tokenval;            /* value of token attribute */
float   ftokenval;           /* global for float RNUM attribute */
int     FloatFlag;           /* used as boolean, 1 if we doing float calcs. */
int     ErrorFlag;           /* 1 if an error has occured, 0 otherwise */
int     DecCount;            /* keeps track of memeory locations */
int     offset;              /* offset within an activation record, local var*/
int     lineno;              /* the current line number */
int     LabelCounter;        /* used to create new labels for the mach. code */
int     NumberC;             /* keeps track of number of commands emitted */
char    lexbuf[BSIZE];       /* stores lexeme to be inserted into symtable */
int     LocalIndex;          /* value returned from lookup in loctable */
int     GlobalIndex;         /* value returned from lookup to glotable */
int     NextLookahead;       /* the next token of the input */
int     NextTokenval;        /* the tokenval associated with the next token */
float   NextFtokenval;       /* the ftokenval associated with the next token */
int     PreviousLookahead;   /* the token that was last matched */
int     PreviousTokenval;    /* tokenval associated with the previous token*/
float   PreviousFtokenval;   /* ftokenval associated with the previous token*/
int     Scope;               /* the label assoc. with the current function
                                to be used in the creation of unique labels */
int     ReturnLabel;         /* stores the label to jump out of a function
                                if a return is encountered */
int     CallReturnAddr;      /* return address of a function call */
int     FuncNameIndex;       /* stores the index of the current function
                                name*/
int     ArrayParsed;         /* flag set to 1 if PushArrayCellAddr() called
                                while lookahead is ARRAY */

struct entry {   /* form of symbol table entry */
     char *lexptr;           /* ptr to lexeme */
     int  token;             /* token: ID or ARRAY */
     int  type;              /* type of entry: INT or FLOAT */
     int  size;              /* size of entry: 1 for INT or FLOAT, 
                                larger for arrays */
     int  function;          /* non-zero if is a function.
                                field is > 0 if function called but not parsed,
                                field is < 0 if function body has been parsed*/
     int  functionlabel;     /* stores the machine code label associated
                                with the function */
   };


struct entry GlobalTable[SYMMAX];    /* Global  symbol table */
struct entry LocalTable[SYMMAX];    /* Local symbol table */

