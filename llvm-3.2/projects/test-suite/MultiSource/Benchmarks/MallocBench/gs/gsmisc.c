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

/* gsmisc.c */
/* Miscellaneous utilities for GhostScript library */
#include "gx.h"
#include "malloc_.h"

/* Debugging flags */
#ifdef DEBUG
char gs_debug[128];
#endif

/* Versions of malloc and free compatible with GhostScript's */
/* model of memory management. */
char *
gs_malloc(uint num_elts, uint elt_size, char *client_name)
{
#ifdef BWGC
    	extern char *gc_malloc();
	char *ptr = gc_malloc(num_elts * elt_size);
#else	
	char *ptr = malloc(num_elts * elt_size);
#endif

	if ( ptr == 0 ) dprintf1("%s: malloc failed\n", client_name);
	return ptr;
}
void
gs_free(char *ptr, uint num_elts, uint elt_size, char *client_name)
{
#ifdef IGNOREFREE
#else    
    free(ptr);
#endif    
}
