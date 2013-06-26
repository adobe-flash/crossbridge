/* "p2c", a Pascal to C translator, version 1.20.
   Copyright (C) 1989, 1990, 1991 Free Software Foundation.
   Author: Dave Gillespie.
   Author's address: daveg@csvax.caltech.edu; 256-80 Caltech/Pasadena CA 91125.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (any version).

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */




#ifdef __STDC__
# define PP(x)  x             /* use true prototypes */
# define PV()   (void)
# define Anyptr void
# define __CAT__(a,b)a##b
#else
# define PP(x)  ()            /* use old-style declarations */
# define PV()   ()
# define Anyptr char
# define __ID__(a)a
# define __CAT__(a,b)__ID__(a)b
#endif

#define Static                /* For debugging purposes */



#include <stdio.h>


/* If the following heuristic fails, compile -DBSD=0 for non-BSD systems,
   or -DBSD=1 for BSD systems. */

#ifdef M_XENIX
# define BSD 0
#endif

#ifdef FILE       /* a #define in BSD, a typedef in SYSV (hp-ux, at least) */
# ifndef BSD
#  define BSD 1
# endif
#endif

#ifdef BSD
# if !BSD
#  undef BSD
# endif
#endif


#ifdef __STDC__
/* # include <stddef.h> */
# include <stdlib.h>
# include <limits.h>
#else
# ifndef BSD
#  include <malloc.h>
#  include <memory.h>
#  include <values.h>
# endif
# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1
# define CHAR_BIT 8
# define LONG_MAX (((unsigned long)~0L) >> 1)
# define LONG_MIN (- LONG_MAX - 1)
#endif



#if defined(BSD) && !defined(__STDC__)
# include <strings.h>
# define memcpy(a,b,n) bcopy(b,a,n)
# define memcmp(a,b,n) bcmp(a,b,n)
char *malloc(), *realloc();
#else
# include <string.h>
#endif

#include <ctype.h>


#ifdef __GNUC__      /* Fast, in-line version of strcmp */
# define strcmp(a,b) ({ char *_aa = (a), *_bb = (b); int _diff;  \
			for (;;) {    \
			    if (!*_aa && !*_bb) { _diff = 0; break; }   \
                            if (*_aa++ != *_bb++)    \
				{ _diff = _aa[-1] - _bb[-1]; break; }   \
			} _diff; })
#endif


#if defined(HASDUMPS) && defined(define_globals)
# define DEFDUMPS
#endif



/* Constants */

#undef MININT      /* we want the Pascal definitions, not the local C definitions */
#undef MAXINT

#define MININT     0x80000000
#define MAXINT     0x7fffffff


#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS  0
# define EXIT_FAILURE  1
#endif


#ifndef P2C_HOME
# ifdef citPWS
#  define    P2C_HOME        "/lib/p2c"
# else
#  define    P2C_HOME        "/usr/local/p2c"     /* sounds reasonable... */
# endif
#endif

#ifdef define_globals
char *p2c_home = P2C_HOME;
#else
extern char *p2c_home;
#endif

#define P2C_VERSION  "1.20"




/* Types */

#ifdef __STDC__
typedef void *anyptr;
#else
typedef char *anyptr;
#endif

typedef unsigned char uchar;



/* Ought to rearrange token assignments at the next full re-compile */

typedef enum E_token {
    TOK_NONE,

    /* reserved words */
    TOK_AND, TOK_ARRAY, TOK_BEGIN, TOK_CASE, TOK_CONST,
    TOK_DIV, TOK_DO, TOK_DOWNTO, TOK_ELSE, TOK_END,
    TOK_FILE, TOK_FOR, TOK_FUNCTION, TOK_GOTO, TOK_IF,
    TOK_IN, TOK_LABEL, TOK_MOD, TOK_NIL, TOK_NOT,
    TOK_OF, TOK_OR, TOK_PACKED, TOK_PROCEDURE, TOK_PROGRAM,
    TOK_RECORD, TOK_REPEAT, TOK_SET, TOK_THEN, TOK_TO,
    TOK_TYPE, TOK_UNTIL, TOK_VAR, TOK_WHILE, TOK_WITH,

    /* symbols */
    TOK_DOLLAR, TOK_STRLIT, TOK_LPAR, TOK_RPAR, TOK_STAR,
    TOK_PLUS, TOK_COMMA, TOK_MINUS, TOK_DOT, TOK_DOTS,
    TOK_SLASH, TOK_INTLIT, TOK_REALLIT, TOK_COLON, TOK_ASSIGN,
    TOK_SEMI, TOK_NE, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_EQ, TOK_LBR, TOK_RBR, TOK_HAT,
    TOK_INCLUDE, TOK_ENDIF,
    TOK_IDENT, TOK_MININT, TOK_EOF,

    /* C symbols */
    TOK_ARROW, TOK_AMP, TOK_VBAR, TOK_BANG,
    TOK_TWIDDLE, TOK_PERC, TOK_QM,
    TOK_LTLT, TOK_GTGT, TOK_EQEQ, TOK_BANGEQ,
    TOK_PLPL, TOK_MIMI, TOK_ANDAND, TOK_OROR,
    TOK_LBRACE, TOK_RBRACE, TOK_CHARLIT,

    /* HP Pascal tokens */
    TOK_ANYVAR, TOK_EXPORT, TOK_IMPLEMENT, TOK_IMPORT, TOK_MODULE,
    TOK_OTHERWISE, TOK_RECOVER, TOK_TRY,

    /* Turbo Pascal tokens */
    TOK_SHL, TOK_SHR, TOK_XOR, TOK_INLINE, TOK_ABSOLUTE,
    TOK_INTERRUPT, TOK_ADDR, TOK_HEXLIT,

    /* Oregon Software Pascal tokens */
    TOK_ORIGIN, TOK_INTFONLY,

    /* VAX Pascal tokens */
    TOK_REM, TOK_VALUE, TOK_VARYING, TOK_OCTLIT, TOK_COLONCOLON,
    TOK_STARSTAR,

    /* Modula-2 tokens */
    TOK_BY, TOK_DEFINITION, TOK_ELSIF, TOK_FROM, TOK_LOOP,
    TOK_POINTER, TOK_QUALIFIED, TOK_RETURN,

    /* UCSD Pascal tokens */
    TOK_SEGMENT,

    TOK_LAST
} Token;

#ifdef define_globals
char *toknames[(int)TOK_LAST] = { "",
    "AND", "ARRAY", "BEGIN", "CASE", "CONST",
    "DIV", "DO", "DOWNTO", "ELSE", "END",
    "FILE", "FOR", "FUNCTION", "GOTO", "IF",
    "IN", "LABEL", "MOD", "NIL", "NOT",
    "OF", "OR", "PACKED", "PROCEDURE", "PROGRAM",
    "RECORD", "REPEAT", "SET", "THEN", "TO",
    "TYPE", "UNTIL", "VAR", "WHILE", "WITH",

    "a '$'", "a string literal", "a '('", "a ')'", "a '*'",
    "a '+'", "a comma", "a '-'", "a '.'", "'..'",
    "a '/'", "an integer", "a real number", "a colon", "a ':='",
    "a semicolon", "a '<>'", "a '<'", "a '>'", "a '<='", "a '>='",
    "an '='", "a '['", "a ']'", "a '^'",
    "an \"include\" file", "$end$",
    "an identifier", "an integer", "end of file",

    "an '->'", "an '&'", "a '|'", "a '!'", 
    "a '~'", "a '%'", "a '?'",
    "a '<<'", "a '>>'", "a '=='", "a '!='",
    "a '++'", "a '--'", "a '&&'", "a '||'",
    "a '{'", "a '}'", "a character literal",

    "ANYVAR", "EXPORT", "IMPLEMENT", "IMPORT", "MODULE",
    "OTHERWISE", "RECOVER", "TRY",

    "SHL", "SHR", "XOR", "INLINE", "ABSOLUTE",
    "INTERRUPT", "an '@'", "a hex integer",

    "ORIGIN", "INTF-ONLY",

    "REM", "VALUE", "VARYING", "an octal integer", "a '::'",
    "a '**'",

    "BY", "DEFINITION", "ELSIF", "FROM", "LOOP",
    "POINTER", "QUALIFIED", "RETURN",

    "SEGMENT"
} ;
#else
extern char *toknames[];
#endif /*define_globals*/

typedef struct S_strlist {
    struct S_strlist *next;
    long value;
    char s[1];
} Strlist;



typedef struct S_value {
    struct S_type *type;
    long i;
    char *s;
} Value;



/* "Symbol" notes:
 *
 * The symbol table is used for several things.  Mainly it records all
 * identifiers in the Pascal program (normally converted to upper case).
 * Also used for recording certain properties about C and Pascal names.
 *
 * The symbol table is a hash table of binary trees.
 */

#define AVOIDNAME  0x1         /* Avoid this name in C code */
#define WARNNAME   0x2	       /* Warn if using this name in C code */
#define AVOIDGLOB  0x4	       /* Avoid C name except private to module */
#define NOSIDEEFF  0x8	       /* Function by this name has no side effects */
#define STRUCTF    0x10	       /* Function by this name is a StructFunction */
#define STRLAPF    0x20	       /* Function by this name is a StrlapFunction */
#define LEAVEALONE 0x40	       /* Do not use custom handler for function */
#define DETERMF    0x80	       /* Function by this name is Deterministic */
#define FMACREC    0x100       /* Used by FieldMacro stuff */
#define AVOIDFIELD 0x200       /* Avoid this name as a struct field name */
#define NEEDSTATIC 0x400       /* This name must be declared static */
#define KWPOSS     0x800       /* This word may be a keyword */
#define FUNCBREAK  0x7000      /* Line breaking flags (see sys.p2crc) */
# define FALLBREAK  0x1000     /*  Break at all commas if at any */
# define FSPCARG1   0x2000     /*  First argument is special */
# define FSPCARG2   0x3000     /*  First two arguments are special */
# define FSPCARG3   0x4000     /*  First three arguments are special */
#define WARNLIBR   0x8000      /* Warn for all uses of this library function */
#define FWDPARAM   0x10000     /* Was a param name for forward-declared func */
#define SSYNONYM   0x20000     /* Symbol is a synonym for another */

typedef struct S_symbol {
    struct S_symbol *left;     /* Left pointer in binary tree */
    struct S_symbol *right;    /* Right pointer in binary tree */
    struct S_meaning *mbase;   /* First normal meaning for this symbol */
    struct S_meaning *fbase;   /* First record-field meaning for this symbol */
    Strlist *symbolnames;      /* List of NameOf's for this name */
    long flags;		       /* (above) */
    Token kwtok;	       /* Token, if symbol is a keyword */
    char name[1];              /* Pascal name (actually variable-sized) */
} Symbol;



/* "Meaning" notes:
 *
 * This represents one meaning of a symbol (see below).  Meanings are
 * organized in a tree of contexts (i.e., scopes), and also in linked
 * lists of meanings per symbol.  Fields described in the following are
 * undefined for kinds where they are not listed.  Other fields are
 * defined in all kinds of meanings.
 *
 * MK_MODULE:  Program, module, or unit.
 *    mp->anyvarflag = 1 if main program, 0 if module.
 *    mp->cbase => First meaning in module's context.
 *
 * MK_CONST:  Pascal CONST.
 *    mp->type => Type of constant, same as mp->constdefn->type & mp->val.type.
 *    mp->anyvarflag = 1 if FoldConstants was true when defined.
 *    mp->constdefn => Expression for the value of the constant.
 *    mp->val = Value of the const, if can be evaluated, else val.type is NULL.
 *    mp->xnext => Next constant in enumeration, else NULL.
 *    mp->isreturn = 1 if constant was declared as a macro (with #define).
 *
 * MK_TYPE:  Pascal type name.
 *    mp->type => Type which name represents.
 *
 * MK_VAR:  Normal variable.
 *    mp->type => Type of variable.
 *    mp->constdefn => Initializer for variable, else NULL.
 *    mp->varstructflag = 1 if variable is in parent function's varstruct.
 *    mp->isforward = 1 if should be declared static.
 *    mp->isfunction = 1 if should be declared extern.
 *    mp->namedfile = 1 if this file variable has a shadow file-name variable.
 *    mp->bufferedfile = 1 if this file variable has a shadow buffer variable.
 *    mp->val.s => name format string if temporary var, else NULL.
 *
 * MK_VARREF:  Variable always referenced through a pointer.
 *    mp->type => Type "pointer to T" where T is type of variable.
 *    mp->constdefn => Initializer for the pointer, else NULL.
 *    (Others same as for MK_VAR.)
 *
 * MK_VARMAC:  Variable which has a VarMacro.
 *    mp->type => Type of variable.
 *    mp->constdefn => Expression for VarMacro definition.
 *    (Others same as for MK_VAR.)
 *
 * MK_SPVAR:  Special variable.
 *    mp->handler => C function to parse and translate the special variable.
 *
 * MK_FIELD:  Record/struct field name.
 *    mp->ctx, cbase = unused (unlike other meanings).
 *    mp->cnext => Next field in record or variant.
 *    mp->type => Type of field (base type if a bit-field).
 *    mp->rectype => Type of containing record.
 *    mp->constdefn => Expression for definition if FieldMacro, else NULL.
 *    mp->val.i = Number of bits if bit-field, or 0 if normal field.
 *    mp->val.type => True type of bit-field, else same as mp->type.
 *    mp->isforward = 1 if tag field for following variant, else 0.
 *    mp->namedfile = 1 if this file field has a shadow file-name field.
 *    mp->bufferedfile = 1 if this file field has a shadow buffer field.
 *
 * MK_VARIANT:  Header for variant record case.
 *    mp->ctx => First field in variant (unlike other meanings).
 *    mp->cbase = unused (unlike other meanings).
 *    mp->cnext => Next variant in record (or next sub-variant in variant).
 *    mp->rectype => Type of containing record.
 *    mp->val = Tag value of variant.
 *
 * MK_LABEL:  Statement label.
 *    mp->val.i => Case number if used by non-local gotos, else -1.
 *    mp->xnext => MK_VAR representing associated jmp_buf variable.
 *    (All optional fields are unused.)
 *
 * MK_FUNCTION:  Procedure or function.
 *    mp->type => TK_FUNCTION type.
 *    mp->cbase => First meaning in procedure's context (when isfunction is 1,
 *		   this will always be the return-value meaning.)
 *    mp->val.i => Body of the function (cast to Stmt *).
 *    mp->constdefn => Expression for definition if FuncMacro, else NULL.
 *    mp->handler => C function to adjust parse tree if predefined, else NULL.
 *    mp->isfunction = 1 if function, 0 if procedure.
 *    mp->isforward = 1 if function has been declared forward.
 *    mp->varstructflag = 1 if function has a varstruct.
 *    mp->needvarstruct = 1 if no varstruct yet but may need one.
 *    mp->namedfile = 1 if function should be declared "inline".
 *
 * MK_SPECIAL:  Special, irregular built-in function.
 *    mp->handler => C function to parse and translate the special function.
 *    mp->constdefn => Expression for definition if FuncMacro, else NULL.
 *    mp->isfunction = 1 if function, 0 if procedure.
 *
 * MK_PARAM:  Procedure or function parameter, or function return value.
 *    mp->type => Type of parameter.
 *    mp->isreturn = 1 if a function return value (not on parameter list).
 *    mp->xnext => Next parameter of function.
 *    mp->fakeparam = 1 if a fake parameter (e.g., conformant array size).
 *    mp->othername => Name of true param if this one is a local copy.
 *    mp->rectype => Type of true param if this one is a local copy.
 *		     If a normal copy param, will be "pointer to" mp->type.
 *		     If copied for varstruct reasons, will be same as mp->type.
 *    mp->varstructflag = 1 if variable is in parent function's varstruct.
 *
 * MK_VARPARAM:  VAR parameter, or StructFunction return value.
 *    mp->type => Type "pointer to T" where T is type of parameter.
 *    mp->anyvarflag = 1 if no type checking is to be applied to parameter.
 *    mp->isreturn = 1 if a StructFunction return value (will be first param).
 *    (Others same as for MK_PARAM.)
 *
 * MK_VARPARAM with mp->type == tp_anyptr:  Turbo "typeless var" parameter.
 *    mp->type = tp_anyptr.
 *    mp->anyvarflag = 1.
 *    (Others same as for MK_PARAM.)
 *
 * MK_VARPARAM with mp->type == tp_strptr:  HP Pascal "var s:string" parameter.
 *    mp->type = tp_strptr.
 *    mp->anyvarflag = 1 if a separate "strmax" parameter is passed.
 *    (Others same as for MK_PARAM.)
 *
 * MK_SYNONYM:  Meaning which should be treated as identical to another.
 *    mp->xnext => Actual meaning to be used.
 *
 */

enum meaningkind {
    MK_NONE, MK_SPECIAL,
    MK_MODULE, MK_FUNCTION, MK_CONST, MK_VAR, MK_TYPE,
    MK_FIELD, MK_LABEL, MK_VARIANT,
    MK_PARAM, MK_VARPARAM, MK_VARREF, MK_VARMAC,
    MK_SPVAR, MK_SYNONYM,
    MK_LAST
} ;

#ifdef DEFDUMPS
char *meaningkindnames[(int)MK_LAST] = {
    "MK_NONE", "MK_SPECIAL",
    "MK_MODULE", "MK_FUNCTION", "MK_CONST", "MK_VAR", "MK_TYPE",
    "MK_FIELD", "MK_LABEL", "MK_VARIANT",
    "MK_PARAM", "MK_VARPARAM", "MK_VARREF", "MK_VARMAC",
    "MK_SPVAR", "MK_SYNONYM"
} ;
#endif /*DEFDUMPS*/

typedef struct S_meaning {
    struct S_meaning *snext;   /* Next meaning for this symbol */
    struct S_meaning *cnext;   /* Next meaning in this meaning's context */
    struct S_meaning *cbase;   /* First meaning in this context */
    struct S_meaning *ctx;     /* Context of this meaning */
    struct S_meaning *xnext;   /* (above) */
    struct S_meaning *dtype;   /* Declared type name, if any */
    struct S_symbol *sym;      /* Symbol of which this is a meaning */
    struct S_type *type;       /* (above) */
    struct S_type *rectype;    /* (above) */
    struct S_expr *constdefn;  /* (above) */
    enum meaningkind kind;     /* Kind of meaning */
    unsigned needvarstruct:1,  /* (above) */
             varstructflag:1,  /* (above) */
             wasdeclared:1,    /* Declaration has been written for meaning */
             istemporary:1,    /* Is a temporary variable */
             isforward:1,      /* (above) */
             isfunction:1,     /* (above) */
             anyvarflag:1,     /* (above) */
             isactive:1,       /* Meaning is currently in scope */
             exported:1,       /* Meaning is visible outside this module */
             warnifused:1,     /* WarnNames was 1 when meaning was declared */
             dumped:1,	       /* Has been dumped (for debugging) */
             isreturn:1,       /* (above) */
             fakeparam:1,      /* (above) */
             namedfile:1,      /* (above) */
             bufferedfile:1,   /* (above) */
             volatilequal:1,   /* Object has C "volatile" qualifier */
             constqual:1,      /* Object has C "const" qualifier */
             dummy17:1, dummy18:1, dummy19:1, 
	     dummy20:1, dummy21:1, dummy22:1, dummy23:1, dummy24:1, dummy25:1, 
	     dummy26:1, dummy27:1, dummy28:1, dummy29:1, dummy30:1, dummy31:1;
    Value val;		       /* (above) */
    int refcount;	       /* Number of references to meaning in program */
    char *name;		       /* Print name (i.e., C name) of the meaning */
    char *othername;	       /* (above) */
    struct S_expr *(*handler)();   /* Custom translator for procedure */
    Strlist *comments;	       /* Comments associated with meaning */
} Meaning;



/* "Type" notes:
 *
 * This struct represents a data type.  Types are stored in a strange
 * cross between Pascal and C semantics.  (This usually works out okay.)
 *
 * TK_INTEGER:  Base integer type.
 *    The following types are TK_INTEGER:
 *        tp_integer, tp_unsigned, tp_int, tp_uint, tp_sint.
 *    All other integer types are represented by subranges.
 *    tp->smin => Minimum value for integer.
 *    tp->smax => Maximum value for integer.
 *
 * TK_CHAR:  Base character type.
 *    The following types are TK_CHAR:  tp_char, tp_schar, tp_uchar.
 *    All other character types are represented by subranges.
 *    tp->smin => Minimum value for character.
 *    tp->smax => Maximum value for character.
 *
 * TK_BOOLEAN:  Boolean type.
 *    The only TK_BOOLEAN type is tp_boolean.
 *    tp->smin => "False" expression.
 *    tp->smax => "True" expression.
 *
 * TK_REAL:  Real types.
 *    The only TK_REAL types are tp_real, tp_longreal, and/or the SINGLE type.
 *
 * TK_VOID:  C "void" type.
 *    The only TK_VOID type is tp_void.
 *
 * TK_SUBR:  Subrange of ordinal type.
 *    tp->basetype => a TK_INTEGER, TK_CHAR, TK_BOOLEAN, or TK_ENUM type.
 *    tp->smin => Minimum ordinal value for subrange.
 *    tp->smax => Maximum ordinal value for subrange.
 *
 * TK_ENUM:  Enumerated type.
 *    tp->fbase => First enumeration constant.
 *    tp->smin => Minimum value (zero).
 *    tp->smax => Maximum value (number of choices minus 1).
 *
 * TK_POINTER:  Pointer type.
 *    tp->basetype => Base type of pointer.
 *    tp->smin => EK_NAME for type if not-yet-resolved forward; else NULL.
 *    tp->fbase => Actual type name for tp->basetype, or NULL.
 *    Only one pointer type is ever generated for a given other type;
 *    each tp->pointertype points back to that type if it has been generated.
 *
 * TK_STRING:  Pascal string or VARYING OF CHAR type.
 *    tp->basetype => tp_char.
 *    tp->indextype => TK_SUBR from 0 to maximum string length.
 *    tp->structdefd = 1 if type is for a conformant VARYING OF CHAR parameter.
 *
 * TK_RECORD:  Pascal record/C struct type.
 *    tp->fbase => First field in record.
 *    tp->structdefd = 1 if struct type has been declared in output.
 *
 * TK_ARRAY with smax == NULL:  Normal array type.
 *    tp->basetype => Element type of array.
 *    tp->indextype => Index type (usually a TK_SUBR).
 *    tp->smin => Integer constant if SkipIndices was used, else NULL.
 *    tp->smax = NULL.
 *    tp->structdefd = 1 if type is for a conformant array parameter.
 *    tp->fbase => Actual type name for tp->basetype, or NULL.
 *
 * TK_ARRAY with smax != NULL:  Large packed array type.
 *    tp->basetype => Element type of C array (tp_ubyte/tp_sbyte/tp_sshort).
 *    tp->indextype => Index type (usually a TK_SUBR).
 *    tp->smin => Integer constant if SkipIndices was used, else NULL.
 *    tp->smax => EK_TYPENAME for element type of Pascal array.
 *    tp->escale = log-base-two of number of bits per packed element, else 0.
 *    tp->issigned = 1 if packed array elements are signed, 0 if unsigned.
 *    tp->structdefd = 1 if type is for a conformant array parameter.
 *    tp->fbase => Actual type name for tp->basetype, or NULL.
 *
 * TK_SMALLARRAY:  Packed array fitting within a single integer.
 *    (Same as for packed TK_ARRAY.)
 *
 * TK_SET:  Normal set type.
 *    tp->basetype => tp_integer.
 *    tp->indextype => Element type of the set.
 *
 * TK_SMALLSET:  Set fitting within a single integer.
 *    (Same as for TK_SET.)
 *
 * TK_FILE:  File type (corresponds to C "FILE" type).
 *    tp->basetype => Type of file elements, or tp_abyte if UCSD untyped file.
 *    A Pascal "file" variable is represented as a TK_POINTER to a TK_FILE.
 *
 * TK_BIGFILE:  File type with attached buffers and name.
 *    tp->basetype => Type of file elements, or tp_abyte if UCSD untyped file.
 *    A Pascal "file" variable is represented directly as a TK_BIGFILE.
 *
 * TK_FUNCTION:  Procedure or procedure-pointer type.
 *    tp->basetype => Return type of function, or tp_void if procedure.
 *    tp->issigned = 1 if type has a generic static link.
 *    tp->fbase => First argument (or StructFunction return buffer pointer).
 *
 * TK_PROCPTR:  Procedure pointer with static link.
 *    tp->basetype => TK_FUNCTION type.
 *    tp->fbase => Internal Meaning struct associated with basetype.
 *    tp->escale = Value of StaticLinks when type was declared.
 *
 * TK_CPROCPTR:  Procedure pointer without static link.
 *    tp->basetype => TK_FUNCTION type.
 *    tp->fbase => Internal Meaning struct associated with basetype.
 *    tp->escale = Value of StaticLinks = 0.
 *
 * TK_SPECIAL:  Special strange data type.
 *    Only TK_SPECIAL type at present is tp_jmp_buf.
 *
 */

enum typekind {
    TK_NONE,
    TK_INTEGER, TK_CHAR, TK_BOOLEAN, TK_REAL, TK_VOID,
    TK_SUBR, TK_ENUM, TK_POINTER, TK_STRING,
    TK_RECORD, TK_ARRAY, TK_SET, TK_FILE, TK_FUNCTION,
    TK_PROCPTR, TK_SMALLSET, TK_SMALLARRAY, TK_CPROCPTR,
    TK_SPECIAL, TK_BIGFILE,
    TK_LAST
} ;

#ifdef DEFDUMPS
char *typekindnames[(int)TK_LAST] = {
    "TK_NONE",
    "TK_INTEGER", "TK_CHAR", "TK_BOOLEAN", "TK_REAL", "TK_VOID",
    "TK_SUBR", "TK_ENUM", "TK_POINTER", "TK_STRING",
    "TK_RECORD", "TK_ARRAY", "TK_SET", "TK_FILE", "TK_FUNCTION",
    "TK_PROCPTR", "TK_SMALLSET", "TK_SMALLARRAY", "TK_CPROCPTR",
    "TK_SPECIAL", "TK_BIGFILE"
} ;
#endif /*DEFDUMPS*/

typedef struct S_type {
    enum typekind kind;        /* Kind of type */
    struct S_type *basetype;   /* (above) */
    struct S_type *indextype;  /* (above) */
    struct S_type *pointertype; /* Pointer to this type */
    struct S_meaning *meaning; /* Name of this type, if any */
    struct S_meaning *fbase;   /* (above) */
    struct S_expr *smin;       /* (above) */
    struct S_expr *smax;       /* (above) */
    unsigned issigned:1,       /* (above) */
             dumped:1,         /* Has been dumped (for debugging) */
             structdefd:1,     /* (above) */
             preserved:1;      /* Declared with preservetypes = 1 */
    short escale;              /* (above) */
} Type;


/* "Expr" notes:
 *
 * Expression trees generally reflect C notation and semantics.  For example,
 * EK_ASSIGN is not generated for string arguments; these would get an
 * EK_BICALL to strcpy instead.
 *
 * The data type of each expression node is stored in its "val.type" field.
 * The rest of the "val" field is used only when shown below.
 * The "nargs" field always contains the number of arguments; the "args"
 * array is allocated to that size and will contain non-NULL Expr pointers.
 *
 * EK_EQ, EK_NE, EK_LT, EK_GT, EK_LE, EK_GE:  Relational operators.
 *    ep->nargs = 2.
 *
 * EK_PLUS:  Addition.
 *    ep->nargs >= 2.
 *
 * EK_NEG:  Negation.
 *    ep->nargs = 1.
 *
 * EK_TIMES:  Multiplication.
 *    ep->nargs >= 2.
 *
 * EK_DIVIDE:  Real division.
 *    ep->nargs = 2.
 *
 * EK_DIV:  Integer division.
 *    ep->nargs = 2.
 *
 * EK_MOD:  Integer modulo (C "%" operator).
 *    ep->nargs = 2.
 *
 * EK_OR, EK_AND:  Logical operators (C "&&" and "||").
 *    ep->nargs = 2.
 *
 * EK_NOT:  Logical NOT (C "!" operator).
 *    ep->nargs = 1.
 *
 * EK_BAND, EK_BOR, EK_BXOR:  Bitwise operators (C "&", "|", "^").
 *    ep->nargs = 2.
 *
 * EK_BNOT:  Bitwise NOT (C "~" operator).
 *    ep->nargs = 1.
 *
 * EK_LSH, EK_RSH:  Shift operators.
 *    ep->nargs = 2.
 *
 * EK_HAT:  Pointer dereference.
 *    ep->nargs = 1.
 *
 * EK_INDEX:  Array indexing.
 *    ep->nargs = 2.
 *
 * EK_CAST:  "Soft" type cast, change data type retaining value.
 *    ep->type => New data type.
 *    ep->nargs = 1.
 *
 * EK_ACTCAST:  "Active" type cast, performs a computation as result of cast.
 *    ep->type => New data type.
 *    ep->nargs = 1.
 *
 * EK_LITCAST:  Literal type cast.
 *    ep->nargs = 2.
 *    ep->args[0] => EK_TYPENAME expression for name of new data type.
 *    ep->args[1] => Argument of cast.
 *
 * EK_DOT:  Struct field extraction.
 *    ep->nargs = 1.  (Only one of the following will be nonzero:)
 *    ep->val.i => MK_FIELD being extracted (cast to Meaning *), else 0.
 *    ep->val.s => Literal name of field being extracted, else NULL.
 *
 * EK_COND:  C conditional expression.
 *    ep->nargs = 3.
 *    ep->args[0] => Condition expression.
 *    ep->args[1] => "Then" expression.
 *    ep->args[2] => "Else" expression.
 *
 * EK_ADDR:  Address-of operator.
 *    ep->nargs = 1.
 *
 * EK_SIZEOF:  Size-of operator.
 *    ep->nargs = 1.
 *    ep->args[0] => Argument expression, may be EK_TYPENAME.
 *
 * EK_CONST:  Literal constant.
 *    ep->nargs = 0 or 1.
 *    ep->val = Value of constant.
 *    ep->args[0] => EK_NAME of printf format string for constant, if any.
 *
 * EK_LONGCONST:  Literal constant, type "long int".
 *    (Same as for EK_CONST.)
 *
 * EK_VAR:  Variable name.
 *    ep->nargs = 0.
 *    ep->val.i => Variable being referenced (cast to Meaning *).
 *
 * EK_ASSIGN:  Assignment operator.
 *    ep->nargs = 2.
 *    ep->args[0] => Destination l-value expression.
 *    ep->args[1] => Source expression.
 *
 * EK_POSTINC, EK_POSTDEC:  Post-increment/post-decrement operators.
 *    ep->nargs = 1.
 *
 * EK_MACARG:  Placeholder for argument in expression for FuncMacro, etc.
 *    ep->nargs = 0.
 *    ep->val.i = Code selecting which argument.
 *
 * EK_CHECKNIL:  Null-pointer check.
 *    ep->nargs = 1.
 *
 * EK_BICALL:  Call to literal function name.
 *    ep->val.s => Name of function.
 *
 * EK_STRUCTCONST:  Structured constant.
 *    ep->nargs = Number of elements in constant.
 *    (Note:  constdefn points to an EK_CONST whose val.i points to this.)
 *
 * EK_STRUCTOF:  Repeated element in structured constant.
 *    ep->nargs = 1.
 *    ep->val.i = Number of repetitions.
 *
 * EK_COMMA:  C comma operator.
 *    ep->nargs >= 2.
 *
 * EK_NAME:  Literal variable name.
 *    ep->nargs = 0.
 *    ep->val.s => Name of variable.
 *
 * EK_CTX:  Name of a context, with static links.
 *    ep->nargs = 0.
 *    ep->val.i => MK_FUNCTION or MK_MODULE to name (cast to Meaning *).
 *
 * EK_SPCALL:  Special function call.
 *    ep->nargs = 1 + number of arguments to function.
 *    ep->args[0] => Expression which is the function to call.
 *
 * EK_TYPENAME:  Type name.
 *    ep->nargs = 0.
 *    ep->val.type => Type whose name should be printed.
 *
 * EK_FUNCTION:  Normal function call.
 *    ep->val.i => MK_FUNCTION being called (cast to Meaning *).
 *
 */

enum exprkind {
    EK_EQ, EK_NE, EK_LT, EK_GT, EK_LE, EK_GE,
    EK_PLUS, EK_NEG, EK_TIMES, EK_DIVIDE,
    EK_DIV, EK_MOD,
    EK_OR, EK_AND, EK_NOT,
    EK_BAND, EK_BOR, EK_BXOR, EK_BNOT, EK_LSH, EK_RSH,
    EK_HAT, EK_INDEX, EK_CAST, EK_DOT, EK_COND,
    EK_ADDR, EK_SIZEOF, EK_ACTCAST,
    EK_CONST, EK_VAR, EK_FUNCTION,
    EK_ASSIGN, EK_POSTINC, EK_POSTDEC, EK_CHECKNIL,
    EK_MACARG, EK_BICALL, EK_STRUCTCONST, EK_STRUCTOF,
    EK_COMMA, EK_LONGCONST, EK_NAME, EK_CTX, EK_SPCALL,
    EK_LITCAST, EK_TYPENAME,
    EK_LAST
} ;

#ifdef DEFDUMPS
char *exprkindnames[(int)EK_LAST] = {
    "EK_EQ", "EK_NE", "EK_LT", "EK_GT", "EK_LE", "EK_GE",
    "EK_PLUS", "EK_NEG", "EK_TIMES", "EK_DIVIDE",
    "EK_DIV", "EK_MOD",
    "EK_OR", "EK_AND", "EK_NOT",
    "EK_BAND", "EK_BOR", "EK_BXOR", "EK_BNOT", "EK_LSH", "EK_RSH",
    "EK_HAT", "EK_INDEX", "EK_CAST", "EK_DOT", "EK_COND",
    "EK_ADDR", "EK_SIZEOF", "EK_ACTCAST",
    "EK_CONST", "EK_VAR", "EK_FUNCTION",
    "EK_ASSIGN", "EK_POSTINC", "EK_POSTDEC", "EK_CHECKNIL",
    "EK_MACARG", "EK_BICALL", "EK_STRUCTCONST", "EK_STRUCTOF",
    "EK_COMMA", "EK_LONGCONST", "EK_NAME", "EK_CTX", "EK_SPCALL",
    "EK_LITCAST", "EK_TYPENAME"
} ;
#endif /*DEFDUMPS*/

typedef struct S_expr {
    enum exprkind kind;
    short nargs;
    Value val;
    struct S_expr *args[1];    /* (Actually, variable-sized) */
} Expr;



/* "Stmt" notes.
 *
 * Statements form linked lists along the "next" pointers.
 * All other pointers are NULL and unused unless shown below.
 *
 * SK_ASSIGN:  Assignment or function call (C expression statement).
 *    sp->exp1 => Expression to be evaluated.
 *
 * SK_RETURN:  C "return" statement.
 *    sp->exp1 => Value to return, else NULL.
 *
 * SK_CASE:  C "switch" statement.
 *    sp->exp1 => Switch selector expression.
 *    sp->stm1 => List of SK_CASELABEL statements, followed by list of
 *		  statements that make up the "default:" clause.
 *
 * SK_CASELABEL:  C "case" label.
 *    sp->exp1 => Case value.
 *    sp->stm1 => List of SK_CASELABELs labelling the same clause, followed
 *                by list of statements in that clause.
 *
 * SK_CASECHECK:  Case-value-range-error, occurs in "default:" clause.
 *
 * SK_IF:  C "if" statement.
 *    sp->exp1 => Conditional expression.
 *    sp->exp2 => Constant expression, "1" if this "if" should be else-if'd
 *		  on to parent "if".  NULL => follow ElseIf parameter.
 *    sp->stm1 => "Then" clause.
 *    sp->stm2 => "Else" clause.
 *
 * SK_FOR:  C "for" statement.
 *    sp->exp1 => Initialization expression (may be NULL).
 *    sp->exp2 => Conditional expression (may be NULL).
 *    sp->exp3 => Iteration expression (may be NULL).
 *    sp->stm1 => Loop body.
 *
 * SK_REPEAT:  C "do-while" statement.
 *    sp->exp1 => Conditional expression (True = continue loop).
 *    sp->stm1 => Loop body.
 *
 * SK_WHILE:  C "while" statement.
 *    sp->exp1 => Conditional expression.
 *    sp->stm1 => Loop body.
 *
 * SK_BREAK:  C "break" statement.
 *
 * SK_CONTINUE:  C "continue" statement.
 *
 * SK_TRY:  HP Pascal TRY-RECOVER statement.
 *    sp->exp1->val.i = Global serial number of the TRY statement.
 *    sp->exp2 = Non-NULL if must generate a label for RECOVER block.
 *    sp->stm1 => TRY block.
 *    sp->stm2 => RECOVER block.
 *
 * SK_GOTO:  C "goto" statement.
 *    sp->exp1 => EK_NAME for the label number or name.
 *
 * SK_LABEL:  C statement label.
 *    sp->exp1 => EK_NAME for the label number of name.
 *
 * SK_HEADER:  Function/module header.
 *    sp->exp1 => EK_VAR pointing to MK_FUNCTION or MK_MODULE.
 *    (This always comes first in a context's statement list.)
 *
 * SK_BODY:  Body of function/module.
 *    sp->stm1 => SK_HEADER that begins the body.
 *    (This exists only during fixblock.)
 *
 */

enum stmtkind {
    SK_ASSIGN, SK_RETURN,
    SK_CASE, SK_CASELABEL, SK_IF,
    SK_FOR, SK_REPEAT, SK_WHILE, SK_BREAK, SK_CONTINUE,
    SK_TRY, SK_GOTO, SK_LABEL,
    SK_HEADER, SK_CASECHECK, SK_BODY,
    SK_LAST
} ;

#ifdef DEFDUMPS
char *stmtkindnames[(int)SK_LAST] = {
    "SK_ASSIGN", "SK_RETURN",
    "SK_CASE", "SK_CASELABEL", "SK_IF",
    "SK_FOR", "SK_REPEAT", "SK_WHILE", "SK_BREAK", "SK_CONTINUE",
    "SK_TRY", "SK_GOTO", "SK_LABEL",
    "SK_HEADER", "SK_CASECHECK", "SK_BODY"
} ;
#endif /*DEFDUMPS*/

typedef struct S_stmt {
    enum stmtkind kind;
    struct S_stmt *next, *stm1, *stm2;
    struct S_expr *exp1, *exp2, *exp3;
    long serial;
} Stmt;



/* Flags for out_declarator(): */

#define ODECL_CHARSTAR      0x1
#define ODECL_FREEARRAY     0x2
#define ODECL_FUNCTION      0x4
#define ODECL_HEADER        0x8
#define ODECL_FORWARD       0x10
#define ODECL_DECL	    0x20
#define ODECL_NOPRES	    0x40


/* Flags for fixexpr(): */

#define ENV_EXPR    0       /* return value needed */
#define ENV_STMT    1       /* return value ignored */
#define ENV_BOOL    2       /* boolean return value needed */


/* Flags for defmacro(): */
#define MAC_VAR     0       /* VarMacro */
#define MAC_CONST   1       /* ConstMacro */
#define MAC_FIELD   2       /* FieldMacro */
#define MAC_FUNC    3       /* FuncMacro */

#define FMACRECname  "<rec>"


/* Kinds of comment lines: */
#define CMT_SHIFT   24
#define CMT_MASK    ((1L<<CMT_SHIFT)-1)
#define CMT_KMASK   ((1<<(32-CMT_SHIFT))-1)
#define CMT_DONE    0       /* comment that has already been printed */
#define CMT_PRE     1       /* comment line preceding subject */
#define CMT_POST    2       /* comment line following subject */
#define CMT_TRAIL   4       /* comment at end of line of code */
#define CMT_ONBEGIN 6       /* comment on "begin" of procedure */
#define CMT_ONEND   7       /* comment on "end" of procedure */
#define CMT_ONELSE  8       /* comment on "else" keyword */
#define CMT_NOT     256     /* negation of above, for searches */

#ifdef define_globals
char *CMT_NAMES[] = { "DONE", "PRE", "POST", "3", "TRAIL", "5",
                      "BEGIN", "END", "ELSE" };
#else
extern char *CMT_NAMES[];
#endif

#define getcommentkind(cmt)  (((cmt)->value >> CMT_SHIFT) & CMT_KMASK)


/* Kinds of operator line-breaking: */
#define BRK_LEFT     0x1
#define BRK_RIGHT    0x2
#define BRK_LPREF    0x4
#define BRK_RPREF    0x8
#define BRK_ALLNONE  0x10
#define BRK_HANG     0x20




/* Translation parameters: */

#ifdef define_parameters
# define extern
#endif /* define_parameters */

extern enum {
    UNIX_ANY, UNIX_BSD, UNIX_SYSV
} which_unix;

extern enum {
    LANG_HP, LANG_UCSD, LANG_TURBO, LANG_OREGON, LANG_VAX,
    LANG_MODULA, LANG_MPW, LANG_BERK
} which_lang;

extern short debug, tokentrace, quietmode, cmtdebug, copysource;
extern int nobanner, showprogress, maxerrors;
extern short hpux_lang, integer16, doublereals, pascalenumsize;
extern short needsignedbyte, unsignedchar, importall;
extern short nestedcomments, pascalsignif, pascalcasesens;
extern short dollar_idents, ignorenonalpha, modula2;
extern short ansiC, cplus, signedchars, signedfield, signedshift;
extern short hassignedchar, voidstar, symcase, ucconsts, csignif;
extern short copystructs, usevextern, implementationmodules;
extern short useAnyptrMacros, usePPMacros;
extern short sprintf_value;
extern char codefnfmt[40], modulefnfmt[40], logfnfmt[40];
extern char headerfnfmt[40], headerfnfmt2[40], includefnfmt[40];
extern char selfincludefmt[40];
extern char constformat[40], moduleformat[40], functionformat[40];
extern char varformat[40], fieldformat[40], typeformat[40];
extern char enumformat[40], symbolformat[40];
extern char p2c_h_name[40], exportsymbol[40], export_symbol[40];
extern char externalias[40];
extern char memcpyname[40], sprintfname[40];
extern char roundname[40], divname[40], modname[40], remname[40];
extern char strposname[40], strcicmpname[40];
extern char strsubname[40], strdeletename[40], strinsertname[40];
extern char strmovename[40], strpadname[40];
extern char strltrimname[40], strrtrimname[40], strrptname[40];
extern char absname[40], oddname[40], evenname[40], swapname[40];
extern char mallocname[40], freename[40], freervaluename[40];
extern char randrealname[40], randintname[40], randomizename[40];
extern char skipspacename[40], readlnname[40], freopenname[40];
extern char eofname[40], eolnname[40], fileposname[40], maxposname[40];
extern char setunionname[40], setintname[40], setdiffname[40];
extern char setinname[40], setaddname[40], setaddrangename[40];
extern char setremname[40];
extern char setequalname[40], subsetname[40], setxorname[40];
extern char setcopyname[40], setexpandname[40], setpackname[40];
extern char getbitsname[40], clrbitsname[40], putbitsname[40];
extern char declbufname[40], declbufncname[40];
extern char resetbufname[40], setupbufname[40];
extern char getfbufname[40], chargetfbufname[40], arraygetfbufname[40];
extern char putfbufname[40], charputfbufname[40], arrayputfbufname[40];
extern char getname[40], chargetname[40], arraygetname[40];
extern char putname[40], charputname[40], arrayputname[40];
extern char eofbufname[40], fileposbufname[40];
extern char storebitsname[40], signextname[40];
extern char filenotfoundname[40], filenotopenname[40];
extern char filewriteerrorname[40], badinputformatname[40], endoffilename[40];
extern short strcpyleft;
extern char language[40], target[40];
extern int sizeof_char, sizeof_short, sizeof_integer, sizeof_pointer, 
           sizeof_double, sizeof_float, sizeof_enum, sizeof_int, sizeof_long;
extern short size_t_long;
extern int setbits, defaultsetsize, seek_base, integerwidth, realwidth;
extern short quoteincludes, expandincludes, collectnest;
extern int phystabsize, intabsize, linewidth, maxlinewidth;
extern int majorspace, minorspace, functionspace, minfuncspace;
extern int casespacing, caselimit;
extern int returnlimit, breaklimit, continuelimit;
extern short nullstmtline, shortcircuit, shortopt, usecommas, elseif;
extern short usereturns, usebreaks, infloopstyle, reusefieldnames;
extern short bracesalways, braceline, bracecombine, braceelse, braceelseline;
extern short newlinefunctions;
extern short eatcomments, spitcomments, spitorphancomments;
extern short commentafter, blankafter;
extern int tabsize, blockindent, bodyindent, argindent;
extern int switchindent, caseindent, labelindent;
extern int openbraceindent, closebraceindent;
extern int funcopenindent, funccloseindent;
extern int structindent, structinitindent, extrainitindent;
extern int constindent, commentindent, bracecommentindent, commentoverindent;
extern int declcommentindent;
extern int minspacing, minspacingthresh;
extern int extraindent, bumpindent;
extern double overwidepenalty, overwideextrapenalty;
extern double commabreakpenalty, commabreakextrapenalty;
extern double assignbreakpenalty, assignbreakextrapenalty;
extern double specialargbreakpenalty;
extern double opbreakpenalty, opbreakextrapenalty, exhyphenpenalty;
extern double logbreakpenalty, logbreakextrapenalty;
extern double relbreakpenalty, relbreakextrapenalty;
extern double morebreakpenalty, morebreakextrapenalty;
extern double parenbreakpenalty, parenbreakextrapenalty;
extern double qmarkbreakpenalty, qmarkbreakextrapenalty;
extern double wrongsidepenalty, earlybreakpenalty, extraindentpenalty;
extern double bumpindentpenalty, nobumpindentpenalty;
extern double indentamountpenalty, sameindentpenalty;
extern double showbadlimit;
extern long maxalts;
extern short breakbeforearith, breakbeforerel, breakbeforelog;
extern short breakbeforedot, breakbeforeassign;
extern short for_allornone;
extern short extraparens, breakparens, returnparens;
extern short variablearrays, initpacstrings, stararrays;
extern short spaceexprs, spacefuncs, spacecommas, implicitzero, starindex;
extern int casetabs;
extern short starfunctions, mixfields, alloczeronil, postincrement;
extern short mixvars, mixtypes, mixinits, nullcharconst, castnull, addindex;
extern short highcharints, highcharbits, hasstaticlinks;
extern short mainlocals, storefilenames, addrstdfiles, readwriteopen;
extern short charfiletext, messagestderr, literalfilesflag, structfilesflag;
extern short printfonly, mixwritelns, usegets, newlinespace, binarymode;
extern char openmode[40], filenamefilter[40];
extern short atan2flag, div_po2, mod_po2, assumebits, assumesigns;
extern short fullstrwrite, fullstrread, whilefgets, buildreads, buildwrites;
extern short foldconsts, foldstrconsts, charconsts, useconsts, useundef;
extern short elimdeadcode, offsetforloops, forevalorder;
extern short smallsetconst, bigsetconst, lelerange, unsignedtrick;
extern short useisalpha, useisspace, usestrncmp;
extern short casecheck, arraycheck, rangecheck, nilcheck, malloccheck;
extern short checkfileopen, checkfileisopen, checkfilewrite;
extern short checkreadformat, checkfileeof, checkstdineof, checkfileseek;
extern short squeezesubr, useenum, enumbyte, packing, packsigned, keepnulls;
extern short compenums, formatstrings, alwayscopyvalues;
extern short use_static, var_static, void_args, prototypes, fullprototyping;
extern short procptrprototypes, promote_enums;
extern short preservetypes, preservepointers, preservestrings;
extern short castargs, castlongargs, promoteargs, fixpromotedargs;
extern short varstrings, varfiles, copystructfuncs;
extern long skipindices;
extern short stringleaders;
extern int stringceiling, stringdefault, stringtrunclimit, longstringsize;
extern short warnnames, warnmacros;
extern Strlist *importfrom, *importdirs, *includedirs, *includefrom;
extern Strlist *librfiles, *bufferedfiles, *unbufferedfiles;
extern Strlist *externwords, *cexternwords;
extern Strlist *varmacros, *constmacros, *fieldmacros;
extern Strlist *funcmacros, *funcmacroargs, *nameoflist;
extern Strlist *specialmallocs, *specialfrees, *specialsizeofs;
extern Strlist *initialcalls, *eatnotes, *literalfiles, *structfiles;

extern char fixedcomment[40], permanentcomment[40], interfacecomment[40];
extern char embedcomment[40],  skipcomment[40], noskipcomment[40];
extern char signedcomment[40], unsignedcomment[40];

extern char name_RETV[40], name_STRMAX[40], name_LINK[40];
extern char name_COPYPAR[40], name_TEMP[40], name_DUMMY[40];
extern char name_LOC[40], name_VARS[40], name_STRUCT[40];
extern char name_FAKESTRUCT[40], name_AHIGH[40], name_ALOW[40];
extern char name_UNION[40], name_VARIANT[40], name_LABEL[40], name_LABVAR[40];
extern char name_WITH[40], name_FOR[40], name_ENUM[40];
extern char name_PTR[40], name_STRING[40], name_SET[40];
extern char name_PROCEDURE[40], name_MAIN[40], name_UNITINIT[40];
extern char name_HSYMBOL[40], name_GSYMBOL[40];
extern char name_SETBITS[40], name_UCHAR[40], name_SCHAR[40];
extern char name_BOOLEAN[40], name_TRUE[40], name_FALSE[40], name_NULL[40];
extern char name_ESCAPECODE[40], name_IORESULT[40];
extern char name_ARGC[40], name_ARGV[40];
extern char name_ESCAPE[40], name_ESCIO[40], name_CHKIO[40], name_SETIO[40];
extern char name_OUTMEM[40], name_CASECHECK[40], name_NILCHECK[40];
extern char name_FNSIZE[40], name_FNVAR[40];
extern char alternatename1[40], alternatename2[40], alternatename[40];


#ifndef define_parameters
extern
#endif
struct rcstruct {
    char kind;
    char chgmode;
    char *name;
    anyptr ptr;
    long def;
} rctable[]
#ifdef define_parameters
   = {
    'S', 'R', "DEBUG",           (anyptr) &debug,             0,
    'I', 'R', "SHOWPROGRESS",    (anyptr) &showprogress,      0,
    'S', 'V', "TOKENTRACE",      (anyptr) &tokentrace,        0,
    'S', 'V', "QUIET",           (anyptr) &quietmode,         0,
    'S', 'V', "COPYSOURCE",      (anyptr) &copysource,        0,
    'I', 'R', "MAXERRORS",	 (anyptr) &maxerrors,	      0,
    'X', ' ', "INCLUDE",         (anyptr) NULL,               2,

/* INPUT LANGUAGE */
    'U', 'T', "LANGUAGE",        (anyptr)  language,         40,
    'S', 'V', "MODULA2",         (anyptr) &modula2,          -1,
    'S', 'T', "INTEGER16",       (anyptr) &integer16,        -1,
    'S', 'T', "DOUBLEREALS",     (anyptr) &doublereals,      -1,
    'S', 'V', "UNSIGNEDCHAR",    (anyptr) &unsignedchar,     -1,
    'S', 'V', "NEEDSIGNEDBYTE",  (anyptr) &needsignedbyte,    0,
    'S', 'V', "PASCALENUMSIZE",  (anyptr) &pascalenumsize,   -1,
    'S', 'V', "NESTEDCOMMENTS",  (anyptr) &nestedcomments,   -1,
    'S', 'V', "IMPORTALL",       (anyptr) &importall,        -1,
    'S', 'V', "IMPLMODULES",     (anyptr) &implementationmodules, -1,
    'A', 'V', "EXTERNWORDS",	 (anyptr) &externwords,	      0,
    'A', 'V', "CEXTERNWORDS",	 (anyptr) &cexternwords,      0,
    'S', 'V', "PASCALSIGNIF",    (anyptr) &pascalsignif,     -1,
    'S', 'V', "PASCALCASESENS",  (anyptr) &pascalcasesens,   -1,
    'S', 'V', "DOLLARIDENTS",    (anyptr) &dollar_idents,    -1,
    'S', 'V', "IGNORENONALPHA",  (anyptr) &ignorenonalpha,   -1,
    'I', 'V', "SEEKBASE",        (anyptr) &seek_base,        -1,
    'I', 'R', "INPUTTABSIZE",    (anyptr) &intabsize,         8,

/* TARGET LANGUAGE */
    'S', 'T', "ANSIC",           (anyptr) &ansiC,            -1,
    'S', 'T', "C++",             (anyptr) &cplus,            -1,
    'S', 'T', "VOID*",           (anyptr) &voidstar,         -1,
    'S', 'T', "HASSIGNEDCHAR",   (anyptr) &hassignedchar,    -1,
    'S', 'V', "CASTNULL",        (anyptr) &castnull,         -1,
    'S', 'V', "COPYSTRUCTS",     (anyptr) &copystructs,      -1,
    'S', 'V', "VARIABLEARRAYS",  (anyptr) &variablearrays,   -1,
    'S', 'V', "INITPACSTRINGS",  (anyptr) &initpacstrings,   -1,
    'S', 'V', "REUSEFIELDNAMES", (anyptr) &reusefieldnames,   1,
    'S', 'V', "USEVEXTERN",      (anyptr) &usevextern,        1,
    'S', 'V', "CSIGNIF",         (anyptr) &csignif,          -1,
    'S', 'V', "USEANYPTRMACROS", (anyptr) &useAnyptrMacros,  -1,
    'S', 'V', "USEPPMACROS",     (anyptr) &usePPMacros,      -1,

/* TARGET MACHINE */
    'U', 'T', "TARGET",          (anyptr)  target,           40,
    'S', 'T', "SIGNEDCHAR",      (anyptr) &signedchars,      -1,
    'S', 'T', "SIGNEDFIELD",     (anyptr) &signedfield,      -1,
    'S', 'T', "SIGNEDSHIFT",     (anyptr) &signedshift,      -1,
    'I', 'T', "CHARSIZE",        (anyptr) &sizeof_char,       0,
    'I', 'T', "SHORTSIZE",       (anyptr) &sizeof_short,      0,
    'I', 'T', "INTSIZE",         (anyptr) &sizeof_int,        0,
    'I', 'T', "LONGSIZE",        (anyptr) &sizeof_long,       0,
    'I', 'T', "PTRSIZE",         (anyptr) &sizeof_pointer,    0,
    'I', 'T', "DOUBLESIZE",      (anyptr) &sizeof_double,     0,
    'I', 'T', "FLOATSIZE",       (anyptr) &sizeof_float,      0,
    'I', 'T', "ENUMSIZE",        (anyptr) &sizeof_enum,       0,
    'S', 'T', "SIZE_T_LONG",     (anyptr) &size_t_long,      -1,

/* BRACES */
    'S', 'V', "NULLSTMTLINE",    (anyptr) &nullstmtline,      0,
    'S', 'V', "BRACESALWAYS",    (anyptr) &bracesalways,     -1,
    'S', 'V', "BRACELINE",       (anyptr) &braceline,        -1,
    'S', 'V', "BRACECOMBINE",    (anyptr) &bracecombine,      0,
    'S', 'V', "BRACEELSE",       (anyptr) &braceelse,         0,
    'S', 'V', "BRACEELSELINE",   (anyptr) &braceelseline,     0,
    'S', 'V', "ELSEIF",          (anyptr) &elseif,           -1,
    'S', 'V', "NEWLINEFUNCS",    (anyptr) &newlinefunctions,  0,

/* INDENTATION */
    'I', 'R', "PHYSTABSIZE",     (anyptr) &phystabsize,       8,
    'D', 'R', "INDENT",          (anyptr) &tabsize,           2,
    'D', 'R', "BLOCKINDENT",     (anyptr) &blockindent,       0,
    'D', 'R', "BODYINDENT",      (anyptr) &bodyindent,        0,
    'D', 'R', "FUNCARGINDENT",   (anyptr) &argindent,      1000,
    'D', 'R', "OPENBRACEINDENT", (anyptr) &openbraceindent,   0,
    'D', 'R', "CLOSEBRACEINDENT",(anyptr) &closebraceindent,  0,
    'D', 'R', "FUNCOPENINDENT",  (anyptr) &funcopenindent,    0,
    'D', 'R', "FUNCCLOSEINDENT", (anyptr) &funccloseindent,   0,
    'D', 'R', "SWITCHINDENT",    (anyptr) &switchindent,      0,
    'D', 'R', "CASEINDENT",      (anyptr) &caseindent,       -2,
    'D', 'R', "LABELINDENT",     (anyptr) &labelindent,    1000,
    'D', 'R', "STRUCTINDENT",    (anyptr) &structindent,      0,
    'D', 'R', "STRUCTINITINDENT",(anyptr) &structinitindent,  0,
    'D', 'R', "EXTRAINITINDENT", (anyptr) &extrainitindent,   2,
    'I', 'R', "EXTRAINDENT",     (anyptr) &extraindent,       2,
    'I', 'R', "BUMPINDENT",      (anyptr) &bumpindent,        1,
    'D', 'R', "CONSTINDENT",     (anyptr) &constindent,    1024,
    'D', 'R', "COMMENTINDENT",   (anyptr) &commentindent,     3,
    'D', 'R', "BRACECOMMENTINDENT",(anyptr)&bracecommentindent, 2,
    'D', 'R', "DECLCOMMENTINDENT",(anyptr)&declcommentindent, -999,
    'D', 'R', "COMMENTOVERINDENT",(anyptr)&commentoverindent, 4,  /*1000*/
    'I', 'R', "MINSPACING",      (anyptr) &minspacing,        2,
    'I', 'R', "MINSPACINGTHRESH",(anyptr) &minspacingthresh, -1,

/* LINE BREAKING */
    'I', 'R', "LINEWIDTH",       (anyptr) &linewidth,        78,
    'I', 'R', "MAXLINEWIDTH",    (anyptr) &maxlinewidth,     90,
    'R', 'V', "OVERWIDEPENALTY",       (anyptr) &overwidepenalty,         2500,
    'R', 'V', "OVERWIDEEXTRAPENALTY",  (anyptr) &overwideextrapenalty,     100,
    'R', 'V', "COMMABREAKPENALTY",     (anyptr) &commabreakpenalty,       1000,
    'R', 'V', "COMMABREAKEXTRAPENALTY",(anyptr) &commabreakextrapenalty,   500,
    'R', 'V', "ASSIGNBREAKPENALTY",    (anyptr) &assignbreakpenalty,      5000,
    'R', 'V', "ASSIGNBREAKEXTRAPENALTY",(anyptr)&assignbreakextrapenalty, 3000,
    'R', 'V', "SPECIALARGBREAKPENALTY",(anyptr) &specialargbreakpenalty,   500,
    'R', 'V', "OPBREAKPENALTY",        (anyptr) &opbreakpenalty,          2500,
    'R', 'V', "OPBREAKEXTRAPENALTY",   (anyptr) &opbreakextrapenalty,     2000,
    'R', 'V', "LOGBREAKPENALTY",       (anyptr) &logbreakpenalty,          500,
    'R', 'V', "LOGBREAKEXTRAPENALTY",  (anyptr) &logbreakextrapenalty,     100,
    'R', 'V', "RELBREAKPENALTY",       (anyptr) &relbreakpenalty,         2000,
    'R', 'V', "RELBREAKEXTRAPENALTY",  (anyptr) &relbreakextrapenalty,    1000,
    'R', 'V', "EXHYPHENPENALTY",       (anyptr) &exhyphenpenalty,         1000,
    'R', 'V', "MOREBREAKPENALTY",      (anyptr) &morebreakpenalty,        -500,
    'R', 'V', "MOREBREAKEXTRAPENALTY", (anyptr) &morebreakextrapenalty,   -300,
    'R', 'V', "QMARKBREAKPENALTY",     (anyptr) &qmarkbreakpenalty,       5000,
    'R', 'V', "QMARKBREAKEXTRAPENALTY",(anyptr) &qmarkbreakextrapenalty,  3000,
    'R', 'V', "PARENBREAKPENALTY",     (anyptr) &parenbreakpenalty,       2500,
    'R', 'V', "PARENBREAKEXTRAPENALTY",(anyptr) &parenbreakextrapenalty,  1000,
    'R', 'V', "WRONGSIDEPENALTY",      (anyptr) &wrongsidepenalty,        1000,
    'R', 'V', "EARLYBREAKPENALTY",     (anyptr) &earlybreakpenalty,        100,
    'R', 'V', "EXTRAINDENTPENALTY",    (anyptr) &extraindentpenalty,      3000,
    'R', 'V', "BUMPINDENTPENALTY",     (anyptr) &bumpindentpenalty,       1000,
    'R', 'V', "NOBUMPINDENTPENALTY",   (anyptr) &nobumpindentpenalty,     2500,
    'R', 'V', "INDENTAMOUNTPENALTY",   (anyptr) &indentamountpenalty,       50,
    'R', 'V', "SAMEINDENTPENALTY",     (anyptr) &sameindentpenalty,        500,
    'R', 'V', "SHOWBADLIMIT",          (anyptr) &showbadlimit,            -120,
    'L', 'R', "MAXLINEBREAKTRIES", (anyptr) &maxalts,      5000,
    'G', 'V', "ALLORNONEBREAK",  (anyptr)  NULL,             FALLBREAK,
    'G', 'V', "ONESPECIALARG",   (anyptr)  NULL,             FSPCARG1,
    'G', 'V', "TWOSPECIALARGS",  (anyptr)  NULL,             FSPCARG2,
    'G', 'V', "THREESPECIALARGS",(anyptr)  NULL,             FSPCARG3,
    'B', 'V', "BREAKARITH",      (anyptr) &breakbeforearith,  BRK_RIGHT,
    'B', 'V', "BREAKREL",        (anyptr) &breakbeforerel,    BRK_RIGHT,
    'B', 'V', "BREAKLOG",        (anyptr) &breakbeforelog,    BRK_RIGHT,
    'B', 'V', "BREAKDOT",        (anyptr) &breakbeforedot,    BRK_RIGHT,
    'B', 'V', "BREAKASSIGN",     (anyptr) &breakbeforeassign, BRK_RIGHT,
    'S', 'V', "FOR_ALLORNONE",   (anyptr) &for_allornone,     1,

/* COMMENTS AND BLANK LINES */
    'S', 'V', "NOBANNER",        (anyptr) &nobanner,	      0,
    'S', 'V', "EATCOMMENTS",     (anyptr) &eatcomments,       0,
    'S', 'V', "SPITCOMMENTS",    (anyptr) &spitcomments,      0,
    'S', 'V', "SPITORPHANCOMMENTS",(anyptr)&spitorphancomments, 0,
    'S', 'V', "COMMENTAFTER",    (anyptr) &commentafter,     -1,
    'S', 'V', "BLANKAFTER",      (anyptr) &blankafter,        1,
    'A', 'V', "EATNOTES",        (anyptr) &eatnotes,          0,

/* SPECIAL COMMENTS */
    'C', 'V', "FIXEDCOMMENT",    (anyptr)  fixedcomment,     40,
    'C', 'V', "PERMANENTCOMMENT",(anyptr)  permanentcomment, 40,
    'C', 'V', "INTERFACECOMMENT",(anyptr)  interfacecomment, 40,
    'C', 'V', "EMBEDCOMMENT",    (anyptr)  embedcomment,     40,
    'C', 'V', "SKIPCOMMENT",     (anyptr)  skipcomment,      40,
    'C', 'V', "NOSKIPCOMMENT",   (anyptr)  noskipcomment,    40,
    'C', 'V', "SIGNEDCOMMENT",   (anyptr)  signedcomment,    40,
    'C', 'V', "UNSIGNEDCOMMENT", (anyptr)  unsignedcomment,  40,

/* STYLISTIC OPTIONS */
    'I', 'V', "MAJORSPACING",    (anyptr) &majorspace,        2,
    'I', 'V', "MINORSPACING",    (anyptr) &minorspace,        1,
    'I', 'V', "FUNCSPACING",     (anyptr) &functionspace,     2,
    'I', 'V', "MINFUNCSPACING",  (anyptr) &minfuncspace,      1,
    'S', 'V', "EXTRAPARENS",     (anyptr) &extraparens,      -1,
    'S', 'V', "BREAKADDPARENS",  (anyptr) &breakparens,      -1,
    'S', 'V', "RETURNPARENS",    (anyptr) &returnparens,     -1,
    'S', 'V', "SPACEEXPRS",      (anyptr) &spaceexprs,       -1,
    'S', 'V', "SPACEFUNCS",	 (anyptr) &spacefuncs,	      0,
    'S', 'V', "SPACECOMMAS",	 (anyptr) &spacecommas,	      1,
    'S', 'V', "IMPLICITZERO",    (anyptr) &implicitzero,     -1,
    'S', 'V', "STARINDEX",       (anyptr) &starindex,        -1,
    'S', 'V', "ADDINDEX",        (anyptr) &addindex,         -1,
    'S', 'V', "STARARRAYS",      (anyptr) &stararrays,        1,
    'S', 'V', "STARFUNCTIONS",   (anyptr) &starfunctions,    -1,
    'S', 'V', "POSTINCREMENT",   (anyptr) &postincrement,     1,
    'S', 'V', "MIXVARS",         (anyptr) &mixvars,          -1,
    'S', 'V', "MIXTYPES",        (anyptr) &mixtypes,         -1,
    'S', 'V', "MIXFIELDS",       (anyptr) &mixfields,        -1,
    'S', 'V', "MIXINITS",        (anyptr) &mixinits,         -1,
    'S', 'V', "MAINLOCALS",      (anyptr) &mainlocals,        1,
    'S', 'V', "NULLCHAR",        (anyptr) &nullcharconst,     1,
    'S', 'V', "HIGHCHARINT",     (anyptr) &highcharints,      1,
    'I', 'V', "CASESPACING",     (anyptr) &casespacing,       1,
    'D', 'V', "CASETABS",        (anyptr) &casetabs,       1000,
    'I', 'V', "CASELIMIT",       (anyptr) &caselimit,         9,
    'S', 'V', "USECOMMAS",       (anyptr) &usecommas,        -1,
    'S', 'V', "USERETURNS",      (anyptr) &usereturns,        1,
    'I', 'V', "RETURNLIMIT",     (anyptr) &returnlimit,       3,
    'S', 'V', "USEBREAKS",       (anyptr) &usebreaks,         1,
    'I', 'V', "BREAKLIMIT",      (anyptr) &breaklimit,        2,
    'I', 'V', "CONTINUELIMIT",   (anyptr) &continuelimit,     5,
    'S', 'V', "INFLOOPSTYLE",    (anyptr) &infloopstyle,      0,

/* NAMING CONVENTIONS */
    'C', 'V', "CODEFILENAME",    (anyptr)  codefnfmt,        40,
    'C', 'V', "MODULEFILENAME",  (anyptr)  modulefnfmt,      40,
    'C', 'V', "HEADERFILENAME",  (anyptr)  headerfnfmt,      40,
    'C', 'V', "HEADERFILENAME2", (anyptr)  headerfnfmt2,     40,
    'C', 'V', "SELFINCLUDENAME", (anyptr)  selfincludefmt,   40,
    'C', 'V', "LOGFILENAME",     (anyptr)  logfnfmt,         40,
    'C', 'V', "INCLUDEFILENAME", (anyptr)  includefnfmt,     40,
    'S', 'V', "SYMCASE",         (anyptr) &symcase,          -1,
    'C', 'V', "SYMBOLFORMAT",    (anyptr)  symbolformat,     40,
    'C', 'V', "CONSTFORMAT",     (anyptr)  constformat,      40,
    'C', 'V', "MODULEFORMAT",    (anyptr)  moduleformat,     40,
    'C', 'V', "FUNCTIONFORMAT",  (anyptr)  functionformat,   40,
    'C', 'V', "VARFORMAT",       (anyptr)  varformat,        40,
    'C', 'V', "FIELDFORMAT",     (anyptr)  fieldformat,      40,
    'C', 'V', "TYPEFORMAT",      (anyptr)  typeformat,       40,
    'C', 'V', "ENUMFORMAT",      (anyptr)  enumformat,       40,
    'C', 'V', "RETURNVALUENAME", (anyptr)  name_RETV,        40,
    'C', 'V', "UNITINITNAME",    (anyptr)  name_UNITINIT,    40,
    'C', 'V', "HSYMBOLNAME",     (anyptr)  name_HSYMBOL,     40,
    'C', 'V', "GSYMBOLNAME",     (anyptr)  name_GSYMBOL,     40,
    'C', 'V', "STRINGMAXNAME",   (anyptr)  name_STRMAX,      40,
    'C', 'V', "ARRAYMINNAME",    (anyptr)  name_ALOW,        40,
    'C', 'V', "ARRAYMAXNAME",    (anyptr)  name_AHIGH,       40,
    'C', 'V', "COPYPARNAME",     (anyptr)  name_COPYPAR,     40,
    'C', 'V', "STATICLINKNAME",  (anyptr)  name_LINK,        40,
    'C', 'V', "LOCALVARSSTRUCT", (anyptr)  name_LOC,         40,
    'C', 'V', "LOCALVARSNAME",   (anyptr)  name_VARS,        40,
    'C', 'V', "FWDSTRUCTNAME",   (anyptr)  name_STRUCT,      40,
    'C', 'V', "ENUMLISTNAME",    (anyptr)  name_ENUM,        40,
    'C', 'V', "UNIONNAME",       (anyptr)  name_UNION,       40,
    'C', 'V', "UNIONPARTNAME",   (anyptr)  name_VARIANT,     40,
    'C', 'V', "FAKESTRUCTNAME",  (anyptr)  name_FAKESTRUCT,  40,
    'C', 'V', "LABELNAME",       (anyptr)  name_LABEL,       40,
    'C', 'V', "LABELVARNAME",    (anyptr)  name_LABVAR,      40,
    'C', 'V', "TEMPNAME",        (anyptr)  name_TEMP,        40,
    'C', 'V', "DUMMYNAME",       (anyptr)  name_DUMMY,       40,
    'C', 'V', "FORNAME",         (anyptr)  name_FOR,         40,
    'C', 'V', "WITHNAME",        (anyptr)  name_WITH,        40,
    'C', 'V', "PTRNAME",         (anyptr)  name_PTR,         40,
    'C', 'V', "STRINGNAME",      (anyptr)  name_STRING,      40,
    'C', 'V', "SETNAME",         (anyptr)  name_SET,         40,
    'C', 'V', "FNVARNAME",       (anyptr)  name_FNVAR,       40,
    'C', 'V', "FNSIZENAME",      (anyptr)  name_FNSIZE,      40,
    'C', 'V', "ALTERNATENAME1",  (anyptr)  alternatename1,   40,
    'C', 'V', "ALTERNATENAME2",  (anyptr)  alternatename2,   40,
    'C', 'V', "ALTERNATENAME",   (anyptr)  alternatename,    40,
    'C', 'V', "EXPORTSYMBOL",    (anyptr)  exportsymbol,     40,
    'C', 'V', "EXPORT_SYMBOL",   (anyptr)  export_symbol,    40,
    'C', 'V', "ALIAS",           (anyptr)  externalias,      40,
    'X', 'V', "SYNONYM",         (anyptr)  NULL,              3,
    'X', 'V', "NAMEOF",          (anyptr) &nameoflist,        1,
    'G', 'V', "AVOIDNAME",       (anyptr)  NULL,             AVOIDNAME,
    'G', 'V', "AVOIDGLOBALNAME", (anyptr)  NULL,             AVOIDGLOB,
    'G', 'V', "WARNNAME",        (anyptr)  NULL,             WARNNAME,
    'G', 'V', "NOSIDEEFFECTS",   (anyptr)  NULL,             NOSIDEEFF,
    'G', 'V', "STRUCTFUNCTION",  (anyptr)  NULL,             STRUCTF,
    'G', 'V', "STRLAPFUNCTION",  (anyptr)  NULL,             STRLAPF,
    'F', 'V', "LEAVEALONE",      (anyptr)  NULL,             LEAVEALONE,
    'G', 'V', "DETERMINISTIC",   (anyptr)  NULL,             DETERMF,
    'G', 'V', "NEEDSTATIC",      (anyptr)  NULL,             NEEDSTATIC,
    'S', 'V', "WARNNAMES",       (anyptr) &warnnames,         0,
    'M', 'V', "VARMACRO",        (anyptr)  NULL,             MAC_VAR,
    'M', 'V', "CONSTMACRO",      (anyptr)  NULL,             MAC_CONST,
    'M', 'V', "FIELDMACRO",      (anyptr)  NULL,             MAC_FIELD,
    'M', 'V', "FUNCMACRO",       (anyptr)  NULL,             MAC_FUNC,
    'S', 'V', "WARNMACROS",      (anyptr) &warnmacros,        0,

/* CODING OPTIONS */
    'A', 'V', "INITIALCALLS",    (anyptr) &initialcalls,      0,
    'S', 'V', "EXPANDINCLUDES",  (anyptr) &expandincludes,   -1,
    'S', 'V', "COLLECTNEST",     (anyptr) &collectnest,       1,
    'S', 'V', "SHORTCIRCUIT",    (anyptr) &shortcircuit,     -1,
    'S', 'V', "SHORTOPT",        (anyptr) &shortopt,          1,
    'S', 'V', "ELIMDEADCODE",    (anyptr) &elimdeadcode,      1,
    'S', 'V', "FOLDCONSTANTS",   (anyptr) &foldconsts,       -1,
    'S', 'V', "FOLDSTRCONSTANTS",(anyptr) &foldstrconsts,    -1,
    'S', 'V', "CHARCONSTS",	 (anyptr) &charconsts,        1,
    'S', 'V', "USECONSTS",       (anyptr) &useconsts,        -1,
    'S', 'V', "USEUNDEF",        (anyptr) &useundef,          1,
    'L', 'V', "SKIPINDICES",     (anyptr) &skipindices,       0,
    'S', 'V', "OFFSETFORLOOPS",  (anyptr) &offsetforloops,    1,
    'S', 'V', "FOREVALORDER",    (anyptr) &forevalorder,      0,
    'S', 'V', "STRINGLEADERS",   (anyptr) &stringleaders,     2,
    'S', 'V', "STOREFILENAMES",  (anyptr) &storefilenames,   -1,
    'S', 'V', "CHARFILETEXT",    (anyptr) &charfiletext,     -1,
    'S', 'V', "SQUEEZESUBR",     (anyptr) &squeezesubr,       1,
    'S', 'T', "USEENUM",         (anyptr) &useenum,          -1,
    'S', 'V', "SQUEEZEENUM",     (anyptr) &enumbyte,         -1,
    'S', 'V', "COMPENUMS",       (anyptr) &compenums,        -1,
    'S', 'V', "PRESERVETYPES",   (anyptr) &preservetypes,     1,
    'S', 'V', "PRESERVEPOINTERS",(anyptr) &preservepointers,  0,
    'S', 'V', "PRESERVESTRINGS", (anyptr) &preservestrings,  -1,
    'S', 'V', "PACKING",         (anyptr) &packing,           1,
    'S', 'V', "PACKSIGNED",      (anyptr) &packsigned,        1,
    'I', 'V', "STRINGCEILING",   (anyptr) &stringceiling,   255,
    'I', 'V', "STRINGDEFAULT",   (anyptr) &stringdefault,   255,
    'I', 'V', "STRINGTRUNCLIMIT",(anyptr) &stringtrunclimit, -1,
    'I', 'V', "LONGSTRINGSIZE",  (anyptr) &longstringsize,   -1,
    'S', 'V', "KEEPNULLS",       (anyptr) &keepnulls,         0,
    'S', 'V', "HIGHCHARBITS",    (anyptr) &highcharbits,     -1,
    'S', 'V', "ALWAYSCOPYVALUES",(anyptr) &alwayscopyvalues,  0,
    'S', 'V', "STATICFUNCTIONS", (anyptr) &use_static,        1,
    'S', 'V', "STATICVARIABLES", (anyptr) &var_static,        1,
    'S', 'V', "VOIDARGS",        (anyptr) &void_args,        -1,
    'S', 'V', "PROTOTYPES",      (anyptr) &prototypes,       -1,
    'S', 'V', "FULLPROTOTYPING", (anyptr) &fullprototyping,  -1,
    'S', 'V', "PROCPTRPROTOTYPES",(anyptr)&procptrprototypes, 1,
    'S', 'V', "CASTARGS",        (anyptr) &castargs,         -1,
    'S', 'V', "CASTLONGARGS",    (anyptr) &castlongargs,     -1,
    'S', 'V', "PROMOTEARGS",     (anyptr) &promoteargs,      -1,
    'S', 'V', "FIXPROMOTEDARGS", (anyptr) &fixpromotedargs,   1,
    'S', 'V', "PROMOTEENUMS",    (anyptr) &promote_enums,    -1,
    'S', 'V', "STATICLINKS",     (anyptr) &hasstaticlinks,   -1,
    'S', 'V', "VARSTRINGS",      (anyptr) &varstrings,        0,
    'S', 'V', "VARFILES",        (anyptr) &varfiles,          1,
    'S', 'V', "ADDRSTDFILES",    (anyptr) &addrstdfiles,      0,
    'S', 'V', "COPYSTRUCTFUNCS", (anyptr) &copystructfuncs,  -1,
    'S', 'V', "ATAN2",           (anyptr) &atan2flag,         0,
    'S', 'V', "BITWISEMOD",      (anyptr) &mod_po2,          -1,
    'S', 'V', "BITWISEDIV",      (anyptr) &div_po2,          -1,
    'S', 'V', "ASSUMEBITS",      (anyptr) &assumebits,        0,
    'S', 'V', "ASSUMESIGNS",     (anyptr) &assumesigns,       1,
    'S', 'V', "ALLOCZERONIL",    (anyptr) &alloczeronil,      0,
    'S', 'V', "PRINTFONLY",      (anyptr) &printfonly,       -1,
    'S', 'V', "MIXWRITELNS",     (anyptr) &mixwritelns,       1,
    'S', 'V', "MESSAGESTDERR",   (anyptr) &messagestderr,     1,
    'I', 'V', "INTEGERWIDTH",    (anyptr) &integerwidth,     -1,
    'I', 'V', "REALWIDTH",       (anyptr) &realwidth,        12,
    'S', 'V', "FORMATSTRINGS",   (anyptr) &formatstrings,     0,
    'S', 'V', "WHILEFGETS",      (anyptr) &whilefgets,        1,
    'S', 'V', "USEGETS",         (anyptr) &usegets,           1,
    'S', 'V', "NEWLINESPACE",    (anyptr) &newlinespace,     -1,
    'S', 'V', "BUILDREADS",      (anyptr) &buildreads,        1,
    'S', 'V', "BUILDWRITES",     (anyptr) &buildwrites,       1,
    'S', 'V', "BINARYMODE",      (anyptr) &binarymode,        1,
    'S', 'V', "READWRITEOPEN",   (anyptr) &readwriteopen,    -1,
    'C', 'V', "OPENMODE",        (anyptr)  openmode,         40,
    'S', 'V', "LITERALFILES",    (anyptr) &literalfilesflag, -1,
    'A', 'V', "LITERALFILE",     (anyptr) &literalfiles,      0,
    'S', 'V', "STRUCTFILES",     (anyptr) &structfilesflag,   0,
    'A', 'V', "STRUCTFILE",      (anyptr) &structfiles,       0,
    'C', 'V', "FILENAMEFILTER",  (anyptr)  filenamefilter,   40,
    'S', 'V', "FULLSTRWRITE",    (anyptr) &fullstrwrite,     -1,
    'S', 'V', "FULLSTRREAD",     (anyptr) &fullstrread,       1,
    'I', 'R', "SETBITS",         (anyptr) &setbits,          -1,
    'I', 'V', "DEFAULTSETSIZE",  (anyptr) &defaultsetsize,   -1,
    'S', 'V', "SMALLSETCONST",   (anyptr) &smallsetconst,    -2,
    'S', 'V', "BIGSETCONST",     (anyptr) &bigsetconst,       1,
    'S', 'V', "LELERANGE",       (anyptr) &lelerange,         0,
    'S', 'V', "UNSIGNEDTRICK",   (anyptr) &unsignedtrick,     1,
    'S', 'V', "USEISALPHA",      (anyptr) &useisalpha,        1,
    'S', 'V', "USEISSPACE",      (anyptr) &useisspace,        0,
    'S', 'V', "USESTRNCMP",	 (anyptr) &usestrncmp,	      1,

/* TARGET LIBRARY */
    'G', 'V', "WARNLIBRARY",     (anyptr)  NULL,             WARNLIBR,
    'S', 'V', "QUOTEINCLUDES",   (anyptr) &quoteincludes,     1,
    'X', 'V', "IMPORTFROM",      (anyptr) &importfrom,        1,
    'A', 'V', "IMPORTDIR",       (anyptr) &importdirs,        0,
    'A', 'V', "INCLUDEDIR",      (anyptr) &includedirs,       0,
    'X', 'V', "INCLUDEFROM",     (anyptr) &includefrom,       1,
    'A', 'V', "LIBRARYFILE",     (anyptr) &librfiles,         0,
    'C', 'V', "HEADERNAME",      (anyptr)  p2c_h_name,       40,
    'C', 'V', "PROCTYPENAME",    (anyptr)  name_PROCEDURE,   40,
    'C', 'V', "UCHARNAME",       (anyptr)  name_UCHAR,       40,
    'C', 'V', "SCHARNAME",       (anyptr)  name_SCHAR,       40,
    'C', 'V', "BOOLEANNAME",     (anyptr)  name_BOOLEAN,     40,
    'C', 'V', "TRUENAME",        (anyptr)  name_TRUE,        40,
    'C', 'V', "FALSENAME",       (anyptr)  name_FALSE,       40,
    'C', 'V', "NULLNAME",        (anyptr)  name_NULL,        40,
    'C', 'V', "ESCAPECODENAME",  (anyptr)  name_ESCAPECODE,  40,
    'C', 'V', "IORESULTNAME",    (anyptr)  name_IORESULT,    40,
    'C', 'V', "ARGCNAME",        (anyptr)  name_ARGC,        40,
    'C', 'V', "ARGVNAME",        (anyptr)  name_ARGV,        40,
    'C', 'V', "MAINNAME",        (anyptr)  name_MAIN,        40,
    'C', 'V', "ESCAPENAME",      (anyptr)  name_ESCAPE,      40,
    'C', 'V', "ESCIONAME",       (anyptr)  name_ESCIO,       40,
    'C', 'V', "CHECKIONAME",     (anyptr)  name_CHKIO,       40,
    'C', 'V', "SETIONAME",       (anyptr)  name_SETIO,       40,
    'C', 'V', "FILENOTFOUNDNAME",(anyptr)  filenotfoundname, 40,
    'C', 'V', "FILENOTOPENNAME", (anyptr)  filenotopenname,  40,
    'C', 'V', "FILEWRITEERRORNAME",(anyptr)filewriteerrorname,40,
    'C', 'V', "BADINPUTFORMATNAME",(anyptr)badinputformatname,40,
    'C', 'V', "ENDOFFILENAME",   (anyptr)  endoffilename,    40,
    'C', 'V', "OUTMEMNAME",      (anyptr)  name_OUTMEM,      40,
    'C', 'V', "CASECHECKNAME",   (anyptr)  name_CASECHECK,   40,
    'C', 'V', "NILCHECKNAME",    (anyptr)  name_NILCHECK,    40,
    'C', 'V', "SETBITSNAME",     (anyptr)  name_SETBITS,     40,
    'S', 'V', "SPRINTFVALUE",    (anyptr) &sprintf_value,    -1,
    'C', 'V', "SPRINTFNAME",     (anyptr)  sprintfname,      40,
    'C', 'V', "MEMCPYNAME",      (anyptr)  memcpyname,       40,
    'C', 'V', "ROUNDNAME",       (anyptr)  roundname,        40,
    'C', 'V', "DIVNAME",	 (anyptr)  divname,	     40,
    'C', 'V', "MODNAME",	 (anyptr)  modname,	     40,
    'C', 'V', "REMNAME",	 (anyptr)  remname,	     40,
    'C', 'V', "STRCICMPNAME",    (anyptr)  strcicmpname,     40,
    'C', 'V', "STRSUBNAME",      (anyptr)  strsubname,       40,
    'C', 'V', "STRPOSNAME",      (anyptr)  strposname,       40,
    'S', 'V', "STRCPYLEFT",      (anyptr) &strcpyleft,        1,
    'C', 'V', "STRDELETENAME",   (anyptr)  strdeletename,    40,
    'C', 'V', "STRINSERTNAME",   (anyptr)  strinsertname,    40,
    'C', 'V', "STRMOVENAME",     (anyptr)  strmovename,	     40,
    'C', 'V', "STRLTRIMNAME",    (anyptr)  strltrimname,     40,
    'C', 'V', "STRRTRIMNAME",    (anyptr)  strrtrimname,     40,
    'C', 'V', "STRRPTNAME",      (anyptr)  strrptname,       40,
    'C', 'V', "STRPADNAME",      (anyptr)  strpadname,       40,
    'C', 'V', "ABSNAME",         (anyptr)  absname,          40,
    'C', 'V', "ODDNAME",         (anyptr)  oddname,          40,
    'C', 'V', "EVENNAME",        (anyptr)  evenname,         40,
    'C', 'V', "SWAPNAME",        (anyptr)  swapname,         40,
    'C', 'V', "MALLOCNAME",      (anyptr)  mallocname,       40,
    'C', 'V', "FREENAME",        (anyptr)  freename,         40,
    'C', 'V', "FREERVALUENAME",  (anyptr)  freervaluename,   40,
    'X', 'V', "SPECIALMALLOC",   (anyptr) &specialmallocs,    1,
    'X', 'V', "SPECIALFREE",     (anyptr) &specialfrees,      1,
    'X', 'V', "SPECIALSIZEOF",   (anyptr) &specialsizeofs,    1,
    'C', 'V', "RANDREALNAME",    (anyptr)  randrealname,     40,
    'C', 'V', "RANDINTNAME",     (anyptr)  randintname,      40,
    'C', 'V', "RANDOMIZENAME",   (anyptr)  randomizename,    40,
    'C', 'V', "SKIPSPACENAME",   (anyptr)  skipspacename,    40,
    'C', 'V', "READLNNAME",      (anyptr)  readlnname,       40,
    'C', 'V', "FREOPENNAME",     (anyptr)  freopenname,      40,
    'C', 'V', "EOFNAME",         (anyptr)  eofname,          40,
    'C', 'V', "EOLNNAME",        (anyptr)  eolnname,         40,
    'C', 'V', "FILEPOSNAME",     (anyptr)  fileposname,      40,
    'C', 'V', "MAXPOSNAME",      (anyptr)  maxposname,       40,
    'C', 'V', "SETUNIONNAME",    (anyptr)  setunionname,     40,
    'C', 'V', "SETINTNAME",      (anyptr)  setintname,       40,
    'C', 'V', "SETDIFFNAME",     (anyptr)  setdiffname,      40,
    'C', 'V', "SETXORNAME",      (anyptr)  setxorname,       40,
    'C', 'V', "SETINNAME",       (anyptr)  setinname,        40,
    'C', 'V', "SETADDNAME",      (anyptr)  setaddname,       40,
    'C', 'V', "SETADDRANGENAME", (anyptr)  setaddrangename,  40,
    'C', 'V', "SETREMNAME",      (anyptr)  setremname,       40,
    'C', 'V', "SETEQUALNAME",    (anyptr)  setequalname,     40,
    'C', 'V', "SUBSETNAME",      (anyptr)  subsetname,       40,
    'C', 'V', "SETCOPYNAME",     (anyptr)  setcopyname,      40,
    'C', 'V', "SETEXPANDNAME",   (anyptr)  setexpandname,    40,
    'C', 'V', "SETPACKNAME",     (anyptr)  setpackname,      40,
    'C', 'V', "SIGNEXTENDNAME",  (anyptr)  signextname,      40,
    'C', 'V', "GETBITSNAME",     (anyptr)  getbitsname,      40,
    'C', 'V', "CLRBITSNAME",     (anyptr)  clrbitsname,      40,
    'C', 'V', "PUTBITSNAME",     (anyptr)  putbitsname,      40,
    'C', 'V', "STOREBITSNAME",   (anyptr)  storebitsname,    40,
    'C', 'V', "DECLBUFNAME",	 (anyptr)  declbufname,	     40,
    'C', 'V', "DECLBUFNCNAME",	 (anyptr)  declbufncname,    40,
    'A', 'V', "BUFFEREDFILE",    (anyptr) &bufferedfiles,     0,
    'A', 'V', "UNBUFFEREDFILE",  (anyptr) &unbufferedfiles,   0,
    'C', 'V', "RESETBUFNAME",	 (anyptr)  resetbufname,     40,
    'C', 'V', "SETUPBUFNAME",	 (anyptr)  setupbufname,     40,
    'C', 'V', "GETFBUFNAME",     (anyptr)  getfbufname,      40,
    'C', 'V', "CHARGETFBUFNAME", (anyptr)  chargetfbufname,  40,
    'C', 'V', "ARRAYGETFBUFNAME",(anyptr)  arraygetfbufname, 40,
    'C', 'V', "PUTFBUFNAME",     (anyptr)  putfbufname,      40,
    'C', 'V', "CHARPUTFBUFNAME", (anyptr)  charputfbufname,  40,
    'C', 'V', "ARRAYPUTFBUFNAME",(anyptr)  arrayputfbufname, 40,
    'C', 'V', "GETNAME",         (anyptr)  getname,          40,
    'C', 'V', "CHARGETNAME",     (anyptr)  chargetname,      40,
    'C', 'V', "ARRAYGETNAME",    (anyptr)  arraygetname,     40,
    'C', 'V', "PUTNAME",         (anyptr)  putname,          40,
    'C', 'V', "CHARPUTNAME",     (anyptr)  charputname,      40,
    'C', 'V', "ARRAYPUTNAME",    (anyptr)  arrayputname,     40,
    'C', 'V', "EOFBUFNAME",      (anyptr)  eofbufname,       40,
    'C', 'V', "FILEPOSBUFNAME",  (anyptr)  fileposbufname,   40,

/* RANGE CHECKING */
    'S', 'V', "CASECHECK",       (anyptr) &casecheck,         0,
    'S', 'V', "ARRAYCHECK",      (anyptr) &arraycheck,        0,
    'S', 'V', "RANGECHECK",      (anyptr) &rangecheck,        0,
    'S', 'V', "NILCHECK",        (anyptr) &nilcheck,          0,
    'S', 'V', "MALLOCCHECK",     (anyptr) &malloccheck,       0,
    'S', 'V', "CHECKFILEOPEN",   (anyptr) &checkfileopen,     1,
    'S', 'V', "CHECKFILEISOPEN", (anyptr) &checkfileisopen,   0,
    'S', 'V', "CHECKFILEWRITE",  (anyptr) &checkfilewrite,    2,
    'S', 'V', "CHECKREADFORMAT", (anyptr) &checkreadformat,   2,
    'S', 'V', "CHECKFILEEOF",    (anyptr) &checkfileeof,      2,
    'S', 'V', "CHECKSTDINEOF",   (anyptr) &checkstdineof,     2,
    'S', 'V', "CHECKFILESEEK",   (anyptr) &checkfileseek,     2,
}
#endif /* define_parameters */
    ;


#undef extern


#ifdef define_parameters
  int numparams = sizeof(rctable) / sizeof(struct rcstruct);
  Strlist *rcprevvalues[sizeof(rctable) / sizeof(struct rcstruct)];
#else
  extern int numparams;
  extern Strlist *rcprevvalues[];
#endif /* define_parameters */



/* Global variables: */

#ifdef define_globals
# define extern
#endif /* define_globals */


extern char *charname, *ucharname, *scharname, *integername;
extern long min_schar, max_schar, max_uchar;
extern long min_sshort, max_sshort, max_ushort;

extern char *alloctemp;
extern short error_crash;
extern int total_bytes, total_exprs, total_meanings, total_strings;
extern int total_symbols, total_types, total_stmts, total_strlists;
extern int total_literals, total_ctxstacks, total_tempvars, total_inprecs;
extern int total_parens, total_ptrdescs, total_misc;
extern int final_bytes, final_exprs, final_meanings, final_strings;
extern int final_symbols, final_types, final_stmts, final_strlists;
extern int final_literals, final_ctxstacks, final_tempvars, final_inprecs;
extern int final_parens, final_ptrdescs, final_misc;

extern char *infname, *outfname, *codefname, *hdrfname;
extern char *requested_module;
extern FILE *inf, *outf, *codef, *hdrf, *logf;
extern short setup_complete, found_module;
extern short regression, verbose, conserve_mem;
extern int inf_lnum, inf_ltotal;

extern int outindent, outputmode;
extern int outf_lnum;
extern short dontbreaklines;

extern Token curtok;
extern char curtokbuf[256], curtokcase[256];
extern char *inbufptr;
extern int inbufindent;
extern long curtokint;
extern Symbol *curtoksym;
extern Meaning *curtokmeaning;
extern Strlist *curcomments;
extern Strlist **keepingstrlist;
extern short ignore_directives, skipping_module;
extern short C_lex;
extern char sysprog_flag, partial_eval_flag, iocheck_flag;
extern char range_flag, ovflcheck_flag, stackcheck_flag;
extern short switch_strpos;
extern int fixedflag;
extern int numimports;
extern Strlist *tempoptionlist;
extern long curserial, serialcount;
extern int notephase;
extern Strlist *permimports;
extern int permflag;

#define SYMHASHSIZE 293
extern Symbol *(symtab[SYMHASHSIZE]);
extern short partialdump;

#define MAXWITHS 100
extern int withlevel;
extern Type *withlist[MAXWITHS];
extern Expr *withexprs[MAXWITHS];

extern Token blockkind;
extern Meaning *curctx, *curctxlast, *nullctx;

extern int fixexpr_tryblock;
extern short fixexpr_tryflag;

extern Type *tp_integer, *tp_char, *tp_boolean, *tp_real, *tp_longreal;
extern Type *tp_anyptr, *tp_jmp_buf, *tp_schar, *tp_uchar, *tp_charptr;
extern Type *tp_int, *tp_sshort, *tp_ushort, *tp_abyte, *tp_sbyte, *tp_ubyte;
extern Type *tp_void, *tp_str255, *tp_strptr, *tp_text, *tp_bigtext;
extern Type *tp_unsigned, *tp_uint, *tp_sint, *tp_smallset, *tp_proc;
extern Meaning *mp_string, *mp_true, *mp_false;
extern Meaning *mp_input, *mp_output, *mp_stderr;
extern Meaning *mp_maxint, *mp_minint, *mp_escapecode, *mp_ioresult;
extern Meaning *mp_uchar, *mp_schar, *mp_unsigned, *mp_uint;
extern Meaning *mp_str_hp, *mp_str_turbo;
extern Meaning *mp_val_modula, *mp_val_turbo;
extern Meaning *mp_blockread_ucsd, *mp_blockread_turbo;
extern Meaning *mp_blockwrite_ucsd, *mp_blockwrite_turbo;
extern Meaning *mp_dec_dec, *mp_dec_turbo;
extern Expr *ex_input, *ex_output;
extern Strlist *attrlist;


#ifndef define_globals
# undef extern
#endif




/* Function declarations are created automatically by "makeproto" */

#include "p2c.hdrs"

#include "p2c.proto"



/* Our library omits declarations for these functions! */

int link           PP( (char *, char *) );
int unlink         PP( (char *) );



#define minspcthresh ((minspacingthresh >= 0) ? minspacingthresh : minspacing)

#define delfreearg(ex, n) freeexpr((*(ex))->args[n]), deletearg(ex, n)
#define delsimpfreearg(ex, n) freeexpr((*(ex))->args[n]), delsimparg(ex, n)

#define swapexprs(a,b) do {register Expr *t=(a);(a)=(b);(b)=(t);} while (0)
#define swapstmts(a,b) do {register Stmt *t=(a);(a)=(b);(b)=(t);} while (0)

#define CHECKORDEXPR(ex,v) ((ex)->kind==EK_CONST ? (ex)->val.i - (v) : -2)

#define FCheck(flag)  ((flag) == 1 || (!iocheck_flag && (flag)))
#define checkeof(fex)  (isvar(fex, mp_input) ? FCheck(checkstdineof)  \
					     : FCheck(checkfileeof))


#ifdef TEST_MALLOC   /* Memory testing */

#define ALLOC(N,TYPE,NAME) \
    (TYPE *) test_malloc((unsigned)((N)*sizeof(TYPE)),  \
			 &__CAT__(total_,NAME), &__CAT__(final_,NAME))

#define ALLOCV(N,TYPE,NAME) \
    (TYPE *) test_malloc((unsigned)(N),  \
			 &__CAT__(total_,NAME), &__CAT__(final_,NAME))

#define REALLOC(P,N,TYPE) \
    (TYPE *) test_realloc((char *)(P), (unsigned)((N)*sizeof(TYPE)))

#define FREE(P) test_free((char*)(P))

#else  /* not TEST_MALLOC */

/* If p2c always halts immediately with an out-of-memory error, try
   recompiling all modules with BROKEN_OR defined. */
#ifdef BROKEN_OR

#define ALLOC(N,TYPE,NAME) \
    ((alloctemp = malloc((unsigned)((N)*sizeof(TYPE)))), \
     (alloctemp ? (TYPE *) alloctemp : (TYPE *) outmem()))

#define ALLOCV(N,TYPE,NAME) \
    ((alloctemp = malloc((unsigned)(N))), \
     (alloctemp ? (TYPE *) alloctemp : (TYPE *) outmem()))

#define REALLOC(P,N,TYPE) \
    ((alloctemp = realloc((char*)(P), (unsigned)((N)*sizeof(TYPE)))), \
     (alloctemp ? (TYPE *) alloctemp : (TYPE *) outmem()))

#define FREE(P) free((char*)(P))

#else  /* not BROKEN_OR */

#define ALLOC(N,TYPE,NAME) \
    ((alloctemp = malloc((unsigned)((N)*sizeof(TYPE)))) || outmem(), \
     (TYPE *) alloctemp)

#define ALLOCV(N,TYPE,NAME) \
    ((alloctemp = malloc((unsigned)(N))) || outmem(), \
     (TYPE *) alloctemp)

#define REALLOC(P,N,TYPE) \
    ((alloctemp = realloc((char*)(P), (unsigned)((N)*sizeof(TYPE)))) || outmem(), \
     (TYPE *) alloctemp)

#define FREE(P) free((char*)(P))

#endif  /* BROKEN_OR */
#endif  /* TEST_MALLOC */


#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))



#ifdef toupper
# undef toupper
# undef tolower
# define toupper(c)   my_toupper(c)
# define tolower(c)   my_tolower(c)
#endif

#ifndef _toupper
# if 'A' == 65 && 'a' == 97
#  define _toupper(c)  ((c)-'a'+'A')
#  define _tolower(c)  ((c)-'A'+'a')
# else
#  ifdef toupper
#   undef toupper   /* hope these are shadowing real functions, */
#   undef tolower   /* because my_toupper calls _toupper! */
#  endif
#  define _toupper(c)  toupper(c)
#  define _tolower(c)  tolower(c)
# endif
#endif




/* End. */

