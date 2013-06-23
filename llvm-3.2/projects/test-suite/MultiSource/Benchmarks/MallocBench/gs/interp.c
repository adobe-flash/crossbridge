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

/* interp.c */
/* GhostScript interpreter */
#include "ghost.h"
#include "errors.h"
#include "name.h"
#include "dict.h"
#include "oper.h"
#include "store.h"
#include "sstorei.h"
#include "stream.h"

/* Forward references */
private int interp(P1(ref *pref));
private op_proc_(interp_exit);

/* Configuration parameters */
#define max_ostack 500
#define max_estack 150
#define max_dstack 20

/* See estack.h for a description of the execution stack. */

/* The logic for managing icount and iref below assumes that */
/* there are no control operators which pop and then push */
/* information on the execution stack. */

/* Stacks */
#define os_guard_under 10
#define os_guard_over 10
private ref ostack[os_guard_under+max_ostack+os_guard_over];
ref estack[max_estack];
ref dstack[max_dstack];
ref *osp_nargs[os_max_nargs];		/* for checking osp */

/* Stack pointers */
ref *osbot, *osp, *ostop;
ref *esp, *estop;
ref *dsp, *dstop;

/* The object that caused an error */
ref error_object;

/* Object related to error handling */
extern ref name_errordict;
extern ref name_ErrorNames;

/* Extended types.  The interpreter may replace the type of operators */
/* in procedures with these, to speed up the interpretation loop. */
#define tx_op t_next_index
extern int zadd(P1(ref *));
extern int zdup(P1(ref *));
extern int zexch(P1(ref *));
extern int zifelse(P1(ref *));
extern int zle(P1(ref *));
extern int zpop(P1(ref *));
extern int zsub(P1(ref *));
private op_proc_p special_ops[] = {
	zadd, zdup, zexch, zifelse, zle, zpop, zsub
};
enum {
	tx_op_add = tx_op,
	tx_op_dup,
	tx_op_exch,
	tx_op_ifelse,
	tx_op_le,
	tx_op_pop,
	tx_op_sub,
	tx_next_op
};
#define num_special_ops ((int)tx_next_op - tx_op)

/* The following is a special "error" code that is used internally */
/* to cause the interpreter to exit. */
#define e_InterpreterExit (-100)

/* Initialize the interpreter */
void
interp_init(int ndict)
{	/* Initialize the guard entries on the operand stack */
	/* with objects that have invalid type and attributes. */
	osbot = ostack + os_guard_under;
	osp = osbot - 1, ostop = osbot + (max_ostack-1);
	   {	ref *op;
		for ( op = ostack; op < osbot; op++ )
			make_tav(op, -1, 0, index, 0);
	   }
	   {	int i;
		for ( i = 1; i < os_max_nargs; i++ )
			osp_nargs[i] = osbot + i - 1;
	   }
	esp = estack - 1, estop = estack + (max_estack-1);
	/* Initialize the dictionary stack to the first ndict */
	/* dictionaries.  ndict is a parameter because during */
	/* initialization, only systemdict exists. */
	dsp = dstack + ndict - 1, dstop = dstack + (max_dstack-1);
}

/* Look up an operator during initialization, */
/* changing its type if appropriate. */
void
interp_fix_op(ref *opref)
{	int i = num_special_ops;
	op_proc_p proc = opref->value.opproc;
	while ( --i >= 0 && proc != special_ops[i] ) ;
	if ( i >= 0 ) make_tav(opref, tx_op + i, a_executable, opproc, proc);
}

/* Invoke the interpreter.  If execution completes normally, return 0. */
/* if an error occurs, then if user_errors is true and the error is a */
/* recoverable one (not an overflow condition), let the user handle it; */
/* otherwise, return the error code. */
int
interpret(ref *pref, int user_errors)
{	ref *epref = pref;
	ref erref;
	ref *perrordict, *pErrorNames;
	int code;
	/* Push a special exit procedure on the execution stack */
	esp++;
	make_tav(esp, t_operator, a_executable, opproc, interp_exit);
retry:	code = interp(epref);
	if ( code == e_InterpreterExit ) return 0;
	/* Adjust osp in case of operand stack underflow */
	if ( osp < osbot - 1 )
		osp = osbot - 1;
	if ( !user_errors ) return code;
	if ( dict_find(dstack, &name_errordict, &perrordict) <= 0 ||
	     dict_find(perrordict, &name_ErrorNames, &pErrorNames) <= 0
	   )
		return code;		/* errordict or ErrorNames not found?? */
	switch ( code )
	   {
	case e_dictstackoverflow:
	case e_execstackoverflow:
	case e_stackoverflow:
	case e_VMerror:
		return code;
	   }
	if ( code > -2 || -code > pErrorNames->size )
		return code;		/* unknown error??? */
	if ( dict_find(perrordict, &pErrorNames->value.refs[-code - 1], &epref) <= 0 )
		return code;		/* error name not in errordict??? */
	erref = *epref;
	epref = &erref;
	/* Push the error object on the operand stack */
	*++osp = error_object;
	goto retry;
}	
private int
interp_exit(ref *op)
{	return e_InterpreterExit;
}

/* Main interpreter. */
/* If execution terminates normally, return e_InterpreterExit. */
/* If an error occurs, leave the current object in error_object */
/* and return a (negative) error code. */
#define return_with_error(code, objp)\
  { esp = iesp; error_object = *(objp); return code; }
private int
interp(ref *pref /* object to interpret */)
{	register ref *iref = pref;
	register int icount = 0;	/* # of consecutive tokens at iref */
	register ref *iosp = osp;	/* private copy of osp */
	register ref *iesp = esp;	/* private copy of esp */
	int code;
	ref token;		/* token read from file or string, */
				/* must be declared in this scope */
	ref *ptoken = &token;		/* for s_store_i */
	/* We want to recognize executable arrays here, */
	/* so we push the argument on the estack and enter */
	/* the loop at the bottom. */
	if ( iesp >= estop ) return_with_error (e_execstackoverflow, pref);
	*++iesp = *pref;
	goto bot;
	/* At this point, if icount > 0, iref and icount correspond */
	/* to the top entry on the execution stack: icount is the */
	/* count of sequential entries remaining AFTER the current one. */
#define store_state(ep)\
  ( icount > 0 ? (ep->value.refs = iref + 1, ep->size = icount) : 0 )
#define next()\
  if ( --icount > 0 ) { iref++; goto top; } else goto out
top:	/* This is the top of the interpreter cycle. */
	/* iref points to the ref being interpreted. */
#ifdef DEBUG
if ( gs_debug['I'] || gs_debug['i'] && r_type(iref) == t_name )
   {	void debug_print_ref(P1(ref *));
	int edepth = iesp - estack;
	char depth[10];
	sprintf(depth, "%d", edepth);
	printf(depth);
	edepth -= strlen(depth);
	do { putchar('.'); } while ( --edepth > 0 );	/* indent */
	printf("%lx(%d)[%d]: ", (ulong)iref, icount, (uint)(iosp - osbot + 1));
	debug_print_ref(iref);
	if ( iosp >= osbot )
	   {	printf(" // ");
		debug_print_ref(iosp);
	   }
	printf("\n");
	fflush(stdout);
   }
#endif
/* Object that have attributes (arrays, dictionaries, files, and strings) */
/* use lit and exec; other objects use plain and plain_exec. */
#define lit(t) (((t) << 2) + a_execute)
#define exec(t) (((t) << 2) + a_execute + a_executable)
#define nox(t) ((t) << 2)
#define nox_exec(t) (((t) << 2) + a_executable)
#define plain(t) ((t) << 2)
#define plain_exec(t) (((t) << 2) + a_executable)
sw:	/* We have to populate enough cases of the switch statement */
	/* to force the compiler to use a dispatch rather than */
	/* a testing loop.  What a nuisance! */
	switch ( r_type_xe(iref) )
	   {
	/* Access errors. */
	case nox(t_array): case nox_exec(t_array):
	case nox(t_dictionary): case nox_exec(t_dictionary):
	case nox(t_file): case nox_exec(t_file):
	case nox(t_packedarray): case nox_exec(t_packedarray):
	case nox(t_string): case nox_exec(t_string):
		return_with_error (e_invalidaccess, iref);
	/* Literal objects.  We have to enumerate all the types. */
	/* In fact, we have to include some extra plain_exec entries */
	/* just to populate the switch. */
	case lit(t_array):
	case plain(t_boolean): case plain_exec(t_boolean):
	case lit(t_dictionary):
	case lit(t_file):
	case plain(t_fontID): case plain_exec(t_fontID):
	case plain(t_integer): case plain_exec(t_integer):
	case plain(t_mark): case plain_exec(t_mark):
	case plain(t_name):
	case plain(t_null):
	case plain(t_operator):
	case lit(t_packedarray):
	case plain(t_real): case plain_exec(t_real):
	case plain(t_save): case plain_exec(t_save):
	case lit(t_string):
	case plain(t_color): case plain_exec(t_color):
	case plain(t_device): case plain_exec(t_device):
		break;
	/* Special operators. */
	case plain_exec(tx_op_add):
		if ( (code = zadd(iosp)) < 0 )
			return_with_error (code, iref);
		iosp--;
		next();
	case plain_exec(tx_op_dup):
		if ( iosp < osp_nargs[1] )
			return_with_error (e_stackunderflow, iref);
		iosp++;
		s_store_r(iosp, 0, -1);
		next();
	case plain_exec(tx_op_exch):
		if ( iosp < osp_nargs[2] )
			return_with_error (e_stackunderflow, iref);
		s_store_i(ptoken, iosp);
		s_store_r(iosp, 0, -1);
		s_store_b(iosp, -1, ptoken, 0);
		next();
	case plain_exec(tx_op_ifelse):
		if ( r_type(iosp - 2) != t_boolean )
			return_with_error (e_typecheck, iref);
		if ( iesp >= estop )
			return_with_error (e_execstackoverflow, iref);
		store_state(iesp);
		iosp -= 3;
		/* Open code "up" for the array case(s) */
		   {	ref *whichp =
			  (iosp[1].value.index ? iosp + 2 : iosp + 3);
			switch( r_type_xe(whichp) )
			   {
			default:
				s_store_b(iesp, 1, whichp, 0);
				iref = iesp + 1;
				icount = 0;
				goto top;
			case exec(t_array): ;
			case exec(t_packedarray): ;
			   }
			iref = whichp->value.refs;
			icount = whichp->size;
			if ( --icount <= 0 )	/* <= 1 more elements */
			   {	if ( icount < 0 ) goto up;
			   }
			else
			   {	iesp++;
				s_store_i(iesp, whichp);
			   }
		   }
		goto top;
	case plain_exec(tx_op_le):
		code = obj_compare(iosp, 2+1);
		if ( code < 0 )
			return_with_error (code, iref);
		iosp--;
		make_bool(iosp, code);
		next();
	case plain_exec(tx_op_pop):
		if ( iosp < osp_nargs[1] )
			return_with_error (e_stackunderflow, iref);
		iosp--;
		next();
	case plain_exec(tx_op_sub):
		if ( (code = zsub(iosp)) < 0 )
			return_with_error (code, iref);
		iosp--;
		next();
	/* Executable types. */
	case plain_exec(t_null):
		goto bot;
	case plain_exec(t_operator):
	   {	esp = iesp;		/* save for operator */
		osp = iosp;		/* ditto */
		/* Operator routines take osp as an argument. */
		/* This is just a convenience, since they adjust */
		/* osp themselves to reflect the results. */
		/* Operators that (net) push information on the */
		/* operand stack must check for overflow: */
		/* this normally happens automatically through */
		/* the push macro (in oper.h). */
		/* Operators that do not typecheck their operands */
		/* must check explicitly for stack underflow. */
		code = (*iref->value.opproc)(iosp);
		iosp = osp;
		if ( code != 0 )
		   {	/* This might be a control operator that changed */
			/* esp.  Check for this specially. */
			switch ( code )
			   {
			case o_check_estack:
				/* If a control operator popped the estack, */
				/* we just go to up.  If a control operator */
				/* pushed something on the estack, we store */
				/* the state and then go to up.  Otherwise, */
				/* we can just go on. */
				if ( esp > iesp ) store_state(iesp);
				else if ( esp == iesp ) goto bot;
				iesp = esp;
				goto up;
			case e_typecheck:
				/* This might be an operand stack */
				/* underflow: check the required # of */
				/* operands now. */
				if ( iosp < osbot - 1 + iref->size )
					code = e_stackunderflow;
				/* (falls through) */
			default:
				return_with_error (code, iref);
			   }
		   }
		next();			/* just go on */
	   }
	case plain_exec(t_name):
	   {	register ref *pvalue = iref->value.pname->pvalue;
		if ( !pv_valid(pvalue) )
		   {	ref *pdvalue;
			if ( dict_lookup(dstack, dsp, iref, &pdvalue) <= 0 )
				return_with_error (e_undefined, iref);
			pvalue = pdvalue;
		   }
		switch ( r_type_xe(pvalue) )
		   {
		case exec(t_array):
		case exec(t_packedarray):
			/* This is an executable procedure, */
			/* execute it. */
			/* Save the current state on the e-stack */
			store_state(iesp);
			/* Push the array on the e-stack */
			if ( iesp >= estop )
				return_with_error (e_execstackoverflow, pvalue);
			++iesp;
			s_store_i(iesp, pvalue);
			iref = pvalue->value.refs;
			icount = pvalue->size - 1;
			if ( icount <= 0 )
			   {	if ( icount < 0 ) goto up;	/* empty procedure */
				iesp--;		/* tail recursion */
			   }
			goto top;
		case plain_exec(t_operator):
		   {	/* Shortcut for operators. */
			/* See above for the logic. */
			esp = iesp;
			osp = iosp;
			code = (*pvalue->value.opproc)(iosp);
			iosp = osp;
			if ( code != 0 )
			   {	switch ( code )
				   {
				case o_check_estack:
				   {	if ( esp > iesp )
					  store_state(iesp);
					else if ( esp == iesp )
					  goto bot;
					iesp = esp;
					goto up;
				   }
				case e_typecheck:
					if ( iosp < osbot - 1 + pvalue->size )
					  code = e_stackunderflow;
				   }
				return_with_error (code, pvalue);
			   }
			next();
		   }
		case lit(t_array):
		case plain(t_boolean):
		case plain(t_integer):
		case lit(t_packedarray):
		case plain(t_real):
		case lit(t_string):
			/* Just push the value */
			if ( iosp >= ostop )
				return_with_error (e_stackoverflow, pvalue);
			++iosp;
			s_store_i(iosp, pvalue);
			next();
		default:		/* handles other literals */
			/* Not a procedure, reinterpret it. */
			store_state(iesp);
			icount = 0;
			iref = pvalue;
			goto top;
		   }
	   }
	case exec(t_file):
	   {	/* Executable file.  Read the next token and interpret it. */
	   	stream *s;
		code = file_check_read(iref, &s);
		if ( code < 0 ) return_with_error (code, iref);
		osp = iosp;		/* scan_token uses ostack */
		switch ( code = scan_token(s, 0, &token) )
		   {
		case 0:			/* read a token */
			store_state(iesp);
			/* Push the file on the e-stack */
			if ( iesp >= estop )
				return_with_error (e_execstackoverflow, iref);
			*++iesp = *iref;
			iref = &token;
			icount = 0;
			goto top;
		case 1:			/* end of file */
			code = file_close(iref, s);
			if ( code < 0 ) return_with_error (code, iref);
			goto bot;
		default:		/* error */
			return_with_error (code, iref);
		   }
	   }
	case exec(t_string):
	   {	/* Executable string.  Read a token and interpret it. */
		stream ss;
		sread_string(&ss, iref->value.bytes, iref->size);
		osp = iosp;		/* scan_token uses ostack */
		switch ( code = scan_token(&ss, 1, &token) )
		  {
		case 0:			/* read a token */
		    store_state(iesp);
		    /* Push the updated string back on the e-stack */
		    if ( iesp >= estop )
		      return_with_error (e_execstackoverflow, iref);
		    ++iesp;
		    iesp->type_attrs = iref->type_attrs;
		    iesp->value.bytes = ss.cptr + 1;
		    iesp->size = ss.cbuf + ss.bsize - ss.cptr - 1;
		    iref = &token;
		    icount = 0;
		    goto top;
		case 1:			/* end of string */
		    goto bot;
		default:		/* error */
		    return_with_error (code, iref);
		  }
	   }
	   }
	/* Literal type, just push it. */
	if ( iosp >= ostop ) return_with_error (e_stackoverflow, iref);
	++iosp;
	s_store_i(iosp, iref);
bot:	next();
out:	/* At most 1 more token in the current procedure. */
	/* (We already decremented icount.) */
	if ( !icount )
	   {	/* Pop the execution stack for tail recursion. */
		iesp--;
		iref++;
		goto top;
	   }
up:	/* See if there is anything left on the execution stack. */
	switch ( r_type_xe(iesp) )
	   {
	default:
		iref = iesp--;
		icount = 0;
		goto top;
	case exec(t_array): ;
	case exec(t_packedarray): ;
	   }
	iref = iesp->value.refs;		/* next element of array */
	icount = iesp->size - 1;
	if ( icount <= 0 )		/* <= 1 more elements */
	   {	iesp--;			/* pop, or tail recursion */
		if ( icount < 0 ) goto up;
	   }
	goto top;
}
