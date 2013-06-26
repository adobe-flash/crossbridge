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

/* zdevice.c */
/* Device-related operators for GhostScript */
#include "ghost.h"
#include "alloc.h"
#include "errors.h"
#include "oper.h"
#include "state.h"
#include "gsmatrix.h"
#include "gsstate.h"
#include "gxdevice.h"
#include "store.h"

/* copypage */
int
zcopypage(register ref *op)
{	return gs_copypage(igs);
}

/* copyscanlines */
int
zcopyscanlines(register ref *op)
{	ref *op1 = op - 1;
	ref *op2 = op - 2;
	gx_device *dev;
	int code;
	uint bytes_copied;
	check_type(*op2, t_device);
	dev = op2->value.pdevice;
	check_type(*op1, t_integer);
	if ( op1->value.intval < 0 || op1->value.intval > dev->height )
		return e_rangecheck;
	check_write_type(*op, t_string);
	code = gs_copyscanlines(dev, (int)op1->value.intval,
		op->value.bytes, op->size, NULL, &bytes_copied);
	if ( code < 0 ) return e_typecheck;	/* not a memory device */
	*op2 = *op;
	op2->size = bytes_copied;
	r_set_attrs(op2, a_subrange);
	pop(2);
	return 0;
}

/* currentdevice */
int
zcurrentdevice(register ref *op)
{	gx_device *dev = gs_currentdevice(igs);
	push(1);
	make_tv(op, t_device, pdevice, dev);
	return 0;
}

/* devicename */
int
zdevicename(register ref *op)
{	char *dname;
	int code;
	check_type(*op, t_device);
	dname = gs_devicename(op->value.pdevice);
	code = string_to_ref(dname, op, "devicename");
	if ( code < 0 ) return code;
	return 0;
}

/* deviceparams */
int
zdeviceparams(register ref *op)
{	int code = write_matrix(op);
	ref *pmat;
	gs_matrix imat;
	int width, height;
	if ( code < 0 ) return code;
	check_type(op[-1], t_device);
	gs_deviceparams(op[-1].value.pdevice, &imat, &width, &height);
	/* Transfer the values to the matrix */
	pmat = op->value.refs;
	make_real(pmat+0, imat.xx); make_real(pmat+1, imat.xy);
	make_real(pmat+2, imat.yx); make_real(pmat+3, imat.yy);
	make_real(pmat+4, imat.tx); make_real(pmat+5, imat.ty);
	push(2);
	make_mark(op - 3);
	make_int(op - 1, width);
	make_int(op, height);
	return 0;
}

/* flushpage */
int
zflushpage(register ref *op)
{	return gs_flushpage(igs);
}

/* getdevice */
int
zgetdevice(register ref *op)
{	gx_device *dev;
	check_type(*op, t_integer);
	if ( op->value.intval != (int)(op->value.intval) )
		return e_rangecheck;	/* won't fit in an int */
	dev = gs_getdevice((int)(op->value.intval));
	if ( dev == 0 ) return e_rangecheck;	/* index out of range */
	make_tv(op, t_device, pdevice, dev);
	return 0;
}

/* makedevice */
int
zmakedevice(register ref *op)
{	gs_matrix imat;
	gx_device *new_dev;
	int code;
	check_type(op[-3], t_device);
	check_type(op[-1], t_integer);	/* width */
	check_type(*op, t_integer);	/* height */
	if (	(ulong)(op[-1].value.intval) > max_uint >> 1 ||
		(ulong)(op->value.intval) > max_uint >> 1
	   ) return e_rangecheck;
	if ( (code = read_matrix(op - 2, &imat)) < 0 ) return code;
	/* Everything OK, create device */
	code = gs_makedevice(&new_dev, op[-3].value.pdevice, &imat,
			     (int)op[-1].value.intval, (int)op->value.intval,
			     alloc);
	if ( code == 0 )
	   {	make_tv(op - 3, t_device, pdevice, new_dev);
		pop(3);
	   }
	return code;
}

/* makeimagedevice */
int
zmakeimagedevice(register ref *op)
{	gs_matrix imat;
	gx_device *new_dev;
	float colors[256 * 3];
	int num_colors;
	int code;
	check_type(op[-2], t_integer);	/* width */
	check_type(op[-1], t_integer);	/* height */
	if ( r_type(op) == t_null )	/* true color */
	   {	num_colors = -24;	/* 24-bit true color */
	   }
	else
	   {	check_array(*op);	/* palette */
		num_colors = op->size;
	   }
	if (	(ulong)(op[-2].value.intval) > max_uint >> 1 ||
		(ulong)(op[-1].value.intval) > max_uint >> 1 ||
		num_colors > 256
	   ) return e_rangecheck;
	if ( (code = read_matrix(op - 3, &imat)) < 0 ) return code;
	/* Check and convert colors */
	   {	int i;
		ref *pc = op->value.refs;
		float *p = colors;
		for ( i = 0; i < num_colors; i++, pc++, p += 3 )
		   {	check_type(*pc, t_color);
			code = gs_colorrgb(pc->value.pcolor, p);
			if ( code < 0 ) return code;
		   }
	   }
	/* Everything OK, create device */
	code = gs_makeimagedevice(&new_dev, &imat,
				  (int)op[-2].value.intval,
				  (int)op[-1].value.intval,
				  colors, num_colors, alloc);
	if ( code == 0 )
	   {	make_tv(op - 3, t_device, pdevice, new_dev);
		pop(3);
	   }
	return code;
}

/* .nulldevice */
int
znulldevice(register ref *op)
{	gs_nulldevice(igs);
	return 0;
}

/* .setdevice */
int
zsetdevice(register ref *op)
{	int code;
	check_type(*op, t_device);
	code = gs_setdevice(igs, op->value.pdevice);
	if ( code == 0 ) pop(1);
	return code;
}

/* ------ Initialization procedure ------ */

void
zdevice_op_init()
{	static op_def my_defs[] = {
		{"0copypage", zcopypage},
		{"3copyscanlines", zcopyscanlines},
		{"0currentdevice", zcurrentdevice},
		{"1devicename", zdevicename},
		{"1deviceparams", zdeviceparams},
		{"0flushpage", zflushpage},
		{"1getdevice", zgetdevice},
		{"4makedevice", zmakedevice},
		{"4makeimagedevice", zmakeimagedevice},
		{"0.nulldevice", znulldevice},
		{"1.setdevice", zsetdevice},
		op_def_end
	};
	z_op_init(my_defs);
}
