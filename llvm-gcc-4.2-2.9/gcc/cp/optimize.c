/* Perform optimizations on tree structure.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2005
   Free Software Foundation, Inc.
   Written by Mark Michell (mark@codesourcery.com).

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "cp-tree.h"
#include "rtl.h"
#include "insn-config.h"
#include "input.h"
#include "integrate.h"
#include "toplev.h"
#include "varray.h"
#include "params.h"
#include "hashtab.h"
#include "target.h"
#include "debug.h"
#include "tree-inline.h"
#include "flags.h"
#include "langhooks.h"
#include "diagnostic.h"
#include "tree-dump.h"
#include "tree-gimple.h"

/* APPLE LOCAL begin ARM structor thunks */
/* We detect cases where two cloned structors are identical,
   and replace the body of one by a call to the other.  This
   is normally shrunk further by the sibcall optimization later. 

   More could be done along these lines.  Where the clones are
   not identical, typically one consists of code identical to
   the other one plus some additional code.  It is possible
   to replace the duplicated code with a call to the smaller
   function.

   This is primarily a space optimization.  One extra unconditional
   branch may be executed.  This is cheap or free on most modern
   hardware and is probably less important than getting the size
   down to reduce cache and paging problems (ymmv), so the
   optimization is done unconditionally.
*/
enum in_charge_use 
{ 
  NO_THUNKS,	/* cannot use thunks */
  ALL_THUNKS,	/* all clones are equivalent (no in-charge
		   param, or unreferenced */
  IN_CHARGE_1,	/* all uses of in-charge parm AND it with 1, so
		   clones with in-charge==0 and 2 are equivalent,
		   likewise 1 and 3 */
  IN_CHARGE_0	/* all uses of in-charge parm test it for equality
		   with 0, so clones with in-charge==1, 2 and 3
		   are equivalent */
};
struct thunk_tree_walk_data 
{
  tree in_charge_parm;
  enum in_charge_use in_charge_use;
};
struct clone_info 
{
  int next_clone;
  tree in_charge_value[3];
  tree clones[3];
  enum in_charge_use which_thunks_ok;
};
/* APPLE LOCAL end ARM structor thunks */

/* Prototypes.  */

static void update_cloned_parm (tree, tree, bool);
/* APPLE LOCAL begin ARM structor thunks */
static void thunk_body (tree, tree, tree);
static tree examine_tree_for_in_charge_use (tree *, int *, void *);
static enum in_charge_use compute_use_thunks (tree);
static tree find_earlier_clone (struct clone_info *);
/* APPLE LOCAL end ARM structor thunks */

/* CLONED_PARM is a copy of CLONE, generated for a cloned constructor
   or destructor.  Update it to ensure that the source-position for
   the cloned parameter matches that for the original, and that the
   debugging generation code will be able to find the original PARM.  */

static void
update_cloned_parm (tree parm, tree cloned_parm, bool first)
{
  DECL_ABSTRACT_ORIGIN (cloned_parm) = parm;

  /* We may have taken its address.  */
  TREE_ADDRESSABLE (cloned_parm) = TREE_ADDRESSABLE (parm);

  /* The definition might have different constness.  */
  TREE_READONLY (cloned_parm) = TREE_READONLY (parm);

  TREE_USED (cloned_parm) = !first || TREE_USED (parm);

  /* The name may have changed from the declaration.  */
  DECL_NAME (cloned_parm) = DECL_NAME (parm);
  DECL_SOURCE_LOCATION (cloned_parm) = DECL_SOURCE_LOCATION (parm);
  TREE_TYPE (cloned_parm) = TREE_TYPE (parm);

  DECL_COMPLEX_GIMPLE_REG_P (cloned_parm) = DECL_COMPLEX_GIMPLE_REG_P (parm);
}

/* APPLE LOCAL begin ARM structor thunks */
/* Callback for walk_tree. We compute data->in_charge_use.  */

static tree
examine_tree_for_in_charge_use (tree *tp, int *walk_subtrees, void *vdata)
{
  struct thunk_tree_walk_data *data = (struct thunk_tree_walk_data *) vdata;
  switch (TREE_CODE (*tp))
    {
      case PARM_DECL:
	if (*tp == data->in_charge_parm)
	  data->in_charge_use = NO_THUNKS;
	return NULL;
      case BIT_AND_EXPR:
	if (TREE_OPERAND (*tp, 0) == data->in_charge_parm
	    && integer_onep (TREE_OPERAND (*tp, 1)))
	  {
	    *walk_subtrees = 0;
	    if (data->in_charge_use == ALL_THUNKS
		|| data->in_charge_use == IN_CHARGE_1)
	      data->in_charge_use = IN_CHARGE_1;
	    else
	      data->in_charge_use = NO_THUNKS;
	  }
	return NULL;
      case EQ_EXPR:
      case NE_EXPR:
	if (TREE_OPERAND (*tp, 0) == data->in_charge_parm
	    && integer_zerop (TREE_OPERAND (*tp, 1)))
	  {
	    *walk_subtrees = 0;
	    if (data->in_charge_use == ALL_THUNKS
		|| data->in_charge_use == IN_CHARGE_0)
	      data->in_charge_use = IN_CHARGE_0;
	    else
	      data->in_charge_use = NO_THUNKS;
	  }
	return NULL;
      default:
	return NULL;
    }
}

/* Determine which clones of FN can use the thunk implementation. */

static enum in_charge_use
compute_use_thunks (tree fn)
{
  tree last_arg, fn_parm;

  if (DECL_HAS_VTT_PARM_P (fn))
    return NO_THUNKS;

  if (flag_apple_kext)
    return NO_THUNKS;

  if (flag_clone_structors)
    return NO_THUNKS;

  /* Functions that are too small will just get inlined back in anyway.
     Let the inliner do something useful instead.  */
  if (flag_inline_functions
      && estimate_num_insns (DECL_SAVED_TREE (fn)) < MAX_INLINE_INSNS_AUTO)
    return NO_THUNKS;

  /* If function accepts variable arguments, give up.  */
  last_arg = tree_last (TYPE_ARG_TYPES (TREE_TYPE (fn)));
  if ( ! VOID_TYPE_P (TREE_VALUE (last_arg)))
       return NO_THUNKS;

  /* If constructor expects vector (AltiVec) arguments, give up.  */
  for (fn_parm = DECL_ARGUMENTS (fn); fn_parm; fn_parm = TREE_CHAIN (fn_parm))
    if (TREE_CODE (fn_parm) == VECTOR_TYPE)
      return NO_THUNKS;

  if (DECL_HAS_IN_CHARGE_PARM_P (fn))
    {
      int parmno;
      struct thunk_tree_walk_data data;
      for (parmno = 0, fn_parm = DECL_ARGUMENTS (fn);
	   fn_parm;
	   ++parmno, fn_parm = TREE_CHAIN (fn_parm))
	if (parmno == 1)
	  {
	    data.in_charge_parm = fn_parm;
	    break;
	  }
      /* If every use of the in-charge parameter ANDs it
	 with 1, then the functions that have in-charge
	 set to 1 and 3 are equivalent, likewise 0 and 2.
	 Check for this (common in practice).  Likewise,
	 if every use tests for equality with 0, then
	 values 1, 2 and 3 are equivalent.  */
      gcc_assert (data.in_charge_parm != NULL_TREE);
      data.in_charge_use = ALL_THUNKS;
      walk_tree_without_duplicates (&DECL_SAVED_TREE (fn), 
				    examine_tree_for_in_charge_use, 
				    &data);
      return data.in_charge_use;
    }

  return ALL_THUNKS;
}

/* An earlier version of this is in Apple's 4.0 tree, and some of the
   modifications here are in 3983462. */

/* FN is a constructor or destructor, and there are FUNCTION_DECLs
   cloned from it nearby.  Instead of cloning this body, leave it
   alone and create tiny one-call bodies for the cloned
   FUNCTION_DECLs.  These clones are sibcall candidates, and their
   resulting code will be very thunk-esque.  */
static void
thunk_body (tree clone, tree fn, tree clone_to_call)
{
  tree bind, block, call, fn_parm, fn_parm_typelist;
  int parmno, vtt_parmno;
  tree clone_parm, parmlist;

  for (vtt_parmno = -1, parmno = 0, fn_parm = DECL_ARGUMENTS (fn);
       fn_parm;
       ++parmno, fn_parm = TREE_CHAIN (fn_parm))
    {
      if (DECL_ARTIFICIAL (fn_parm) && DECL_NAME (fn_parm) == vtt_parm_identifier)
	{
	  vtt_parmno = parmno;	/* Compensate for removed in_charge parameter.  */
	  break;
	}
    }
  /* Currently, we are not supposed to have a vtt argument. */
  gcc_assert(vtt_parmno == -1);

  /* Walk parameter lists together, creating parameter list for call to original function.  */
  for (parmno = 0,
	 parmlist = NULL,
	 fn_parm = DECL_ARGUMENTS (fn),
	 fn_parm_typelist = TYPE_ARG_TYPES (TREE_TYPE (fn)),
	 clone_parm = DECL_ARGUMENTS (clone);
       fn_parm;
       ++parmno,
	 fn_parm = TREE_CHAIN (fn_parm))
    {
      if (parmno == vtt_parmno && ! DECL_HAS_VTT_PARM_P (clone))
	{
	  tree typed_null_pointer_node = copy_node (null_pointer_node);
	  gcc_assert (fn_parm_typelist);
	  /* Clobber actual parameter with formal parameter type.  */
	  TREE_TYPE (typed_null_pointer_node) = TREE_VALUE (fn_parm_typelist);
	  parmlist = tree_cons (NULL, typed_null_pointer_node, parmlist);
	}
      else if (parmno == 1 && DECL_HAS_IN_CHARGE_PARM_P (fn))
	{
	  /* Just skip it. */
	}
      /* Map other parameters to their equivalents in the cloned
	 function.  */
      else
	{
	  gcc_assert (clone_parm);
	  DECL_ABSTRACT_ORIGIN (clone_parm) = NULL;
	  parmlist = tree_cons (NULL, clone_parm, parmlist);
	  clone_parm = TREE_CHAIN (clone_parm);
	}
      if (fn_parm_typelist)
	fn_parm_typelist = TREE_CHAIN (fn_parm_typelist);
    }

    /* We built this list backwards; fix now.  */
    parmlist = nreverse (parmlist);

    TREE_USED (clone_to_call) = 1;
    call = build_cxx_call (clone_to_call, parmlist);

    for (parmlist = TREE_OPERAND (call, 1); parmlist; parmlist = TREE_CHAIN (parmlist))
      {
	fn_parm = TREE_VALUE (parmlist);
	/* Remove the EMPTY_CLASS_EXPR because it upsets estimate_num_insns().  */
	if (TREE_CODE (fn_parm) == COMPOUND_EXPR)
	  {
	    gcc_assert (TREE_CODE (TREE_OPERAND (fn_parm, 1)) == EMPTY_CLASS_EXPR);
	    TREE_VALUE (parmlist) = TREE_OPERAND (fn_parm, 0);
	  }
      }
    block = make_node (BLOCK);
    if (targetm.cxx.cdtor_returns_this ())
      {
	tree clone_result = DECL_RESULT (clone);
	tree modify = build2 (MODIFY_EXPR, TREE_TYPE (clone_result), clone_result, call);
	add_stmt (modify);
	BLOCK_VARS (block) = clone_result;
      }
    else
      {
	add_stmt (call);
      }
    bind = c_build_bind_expr (block, cur_stmt_list);
    DECL_SAVED_TREE (clone) = push_stmt_list ();
    add_stmt (bind);
}

/* Determine whether the current clone (the one indexed by
   INFO->NEXT_CLONE) can be implemented by a call to an
   earlier (already emitted) clone. */

static tree
find_earlier_clone (struct clone_info* info)
{
  int i;

  if (info->which_thunks_ok == NO_THUNKS
      || info->next_clone == 0)
    return NULL_TREE;

  if (info->which_thunks_ok == ALL_THUNKS)
    return info->clones [0];

  if (info->which_thunks_ok == IN_CHARGE_1)
    for (i = 0; i < info->next_clone; i++)
      if ((TREE_INT_CST_LOW (info->in_charge_value [i]) & 1)
	  == (TREE_INT_CST_LOW (info->in_charge_value [info->next_clone]) & 1))
	return info->clones [i];

  if (info->which_thunks_ok == IN_CHARGE_0)
    for (i = 0; i < info->next_clone; i++)
      if ((TREE_INT_CST_LOW (info->in_charge_value [i]) == 0)
	  == (TREE_INT_CST_LOW (info->in_charge_value [info->next_clone]) == 0))
	return info->clones [i];

  return NULL_TREE;
}
/* APPLE LOCAL end ARM structor thunks */

/* FN is a function that has a complete body.  Clone the body as
   necessary.  Returns nonzero if there's no longer any need to
   process the main body.  */

bool
maybe_clone_body (tree fn)
{
  tree clone;
  bool first = true;
/* APPLE LOCAL begin ARM structor thunks */
  tree clone_to_call;
  struct clone_info info;
/* APPLE LOCAL end ARM structor thunks */

  /* We only clone constructors and destructors.  */
  if (!DECL_MAYBE_IN_CHARGE_CONSTRUCTOR_P (fn)
      && !DECL_MAYBE_IN_CHARGE_DESTRUCTOR_P (fn))
    return 0;

  /* Emit the DWARF1 abstract instance.  */
  (*debug_hooks->deferred_inline_function) (fn);

/* APPLE LOCAL begin ARM structor thunks */
  /* Figure out whether we can use the 'thunk' implementation,
     and if so on which clones. */
  info.next_clone = 0;
  info.which_thunks_ok = compute_use_thunks (fn);
/* APPLE LOCAL end ARM structor thunks */

  /* We know that any clones immediately follow FN in the TYPE_METHODS
     list.  */
  push_to_top_level ();
  FOR_EACH_CLONE (clone, fn)
    {
      tree parm;
      tree clone_parm;
      int parmno;
      splay_tree decl_map;

      /* Update CLONE's source position information to match FN's.  */
      DECL_SOURCE_LOCATION (clone) = DECL_SOURCE_LOCATION (fn);
      DECL_INLINE (clone) = DECL_INLINE (fn);
      DECL_DECLARED_INLINE_P (clone) = DECL_DECLARED_INLINE_P (fn);
      /* LLVM LOCAL begin inlinehint attribute */
      DECL_EXPLICIT_INLINE_P (clone) = DECL_EXPLICIT_INLINE_P (fn);
      /* LLVM LOCAL end inlinehint attribute */
      DECL_COMDAT (clone) = DECL_COMDAT (fn);
      DECL_WEAK (clone) = DECL_WEAK (fn);
      DECL_ONE_ONLY (clone) = DECL_ONE_ONLY (fn);
      DECL_SECTION_NAME (clone) = DECL_SECTION_NAME (fn);
      DECL_USE_TEMPLATE (clone) = DECL_USE_TEMPLATE (fn);
      DECL_EXTERNAL (clone) = DECL_EXTERNAL (fn);
      DECL_INTERFACE_KNOWN (clone) = DECL_INTERFACE_KNOWN (fn);
      DECL_NOT_REALLY_EXTERN (clone) = DECL_NOT_REALLY_EXTERN (fn);
      TREE_PUBLIC (clone) = TREE_PUBLIC (fn);
      DECL_VISIBILITY (clone) = DECL_VISIBILITY (fn);
      DECL_VISIBILITY_SPECIFIED (clone) = DECL_VISIBILITY_SPECIFIED (fn);

      /* Adjust the parameter names and locations.  */
      parm = DECL_ARGUMENTS (fn);
      clone_parm = DECL_ARGUMENTS (clone);
      /* Update the `this' parameter, which is always first.  */
      update_cloned_parm (parm, clone_parm, first);
      parm = TREE_CHAIN (parm);
      clone_parm = TREE_CHAIN (clone_parm);
      if (DECL_HAS_IN_CHARGE_PARM_P (fn))
	parm = TREE_CHAIN (parm);
      if (DECL_HAS_VTT_PARM_P (fn))
	parm = TREE_CHAIN (parm);
      if (DECL_HAS_VTT_PARM_P (clone))
	clone_parm = TREE_CHAIN (clone_parm);
      for (; parm;
	   parm = TREE_CHAIN (parm), clone_parm = TREE_CHAIN (clone_parm))
	/* Update this parameter.  */
	update_cloned_parm (parm, clone_parm, first);

      /* Start processing the function.  */
      start_preparsed_function (clone, NULL_TREE, SF_PRE_PARSED);

      /* Remap the parameters.  */
      decl_map = splay_tree_new (splay_tree_compare_pointers, NULL, NULL);
      for (parmno = 0,
	     parm = DECL_ARGUMENTS (fn),
	     clone_parm = DECL_ARGUMENTS (clone);
	   parm;
	   ++parmno,
	     parm = TREE_CHAIN (parm))
	{
	  /* Map the in-charge parameter to an appropriate constant.  */
	  if (DECL_HAS_IN_CHARGE_PARM_P (fn) && parmno == 1)
	    {
	      tree in_charge;
	      in_charge = in_charge_arg_for_name (DECL_NAME (clone));
	      splay_tree_insert (decl_map,
				 (splay_tree_key) parm,
				 (splay_tree_value) in_charge);
	      /* APPLE LOCAL ARM structor thunks */
	      info.in_charge_value [info.next_clone] = in_charge;
	    }
	  else if (DECL_ARTIFICIAL (parm)
		   && DECL_NAME (parm) == vtt_parm_identifier)
	    {
	      /* For a subobject constructor or destructor, the next
		 argument is the VTT parameter.  Remap the VTT_PARM
		 from the CLONE to this parameter.  */
	      if (DECL_HAS_VTT_PARM_P (clone))
		{
		  DECL_ABSTRACT_ORIGIN (clone_parm) = parm;
		  splay_tree_insert (decl_map,
				     (splay_tree_key) parm,
				     (splay_tree_value) clone_parm);
		  clone_parm = TREE_CHAIN (clone_parm);
		}
	      /* Otherwise, map the VTT parameter to `NULL'.  */
	      else
		{
		  splay_tree_insert (decl_map,
				     (splay_tree_key) parm,
				     (splay_tree_value) null_pointer_node);
		}
	    }
	  /* Map other parameters to their equivalents in the cloned
	     function.  */
	  else
	    {
	      splay_tree_insert (decl_map,
				 (splay_tree_key) parm,
				 (splay_tree_value) clone_parm);
	      clone_parm = TREE_CHAIN (clone_parm);
	    }
	}

      if (targetm.cxx.cdtor_returns_this ())
	{
	  parm = DECL_RESULT (fn);
	  clone_parm = DECL_RESULT (clone);
	  splay_tree_insert (decl_map, (splay_tree_key) parm,
			     (splay_tree_value) clone_parm);
	}
      /* APPLE LOCAL begin ARM structor thunks */
      clone_to_call = find_earlier_clone (&info);
      if (clone_to_call)
	/* Bodies are identical; replace later one with call to an
	   earlier one. */
	thunk_body (clone, fn, clone_to_call);
      else
	/* Clone the body.  */
	clone_body (clone, fn, decl_map);
      /* APPLE LOCAL end ARM structor thunks */

      /* The clone can throw iff the original function can throw.  */
      cp_function_chain->can_throw = !TREE_NOTHROW (fn);

      /* Now, expand this function into RTL, if appropriate.  */
      finish_function (0);
      BLOCK_ABSTRACT_ORIGIN (DECL_INITIAL (clone)) = DECL_INITIAL (fn);
      expand_or_defer_fn (clone);
      first = false;
      /* APPLE LOCAL begin ARM structor thunks */
      info.clones [info.next_clone] = clone;
      info.next_clone++;
      /* APPLE LOCAL end ARM structor thunks */
    }
  pop_from_top_level ();

  /* We don't need to process the original function any further.  */
  return 1;
}
