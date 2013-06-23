/* Copyright (C) 1990 Aladdin Enterprises.  All rights reserved.
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

/* gstype1.c */
/* Adobe Type 1 font routines for GhostScript library */
#include "math_.h"
#include "memory_.h"
#include "gx.h"
#include "gserrors.h"
#include "gxarith.h"
#include "gxfixed.h"
#include "gxmatrix.h"
#include "gzstate.h"
#include "gzdevice.h"			/* for gxchar */
#include "gxdevmem.h"			/* ditto */
#include "gzpath.h"
#include "gxchar.h"
#include "gxfont1.h"
#include "gxtype1.h"

/* Encrypt a string. */
int
gs_type1_encrypt(byte *dest, byte *src, uint len, crypt_state *pstate)
{	register crypt_state state = *pstate;
	register byte *from = src;
	register byte *to = dest;
	register uint count = len;
	while ( count )
	   {	encrypt_next(*from, state, *to);
		from++, to++, count--;
	   }
	*pstate = state;
	return 0;
}
/* Decrypt a string. */
int
gs_type1_decrypt(byte *dest, byte *src, uint len, crypt_state *pstate)
{	register crypt_state state = *pstate;
	register byte *from = src;
	register byte *to = dest;
	register uint count = len;
	while ( count )
	   {	/* If from == to, we can't use the obvious */
		/*	decrypt_next(*from, state, *to);	*/
		register byte ch = *from++;
		decrypt_next(ch, state, *to);
		to++, count--;
	   }
	*pstate = state;
	return 0;
}

/* Define the structures for the state of a Type 1 interpreter. */
/* This is the interpreter state that must be saved and restored */
/* when calling a CharString subroutine. */
typedef struct {
	byte *ip;
	crypt_state dstate;
} ip_state;
/* Define the fixed-point coefficient structure */
/* for avoiding floating point. */
#define max_coeff_bits 11		/* max coefficient in char space */
typedef struct {
	long xx, xy, yx, yy;
	int skewed;
	int shift;			/* see below */
	fixed round;			/* see below */
} fixed_coeff;
/* This is the full state of the Type 1 interpreter. */
struct gs_type1_state_s {
		/* The following are set at initialization */
	gs_show_enum *penum;		/* show enumerator */
	gs_state *pgs;			/* graphics state */
	gs_type1_data *pdata;		/* font-specific data */
	int charpath_flag;		/* 0 for show, 1 for false */
					/* charpath, 2 for true charpath */
	int paint_type;			/* 0 for fill, non-0 for stroke */
	fixed_coeff fc;			/* cached fixed coefficients */
		/* The following are updated dynamically */
#define ostack_size 24
	fixed ostack[ostack_size];	/* the Type 1 operand stack */
	int os_count;			/* # of occupied stack entries */
#define ipstack_size 10
	ip_state ipstack[ipstack_size+1];	/* control stack */
	int ips_count;			/* # of occupied entries */
	int ip_skip;			/* # of bytes to skip */
	gs_fixed_point lsb;		/* left side bearing */
	gs_fixed_point width;		/* character width */
	int seac_base;			/* base character code for seac, */
					/* or -1 */
};

/* Export the size of the structure */
int gs_type1_state_sizeof = sizeof(gs_type1_state);

/* Initialize the cached matrix in a Type 1 interpreter */
/* from the matrix in the graphics state. */
int
gs_type1_init_matrix(register gs_type1_state *pis)
{	gs_matrix ctm;
	int scale = -10000;
	int expt, shift;
	ctm = ctm_only(pis->pgs);
	pis->fc.skewed = 0;
	if ( !is_fzero(ctm.xx) )
	   {	(void)frexp(ctm.xx, &scale);
	   }
	if ( !is_fzero(ctm.xy) )
	   {	(void)frexp(ctm.xy, &expt);
		if ( expt > scale ) scale = expt;
		pis->fc.skewed = 1;
	   }
	if ( !is_fzero(ctm.yx) )
	   {	(void)frexp(ctm.yx, &expt);
		if ( expt > scale ) scale = expt;
		pis->fc.skewed = 1;
	   }
	if ( !is_fzero(ctm.yy) )
	   {	(void)frexp(ctm.yy, &expt);
		if ( expt > scale ) scale = expt;
	   }
	scale = 31 - max_coeff_bits - scale;
	pis->fc.xx = (is_fzero(ctm.xx) ? 0 : (long)ldexp(ctm.xx, scale));
	pis->fc.yy = (is_fzero(ctm.yy) ? 0 : (long)ldexp(ctm.yy, scale));
	if ( pis->fc.skewed )
	   {	pis->fc.xy = (is_fzero(ctm.xy) ? 0 : (long)ldexp(ctm.xy, scale));
		pis->fc.yx = (is_fzero(ctm.yx) ? 0 : (long)ldexp(ctm.yx, scale));
	   }
	else
		pis->fc.xy = pis->fc.yx = 0;
	shift = scale - _fixed_shift;
	pis->fc.shift = shift;
	pis->fc.round = (shift > 0 ? (fixed)1 << (shift - 1) : 0);
#ifdef DEBUG
if ( gs_debug['1'] )
   {	printf("[1]ctm: [%6g %6g %6g %6g ; %6g %6g] scale=%d\n",
		ctm.xx, ctm.xy, ctm.yx, ctm.yy, ctm.tx, ctm.ty, scale);
	printf("   fc: [%lx %lx %lx %lx] shift=%d\n",
		pis->fc.xx, pis->fc.xy, pis->fc.yx, pis->fc.yy,
		pis->fc.shift);
   }
#endif
	return 0;
}

/* Initialize a Type 1 interpreter */
int
gs_type1_init(register gs_type1_state *pis, gs_show_enum *penum,
  int charpath_flag, int paint_type, byte *str, gs_type1_data *pdata)
{	gs_state *pgs = penum->pgs;
	pis->penum = penum;
	pis->pgs = pgs;
	pis->pdata = pdata;
	pis->charpath_flag = charpath_flag;
	pis->paint_type = paint_type;
	pis->os_count = 0;
	pis->ipstack[0].ip = str;
	pis->ipstack[0].dstate = crypt_charstring_seed;
	pis->ips_count = 1;
	pis->ip_skip = pdata->lenIV;
	pis->seac_base = -1;
	gs_type1_init_matrix(pis);
	/* Set the current point of the path to the origin, */
	/* in anticipation of the initial [h]sbw. */
	{ gx_path *ppath = pgs->path;
	  ppath->position.x = pgs->ctm.tx_fixed;
	  ppath->position.y = pgs->ctm.ty_fixed;
	}
	return 0;
}

/* Tracing for type 1 interpreter */
#ifdef DEBUG
#  define dc(str) type1_trace(cip, c, str);
private void
type1_trace(byte *cip, byte c, char *str)
{	if ( gs_debug['1'] )
		printf("[1]%lx: %02x %s\n", (ulong)(cip - 1), c, str);
}
#else
#  define dc(str)
#endif

/* Continue interpreting a Type 1 CharString. */
/* If str != 0, it is taken as the byte string to interpret. */
/* Return 0 on successful completion, <0 on error, */
/* (code<<1)+1 for seac, or (N+1)<<1 for callothersubr(N). */
int
gs_type1_interpret(register gs_type1_state *pis, byte *str)
{	gs_state *pgs = pis->pgs;
	gx_path *ppath = pgs->path;
	gs_type1_data *pdata = pis->pdata;
#define clear csp = cstack - 1
	fixed cstack[ostack_size];
#define cs0 cstack[0]
#define cs1 cstack[1]
#define cs2 cstack[2]
#define cs3 cstack[3]
#define cs4 cstack[4]
#define cs5 cstack[5]
	register fixed *csp;
	ip_state *ipsp = &pis->ipstack[pis->ips_count - 1];
	int skip = pis->ip_skip;
	register byte *cip;
	register crypt_state state;		/* decryption state */
	register int c;
	int code;
	fixed_coeff fc;
	fixed	ftx = pgs->ctm.tx_fixed,
		fty = pgs->ctm.ty_fixed;
	fixed ptx = ppath->position.x, pty = ppath->position.y;
	fixed x1, y1, x2, y2;

	fc = pis->fc;

#define c_fixed(d, c)\
  (fc.shift >= 0 ? arith_rshift(fixed2int(d) * c + fc.round, fc.shift) :\
   (fixed2int(d) * c) << -fc.shift)
/****** THE FOLLOWING ARE NOT ACCURATE FOR NON-INTEGER DELTAS. ******/
/* This probably doesn't make any difference in practice. */
#define accum_x(dx)\
    ptx += c_fixed(dx, fc.xx);\
    if ( fc.skewed ) pty += c_fixed(dx, fc.xy)
#define accum_y(dy)\
    pty += c_fixed(dy, fc.yy);\
    if ( fc.skewed ) ptx += c_fixed(dy, fc.yx)
#define accum_xy(dx,dy)\
    ptx += c_fixed(dx, fc.xx);\
    pty += c_fixed(dy, fc.yy);\
    if ( fc.skewed ) pty += c_fixed(dx, fc.xy), ptx += c_fixed(dy, fc.yx)

	/* Copy the operand stack out of the saved state. */
	if ( pis->os_count == 0 )
	   {	clear;
	   }
	else
	   {	memcpy(cstack, pis->ostack, pis->os_count * sizeof(fixed));
		csp = &cstack[pis->os_count - 1];
	   }

	if ( str != 0 )
		ipsp->ip = str;
itop:	cip = ipsp->ip;
	state = ipsp->dstate;
top:	/* Skip initial random bytes */
	for ( ; skip > 0; --skip )
	   {	decrypt_next(*cip, state, c); ++cip;
	   }
	while ( 1 )
	 { decrypt_next(*cip, state, c); ++cip;
	   switch ( (char_command)c )
	   {
	case c_hstem: dc("hstem")	/* hint, ignore */
		clear; break;
	case c_vstem: dc("vstem")	/* hint, ignore */
		clear; break;
	case c_vmoveto: dc("vmoveto")
		accum_y(cs0);
		goto mto;
	case c_rlineto: dc("rlineto")
		accum_xy(cs0, cs1);
lto:		code = gx_path_add_line(ppath, ptx, pty);
moved:		if ( code < 0 ) return code;
pp:
#ifdef DEBUG
if ( gs_debug['1'] )
		printf("[1]pt=(%g,%g)\n", fixed2float(ptx), fixed2float(pty));
#endif
		clear; break;
	case c_hlineto: dc("hlineto")
		accum_x(cs0);
		goto lto;
	case c_vlineto: dc("vlineto")
		accum_y(cs0);
		goto lto;
	case c_rrcurveto: dc("rrcurveto")
		accum_xy(cs0, cs1);
		x1 = ptx, y1 = pty;
		accum_xy(cs2, cs3);
		x2 = ptx, y2 = pty;
		accum_xy(cs4, cs5);
curve:		code = gx_path_add_curve(ppath, x1, y1, x2, y2, ptx, pty);
		goto moved;
	case c_closepath: dc("closepath")
		/* Note that this does NOT reset the current point! */
		code = gx_path_close_subpath(ppath);
		if ( code < 0 ) return code;
		goto mto;		/* put the point where it was */
	case c_callsubr: dc("callsubr")
	   {	int index = fixed2int(*csp);
		byte *nip;
		code = (*pdata->subr_proc)(pdata, index, &nip);
		if ( code < 0 ) return code;
		--csp;
		ipsp->ip = cip, ipsp->dstate = state;
		++ipsp;
		cip = nip;
	   }
		state = crypt_charstring_seed;
		skip = pis->pdata->lenIV;
		goto top;
	case c_return: dc("return")
		--ipsp;
		goto itop;
	case c_escape: dc("escape:")
		decrypt_next(*cip, state, c); ++cip;
		switch ( (char_extended_command)c )
		   {
		case ce_dotsection: dc("  dotsection")	/* hint, ignore */
			clear; break;
		case ce_vstem3: dc("  vstem3")		/* hint, ignore */
			clear; break;
		case ce_hstem3: dc("  hstem3")		/* hint, ignore */
			clear; break;
		case ce_seac: dc("  seac")
			/* Do the accent now.  When it finishes */
			/* (detected in endchar), do the base character. */
			pis->seac_base = (int)(byte)fixed2int(cs3);
			/* Adjust the origin of the coordinate system */
			/* for the accent (endchar puts it back). */
			ptx = ftx, pty = fty;
			cs1 -= cs0;	 /* subtract off asb */
			accum_xy(cs1, cs2);
			ppath->position.x = ptx;
			ppath->position.y = pty;
			clear;
			/* Give control back to the caller, who must */
			/* re-invoke the interpreter with the seac string. */
			state = crypt_charstring_seed;
			skip = pis->pdata->lenIV;
			return ((int)(byte)fixed2int(cs4) << 1) + 1;
		case ce_sbw: dc("  sbw")
			pis->lsb.x = cs0, pis->lsb.y = cs1;
			pis->width.x = cs2, pis->width.y = cs3;
			goto setp;
		case ce_div: dc("  div")
			csp[-1] = float2fixed((float)csp[-1] / (float)*csp);
			--csp; break;
		case ce_testadd: dc("  testadd")
			/*
			 * NOTE: this opcode is not documented by Adobe.
			 */
			if ( csp[-2] > csp[-1] ) csp[-3] += *csp;
			csp -= 3; break;
		case ce_callothersubr: dc("  callothersubr")
		   {	int index = fixed2int(*csp);
			int scount = csp - cstack;
			/* Exit to caller */
			ipsp->ip = cip, ipsp->dstate = state;
			pis->os_count = scount;
			pis->ips_count = ipsp - &pis->ipstack[0] + 1;
			pis->ip_skip = 0;
			if ( scount )
				memcpy(pis->ostack, cstack,
				       scount * sizeof(fixed));
			return (index + 1) << 1;
		   }
		case ce_pop: dc("  pop")
			++csp;
			code = (*pdata->pop_proc)(pdata, csp);
			if ( code < 0 ) return code;
			goto pushed;
		case ce_setcurrentpoint: dc("  setcurrentpoint")
			ptx = ftx, pty = fty;
			goto setp;
		default:
			return_error(gs_error_invalidfont);
		   }
		break;
	case c_hsbw: dc("hsbw")
		pis->lsb.x = cs0, pis->lsb.y = 0;
		pis->width.x = cs1, pis->width.y = 0;
		cs1 = 0;
setp:		accum_xy(cs0, cs1);
		goto pp;
	case c_endchar: dc("endchar")
		if ( pis->seac_base >= 0 )
		   {	/* We just finished the accent of a seac. */
			/* Do the base character. */
			int base_code = pis->seac_base;
			pis->seac_base = -1;
			/* Restore the coordinate system origin */
			ppath->position.x = pgs->ctm.tx_fixed;
			ppath->position.y = pgs->ctm.ty_fixed;
			clear;
			/* Give control back to the caller, who must */
			/* re-invoke the interpreter with the seac string. */
			state = crypt_charstring_seed;
			skip = pis->pdata->lenIV;
			return (base_code << 1) + 1;
		   }
		/* Set the current point to the character origin: */
		/* the 'show' loop will take care of adding in */
		/* the width we supply to setcharwidth/cachedevice. */
		gx_path_add_point(ppath,
				  pgs->ctm.tx_fixed, pgs->ctm.ty_fixed);
		if ( pis->charpath_flag )
		   {	code = gs_setcharwidth(pis->penum,
					fixed2float(pis->width.x),
					fixed2float(pis->width.y));
			if ( code < 0 ) return code;
			/* Merge the path into its parent */
			code = gx_path_merge(ppath, pgs->saved->path);
		   }
		else
		   {	gs_rect bbox;
			code = gs_pathbbox(pgs, &bbox);
			/* Restore the current point to where it was */
			/* at the beginning of the character, so that */
			/* the right thing will happen when we copy */
			/* a just-cached character to the output. */
			gx_path_add_point(ppath, ftx, fty);
			if ( code < 0 )		/* must be a null path */
			   {	bbox.p.x = bbox.p.y = bbox.q.x = bbox.q.y = 0;
			   }
#ifdef DEBUG
if ( gs_debug['1'] )
			printf("[1]bbox=(%g,%g),(%g,%g)\n",
				bbox.p.x, bbox.p.y, bbox.q.x, bbox.q.y);
#endif
			if ( pis->paint_type )
			   {	/* Expand the bounding box to encompass */
				/* the width of the stroke, plus a little */
				/* to overcome rounding problems. */
				float adjust = gs_currentlinewidth(pgs);
				if ( adjust == 0 ) adjust = 1;
				bbox.p.x -= adjust;
				bbox.p.y -= adjust;
				bbox.q.x += adjust;
				bbox.q.y += adjust;
			   }
			code = gs_setcachedevice(pis->penum,
					fixed2float(pis->width.x),
					fixed2float(pis->width.y),
					bbox.p.x, bbox.p.y,
					bbox.q.x, bbox.q.y);
			if ( code < 0 ) return code;
			/* We've already constructed the path: */
			/* translate it so it matches the cache device. */
			gx_path_translate(pgs->path, pgs->ctm.tx_fixed - ftx,
					  pgs->ctm.ty_fixed - fty);
			if ( code < 0 ) return code;
			/******
			 ****** The trim parameter is a hack to make
			 ****** characters come out less bold, since we
			 ****** don't look at the hints.
			 ******/
			code = (pis->paint_type ? gs_stroke(pgs) :
				gs_fill_trim(pgs, float2fixed(0.2)));
		   }
		return code;
	case c_rmoveto: dc("rmoveto")
		accum_xy(cs0, cs1);
mto:		code = gx_path_add_point(ppath, ptx, pty);
		goto moved;
	case c_hmoveto: dc("hmoveto")
		accum_x(cs0);
		goto mto;
	case c_vhcurveto: dc("vhcurveto")
		accum_y(cs0);
		x1 = ptx, y1 = pty;
		accum_xy(cs1, cs2);
		x2 = ptx, y2 = pty;
		accum_x(cs3);
		goto curve;
	case c_hvcurveto: dc("hvcurveto")
		accum_x(cs0);
		x1 = ptx, y1 = pty;
		accum_xy(cs1, cs2);
		x2 = ptx, y2 = pty;
		accum_y(cs3);
		goto curve;
	/* Fill up the dispatch up to 32 */
	case c_undef0: case c_undef2:
	case c_undef15:
	case c_undef16: case c_undef17: case c_undef18: case c_undef19:
	case c_undef20: case c_undef23:
	case c_undef24: case c_undef25: case c_undef26: case c_undef27:
	case c_undef28: case c_undef29:
		return_error(gs_error_invalidfont);
	default:			/* a number */
		csp++;
		if ( c <= c_max_num1 )
			*csp = int2fixed(c_value_num1(c));
		else
		   {	byte c0;
			decrypt_next(*cip, state, c0); ++cip;
			if ( c <= c_max_num2 )
				*csp = int2fixed(c_value_num2(c, (int)c0));
			else if ( c <= c_max_num3 )
				*csp = int2fixed(c_value_num3(c, (int)c0));
			else			/* c = 255 */
			   {	byte c1, c2;
				long lw;
				decrypt_next(*cip, state, c1); ++cip;
				decrypt_next(*cip, state, c2); ++cip;
				decrypt_next(*cip, state, lw); ++cip;
				lw += (long)c0 << 24;
				lw += (uint)c1 << 16;
				lw += (uint)c2 << 8;
				*csp = int2fixed(lw);
				if ( lw != fixed2int(*csp) )
					return_error(gs_error_rangecheck);
			   }
		   }
pushed:
#ifdef DEBUG
if ( gs_debug['1'] )
		printf("[1]%d: (%d) %f\n", (int)(csp - cstack), c, fixed2float(*csp));
#endif
		;			/* in case no debug */
	   }
	 }
}

/* Pop a (fixed) number off the internal stack. */
/* The client uses this to get the arguments for an OtherSubr. */
int
gs_type1_pop(gs_type1_state *pis, fixed *pf)
{	*pf = pis->ostack[--(pis->os_count)];
	return 0;
}
