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

/* ghost.h */
/* Common definitions for GhostScript */
#include "gx.h"

/* The typedef for object references */
typedef struct ref_s ref;

/* Operator procedure.  Returns an error code or 0 for success. */
/* We would like to define */
/*	typedef int op_proc_p(P1(ref *));	*/
/* but the standard Unix C compiler can't handle a typedef with a */
/* procedure type, so we have to work around it.... */
#define op_proc_(proc) int proc(P1(ref *))
typedef int (*op_proc_p)(P1(ref *));

/*
 * Object types.  This should be an enum, but there is no way
 * to declare an enum a subrange of byte rather than int....
 */
#define t_array 0			/* value.refs, uses size */
#define t_boolean 1			/* value.index */
#define t_dictionary 2			/* value.pdict */
#define t_file 3			/* value.pfile, also uses size */
#define t_fontID 4			/* value.pfont */
#define t_integer 5			/* value.intval */
#define t_mark 6			/* (no value) */
#define t_name 7			/* value.pname */
#define t_null 8			/* (no value) */
#define t_operator 9			/* value.opproc, also uses size */
#define t_packedarray 10		/* value.refs, uses size */
#define t_real 11			/* value.realval */
#define t_save 12			/* value.psave */
#define t_string 13			/* value.bytes, uses size */
/*
 * The following are extensions to the PostScript type set.
 * When adding new types, be sure to edit the table in ghost.ps
 * (type and ==only operators), the printing routine in idebug.c,
 * and also type_print_strings below.
 */
#define t_color 14			/* value.pcolor */
#define t_device 15			/* value.pdevice */
#define t_next_index 16		/* *** change this if adding types *** */
/*
 * The interpreter uses types starting at t_next_index for representing
 * a few high-frequency operators.
 * Since there are no operations specifically on operators,
 * there is no need for any operators to check specifically for these
 * types.  The r_btype macro takes care of the conversion when required.
 *
 * Define the type names for debugging printout.
 * All names must be the same length, so that columns will line up.
 */
#define type_print_strings\
  "arry","bool","dict","file","font","int ","mark","name","null","oper",\
  "pary","real","save","str ","colr","devc"

/* Object attributes */
#define a_executable 1			/* must be 1 or 2 */
#define a_subrange 0x8000		/* subarray or substring */
#define a_write 0x100			/* must be >= 0x100 */
#define a_read 0x200			/* must be >= 0x100 */
#define a_execute 2			/* must be 1 or 2 */
#define a_all (a_write+a_read+a_execute)
/* Define the attribute names for debugging printout. */
#define attr_print_string "xe......wr?????s"

/* Abstract types */
typedef struct dict_s dict;
typedef struct name_s name;

/* Object reference */
struct ref_s {
	union v {			/* name the union to keep gdb happy */
		long intval;
		ushort index;		/* for enumerated things */
		float realval;
		byte *bytes;
		struct ref_s *refs;
		name *pname;
		dict *pdict;
		op_proc_p opproc;
		struct file_entry_s *pfile;
		struct gs_font_s *pfont;
		struct gs_color_s *pcolor;
		struct gx_device_s *pdevice;
		struct vm_save_s *psave;
	} value;
/* type_attrs is a single element for fast dispatching in the interpreter */
#define r_type(rp) (((rp)->type_attrs & 0xff) >> 2)
#define r_has_type(rp,typ) (((rp)->type_attrs & 0xfc) == ((typ) << 2))
#define r_set_type(rp,typ) ((rp)->type_attrs = (typ) << 2)
#define r_btype(rp) (r_type(rp) >= t_next_index ? t_operator : r_type(rp))
#define r_type_xe(rp) ((rp)->type_attrs & 0xff)
#define r_attrs(rp) ((rp)->type_attrs)	/* reading only */
#define r_has_attrs(rp,mask) !(~r_attrs(rp) & (mask))
#define r_set_attrs(rp,mask) ((rp)->type_attrs |= (mask))
#define r_clear_attrs(rp,mask) ((rp)->type_attrs &= ~(mask))
#define r_set_type_attrs(rp,typ,mask) ((rp)->type_attrs = ((typ) << 2) + (mask))
#define null_type_attrs (t_null << 2)
	ushort type_attrs;
	ushort size;
};
