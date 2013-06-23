/*
 * debug.c -- Various debugging routines 
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

#ifdef DEBUG

extern NODE **fields_arr;


/* This is all debugging stuff.  Ignore it and maybe it'll go away. */

/*
 * Some of it could be turned into a really cute trace command, if anyone
 * wants to.  
 */
char *nnames[] = {
	"illegal", "times", "quotient", "mod", "plus",
	"minus", "cond_pair", "subscript", "concat", "exp",
	/* 10 */
	"preincrement", "predecrement", "postincrement", "postdecrement",
	"unary_minus",
	"field_spec", "assign", "assign_times", "assign_quotient", "assign_mod",
	/* 20 */
	"assign_plus", "assign_minus", "assign_exp", "and", "or",
	"equal", "notequal", "less", "greater", "leq",
	/* 30 */
	"geq", "match", "nomatch", "not", "rule_list",
	"rule_node", "statement_list", "if_branches", "expression_list",
	"param_list",
	/* 40 */
	"K_if", "K_while", "K_for", "K_arrayfor", "K_break",
	"K_continue", "K_print", "K_printf", "K_next", "K_exit",
	/* 50 */
	"K_do", "K_return", "K_delete", "K_getline", "K_function",
	"redirect_output", "redirect_append", "redirect_pipe",
	"redirect_pipein", "redirect_input",
	/* 60 */
	"var", "var_array", "val", "builtin", "line_range",
	"in_array", "func", "func_call", "cond_exp", "regex",
	/* 70 */
	"hashnode", "ahash"
};

ptree(n)
NODE *n;
{
	print_parse_tree(n);
}

pt()
{
	long x;

	(void) scanf("%x", &x);
	printf("0x%x\n", x);
	print_parse_tree((NODE *) x);
	fflush(stdout);
}

static depth = 0;

print_parse_tree(ptr)
NODE *ptr;
{
	if (!ptr) {
		printf("NULL\n");
		return;
	}
	if ((int) (ptr->type) < 0 || (int) (ptr->type) > sizeof(nnames) / sizeof(nnames[0])) {
		printf("(0x%x Type %d??)\n", ptr, ptr->type);
		return;
	}
	printf("(%d)%*s", depth, depth, "");
	switch ((int) ptr->type) {
	case (int) Node_val:
		printf("(0x%x Value ", ptr);
		if (ptr->flags&STR)
			printf("str: \"%.*s\" ", ptr->stlen, ptr->stptr);
		if (ptr->flags&NUM)
			printf("num: %g", ptr->numbr);
		printf(")\n");
		return;
	case (int) Node_var_array:
		{
		struct search *l;

		printf("(0x%x Array)\n", ptr);
		for (l = assoc_scan(ptr); l; l = assoc_next(l)) {
			printf("\tindex: ");
			print_parse_tree(l->retval);
			printf("\tvalue: ");
			print_parse_tree(*assoc_lookup(ptr, l->retval));
			printf("\n");
		}
		return;
		}
	case Node_param_list:
		printf("(0x%x Local variable %s)\n", ptr, ptr->param);
		if (ptr->rnode)
			print_parse_tree(ptr->rnode);
		return;
	case Node_regex:
		printf("(0x%x Regular expression %s\n", ptr, ptr->re_text);
		return;
	}
	if (ptr->lnode)
		printf("0x%x = left<--", ptr->lnode);
	printf("(0x%x %s.%d)", ptr, nnames[(int) (ptr->type)], ptr->type);
	if (ptr->rnode)
		printf("-->right = 0x%x", ptr->rnode);
	printf("\n");
	depth++;
	if (ptr->lnode)
		print_parse_tree(ptr->lnode);
	switch ((int) ptr->type) {
	case (int) Node_line_range:
	case (int) Node_match:
	case (int) Node_nomatch:
		break;
	case (int) Node_builtin:
		printf("Builtin: %d\n", ptr->proc);
		break;
	case (int) Node_K_for:
	case (int) Node_K_arrayfor:
		printf("(%s:)\n", nnames[(int) (ptr->type)]);
		print_parse_tree(ptr->forloop->init);
		printf("looping:\n");
		print_parse_tree(ptr->forloop->cond);
		printf("doing:\n");
		print_parse_tree(ptr->forloop->incr);
		break;
	default:
		if (ptr->rnode)
			print_parse_tree(ptr->rnode);
		break;
	}
	--depth;
}


/*
 * print out all the variables in the world 
 */

dump_vars()
{
	register int n;
	register NODE *buc;

#ifdef notdef
	printf("Fields:");
	dump_fields();
#endif
	printf("Vars:\n");
	for (n = 0; n < HASHSIZE; n++) {
		for (buc = variables[n]; buc; buc = buc->hnext) {
			printf("'%.*s': ", buc->hlength, buc->hname);
			print_parse_tree(buc->hvalue);
		}
	}
	printf("End\n");
}

#ifdef notdef
dump_fields()
{
	register NODE **p;
	register int n;

	printf("%d fields\n", f_arr_siz);
	for (n = 0, p = &fields_arr[0]; n < f_arr_siz; n++, p++) {
		printf("$%d is '", n);
		print_simple(*p, stdout);
		printf("'\n");
	}
}
#endif

/* VARARGS1 */
print_debug(str, n)
char *str;
{
	extern int debugging;

	if (debugging)
		printf("%s:0x%x\n", str, n);
}

int indent = 0;

print_a_node(ptr)
NODE *ptr;
{
	NODE *p1;
	char *str, *str2;
	int n;
	NODE *buc;

	if (!ptr)
		return;		/* don't print null ptrs */
	switch (ptr->type) {
	case Node_val:
		if (ptr->flags&NUM)
			printf("%g", ptr->numbr);
		else
			printf("\"%.*s\"", ptr->stlen, ptr->stptr);
		return;
	case Node_times:
		str = "*";
		goto pr_twoop;
	case Node_quotient:
		str = "/";
		goto pr_twoop;
	case Node_mod:
		str = "%";
		goto pr_twoop;
	case Node_plus:
		str = "+";
		goto pr_twoop;
	case Node_minus:
		str = "-";
		goto pr_twoop;
	case Node_exp:
		str = "^";
		goto pr_twoop;
	case Node_concat:
		str = " ";
		goto pr_twoop;
	case Node_assign:
		str = "=";
		goto pr_twoop;
	case Node_assign_times:
		str = "*=";
		goto pr_twoop;
	case Node_assign_quotient:
		str = "/=";
		goto pr_twoop;
	case Node_assign_mod:
		str = "%=";
		goto pr_twoop;
	case Node_assign_plus:
		str = "+=";
		goto pr_twoop;
	case Node_assign_minus:
		str = "-=";
		goto pr_twoop;
	case Node_assign_exp:
		str = "^=";
		goto pr_twoop;
	case Node_and:
		str = "&&";
		goto pr_twoop;
	case Node_or:
		str = "||";
		goto pr_twoop;
	case Node_equal:
		str = "==";
		goto pr_twoop;
	case Node_notequal:
		str = "!=";
		goto pr_twoop;
	case Node_less:
		str = "<";
		goto pr_twoop;
	case Node_greater:
		str = ">";
		goto pr_twoop;
	case Node_leq:
		str = "<=";
		goto pr_twoop;
	case Node_geq:
		str = ">=";
		goto pr_twoop;

pr_twoop:
		print_a_node(ptr->lnode);
		printf("%s", str);
		print_a_node(ptr->rnode);
		return;

	case Node_not:
		str = "!";
		str2 = "";
		goto pr_oneop;
	case Node_field_spec:
		str = "$(";
		str2 = ")";
		goto pr_oneop;
	case Node_postincrement:
		str = "";
		str2 = "++";
		goto pr_oneop;
	case Node_postdecrement:
		str = "";
		str2 = "--";
		goto pr_oneop;
	case Node_preincrement:
		str = "++";
		str2 = "";
		goto pr_oneop;
	case Node_predecrement:
		str = "--";
		str2 = "";
		goto pr_oneop;
pr_oneop:
		printf(str);
		print_a_node(ptr->subnode);
		printf(str2);
		return;

	case Node_expression_list:
		print_a_node(ptr->lnode);
		if (ptr->rnode) {
			printf(",");
			print_a_node(ptr->rnode);
		}
		return;

	case Node_var:
		for (n = 0; n < HASHSIZE; n++) {
			for (buc = variables[n]; buc; buc = buc->hnext) {
				if (buc->hvalue == ptr) {
					printf("%.*s", buc->hlength, buc->hname);
					n = HASHSIZE;
					break;
				}
			}
		}
		return;
	case Node_subscript:
		print_a_node(ptr->lnode);
		printf("[");
		print_a_node(ptr->rnode);
		printf("]");
		return;
	case Node_builtin:
		printf("some_builtin(");
		print_a_node(ptr->subnode);
		printf(")");
		return;

	case Node_statement_list:
		printf("{\n");
		indent++;
		for (n = indent; n; --n)
			printf("  ");
		while (ptr) {
			print_maybe_semi(ptr->lnode);
			if (ptr->rnode)
				for (n = indent; n; --n)
					printf("  ");
			ptr = ptr->rnode;
		}
		--indent;
		for (n = indent; n; --n)
			printf("  ");
		printf("}\n");
		for (n = indent; n; --n)
			printf("  ");
		return;

	case Node_K_if:
		printf("if(");
		print_a_node(ptr->lnode);
		printf(") ");
		ptr = ptr->rnode;
		if (ptr->lnode->type == Node_statement_list) {
			printf("{\n");
			indent++;
			for (p1 = ptr->lnode; p1; p1 = p1->rnode) {
				for (n = indent; n; --n)
					printf("  ");
				print_maybe_semi(p1->lnode);
			}
			--indent;
			for (n = indent; n; --n)
				printf("  ");
			if (ptr->rnode) {
				printf("} else ");
			} else {
				printf("}\n");
				return;
			}
		} else {
			print_maybe_semi(ptr->lnode);
			if (ptr->rnode) {
				for (n = indent; n; --n)
					printf("  ");
				printf("else ");
			} else
				return;
		}
		if (!ptr->rnode)
			return;
		deal_with_curls(ptr->rnode);
		return;

	case Node_K_while:
		printf("while(");
		print_a_node(ptr->lnode);
		printf(") ");
		deal_with_curls(ptr->rnode);
		return;

	case Node_K_do:
		printf("do ");
		deal_with_curls(ptr->rnode);
		printf("while(");
		print_a_node(ptr->lnode);
		printf(") ");
		return;

	case Node_K_for:
		printf("for(");
		print_a_node(ptr->forloop->init);
		printf(";");
		print_a_node(ptr->forloop->cond);
		printf(";");
		print_a_node(ptr->forloop->incr);
		printf(") ");
		deal_with_curls(ptr->forsub);
		return;
	case Node_K_arrayfor:
		printf("for(");
		print_a_node(ptr->forloop->init);
		printf(" in ");
		print_a_node(ptr->forloop->incr);
		printf(") ");
		deal_with_curls(ptr->forsub);
		return;

	case Node_K_printf:
		printf("printf(");
		print_a_node(ptr->lnode);
		printf(")");
		return;
	case Node_K_print:
		printf("print(");
		print_a_node(ptr->lnode);
		printf(")");
		return;
	case Node_K_next:
		printf("next");
		return;
	case Node_K_break:
		printf("break");
		return;
	case Node_K_delete:
		printf("delete ");
		print_a_node(ptr->lnode);
		return;
	case Node_func:
		printf("function %s (", ptr->lnode->param);
		if (ptr->lnode->rnode)
			print_a_node(ptr->lnode->rnode);
		printf(")\n");
		print_a_node(ptr->rnode);
		return;
	case Node_param_list:
		printf("%s", ptr->param);
		if (ptr->rnode) {
			printf(", ");
			print_a_node(ptr->rnode);
		}
		return;
	default:
		print_parse_tree(ptr);
		return;
	}
}

print_maybe_semi(ptr)
NODE *ptr;
{
	print_a_node(ptr);
	switch (ptr->type) {
	case Node_K_if:
	case Node_K_for:
	case Node_K_arrayfor:
	case Node_statement_list:
		break;
	default:
		printf(";\n");
		break;
	}
}

deal_with_curls(ptr)
NODE *ptr;
{
	int n;

	if (ptr->type == Node_statement_list) {
		printf("{\n");
		indent++;
		while (ptr) {
			for (n = indent; n; --n)
				printf("  ");
			print_maybe_semi(ptr->lnode);
			ptr = ptr->rnode;
		}
		--indent;
		for (n = indent; n; --n)
			printf("  ");
		printf("}\n");
	} else {
		print_maybe_semi(ptr);
	}
}

NODE *
do_prvars()
{
	dump_vars();
	return Nnull_string;
}

NODE *
do_bp()
{
	return Nnull_string;
}

#endif

#ifdef MEMDEBUG

#undef free
extern void free();

void
do_free(s)
char *s;
{
	free(s);
}

#endif
