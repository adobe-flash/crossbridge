/* Copyright (C) 1989 Aladdin Enterprises.  All rights reserved.
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

/* zmatrix.c */
/* Matrix operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "gsmatrix.h"
#include "state.h"
#include "gscoord.h"
#include "store.h"

/* Forward references */
private int common_transform(P3(ref *,
  int (*)(P4(gs_state *, floatp, floatp, gs_point *)),
  int (*)(P4(floatp, floatp, gs_matrix *, gs_point *))));

/* Imported from util.c */
extern int num_params(P3(ref *, int, float *));

/* Initialize the type and attributes of the identity matrix */
void
zmatrix_init()
{	extern gs_matrix gs_identity_matrix;
	ref *mp = (ref *)&gs_identity_matrix;
	int i;
	for ( i = 0; i < 6; i++, mp++ )
		make_real(mp, mp->value.realval);
}

/* currentmatrix */
int
zcurrentmatrix(register ref *op)
{	int code = write_matrix(op);
	if ( code < 0 ) return code;
	gs_currentmatrix(igs, (gs_matrix *)(op->value.refs));
	return 0;
}

/* setmatrix */
int
zsetmatrix(register ref *op)
{	gs_matrix mat;
	int code = read_matrix(op, &mat);
	if ( code < 0 ) return code;
	if ( (code = gs_setmatrix(igs, &mat)) < 0 ) return code;
	pop(1);
	return 0;
}

/* translate */
int
ztranslate(register ref *op)
{	int code = write_matrix(op);
	float trans[2];
	if ( code < 0 )			/* no matrix operand */
	   {	if ( (code = num_params(op, 2, trans)) < 0 ) return code;
		code = gs_translate(igs, trans[0], trans[1]);
	   }
	else				/* matrix operand */
	   {	gs_matrix *pmat = (gs_matrix *)op->value.refs;
		if ( (code = num_params(op - 1, 2, trans)) < 0 ) return code;
		code = gs_make_translation(trans[0], trans[1], pmat);
		op[-2] = *op;
	   }
	if ( code >= 0 ) pop(2);
	return code;
}

/* scale */
int
zscale(register ref *op)
{	float scale[2];
	int code = write_matrix(op);
	if ( code < 0 )			/* no matrix operand */
	   {	if ( (code = num_params(op, 2, scale)) < 0 ) return code;
		code = gs_scale(igs, scale[0], scale[1]);
	   }
	else				/* matrix operand */
	   {	gs_matrix *pmat = (gs_matrix *)op->value.refs;
		if ( (code = num_params(op - 1, 2, scale)) < 0 ) return code;
		code = gs_make_scaling(scale[0], scale[1], pmat);
		op[-2] = *op;
	   }
	if ( code >= 0 ) pop(2);
	return code;
}

/* rotate */
int
zrotate(register ref *op)
{	int code = write_matrix(op);
	float ang;
	if ( code < 0 )			/* no matrix operand */
	   {	if ( (code = num_params(op, 1, &ang)) < 0 ) return code;
		code = gs_rotate(igs, ang);
	   }
	else				/* matrix operand */
	   {	gs_matrix *pmat = (gs_matrix *)op->value.refs;
		if ( (code = num_params(op - 1, 1, &ang)) < 0 ) return code;
		code = gs_make_rotation(ang, pmat);
		op[-1] = *op;
	   }
	if ( code >= 0 ) pop(1);
	return code;
}

/* concat */
int
zconcat(register ref *op)
{	gs_matrix mat;
	int code = read_matrix(op, &mat);
	if ( code < 0 ) return code;
	code = gs_concat(igs, &mat);
	if ( code < 0 ) return code;
	pop(1);
	return 0;
}

/* concatmatrix */
int
zconcatmatrix(register ref *op)
{	gs_matrix m1, m2;
	int code;
	if (	(code = read_matrix(op - 2, &m1)) < 0 ||
		(code = read_matrix(op - 1, &m2)) < 0 ||
		(code = write_matrix(op)) < 0 ||
		(code = gs_matrix_multiply(&m1, &m2, (gs_matrix *)(op->value.refs))) < 0
	   ) return code;
	op[-2] = *op;
	pop(2);
	return code;
}

/* transform */
int
ztransform(register ref *op)
{	return common_transform(op, gs_transform, gs_point_transform);
}

/* dtransform */
int
zdtransform(register ref *op)
{	return common_transform(op, gs_dtransform, gs_distance_transform);
}

/* itransform */
int
zitransform(register ref *op)
{	return common_transform(op, gs_itransform, gs_point_transform_inverse);
}

/* idtransform */
int
zidtransform(register ref *op)
{	return common_transform(op, gs_idtransform, gs_distance_transform_inverse);
}

/* Common logic for [i][d]transform */
private int
common_transform(register ref *op,
  int (*ptproc)(P4(gs_state *, floatp, floatp, gs_point *)),
  int (*matproc)(P4(floatp, floatp, gs_matrix *, gs_point *)))
{	float opxy[2];
	gs_point pt;
	int code;
	/* Optimize for the non-matrix case */
	switch ( r_type(op) )
	   {
	case t_real: opxy[1] = op->value.realval; break;
	case t_integer: opxy[1] = op->value.intval; break;
	case t_array: case t_packedarray:	/* might be a matrix */
	   {	gs_matrix mat;
		gs_matrix *pmat = &mat;
		if (	(code = read_matrix(op, pmat)) < 0 ||
			(code = num_params(op - 1, 2, opxy)) < 0 ||
			(code = (*matproc)(opxy[0], opxy[1], pmat, &pt)) < 0
		   ) return code;
		op--;
		pop(1);
		goto out;
	   }
	default: return e_typecheck;
	   }
	switch ( r_type(op - 1) )
	   {
	case t_real: opxy[0] = (op - 1)->value.realval; break;
	case t_integer: opxy[0] = (op - 1)->value.intval; break;
	default: return e_typecheck;
	   }
	if ( (code = (*ptproc)(igs, opxy[0], opxy[1], &pt)) < 0 )
		return code;
out:	make_real(op - 1, pt.x);
	make_real(op, pt.y);
	return 0;
}

/* invertmatrix */
int
zinvertmatrix(register ref *op)
{	gs_matrix m;
	int code;
	if (	(code = read_matrix(op - 1, &m)) < 0 ||
		(code = write_matrix(op)) < 0 ||
		(code = gs_matrix_invert(&m, (gs_matrix *)op->value.refs)) < 0
	   ) return code;
	op[-1] = *op;
	pop(1);
	return code;
}

/* ------ Initialization procedure ------ */

void
zmatrix_op_init()
{	static op_def my_defs[] = {
		{"1concat", zconcat},
		{"2dtransform", zdtransform},
		{"3concatmatrix", zconcatmatrix},
		{"1currentmatrix", zcurrentmatrix},
		{"2idtransform", zidtransform},
		{"2invertmatrix", zinvertmatrix},
		{"2itransform", zitransform},
		{"1rotate", zrotate},
		{"2scale", zscale},
		{"1setmatrix", zsetmatrix},
		{"2transform", ztransform},
		{"2translate", ztranslate},
		op_def_end
	};
	z_op_init(my_defs);
}
