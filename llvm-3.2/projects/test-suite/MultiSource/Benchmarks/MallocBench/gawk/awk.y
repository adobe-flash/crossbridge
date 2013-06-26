/*
 * awk.y --- yacc/bison parser
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

%{
#ifdef DEBUG
#define YYDEBUG 12
#endif

#include "gawk.h"

/*
 * This line is necessary since the Bison parser skeleton uses bcopy.
 * Systems without memcpy should use -DMEMCPY_MISSING, per the Makefile.
 * It should not hurt anything if Yacc is being used instead of Bison.
 */
#define bcopy(s,d,n)	memcpy((d),(s),(n))

extern void msg(char * message, ...);
extern struct re_pattern_buffer *mk_re_parse();

NODE *node();
NODE *lookup();
NODE *install();

static NODE *snode();
static NODE *mkrangenode();
static FILE *pathopen();
static NODE *make_for_loop();
static NODE *append_right();
static void func_install();
static NODE *make_param();
static int hashf();
static void pop_params();
static void pop_var();
static int yylex ();
static void yyerror(char * msg, ...);

static int want_regexp;		/* lexical scanning kludge */
static int want_assign;		/* lexical scanning kludge */
static int can_return;		/* lexical scanning kludge */
static int io_allowed = 1;	/* lexical scanning kludge */
static int lineno = 1;		/* for error msgs */
static char *lexptr;		/* pointer to next char during parsing */
static char *lexptr_begin;	/* keep track of where we were for error msgs */
static int curinfile = -1;	/* index into sourcefiles[] */
static int param_counter;

NODE *variables[HASHSIZE];

extern int errcount;
extern NODE *begin_block;
extern NODE *end_block;
%}

%union {
	long lval;
	AWKNUM fval;
	NODE *nodeval;
	NODETYPE nodetypeval;
	char *sval;
	NODE *(*ptrval)();
}

%type <nodeval> function_prologue function_body
%type <nodeval> rexp exp start program rule simp_exp
%type <nodeval> pattern 
%type <nodeval>	action variable param_list
%type <nodeval>	rexpression_list opt_rexpression_list
%type <nodeval>	expression_list opt_expression_list
%type <nodeval>	statements statement if_statement opt_param_list 
%type <nodeval> opt_exp opt_variable regexp 
%type <nodeval> input_redir output_redir
%type <nodetypeval> r_paren comma nls opt_nls print

%type <sval> func_name
%token <sval> FUNC_CALL NAME REGEXP
%token <lval> ERROR
%token <nodeval> NUMBER YSTRING
%token <nodetypeval> RELOP APPEND_OP
%token <nodetypeval> ASSIGNOP MATCHOP NEWLINE CONCAT_OP
%token <nodetypeval> LEX_BEGIN LEX_END LEX_IF LEX_ELSE LEX_RETURN LEX_DELETE
%token <nodetypeval> LEX_WHILE LEX_DO LEX_FOR LEX_BREAK LEX_CONTINUE
%token <nodetypeval> LEX_PRINT LEX_PRINTF LEX_NEXT LEX_EXIT LEX_FUNCTION
%token <nodetypeval> LEX_GETLINE
%token <nodetypeval> LEX_IN
%token <lval> LEX_AND LEX_OR INCREMENT DECREMENT
%token <ptrval> LEX_BUILTIN LEX_LENGTH

/* these are just yylval numbers */

/* Lowest to highest */
%right ASSIGNOP
%right '?' ':'
%left LEX_OR
%left LEX_AND
%left LEX_GETLINE
%nonassoc LEX_IN
%left FUNC_CALL LEX_BUILTIN LEX_LENGTH
%nonassoc MATCHOP
%nonassoc RELOP '<' '>' '|' APPEND_OP
%left CONCAT_OP
%left YSTRING NUMBER
%left '+' '-'
%left '*' '/' '%'
%right '!' UNARY
%right '^'
%left INCREMENT DECREMENT
%left '$'
%left '(' ')'

%%

start
	: opt_nls program opt_nls
		{ expression_value = $2; }
	;

program
	: rule
		{ 
			if ($1 != NULL)
				$$ = $1;
			else
				$$ = NULL;
			yyerrok;
		}
	| program rule
		/* add the rule to the tail of list */
		{
			if ($2 == NULL)
				$$ = $1;
			else if ($1 == NULL)
				$$ = $2;
			else {
				if ($1->type != Node_rule_list)
					$1 = node($1, Node_rule_list,
						(NODE*)NULL);
				$$ = append_right ($1,
				   node($2, Node_rule_list,(NODE *) NULL));
			}
			yyerrok;
		}
	| error	{ $$ = NULL; }
	| program error { $$ = NULL; }
	;

rule
	: LEX_BEGIN { io_allowed = 0; }
	  action
	  {
		if (begin_block) {
			if (begin_block->type != Node_rule_list)
				begin_block = node(begin_block, Node_rule_list,
					(NODE *)NULL);
			append_right (begin_block, node(
			    node((NODE *)NULL, Node_rule_node, $3),
			    Node_rule_list, (NODE *)NULL) );
		} else
			begin_block = node((NODE *)NULL, Node_rule_node, $3);
		$$ = NULL;
		io_allowed = 1;
		yyerrok;
	  }
	| LEX_END { io_allowed = 0; }
	  action
	  {
		if (end_block) {
			if (end_block->type != Node_rule_list)
				end_block = node(end_block, Node_rule_list,
					(NODE *)NULL);
			append_right (end_block, node(
			    node((NODE *)NULL, Node_rule_node, $3),
			    Node_rule_list, (NODE *)NULL));
		} else
			end_block = node((NODE *)NULL, Node_rule_node, $3);
		$$ = NULL;
		io_allowed = 1;
		yyerrok;
	  }
	| LEX_BEGIN statement_term
	  {
		msg ("error near line %d: BEGIN blocks must have an action part", lineno);
		errcount++;
		yyerrok;
	  }
	| LEX_END statement_term
	  {
		msg ("error near line %d: END blocks must have an action part", lineno);
		errcount++;
		yyerrok;
	  }
	| pattern action
		{ $$ = node ($1, Node_rule_node, $2); yyerrok; }
	| action
		{ $$ = node ((NODE *)NULL, Node_rule_node, $1); yyerrok; }
	| pattern statement_term
		{ if($1) $$ = node ($1, Node_rule_node, (NODE *)NULL); yyerrok; }
	| function_prologue function_body
		{
			func_install($1, $2);
			$$ = NULL;
			yyerrok;
		}
	;

func_name
	: NAME
		{ $$ = $1; }
	| FUNC_CALL
		{ $$ = $1; }
	;
		
function_prologue
	: LEX_FUNCTION 
		{
			param_counter = 0;
		}
	  func_name '(' opt_param_list r_paren opt_nls
		{
			$$ = append_right(make_param($3), $5);
			can_return = 1;
		}
	;

function_body
	: l_brace statements r_brace
	  {
		$$ = $2;
		can_return = 0;
	  }
	;


pattern
	: exp
		{ $$ = $1; }
	| exp comma exp
		{ $$ = mkrangenode ( node($1, Node_cond_pair, $3) ); }
	;

regexp
	/*
	 * In this rule, want_regexp tells yylex that the next thing
	 * is a regexp so it should read up to the closing slash.
	 */
	: '/'
		{ ++want_regexp; }
	   REGEXP '/'
		{
		  want_regexp = 0;
		  $$ = node((NODE *)NULL,Node_regex,(NODE *)mk_re_parse($3, 0));
		  $$ -> re_case = 0;
		  emalloc ($$ -> re_text, char *, strlen($3)+1, "regexp");
		  strcpy ($$ -> re_text, $3);
		}
	;

action
	: l_brace r_brace opt_semi
		{
			/* empty actions are different from missing actions */
			$$ = node ((NODE *) NULL, Node_illegal, (NODE *) NULL);
		}
	| l_brace statements r_brace opt_semi
		{ $$ = $2 ; }
	;

statements
	: statement
		{ $$ = $1; }
	| statements statement
		{
			if ($1 == NULL || $1->type != Node_statement_list)
				$1 = node($1, Node_statement_list,(NODE *)NULL);
	    		$$ = append_right($1,
				node( $2, Node_statement_list, (NODE *)NULL));
	    		yyerrok;
		}
	| error
		{ $$ = NULL; }
	| statements error
		{ $$ = NULL; }
	;

statement_term
	: nls
		{ $<nodetypeval>$ = Node_illegal; }
	| semi opt_nls
		{ $<nodetypeval>$ = Node_illegal; }
	;

	
statement
	: semi opt_nls
		{ $$ = NULL; }
	| l_brace r_brace
		{ $$ = NULL; }
	| l_brace statements r_brace
		{ $$ = $2; }
	| if_statement
		{ $$ = $1; }
	| LEX_WHILE '(' exp r_paren opt_nls statement
		{ $$ = node ($3, Node_K_while, $6); }
	| LEX_DO opt_nls statement LEX_WHILE '(' exp r_paren opt_nls
		{ $$ = node ($6, Node_K_do, $3); }
	| LEX_FOR '(' NAME LEX_IN NAME r_paren opt_nls statement
	  {
		$$ = node ($8, Node_K_arrayfor, make_for_loop(variable($3),
			(NODE *)NULL, variable($5)));
	  }
	| LEX_FOR '(' opt_exp semi exp semi opt_exp r_paren opt_nls statement
	  {
		$$ = node($10, Node_K_for, (NODE *)make_for_loop($3, $5, $7));
	  }
	| LEX_FOR '(' opt_exp semi semi opt_exp r_paren opt_nls statement
	  {
		$$ = node ($9, Node_K_for,
			(NODE *)make_for_loop($3, (NODE *)NULL, $6));
	  }
	| LEX_BREAK statement_term
	   /* for break, maybe we'll have to remember where to break to */
		{ $$ = node ((NODE *)NULL, Node_K_break, (NODE *)NULL); }
	| LEX_CONTINUE statement_term
	   /* similarly */
		{ $$ = node ((NODE *)NULL, Node_K_continue, (NODE *)NULL); }
	| print '(' expression_list r_paren output_redir statement_term
		{ $$ = node ($3, $1, $5); }
	| print opt_rexpression_list output_redir statement_term
		{ $$ = node ($2, $1, $3); }
	| LEX_NEXT
		{ if (! io_allowed) yyerror("next used in BEGIN or END action"); }
	  statement_term
		{ $$ = node ((NODE *)NULL, Node_K_next, (NODE *)NULL); }
	| LEX_EXIT opt_exp statement_term
		{ $$ = node ($2, Node_K_exit, (NODE *)NULL); }
	| LEX_RETURN
		{ if (! can_return) yyerror("return used outside function context"); }
	  opt_exp statement_term
		{ $$ = node ($3, Node_K_return, (NODE *)NULL); }
	| LEX_DELETE NAME '[' expression_list ']' statement_term
		{ $$ = node (variable($2), Node_K_delete, $4); }
	| exp statement_term
		{ $$ = $1; }
	;

print
	: LEX_PRINT
		{ $$ = $1; }
	| LEX_PRINTF
		{ $$ = $1; }
	;

if_statement
	: LEX_IF '(' exp r_paren opt_nls statement
	  {
		$$ = node($3, Node_K_if, 
			node($6, Node_if_branches, (NODE *)NULL));
	  }
	| LEX_IF '(' exp r_paren opt_nls statement
	     LEX_ELSE opt_nls statement
		{ $$ = node ($3, Node_K_if,
				node ($6, Node_if_branches, $9)); }
	;

nls
	: NEWLINE
		{ $<nodetypeval>$ = 0; }
	| nls NEWLINE
		{ $<nodetypeval>$ = 0; }
	;

opt_nls
	: /* empty */
		{ $<nodetypeval>$ = 0; }
	| nls
		{ $<nodetypeval>$ = 0; }
	;

input_redir
	: /* empty */
		{ $$ = NULL; }
	| '<' simp_exp
		{ $$ = node ($2, Node_redirect_input, (NODE *)NULL); }
	;

output_redir
	: /* empty */
		{ $$ = NULL; }
	| '>' exp
		{ $$ = node ($2, Node_redirect_output, (NODE *)NULL); }
	| APPEND_OP exp
		{ $$ = node ($2, Node_redirect_append, (NODE *)NULL); }
	| '|' exp
		{ $$ = node ($2, Node_redirect_pipe, (NODE *)NULL); }
	;

opt_param_list
	: /* empty */
		{ $$ = NULL; }
	| param_list
		{ $$ = $1; }
	;

param_list
	: NAME
		{ $$ = make_param($1); }
	| param_list comma NAME
		{ $$ = append_right($1, make_param($3)); yyerrok; }
	| error
		{ $$ = NULL; }
	| param_list error
		{ $$ = NULL; }
	| param_list comma error
		{ $$ = NULL; }
	;

/* optional expression, as in for loop */
opt_exp
	: /* empty */
		{ $$ = NULL; }
	| exp
		{ $$ = $1; }
	;

opt_rexpression_list
	: /* empty */
		{ $$ = NULL; }
	| rexpression_list
		{ $$ = $1; }
	;

rexpression_list
	: rexp
		{ $$ = node ($1, Node_expression_list, (NODE *)NULL); }
	| rexpression_list comma rexp
	  {
		$$ = append_right($1,
			node( $3, Node_expression_list, (NODE *)NULL));
		yyerrok;
	  }
	| error
		{ $$ = NULL; }
	| rexpression_list error
		{ $$ = NULL; }
	| rexpression_list error rexp
		{ $$ = NULL; }
	| rexpression_list comma error
		{ $$ = NULL; }
	;

opt_expression_list
	: /* empty */
		{ $$ = NULL; }
	| expression_list
		{ $$ = $1; }
	;

expression_list
	: exp
		{ $$ = node ($1, Node_expression_list, (NODE *)NULL); }
	| expression_list comma exp
		{
			$$ = append_right($1,
				node( $3, Node_expression_list, (NODE *)NULL));
			yyerrok;
		}
	| error
		{ $$ = NULL; }
	| expression_list error
		{ $$ = NULL; }
	| expression_list error exp
		{ $$ = NULL; }
	| expression_list comma error
		{ $$ = NULL; }
	;

/* Expressions, not including the comma operator.  */
exp	: variable ASSIGNOP
		{ want_assign = 0; }
		exp
		{ $$ = node ($1, $2, $4); }
	| '(' expression_list r_paren LEX_IN NAME
		{ $$ = node (variable($5), Node_in_array, $2); }
	| exp '|' LEX_GETLINE opt_variable
		{
		  $$ = node ($4, Node_K_getline,
			 node ($1, Node_redirect_pipein, (NODE *)NULL));
		}
	| LEX_GETLINE opt_variable input_redir
		{
		  /* "too painful to do right" */
		  /*
		  if (! io_allowed && $3 == NULL)
			yyerror("non-redirected getline illegal inside BEGIN or END action");
		  */
		  $$ = node ($2, Node_K_getline, $3);
		}
	| exp LEX_AND exp
		{ $$ = node ($1, Node_and, $3); }
	| exp LEX_OR exp
		{ $$ = node ($1, Node_or, $3); }
	| exp MATCHOP exp
		 { $$ = node ($1, $2, $3); }
	| regexp
		{ $$ = $1; }
	| '!' regexp %prec UNARY
		{ $$ = node((NODE *) NULL, Node_nomatch, $2); }
	| exp LEX_IN NAME
		{ $$ = node (variable($3), Node_in_array, $1); }
	| exp RELOP exp
		{ $$ = node ($1, $2, $3); }
	| exp '<' exp
		{ $$ = node ($1, Node_less, $3); }
	| exp '>' exp
		{ $$ = node ($1, Node_greater, $3); }
	| exp '?' exp ':' exp
		{ $$ = node($1, Node_cond_exp, node($3, Node_if_branches, $5));}
	| simp_exp
		{ $$ = $1; }
	| exp exp %prec CONCAT_OP
		{ $$ = node ($1, Node_concat, $2); }
	;

rexp	
	: variable ASSIGNOP
		{ want_assign = 0; }
		rexp
		{ $$ = node ($1, $2, $4); }
	| rexp LEX_AND rexp
		{ $$ = node ($1, Node_and, $3); }
	| rexp LEX_OR rexp
		{ $$ = node ($1, Node_or, $3); }
	| LEX_GETLINE opt_variable input_redir
		{
		  /* "too painful to do right" */
		  /*
		  if (! io_allowed && $3 == NULL)
			yyerror("non-redirected getline illegal inside BEGIN or END action");
		  */
		  $$ = node ($2, Node_K_getline, $3);
		}
	| regexp
		{ $$ = $1; } 
	| '!' regexp %prec UNARY
		{ $$ = node((NODE *) NULL, Node_nomatch, $2); }
	| rexp MATCHOP rexp
		 { $$ = node ($1, $2, $3); }
	| rexp LEX_IN NAME
		{ $$ = node (variable($3), Node_in_array, $1); }
	| rexp RELOP rexp
		{ $$ = node ($1, $2, $3); }
	| rexp '?' rexp ':' rexp
		{ $$ = node($1, Node_cond_exp, node($3, Node_if_branches, $5));}
	| simp_exp
		{ $$ = $1; }
	| rexp rexp %prec CONCAT_OP
		{ $$ = node ($1, Node_concat, $2); }
	;

simp_exp
	: '!' simp_exp %prec UNARY
		{ $$ = node ($2, Node_not,(NODE *) NULL); }
	| '(' exp r_paren
		{ $$ = $2; }
	| LEX_BUILTIN '(' opt_expression_list r_paren
		{ $$ = snode ($3, Node_builtin, $1); }
	| LEX_LENGTH '(' opt_expression_list r_paren
		{ $$ = snode ($3, Node_builtin, $1); }
	| LEX_LENGTH
		{ $$ = snode ((NODE *)NULL, Node_builtin, $1); }
	| FUNC_CALL '(' opt_expression_list r_paren
	  {
		$$ = node ($3, Node_func_call, make_string($1, strlen($1)));
	  }
	| INCREMENT variable
		{ $$ = node ($2, Node_preincrement, (NODE *)NULL); }
	| DECREMENT variable
		{ $$ = node ($2, Node_predecrement, (NODE *)NULL); }
	| variable INCREMENT
		{ $$ = node ($1, Node_postincrement, (NODE *)NULL); }
	| variable DECREMENT
		{ $$ = node ($1, Node_postdecrement, (NODE *)NULL); }
	| variable
		{ $$ = $1; }
	| NUMBER
		{ $$ = $1; }
	| YSTRING
		{ $$ = $1; }

	/* Binary operators in order of decreasing precedence.  */
	| simp_exp '^' simp_exp
		{ $$ = node ($1, Node_exp, $3); }
	| simp_exp '*' simp_exp
		{ $$ = node ($1, Node_times, $3); }
	| simp_exp '/' simp_exp
		{ $$ = node ($1, Node_quotient, $3); }
	| simp_exp '%' simp_exp
		{ $$ = node ($1, Node_mod, $3); }
	| simp_exp '+' simp_exp
		{ $$ = node ($1, Node_plus, $3); }
	| simp_exp '-' simp_exp
		{ $$ = node ($1, Node_minus, $3); }
	| '-' simp_exp    %prec UNARY
		{ $$ = node ($2, Node_unary_minus, (NODE *)NULL); }
	| '+' simp_exp    %prec UNARY
		{ $$ = $2; }
	;

opt_variable
	: /* empty */
		{ $$ = NULL; }
	| variable
		{ $$ = $1; }
	;

variable
	: NAME
		{ want_assign = 1; $$ = variable ($1); }
	| NAME '[' expression_list ']'
		{ want_assign = 1; $$ = node (variable($1), Node_subscript, $3); }
	| '$' simp_exp
		{ want_assign = 1; $$ = node ($2, Node_field_spec, (NODE *)NULL); }
	;

l_brace
	: '{' opt_nls
	;

r_brace
	: '}' opt_nls	{ yyerrok; }
	;

r_paren
	: ')' { $<nodetypeval>$ = Node_illegal; yyerrok; }
	;

opt_semi
	: /* empty */
	| semi
	;

semi
	: ';'	{ yyerrok; }
	;

comma	: ',' opt_nls	{ $<nodetypeval>$ = Node_illegal; yyerrok; }
	;

%%

struct token {
	char *operator;		/* text to match */
	NODETYPE value;		/* node type */
	int class;		/* lexical class */
	short nostrict;		/* ignore if in strict compatibility mode */
	NODE *(*ptr) ();	/* function that implements this keyword */
};

extern NODE
	*do_exp(),	*do_getline(),	*do_index(),	*do_length(),
	*do_sqrt(),	*do_log(),	*do_sprintf(),	*do_substr(),
	*do_split(),	*do_system(),	*do_int(),	*do_close(),
	*do_atan2(),	*do_sin(),	*do_cos(),	*do_rand(),
	*do_srand(),	*do_match(),	*do_tolower(),	*do_toupper(),
	*do_sub(),	*do_gsub();

/* Special functions for debugging */
#ifdef DEBUG
NODE *do_prvars(), *do_bp();
#endif

/* Tokentab is sorted ascii ascending order, so it can be binary searched. */

static struct token tokentab[] = {
	{ "BEGIN",	Node_illegal,		LEX_BEGIN,	0,	0 },
	{ "END",	Node_illegal,		LEX_END,	0,	0 },
	{ "atan2",	Node_builtin,		LEX_BUILTIN,	0,	do_atan2 },
#ifdef DEBUG
	{ "bp",		Node_builtin,		LEX_BUILTIN,	0,	do_bp },
#endif
	{ "break",	Node_K_break,		LEX_BREAK,	0,	0 },
	{ "close",	Node_builtin,		LEX_BUILTIN,	0,	do_close },
	{ "continue",	Node_K_continue,	LEX_CONTINUE,	0,	0 },
	{ "cos",	Node_builtin,		LEX_BUILTIN,	0,	do_cos },
	{ "delete",	Node_K_delete,		LEX_DELETE,	0,	0 },
	{ "do",		Node_K_do,		LEX_DO,		0,	0 },
	{ "else",	Node_illegal,		LEX_ELSE,	0,	0 },
	{ "exit",	Node_K_exit,		LEX_EXIT,	0,	0 },
	{ "exp",	Node_builtin,		LEX_BUILTIN,	0,	do_exp },
	{ "for",	Node_K_for,		LEX_FOR,	0,	0 },
	{ "func",	Node_K_function,	LEX_FUNCTION,	0,	0 },
	{ "function",	Node_K_function,	LEX_FUNCTION,	0,	0 },
	{ "getline",	Node_K_getline,		LEX_GETLINE,	0,	0 },
	{ "gsub",	Node_builtin,		LEX_BUILTIN,	0,	do_gsub },
	{ "if",		Node_K_if,		LEX_IF,		0,	0 },
	{ "in",		Node_illegal,		LEX_IN,		0,	0 },
	{ "index",	Node_builtin,		LEX_BUILTIN,	0,	do_index },
	{ "int",	Node_builtin,		LEX_BUILTIN,	0,	do_int },
	{ "length",	Node_builtin,		LEX_LENGTH,	0,	do_length },
	{ "log",	Node_builtin,		LEX_BUILTIN,	0,	do_log },
	{ "match",	Node_builtin,		LEX_BUILTIN,	0,	do_match },
	{ "next",	Node_K_next,		LEX_NEXT,	0,	0 },
	{ "print",	Node_K_print,		LEX_PRINT,	0,	0 },
	{ "printf",	Node_K_printf,		LEX_PRINTF,	0,	0 },
#ifdef DEBUG
	{ "prvars",	Node_builtin,		LEX_BUILTIN,	0,	do_prvars },
#endif
	{ "rand",	Node_builtin,		LEX_BUILTIN,	0,	do_rand },
	{ "return",	Node_K_return,		LEX_RETURN,	0,	0 },
	{ "sin",	Node_builtin,		LEX_BUILTIN,	0,	do_sin },
	{ "split",	Node_builtin,		LEX_BUILTIN,	0,	do_split },
	{ "sprintf",	Node_builtin,		LEX_BUILTIN,	0,	do_sprintf },
	{ "sqrt",	Node_builtin,		LEX_BUILTIN,	0,	do_sqrt },
	{ "srand",	Node_builtin,		LEX_BUILTIN,	0,	do_srand },
	{ "sub",	Node_builtin,		LEX_BUILTIN,	0,	do_sub },
	{ "substr",	Node_builtin,		LEX_BUILTIN,	0,	do_substr },
	{ "system",	Node_builtin,		LEX_BUILTIN,	0,	do_system },
	{ "tolower",	Node_builtin,		LEX_BUILTIN,	0,	do_tolower },
	{ "toupper",	Node_builtin,		LEX_BUILTIN,	0,	do_toupper },
	{ "while",	Node_K_while,		LEX_WHILE,	0,	0 },
};

static char *token_start;

/* VARARGS0 */
static void
yyerror(char * the_msg, ...)
{
	va_list args;
	char *mesg;
	register char *ptr, *beg;
	char *scan;

	errcount++;
	/* Find the current line in the input file */
	if (! lexptr) {
		beg = "(END OF FILE)";
		ptr = beg + 13;
	} else {
		if (*lexptr == '\n' && lexptr != lexptr_begin)
			--lexptr;
		for (beg = lexptr; beg != lexptr_begin && *beg != '\n'; --beg)
			;
		/* NL isn't guaranteed */
		for (ptr = lexptr; *ptr && *ptr != '\n'; ptr++)
			;
		if (beg != lexptr_begin)
			beg++;
	}
	msg("syntax error near line %d:\n%.*s", lineno, ptr - beg, beg);
	scan = beg;
	while (scan < token_start)
		if (*scan++ == '\t')
			putc('\t', stderr);
		else
			putc(' ', stderr);
	putc('^', stderr);
	putc(' ', stderr);
	va_start(args, the_msg);
	mesg = va_arg(args, char *);
	vfprintf(stderr, mesg, args);
	va_end(args);
	putc('\n', stderr);
	exit(1);
}

/*
 * Parse a C escape sequence.  STRING_PTR points to a variable containing a
 * pointer to the string to parse.  That pointer is updated past the
 * characters we use.  The value of the escape sequence is returned. 
 *
 * A negative value means the sequence \ newline was seen, which is supposed to
 * be equivalent to nothing at all. 
 *
 * If \ is followed by a null character, we return a negative value and leave
 * the string pointer pointing at the null character. 
 *
 * If \ is followed by 000, we return 0 and leave the string pointer after the
 * zeros.  A value of 0 does not mean end of string.  
 */

int
parse_escape(string_ptr)
char **string_ptr;
{
	register int c = *(*string_ptr)++;
	register int i;
	register int count;

	switch (c) {
	case 'a':
		return BELL;
	case 'b':
		return '\b';
	case 'f':
		return '\f';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 't':
		return '\t';
	case 'v':
		return '\v';
	case '\n':
		return -2;
	case 0:
		(*string_ptr)--;
		return -1;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		i = c - '0';
		count = 0;
		while (++count < 3) {
			if ((c = *(*string_ptr)++) >= '0' && c <= '7') {
				i *= 8;
				i += c - '0';
			} else {
				(*string_ptr)--;
				break;
			}
		}
		return i;
	case 'x':
		i = 0;
		while (1) {
			if (isxdigit((c = *(*string_ptr)++))) {
				if (isdigit(c))
					i += c - '0';
				else if (isupper(c))
					i += c - 'A' + 10;
				else
					i += c - 'a' + 10;
			} else {
				(*string_ptr)--;
				break;
			}
		}
		return i;
	default:
		return c;
	}
}

/*
 * Read the input and turn it into tokens. Input is now read from a file
 * instead of from malloc'ed memory. The main program takes a program
 * passed as a command line argument and writes it to a temp file. Otherwise
 * the file name is made available in an external variable.
 */

static int
yylex()
{
	register int c;
	register int namelen;
	register char *tokstart;
	char *tokkey;
	static did_newline = 0;	/* the grammar insists that actions end
				 * with newlines.  This was easier than
				 * hacking the grammar. */
	int seen_e = 0;		/* These are for numbers */
	int seen_point = 0;
	int esc_seen;
	extern char **sourcefile;
	extern int tempsource, numfiles;
	static int file_opened = 0;
	static FILE *fin;
	static char cbuf[BUFSIZ];
	int low, mid, high;
#ifdef DEBUG
	extern int debugging;
#endif

	if (! file_opened) {
		file_opened = 1;
#ifdef DEBUG
		if (debugging) {
			int i;

			for (i = 0; i <= numfiles; i++)
				fprintf (stderr, "sourcefile[%d] = %s\n", i,
						sourcefile[i]);
		}
#endif
	nextfile:
		if ((fin = pathopen (sourcefile[++curinfile])) == NULL)
			fatal("cannot open `%s' for reading (%s)",
				sourcefile[curinfile],
				strerror(errno));
		*(lexptr = cbuf) = '\0';
		/*
		 * immediately unlink the tempfile so that it will
		 * go away cleanly if we bomb.
		 */
		if (tempsource && curinfile == 0)
			(void) unlink (sourcefile[curinfile]);
	}

retry:
	if (! *lexptr)
		if (fgets (cbuf, sizeof cbuf, fin) == NULL) {
			if (fin != NULL)
				fclose (fin);	/* be neat and clean */
			if (curinfile < numfiles)
				goto nextfile;
			return 0;
		} else
			lexptr = lexptr_begin = cbuf;

	if (want_regexp) {
		int in_brack = 0;

		want_regexp = 0;
		token_start = tokstart = lexptr;
		while (c = *lexptr++) {
			switch (c) {
			case '[':
				in_brack = 1;
				break;
			case ']':
				in_brack = 0;
				break;
			case '\\':
				if (*lexptr++ == '\0') {
					yyerror("unterminated regexp ends with \\");
					return ERROR;
				} else if (lexptr[-1] == '\n')
					goto retry;
				break;
			case '/':	/* end of the regexp */
				if (in_brack)
					break;

				lexptr--;
				yylval.sval = tokstart;
				return REGEXP;
			case '\n':
				lineno++;
			case '\0':
				lexptr--;	/* so error messages work */
				yyerror("unterminated regexp");
				return ERROR;
			}
		}
	}

	if (*lexptr == '\n') {
		lexptr++;
		lineno++;
		return NEWLINE;
	}

	while (*lexptr == ' ' || *lexptr == '\t')
		lexptr++;

	token_start = tokstart = lexptr;

	switch (c = *lexptr++) {
	case 0:
		return 0;

	case '\n':
		lineno++;
		return NEWLINE;

	case '#':		/* it's a comment */
		while (*lexptr != '\n' && *lexptr != '\0')
			lexptr++;
		goto retry;

	case '\\':
		if (*lexptr == '\n') {
			lineno++;
			lexptr++;
			goto retry;
		} else
			break;
	case ')':
	case ']':
	case '(':	
	case '[':
	case '$':
	case ';':
	case ':':
	case '?':

		/*
		 * set node type to ILLEGAL because the action should set it
		 * to the right thing 
		 */
		yylval.nodetypeval = Node_illegal;
		return c;

	case '{':
	case ',':
		yylval.nodetypeval = Node_illegal;
		return c;

	case '*':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_assign_times;
			lexptr++;
			return ASSIGNOP;
		} else if (*lexptr == '*') {	/* make ** and **= aliases
						 * for ^ and ^= */
			if (lexptr[1] == '=') {
				yylval.nodetypeval = Node_assign_exp;
				lexptr += 2;
				return ASSIGNOP;
			} else {
				yylval.nodetypeval = Node_illegal;
				lexptr++;
				return '^';
			}
		}
		yylval.nodetypeval = Node_illegal;
		return c;

	case '/':
		if (want_assign && *lexptr == '=') {
			yylval.nodetypeval = Node_assign_quotient;
			lexptr++;
			return ASSIGNOP;
		}
		yylval.nodetypeval = Node_illegal;
		return c;

	case '%':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_assign_mod;
			lexptr++;
			return ASSIGNOP;
		}
		yylval.nodetypeval = Node_illegal;
		return c;

	case '^':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_assign_exp;
			lexptr++;
			return ASSIGNOP;
		}
		yylval.nodetypeval = Node_illegal;
		return c;

	case '+':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_assign_plus;
			lexptr++;
			return ASSIGNOP;
		}
		if (*lexptr == '+') {
			yylval.nodetypeval = Node_illegal;
			lexptr++;
			return INCREMENT;
		}
		yylval.nodetypeval = Node_illegal;
		return c;

	case '!':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_notequal;
			lexptr++;
			return RELOP;
		}
		if (*lexptr == '~') {
			yylval.nodetypeval = Node_nomatch;
			lexptr++;
			return MATCHOP;
		}
		yylval.nodetypeval = Node_illegal;
		return c;

	case '<':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_leq;
			lexptr++;
			return RELOP;
		}
		yylval.nodetypeval = Node_less;
		return c;

	case '=':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_equal;
			lexptr++;
			return RELOP;
		}
		yylval.nodetypeval = Node_assign;
		return ASSIGNOP;

	case '>':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_geq;
			lexptr++;
			return RELOP;
		} else if (*lexptr == '>') {
			yylval.nodetypeval = Node_redirect_append;
			lexptr++;
			return APPEND_OP;
		}
		yylval.nodetypeval = Node_greater;
		return c;

	case '~':
		yylval.nodetypeval = Node_match;
		return MATCHOP;

	case '}':
		/*
		 * Added did newline stuff.  Easier than
		 * hacking the grammar
		 */
		if (did_newline) {
			did_newline = 0;
			return c;
		}
		did_newline++;
		--lexptr;
		return NEWLINE;

	case '"':
		esc_seen = 0;
		while (*lexptr != '\0') {
			switch (*lexptr++) {
			case '\\':
				esc_seen = 1;
				if (*lexptr == '\n')
					yyerror("newline in string");
				if (*lexptr++ != '\0')
					break;
				/* fall through */
			case '\n':
				lexptr--;
				yyerror("unterminated string");
				return ERROR;
			case '"':
				yylval.nodeval = make_str_node(tokstart + 1,
						lexptr-tokstart-2, esc_seen);
				yylval.nodeval->flags |= PERM;
				return YSTRING;
			}
		}
		return ERROR;

	case '-':
		if (*lexptr == '=') {
			yylval.nodetypeval = Node_assign_minus;
			lexptr++;
			return ASSIGNOP;
		}
		if (*lexptr == '-') {
			yylval.nodetypeval = Node_illegal;
			lexptr++;
			return DECREMENT;
		}
		yylval.nodetypeval = Node_illegal;
		return c;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
		/* It's a number */
		for (namelen = 0; (c = tokstart[namelen]) != '\0'; namelen++) {
			switch (c) {
			case '.':
				if (seen_point)
					goto got_number;
				++seen_point;
				break;
			case 'e':
			case 'E':
				if (seen_e)
					goto got_number;
				++seen_e;
				if (tokstart[namelen + 1] == '-' ||
				    tokstart[namelen + 1] == '+')
					namelen++;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				break;
			default:
				goto got_number;
			}
		}

got_number:
		lexptr = tokstart + namelen;
		/*
		yylval.nodeval = make_string(tokstart, namelen);
		(void) force_number(yylval.nodeval);
		*/
		yylval.nodeval = make_number(atof(tokstart));
		yylval.nodeval->flags |= PERM;
		return NUMBER;

	case '&':
		if (*lexptr == '&') {
			yylval.nodetypeval = Node_and;
			while (c = *++lexptr) {
				if (c == '#')
					while ((c = *++lexptr) != '\n'
					       && c != '\0')
						;
				if (c == '\n')
					lineno++;
				else if (! isspace(c))
					break;
			}
			return LEX_AND;
		}
		return ERROR;

	case '|':
		if (*lexptr == '|') {
			yylval.nodetypeval = Node_or;
			while (c = *++lexptr) {
				if (c == '#')
					while ((c = *++lexptr) != '\n'
					       && c != '\0')
						;
				if (c == '\n')
					lineno++;
				else if (! isspace(c))
					break;
			}
			return LEX_OR;
		}
		yylval.nodetypeval = Node_illegal;
		return c;
	}

	if (c != '_' && ! isalpha(c)) {
		yyerror("Invalid char '%c' in expression\n", c);
		return ERROR;
	}

	/* it's some type of name-type-thing.  Find its length */
	for (namelen = 0; is_identchar(tokstart[namelen]); namelen++)
		/* null */ ;
	emalloc(tokkey, char *, namelen+1, "yylex");
	memcpy(tokkey, tokstart, namelen);
	tokkey[namelen] = '\0';

	/* See if it is a special token.  */
	low = 0;
	high = (sizeof (tokentab) / sizeof (tokentab[0])) - 1;
	while (low <= high) {
		int i, c;

		mid = (low + high) / 2;
		c = *tokstart - tokentab[mid].operator[0];
		i = c ? c : strcmp (tokkey, tokentab[mid].operator);

		if (i < 0) {		/* token < mid */
			high = mid - 1;
		} else if (i > 0) {	/* token > mid */
			low = mid + 1;
		} else {
			lexptr = tokstart + namelen;
			if (strict && tokentab[mid].nostrict)
				break;
			if (tokentab[mid].class == LEX_BUILTIN
			    || tokentab[mid].class == LEX_LENGTH)
				yylval.ptrval = tokentab[mid].ptr;
			else
				yylval.nodetypeval = tokentab[mid].value;
			return tokentab[mid].class;
		}
	}

	/* It's a name.  See how long it is.  */
	yylval.sval = tokkey;
	lexptr = tokstart + namelen;
	if (*lexptr == '(')
		return FUNC_CALL;
	else
		return NAME;
}

#ifndef DEFPATH
#ifdef MSDOS
#define DEFPATH	"."
#define ENVSEP	';'
#else
#define DEFPATH	".:/usr/lib/awk:/usr/local/lib/awk"
#define ENVSEP	':'
#endif
#endif

static FILE *
pathopen (file)
char *file;
{
	static char *savepath = DEFPATH;
	static int first = 1;
	char *awkpath, *cp;
	char trypath[BUFSIZ];
	FILE *fp;
#ifdef DEBUG
	extern int debugging;
#endif
	int fd;

	if (strcmp (file, "-") == 0)
		return (stdin);

	if (strict)
		return (fopen (file, "r"));

	if (first) {
		first = 0;
		if ((awkpath = getenv ("AWKPATH")) != NULL && *awkpath)
			savepath = awkpath;	/* used for restarting */
	}
	awkpath = savepath;

	/* some kind of path name, no search */
#ifndef MSDOS
	if (strchr (file, '/') != NULL)
#else
	if (strchr (file, '/') != NULL || strchr (file, '\\') != NULL
			|| strchr (file, ':') != NULL)
#endif
		return ( (fd = devopen (file, "r")) >= 0 ?
				fdopen(fd, "r") :
				NULL);

	do {
		trypath[0] = '\0';
		/* this should take into account limits on size of trypath */
		for (cp = trypath; *awkpath && *awkpath != ENVSEP; )
			*cp++ = *awkpath++;

		if (cp != trypath) {	/* nun-null element in path */
			*cp++ = '/';
			strcpy (cp, file);
		} else
			strcpy (trypath, file);
#ifdef DEBUG
		if (debugging)
			fprintf(stderr, "trying: %s\n", trypath);
#endif
		if ((fd = devopen (trypath, "r")) >= 0
		    && (fp = fdopen(fd, "r")) != NULL)
			return (fp);

		/* no luck, keep going */
		if(*awkpath == ENVSEP && awkpath[1] != '\0')
			awkpath++;	/* skip colon */
	} while (*awkpath);
#ifdef MSDOS
	/*
	 * Under DOS (and probably elsewhere) you might have one of the awk
	 * paths defined, WITHOUT the current working directory in it.
	 * Therefore you should try to open the file in the current directory.
	 */
	return ( (fd = devopen(file, "r")) >= 0 ? fdopen(fd, "r") : NULL);
#else
	return (NULL);
#endif
}

static NODE *
node_common(op)
NODETYPE op;
{
	register NODE *r;
	extern int numfiles;
	extern int tempsource;
	extern char **sourcefile;

	r = newnode(op);
	r->source_line = lineno;
	if (numfiles > -1 && ! tempsource)
		r->source_file = sourcefile[curinfile];
	else
		r->source_file = NULL;
	return r;
}

/*
 * This allocates a node with defined lnode and rnode. 
 * This should only be used by yyparse+co while reading in the program 
 */
NODE *
node(left, op, right)
NODE *left, *right;
NODETYPE op;
{
	register NODE *r;

	r = node_common(op);
	r->lnode = left;
	r->rnode = right;
	return r;
}

/*
 * This allocates a node with defined subnode and proc
 * Otherwise like node()
 */
static NODE *
snode(subn, op, procp)
NODETYPE op;
NODE *(*procp) ();
NODE *subn;
{
	register NODE *r;

	r = node_common(op);
	r->subnode = subn;
	r->proc = procp;
	return r;
}

/*
 * This allocates a Node_line_range node with defined condpair and
 * zeroes the trigger word to avoid the temptation of assuming that calling
 * 'node( foo, Node_line_range, 0)' will properly initialize 'triggered'. 
 */
/* Otherwise like node() */
static NODE *
mkrangenode(cpair)
NODE *cpair;
{
	register NODE *r;

	r = newnode(Node_line_range);
	r->condpair = cpair;
	r->triggered = 0;
	return r;
}

/* Build a for loop */
static NODE *
make_for_loop(init, cond, incr)
NODE *init, *cond, *incr;
{
	register FOR_LOOP_HEADER *r;
	NODE *n;

	emalloc(r, FOR_LOOP_HEADER *, sizeof(FOR_LOOP_HEADER), "make_for_loop");
	n = newnode(Node_illegal);
	r->init = init;
	r->cond = cond;
	r->incr = incr;
	n->sub.nodep.r.hd = r;
	return n;
}

/*
 * Install a name in the hash table specified, even if it is already there.
 * Name stops with first non alphanumeric. Caller must check against
 * redefinition if that is desired. 
 */
NODE *
install(table, name, value)
NODE **table;
char *name;
NODE *value;
{
	register NODE *hp;
	register int len, bucket;
	register char *p;

	len = 0;
	p = name;
	while (is_identchar(*p))
		p++;
	len = p - name;

	hp = newnode(Node_hashnode);
	bucket = hashf(name, len, HASHSIZE);
	hp->hnext = table[bucket];
	table[bucket] = hp;
	hp->hlength = len;
	hp->hvalue = value;
	emalloc(hp->hname, char *, len + 1, "install");
	memcpy(hp->hname, name, len);
	hp->hname[len] = '\0';
	return hp->hvalue;
}

/*
 * find the most recent hash node for name name (ending with first
 * non-identifier char) installed by install 
 */
NODE *
lookup(table, name)
NODE **table;
char *name;
{
	register char *bp;
	register NODE *bucket;
	register int len;

	for (bp = name; is_identchar(*bp); bp++)
		;
	len = bp - name;
	bucket = table[hashf(name, len, HASHSIZE)];
	while (bucket) {
		if (bucket->hlength == len && STREQN(bucket->hname, name, len))
			return bucket->hvalue;
		bucket = bucket->hnext;
	}
	return NULL;
}

#define HASHSTEP(old, c) ((old << 1) + c)
#define MAKE_POS(v) (v & ~0x80000000)	/* make number positive */

/*
 * return hash function on name.
 */
static int
hashf(name, len, hashsize)
register char *name;
register int len;
int hashsize;
{
	register int r = 0;

	while (len--)
		r = HASHSTEP(r, *name++);

	r = MAKE_POS(r) % hashsize;
	return r;
}

/*
 * Add new to the rightmost branch of LIST.  This uses n^2 time, so we make
 * a simple attempt at optimizing it.
 */
static NODE *
append_right(list, new)
NODE *list, *new;

{
	register NODE *oldlist;
	static NODE *savefront = NULL, *savetail = NULL;

	oldlist = list;
	if (savefront == oldlist) {
		savetail = savetail->rnode = new;
		return oldlist;
	} else
		savefront = oldlist;
	while (list->rnode != NULL)
		list = list->rnode;
	savetail = list->rnode = new;
	return oldlist;
}

/*
 * check if name is already installed;  if so, it had better have Null value,
 * in which case def is added as the value. Otherwise, install name with def
 * as value. 
 */
static void
func_install(params, def)
NODE *params;
NODE *def;
{
	NODE *r;

	pop_params(params->rnode);
	pop_var(params, 0);
	r = lookup(variables, params->param);
	if (r != NULL) {
		fatal("function name `%s' previously defined", params->param);
	} else
		(void) install(variables, params->param,
			node(params, Node_func, def));
}

static void
pop_var(np, freeit)
NODE *np;
int freeit;
{
	register char *bp;
	register NODE *bucket, **save;
	register int len;
	char *name;

	name = np->param;
	for (bp = name; is_identchar(*bp); bp++)
		;
	len = bp - name;
	save = &(variables[hashf(name, len, HASHSIZE)]);
	bucket = *save;
	while (bucket != NULL) {
	        NODE *next = bucket->hnext;
	    
/*	for (bucket = *save; bucket; bucket = bucket->hnext) {
*/
	    
		if (len == bucket->hlength && STREQN(bucket->hname, name, len)) {
			*save = bucket->hnext;
			save = &(bucket->hnext);
			free(bucket->hname);
			freenode(bucket);
			if (freeit)
				free(np->param);
			return;
		} else {
			save = &(bucket->hnext);
		}
		bucket = next;
	}
}

static void
pop_params(params)
NODE *params;
{
	register NODE *np;

	for (np = params; np != NULL; np = np->rnode)
		pop_var(np, 1);
}

static NODE *
make_param(name)
char *name;
{
	NODE *r;

	r = newnode(Node_param_list);
	r->param = name;
	r->rnode = NULL;
	r->param_cnt = param_counter++;
	return (install(variables, name, r));
}

/* Name points to a variable name.  Make sure its in the symbol table */
NODE *
variable(name)
char *name;
{
	register NODE *r;

	if ((r = lookup(variables, name)) == NULL)
		r = install(variables, name,
			node(Nnull_string, Node_var, (NODE *) NULL));
	return r;
}
