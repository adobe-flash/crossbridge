/*
 * eval.c - gawk parse tree interpreter 
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

#include "gawk.h"

extern void do_print();
extern void do_printf();
extern NODE *do_match();
extern NODE *do_sub();
extern NODE *do_getline();
extern NODE *concat_exp();
extern int in_array();
extern void do_delete();
extern double pow();

static int eval_condition();
static NODE *op_assign();
static NODE *func_call();
static NODE *match_op();

NODE *_t;		/* used as a temporary in macros */
#ifdef MSDOS
double _msc51bug;	/* to get around a bug in MSC 5.1 */
#endif
NODE *ret_node;

/* More of that debugging stuff */
#ifdef	DEBUG
#define DBG_P(X) print_debug X
#else
#define DBG_P(X)
#endif

/* Macros and variables to save and restore function and loop bindings */
/*
 * the val variable allows return/continue/break-out-of-context to be
 * caught and diagnosed
 */
#define PUSH_BINDING(stack, x, val) (memcpy ((char *)(stack), (char *)(x), sizeof (jmp_buf)), val++)
#define RESTORE_BINDING(stack, x, val) (memcpy ((char *)(x), (char *)(stack), sizeof (jmp_buf)), val--)

static jmp_buf loop_tag;	/* always the current binding */
static int loop_tag_valid = 0;	/* nonzero when loop_tag valid */
static int func_tag_valid = 0;
static jmp_buf func_tag;
extern int exiting, exit_val;

/*
 * This table is used by the regexp routines to do case independant
 * matching. Basically, every ascii character maps to itself, except
 * uppercase letters map to lower case ones. This table has 256
 * entries, which may be overkill. Note also that if the system this
 * is compiled on doesn't use 7-bit ascii, casetable[] should not be
 * defined to the linker, so gawk should not load.
 *
 * Do NOT make this array static, it is used in several spots, not
 * just in this file.
 */
#if 'a' == 97	/* it's ascii */
char casetable[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	/* ' '     '!'     '"'     '#'     '$'     '%'     '&'     ''' */
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	/* '('     ')'     '*'     '+'     ','     '-'     '.'     '/' */
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	/* '0'     '1'     '2'     '3'     '4'     '5'     '6'     '7' */
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	/* '8'     '9'     ':'     ';'     '<'     '='     '>'     '?' */
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	/* '@'     'A'     'B'     'C'     'D'     'E'     'F'     'G' */
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	/* 'H'     'I'     'J'     'K'     'L'     'M'     'N'     'O' */
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	/* 'P'     'Q'     'R'     'S'     'T'     'U'     'V'     'W' */
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	/* 'X'     'Y'     'Z'     '['     '\'     ']'     '^'     '_' */
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	/* '`'     'a'     'b'     'c'     'd'     'e'     'f'     'g' */
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	/* 'h'     'i'     'j'     'k'     'l'     'm'     'n'     'o' */
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	/* 'p'     'q'     'r'     's'     't'     'u'     'v'     'w' */
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	/* 'x'     'y'     'z'     '{'     '|'     '}'     '~' */
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};
#else
#include "You lose. You will need a translation table for your character set."
#endif

/*
 * Tree is a bunch of rules to run. Returns zero if it hit an exit()
 * statement 
 */
int
interpret(tree)
NODE *tree;
{
	volatile jmp_buf loop_tag_stack; /* shallow binding stack for loop_tag */
	static jmp_buf rule_tag;/* tag the rule currently being run, for NEXT
				 * and EXIT statements.  It is static because
				 * there are no nested rules */
	register NODE *t = NULL;/* temporary */
	volatile NODE **lhs;	/* lhs == Left Hand Side for assigns, etc */
	volatile struct search *l;	/* For array_for */
	volatile NODE *stable_tree;

	if (tree == NULL)
		return 1;
	sourceline = tree->source_line;
	source = tree->source_file;
	switch (tree->type) {
	case Node_rule_list:
		for (t = tree; t != NULL; t = t->rnode) {
			tree = t->lnode;
		/* FALL THROUGH */
	case Node_rule_node:
			sourceline = tree->source_line;
			source = tree->source_file;
			switch (setjmp(rule_tag)) {
			case 0:	/* normal non-jump */
				/* test pattern, if any */
				if (tree->lnode == NULL 
				    || eval_condition(tree->lnode)) {
					DBG_P(("Found a rule", tree->rnode));
					if (tree->rnode == NULL) {
						/*
						 * special case: pattern with
						 * no action is equivalent to
						 * an action of {print}
						 */
						NODE printnode;

						printnode.type = Node_K_print;
						printnode.lnode = NULL;
						printnode.rnode = NULL;
						do_print(&printnode);
					} else if (tree->rnode->type == Node_illegal) {
						/*
						 * An empty statement
						 * (``{ }'') is different
						 * from a missing statement.
						 * A missing statement is
						 * equal to ``{ print }'' as
						 * above, but an empty
						 * statement is as in C, do
						 * nothing.
						 */
					} else
						(void) interpret(tree->rnode);
				}
				break;
			case TAG_CONTINUE:	/* NEXT statement */
				return 1;
			case TAG_BREAK:
				return 0;
			default:
				cant_happen();
			}
			if (t == NULL)
				break;
		}
		break;

	case Node_statement_list:
		for (t = tree; t != NULL; t = t->rnode) {
			DBG_P(("Statements", t->lnode));
			(void) interpret(t->lnode);
		}
		break;

	case Node_K_if:
		DBG_P(("IF", tree->lnode));
		if (eval_condition(tree->lnode)) {
			DBG_P(("True", tree->rnode->lnode));
			(void) interpret(tree->rnode->lnode);
		} else {
			DBG_P(("False", tree->rnode->rnode));
			(void) interpret(tree->rnode->rnode);
		}
		break;

	case Node_K_while:
		PUSH_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);

		DBG_P(("WHILE", tree->lnode));
		stable_tree = tree;
		while (eval_condition(stable_tree->lnode)) {
			switch (setjmp(loop_tag)) {
			case 0:	/* normal non-jump */
				DBG_P(("DO", stable_tree->rnode));
				(void) interpret(stable_tree->rnode);
				break;
			case TAG_CONTINUE:	/* continue statement */
				break;
			case TAG_BREAK:	/* break statement */
				RESTORE_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
				return 1;
			default:
				cant_happen();
			}
		}
		RESTORE_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
		break;

	case Node_K_do:
		PUSH_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
		stable_tree = tree;
		do {
			switch (setjmp(loop_tag)) {
			case 0:	/* normal non-jump */
				DBG_P(("DO", stable_tree->rnode));
				(void) interpret(stable_tree->rnode);
				break;
			case TAG_CONTINUE:	/* continue statement */
				break;
			case TAG_BREAK:	/* break statement */
				RESTORE_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
				return 1;
			default:
				cant_happen();
			}
			DBG_P(("WHILE", stable_tree->lnode));
		} while (eval_condition(stable_tree->lnode));
		RESTORE_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
		break;

	case Node_K_for:
		PUSH_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
		DBG_P(("FOR", tree->forloop->init));
		(void) interpret(tree->forloop->init);
		DBG_P(("FOR.WHILE", tree->forloop->cond));
		stable_tree = tree;
		while (eval_condition(stable_tree->forloop->cond)) {
			switch (setjmp(loop_tag)) {
			case 0:	/* normal non-jump */
				DBG_P(("FOR.DO", stable_tree->lnode));
				(void) interpret(stable_tree->lnode);
				/* fall through */
			case TAG_CONTINUE:	/* continue statement */
				DBG_P(("FOR.INCR", stable_tree->forloop->incr));
				(void) interpret(stable_tree->forloop->incr);
				break;
			case TAG_BREAK:	/* break statement */
				RESTORE_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
				return 1;
			default:
				cant_happen();
			}
		}
		RESTORE_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
		break;

	case Node_K_arrayfor:
#define hakvar forloop->init
#define arrvar forloop->incr
		PUSH_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
		DBG_P(("AFOR.VAR", tree->hakvar));
		lhs = (volatile NODE **) get_lhs(tree->hakvar, 1);
		t = tree->arrvar;
		if (t->type == Node_param_list)
			t = stack_ptr[t->param_cnt];
		stable_tree = tree;
		for (l = assoc_scan(t); l; l = assoc_next((struct search *)l)) {
			deref = *((NODE **) lhs);
			do_deref();
			*lhs = dupnode(l->retval);
			if (field_num == 0)
				set_record(fields_arr[0]->stptr,
				    fields_arr[0]->stlen);
			DBG_P(("AFOR.NEXTIS", *lhs));
			switch (setjmp(loop_tag)) {
			case 0:
				DBG_P(("AFOR.DO", stable_tree->lnode));
				(void) interpret(stable_tree->lnode);
			case TAG_CONTINUE:
				break;

			case TAG_BREAK:
				RESTORE_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
				field_num = -1;
				return 1;
			default:
				cant_happen();
			}
		}
		field_num = -1;
		RESTORE_BINDING(loop_tag_stack, loop_tag, loop_tag_valid);
		break;

	case Node_K_break:
		DBG_P(("BREAK", NULL));
		if (loop_tag_valid == 0)
			fatal("unexpected break");
		longjmp(loop_tag, TAG_BREAK);
		break;

	case Node_K_continue:
		DBG_P(("CONTINUE", NULL));
		if (loop_tag_valid == 0)
			fatal("unexpected continue");
		longjmp(loop_tag, TAG_CONTINUE);
		break;

	case Node_K_print:
		DBG_P(("PRINT", tree));
		do_print(tree);
		break;

	case Node_K_printf:
		DBG_P(("PRINTF", tree));
		do_printf(tree);
		break;

	case Node_K_next:
		DBG_P(("NEXT", NULL));
		longjmp(rule_tag, TAG_CONTINUE);
		break;

	case Node_K_exit:
		/*
		 * In A,K,&W, p. 49, it says that an exit statement "...
		 * causes the program to behave as if the end of input had
		 * occurred; no more input is read, and the END actions, if
		 * any are executed." This implies that the rest of the rules
		 * are not done. So we immediately break out of the main loop.
		 */
		DBG_P(("EXIT", NULL));
		exiting = 1;
		if (tree) {
			t = tree_eval(tree->lnode);
			exit_val = (int) force_number(t);
		}
		free_temp(t);
		longjmp(rule_tag, TAG_BREAK);
		break;

	case Node_K_return:
		DBG_P(("RETURN", NULL));
		t = tree_eval(tree->lnode);
		ret_node = dupnode(t);
		free_temp(t);
		longjmp(func_tag, TAG_RETURN);
		break;

	default:
		/*
		 * Appears to be an expression statement.  Throw away the
		 * value. 
		 */
		DBG_P(("E", NULL));
		t = tree_eval(tree);
		free_temp(t);
		break;
	}
	return 1;
}

/* evaluate a subtree, allocating strings on a temporary stack. */

NODE *
r_tree_eval(tree)
NODE *tree;
{
	register NODE *r, *t1, *t2;	/* return value & temporary subtrees */
	int i;
	register NODE **lhs;
	int di;
	AWKNUM x, x2;
	long lx;
	extern NODE **fields_arr;

	source = tree->source_file;
	sourceline = tree->source_line;
	switch (tree->type) {
	case Node_and:
		DBG_P(("AND", tree));
		return tmp_number((AWKNUM) (eval_condition(tree->lnode)
					    && eval_condition(tree->rnode)));

	case Node_or:
		DBG_P(("OR", tree));
		return tmp_number((AWKNUM) (eval_condition(tree->lnode)
					    || eval_condition(tree->rnode)));

	case Node_not:
		DBG_P(("NOT", tree));
		return tmp_number((AWKNUM) ! eval_condition(tree->lnode));

		/* Builtins */
	case Node_builtin:
		DBG_P(("builtin", tree));
		return ((*tree->proc) (tree->subnode));

	case Node_K_getline:
		DBG_P(("GETLINE", tree));
		return (do_getline(tree));

	case Node_in_array:
		DBG_P(("IN_ARRAY", tree));
		return tmp_number((AWKNUM) in_array(tree->lnode, tree->rnode));

	case Node_func_call:
		DBG_P(("func_call", tree));
		return func_call(tree->rnode, tree->lnode);

	case Node_K_delete:
		DBG_P(("DELETE", tree));
		do_delete(tree->lnode, tree->rnode);
		return Nnull_string;

		/* unary operations */

	case Node_var:
	case Node_var_array:
	case Node_param_list:
	case Node_subscript:
	case Node_field_spec:
		DBG_P(("var_type ref", tree));
		lhs = get_lhs(tree, 0);
		field_num = -1;
		deref = 0;
		return *lhs;

	case Node_unary_minus:
		DBG_P(("UMINUS", tree));
		t1 = tree_eval(tree->subnode);
		x = -force_number(t1);
		free_temp(t1);
		return tmp_number(x);

	case Node_cond_exp:
		DBG_P(("?:", tree));
		if (eval_condition(tree->lnode)) {
			DBG_P(("True", tree->rnode->lnode));
			return tree_eval(tree->rnode->lnode);
		}
		DBG_P(("False", tree->rnode->rnode));
		return tree_eval(tree->rnode->rnode);

	case Node_match:
	case Node_nomatch:
	case Node_regex:
		DBG_P(("[no]match_op", tree));
		return match_op(tree);

	case Node_func:
		fatal("function `%s' called with space between name and (,\n%s",
			tree->lnode->param,
			"or used in other expression context");

	/* assignments */
	case Node_assign:
		DBG_P(("ASSIGN", tree));
		r = tree_eval(tree->rnode);
		lhs = get_lhs(tree->lnode, 1);
		*lhs = dupnode(r);
		free_temp(r);
		do_deref();
		if (field_num == 0)
			set_record(fields_arr[0]->stptr, fields_arr[0]->stlen);
		field_num = -1;
		return *lhs;

	/* other assignment types are easier because they are numeric */
	case Node_preincrement:
	case Node_predecrement:
	case Node_postincrement:
	case Node_postdecrement:
	case Node_assign_exp:
	case Node_assign_times:
	case Node_assign_quotient:
	case Node_assign_mod:
	case Node_assign_plus:
	case Node_assign_minus:
		return op_assign(tree);
	default:
		break;	/* handled below */
	}

	/* evaluate subtrees in order to do binary operation, then keep going */
	t1 = tree_eval(tree->lnode);
	t2 = tree_eval(tree->rnode);

	switch (tree->type) {
	case Node_concat:
		DBG_P(("CONCAT", tree));
		t1 = force_string(t1);
		t2 = force_string(t2);

		r = newnode(Node_val);
		r->flags |= (STR|TEMP);
		r->stlen = t1->stlen + t2->stlen;
		r->stref = 1;
		emalloc(r->stptr, char *, r->stlen + 1, "tree_eval");
		memcpy(r->stptr, t1->stptr, t1->stlen);
		memcpy(r->stptr + t1->stlen, t2->stptr, t2->stlen + 1);
		free_temp(t1);
		free_temp(t2);
		return r;

	case Node_geq:
	case Node_leq:
	case Node_greater:
	case Node_less:
	case Node_notequal:
	case Node_equal:
		di = cmp_nodes(t1, t2);
		free_temp(t1);
		free_temp(t2);
		switch (tree->type) {
		case Node_equal:
			DBG_P(("EQUAL", tree));
			return tmp_number((AWKNUM) (di == 0));
		case Node_notequal:
			DBG_P(("NOT_EQUAL", tree));
			return tmp_number((AWKNUM) (di != 0));
		case Node_less:
			DBG_P(("LESS_THAN", tree));
			return tmp_number((AWKNUM) (di < 0));
		case Node_greater:
			DBG_P(("GREATER_THAN", tree));
			return tmp_number((AWKNUM) (di > 0));
		case Node_leq:
			DBG_P(("LESS_THAN_EQUAL", tree));
			return tmp_number((AWKNUM) (di <= 0));
		case Node_geq:
			DBG_P(("GREATER_THAN_EQUAL", tree));
			return tmp_number((AWKNUM) (di >= 0));
		default:
			cant_happen();
		}
		break;
	default:
		break;	/* handled below */
	}

	(void) force_number(t1);
	(void) force_number(t2);

	switch (tree->type) {
	case Node_exp:
		DBG_P(("EXPONENT", tree));
		if ((lx = t2->numbr) == t2->numbr) {	/* integer exponent */
			if (lx == 0)
				x = 1;
			else if (lx == 1)
				x = t1->numbr;
			else {
				/* doing it this way should be more precise */
				for (x = x2 = t1->numbr; --lx; )
					x *= x2;
			}
		} else
			x = pow((double) t1->numbr, (double) t2->numbr);
		free_temp(t1);
		free_temp(t2);
		return tmp_number(x);

	case Node_times:
		DBG_P(("MULT", tree));
		x = t1->numbr * t2->numbr;
		free_temp(t1);
		free_temp(t2);
		return tmp_number(x);

	case Node_quotient:
		DBG_P(("DIVIDE", tree));
		x = t2->numbr;
		free_temp(t2);
		if (x == (AWKNUM) 0)
			fatal("division by zero attempted");
			/* NOTREACHED */
		else {
			x = t1->numbr / x;
			free_temp(t1);
			return tmp_number(x);
		}

	case Node_mod:
		DBG_P(("MODULUS", tree));
		x = t2->numbr;
		free_temp(t2);
		if (x == (AWKNUM) 0)
			fatal("division by zero attempted in mod");
			/* NOTREACHED */
		lx = t1->numbr / x;	/* assignment to long truncates */
		x2 = lx * x;
		x = t1->numbr - x2;
		free_temp(t1);
		return tmp_number(x);

	case Node_plus:
		DBG_P(("PLUS", tree));
		x = t1->numbr + t2->numbr;
		free_temp(t1);
		free_temp(t2);
		return tmp_number(x);

	case Node_minus:
		DBG_P(("MINUS", tree));
		x = t1->numbr - t2->numbr;
		free_temp(t1);
		free_temp(t2);
		return tmp_number(x);

	default:
		fatal("illegal type (%d) in tree_eval", tree->type);
	}
	return 0;
}

/*
 * This makes numeric operations slightly more efficient. Just change the
 * value of a numeric node, if possible 
 */
void
assign_number(ptr, value)
NODE **ptr;
AWKNUM value;
{
	extern NODE *deref;
	register NODE *n = *ptr;

#ifdef DEBUG
	if (n->type != Node_val)
		cant_happen();
#endif
	if (n == Nnull_string) {
		*ptr = make_number(value);
		deref = 0;
		return;
	}
	if (n->stref > 1) {
		*ptr = make_number(value);
		return;
	}
	if ((n->flags & STR) && (n->flags & (MALLOC|TEMP)))
		free(n->stptr);
	n->numbr = value;
	n->flags |= (NUM|NUMERIC);
	n->flags &= ~STR;
	n->stref = 0;
	deref = 0;
}


/* Is TREE true or false?  Returns 0==false, non-zero==true */
static int
eval_condition(tree)
NODE *tree;
{
	register NODE *t1;
	int ret;

	if (tree == NULL)	/* Null trees are the easiest kinds */
		return 1;
	if (tree->type == Node_line_range) {
		/*
		 * Node_line_range is kind of like Node_match, EXCEPT: the
		 * lnode field (more properly, the condpair field) is a node
		 * of a Node_cond_pair; whether we evaluate the lnode of that
		 * node or the rnode depends on the triggered word.  More
		 * precisely:  if we are not yet triggered, we tree_eval the
		 * lnode; if that returns true, we set the triggered word. 
		 * If we are triggered (not ELSE IF, note), we tree_eval the
		 * rnode, clear triggered if it succeeds, and perform our
		 * action (regardless of success or failure).  We want to be
		 * able to begin and end on a single input record, so this
		 * isn't an ELSE IF, as noted above.
		 */
		if (!tree->triggered)
			if (!eval_condition(tree->condpair->lnode))
				return 0;
			else
				tree->triggered = 1;
		/* Else we are triggered */
		if (eval_condition(tree->condpair->rnode))
			tree->triggered = 0;
		return 1;
	}

	/*
	 * Could just be J.random expression. in which case, null and 0 are
	 * false, anything else is true 
	 */

	t1 = tree_eval(tree);
	if (t1->flags & NUMERIC)
		ret = t1->numbr != 0.0;
	else
		ret = t1->stlen != 0;
	free_temp(t1);
	return ret;
}

int
cmp_nodes(t1, t2)
NODE *t1, *t2;
{
	AWKNUM d;
	AWKNUM d1;
	AWKNUM d2;
	int ret;
	int len1, len2;

	if (t1 == t2)
		return 0;
	d1 = force_number(t1);
	d2 = force_number(t2);
	if ((t1->flags & NUMERIC) && (t2->flags & NUMERIC)) {
		d = d1 - d2;
		if (d == 0.0)	/* from profiling, this is most common */
			return 0;
		if (d > 0.0)
			return 1;
		return -1;
	}
	t1 = force_string(t1);
	t2 = force_string(t2);
	len1 = t1->stlen;
	len2 = t2->stlen;
	if (len1 == 0) {
		if (len2 == 0)
			return 0;
		else
			return -1;
	} else if (len2 == 0)
		return 1;
	ret = memcmp(t1->stptr, t2->stptr, len1 <= len2 ? len1 : len2);
	if (ret == 0 && len1 != len2)
		return len1 < len2 ? -1: 1;
	return ret;
}

static NODE *
op_assign(tree)
NODE *tree;
{
	AWKNUM rval, lval;
	NODE **lhs;
	AWKNUM t1, t2;
	long ltemp;
	NODE *tmp;

	lhs = get_lhs(tree->lnode, 1);
	lval = force_number(*lhs);

	switch(tree->type) {
	case Node_preincrement:
	case Node_predecrement:
		DBG_P(("+-X", tree));
		assign_number(lhs,
		    lval + (tree->type == Node_preincrement ? 1.0 : -1.0));
		do_deref();
		if (field_num == 0)
			set_record(fields_arr[0]->stptr, fields_arr[0]->stlen);
		field_num = -1;
		return *lhs;

	case Node_postincrement:
	case Node_postdecrement:
		DBG_P(("X+-", tree));
		assign_number(lhs,
		    lval + (tree->type == Node_postincrement ? 1.0 : -1.0));
		do_deref();
		if (field_num == 0)
			set_record(fields_arr[0]->stptr, fields_arr[0]->stlen);
		field_num = -1;
		return tmp_number(lval);
	default:
		break;	/* handled below */
	}

	tmp = tree_eval(tree->rnode);
	rval = force_number(tmp);
	free_temp(tmp);
	switch(tree->type) {
	case Node_assign_exp:
		DBG_P(("ASSIGN_exp", tree));
		if ((ltemp = rval) == rval) {	/* integer exponent */
			if (ltemp == 0)
				assign_number(lhs, (AWKNUM) 1);
			else if (ltemp == 1)
				assign_number(lhs, lval);
			else {
				/* doing it this way should be more precise */
				for (t1 = t2 = lval; --ltemp; )
					t1 *= t2;
				assign_number(lhs, t1);
			}
		} else
			assign_number(lhs, (AWKNUM) pow((double) lval, (double) rval));
		break;

	case Node_assign_times:
		DBG_P(("ASSIGN_times", tree));
		assign_number(lhs, lval * rval);
		break;

	case Node_assign_quotient:
		DBG_P(("ASSIGN_quotient", tree));
		if (rval == (AWKNUM) 0)
			fatal("division by zero attempted in /=");
		assign_number(lhs, lval / rval);
		break;

	case Node_assign_mod:
		DBG_P(("ASSIGN_mod", tree));
		if (rval == (AWKNUM) 0)
			fatal("division by zero attempted in %=");
		ltemp = lval / rval;	/* assignment to long truncates */
		t1 = ltemp * rval;
		t2 = lval - t1;
		assign_number(lhs, t2);
		break;

	case Node_assign_plus:
		DBG_P(("ASSIGN_plus", tree));
		assign_number(lhs, lval + rval);
		break;

	case Node_assign_minus:
		DBG_P(("ASSIGN_minus", tree));
		assign_number(lhs, lval - rval);
		break;
	default:
		cant_happen();
	}
	do_deref();
	if (field_num == 0)
		set_record(fields_arr[0]->stptr, fields_arr[0]->stlen);
	field_num = -1;
	return *lhs;
}

NODE **stack_ptr;

static NODE *
func_call(name, arg_list)
NODE *name;		/* name is a Node_val giving function name */
NODE *arg_list;		/* Node_expression_list of calling args. */
{
	register NODE *arg, *argp, *r;
	NODE *n, *f;
	volatile jmp_buf func_tag_stack;
	volatile jmp_buf loop_tag_stack;
	volatile int save_loop_tag_valid = 0;
	volatile NODE **save_stack, *save_ret_node;
	NODE **local_stack, **sp;
	int count;
	extern NODE *ret_node;

	/*
	 * retrieve function definition node
	 */
	f = lookup(variables, name->stptr);
	if (!f || f->type != Node_func)
		fatal("function `%s' not defined", name->stptr);
#ifdef FUNC_TRACE
	fprintf(stderr, "function %s called\n", name->stptr);
#endif
	count = f->lnode->param_cnt;
	emalloc(local_stack, NODE **, count * sizeof(NODE *), "func_call");
	sp = local_stack;

	/*
	 * for each calling arg. add NODE * on stack
	 */
	for (argp = arg_list; count && argp != NULL; argp = argp->rnode) {
		arg = argp->lnode;
		r = newnode(Node_var);
		/*
		 * call by reference for arrays; see below also
		 */
		if (arg->type == Node_param_list)
			arg = stack_ptr[arg->param_cnt];
		if (arg->type == Node_var_array)
			*r = *arg;
		else {
			n = tree_eval(arg);
			r->lnode = dupnode(n);
			r->rnode = (NODE *) NULL;
			free_temp(n);
  		}
		*sp++ = r;
		count--;
	}
	if (argp != NULL)	/* left over calling args. */
		warning(
		    "function `%s' called with more arguments than declared",
		    name->stptr);
	/*
	 * add remaining params. on stack with null value
	 */
	while (count-- > 0) {
		r = newnode(Node_var);
		r->lnode = Nnull_string;
		r->rnode = (NODE *) NULL;
		*sp++ = r;
	}

	/*
	 * Execute function body, saving context, as a return statement
	 * will longjmp back here.
	 *
	 * Have to save and restore the loop_tag stuff so that a return
	 * inside a loop in a function body doesn't scrog any loops going
	 * on in the main program.  We save the necessary info in variables
	 * local to this function so that function nesting works OK.
	 * We also only bother to save the loop stuff if we're in a loop
	 * when the function is called.
	 */
	if (loop_tag_valid) {
		int junk = 0;

		save_loop_tag_valid = (volatile int) loop_tag_valid;
		PUSH_BINDING(loop_tag_stack, loop_tag, junk);
		loop_tag_valid = 0;
	}
	save_stack = (volatile NODE **) stack_ptr;
	stack_ptr = local_stack;
	PUSH_BINDING(func_tag_stack, func_tag, func_tag_valid);
	save_ret_node = (volatile NODE *) ret_node;
	ret_node = Nnull_string;	/* default return value */
	if (setjmp(func_tag) == 0)
		(void) interpret(f->rnode);

	r = ret_node;
	ret_node = (NODE *) save_ret_node;
	RESTORE_BINDING(func_tag_stack, func_tag, func_tag_valid);
	stack_ptr = (NODE **) save_stack;

	/*
	 * here, we pop each parameter and check whether
	 * it was an array.  If so, and if the arg. passed in was
	 * a simple variable, then the value should be copied back.
	 * This achieves "call-by-reference" for arrays.
	 */
	sp = local_stack;
	count = f->lnode->param_cnt;
	for (argp = arg_list; count > 0 && argp != NULL; argp = argp->rnode) {
		arg = argp->lnode;
		n = *sp++;
		if (arg->type == Node_var && n->type == Node_var_array) {
			arg->var_array = n->var_array;
			arg->type = Node_var_array;
		}
		deref = n->lnode;
		do_deref();
		freenode(n);
		count--;
	}
	while (count-- > 0) {
		n = *sp++;
		deref = n->lnode;
		do_deref();
		freenode(n);
	}
	free((char *) local_stack);

	/* Restore the loop_tag stuff if necessary. */
	if (save_loop_tag_valid) {
		int junk = 0;

		loop_tag_valid = (int) save_loop_tag_valid;
		RESTORE_BINDING(loop_tag_stack, loop_tag, junk);
	}

	if (!(r->flags & PERM))
		r->flags |= TEMP;
	return r;
}

/*
 * This returns a POINTER to a node pointer. get_lhs(ptr) is the current
 * value of the var, or where to store the var's new value 
 */

NODE **
get_lhs(ptr, assign)
NODE *ptr;
int assign;		/* this is being called for the LHS of an assign. */
{
	register NODE **aptr;
	NODE *n;

#ifdef DEBUG
	if (ptr == NULL)
		cant_happen();
#endif
	deref = NULL;
	field_num = -1;
	switch (ptr->type) {
	case Node_var:
	case Node_var_array:
		if (ptr == NF_node && (int) NF_node->var_value->numbr == -1)
			(void) get_field(HUGE-1, assign); /* parse record */
		deref = ptr->var_value;
#ifdef DEBUG
		if (deref->type != Node_val)
			cant_happen();
		if (deref->flags == 0)
			cant_happen();
#endif
		return &(ptr->var_value);

	case Node_param_list:
		n = stack_ptr[ptr->param_cnt];
		deref = n->var_value;
#ifdef DEBUG
		if (deref->type != Node_val)
			cant_happen();
		if (deref->flags == 0)
			cant_happen();
#endif
		return &(n->var_value);

	case Node_field_spec:
		n = tree_eval(ptr->lnode);
		field_num = (int) force_number(n);
		free_temp(n);
		if (field_num < 0)
			fatal("attempt to access field %d", field_num);
		aptr = get_field(field_num, assign);
		deref = *aptr;
		return aptr;

	case Node_subscript:
		n = ptr->lnode;
		if (n->type == Node_param_list)
			n = stack_ptr[n->param_cnt];
		aptr = assoc_lookup(n, concat_exp(ptr->rnode));
		deref = *aptr;
#ifdef DEBUG
		if (deref->type != Node_val)
			cant_happen();
		if (deref->flags == 0)
			cant_happen();
#endif
		return aptr;
	case Node_func:
		fatal ("`%s' is a function, assignment is not allowed",
			ptr->lnode->param);
	default:
		cant_happen();
	}
	return 0;
}

static NODE *
match_op(tree)
NODE *tree;
{
	NODE *t1;
	struct re_pattern_buffer *rp;
	int i;
	int match = 1;

	if (tree->type == Node_nomatch)
		match = 0;
	if (tree->type == Node_regex)
		t1 = WHOLELINE;
	else {
		if (tree->lnode)
			t1 = force_string(tree_eval(tree->lnode));
		else
			t1 = WHOLELINE;
		tree = tree->rnode;
	}
	if (tree->type == Node_regex) {
		rp = tree->rereg;
		if (!strict && ((IGNORECASE_node->var_value->numbr != 0)
		    ^ (tree->re_case != 0))) {
			/* recompile since case sensitivity differs */
			rp = tree->rereg =
			    mk_re_parse(tree->re_text,
			    (IGNORECASE_node->var_value->numbr != 0));
			tree->re_case =
			    (IGNORECASE_node->var_value->numbr != 0);
		}
	} else {
		rp = make_regexp(force_string(tree_eval(tree)),
			(IGNORECASE_node->var_value->numbr != 0));
		if (rp == NULL)
			cant_happen();
	}
	i = re_search(rp, t1->stptr, t1->stlen, 0, t1->stlen,
		(struct re_registers *) NULL);
	i = (i == -1) ^ (match == 1);
	free_temp(t1);
	if (tree->type != Node_regex) {
		free(rp->buffer);
		free(rp->fastmap);
		free((char *) rp);
	}
	return tmp_number((AWKNUM) i);
}
