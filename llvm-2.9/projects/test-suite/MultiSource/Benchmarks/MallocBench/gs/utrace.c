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

/* utrace.c */
/* Dummy tracing package for Unix C */
#include <stdio.h>
#include <setjmp.h>
#include "std.h"
#include "cframe_.h"

/* This file replaces trace.c for Unix and VMS. */
/* It has only the stack parsing functionality of trace.c -- */
/* the tracing procedures are stubs. */

char *
strupr(char *str)
{	return str;
}

/* Open the map file */
FILE *
trace_open_map(char *mapname, long *preloc)
{	return NULL;
}

/* Enumerate a symbol file */
char *
trace_next_symbol(char **paddr, FILE *mapf)
{	return NULL;
}


/* Look up a symbol in a file */
char *
trace_find_symbol(char *name, FILE *mapf)
{	return NULL;
}

int trace_flush_flag;

/* Trace a named procedure */
int
trace_name(char *name, FILE *mapf, char *arg_format, int retsize)
{	return -1;
}

/* Trace a procedure */
int
trace(void (*proc)(), char *name, char *arg_format, int retsize)
{	return -1;
}

/* ------ Stack parsing ------ */

/* Forward declarations */
char *stack_next_frame(P1(char *));

/* Get the address of the caller's frame */
char *
stack_top_frame()
{	jmp_buf buf;
	setjmp(buf);			/* acquire registers */
	return stack_next_frame(cstack_top_frame(buf));
}

/* Get the return address of a frame. */
unsigned long
stack_return(char *bp)
{	return *(unsigned long *)(bp + 16);
}

/* Get the address of the next higher frame, */
/* or 0 if there is none. */
char *
stack_next_frame(char *bp)
{	char *nbp = cstack_next_frame(bp);
	if ( nbp < bp ) return 0;
	return nbp;
}
