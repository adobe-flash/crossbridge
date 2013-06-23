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

/* dosstore.h */
/* Fast assignment macros for MS-DOS machines only */

/* See sstorei.h for a discussion of the purpose of the following. */
/* This is in a file of its own so that non-DOS compilers */
/* don't encounter the pragma directive. */

#pragma inline
#ifdef FOR80386				/* use 32-bit moves */
/* 66h is the operand size prefix */
#  define s_store_b(pto,ito,pfrom,ifrom)\
   {	asm les bx,dword ptr pfrom;\
	asm db 66h; asm mov ax,es:[bx+((ifrom)*8)];\
	asm db 66h; asm mov cx,es:[bx+((ifrom)*8)+4];\
	asm les bx,dword ptr pto;\
	asm db 66h; asm mov es:[bx+((ito)*8)],ax;\
	asm db 66h; asm mov es:[bx+((ito)*8)+4],cx;\
   }
#  define s_store_r(pboth,ito,ifrom)\
   {	asm les bx,dword ptr pboth;\
	asm db 66h; asm mov ax,es:[bx+((ifrom)*8)];\
	asm db 66h; asm mov cx,es:[bx+((ifrom)*8)+4];\
	asm db 66h; asm mov es:[bx+((ito)*8)],ax;\
	asm db 66h; asm mov es:[bx+((ito)*8)+4],cx;\
   }
#else
#  define s_store_b(pto,ito,pfrom,ifrom)\
   {	asm les bx,dword ptr pfrom;\
	asm mov ax,es:[bx+((ifrom)*8)]; asm mov cx,es:[bx+((ifrom)*8)+2];\
	asm mov dx,es:[bx+((ifrom)*8)+4]; asm push es:[bx+((ifrom)*8)+6];\
	asm les bx,dword ptr pto;\
	asm mov es:[bx+((ito)*8)],ax; asm mov es:[bx+((ito)*8)+2],cx;\
	asm mov es:[bx+((ito)*8)+4],dx; asm pop es:[bx+((ito)*8)+6];\
   }
#  define s_store_r(pboth,ito,ifrom)\
   {	asm les bx,dword ptr pboth;\
	asm mov ax,es:[bx+((ifrom)*8)]; asm mov cx,es:[bx+((ifrom)*8)+2];\
	asm mov es:[bx+((ito)*8)],ax; asm mov es:[bx+((ito)*8)+2],cx;\
	asm mov ax,es:[bx+((ifrom)*8)+4]; asm mov cx,es:[bx+((ifrom)*8)+6];\
	asm mov es:[bx+((ito)*8)+4],ax; asm mov es:[bx+((ito)*8)+6],cx;\
   }
#endif
#define s_store_i(pto,pfrom) s_store_b(pto,0,pfrom,0)
