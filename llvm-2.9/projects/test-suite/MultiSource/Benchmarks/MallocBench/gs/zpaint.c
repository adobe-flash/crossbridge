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

/* zpaint.c */
/* Painting operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "estack.h"			/* for image[mask] */
#include "store.h"
#include "gsmatrix.h"
#include "gspaint.h"
#include "state.h"

/* Forward references */
private int image_setup(P3(ref *, int, int));
private int image_continue(P1(ref *));

/* erasepage */
int
zerasepage(register ref *op)
{	return gs_erasepage(igs);
}

/* fill */
int
zfill(register ref *op)
{	return gs_fill(igs);
}

/* eofill */
int
zeofill(register ref *op)
{	return gs_eofill(igs);
}

/* stroke */
int
zstroke(register ref *op)
{	return gs_stroke(igs);
}

/* colorimage */
int
zcolorimage(register ref *op)
{	int spp;			/* samples per pixel */
	int npop = 7;
	ref *procp = op - 2;
	int code;
	check_type(*op, t_integer);	/* ncolors */
	check_type(op[-1], t_boolean);	/* multiproc */
	if ( (ulong)(op->value.intval) > 4 ) return e_rangecheck;
	switch ( (spp = (int)(op->value.intval)) )
	   {
	case 1:
		pop(2);			/* WRONG if zimage fails */
		return zimage(op);
	case 3: case 4:
		break;
	default:
		return e_rangecheck;
	   }
	if ( op[-1].value.index )	/* planar format */
		npop += spp - 1,
		procp -= spp - 1,
		spp = - spp;
	check_type(procp[-2], t_integer);	/* bits/sample */
	if ( (ulong)(procp[-2].value.intval) > 8 ) return e_rangecheck;
	code = image_setup(procp, (int)procp[-2].value.intval, spp);
	if ( code >= 0 ) pop(npop);
	return code;
}

/* image */
int
zimage(register ref *op)
{	int code;
	check_type(op[-2], t_integer);	/* bits/sample */
	if ( (ulong)(op[-2].value.intval) > 8 ) return e_rangecheck;
	code = image_setup(op, (int)op[-2].value.intval, 1);
	if ( code >= 0 ) pop(5);
	return code;
}

/* imagemask */
int
zimagemask(register ref *op)
{	int code;
	check_type(op[-2], t_boolean);
	code = image_setup(op, op[-2].value.index, 0);
	if ( code >= 0 ) pop(5);
	return code;
}

/* Common setup for [color]image and imagemask. */
/* spp is 0 for imagemask, 1 for image, and [-]3 or [-]4 for colorimage. */
private int
image_setup(ref *op, int param3 /* bits/sample or invert */, int spp)
{	int code;
	gs_image_enum *penum;
	gs_matrix mat;
	int px;
	int pmax = (spp < 0 ? ~ spp : 0);
	/* We push on the estack: */
	/*	Control mark, 4 procs, last plane index, */
	/*	enumeration structure (as bytes). */
#define inumpush 7
	check_estack(inumpush + 2);	/* stuff above, + continuation + proc */
	check_type(op[-4], t_integer);	/* width */
	check_type(op[-3], t_integer);	/* height */
	/* Note that the "procedures" might not be procedures, */
	/* but might be literal strings. */
	for ( px = 0; px <= pmax; px++ )
	  if ( r_type(op + px) != t_string )
	   {	check_proc(op[px]);	/* proc(s) */
	   }
	if ( op[-4].value.intval <= 0 || op[-3].value.intval < 0 )
		return e_undefinedresult;
	if ( op[-3].value.intval == 0 ) return 0;	/* empty image */
	if ( (code = read_matrix(op - 1, &mat)) < 0 )
		return code;
	if ( (penum = (gs_image_enum *)alloc(1, gs_image_enum_sizeof, "image_setup")) == 0 )
		return e_VMerror;
	code = (spp == 0 ?
		gs_imagemask_init(penum, igs, (int)op[-4].value.intval,
				  (int)op[-3].value.intval, param3, &mat) :
		gs_image_init(penum, igs, (int)op[-4].value.intval,
			      (int)op[-3].value.intval, param3, spp, &mat) );
	if ( code < 0 ) return code;
	mark_estack(es_other);
	++esp;
	for ( px = 0; px < 4; esp++, px++ )
	  if ( px <= pmax )
		*esp = op[px];
	  else
		make_null(esp);
	make_int(esp, 0);		/* current plane */
	esp->size = pmax;
	++esp;
	make_tasv(esp, t_string, 0, gs_image_enum_sizeof, bytes, (byte *)penum);
	push_op_estack(image_continue);
	*++esp = *op;			/* run the (first) procedure */
	return o_check_estack;
}
/* Continuation procedure.  Hand the string to the enumerator. */
private int
image_continue(register ref *op)
{	gs_image_enum *penum = (gs_image_enum *)esp->value.bytes;
	int code;
	if ( r_type(op) != t_string )
	   {	/* Procedure didn't return a string.  Quit. */
		esp -= inumpush;
		alloc_free((char *)penum, 1, gs_image_enum_sizeof,
			   "image_continue(quit)");
		return e_typecheck;
	   }
	code = gs_image_next(penum, op->value.bytes, op->size);
	if ( op->size == 0 || code > 0 )	/* stop now */
	   {	esp -= inumpush;
		alloc_free((char *)penum, 1, gs_image_enum_sizeof,
			   "image_continue(finished)");
	   }
	else
	   {	int px = (int)++(esp[-1].value.intval);
		ref *pproc = esp - 5;
		if ( px > esp[-1].size )
			esp[-1].value.intval = px = 0;
		push_op_estack(image_continue);
		*++esp = pproc[px];
	   }
	pop(1);
	return o_check_estack;
}

/* ------ Initialization procedure ------ */

void
zpaint_op_init()
{	static op_def my_defs[] = {
		{"0eofill", zeofill},
		{"0erasepage", zerasepage},
		{"0fill", zfill},
		{"7colorimage", zcolorimage},
		{"5image", zimage},
		{"5imagemask", zimagemask},
		{"0stroke", zstroke},
		op_def_end
	};
	z_op_init(my_defs);
}
