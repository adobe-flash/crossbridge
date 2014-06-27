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

/* zmisc.c */
/* Miscellaneous operators for GhostScript */
#include "string_.h"
#include "ghost.h"
#include "memory_.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "dict.h"
#include "store.h"
#include "gxfixed.h"			/* for gstype1.h */
#include "gstype1.h"

/* Import the dictionary stack pointer (for bind) */
extern ref dstack[];
extern ref *dsp;

/* Import the C getenv function */
extern char *getenv(P1(char *));

/* Import from iutil.c */
extern char *ref_to_string(P2(ref *, char *));

/* bind */
int
zbind(register ref *op)
{	ref *bsp = op;			/* bottom of stack */
	switch ( r_type(op) )
	   {
	case t_array:
	case t_packedarray: break;
	default: return e_typecheck;
	   }
	/* It appears from the PostScript manual that bind works */
	/* even on procedures whose top level isn't writable. */
	/* This seems a little counter-intuitive, but we do the same. */
	*++bsp = *op;
	/* Here are the invariants for the following loop: */
	/*	op < bsp <= ostop; */
	/*	for every pointer p such that op < p <= bsp, */
	/*	  *p is an array (or packedarray) ref. */
	while ( bsp > op )
	   {	while ( bsp->size )
		   {	ref *tp = bsp->value.refs++;
			bsp->size--;
			switch ( r_type(tp) )
			 {
			case t_name:	/* bind the name if an operator */
			  if ( r_has_attrs(tp, a_executable) )
			   {	ref *pvalue;
				if ( dict_lookup(dstack, dsp, tp, &pvalue) > 0 &&
				     r_btype(pvalue) == t_operator && r_has_attrs(pvalue, a_executable)
				   )
					store(tp, *pvalue);
			   }
			  break;
			case t_array:	/* push into array if procedure */
			  if ( !r_has_attrs(tp, a_write) ) break;
			case t_packedarray:
			  if ( r_has_attrs(tp, a_executable) && bsp < ostop )
			   {	/* Make reference read-only */
				r_clear_attrs(tp, a_write);
				*++bsp = *tp;
			   }
			 }
		   }
		bsp--;
	   }
	return 0;
}

/* currenttime */
int
zcurrenttime(register ref *op)
{	long date_time[2];
	gs_get_clock(date_time);
	push(1);
	make_real(op, date_time[0] * 1440.0 + date_time[1] / 60000.0);
	return 0;
}

/* getenv */
int
zgetenv(register ref *op)
{	char *str, *value;
	int code;
	check_read_type(*op, t_string);
	str = ref_to_string(op, "getenv name");
	if ( str == 0 ) return e_VMerror;
	value = getenv(str);
	alloc_free(str, op->size + 1, 1, "getenv name");
	if ( value == 0 )		/* not found */
	   {	make_bool(op, 0);
		return 0;
	   }
	code = string_to_ref(value, op, "getenv value");
	if ( code < 0 ) return code;
	push(1);
	make_bool(op, 1);
	return 0;
}

/* setdebug */
int
zsetdebug(register ref *op)
{	check_read_type(op[-1], t_string);
	check_type(*op, t_boolean);
#ifdef DEBUG
	   {	int i;
		for ( i = 0; i < op[-1].size; i++ )
			gs_debug[op[-1].value.bytes[i] & 127] =
				op->value.index;
	   }
#endif
	pop(2);
	return 0;
}

/* type1encrypt, type1decrypt */
private int type1crypt(P2(ref *,
			  int (*)(P4(byte *, byte *, uint, ushort *))));
int
ztype1encrypt(ref *op)
{	return type1crypt(op, gs_type1_encrypt);
}
int
ztype1decrypt(ref *op)
{	return type1crypt(op, gs_type1_decrypt);
}
private int
type1crypt(register ref *op, int (*proc)(P4(byte *, byte *, uint, ushort *)))
{	crypt_state state;
	check_type(op[-2], t_integer);
	state = op[-2].value.intval;
	if ( op[-2].value.intval != state )
		return e_rangecheck;	/* state value was truncated */
	check_read_type(op[-1], t_string);
	check_write_type(*op, t_string);
	if ( op->size < op[-1].size )
		return e_rangecheck;
	(void) (*proc)(op->value.bytes, op[-1].value.bytes, op[-1].size,
		       &state);		/* can't fail */
	op[-2].value.intval = state;
	op[-1] = *op;
	op[-1].size = op->size;
	r_set_attrs(op - 1, a_subrange);
	pop(1);
	return 0;
}

/* usertime */
int
zusertime(register ref *op)
{	long date_time[2];
	gs_get_clock(date_time);
	push(1);
	make_int(op, date_time[0] * 86400000L + date_time[1]);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zmisc_op_init()
{	static op_def my_defs[] = {
		{"1bind", zbind},
		{"0currenttime", zcurrenttime},
		{"1getenv", zgetenv},
		{"2setdebug", zsetdebug},
		{"3type1encrypt", ztype1encrypt},
		{"3type1decrypt", ztype1decrypt},
		{"0usertime", zusertime},
		op_def_end
	};
	z_op_init(my_defs);
}
