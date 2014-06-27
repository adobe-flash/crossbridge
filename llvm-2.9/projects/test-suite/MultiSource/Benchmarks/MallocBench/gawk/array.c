/*
 * array.c - routines for associative arrays.
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

#ifdef DONTDEF
int primes[] = {31, 61, 127, 257, 509, 1021, 2053, 4099, 8191, 16381};
#endif

#define ASSOC_HASHSIZE 127
#define STIR_BITS(n) ((n) << 5 | (((n) >> 27) & 0x1f))
#define HASHSTEP(old, c) ((old << 1) + c)
#define MAKE_POS(v) (v & ~0x80000000)	/* make number positive */

NODE *
concat_exp(tree)
NODE *tree;
{
	NODE *r;
	char *str;
	char *s;
	unsigned len;
	int offset;
	int subseplen;
	char *subsep;

	if (tree->type != Node_expression_list)
		return force_string(tree_eval(tree));
	r = force_string(tree_eval(tree->lnode));
	if (tree->rnode == NULL)
		return r;
	subseplen = SUBSEP_node->lnode->stlen;
	subsep = SUBSEP_node->lnode->stptr;
	len = r->stlen + subseplen + 1;
	emalloc(str, char *, len, "concat_exp");
	memcpy(str, r->stptr, r->stlen+1);
	s = str + r->stlen;
	free_temp(r);
	tree = tree->rnode;
	while (tree) {
		if (subseplen == 1)
			*s++ = *subsep;
		else {
			memcpy(s, subsep, subseplen+1);
			s += subseplen;
		}
		r = force_string(tree_eval(tree->lnode));
		len += r->stlen + subseplen;
		offset = s - str;
		erealloc(str, char *, len, "concat_exp");
		s = str + offset;
		memcpy(s, r->stptr, r->stlen+1);
		s += r->stlen;
		free_temp(r);
		tree = tree->rnode;
	}
	r = tmp_string(str, s - str);
	free(str);
	return r;
}

/* Flush all the values in symbol[] before doing a split() */
void
assoc_clear(symbol)
NODE *symbol;
{
	int i;
	NODE *bucket, *next;

	if (symbol->var_array == 0)
		return;
	for (i = 0; i < ASSOC_HASHSIZE; i++) {
		for (bucket = symbol->var_array[i]; bucket; bucket = next) {
			next = bucket->ahnext;
			deref = bucket->ahname;
			do_deref();
			deref = bucket->ahvalue;
			do_deref();
			freenode(bucket);
		}
		symbol->var_array[i] = 0;
	}
}

/*
 * calculate the hash function of the string subs, also returning in *typtr
 * the type (string or number) 
 */
static int
hash_calc(subs)
NODE *subs;
{
	register int hash1 = 0, i;

	subs = force_string(subs);
	for (i = 0; i < subs->stlen; i++)
		hash1 = HASHSTEP(hash1, subs->stptr[i]);

	hash1 = MAKE_POS(STIR_BITS((int) hash1)) % ASSOC_HASHSIZE;
	return (hash1);
}

/*
 * locate symbol[subs], given hash of subs and type 
 */
static NODE *				/* NULL if not found */
assoc_find(symbol, subs, hash1)
NODE *symbol, *subs;
int hash1;
{
	register NODE *bucket;

	for (bucket = symbol->var_array[hash1]; bucket; bucket = bucket->ahnext) {
		if (cmp_nodes(bucket->ahname, subs))
			continue;
		return bucket;
	}
	return NULL;
}

/*
 * test whether the array element symbol[subs] exists or not 
 */
int
in_array(symbol, subs)
NODE *symbol, *subs;
{
	register int hash1;

	if (symbol->type == Node_param_list)
		symbol = stack_ptr[symbol->param_cnt];
	if (symbol->var_array == 0)
		return 0;
	subs = concat_exp(subs);
	hash1 = hash_calc(subs);
	if (assoc_find(symbol, subs, hash1) == NULL) {
		free_temp(subs);
		return 0;
	} else {
		free_temp(subs);
		return 1;
	}
}

/*
 * SYMBOL is the address of the node (or other pointer) being dereferenced.
 * SUBS is a number or string used as the subscript. 
 *
 * Find SYMBOL[SUBS] in the assoc array.  Install it with value "" if it
 * isn't there. Returns a pointer ala get_lhs to where its value is stored 
 */
NODE **
assoc_lookup(symbol, subs)
NODE *symbol, *subs;
{
	register int hash1, i;
	register NODE *bucket;

	hash1 = hash_calc(subs);

	if (symbol->var_array == 0) {	/* this table really should grow
					 * dynamically */
		emalloc(symbol->var_array, NODE **, (sizeof(NODE *) *
			ASSOC_HASHSIZE), "assoc_lookup");
		for (i = 0; i < ASSOC_HASHSIZE; i++)
			symbol->var_array[i] = 0;
		symbol->type = Node_var_array;
	} else {
		bucket = assoc_find(symbol, subs, hash1);
		if (bucket != NULL) {
			free_temp(subs);
			return &(bucket->ahvalue);
		}
	}
	bucket = newnode(Node_ahash);
	bucket->ahname = dupnode(subs);
	bucket->ahvalue = Nnull_string;
	bucket->ahnext = symbol->var_array[hash1];
	symbol->var_array[hash1] = bucket;
	return &(bucket->ahvalue);
}

void
do_delete(symbol, tree)
NODE *symbol, *tree;
{
	register int hash1;
	register NODE *bucket, *last;
	NODE *subs;

	if (symbol->var_array == 0)
		return;
	subs = concat_exp(tree);
	hash1 = hash_calc(subs);

	last = NULL;
	for (bucket = symbol->var_array[hash1]; bucket; last = bucket, bucket = bucket->ahnext)
		if (cmp_nodes(bucket->ahname, subs) == 0)
			break;
	free_temp(subs);
	if (bucket == NULL)
		return;
	if (last)
		last->ahnext = bucket->ahnext;
	else
		symbol->var_array[hash1] = bucket->ahnext;
	deref = bucket->ahname;
	do_deref();
	deref = bucket->ahvalue;
	do_deref();
	freenode(bucket);
}

struct search *
assoc_scan(symbol)
NODE *symbol;
{
	struct search *lookat;

	if (!symbol->var_array)
		return 0;
	emalloc(lookat, struct search *, sizeof(struct search), "assoc_scan");
	lookat->numleft = ASSOC_HASHSIZE;
	lookat->arr_ptr = symbol->var_array;
	lookat->bucket = symbol->var_array[0];
	return assoc_next(lookat);
}

struct search *
assoc_next(lookat)
struct search *lookat;
{
	for (; lookat->numleft; lookat->numleft--) {
		while (lookat->bucket != 0) {
			lookat->retval = lookat->bucket->ahname;
			lookat->bucket = lookat->bucket->ahnext;
			return lookat;
		}
		lookat->bucket = *++(lookat->arr_ptr);
	}
	free((char *) lookat);
	return 0;
}
