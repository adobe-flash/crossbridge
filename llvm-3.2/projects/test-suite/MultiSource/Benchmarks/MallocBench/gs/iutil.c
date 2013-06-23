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

/* iutil.c */
/* Utilities for GhostScript interpreter */
#include "memory_.h"
#include "string_.h"
#include "ghost.h"
#include "errors.h"
#include "alloc.h"
#include "oper.h"
#include "store.h"
#include "gsmatrix.h"
#include "gxdevice.h"			/* for gx_color_index */
#include "gzcolor.h"

/* Imported from name.c */
extern ref *name_string_ref(P2(ref *, ref *));

/* ------ Object utilities ------ */

/* Copy refs from one place to another. */
void
refcpy(register ref *to, register ref *from, register uint size)
{	while ( size-- ) store_i(to++, from++);
}

/* Compare two objects for equality.  Return 1 if equal, 0 if not. */
int
obj_eq(register ref *pref1, register ref *pref2)
{	ref nref;
	if ( r_btype(pref1) != r_btype(pref2) )
	   {	/* Only a few cases need be considered here: */
		/* integer/real, name/string, and vice versa. */
		switch ( r_type(pref1) )
		   {
		case t_integer:
			return (r_type(pref2) == t_real &&
				pref2->value.realval == pref1->value.intval);
		case t_real:
			return (r_type(pref2) == t_integer &&
				pref2->value.intval == pref1->value.realval);
		case t_name:
			if ( r_type(pref2) != t_string ) return 0;
			name_string_ref(pref1, &nref);
			pref1 = &nref;
			break;
		case t_string:
			if ( r_type(pref2) != t_name ) return 0;
			name_string_ref(pref2, &nref);
			pref2 = &nref;
			break;
		default:
			return 0;
		   }
	   }
	/* Now do a type-dependent comparison. */
	/* This would be very simple if we always filled in */
	/* all 8 bytes of a ref, but we currently don't. */
	switch ( r_btype(pref1) )
	   {
	case t_array:
	case t_packedarray:
		return (pref1->value.refs == pref2->value.refs &&
			pref1->size == pref2->size);
	case t_boolean:
		return (pref1->value.index == pref2->value.index);
	case t_dictionary:
		return (pref1->value.pdict == pref2->value.pdict);
	case t_file:
		return (pref1->value.pfile == pref2->value.pfile);
	case t_fontID:
		return (pref1->value.pfont == pref2->value.pfont);
	case t_integer:
		return (pref1->value.intval == pref2->value.intval);
	case t_mark:
	case t_null:
		return 1;
	case t_name:
		return (pref1->value.pname == pref2->value.pname);
	case t_operator:
		return ((long)pref1->value.opproc ==
			(long)pref2->value.opproc);
	case t_real:
		return (pref1->value.realval == pref2->value.realval);
	case t_save:
		return (pref1->value.psave == pref2->value.psave);
	case t_string:
		return (!bytes_compare(pref1->value.bytes, pref1->size,
				       pref2->value.bytes, pref2->size));
	case t_color:
	   {	struct gs_color_s
		  *pc1 = pref1->value.pcolor,
		  *pc2 = pref2->value.pcolor;
		return (pc1->red == pc2->red && pc1->green == pc2->green &&
			pc1->blue == pc2->blue);
	   }
	case t_device:
		return (pref1->value.pdevice == pref2->value.pdevice);
	   }
	return 0;			/* shouldn't happen! */
}

/* ------ String utilities ------ */

/* Compare two strings, returning -1 if the first is less, */
/* 0 if they are equal, and 1 if first is greater. */
/* We can't use memcmp, because we always use unsigned characters. */
int
bytes_compare(byte *s1, uint len1, byte *s2, uint len2)
{	register uint len = len1;
	if ( len2 < len ) len = len2;
	   {	register byte *p1 = s1;
		register byte *p2 = s2;
		while ( len-- )
			if ( *p1++ != *p2++ )
				return (p1[-1] < p2[-1] ? -1 : 1);
	   }
	/* Now check for differing lengths */
	return (len1 == len2 ? 0 : len1 < len2 ? -1 : 1);
}

/* Compute a hash for a string */
uint
string_hash(byte *ptr, uint len)
{	register byte *p = ptr;
	register uint hash = 0;
	register uint n = len;
	while ( n-- ) hash = hash * 19 + *p++;
	return hash;
}

/* Convert a C string to a Ghostscript string */
int
string_to_ref(char *cstr, ref *pref, char *cname)
{	uint size = strlen(cstr);
	char *str = alloc(size, 1, cname);
	if ( str == 0 ) return e_VMerror;
	memcpy(str, cstr, size);
	make_tasv(pref, t_string, a_all, size, bytes, (byte *)str);
	return 0;
}

/* Convert a Ghostscript string to a C string. */
/* Return 0 iff the buffer can't be allocated. */
char *
ref_to_string(ref *pref, char *client_name)
{	uint size = pref->size;
	char *str = alloc(size + 1, 1, client_name);
	if ( str == 0 ) return 0;
	memcpy(str, (char *)pref->value.bytes, size);
	str[size] = 0;
	return str;
}

/* ------ Operand utilities ------ */

/* Get N numeric operands from the stack. */
/* Return a bit-mask indicating which ones are integers, */
/* or a (negative) error indication. */
/* The 1-bit in the bit-mask refers to the bottommost stack entry. */
/* If pval is non-zero, also store float versions of the operands there. */
int
num_params(ref *op, int count, float *pval)
{	int mask = 0;
	while ( --count >= 0 )
	   {	mask <<= 1;
		switch ( r_type(op) )
		   {
		case t_real:
			if ( pval ) pval[count] = op->value.realval;
			break;
		case t_integer:
			if ( pval ) pval[count] = op->value.intval;
			mask++;
			break;
		default:
			return e_typecheck;
		   }
		op--;
	   }
	return mask;
}

/* Get a real parameter. */
/* If unit is true, the parameter must lie in [0..1],
/* to within a reasonable rounding error. */
/* If an error is returned, the return value is not updated. */
int
real_param(ref *op, float *pparam, int unit)
{	float fval;
	switch ( r_type(op) )
	   {
	case t_integer: fval = op->value.intval; break;
	case t_real: fval = op->value.realval; break;
	default: return e_typecheck;
	   }
	if ( unit )
	   {	if ( fval < 0.0 )
		   {	if ( fval < -0.00001 ) return e_rangecheck;
			fval = 0.0;
		   }
		else if ( fval > 1.0 )
		   {	if ( fval > 1.00001 ) return e_rangecheck;
			fval = 1.0;
		   }
	   }
	*pparam = fval;
	return 0;
}

/* Check for a matrix operand with read access. */
/* Return 0 if OK, error code if not. */
/* Store an all-float version of the matrix in *pmat. */
int
read_matrix(ref *op, gs_matrix *pmat)
{	check_array(*op);
	if ( op->size != 6 ) return e_rangecheck;
	if ( !(r_attrs(op) & a_read) ) return e_invalidaccess;
	*pmat = *(gs_matrix *)op->value.refs;
	   {	ref *pel = (ref *)pmat;
		int i;
		for ( i = 0; i < 6; i++ )
		   {	switch ( r_type(pel) )
			   {
			default: return e_typecheck;
			case t_integer:
				make_real(pel, pel->value.intval);
			case t_real: ;
			   }
			pel++;
		   }
	   }
	return 0;
}

/* Check for a matrix operand with write access. */
/* Return 0 if OK, error code if not. */
/* Any element that isn't a number gets set to 0. */
int
write_matrix(register ref *op)
{	ref *aptr;
	int i;
	if ( r_type(op) != t_array ) return e_typecheck;
	if ( op->size != 6 ) return e_rangecheck;
	if ( !(r_attrs(op) & a_write) ) return e_invalidaccess;
	aptr = op->value.refs;
	for ( i = 5; i >= 0; i--, aptr++ )
	  { switch ( r_type(aptr) )
	      {
	      default: store_int(aptr, 0);
	      case t_real: case t_integer: ;
	      }
	  }
	return 0;
}
