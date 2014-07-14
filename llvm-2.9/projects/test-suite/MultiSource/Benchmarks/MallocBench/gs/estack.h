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

/* estack.h */
/* Definitions for execution stack operators for GhostScript */

/* Import the execution stack */
extern ref estack[];
extern ref *esp;	/* points to current top element of stack */
extern ref *estop;	/* points to end of stack */

/* The execution stack is used for three purposes: */
/*	- Procedures being executed are held here.  They always have */
/* type = t_array or type = t_packedarray, with a_executable set. */
/*	- if, ifelse, etc. push arguments to be executed here. */
/* They may be any kind of object whatever. */
/*	- for, repeat, loop, forall, pathforall, run, and stopped */
/* mark the stack by pushing an object with type = t_null, */
/* attrs = a_executable, and value.index = 0 for loops, 1 for run/stopped. */
/* (Null objects can't ever appear on the stack otherwise: */
/* if a control operator pushes one, it gets popped immediately.) */
/* The loop operators also push whatever state they need, */
/* followed by an operator object that handles continuing the loop. */

/* Macro for marking the execution stack */
#define mark_estack(idx)\
  (++esp, make_tav(esp, t_null, a_executable, index, idx))

/* Macro for pushing an operator on the execution stack */
/* to represent a continuation procedure */
#define push_op_estack(proc)\
  (++esp, make_tasv(esp, t_operator, a_executable, 0, opproc, proc))

/* Macro to ensure enough room on the execution stack */
#define check_estack(n)\
  if ( esp + (n) > estop ) return e_execstackoverflow

/* Define the various kinds of execution stack marks. */
#define es_other 0			/* internal use */
#define es_show 1			/* show operators */
#define es_for 2			/* iteration operators */
#define es_stopped 3			/* stopped operator */
