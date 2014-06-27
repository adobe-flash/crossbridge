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

/* iinit.c */
/* Initialize internally known objects for GhostScript interpreter */
#include "ghost.h"
#include "dict.h"
#include "oper.h"
#include "store.h"

/* Implementation parameters */
#define systemdict_size 401		/* a nice prime number */

/* Standard dictionaries */
ref name_errordict;
extern ref dstack[];
#define systemdict (dstack[0])
/* Error names */
ref name_ErrorNames;

/* Enter a name and value into systemdict */
void
initial_enter_name(char *nstr, ref *pref)
{	ref nref;
	name_enter(nstr, &nref);
	if ( dict_put(&systemdict, &nref, pref) )
		dprintf("dict_put failed!\n"), exit(1);
}

/* Enter an operator into systemdict. */
/* The first character of the name is a digit */
/* giving the minimum acceptable number of operands. */
void
initial_enter_op(char *nstr, op_proc_p proc)
{	ref oper;
	make_tasv(&oper, t_operator, a_executable, *nstr - '0', opproc, proc);
	interp_fix_op(&oper);		/* optimize if possible */
	initial_enter_name(nstr + 1, &oper);
}

/* Initialize objects other than operators */
void
obj_init()
{
	/* Initialize the standard objects */
	ref vmark, vnull;
	make_tv(&vmark, t_mark, intval, 0);
	make_tv(&vnull, t_null, intval, 0);

	/* Create the system dictionary */
	dict_create(systemdict_size, &systemdict);
	dstack[1] = dstack[0];		/* just during initialization */

	/* Initialize the predefined names other than operators */
	initial_enter_name("mark", &vmark);
	initial_enter_name("null", &vnull);

	/* Create other system-known names */
	name_enter("errordict", &name_errordict);
	name_enter("ErrorNames", &name_ErrorNames);
}

/* Initialize the operators */
void
op_init()
{
#define do_init(proc)\
 { extern void proc(); proc(); }
	/* Non-graphics-related operators */
	do_init(zarith_op_init);
	do_init(zarray_op_init);
	do_init(zcontrol_op_init);
	do_init(zdict_op_init);
	do_init(zfile_op_init);
	do_init(zgeneric_op_init);
	do_init(zmath_op_init);
	do_init(zmisc_op_init);
	do_init(zpacked_op_init);
	do_init(zrelbit_op_init);
	do_init(zstack_op_init);
	do_init(zstring_op_init);
	do_init(ztype_op_init);
	do_init(zvmem_op_init);
	/* Graphics-related operators */
	do_init(zchar_op_init);
	do_init(zcolor_op_init);
	do_init(zdevice_op_init);
	do_init(zfont_op_init);
	do_init(zgstate_op_init);
	do_init(zht_op_init);
	do_init(zmatrix_op_init);
	do_init(zpaint_op_init);
	do_init(zpath_op_init);
	do_init(zpath2_op_init);
}
/* This routine is called by the individual zxxx_op_init routines. */
void
z_op_init(op_def *op_table)
{	op_def *def = op_table;
	while ( def->name )
		initial_enter_op(def->name, def->proc),
		def++;
}
