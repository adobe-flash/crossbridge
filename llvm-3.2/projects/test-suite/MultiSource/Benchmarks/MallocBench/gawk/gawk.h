/*
 * awk.h -- Definitions for gawk. 
 */

/* 
 * Copyright (C) 1986, 1988, 1989 the Free Software Foundation, Inc.
 * 
 * This file is part of GAWK, the GNU implementation of the
 * AWK Progamming Language.
 * 
 * GAWK is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 * 
 * GAWK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GAWK; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* ------------------------------ Includes ------------------------------ */
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
//#include <varargs.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "regex.h"

/* ------------------- System Functions, Variables, etc ------------------- */
/* nasty nasty SunOS-ism */
#ifdef sparc
#include <alloca.h>
#else
extern char *alloca();
#endif
/*
 * if you don't have vprintf, but you are BSD, the version defined in
 * vprintf.c should do the trick.  Otherwise, use this and cross your fingers.
 */
#if defined(VPRINTF_MISSING) && !defined(DOPRNT_MISSING) && !defined(BSDSTDIO)
#define vfprintf(fp,fmt,arg)	_doprnt((fmt), (arg), (fp))
#endif

#if 0
#ifdef __STDC__
extern void *malloc(unsigned), *realloc(void *, unsigned);
/* LLVM - Changed this to match Linux/Solaris free() */
extern void free(void *);
/* LLVM - Fixed prototype */
extern char *getenv(const char *);

extern char *strcpy(char *, char *), *strcat(char *, char *), *strncpy(char *, char *, int);
extern int strcmp(char *, char *);
extern int strncmp(char *, char *, int);
extern int strncasecmp(char *, char *, int);
extern char *strerror(int);
extern char *strchr(char *, int);
extern int strlen(char *);
extern	char *memcpy(char *, char *, int);
extern	int memcmp(char *, char *, int);
extern	char *memset(char *, int, int);

/* extern int fprintf(FILE *, char *, ...); */
extern int fprintf();
extern int vfprintf();
#ifndef MSDOS
extern unsigned int fwrite(const void *, unsigned int, unsigned int, FILE *);
#endif
extern int fflush(FILE *);
extern int fclose(FILE *);
extern int pclose(FILE *);
#ifndef MSDOS
extern int fputs(const char *, FILE *);
#endif
extern void abort();
extern int isatty(int);
extern void exit(int);
/* LLVM - Fixed prototype */
extern int system(const char *);
extern int sscanf(/* char *, char *, ... */);

/* LLVM - Fixed prototype */
extern double atof(const char *);
extern int fstat(int, struct stat *);
extern off_t lseek(int, off_t, int);
extern int fseek(FILE *, long, int);
extern int close(int);
#endif
/* LLVM - Get rid of these prototypes */
#if 0
extern int open();
extern int pipe(int *);
extern int dup2(int, int);
#endif

#if 0
#ifndef MSDOS
extern int unlink(char *);
#endif
extern int fork();
extern int execl(/* char *, char *, ... */);
extern int read(int, char *, int);
extern int wait(int *);
extern void _exit(int);
#else
extern void _exit();
extern int wait();
extern int read();
extern int execl();
extern int fork();
extern int unlink();
extern int dup2();
extern int pipe();
extern int open();
extern int close();
extern int fseek();
extern off_t lseek();
extern int fstat();
extern void exit();
extern int system();
extern int isatty();
extern void abort();
extern int fputs();
extern int fclose();
extern int pclose();
extern int fflush();
extern int fwrite();
extern int fprintf();
extern int vfprintf();
extern int sscanf();
extern char *malloc(), *realloc();
extern void free();
extern char *getenv();

extern int strcmp();
extern int strncmp();
extern int strncasecmp();
extern int strlen();
extern char *strcpy(), *strcat(), *strncpy();
extern	char *memset();
extern	int memcmp();
extern	char *memcpy();
extern char *strerror();
extern char *strchr();

extern double atof();
#endif
#endif

#ifndef MSDOS
extern int errno;
#endif	/* MSDOS */

/* ------------------ Constants, Structures, Typedefs  ------------------ */
#define AWKNUM	double

typedef enum {
	/* illegal entry == 0 */
	Node_illegal,

	/* binary operators  lnode and rnode are the expressions to work on */
	Node_times,
	Node_quotient,
	Node_mod,
	Node_plus,
	Node_minus,
	Node_cond_pair,		/* conditional pair (see Node_line_range) */
	Node_subscript,
	Node_concat,
	Node_exp,

	/* unary operators   subnode is the expression to work on */
/*10*/	Node_preincrement,
	Node_predecrement,
	Node_postincrement,
	Node_postdecrement,
	Node_unary_minus,
	Node_field_spec,

	/* assignments   lnode is the var to assign to, rnode is the exp */
	Node_assign,
	Node_assign_times,
	Node_assign_quotient,
	Node_assign_mod,
/*20*/	Node_assign_plus,
	Node_assign_minus,
	Node_assign_exp,

	/* boolean binaries   lnode and rnode are expressions */
	Node_and,
	Node_or,

	/* binary relationals   compares lnode and rnode */
	Node_equal,
	Node_notequal,
	Node_less,
	Node_greater,
	Node_leq,
/*30*/	Node_geq,
	Node_match,
	Node_nomatch,

	/* unary relationals   works on subnode */
	Node_not,

	/* program structures */
	Node_rule_list,		/* lnode is a rule, rnode is rest of list */
	Node_rule_node,		/* lnode is pattern, rnode is statement */
	Node_statement_list,	/* lnode is statement, rnode is more list */
	Node_if_branches,	/* lnode is to run on true, rnode on false */
	Node_expression_list,	/* lnode is an exp, rnode is more list */
	Node_param_list,	/* lnode is a variable, rnode is more list */

	/* keywords */
/*40*/	Node_K_if,		/* lnode is conditonal, rnode is if_branches */
	Node_K_while,		/* lnode is condtional, rnode is stuff to run */
	Node_K_for,		/* lnode is for_struct, rnode is stuff to run */
	Node_K_arrayfor,	/* lnode is for_struct, rnode is stuff to run */
	Node_K_break,		/* no subs */
	Node_K_continue,	/* no stuff */
	Node_K_print,		/* lnode is exp_list, rnode is redirect */
	Node_K_printf,		/* lnode is exp_list, rnode is redirect */
	Node_K_next,		/* no subs */
	Node_K_exit,		/* subnode is return value, or NULL */
	Node_K_do,		/* lnode is conditional, rnode stuff to run */
	Node_K_return,
	Node_K_delete,
	Node_K_getline,
	Node_K_function,	/* lnode is statement list, rnode is params */

	/* I/O redirection for print statements */
	Node_redirect_output,	/* subnode is where to redirect */
	Node_redirect_append,	/* subnode is where to redirect */
	Node_redirect_pipe,	/* subnode is where to redirect */
	Node_redirect_pipein,	/* subnode is where to redirect */
	Node_redirect_input,	/* subnode is where to redirect */

	/* Variables */
	Node_var,		/* rnode is value, lnode is array stuff */
	Node_var_array,		/* array is ptr to elements, asize num of
				 * eles */
	Node_val,		/* node is a value - type in flags */

	/* Builtins   subnode is explist to work on, proc is func to call */
	Node_builtin,

	/*
	 * pattern: conditional ',' conditional ;  lnode of Node_line_range
	 * is the two conditionals (Node_cond_pair), other word (rnode place)
	 * is a flag indicating whether or not this range has been entered.
	 */
	Node_line_range,

	/*
	 * boolean test of membership in array lnode is string-valued
	 * expression rnode is array name 
	 */
	Node_in_array,

	Node_func,		/* lnode is param. list, rnode is body */
	Node_func_call,		/* lnode is name, rnode is argument list */

	Node_cond_exp,		/* lnode is conditonal, rnode is if_branches */
	Node_regex,
	Node_hashnode,
	Node_ahash,
} NODETYPE;

/*
 * NOTE - this struct is a rather kludgey -- it is packed to minimize
 * space usage, at the expense of cleanliness.  Alter at own risk.
 */
typedef struct exp_node {
	union {
		struct {
			union {
				struct exp_node *lptr;
				char *param_name;
				char *retext;
				struct exp_node *nextnode;
			} l;
			union {
				struct exp_node *rptr;
				struct exp_node *(*pptr) ();
				struct re_pattern_buffer *preg;
				struct for_loop_header *hd;
				struct exp_node **av;
				int r_ent;	/* range entered */
			} r;
			char *name;
			short number;
			unsigned char recase;
		} nodep;
		struct {
			AWKNUM fltnum;	/* this is here for optimal packing of
					 * the structure on many machines
					 */
			char *sp;
			short slen;
			unsigned char sref;
		} val;
		struct {
			struct exp_node *next;
			char *name;
			int length;
			struct exp_node *value;
		} hash;
#define	hnext	sub.hash.next
#define	hname	sub.hash.name
#define	hlength	sub.hash.length
#define	hvalue	sub.hash.value
		struct {
			struct exp_node *next;
			struct exp_node *name;
			struct exp_node *value;
		} ahash;
#define	ahnext	sub.ahash.next
#define	ahname	sub.ahash.name
#define	ahvalue	sub.ahash.value
	} sub;
	NODETYPE type;
	unsigned char flags;
#			define	MEM	0x7
#			define	MALLOC	1	/* can be free'd */
#			define	TEMP	2	/* should be free'd */
#			define	PERM	4	/* can't be free'd */
#			define	VAL	0x18
#			define	NUM	8	/* numeric value is valid */
#			define	STR	16	/* string value is valid */
#			define	NUMERIC	32	/* entire field is numeric */
} NODE;

#define lnode	sub.nodep.l.lptr
#define nextp	sub.nodep.l.nextnode
#define rnode	sub.nodep.r.rptr
#define source_file	sub.nodep.name
#define	source_line	sub.nodep.number
#define	param_cnt	sub.nodep.number
#define param	sub.nodep.l.param_name

#define subnode	lnode
#define proc	sub.nodep.r.pptr

#define reexp	lnode
#define rereg	sub.nodep.r.preg
#define re_case sub.nodep.recase
#define re_text sub.nodep.l.retext

#define forsub	lnode
#define forloop	rnode->sub.nodep.r.hd

#define stptr	sub.val.sp
#define stlen	sub.val.slen
#define stref	sub.val.sref
#define	valstat	flags

#define numbr	sub.val.fltnum

#define var_value lnode
#define var_array sub.nodep.r.av

#define condpair lnode
#define triggered sub.nodep.r.r_ent

#define HASHSIZE 101

typedef struct for_loop_header {
	NODE *init;
	NODE *cond;
	NODE *incr;
} FOR_LOOP_HEADER;

/* for "for(iggy in foo) {" */
struct search {
	int numleft;
	NODE **arr_ptr;
	NODE *bucket;
	NODE *retval;
};

/* for faster input, bypass stdio */
typedef struct iobuf {
	int fd;
	char *buf;
	char *off;
	int size;	/* this will be determined by an fstat() call */
	int cnt;
	char *secbuf;
	int secsiz;
	int flag;
#	define		IOP_IS_TTY	1
} IOBUF;

/*
 * structure used to dynamically maintain a linked-list of open files/pipes
 */
struct redirect {
	int flag;
#		define		RED_FILE	1
#		define		RED_PIPE	2
#		define		RED_READ	4
#		define		RED_WRITE	8
#		define		RED_APPEND	16
#		define		RED_NOBUF	32
	char *value;
	FILE *fp;
	IOBUF *iop;
	int pid;
	int status;
	long offset;		/* used for dynamic management of open files */
	struct redirect *prev;
	struct redirect *next;
};

/* longjmp return codes, must be nonzero */
/* Continue means either for loop/while continue, or next input record */
#define TAG_CONTINUE 1
/* Break means either for/while break, or stop reading input */
#define TAG_BREAK 2
/* Return means return from a function call; leave value in ret_node */
#define	TAG_RETURN 3

#ifdef MSDOS
#define HUGE	0x7fff
#else
#define HUGE	0x7fffffff
#endif

/* -------------------------- External variables -------------------------- */
/* gawk builtin variables */
extern NODE *FS_node, *NF_node, *RS_node, *NR_node;
extern NODE *FILENAME_node, *OFS_node, *ORS_node, *OFMT_node;
extern NODE *FNR_node, *RLENGTH_node, *RSTART_node, *SUBSEP_node;
extern NODE *IGNORECASE_node;

extern NODE **stack_ptr;
extern NODE *Nnull_string;
extern NODE *deref;
extern NODE **fields_arr;
extern int sourceline;
extern char *source;
extern NODE *expression_value;

extern NODE *variables[];

extern NODE *_t;	/* used as temporary in tree_eval */

extern char *myname;

extern int node0_valid;
extern int field_num;
extern int strict;

/* ------------------------- Pseudo-functions ------------------------- */
#define is_identchar(c) (isalnum(c) || (c) == '_')


#define	free_temp(n)	if ((n)->flags&TEMP) { deref = (n); do_deref(); } else
#define	tree_eval(t)	(_t = (t),(_t) == NULL ? Nnull_string : \
			((_t)->type == Node_val ? (_t) : r_tree_eval((_t))))
#define	make_string(s,l)	make_str_node((s),(l),0)

#define	cant_happen()	fatal("line %d, file: %s; bailing out", \
				__LINE__, __FILE__);
#ifdef MEMDEBUG
#define memmsg(x,y,z,zz)	fprintf(stderr, "malloc: %s: %s: %d %0x\n", z, x, y, zz)
#define free(s)	fprintf(stderr, "free: s: %0x\n", s), do_free(s)
#else
#define memmsg(x,y,z,zz)
#endif

#ifdef BWGC
#define	emalloc(var,ty,x,str)	if ((var = (ty) GC_malloc((unsigned)((x)==0?1:(x)))) == NULL)\
				    fatal("%s: %s: can't allocate memory (%s)",\
					(str), "var", strerror(errno)); else\
				    memmsg("var", x, str, var)
#define	erealloc(var,ty,x,str)	if((var=(ty)GC_realloc((char *)var,\
						(unsigned)(x)))==NULL)\
				    fatal("%s: %s: can't allocate memory (%s)",\
					(str), "var", strerror(errno)); else\
				    memmsg("re: var", x, str, var)
#else
#define	emalloc(var,ty,x,str)	if ((var = (ty) malloc((unsigned)(x))) == NULL)\
				    fatal("%s: %s: can't allocate memory (%s)",\
					(str), "var", strerror(errno)); else\
				    memmsg("var", x, str, var)
#define	erealloc(var,ty,x,str)	if((var=(ty)realloc((char *)var,\
						(unsigned)(x)))==NULL)\
				    fatal("%s: %s: can't allocate memory (%s)",\
					(str), "var", strerror(errno)); else\
				    memmsg("re: var", x, str, var)
#endif BWGC
#ifdef DEBUG
#define	force_number	r_force_number
#define	force_string	r_force_string
#else
#ifdef lint
extern AWKNUM force_number();
#endif
#ifdef MSDOS
extern double _msc51bug;
#define	force_number(n)	(_msc51bug=(_t = (n),(_t->flags & NUM) ? _t->numbr : r_force_number(_t)))
#else
#define	force_number(n)	(_t = (n),(_t->flags & NUM) ? _t->numbr : r_force_number(_t))
#endif
#define	force_string(s)	(_t = (s),(_t->flags & STR) ? _t : r_force_string(_t))
#endif

#define	STREQ(a,b)	(*(a) == *(b) && strcmp((a), (b)) == 0)
#define	STREQN(a,b,n)	((n) && *(a) == *(b) && strncmp((a), (b), (n)) == 0)

#define	WHOLELINE	(node0_valid ? fields_arr[0] : *get_field(0,0))

/* ------------- Function prototypes or defs (as appropriate) ------------- */
#ifdef __STDC__
extern	int parse_escape(char **);
extern	int devopen(char *, char *);
extern	struct re_pattern_buffer *make_regexp(NODE *, int);
extern	struct re_pattern_buffer *mk_re_parse(char *, int);
extern	NODE *variable(char *);
extern	NODE *install(NODE **, char *, NODE *);
extern	NODE *lookup(NODE **, char *);
extern	NODE *make_name(char *, NODETYPE);
extern	int interpret(NODE *);
extern	NODE *r_tree_eval(NODE *);
extern	void assign_number(NODE **, double);
extern	int cmp_nodes(NODE *, NODE *);
extern	struct redirect *redirect(NODE *, int *);
extern	int flush_io(void);
extern	void print_simple(NODE *, FILE *);
extern	void warning(char *,...);
/* extern	void warning(); */
extern	void fatal(char *,...);
/* extern	void fatal(); */
extern	void set_record(char *, int);
extern	NODE **get_field(int, int);
extern	NODE **get_lhs(NODE *, int);
extern	void do_deref(void );
extern	struct search *assoc_scan(NODE *);
extern	struct search *assoc_next(struct search *);
extern	NODE **assoc_lookup(NODE *, NODE *);
extern	double r_force_number(NODE *);
extern	NODE *r_force_string(NODE *);
extern	NODE *newnode(NODETYPE);
extern	NODE *dupnode(NODE *);
extern	NODE *make_number(double);
extern	NODE *tmp_number(double);
extern	NODE *make_str_node(char *, int, int);
extern	NODE *tmp_string(char *, int);
extern	char *re_compile_pattern(char *, int, struct re_pattern_buffer *);
extern	int re_search(struct re_pattern_buffer *, char *, int, int, int, struct re_registers *);
extern	void freenode(NODE *);

#else
extern	int parse_escape();
extern	void freenode();
extern	int devopen();
extern	struct re_pattern_buffer *make_regexp();
extern	struct re_pattern_buffer *mk_re_parse();
extern	NODE *variable();
extern	NODE *install();
extern	NODE *lookup();
extern	int interpret();
extern	NODE *r_tree_eval();
extern	void assign_number();
extern	int cmp_nodes();
extern	struct redirect *redirect();
extern	int flush_io();
extern	void print_simple();
extern	void warning();
extern	void fatal();
extern	void set_record();
extern	NODE **get_field();
extern	NODE **get_lhs();
extern	void do_deref();
extern	struct search *assoc_scan();
extern	struct search *assoc_next();
extern	NODE **assoc_lookup();
extern	double r_force_number();
extern	NODE *r_force_string();
extern	NODE *newnode();
extern	NODE *dupnode();
extern	NODE *make_number();
extern	NODE *tmp_number();
extern	NODE *make_str_node();
extern	NODE *tmp_string();
extern	char *re_compile_pattern();
extern	int re_search();
#endif

#if !defined(__STDC__) || __STDC__ <= 0
#define volatile
#endif

/* Figure out what '\a' really is. */
#ifdef __STDC__
#define BELL	'\a'		/* sure makes life easy, don't it? */
#else
#	if 'z' - 'a' == 25	/* ascii */
#		if 'a' != 97	/* machine is dumb enough to use mark parity */
#			define BELL	'\207'
#		else
#			define BELL	'\07'
#		endif
#	else
#		define BELL	'\057'
#	endif
#endif

#ifndef SIGTYPE
#define SIGTYPE	void
#endif

extern char casetable[];	/* for case-independent regexp matching */

#ifdef BWGC
#define	malloc(n)	GC_malloc((n)==0?1:(n))
#endif
#ifdef IGNOREFREE
#define	free(ptr)	{};
#endif
