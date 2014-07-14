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

/* idebug.c */
/* Debugging support for GhostScript interpreter */
#include "ghost.h"
#include "name.h"

/* Forward references */
void debug_print_string(P2(byte *, ushort));

/* Print a ref */
void
debug_print_ref(ref *pref)
{	unsigned size = pref->size;
	printf("(%x)", pref->type_attrs);
	switch ( r_type(pref) )
	   {
	case t_array:
	  printf("array(%u)0x%lx", size, (ulong)pref->value.refs); break;
	case t_boolean: printf("boolean %x", pref->value.index); break;
	case t_device:
	  printf("device 0x%lx", (ulong)pref->value.pdevice); break;
	case t_dictionary:
	  printf("dict(%u/%u)0x%lx",
		 dict_length(pref), dict_maxlength(pref),
		 (ulong)pref->value.pdict); break;
	case t_file: printf("file 0x%lx", (ulong)pref->value.pfile); break;
	case t_integer: printf("int %ld", pref->value.intval); break;
	case t_mark: printf("mark"); break;
	case t_name:
	  printf("name(0x%lx#%x)", (ulong)pref->value.pname,
		 pref->value.pname->index);
	  debug_print_string(pref->value.pname->string_bytes,
			     pref->value.pname->string_size);
	  break;
	case t_null: printf("null"); break;
	case t_operator:
	  printf("op(%u)0x%lx", size, (ulong)pref->value.opproc); break;
	case t_packedarray:
	  printf("packedarray(%u)0x%lx", size, (ulong)pref->value.refs); break;
	case t_real: printf("real %f", pref->value.realval); break;
	case t_string:
	  printf("string(%u)0x%lx", size, (ulong)pref->value.bytes); break;
	default: printf("type 0x%x", r_type(pref));
	   }
}

/* Print a string */
void
debug_print_string(byte *chrs, ushort len)
{	ushort i;
	for ( i = 0; i < len; i++ )
		putchar(chrs[i]);
}

/* Dump a region of memory containing refs */
void
debug_dump_refs(ref *from, ref *to, char *msg)
{	ref *p = from;
	if ( from < to ) printf("%s:\n", msg);
	while ( p < to )
	   {	unsigned type = r_type(p), attrs = p->type_attrs;
		unsigned btype = r_btype(p);
		static char *ts[] = { type_print_strings };
		static char *as = attr_print_string;
		char *ap = as;
		printf("0x%lx: 0x%02x ", (ulong)p, type);
		if ( btype >= t_next_index )
			printf("0x%02x?? ", btype);
		else
			printf("%s ", ts[btype]);
		for ( ; *ap; ap++, attrs >>= 1 )
		  if ( *ap != '.' )
		    putchar(((attrs & 1) ? *ap : '-'));
		printf(" 0x%04x 0x%08lx", p->size, *(ulong *)&p->value);
		switch ( btype )
		   {
		case t_real: printf("  =  %g", p->value.realval); break;
		case t_integer: printf("  =  %ld", p->value.intval); break;
		default: ;
		   }
		printf("\n");
		p++;
	   }
}

/* Dump a region of memory */
void
debug_dump_bytes(byte *from, byte *to, char *msg)
{	byte *p = from;
	if ( from < to ) printf("%s:\n", msg);
	while ( p != to )
	   {	byte *q = min(p + 16, to);
		printf("%lx:", (ulong)p);
		while ( p != q ) printf(" %02x", *p++);
		printf("\n");
	   }
}
