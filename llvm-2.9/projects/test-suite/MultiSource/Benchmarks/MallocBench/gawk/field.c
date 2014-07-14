/*
 * field.c - routines for dealing with fields and record parsing
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

extern void assoc_clear();
extern int a_get_three();
extern int get_rs();

static char *get_fs();
static int re_split();
static int parse_fields();
static void set_element();

char *line_buf = NULL;	/* holds current input line */

static char *parse_extent;	/* marks where to restart parse of record */
static int parse_high_water=0;	/* field number that we have parsed so far */
static char f_empty[] = "";
static char *save_fs = " ";	/* save current value of FS when line is read,
				 * to be used in deferred parsing
				 */


NODE **fields_arr;		/* array of pointers to the field nodes */
NODE node0;			/* node for $0 which never gets free'd */
int node0_valid = 1;		/* $(>0) has not been changed yet */

void
init_fields()
{
	emalloc(fields_arr, NODE **, sizeof(NODE *), "init_fields");
	node0.type = Node_val;
	node0.stref = 0;
	node0.stptr = "";
	node0.flags = (STR|PERM);	/* never free buf */
	fields_arr[0] = &node0;
}

/*
 * Danger!  Must only be called for fields we know have just been blanked, or
 * fields we know don't exist yet.  
 */

/*ARGSUSED*/
static void
set_field(num, str, len, dummy)
int num;
char *str;
int len;
NODE *dummy;	/* not used -- just to make interface same as set_element */
{
	NODE *n;
	int t;
	static int nf_high_water = 0;

	if (num > nf_high_water) {
		erealloc(fields_arr, NODE **, (num + 1) * sizeof(NODE *), "set_field");
		nf_high_water = num;
	}
	/* fill in fields that don't exist */
	for (t = parse_high_water + 1; t < num; t++)
		fields_arr[t] = Nnull_string;
	n = make_string(str, len);
	(void) force_number(n);
	fields_arr[num] = n;
	parse_high_water = num;
}

/* Someone assigned a value to $(something).  Fix up $0 to be right */
static void
rebuild_record()
{
	register int tlen;
	register NODE *tmp;
	NODE *ofs;
	char *ops;
	register char *cops;
	register NODE **ptr;
	register int ofslen;

	tlen = 0;
	ofs = force_string(OFS_node->var_value);
	ofslen = ofs->stlen;
	ptr = &fields_arr[parse_high_water];
	while (ptr > &fields_arr[0]) {
		tmp = force_string(*ptr);
		tlen += tmp->stlen;
		ptr--;
	}
	tlen += (parse_high_water - 1) * ofslen;
	emalloc(ops, char *, tlen + 1, "fix_fields");
	cops = ops;
	ops[0] = '\0';
	for (ptr = &fields_arr[1]; ptr <= &fields_arr[parse_high_water]; ptr++) {
		tmp = *ptr;
		if (tmp->stlen == 1)
			*cops++ = tmp->stptr[0];
		else if (tmp->stlen != 0) {
			memcpy(cops, tmp->stptr, tmp->stlen);
			cops += tmp->stlen;
		}
		if (ptr != &fields_arr[parse_high_water]) {
			if (ofslen == 1)
				*cops++ = ofs->stptr[0];
			else if (ofslen != 0) {
				memcpy(cops, ofs->stptr, ofslen);
				cops += ofslen;
			}
		}
	}
	tmp = make_string(ops, tlen);
	free(ops);
	deref = fields_arr[0];
	do_deref();
	fields_arr[0] = tmp;
}

/*
 * setup $0, but defer parsing rest of line until reference is made to $(>0)
 * or to NF.  At that point, parse only as much as necessary.
 */
void
set_record(buf, cnt)
char *buf;
int cnt;
{
	register int i;

	assign_number(&NF_node->var_value, (AWKNUM)-1);
	for (i = 1; i <= parse_high_water; i++) {
		deref = fields_arr[i];
		do_deref();
	}
	parse_high_water = 0;
	node0_valid = 1;
	if (buf == line_buf) {
		deref = fields_arr[0];
		do_deref();
		save_fs = get_fs();
		node0.type = Node_val;
		node0.stptr = buf;
		node0.stlen = cnt;
		node0.stref = 1;
		node0.flags = (STR|PERM);	/* never free buf */
		fields_arr[0] = &node0;
	}
}

NODE **
get_field(num, assign)
int num;
int assign;	/* this field is on the LHS of an assign */
{
	int n;

	/*
	 * if requesting whole line but some other field has been altered,
	 * then the whole line must be rebuilt
	 */
	if (num == 0 && (node0_valid == 0 || assign)) {
		/* first, parse remainder of input record */
		if (NF_node->var_value->numbr == -1) {
			if (parse_high_water == 0)
				parse_extent = node0.stptr;
			n = parse_fields(HUGE-1, &parse_extent,
		    		node0.stlen - (parse_extent - node0.stptr),
		    		save_fs, set_field, (NODE *)NULL);
			assign_number(&NF_node->var_value, (AWKNUM)n);
		}
		if (node0_valid == 0)
			rebuild_record();
		return &fields_arr[0];
	}
	if (num > 0 && assign)
		node0_valid = 0;
	if (num <= parse_high_water)	/* we have already parsed this field */
		return &fields_arr[num];
	if (parse_high_water == 0 && num > 0)	/* starting at the beginning */
		parse_extent = fields_arr[0]->stptr;
	/*
	 * parse up to num fields, calling set_field() for each, and saving
	 * in parse_extent the point where the parse left off
	 */
	n = parse_fields(num, &parse_extent,
		fields_arr[0]->stlen - (parse_extent-fields_arr[0]->stptr),
		save_fs, set_field, (NODE *)NULL);
	if (num == HUGE-1)
		num = n;
	if (n < num) {	/* requested field number beyond end of record;
			 * set_field will just extend the number of fields,
			 * with empty fields
			 */
		set_field(num, f_empty, 0, (NODE *) NULL);
		/*
		 * if this field is onthe LHS of an assignment, then we want to
		 * set NF to this value, below
		 */
		if (assign)
			n = num;
	}
	/*
	 * if we reached the end of the record, set NF to the number of fields
	 * so far.  Note that num might actually refer to a field that
	 * is beyond the end of the record, but we won't set NF to that value at
	 * this point, since this is only a reference to the field and NF
	 * only gets set if the field is assigned to -- in this case n has
	 * been set to num above
	 */
	if (*parse_extent == '\0')
		assign_number(&NF_node->var_value, (AWKNUM)n);

	return &fields_arr[num];
}

/*
 * this is called both from get_field() and from do_split()
 */
static int
parse_fields(up_to, buf, len, fs, set, n)
int up_to;	/* parse only up to this field number */
char **buf;	/* on input: string to parse; on output: point to start next */
int len;
register char *fs;
void (*set) ();	/* routine to set the value of the parsed field */
NODE *n;
{
	char *s = *buf;
	register char *field;
	register char *scan;
	register char *end = s + len;
	int NF = parse_high_water;
	char rs = get_rs();


	if (up_to == HUGE)
		NF = 0;
	if (*fs && *(fs + 1) != '\0') {	/* fs is a regexp */
		struct re_registers reregs;

		scan = s;
		if (rs == 0 && STREQ(FS_node->var_value->stptr, " ")) {
			while ((*scan == '\n' || *scan == ' ' || *scan == '\t')
			    && scan < end)
				scan++;
		}
		s = scan;
		while (scan < end
		    && re_split(scan, (int)(end - scan), fs, &reregs) != -1
		    && NF < up_to) {
			if (reregs.end[0] == 0) {	/* null match */
				scan++;
				if (scan == end) {
					(*set)(++NF, s, scan - s, n);
					up_to = NF;
					break;
				}
				continue;
			}
			(*set)(++NF, s, scan - s + reregs.start[0], n);
			scan += reregs.end[0];
			s = scan;
		}
		if (NF != up_to && scan <= end) {
			if (!(rs == 0 && scan == end)) {
				(*set)(++NF, scan, (int)(end - scan), n);
				scan = end;
			}
		}
		*buf = scan;
		return (NF);
	}
	for (scan = s; scan < end && NF < up_to; scan++) {
		/*
		 * special case:  fs is single space, strip leading
		 * whitespace 
		 */
		if (*fs == ' ') {
			while ((*scan == ' ' || *scan == '\t') && scan < end)
				scan++;
			if (scan >= end)
				break;
		}
		field = scan;
		if (*fs == ' ')
			while (*scan != ' ' && *scan != '\t' && scan < end)
				scan++;
		else {
			while (*scan != *fs && scan < end)
				scan++;
			if (rs && scan == end-1 && *scan == *fs) {
				(*set)(++NF, field, (int)(scan - field), n);
				field = scan;
			}
		}
		(*set)(++NF, field, (int)(scan - field), n);
		if (scan == end)
			break;
	}
	*buf = scan;
	return NF;
}

static int
re_split(buf, len, fs, reregsp)
char *buf, *fs;
int len;
struct re_registers *reregsp;
{
	typedef struct re_pattern_buffer RPAT;
	static RPAT *rp;
	static char *last_fs = NULL;

	if ((last_fs != NULL && !STREQ(fs, last_fs))
	    || (rp && ! strict && ((IGNORECASE_node->var_value->numbr != 0)
			 ^ (rp->translate != NULL))))
	{
		/* fs has changed or IGNORECASE has changed */
		free(rp->buffer);
		free(rp->fastmap);
		free((char *) rp);
		free(last_fs);
		last_fs = NULL;
	}
	if (last_fs == NULL) {	/* first time */
		emalloc(rp, RPAT *, sizeof(RPAT), "re_split");
		memset((char *) rp, 0, sizeof(RPAT));
		emalloc(rp->buffer, char *, 8, "re_split");
		rp->allocated = 8;
		emalloc(rp->fastmap, char *, 256, "re_split");
		emalloc(last_fs, char *, strlen(fs) + 1, "re_split");
		(void) strcpy(last_fs, fs);
		if (! strict && IGNORECASE_node->var_value->numbr != 0.0)
			rp->translate = casetable;
		else
			rp->translate = NULL;
		if (re_compile_pattern(fs, strlen(fs), rp) != NULL)
			fatal("illegal regular expression for FS: `%s'", fs);
	}
	return re_search(rp, buf, len, 0, len, reregsp);
}

NODE *
do_split(tree)
NODE *tree;
{
	NODE *t1, *t2, *t3;
	register char *splitc;
	char *s;
	NODE *n;

	if (a_get_three(tree, &t1, &t2, &t3) < 3)
		splitc = get_fs();
	else
		splitc = force_string(t3)->stptr;

	n = t2;
	if (t2->type == Node_param_list)
		n = stack_ptr[t2->param_cnt];
	if (n->type != Node_var && n->type != Node_var_array)
		fatal("second argument of split is not a variable");
	assoc_clear(n);

	tree = force_string(t1);

	s = tree->stptr;
	return tmp_number((AWKNUM)
		parse_fields(HUGE, &s, tree->stlen, splitc, set_element, n));
}

static char *
get_fs()
{
	register NODE *tmp;
	static char buf[10];

	tmp = force_string(FS_node->var_value);
	if (get_rs() == 0) {
		if (tmp->stlen == 1) {
			if (tmp->stptr[0] == ' ')
				(void) strcpy(buf, "[ 	\n]+");
			else
				sprintf(buf, "[%c\n]", tmp->stptr[0]);
		} else if (tmp->stlen == 0) {
			buf[0] = '\n';
			buf[1] = '\0';
		} else
			return tmp->stptr;
		return buf;
	}
	return tmp->stptr;
}

static void
set_element(num, s, len, n)
int num;
char *s;
int len;
NODE *n;
{
	*assoc_lookup(n, tmp_number((AWKNUM) (num))) = make_string(s, len);
}
