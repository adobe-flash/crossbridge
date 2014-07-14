/* Copyright (C) 1989, 1990, 1991 Aladdin Enterprises.  All rights reserved.
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

/* iscan.c */
/* Token scanner for Ghostscript interpreter */
#include <ctype.h>
#include "memory_.h"
#include "ghost.h"
#include "arch.h"
#include "alloc.h"
#include "dict.h"			/* for //name lookup */
#include "errors.h"
#include "name.h"
#include "store.h"
#include "stream.h"
#include "scanchar.h"
#include <stdint.h>

/* Array packing flag */
int array_packing;

/* Forward references */
private	int	scan_hex_string(P2(stream *, ref *)),
		scan_int(P4(stream *, int, long *, double *)),
		scan_string(P3(stream *, int, ref *));

/* Import the dictionary stack for //name lookup */
extern ref dstack[];
extern ref *dsp;

/* Import the operand stack for accumulating procedure bodies */
extern ref *osp, *ostop;

/* Static constants */
private ref left_bracket;
private ref right_bracket;
private ref empty_name;

/* Define the character scanning table (see scanchar.h). */
byte scan_char_array[257];

/* A structure for dynamically growable objects */
typedef struct dynamic_area_s {
	byte *base;
	byte *next;
	uint num_elts;
	uint elt_size;
	int is_dynamic;			/* false if using fixed buffer */
	byte *limit;
} dynamic_area;

/* Begin a dynamic object. */
/* dynamic_begin returns the value of alloc_dynamic, which may be 0: */
/* the invoker of dynamic_begin must test the value against 0. */
#define dynamic_begin(pda, dnum, desize)\
	((pda)->base = alloc_dynamic((pda)->num_elts = (dnum),\
				     (pda)->elt_size = (desize), "scanner"),\
	 (pda)->limit = (pda)->base + (dnum) * (desize),\
	 (pda)->is_dynamic = 1,\
	 (pda)->next = (pda)->base)

/* Grow a dynamic object */
private int
dynamic_grow(register dynamic_area *pda)
{	uint num = pda->num_elts;
	uintptr_t size = num * pda->elt_size;
	uintptr_t new_num;
	uint pos = pda->next - pda->base;
	size = (size < 10 ? 20 : size >= (max_uint >> 1) ? UINTPTR_MAX : size << 1);
	new_num = size / pda->elt_size;
	if ( pda->is_dynamic )
	   {	pda->base = alloc_grow(pda->base, num, new_num, pda->elt_size, "scanner");
		if ( pda->base == 0 ) return 0;
		pda->num_elts = new_num;
		pda->limit = pda->base + size;
	   }
	else
	   {	byte *base = pda->base;
		uint old_size = size;
		if ( !dynamic_begin(pda, new_num, pda->elt_size) ) return 0;
		memcpy(pda->base, base, old_size);
		pda->is_dynamic = 1;
	   }
	pda->next = pda->base + pos;
	return 1;
}

/* Get rid of an unwanted dynamic object */
#define dynamic_free(pda)\
  if ( (pda)->is_dynamic )\
    alloc_free((char *)((pda)->base), (pda)->num_elts, (pda)->elt_size, "scanner")

/* Initialize the scanner. */
void
scan_init()
{	register byte *decoder = scan_char_decoder;
	/* Precompute left and right bracket and empty name tokens */
	   {	name_ref((byte *)"[", 1, &left_bracket, 0);
		name_ref((byte *)"]", 1, &right_bracket, 0);
		name_ref((byte *)"", 0, &empty_name, 0);
	   }
	/* Initialize decoder array */
	   {	static char stop_chars[] = "()<>[]{}/%";
		static char space_chars[] = " \f\t\n\r";
		register int i;
		char *p;
		decoder[-1] = ctype_eof;
		memset(decoder, ctype_name, 256);
		for ( p = space_chars; *p; p++ )
		  decoder[*p] = ctype_space;
		decoder[char_NULL] = decoder[char_VT] =
		  decoder[char_DOS_EOF] = ctype_space;
		for ( p = stop_chars; *p; p++ )
		  decoder[*p] = ctype_other;
		for ( i = 0; i < 10; i++ )
			decoder['0' + i] = i;
		for ( i = 0; i < max_radix - 10; i++ )
			decoder['A' + i] = decoder['a' + i] = i + 10;
	   }
	array_packing = 0;
}

/* Read a token from a stream. */
/* Return 1 for end-of-stream, 0 if a token was read, */
/* or a (negative) error code. */
/* If the token required a terminating character (i.e., was a name or */
/* number) and the next character was whitespace, read and discard */
/* that character: see the description of the 'token' operator on */
/* p. 232 of the Red Book. */
/* from_string indicates reading from a string vs. a file, */
/* because \ escapes are not recognized in the former case. */
/* (See the footnote on p. 23 of the Red Book.) */
int
scan_token(register stream *s, int from_string, ref *pref)
{	ref *myref = pref;
	dynamic_area proc_da;
	int pstack = 0;		/* offset from proc_da.base */
	int retcode = 0;
	register int c;
	int name_type;			/* number of /'s preceding */
	int try_number;
	register byte *decoder = scan_char_decoder;
	/******
	 ****** Newer P*stScr*pt interpreters don't use from_string.
	 ******/
	from_string = 0;
top:	c = sgetc(s);
#ifdef DEBUG
if ( gs_debug['s'] )
	printf((c >= 32 && c <= 126 ? "`%c'" : "`%03o'"), c);
#endif
	switch ( c )
	   {
	case ' ': case '\f': case '\t': case '\n': case '\r':
	case char_NULL: case char_VT: case char_DOS_EOF:
		goto top;
	case '[':
		*myref = left_bracket;
		r_set_attrs(myref, a_executable);
		break;
	case ']':
		*myref = right_bracket;
		r_set_attrs(myref, a_executable);
		break;
	case '<':
		retcode = scan_hex_string(s, myref);
		break;
	case '(':
		retcode = scan_string(s, from_string, myref);
		break;
	case '{':
		if ( pstack == 0 )
		   {	/* Use the operand stack to accumulate procedures. */
			myref = osp + 1;
			proc_da.base = (byte *)myref;
			proc_da.limit = (byte *)(ostop + 1);
			proc_da.is_dynamic = 0;
			proc_da.elt_size = sizeof(ref);
			proc_da.num_elts = ostop - osp;
		   }
		if ( proc_da.limit - (byte *)myref < 2 * sizeof(ref) )
		   {	proc_da.next = (byte *)myref;
			if ( !dynamic_grow(&proc_da) )
			  return e_VMerror;
			myref = (ref *)proc_da.next;
		   }
		myref->size = pstack;
		myref++;
		pstack = (byte *)myref - proc_da.base;
		goto top;
	case '>':
	case ')':
		retcode = e_syntaxerror;
		break;
	case '}':
		if ( pstack == 0 )
		   {	retcode = e_syntaxerror;
			break;	
		   }
		   {	ref *ref0 = (ref *)(proc_da.base + pstack);
			uint size = myref - ref0;
			ref *aref;
			myref = ref0 - 1;
			pstack = myref->size;
			if ( pstack == 0 && proc_da.is_dynamic )
			   {	/* Top-level procedure, shrink in place. */
				memcpy(myref, ref0, size * sizeof(ref));
				aref = (ref *)alloc_shrink((byte *)myref, proc_da.num_elts, size, sizeof(ref), "scanner(top proc)");
				if ( aref == 0 ) return e_VMerror;
				myref = pref;
			   }
			else
			   {	/* Not top-level, or in ostack: copy it. */
				aref = (ref *)alloc(size, sizeof(ref), "scanner(proc)");
				if ( aref == 0 ) return e_VMerror;
				memcpy(aref, ref0, size * sizeof(ref));
				if ( pstack == 0 ) myref = pref;
			   }
			if ( array_packing )
				make_tasv(myref, t_packedarray, a_executable + a_read + a_execute, size, refs, aref);
			else
				make_tasv(myref, t_array, a_executable + a_all, size, refs, aref);
		   }
		break;
	case '/':
		c = sgetc(s);
		if ( c == '/' )
		   {	name_type = 2;
			c = sgetc(s);
		   }
		else
			name_type = 1;
		try_number = 0;
		switch ( decoder[c] )
		   {
		case ctype_name:
		default:
			goto do_name;
		case ctype_eof:
			/* Empty name: bizarre but legitimate. */
			*myref = empty_name;
			goto have_name;
		case ctype_other:
			switch ( c )
			   {
			case '[':	/* only special as first character */
				*myref = left_bracket;
				goto have_name;
			case ']':	/* ditto */
				*myref = right_bracket;
				goto have_name;
			default:
				/* Empty name: bizarre but legitimate. */
				*myref = empty_name;
				sputback(s);
				goto have_name;
			   }
		case ctype_space:
			/* Empty name: bizarre but legitimate. */
			*myref = empty_name;
			/* Check for \r\n */
			if ( c == '\r' && (c = sgetc(s)) != '\n' && c != EOFC )
				sputback(s);
			goto have_name;
		   }
		/* NOTREACHED */
	case '%':
	   {	int c1;
		do { c = sgetc(s); }
		while ( c != '\f' && c != '\n' && c != '\r' && c != EOFC );
		if ( c == '\r' && (c1 = sgetc(s)) != '\n' && c1 != EOFC )
			sputback(s);
		if ( c != EOFC ) goto top;
	   }	/* falls through */
	case EOFC:
		retcode = (pstack != 0 ? e_syntaxerror : 1);
		break;
	/* Handle separately the names that might be a number */
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '.': case '+': case '-':
		try_number = 1;
		name_type = 0;
		goto do_name;
	/* Check for a binary object */
	default:			/* ****** NYI ****** */
	/* Handle the common cases (letters and _) explicitly, */
	/* rather than going through the default test. */
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
	case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
	case 'n': case 'o': case 'p': case 'q': case 'r': case 's':
	case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
	case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
	case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S':
	case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
	case '_':
		try_number = 0;
		name_type = 0;
		/* Common code for scanning a name. */
		/* try_number and name_type are already set. */
		/* We know c has ctype_name or is a digit. */
do_name:
	   {	dynamic_area da;
		/* Try to scan entirely within the stream buffer. */
		/* We stop 1 character early, so we don't switch buffers */
		/* looking ahead if the name is terminated by \r\n. */
		register byte *ptr = sbufptr(s);
		byte *end = sbufend(s) - 1;
		da.base = ptr - 1;
		da.is_dynamic = 0;
		do
		   {	if ( ptr >= end )
			   {	ssetbufptr(s, ptr);
				/* Initialize the dynamic area. */
				/* We have to do this before the next */
				/* sgetc, which will overwrite the buffer. */
				da.next = da.limit = ptr;
				da.num_elts = ptr - da.base;
				da.elt_size = 1;
				if ( !dynamic_grow(&da) ) return e_VMerror;
				ptr = da.next;
				goto dyn_name;
			   }
			c = *ptr++;
		   }
		while ( decoder[c] <= ctype_name );	/* digit or name */
		/* Name ended within the buffer. */
		ssetbufptr(s, ptr);
		ptr--;
		goto nx;
		/* Name overran buffer. */
dyn_name:	while ( decoder[c = sgetc(s)] <= ctype_name )
		  {	if ( ptr == da.limit )
			   {	da.next = ptr;
				if ( !dynamic_grow(&da) )
				  return e_VMerror;
				ptr = da.next;
			   }
			*ptr++ = c;
		   }
nx:		switch ( decoder[c] )
		  {
		  case ctype_other:
			sputback(s);
		  case ctype_space:
			/* Check for \r\n */
			if ( c == '\r' && (c = sgetc(s)) != '\n' && c != EOFC )
				sputback(s);
		  case ctype_eof: ;
		  }
		/* Check for a number */
		if ( try_number )
		   {	stream nst;
			stream *ns = &nst;
			sread_string(ns, da.base, (uint)(ptr - da.base));
			retcode = scan_number(ns, myref);
			if ( retcode != e_syntaxerror )
			   {	dynamic_free(&da);
				goto have_name;	/* might be e_limitcheck */
			   }
		   }
		retcode = name_ref(da.base, (uint)(ptr - da.base), myref, 1);
		dynamic_free(&da);
	   }
		/* Done scanning.  Check for preceding /'s. */
have_name:	if ( retcode < 0 ) return retcode;
		switch ( name_type )
		   {
		case 0:			/* ordinary executable name */
			if ( r_type(myref) == t_name )	/* i.e., not a number */
			  r_set_attrs(myref, a_executable);
		case 1:			/* quoted name */
			break;
		case 2:			/* immediate lookup */
		   {	ref *pvalue;
			if ( dict_lookup(dstack, dsp, myref, &pvalue) <= 0 )
				return e_undefined;
			store(myref, *pvalue);
		   }
		   }
	   }
	/* If we are the top level, return the object, otherwise keep going */
	if ( pstack == 0 || retcode < 0 )
	  return retcode;
	if ( proc_da.limit - (byte *)myref < 2 * sizeof(ref) )
	   {	proc_da.next = (byte *)myref;
		if ( !dynamic_grow(&proc_da) )
		  return e_VMerror;		/* out of room */
		myref = (ref *)proc_da.next;
	   }
	myref++;
	goto top;
}

/* The internal scanning procedures return 0 on success, */
/* or a (negative) error code on failure. */

/* Procedure to scan a number.  This is also called by cvi and cvr. */
int
scan_number(register stream *s, ref *pref)
{	/* Powers of 10 up to 6 can be represented accurately as */
	/* a single-precision float. */
#define num_powers_10 6
	static float powers_10[num_powers_10+1] =
	   {	1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6	};
	int sign = 0;
	long ival;
	double dval;
	int exp10 = 0;
	int code;
	register int c;
	switch ( c = sgetc(s) )
	   {
	case '+': sign = 1; c = sgetc(s); break;
	case '-': sign = -1; c = sgetc(s); break;
	   }
	if ( !isdigit(c) )
	   {	if ( c != '.' ) return e_syntaxerror;
		c = sgetc(s);
		if ( !isdigit(c) ) return e_syntaxerror;
		ival = 0;
		goto fi;
	   }
	sputback(s);
	if ( (code = scan_int(s, 10, &ival, &dval)) != 0 )
	   {	if ( code < 0 ) return code;	/* e_syntaxerror */
		/* Code == 1, i.e., the integer overflowed. */
		switch ( c = sgetc(s) )
		   {
		default: return e_syntaxerror;	/* not terminated properly */
		case '.': c = sgetc(s); goto fd;
		case EOFC:		/* return a float */
			make_real(pref, (float)(sign < 0 ? -dval : dval));
			return 0;
		   }
	   }
	switch ( c = sgetc(s) )
	   {
	default: return e_syntaxerror;	/* not terminated properly */
	case '.': c = sgetc(s); goto fi;
	case '#':
		if ( sign || ival < min_radix || ival > max_radix )
			return e_syntaxerror;
		code = scan_int(s, (int)ival, &ival, NULL);
		if ( code ) return code;
		if ( sgetc(s) != EOFC ) return e_syntaxerror;
	case EOFC: ;
	   }
	/* Return an integer */
	make_int(pref, (sign < 0 ? -ival : ival));
	return 0;
	/* Handle a real.  We just saw the decimal point. */
	/* Enter here if we are still accumulating an integer in ival. */
fi:	while ( isdigit(c) )
	   {	/* Check for overflowing ival */
		if ( ival >= (max_ulong >> 1) / 10 - 1 )
		   {	dval = ival;
			goto fd;
		   }
		ival = ival * 10 + (c - '0');
		c = sgetc(s);
		exp10--;
	   }
	if ( sign < 0 ) ival = -ival;
	/* Take a shortcut for the common case */
	if ( !(c == 'e' || c == 'E' || exp10 < -num_powers_10) )
	   {	make_real(pref, (float)(ival / powers_10[-exp10]));
		return 0;
	   }
	dval = ival;
	goto fe;
	/* Now we are accumulating a double in dval. */
fd:	while ( isdigit(c) )
	   {	dval = dval * 10 + (c - '0');
		c = sgetc(s);
		exp10--;
	   }
	if ( sign < 0 ) dval = -dval;
fe:	/* dval contains the value, negated if necessary */
	if ( c == 'e' || c == 'E' )
	   {	/* Check for a following exponent. */
		int esign = 0;
		long eexp;
		switch ( c = sgetc(s) )
		   {
		case '+': break;
		case '-': esign = 1; break;
		default: sputback(s);
		   }
		code = scan_int(s, 10, &eexp, NULL);
		if ( code < 0 ) return code;
		if ( code > 0 || eexp > 999 )
			return e_limitcheck;	/* semi-arbitrary */
		if ( esign )
			exp10 -= (int)eexp;
		else
			exp10 += (int)eexp;
		c = sgetc(s);
	   }
	if ( c != EOFC ) return e_syntaxerror;
	/* Compute dval * 10^exp10. */
	if ( exp10 > 0 )
	   {	while ( exp10 > num_powers_10 )
			dval *= powers_10[num_powers_10],
			exp10 -= num_powers_10;
		if ( exp10 > 0 )
			dval *= powers_10[exp10];
	   }
	else if ( exp10 < 0 )
	   {	while ( exp10 < -num_powers_10 )
			dval /= powers_10[num_powers_10],
			exp10 += num_powers_10;
		if ( exp10 < 0 )
			dval /= powers_10[-exp10];
	   }
	make_real(pref, (float)dval);
	return 0;
}
/* Internal subroutine to scan an integer. */
/* Return 0, e_limitcheck, or e_syntaxerror. */
/* (The only syntax error is no digits encountered.) */
/* If the integer won't fit in a long, then: */
/*   if pdval == NULL, return e_limitcheck; */
/*   if pdval != NULL, return 1 and store a double value in *pdval. */
/* Put back the terminating character. */
private int
scan_int(register stream *s, int radix, long *pval, double *pdval)
{	int ival = 0, imax, irem;
#if ints_are_short
	long lval, lmax;
	int lrem;
#else
#  define lval ival			/* for overflowing into double */
#endif
	double dval;
	register int c, d;
	register byte *decoder = scan_char_decoder;
	/* Avoid the long divisions when radix = 10 */
#define set_max(vmax, vrem, big)\
  if ( radix == 10 )	vmax = (big) / 10, vrem = (big) % 10;\
  else			vmax = (big) / radix, vrem = (big) % radix
	set_max(imax, irem, max_uint >> 1);
#define convert_digit_fails(c, d)\
  (d = decoder[c]) >= radix
	while ( 1 )
	   {	c = sgetc(s);
		if ( convert_digit_fails(c, d) )
		   {	if ( c != EOFC ) sputback(s);
			*pval = ival;
			return 0;
		   }
		if ( ival >= imax && (ival > imax || d > irem) )
			break;		/* overflow */
		ival = ival * radix + d;
	   }
#if ints_are_short
	/* Short integer overflowed.  Accumulate in a long. */
	lval = (long)ival * radix + d;
	set_max(lmax, lrem, max_ulong >> 1);
	while ( 1 )
	   {	c = sgetc(s);
		if ( convert_digit_fails(c, d) )
		   {	if ( c != EOFC ) sputback(s);
			*pval = lval;
			return 0;
		   }
		if ( lval >= lmax && (lval > lmax || d > lrem) )
			break;		/* overflow */
		lval = lval * radix + d;
	   }
#endif
	/* Integer overflowed.  Accumulate the result as a double. */
	if ( pdval == NULL ) return e_limitcheck;
	dval = (double)lval * radix + d;
	while ( 1 )
	   {	c = sgetc(s);
		if ( convert_digit_fails(c, d) )
		   {	if ( c != EOFC ) sputback(s);
			*pdval = dval;
			return 1;
		   }
		dval = dval * radix + d;
	   }
	/* Control doesn't get here */
}

/* Make a string */
private int
mk_string(ref *pref, dynamic_area *pda)
{	uint size = pda->next - pda->base;
	byte *body = alloc_shrink(pda->base, pda->num_elts, size, 1, "scanner(string)");
	if ( body == 0 ) return e_VMerror;
	make_tasv(pref, t_string, a_all, size, bytes, body);
	return 0;
}

/* Internal procedure to scan a string. */
private int
scan_string(register stream *s, int from_string, ref *pref)
{	dynamic_area da;
	register int c;
	register byte *ptr = dynamic_begin(&da, 100, 1);
	int plevel = 0;
	if ( ptr == 0 ) return e_VMerror;
top:	while ( 1 )
	   {	c = sgetc(s);
		if ( c == EOFC ) return e_syntaxerror;
		else if ( c == '(' ) plevel++;
		else if ( c == ')' ) { if ( --plevel < 0 ) break; }
		else if ( c == '\\' && !from_string )
		   {	c = sgetc(s);
			switch ( c )
			   {
			case 'n': c = '\n'; break;
			case 'r': c = '\r'; break;
			case 't': c = '\t'; break;
			case 'b': c = '\b'; break;
			case 'f': c = '\f'; break;
			case '\n': goto top;	/* ignore */
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
			   {	int d = sgetc(s);
				c -= '0';
				if ( d >= '0' && d <= '7' )
				   {	c = (c << 3) + d - '0';
					d = sgetc(s);
					if ( d >= '0' && d <= '7' )
					   {	c = (c << 3) + d - '0';
						break;
					   }
				   }
				if ( d == EOFC ) return e_syntaxerror;
				sputback(s);
			   }
				break;
			default: ;	/* ignore the \ */
			   }
		   }
		if ( ptr == da.limit )
		   {	da.next = ptr;
			if ( !dynamic_grow(&da) )
			  return e_VMerror;
			ptr = da.next;
		   }
		*ptr++ = c;
	   }
	da.next = ptr;
	return mk_string(pref, &da);
}

/* Internal procedure to scan a hex string. */
private int
scan_hex_string(stream *s, ref *pref)
{	dynamic_area da;
	int c1, c2, val1, val2;
	byte *ptr = dynamic_begin(&da, 100, 1);
	register byte *decoder = scan_char_decoder;
	if ( ptr == 0 ) return e_VMerror;
l1:	do
	   {	c1 = sgetc(s);
		if ( (val1 = decoder[c1]) < 0x10 )
		   {	do
			   {	c2 = sgetc(s);
				if ( (val2 = decoder[c2]) < 0x10 )
				   {	if ( ptr == da.limit )
					   {	da.next = ptr;
						if ( !dynamic_grow(&da) )
						  return e_VMerror;
						ptr = da.next;
					   }
					*ptr++ = (val1 << 4) + val2;
					goto l1;
				   }
			   }
			while ( val2 == ctype_space );
			if ( c2 != '>' ) return e_syntaxerror;
			if ( ptr == da.limit )
			   {	da.next = ptr;
				if ( !dynamic_grow(&da) )
				  return e_VMerror;
				ptr = da.next;
			   }
			*ptr++ = val1 << 4;	/* no 2nd char */
			goto lx;
		   }
	   }
	while ( val1 == ctype_space );
	if ( c1 != '>' ) return e_syntaxerror;
lx:	da.next = ptr;
	return mk_string(pref, &da);
}
