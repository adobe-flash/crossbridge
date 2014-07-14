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

/* zvmem.c */
/* "Virtual memory" operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "state.h"
#include "store.h"
#include "gsmatrix.h"			/* for gsstate.h */
#include "gsstate.h"

/* Import the stacks for checking in restore */
extern ref estack[], dstack[];
extern ref *osbot, *esp, *dsp;

/* 'Save' structure */
typedef struct vm_save_s vm_save;
struct vm_save_s {
	alloc_save *asave;		/* allocator save */
	int_state *isave;		/* old interpreter state */
	gs_state *gsave;		/* old graphics state */
};

/* save */
int
zsave(register ref *op)
{	int code = zgsave(op);
	vm_save *vmsave = (vm_save *)alloc(1, sizeof(vm_save), "zsave");
	alloc_save *asave = alloc_save_state();
	if ( code < 0 ) return code;
	if ( vmsave == 0 || asave == 0 ) return e_VMerror;
	vmsave->asave = asave;
	/* Save the old interpreter state pointers, */
	/* and cut the chains so we can't grestore past here. */
	vmsave->isave = istate.saved;
	istate.saved = 0;
	vmsave->gsave = gs_state_swap_saved(igs, (gs_state *)0);
	push(1);
	make_tv(op, t_save, psave, vmsave);
	return 0;
}

/* restore */
private int restore_check_stack(P3(ref *, ref *, alloc_save *));
int
zrestore(register ref *op)
{	vm_save *vmsave;
	alloc_save *asave;
	int code;
	check_type(*op, t_save);
	vmsave = op->value.psave;
	asave = vmsave->asave;
	/* Check the contents of the stacks. */
	   {	int code;
		if ( (code = restore_check_stack(osbot, op, asave)) < 0 ||
		     (code = restore_check_stack(estack, esp + 1, asave)) < 0 ||
		     (code = restore_check_stack(dstack, dsp + 1, asave)) < 0
		   )
			return code;
	   }
	if ( alloc_restore_state_check(asave) < 0 )
		return e_invalidrestore;
	/* Restore the graphics state back to the state */
	/* that was allocated by the save. */
	/* This must be done before the alloc_restore_state. */
	zgrestoreall(op);
	/* Now it's safe to restore the state of memory. */
	alloc_restore_state(asave);
	/* Put back the interpreter state chain pointers, */
	/* and do a grestore. */
	istate.saved = vmsave->isave;
	gs_state_swap_saved(igs, vmsave->gsave);
	if ( (code = zgrestore(op)) < 0 ) return code;
	alloc_free((char *)vmsave, 1, sizeof(vm_save), "zrestore");
	pop(1);
	return 0;
}
/* Check a stack to make sure all its elements are older than a save. */
private int
restore_check_stack(ref *bot, ref *top, alloc_save *asave)
{	ref *stkp;
	for ( stkp = bot; stkp < top; stkp++ )
	   {	char *ptr;
		switch ( r_type(stkp) )
		   {
		case t_array:
		case t_packedarray: ptr = (char *)stkp->value.refs; break;
		case t_dictionary: ptr = (char *)stkp->value.pdict; break;
		case t_fontID: ptr = (char *)stkp->value.pfont; break;
		/* case t_file: ****** WHAT? ****** */
		case t_name: ptr = (char *)stkp->value.pname; break;
		case t_save: ptr = (char *)stkp->value.psave; break;
		case t_string: ptr = (char *)stkp->value.bytes; break;
		case t_color: ptr = (char *)stkp->value.pcolor; break;
		case t_device: ptr = (char *)stkp->value.pdevice; break;
		default: continue;
		   }
		if ( alloc_is_since_save(ptr, asave) )
			return e_invalidrestore;
	   }

	return 0;			/* OK */
}

/* vmstatus */
int
zvmstatus(register ref *op)
{	long used, total;
	alloc_status(&used, &total);
	push(3);
	make_int(op - 2, alloc_save_level());
	make_int(op - 1, used);
	make_int(op, total);
	return 0;
}

/* ------ Initialization procedure ------ */

void
zvmem_op_init()
{	static op_def my_defs[] = {
		{"1restore", zrestore},
		{"0save", zsave},
		{"0vmstatus", zvmstatus},
		op_def_end
	};
	z_op_init(my_defs);
}
