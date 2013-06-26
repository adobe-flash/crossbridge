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

/* zcontrol.c */
/* Control operators for GhostScript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "estack.h"
#include "store.h"
#include "sstorei.h"

/* Imported from util.h */
extern int num_params(P3(ref *, int, float *));

/* exec */
int
zexec(register ref *op)
{	check_op(1);
	check_estack(1);
	++esp;
	s_store_i(esp, op);
	pop(1);
	return o_check_estack;
}

/* if */
int
zif(register ref *op)
{	check_type(op[-1], t_boolean);
	if ( op[-1].value.index )		/* true */
	   {	check_estack(1);
		++esp;
		s_store_i(esp, op);
	   }
	pop(2);
	return o_check_estack;
}

/* ifelse */
int
zifelse(register ref *op)
{	check_type(op[-2], t_boolean);
	check_estack(1);
	++esp;
	if ( op[-2].value.index )
	   {	s_store_b(esp, 0, op, -1);
	   }
	else
	   {	s_store_i(esp, op);
	   }
	pop(3);
	return o_check_estack;
}

/* for */
private int
  for_int_continue(P1(ref *)),
  for_real_continue(P1(ref *));
int
zfor(register ref *op)
{	int code = num_params(op - 1, 3, (float *)0);
	if ( code < 0 ) return code;	/* non-numeric arg */
	check_estack(7);
	/* Push a mark, the control variable, the increment, */
	/* the limit, and the procedure, and invoke */
	/* the continuation operator. */
	mark_estack(es_for);
	*++esp = op[-3];
	*++esp = op[-2];
	*++esp = op[-1];
	*++esp = *op;
	pop(4);
	op -= 4;
	/* If the parameters are not all integers, */
	/* coerce them all to floats now. */
	if ( code == 7 )		/* i.e. all integer args */
		return for_int_continue(op);
	else
	   {	if ( code & 1 ) make_real(esp - 3, esp[-3].value.intval);
		if ( code & 2 ) make_real(esp - 2, esp[-2].value.intval);
		if ( code & 4 ) make_real(esp - 1, esp[-1].value.intval);
		return for_real_continue(op);
	   }
}
/* Continuation operators for for, separate for integer and real. */
/* Execution stack contains mark, control variable, increment, */
/* limit, and procedure (procedure is topmost.) */
/* Continuation operator for integers. */
private int
for_int_continue(register ref *op)
{	ref proc;
	long var = esp[-3].value.intval;
	long incr = esp[-2].value.intval;
	if ( incr >= 0 ? (var > esp[-1].value.intval) :
		(var < esp[-1].value.intval) )
		   {	esp -= 5;	/* pop everything */
			return o_check_estack;
		   }
	push(1);
	*op = esp[-3];
	esp[-3].value.intval = var + incr;
	proc = *esp;		/* saved proc */
	push_op_estack(for_int_continue);	/* push continuation */
	*++esp = proc;
	return o_check_estack;
}
/* Continuation operator for reals. */
private int
for_real_continue(register ref *op)
{	ref proc;
	float var = esp[-3].value.realval;
	float incr = esp[-2].value.realval;
	if ( incr >= 0 ? (var > esp[-1].value.realval) :
		(var < esp[-1].value.realval) )
		   {	esp -= 5;	/* pop everything */
			return o_check_estack;
		   }
	push(1);
	*op = esp[-3];
	esp[-3].value.realval = var + incr;
	proc = *esp;		/* saved proc */
	push_op_estack(for_real_continue);	/* push continuation */
	*++esp = proc;
	return o_check_estack;
}

/* repeat */
private int repeat_continue(P1(ref *));
int
zrepeat(register ref *op)
{	check_type(op[-1], t_integer);
	if ( op[-1].value.intval < 0 ) return e_rangecheck;
	check_estack(5);
	/* Push a mark, the count, and the procedure, and invoke */
	/* the continuation operator. */
	mark_estack(es_for);
	*++esp = op[-1];
	*++esp = *op;
	pop(2);
	return repeat_continue(op - 2);
}
/* Continuation operator for repeat */
private int
repeat_continue(register ref *op)
{	ref proc;
	proc = *esp;		/* saved proc */
	if ( --(esp[-1].value.intval) >= 0 )	/* continue */
	   {	push_op_estack(repeat_continue);	/* push continuation */
		*++esp = proc;
	   }
	else				/* done */
	   {	esp -= 3;		/* pop mark, count, proc */
	   }
	return o_check_estack;
}

/* loop */
private int loop_continue(P1(ref *));
int
zloop(register ref *op)
{	check_op(1);
	check_estack(4);
	/* Push a mark and the procedure, and invoke */
	/* the continuation operator. */
	mark_estack(es_for);
	*++esp = *op;
	pop(1);
	return loop_continue(op - 1);
}
/* Continuation operator for loop */
private int
loop_continue(register ref *op)
{	ref proc;
	proc = *esp;		/* saved proc */
	push_op_estack(loop_continue);	/* push continuation */
	*++esp = proc;
	return o_check_estack;
}

/* exit */
int
zexit(register ref *op)
{	ref *ep = esp;
	while ( ep >= estack )
	   {	if ( r_type(ep) == t_null )	/* control mark */
			switch ( (ep--)->value.index )
			   {
			case es_for: esp = ep; return o_check_estack;
			case es_stopped: return e_invalidexit;	/* not a loop */
			   }
		else
			ep--;
	   }
	/* Return e_invalidexit if there is no mark at all. */
	/* This is different from PostScript, which aborts. */
	/* It shouldn't matter in practice. */
	return e_invalidexit;
}

/* stop */
int
zstop(register ref *op)
{	ref *ep = esp;
	while ( ep >= estack )
	   {	if ( r_type(ep) == t_null && ep->value.index == es_stopped )
		   {	esp = ep - 1;
			push(1);
			make_bool(op, 1);
			return o_check_estack;
		   }
		ep--;
	   }
	/* Return e_invalidexit if there is no mark at all. */
	/* This is different from PostScript, which aborts. */
	/* It shouldn't matter in practice. */
	return e_invalidexit;
}

/* stopped */
int
zstopped(register ref *op)
{	check_op(1);
	/* Mark the execution stack, and push a false in case */
	/* control returns normally. */
	check_estack(3);
	mark_estack(es_stopped);
	++esp; make_false(esp);
	*++esp = *op;			/* execute the operand */
	pop(1);
	return o_check_estack;
}

/* countexecstack */
int
zcountexecstack(register ref *op)
{	push(1);
	make_int(op, esp - estack + 1);
	return 0;
}

/* execstack */
private int continue_execstack(P1(ref *));
int
zexecstack(register ref *op)
{	/* We can't do this directly, because the interpreter */
	/* might have cached some state.  To force the interpreter */
	/* to update the stored state, we push a continuation on */
	/* the exec stack; the continuation is executed immediately, */
	/* and does the actual transfer. */
	int depth = esp - estack + 1;
	check_write_type(*op, t_array);
	if ( depth > op->size ) return e_rangecheck;
	if ( estack >= estop ) return e_execstackoverflow;	/* no room to push */
	op->size = depth;
	r_set_attrs(op, a_subrange);
	push_op_estack(continue_execstack);
	return o_check_estack;
}
/* Continuation operator to do the actual transfer */
private int
continue_execstack(register ref *op)
{	int depth = op->size;		/* was set above */
	refcpy(op->value.refs, estack, depth);
	return 0;
}


/* quit */
int
zquit(register ref *op)
{	gs_exit(0);
}

/* ------ Initialization procedure ------ */

void
zcontrol_op_init()
{	static op_def my_defs[] = {
		{"0countexecstack", zcountexecstack},
		{"1exec", zexec},
		{"0execstack", zexecstack},
		{"0exit", zexit},
		{"2if", zif},
		{"3ifelse", zifelse},
		{"4for", zfor},
		{"1loop", zloop},
		{"0quit", zquit},
		{"2repeat", zrepeat},
		{"0stop", zstop},
		{"1stopped", zstopped},
		op_def_end
	};
	z_op_init(my_defs);
}
