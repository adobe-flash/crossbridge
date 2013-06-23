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

/* idict.c */
/* Dictionaries for Ghostscript */
#include "ghost.h"
#include "alloc.h"
#include "errors.h"
#include "name.h"
#include "store.h"
#include "dict.h"	/* interface definition */

/* Import the dictionary stack */
extern ref dstack[];

/* Import systemdict and userdict for the name value cache. */
#define systemdict (dstack[0])
#define userdict (dstack[1])

/*
 * A dictionary is a structure of two elements (refs).
 * The first element is a t_integer whose value says how many
 * entries are occupied (N).  The second element is a t_array
 * of 2N+2 elements, containing alternating keys and values.
 * Unused entries have null as the key.  The first entry also
 * has null as the key (to avoid a special wrap-around check).
 * The access attributes for the dictionary are stored in
 * the contents ref.
 */
struct dict_s {
	ref count;			/* t_integer */
	ref contents;			/* t_array */
};
struct pair_s {
	ref key;
	ref value;
};
typedef struct pair_s pair;
#define pairs(dct) (pair *)((dct)->contents.value.refs)
#define npairs(dct) (((dct)->contents.size >> 1) - 1)

/* Define the size of the largest valid dictionary. */
/* This is limited by the size field of the contents ref. */
uint dict_max_size = max_ushort / 2 - 1;

/* Create a dictionary */
int
dict_create(uint size, ref *pref)
{	uint asize = (size == 0 ? 1 : size) + 1;
	dict *pdict = (dict *)alloc(1, sizeof(dict), "dict_create");
	pair *pp;
	if ( pdict == 0 ) return e_VMerror;
	pp = (pair *)alloc(asize, sizeof(pair), "dict_create(pairs)");
	if ( pp == 0 )
	   {	alloc_free((char *)pdict, 1, sizeof(dict), "dict_create");
		return e_VMerror;
	   }
	n_store_int(&pdict->count, 0);
	n_store_tasv(&pdict->contents, t_array, a_all, asize * 2,
		     refs, (ref *)pp);
	n_store_tav(pref, t_dictionary, a_all, pdict, pdict);
	pp = pairs(pdict);
	while ( asize-- )
		pp->key.type_attrs = pp->value.type_attrs = null_type_attrs,
		pp++;
	return 0;
   }

/* Return a pointer to a ref that holds the access attributes */
/* for a dictionary. */
ref *
dict_access_ref(ref *pdref)
{	return &pdref->value.pdict->contents;
}

/* Look up in a stack of dictionaries.  Store a pointer to the value slot */
/* where found, or to the (value) slot for inserting. */
/* Return 1 if found, 0 if not and there is room for a new entry, */
/* or e_dictfull if the dictionary is full and the key is missing. */
/* The caller is responsible for ensuring key is not a null. */
/* Note that pdbot <= pdtop, and the search starts at pdtop. */
int
dict_lookup(ref *pdbot, ref *pdtop, ref *pkey,
  ref **ppvalue /* result is stored here */)
{	ref *pdref = pdtop;
	uint hash;
	int ktype;
	name *kpname;
	int full = 1;			/* gets set to 0 or e_dictfull */
	/* Compute hash.  The only types we bother with are strings */
	/* and names. */
	switch ( r_type(pkey) )
	   {
	case t_name:
		kpname = pkey->value.pname;
nh:		hash = kpname->index * 40503;
		ktype = t_name; break;
	case t_string:			/* convert to a name first */
	   {	ref nref;
		int code = name_ref(pkey->value.bytes, pkey->size, &nref, 1);
		if ( code < 0 ) return code;
		kpname = nref.value.pname;
	   }	goto nh;
	default:
		hash = r_btype(pkey) * 99;	/* yech */
		ktype = -1;
	   }
	do
	   {	dict *pdict = pdref->value.pdict;
		uint size = npairs(pdict);
		pair *ppbot = pairs(pdict);
		register pair *pp;		/* probe pointer */
		int wrap = 0;
		register int etype;
		/* Search the dictionary */
#ifdef DEBUG
if ( gs_debug['d'] )
		   {	extern void debug_print_ref(P1(ref *));
			printf("[d]");
			debug_print_ref(pdref);
			printf(":");
			debug_print_ref(pkey);
			printf("->");
		   }
#endif
#ifdef DEBUG
#  define print_found()\
if ( gs_debug['d'] )\
   {	extern void debug_print_ref(P1(ref *));\
	debug_print_ref(&pp->value);\
	printf("; ");\
   }
#else
#  define print_found()
#endif
		for ( pp = ppbot + (hash % size) + 2; ; )
		   {	if ( (etype = r_type(&(--pp)->key)) == ktype )
			   {	/* Fast comparison if both keys are names */
				if ( pp->key.value.pname == kpname )
				   {	*ppvalue = &pp->value;
					print_found();
					return 1;
				   }
			   }
			else if ( etype == t_null )
			   {	/* We might have hit the dummy entry at */
				/* the beginning, in which case we should */
				/* wrap around to the end. */
				if ( pp == ppbot )	/* wrap */
				   {	if ( wrap++ )	/* wrapped twice */
					   {	if ( full > 0 )
							full = e_dictfull;
						goto next_dict;
					   }
					pp += size + 1;
				   }
				else	/* key not found */
					break;
			   }
			else
			   {	if ( obj_eq(&pp->key, pkey) )
				   {	*ppvalue = &pp->value;
					print_found();
					return 1;
				   }
			   }
		   }
		if ( full > 0 )
		   {	*ppvalue = &pp->value;
#ifdef DEBUG
if ( gs_debug['d'] )
			printf("0(%lx); ", (ulong)&pp->value);
#endif
			full = 0;
		   }
next_dict: ;
	   }
	while ( --pdref >= pdbot );
	return full;
}

/* Enter a key-value pair in a dictionary. */
/* Return 0 or e_dictfull. */
int
dict_put(ref *pdref /* t_dictionary */, ref *pkey, ref *pvalue)
{	ref *pvslot;
	if ( dict_find(pdref, pkey, &pvslot) <= 0 )	/* not found */
	   {	/* Check for overflow */
		dict *pdict = pdref->value.pdict;
		if ( pdict->count.value.intval == npairs(pdict) )
			return e_dictfull;
		pdict->count.value.intval++;
		pvslot[-1] = *pkey;	/* i.e. key of pair */
		/* If the key is a name, update its 1-element cache. */
		if ( r_type(pkey) == t_name )
		   {	name *pname = pkey->value.pname;
			if ( pname->pvalue == pv_no_defn &&
				(pdict == systemdict.value.pdict ||
				 pdict == userdict.value.pdict) )
			   {	/* Initialize the cache */
				pname->pvalue = pvslot;
			   }
			else	/* The cache is worthless */
				pname->pvalue = pv_other;
		   }
	   }
	store_i(pvslot, pvalue);
	return 0;
}

/* Return the number of elements in a dictionary. */
uint
dict_length(ref *pdref /* t_dictionary */)
{	return (uint)(pdref->value.pdict->count.value.intval);
}

/* Return the capacity of a dictionary. */
uint
dict_maxlength(ref *pdref /* t_dictionary */)
{	return npairs(pdref->value.pdict);
}

/* Copy one dictionary into another. */
int
dict_copy(ref *pdrfrom /* t_dictionary */, ref *pdrto /* t_dictionary */)
{	dict *pdict = pdrfrom->value.pdict;
	int count = npairs(pdict) + 1;	/* +1 for dummy first entry */
	pair *pp = pairs(pdict);
	int code;
	while ( count-- )
	   {	if ( r_type(&pp->key) != t_null )
			if ( (code = dict_put(pdrto, &pp->key, &pp->value)) != 0 )
				return code;
		pp++;
	   }
	return 0;
}

/* Resize a dictionary */
int
dict_resize(ref *pdrfrom, uint new_size)
{	dict *pdict = pdrfrom->value.pdict;
	ref drto;
	int code;
	if ( (code = dict_create(new_size, &drto)) < 0 ) return code;
	dict_copy(pdrfrom, &drto);	/* can't fail */
	/* Free the old dictionary */
	alloc_free((char *)pdict->contents.value.refs,
		   dict_maxlength(pdrfrom), sizeof(pair), "dict_resize(old)");
	*pdict = *drto.value.pdict;
	/* Free the new dictionary header */
	alloc_free((char *)drto.value.pdict,
		   1, sizeof(dict), "dict_resize(new)");
	return 0;
}

/* Prepare to enumerate a dictionary. */
int
dict_first(ref *pdref)
{	return (int)(npairs(pdref->value.pdict) + 1);	/* +1 for dummy */
}

/* Enumerate the next element of a dictionary. */
int
dict_next(ref *pdref, int index, ref *eltp /* ref eltp[2] */)
{	pair *pp = pairs(pdref->value.pdict) + index;
	while ( pp--, --index >= 0 )
	   {	if ( r_type(&pp->key) != t_null )
		   {	eltp[0] = pp->key;
			eltp[1] = pp->value;
			return index;
		   }
	   }
	return -1;			/* no more elements */
}
