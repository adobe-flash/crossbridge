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

/* store.h */
/* Assignment macros */

/*
 * Macros for storing a ref.  We use macros for storing into objects,
 * since this is what a reference counting storage manager needs to know.
 * We also use macros for other ref assignments, because (as it happens)
 * Turbo C generates pretty awful code for doing this.
 *
 * The potentially reference counted macro names all have the form
 *	<place><new_type>(place_to_store, new_value)
 * <place> may be either
 *	store (for an unrestricted store)
 *	n_store (for storing into a new object, or when the old contents
 *	  are known not to be a pointer-type ref)
 * <new_type> and new_value are chosen from the following alternatives:
 *	_i		POINTER TO arbitrary ref
 *	_i_n		POINTER TO non-pointer-type ref
 *	(nothing)	arbitrary ref
 *	_n		non-pointer-type ref
 *	_tv		type, value field name, value
 *			  (only for scalars, which don't have attributes)
 *	_tav		type, attributes, value field name, value
 *	_tasv		type, attributes, size, value field name, value
 *	_int		integer value
 *	_real		float value
 *
 * For _tav and _tasv, we must store the value first, because sometimes
 * it depends on the contents of the place being stored into.
 * Note that n_store_tav and n_store_tasv are only needed if
 * the type being stored is a pointer type.
 *
 * In addition, we define make_xxx macros for making refs on the stack,
 * and s_store[_*](place_to_store, new_value) for stores to the stack
 * or to local variables.  For s_store_*, new_value must be a simple
 * variable because of the ref_assign macro (see below).
 *
 * Note that the storage manager is not actually implemented yet,
 * so none of the macros do anything special.
 */

/*
 * Here is where we check for getting around Turbo C's inefficiencies.
 */
#ifdef __MSDOS__
	/* ref_assign is an assembly code procedure */
extern	void	ref_assign(P2(ref *, ref *));
#else
	/* ref_assign is a macro */
#  define ref_assign(pto,pfrom) (*(pto) = *(pfrom))
#endif

#define n_store_i(pto,pfrom) ref_assign(pto,pfrom)
#define n_store_i_n(pto,pfrom) ref_assign(pto,pfrom)
#define n_store(pref,newref) (*(pref)=(newref))
#define n_store_n(pref,newref) (*(pref)=(newref))
#define store_i(pto,pfrom) ref_assign(pto,pfrom)
#define store_i_n(pto,pfrom) ref_assign(pto,pfrom)
#define store(pref,newref) n_store(pref,newref)
#define store_n(pref,newref) n_store(pref,newref)

#define n_store_tv(pref,newtype,valfield,newvalue)\
  ((pref)->value.valfield = (newvalue),\
   r_set_type(pref, newtype))
#define store_tv(pref,t,vf,v) n_store_tv(pref,t,vf,v)
#define make_tv(pref,t,vf,v) n_store_tv(pref,t,vf,v)

#define n_store_tav(pref,newtype,newattrs,valfield,newvalue)\
  ((pref)->value.valfield = (newvalue),\
   r_set_type_attrs(pref, newtype, newattrs))
#define store_tav(pref,t,a,vf,v) n_store_tav(pref,t,a,vf,v)
#define make_tav(pref,t,a,vf,v) n_store_tav(pref,t,a,vf,v)

#define n_store_tasv(pref,newtype,newattrs,newsize,valfield,newvalue)\
  (n_store_tav(pref,newtype,newattrs,valfield,newvalue),\
   (pref)->size = (newsize))
#define store_tasv(pref,t,a,s,vf,v) n_store_tasv(pref,t,a,s,vf,v)
#define make_tasv(pref,t,a,s,vf,v) n_store_tasv(pref,t,a,s,vf,v)

#define make_bool(pref,bval)\
  n_store_tv(pref,t_boolean,index,bval)
#define make_false(pref) make_bool(pref,0)
#define make_true(pref) make_bool(pref,1)

#define n_store_int(pref,ival)\
  n_store_tv(pref,t_integer,intval,ival)
#define store_int(pref,ival) n_store_int(pref,ival)
#define make_int(pref,ival) n_store_int(pref,ival)

#define make_mark(pref) make_tv(pref, t_mark, intval, 0)

#define make_null(pref) make_tv(pref, t_null, intval, 0)

#define n_store_real(pref,rval)\
  n_store_tv(pref,t_real,realval,rval)
#define store_real(pref,rval) n_store_real(pref,rval)
#define make_real(pref,rval) n_store_real(pref,rval)

#define s_store(pref,newref) (*(pref)=(newref))
