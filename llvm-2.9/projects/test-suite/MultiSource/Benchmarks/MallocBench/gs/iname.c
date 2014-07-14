/* Copyright (C) 1989, 1990 Aladdin Enterprises.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* iname.c */
/* Name lookup for Ghostscript interpreter */
#include "memory_.h"
#include "ghost.h"
#include "alloc.h"
#include "errors.h"
#include "name.h"
#include "store.h"

/* Definitions and structure for the name table. */
/* 1-character names are the first nt_1char_size entries. */
#define nt_log2_sub_size 7
#define nt_sub_size (1 << nt_log2_sub_size)
#define nt_hash_size 256		/* must be a power of 2 */
#define nt_1char_size 256		/* must cover a full byte */
typedef name name_sub_table[nt_sub_size];
typedef struct {
	name *hash[nt_hash_size];
	name *table[1 << (16 - nt_log2_sub_size)];	/* name_sub_table */
	uint count;
} name_table;
#define name_index_ptr(nt, index)\
  ((nt)->table[(index) >> nt_log2_sub_size] + ((index) & (nt_sub_size - 1)))

/* The one and only name table (for now). */
private name_table *the_nt;

/* Forward references */
private int name_alloc_sub(P1(name_table *));

/* Make a t_name ref out of a name * */
#define make_name(pref, pnm) make_tv(pref, t_name, pname, pnm)

/* Initialize the name table */
void
name_init()
{	uint i;
	the_nt = (name_table *)alloc(1, sizeof(name_table), "name_init");
	memset(the_nt, 0, sizeof(name_table));
	for ( i = 0; i < nt_1char_size; i += nt_sub_size )
	   {	the_nt->count = i;
		name_alloc_sub(the_nt);
	   }
}

/* Look up or enter a name in the table. */
/* Return 0 or an error code. */
/* The return may overlap the characters of the string! */
/* See name.h for the meaning of enterflag. */
int
name_ref(byte *ptr, uint isize, ref *pref, int enterflag)
{	register name **ppname;
	register name *pname;
	byte *cptr;
	ushort size = (ushort)isize;	/* see name.h */
	if ( size == 1 )
	   {	pname = name_index_ptr(the_nt, *ptr);
		if ( pname->string_size != 0 )
		   {	make_name(pref, pname);
			return 0;
		   }
		if ( enterflag < 0 ) return e_undefined;
		pname->index = *ptr;
	   }
	else
	   {	ushort hash = (ushort)string_hash(ptr, size);
		ppname = the_nt->hash + (hash & (nt_hash_size - 1));
		while ( (pname = *ppname) != 0 )
		   {	if ( pname->string_size == size &&
			     !memcmp(ptr, pname->string_bytes, size)
			   )
			   {	make_name(pref, pname);
				return 0;
			   }
			ppname = &pname->next;
		   }
		/* Not in table, allocate a new entry. */
		if ( enterflag < 0 ) return e_undefined;
		if ( !(the_nt->count & (nt_sub_size - 1)) )
		   {	int code = name_alloc_sub(the_nt);
			if ( code < 0 ) return code;
		   }
		pname = name_index_ptr(the_nt, the_nt->count);
		pname->index = the_nt->count++;
		*ppname = pname;
	   }
	/* Name was not in the table.  Make a new entry. */
	if ( enterflag )
	   {	cptr = (byte *)alloc(size, 1, "name_ref(string)");
		if ( cptr == 0 ) return e_VMerror;
		memcpy(cptr, ptr, size);
	   }
	else
		cptr = ptr;
	pname->string_size = size;
	pname->string_bytes = cptr;
	pname->next = 0;
	pname->pvalue = pv_no_defn;
	make_name(pref, pname);
	return 0;
}

/* Get the string for a name. */
void
name_string_ref(ref *pnref /* t_name */, ref *psref /* result, t_string */)
{	name *pname = pnref->value.pname;
	make_tasv(psref, t_string, a_read+a_execute, pname->string_size,
		  bytes, pname->string_bytes);
}

/* Enter a name during initialization. */
/* Fatal error if the entry fails. */
void
name_enter(char *str, ref *pref)
{	if ( name_ref((byte *)str, strlen(str), pref, 0) )
		dprintf1("name_enter failed - %s", str), exit(1);
}

/* Get the name with a given index. */
void
name_index_ref(uint index, ref *pnref)
{	make_name(pnref, name_index_ptr(the_nt, index));
}

/* ------ Internal procedures ------ */

/* Allocate the next sub-table. */
private int
name_alloc_sub(name_table *nt)
{	name *sub = (name *)alloc(1, sizeof(name_sub_table), "name_alloc_sub");
	if ( sub == 0 ) return e_VMerror;
	memset(sub, 0, sizeof(name_sub_table));
	nt->table[nt->count >> nt_log2_sub_size] = sub;
	return 0;
}
