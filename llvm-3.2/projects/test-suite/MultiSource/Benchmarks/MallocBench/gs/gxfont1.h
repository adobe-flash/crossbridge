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

/* gxfont1.h */
/* Type 1 font information for GhostScript library. */

/* This is (part of) the client_data for a gs_font. */
/* Eventually there may be many other entries here, */
/* per the Adobe Black Book. */
typedef struct gs_type1_data_s gs_type1_data;
struct gs_type1_data_s {
	int (*subr_proc)(P3(gs_type1_data *pdata,
			    int index, byte **pcharstring));
	int (*pop_proc)(P2(gs_type1_data *, fixed *));
	char *proc_data;		/* data for subr_proc */
	int lenIV;			/* # of leading garbage bytes */
};
