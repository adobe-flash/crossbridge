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

/* zfont.c */
/* Font operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
/*
 * The following lines used to say:
 *	#include "gsmatrix.h"
 *	#include "gxdevice.h"		/. for gxfont.h ./
 * Tony Li says the longer list is necessary to keep the GNU compiler
 * happy, but this is pretty hard to understand....
 */
#include	"gxfixed.h"
#include	"gxmatrix.h"
#include	"gzstate.h"		/* must precede gxdevice */
#include	"gxdevice.h"		/* must precede gxfont */
#include	"gschar.h"
#include "gxfont.h"
#include "gxfont1.h"
#include "alloc.h"
#include "font.h"
#include "dict.h"
#include "name.h"
#include "state.h"
#include "store.h"

/* Forward references */
private int font_param(P2(ref *, gs_font **));
private int add_FID(P2(ref *, gs_font *));
private int make_font(P2(ref *, gs_matrix *));
private void make_uint_array(P3(ref *, uint *, int));

/* Imported from util.h */
extern int num_params(P3(ref *, int, float *));

/* The (global) font directory */
private gs_font_dir *ifont_dir;

/* Import systemdict for looking up FontDirectory */
extern ref dstack[];
#define systemdict (dstack[0])

/* Type 1 auxiliary procedures (defined in zchar.c) */
extern int z1_subr_proc(P3(gs_type1_data *, int, byte **));
extern int z1_pop_proc(P2(gs_type1_data *, fixed *));

/* Global font-related objects */
private ref name_FontDirectory;
/* Names of system-known keys in font dictionaries: */
private ref name_FontMatrix;
private ref name_FontType;
private ref name_FontBBox;
private ref name_Encoding;		/* only needed for seac */
private ref name_PaintType;
private ref name_UniqueID;
private ref name_BuildChar;
private ref name_Type1BuildChar;
private ref name_Private;
private ref name_CharStrings;		/* only needed for seac */
private ref name_FID;
/* Names of system-known keys in type 1 font Private dictionary: */
private ref name_Subrs;
private ref name_lenIV;

/* Default value of lenIV */
#define default_lenIV 4

/* Initialize the font operators */
void
zfont_init()
{	ifont_dir = gs_font_dir_alloc(alloc, alloc_free);

	/* Create the name of the font dictionary. */
	name_enter("FontDirectory", &name_FontDirectory);

	/* Create the names of the standard elements of */
	/* a font dictionary. */
	name_enter("FontMatrix", &name_FontMatrix);
	name_enter("FontType", &name_FontType);
	name_enter("FontBBox", &name_FontBBox);
	name_enter("Encoding", &name_Encoding);
	name_enter("PaintType", &name_PaintType);
	name_enter("UniqueID", &name_UniqueID);
	name_enter("BuildChar", &name_BuildChar);
	name_enter("Type1BuildChar", &name_Type1BuildChar);
	name_enter("Private", &name_Private);
	name_enter("CharStrings", &name_CharStrings);
	name_enter("FID", &name_FID);

	/* Create the names of the known entries in */
	/* a type 1 font Private dictionary. */
	name_enter("Subrs", &name_Subrs);
	name_enter("lenIV", &name_lenIV);
}

/* definefont */
int
zdefinefont(register ref *op)
{	ref *pFontDirectory;
	int code;
	ref *pfid;
	/* Validate and unpack the font. */
	ref *pmatrix, *ptype, *pbbox, *pencoding, *pcharstrings;
	ref *ppainttype, *puniqueid, *pbuildchar;
	ref *psubrs;
	int paint_type;
	long unique_id;
	static ref no_subrs, no_charstrings;
	int lenIV;
	gs_matrix mat;
	float bbox[4];
	check_op(2);
	check_type(*op, t_dictionary);
	if ( dict_find(dstack, &name_FontDirectory, &pFontDirectory) <= 0 )
		return e_invalidfont;	/* no FontDirectory?? */
	if ( dict_find(op, &name_FontMatrix, &pmatrix) <= 0 ||
	    dict_find(op, &name_FontType, &ptype) <= 0 ||
	       r_type(ptype) != t_integer ||
	       (ulong)(ptype->value.intval) > 255 ||
	    dict_find(op, &name_FontBBox, &pbbox) <= 0 ||
	    dict_find(op, &name_Encoding, &pencoding) <= 0 ||
	    read_matrix(pmatrix, &mat) < 0
	   )
	  return e_invalidfont;
	check_array_else(*pbbox, e_invalidfont);
	check_array_else(*pencoding, e_invalidfont);
	if ( pbbox->size != 4 ||
	    num_params(pbbox->value.refs + 3, 4, bbox) < 0
	   )
	  return e_invalidfont;
	/* If no PaintType entry, set the paint_type member to -1. */
	if ( dict_find(op, &name_PaintType, &ppainttype) <= 0 )
		paint_type = -1;
	else
	   {	if ( r_type(ppainttype) != t_integer ||
		     ppainttype->value.intval < 0 ||
		     ppainttype->value.intval > 0xffff
		   )
			return e_invalidfont;
		paint_type = (int)ppainttype->value.intval;
	   }
	/* If no UniqueID entry, set the unique_id member to -1, */
	/* because the specifications say UniqueID need not be */
	/* present in all fonts -- and if it is, the legal range */
	/* is 0 to 2^24-1.  */
	if ( dict_find(op, &name_UniqueID, &puniqueid) <= 0 )
		unique_id = -1;
	else
	   {	if ( r_type(puniqueid) != t_integer ||
		     puniqueid->value.intval < 0 ||
		     puniqueid->value.intval > ((1L << 24) - 1)
		   )
			return e_invalidfont;
		unique_id = puniqueid->value.intval;
	   }
	/* In contrast to what is stated (or at least implied) in */
	/* the PostScript manual, every font in GhostScript, */
	/* other than type 1 fonts, must have a BuildChar procedure. */
	code = dict_find(op, &name_BuildChar, &pbuildchar);
	/* Pre-initialize the subrs array, just in case. */
	make_tasv(&no_subrs, t_string, 0, 0, bytes, (byte *)0),
	psubrs = &no_subrs;
	make_null(&no_charstrings);
	pcharstrings = &no_charstrings;
	if ( ptype->value.intval == 1 )
	   {	ref *pprivate;
		ref *plenIV;
		if ( code > 0 ||
		    dict_find(op, &name_CharStrings, &pcharstrings) <= 0 ||
		    r_type(pcharstrings) != t_dictionary ||
		    dict_find(op, &name_Private, &pprivate) <= 0 ||
		    r_type(pprivate) != t_dictionary
		   )
			return e_invalidfont;
		if ( dict_find(pprivate, &name_Subrs, &psubrs) > 0 )
		   {	check_array_else(*psubrs, e_invalidfont);
		   }
		if ( dict_find(pprivate, &name_lenIV, &plenIV) > 0 )
		   {	if ( r_type(plenIV) != t_integer ||
			    (ulong)(plenIV->value.intval) > 255 /* arbitrary */
			   )
				return e_invalidfont;
			lenIV = plenIV->value.intval;
		   }
		else
			lenIV = default_lenIV;
		/* Check that the UniqueIDs match.  This is part of the */
		/* Adobe protection scheme, but we may as well emulate it. */
		if ( unique_id >= 0 )
		   {	if ( dict_find(pprivate, &name_UniqueID, &puniqueid) <= 0 ||
			     r_type(puniqueid) != t_integer ||
			     puniqueid->value.intval != unique_id
			   )
				unique_id = -1;
		   }
		pbuildchar = &name_Type1BuildChar;
		r_set_attrs(pbuildchar, a_executable);
	   }
	else				/* not type 1 */
	   {	if ( code <= 0 ) return e_invalidfont;
		check_proc(*pbuildchar);
	   }
	code = dict_find(op, &name_FID, &pfid);
	if ( r_attrs(op) & a_write )
	   {	/* Assume this is a new font */
		gs_font *pfont;
		font_data *pdata;
		if ( code > 0 ) return e_invalidfont;	/* has FID already */
		if ( (pfont = (gs_font *)alloc(1, sizeof(gs_font), "definefont(font)")) == 0 ||
		     (pdata = (font_data *)alloc(1, sizeof(font_data), "definefont(data)")) == 0
		   )
		  return e_VMerror;
		if ( (code = add_FID(op, pfont)) < 0 ) return code;
		store_i(&pdata->dict, op);
		store_i(&pdata->BuildChar, pbuildchar);
		store_i(&pdata->Encoding, pencoding);
		store_i(&pdata->CharStrings, pcharstrings);
		store_i(&pdata->Subrs, psubrs);
		pdata->type1_data.subr_proc = z1_subr_proc;
		pdata->type1_data.pop_proc = z1_pop_proc;
		pdata->type1_data.proc_data = (char *)pdata;
		pdata->type1_data.lenIV = lenIV;
		pfont->base = pfont;
		pfont->dir = ifont_dir;
		pfont->client_data = (char *)pdata;
		pfont->matrix = mat;
		pfont->font_type = ptype->value.intval;
		pfont->xmin = bbox[0];
		pfont->ymin = bbox[1];
		pfont->xmax = bbox[2];
		pfont->ymax = bbox[3];
		pfont->build_char_proc = gs_no_build_char_proc;
		pfont->paint_type = paint_type;
		pfont->unique_id = unique_id;
	   }
	else
	   {	/* Assume this was made by makefont or scalefont */
		if ( code <= 0 ) return e_invalidfont;	/* no FID */
	   }
	r_clear_attrs(op, a_write);	/****** SHOULD ALTER DICT ACCESS ******/
	if ( (code = dict_put(pFontDirectory, op - 1, op)) < 0 ) return code;
	op[-1] = *op;
	pop(1);
	return 0;
}

/* scalefont */
int
zscalefont(register ref *op)
{	int code;
	float scale;
	gs_matrix mat;
	if ( (code = num_params(op, 1, &scale)) < 0 ) return code;
	if ( (code = gs_make_scaling(scale, scale, &mat)) < 0 ) return code;
	return make_font(op, &mat);
}

/* makefont */
int
zmakefont(register ref *op)
{	int code;
	gs_matrix mat;
	if ( (code = read_matrix(op, &mat)) < 0 ) return code;
	return make_font(op, &mat);
}

/* setfont */
int
zsetfont(register ref *op)
{	gs_font *pfont;
	int code = font_param(op, &pfont);
	if ( code < 0 || (code = gs_setfont(igs, pfont)) < 0 )
	  return code;
	istate.font = *op;
	pop(1);
	return code;
}

/* currentfont */
int
zcurrentfont(register ref *op)
{	push(1);
	*op = istate.font;
	return 0;
}

/* cachestatus */
int
zcachestatus(register ref *op)
{	uint status[7];
	gs_cachestatus(ifont_dir, status);
	push(7);
	make_uint_array(op - 6, status, 7);
	return 0;
}

/* setcachelimit */
int
zsetcachelimit(register ref *op)
{	int code = num_params(op, 1, (float *)0);
	long limit;
	if ( code != 1 ) return (code < 0 ? code : e_typecheck);
	limit = op->value.intval;
	if ( limit < 0 || limit > max_uint )
		return e_rangecheck;
	gs_setcachelimit(ifont_dir, (uint)limit);
	pop(1);
	return 0;
}

/* setcacheparams */
int
zsetcacheparams(register ref *op)
{	uint params[2];
	int i, code;
	for ( i = 0; i < 2 && r_type(op - i) != t_mark ; i++ )
	   {	long parm;
		code = num_params(op - i, 1, (float *)0);
		if ( code != 1 ) return (code < 0 ? code : e_typecheck);
		parm = op[-i].value.intval;
		if ( parm < 0 || parm > max_uint )
			return e_rangecheck;
		params[i] = parm;
	   }
	switch ( i )
	   {
	case 2:
		if ( (code = gs_setcachelower(ifont_dir, params[1])) < 0 )
			return code;
	case 1:
		if ( (code = gs_setcacheupper(ifont_dir, params[0])) < 0 )
			return code;
	case 0: ;
	   }
	return zcleartomark(op);
}

/* currentcacheparams */
int
zcurrentcacheparams(register ref *op)
{	uint params[2];
	params[0] = gs_currentcachelower(ifont_dir);
	params[1] = gs_currentcacheupper(ifont_dir);
	push(3);
	make_tv(op - 2, t_mark, intval, 0);
	make_uint_array(op - 1, params, 2);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zfont_op_init()
{	static op_def my_defs[] = {
		{"0currentfont", zcurrentfont},
		{"2definefont", zdefinefont},
		{"2makefont", zmakefont},
		{"2scalefont", zscalefont},
		{"1setfont", zsetfont},
		{"0cachestatus", zcachestatus},
		{"1setcachelimit", zsetcachelimit},
		{"1setcacheparams", zsetcacheparams},
		{"0currentcacheparams", zcurrentcacheparams},
		op_def_end
	};
	z_op_init(my_defs);
}

/* ------ Subroutines ------ */

/* Validate a font parameter. */
private int
font_param(ref *fp, gs_font **pfont)
{	/* Check that fp is a read-only dictionary, */
	/* and that it has a FID entry. */
	ref *pid;
	int code;
	check_type(*fp, t_dictionary);
	if ( (code = dict_find(fp, &name_FID, &pid)) < 0 ) return code;
	*pfont = pid->value.pfont;
	if ( *pfont == 0 ) return e_invalidfont; /* unregistered font */
	return 0;
}

/* Add the FID entry to a font dictionary. */
private int
add_FID(ref *fp /* t_dictionary */,  gs_font *pfont)
{	ref fid;
	make_tv(&fid, t_fontID, pfont, pfont);
	return dict_put(fp, &name_FID, &fid);
}

/* Make a transformed font (common code for makefont/scalefont). */
private int
make_font(ref *op, gs_matrix *pmat)
{	ref *fp = op - 1;
	gs_font *oldfont, *newfont, *ffont;
	ref newdict;
	ref newmat;
	int code;
	make_int(&newmat, 6);	/* for zarray */
	if ( (code = font_param(fp, &oldfont)) < 0 ||
	     (code = gs_makefont(ifont_dir, oldfont, pmat,
				 &newfont, &ffont)) < 0 ||
	     (code = dict_create(dict_maxlength(fp), &newdict)) < 0 ||
	     (code = dict_copy(fp, &newdict)) < 0 ||
	     (code = zarray(&newmat)) < 0 ||
	     (code = dict_put(&newdict, &name_FontMatrix, &newmat)) < 0 ||
	     (code = add_FID(&newdict, newfont)) < 0
	   )
	  return code;
	*(gs_matrix *)(newmat.value.refs) = newfont->matrix;
	if ( ffont )
	  { /****** SHOULD DECREMENT REFCT ******/
	  }
	*fp = newdict;
	r_clear_attrs(fp, a_write);	/****** SHOULD SET DICT ACCESS ******/
	pop(1);
	return 0;
}

/* Convert an array of (unsigned) integers to stack form. */
private void
make_uint_array(register ref *op, uint *intp, int count)
{	int i;
	for ( i = 0; i < count; i++, op++, intp++ )
		make_int(op, *intp);
}
