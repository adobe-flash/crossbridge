/*************************************************************
 *							     *
 * 	Macros defining special characters.		     *
 *							     *
 *************************************************************/

#define NUL	    '\0'
#define ASCII_MIN   '\001'
#define ASCII_MAX   '\177'

/*************************************************************
 *							     *
 * 	Macros defining lexical categories.		     *
 *							     *
 *************************************************************/

#define C_LIT	0   /* individual character literal */
#define C_SET	1   /* character set literal	    */

#define EOS		0	/* end-of-string */
#define LITERAL		1
#define OPSTAR		2
#define OPALT		3
#define OPOPT		4
#define OPCAT		5
#define LPAREN		6
#define RPAREN		7

/*************************************************************
 *							     *
 * 	Macros for manipulating syntax tree nodes.	     *
 *							     *
 *************************************************************/

#define lit_type(x)	(x->l_type)
#define lit_pos(x)	(x->pos)
#define lit_char(x)	((x->val).c)
#define lit_cset(x)	((x->val).cset)

#define tok_type(x)	(x->type)
#define tok_val(x)	(x->val)
#define tok_op(x)       (x->val->op)
#define tok_lit(x)	((x->val->refs).lit)

#define Op(x)		(x->op)
#define Lit(x)		((x->refs).lit)
#define Child(x)	((x->refs).child)
#define Lchild(x)	((x->refs).children.l_child)
#define Rchild(x)	((x->refs).children.r_child)
#define Nullable(x)	(x->nullable)
#define Firstpos(x)	(x->firstposn)
#define Lastpos(x)	(x->lastposn)

/*************************************************************
 *							     *
 *  Macros for manipulating DFA states and sets of states.   *
 *							     *
 *************************************************************/

#define Positions(x)	(x->posns)
#define Final_St(x)	(x->final)
#define Goto(x, c)	((x->trans)[c])
#define Next_State(x)	((x)->next_state)

/*************************************************************/

#define new_node(x)	malloc(sizeof(*x))

typedef struct {	/* character range literals */
	    char low_bd, hi_bd;
	} *Ch_Range;

typedef struct ch_set {	    /* character set literals */
	    Ch_Range elt;	/* rep. as list of ranges */
	    struct ch_set *rest;
	} *Ch_Set;

typedef struct {	/* regular expression literal */
	    int pos;	     /* position in syntax tree */
	    short l_type;    /* type of literal */
	    union {
		char c;     /* for character literals */
		Ch_Set cset; /* for character sets */
	    } val;
	} *Re_Lit, *(*Re_lit_array)[];

typedef struct pnode {
	    int posnum;
	    struct pnode *nextpos;
	} *Pset, *(*Pset_array)[];

typedef struct rnode {	/* regular expression node */
	    short op;     /* operator at that node */
	    union {
		Re_Lit lit;		/* child is a leaf node */
		struct rnode *child;	/* child of unary op */
		struct {
		    struct rnode *l_child; 
		    struct rnode *r_child;
		} children;		/* children of binary op */
	    } refs;
	    short nullable;
	    Pset firstposn, lastposn;
	} *Re_node;

typedef struct {			/* token node */
	    short type;
	    Re_node val;
	} *Tok_node;


typedef struct snode {
	    Re_node val;
	    int size;
	    struct snode *next;
	} *Stack;

typedef struct dfa_st {
	    Pset posns;
	    int final;	    /* 1 if the state is a final state, 0 o/w */
	    struct dfa_st *trans[128];
	} *Dfa_state;

typedef struct dfa_stset {
	    Dfa_state st;
	    struct dfa_stset *next_state;
	} *Dfa_state_set;


