/* Data definitions for internal representation of bison's input,
   Copyright (C) 1984, 1986 Bob Corbett and Free Software Foundation, Inc.

BISON is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the BISON General Public License for full details.

Everyone is granted permission to copy, modify and redistribute BISON,
but only under the conditions described in the BISON General Public
License.  A copy of this license is supposed to have been given to you
along with BISON so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

/* representation of the grammar rules:

ntokens is the number of tokens, and nvars is the number of variables (nonterminals).
nsyms is the total number, ntokens + nvars.

Each symbol (either token or variable) receives a symbol number.
Numbers 0 to ntokens-1 are for tokens, and ntokens to nsyms-1 are for variables.
Symbol number zero is the end-of-input token.  This token is counted in ntokens.

The rules receive rule numbers 1 to nrules in the order they are written.
Actions and guards are accessed via the rule number.

The rules themselves are described by three arrays: rrhs, rlhs and ritems.
rlhs[r] is the symbol number of the left hand side of rule r.
The right hand side is stored as symbol numbers in a portion of ritems.
rrhs[r] contains the index in ritems of the beginning of the portion for rule r.
The length of the portion is one greater
 than the number of symbols in the rule's right hand side.
The last element in the portion contains minus r, which
identifies it as the end of a portion and says which rule it is for.

The portions of ritems come in order of increasing rule number and are
followed by an element which is zero to mark the end.  nitems is the
total length of ritems, not counting the final zero.  Each element of
ritems is called an "item" and its index in ritems is an item number.

Item numbers are used in the finite state machine to represent
places that parsing can get to.

Precedence levels are recorded in the vectors sprec and rprec.
sprec records the precedence level of each symbol,
rprec the precedence level of each rule.

Precedence levels are assigned in increasing order starting with 1
so that numerically higher precedence values mean tighter binding
as they ought to.  Zero as a symbol or rule's precedence means none is assigned.

Associativities are recorded similarly in rassoc and sassoc.  */


#define	ISTOKEN(s)	((s) < ntokens)
#define	ISVAR(s)	((s) >= ntokens)


extern int nitems;
extern int nrules;
extern int nsyms;
extern int ntokens;
extern int nvars;

extern short *ritem;
extern short *rlhs;
extern short *rrhs;
extern short *rprec;
extern short *sprec;
extern short *rassoc;
extern short *sassoc;
extern short *rline;		/* Source line number of each rule */

extern int start_symbol;


/* associativity values in elements of rassoc, sassoc.  */

#define RIGHT_ASSOC 1
#define LEFT_ASSOC 2
#define NON_ASSOC 3

/* token translation table:
indexed by a token number as returned by the user's yylex routine,
it yields the internal token number used by the parser and throughout bison.
If translations is zero, the translation table is not used because
the two kinds of token numbers are the same.  */

extern short *token_translations;
extern int translations;
extern int max_user_token_number;

/* semantic_parser is nonzero if the input file says to use the hairy parser
that provides for semantic error recovery.  If it is zero, the yacc-compatible
simplified parser is used.  */

extern int semantic_parser;

/* pure_parser is nonzero if should generate a parser that is all pure and reentrant. */

extern int pure_parser;

/* error_token_number is the token number of the error token.  */

extern int error_token_number;
