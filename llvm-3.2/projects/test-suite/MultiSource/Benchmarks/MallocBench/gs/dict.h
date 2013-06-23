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

/* dict.h */
/* Interfaces for GhostScript dictionary package */

/* Define the maximum size of a dictionary */
extern uint dict_max_size;

/* Create a dictionary */
extern int dict_create(P2(uint maxlength, ref *dict));

/* Return a pointer to a ref that holds the access attributes */
/* for a dictionary. */
extern ref *dict_access_ref(P1(ref *dict));
#define check_dict_read(dict) check_read(*dict_access_ref(&dict))
#define check_dict_write(dict) check_write(*dict_access_ref(&dict))

/* Look up in a stack of dictionaries.  Store a pointer to the value slot */
/* where found, or to the (value) slot for inserting. */
/* Return 1 if found, 0 if not and there is room for a new entry, */
/* or e_dictfull if the dictionary is full and the key is missing. */
/* The caller is responsible for ensuring key is not a null. */
/* Note that pdbot <= pdtop, and the search starts at pdtop. */
extern int dict_lookup(P4(ref *pdbot, ref *pdtop, ref *key, ref **ppvalue));
/* Look up in just one dictionary. */
#define dict_find(dict,key,ppvalue) dict_lookup(dict,dict,key,ppvalue)

/* Enter a key-value pair in a dictionary. */
/* Return 0 or e_dictfull. */
extern int dict_put(P3(ref *dict, ref *key, ref *pvalue));

/* Return the number of elements in a dictionary. */
extern uint dict_length(P1(ref *dict));

/* Return the capacity of a dictionary. */
extern uint dict_maxlength(P1(ref *dict));

/* Copy one dictionary into another. */
/* Return 0 or e_dictfull. */
extern int dict_copy(P2(ref *dfrom, ref *dto));

/* Grow or shrink a dictionary. */
/* Return 0 or e_dictfull. */
extern int dict_resize(P2(ref *dict, uint newmaxlength));

/* Prepare to enumerate a dictionary. */
/* Return an integer suitable for the first call to dict_next. */
extern int dict_first(P1(ref *dict));

/* Enumerate the next element of a dictionary. */
/* index is initially the result of a call on dict_first. */
/* Either store a key and value at eltp[0] and eltp[1] */
/* and return an updated index, or return -1 */
/* to signal that there are no more elements in the dictionary. */
extern int dict_next(P3(ref *dict, int index, ref *eltp));
