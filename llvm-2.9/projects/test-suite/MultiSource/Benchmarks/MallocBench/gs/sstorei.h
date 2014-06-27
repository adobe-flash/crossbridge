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

/* sstorei.h */
/* Fast assignment macros */

/* See store.h for a discussion of the purpose of the following. */

/* Here is where we check for getting around Turbo C's inefficiencies. */
#ifdef __MSDOS__
	/* s_store_* are in-line assembly code */
/* This is a separate file so that other compilers don't have to */
/* deal with Turbo C's #pragma directive. */
#  include "dosstore.h"
#else
	/* s_store_* are simple assignment */
#  define s_store_i(pto,pfrom) (*(pto) = *(pfrom))
#  define s_store_b(pto,ito,pfrom,ifrom) ((pto)[ito] = (pfrom)[ifrom])
#  define s_store_r(ptr,ito,ifrom) ((ptr)[ito] = (ptr)[ifrom])
#endif
