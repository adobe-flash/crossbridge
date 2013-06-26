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

/* oper.h */
/* Definitions for GhostScript operators */

/* Structure for initializing the operator table. */
/* Each operator file declares an array of these, */
/* and supplies an initialization procedure of the following kind: */
/*
void
zxxx_init()
{	static op_def my_defs[] = {
		{"1name", zname},
		    ...
		op_def_end
	};
	z_init(my_defs);
}
 */
/* This definition always appears at the END of the file, */
/* to avoid the necessity for forward declarations for all the */
/* operator procedures. */
typedef struct {
	char *name;
	op_proc_p proc;
} op_def;
#define op_def_end {(char *)0, (op_proc_p)0}

/* Operand stack manipulation */
extern ref *osbot, *osp, *ostop;
/* The most efficient code is different on MS-DOS vs. Unix.... */
#ifdef __MSDOS__			/* stupid compiler */
#define push(n)\
  if ( (op += (n)) > ostop ) return (e_stackoverflow); else osp += (n)
#else					/* reasonable compiler */
#define push(n)\
  if ( (osp = op += (n)) > ostop ) return (osp -= (n), e_stackoverflow)
#endif
/*
 * Note that the pop macro only decrements osp, not op.  For this reason,
 *
 *	>>>	pop should only be used just before returning,	<<<
 *	>>>	or else op must be decremented explicitly.	<<<
 */
#define pop(n) (osp -= (n))
/*
 * Note that the interpreter does not check for operand stack underflow
 * before calling the operator procedure.  There are "guard" entries
 * with invalid types and attributes just below the bottom of the
 * operand stack: if the operator returns with a typecheck error,
 * the interpreter checks for underflow at that time.
 * Operators that don't typecheck their arguments must check for
 * operand stack underflow explicitly.
 */
#define os_max_nargs 6
extern ref *osp_nargs[os_max_nargs];
#define check_op(nargs)\
  if ( op < osp_nargs[(nargs) - 1] ) return e_stackunderflow

/* Check type */
#define check_type(rf,typ)\
  if ( !r_has_type(&rf,typ) ) return e_typecheck
/* Check for array */
#define check_array_else(rf,err)\
  switch ( r_type(&rf) )\
   { default: return err; case t_array: case t_packedarray: ; }
#define check_array(rf) check_array_else(rf, e_typecheck)
/* Check for procedure */
#define check_proc(rf)\
  check_array(rf); check_access(rf, (a_execute+a_executable))

/* Check for read, write, or execute access */
#define check_access(rf,acc)\
  if ( !r_has_attrs(&rf,acc) ) return e_invalidaccess
#define check_read(rf) check_access(rf,a_read)
#define check_read_type(rf,typ) check_type(rf,typ); check_read(rf)
#define check_write(rf) check_access(rf,a_write)
#define check_write_type(rf,typ) check_type(rf,typ); check_write(rf)
#define check_execute(rf) check_access(rf,a_execute)

/* Macro for as yet unimplemented operators. */
/* The if ( 1 ) is to prevent the compiler from complaining about */
/* unreachable code. */
#define NYI(msg) if ( 1 ) return e_undefined

/* If an operator may have popped or pushed something on the control stack, */
/* it must return o_check_estack rather than 0 to indicate success. */
#define o_check_estack 1
