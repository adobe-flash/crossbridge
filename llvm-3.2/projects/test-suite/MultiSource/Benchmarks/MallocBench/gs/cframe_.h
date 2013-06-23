/*
 * LLVM - http://llvm.org/
 *
 * Changes:
 *  1) 2/18/2004
 *     Modified cstack_top_frame() macro for UNIX so that it returns an
 *     incorrect but correctly typed return value.
 */

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

/* cframe_.h */
/* System-dependent definitions for C stack frame format */

/* cstack_top_frame(a_jmp_buf) returns a pointer which can be */
/* handed to stack_next_frame to give the top frame on a stack. */
/* cstack_next_frame(a_frame_ptr) gives the next frame pointer. */
#ifdef VMS
#  define cstack_top_frame(buf)		/* buf is a jmp_buf */\
	(char *)&buf
#  define cstack_next_frame(fptr)\
	*((char **)(fptr) + 12)
#else					/* generic Unix */
#  define cstack_top_frame(buf)		/* buf is a jmp_buf */\
	(char *)&(buf[0])
#  define cstack_next_frame(fptr)\
	*((char **)(fptr) + 3)
#endif
