/*
 * node.c -- routines for node management
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

extern double strtod();

/*
 * We can't dereference a variable until after we've given it its new value.
 * This variable points to the value we have to free up 
 */
NODE *deref;

AWKNUM
r_force_number(n)
NODE *n;
{
	char *ptr;

#ifdef DEBUG
	if (n == NULL)
		cant_happen();
	if (n->type != Node_val)
		cant_happen();
	if(n->flags == 0)
		cant_happen();
	if (n->flags & NUM)
		return n->numbr;
#endif
	if (n->stlen == 0)
		n->numbr = 0.0;
	else if (n->stlen == 1) {
		if (isdigit(n->stptr[0])) {
			n->numbr = n->stptr[0] - '0';
			n->flags |= NUMERIC;
		} else
			n->numbr = 0.0;
	} else {
		errno = 0;
		n->numbr = (AWKNUM) strtod(n->stptr, &ptr);
		/* the following >= should be ==, but for SunOS 3.5 strtod() */
		if (errno == 0 && ptr >= n->stptr + n->stlen)
			n->flags |= NUMERIC;
	}
	n->flags |= NUM;
	return n->numbr;
}

/*
 * the following lookup table is used as an optimization in force_string
 * (more complicated) variations on this theme didn't seem to pay off, but 
 * systematic testing might be in order at some point
 */
static char *values[] = {
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
};
#define	NVAL	(sizeof(values)/sizeof(values[0]))

NODE *
r_force_string(s)
NODE *s;
{
	char buf[128];
	char *fmt;
	long num;
	char *sp = buf;

#ifdef DEBUG
	if (s == NULL)
		cant_happen();
	if (s->type != Node_val)
		cant_happen();
	if (s->flags & STR)
		return s;
	if (!(s->flags & NUM))
		cant_happen();
	if (s->stref != 0)
		cant_happen();
#endif
	s->flags |= STR;
	/* should check validity of user supplied OFMT */
	fmt = OFMT_node->var_value->stptr;
	if ((num = s->numbr) == s->numbr) {
		/* integral value */
		if (num < NVAL && num >= 0) {
			sp = values[num];
			s->stlen = 1;
		} else {
			(void) sprintf(sp, "%ld", num);
			s->stlen = strlen(sp);
		}
	} else {
		(void) sprintf(sp, fmt, s->numbr);
		s->stlen = strlen(sp);
	}
	s->stref = 1;
	emalloc(s->stptr, char *, s->stlen + 1, "force_string");
	memcpy(s->stptr, sp, s->stlen+1);
	return s;
}

/*
 * Duplicate a node.  (For strings, "duplicate" means crank up the
 * reference count.)
 */
NODE *
dupnode(n)
NODE *n;
{
	register NODE *r;

	if (n->flags & TEMP) {
		n->flags &= ~TEMP;
		n->flags |= MALLOC;
		return n;
	}
	if ((n->flags & (MALLOC|STR)) == (MALLOC|STR)) {
		if (n->stref < 255)
			n->stref++;
		return n;
	}
	r = newnode(Node_illegal);
	*r = *n;
	r->flags &= ~(PERM|TEMP);
	r->flags |= MALLOC;
	if (n->type == Node_val && (n->flags & STR)) {
		r->stref = 1;
		emalloc(r->stptr, char *, r->stlen + 1, "dupnode");
		memcpy(r->stptr, n->stptr, r->stlen+1);
	}
	return r;
}

/* this allocates a node with defined numbr */
NODE *
make_number(x)
AWKNUM x;
{
	register NODE *r;

	r = newnode(Node_val);
	r->numbr = x;
	r->flags |= (NUM|NUMERIC);
	r->stref = 0;
	return r;
}

/*
 * This creates temporary nodes.  They go away quite quickly, so don't use
 * them for anything important 
 */
NODE *
tmp_number(x)
AWKNUM x;
{
	NODE *r;

	r = make_number(x);
	r->flags |= TEMP;
	return r;
}

/*
 * Make a string node.
 */

NODE *
make_str_node(s, len, scan)
char *s;
int len;
int scan;
{
	register NODE *r;
	char *pf;
	register char *pt;
	register int c;
	register char *end;

	r = newnode(Node_val);
	emalloc(r->stptr, char *, len + 1, s);
	memcpy(r->stptr, s, len);
	r->stptr[len] = '\0';
	end = &(r->stptr[len]);
	       
	if (scan) {	/* scan for escape sequences */
		for (pf = pt = r->stptr; pf < end;) {
			c = *pf++;
			if (c == '\\') {
				c = parse_escape(&pf);
				if (c < 0)
					cant_happen();
				*pt++ = c;
			} else
				*pt++ = c;
		}
		len = pt - r->stptr;
		erealloc(r->stptr, char *, len + 1, "make_str_node");
		r->stptr[len] = '\0';
		r->flags |= PERM;
	}
	r->stlen = len;
	r->stref = 1;
	r->flags |= (STR|MALLOC);

	return r;
}

/* Read the warning under tmp_number */
NODE *
tmp_string(s, len)
char *s;
int len;
{
	register NODE *r;

	r = make_string(s, len);
	r->flags |= TEMP;
	return r;
}


#define NODECHUNK	100

static NODE *nextfree = NULL;

NODE *
newnode(ty)
NODETYPE ty;
{
	NODE *it;
	NODE *np;

#if defined(MPROF) || defined(NOMEMOPT)
	emalloc(it, NODE *, sizeof(NODE), "newnode");
#else
	if (nextfree == NULL) {
		/* get more nodes and initialize list */
		emalloc(nextfree, NODE *, NODECHUNK * sizeof(NODE), "newnode");
		for (np = nextfree; np < &nextfree[NODECHUNK - 1]; np++)
			np->nextp = np + 1;
		np->nextp = NULL;
	}
	/* get head of freelist */
	it = nextfree;
	nextfree = nextfree->nextp;
#endif
	it->type = ty;
	it->flags = MALLOC;
#ifdef MEMDEBUG
	fprintf(stderr, "node: new: %0x\n", it);
#endif
	return it;
}

void
freenode(it)
NODE *it;
{
#ifdef DEBUG
	NODE *nf;
#endif
#ifdef MEMDEBUG
	fprintf(stderr, "node: free: %0x\n", it);
#endif
#if defined(MPROF) || defined(NOMEMOPT) 
	free((char *) it);
#elif defined(IGNOREFREE)	
#else
#ifdef DEBUG
	for (nf = nextfree; nf; nf = nf->nextp)
		if (nf == it)
			fatal("attempt to free free node");
#endif
	/* add it to head of freelist */
	it->nextp = nextfree;
	nextfree = it;
#endif
}

#ifdef DEBUG
pf()
{
	NODE *nf = nextfree;
	while (nf != NULL) {
		fprintf(stderr, "%0x ", nf);
		nf = nf->nextp;
	}
}
#endif

void
do_deref()
{
	if (deref == NULL)
		return;
	if (deref->flags & PERM) {
		deref = 0;
		return;
	}
	if ((deref->flags & MALLOC) || (deref->flags & TEMP)) {
		deref->flags &= ~TEMP;
		if (deref->flags & STR) {
			if (deref->stref > 1 && deref->stref != 255) {
				deref->stref--;
				deref = 0;
				return;
			}
			free(deref->stptr);
		}
		freenode(deref);
	}
	deref = 0;
}
