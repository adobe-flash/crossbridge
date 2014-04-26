// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/* LLVM LOCAL begin - AVM2 target */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-attr.h"
#include "flags.h"
#include "recog.h"
#include "tree.h"
#include "output.h"
#include "expr.h"
#include "obstack.h"
#include "except.h"
#include "function.h"
#include "toplev.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "cgraph.h"

/* Initialize the GCC target structure.  */
const struct attribute_spec avm2_attribute_table[];
#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE avm2_attribute_table

#ifdef SUBTARGET_INSERT_ATTRIBUTES
#undef TARGET_INSERT_ATTRIBUTES
#define TARGET_INSERT_ATTRIBUTES SUBTARGET_INSERT_ATTRIBUTES
#endif

/* Initialize our target machine, mostly with defaults since this is an llvm only machine */
struct gcc_target targetm = TARGET_INITIALIZER;

// Disable function sections.
int flag_function_sections = 0;

/* Table of valid machine attributes.  */
static tree avm2_handle_cconv_attribute (tree *, tree, tree, int, bool *);
static tree avm2_handle_struct_attribute (tree *, tree, tree, int, bool *);
static tree avm2_handle_fndecl_attribute (tree *, tree, tree, int, bool *);
const struct attribute_spec avm2_attribute_table[] =
{
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler } */
  /* Stdcall attribute says callee is responsible for popping arguments
     if they are not variable.  */
  { "stdcall",   0, 0, false, true,  true,  avm2_handle_cconv_attribute },
  /* Fastcall attribute says callee is responsible for popping arguments
     if they are not variable.  */
  { "fastcall",  0, 0, false, true,  true,  avm2_handle_cconv_attribute },
  /* Cdecl attribute says the callee is a normal C declaration */
  { "cdecl",     0, 0, false, true,  true,  avm2_handle_cconv_attribute },
  /* Regparm attribute specifies how many integer arguments are to be
     passed in registers.  */
  { "regparm",   1, 1, false, true,  true,  avm2_handle_cconv_attribute },
  { "naked",     0, 0, true, false,  false,  avm2_handle_fndecl_attribute },
  { "ms_struct", 0, 0, false, false,  false, avm2_handle_struct_attribute },
  { "gcc_struct", 0, 0, false, false,  false, avm2_handle_struct_attribute },
  { NULL,        0, 0, false, false, false, NULL }
};

/* For now AVM2 mimics X86 but does not enable SSE or MMX access */
enum reg_class const regclass_map[FIRST_PSEUDO_REGISTER] =
{
  /* ax, dx, cx, bx */
  AREG, DREG, CREG, BREG,
  /* si, di, bp, sp */
  SIREG, DIREG, NON_Q_REGS, NON_Q_REGS,
  /* FP registers */
  FP_TOP_REG, FP_SECOND_REG, FLOAT_REGS, FLOAT_REGS,
  FLOAT_REGS, FLOAT_REGS, FLOAT_REGS, FLOAT_REGS,
  /* arg pointer */
  NON_Q_REGS,
  /* flags, fpsr, dirflag, frame */
  NO_REGS, NO_REGS, NO_REGS, NON_Q_REGS,
  SSE_REGS, SSE_REGS, SSE_REGS, SSE_REGS, SSE_REGS, SSE_REGS,
  SSE_REGS, SSE_REGS,
  MMX_REGS, MMX_REGS, MMX_REGS, MMX_REGS, MMX_REGS, MMX_REGS,
  MMX_REGS, MMX_REGS,
  NON_Q_REGS, NON_Q_REGS, NON_Q_REGS, NON_Q_REGS,
  NON_Q_REGS, NON_Q_REGS, NON_Q_REGS, NON_Q_REGS,
  SSE_REGS, SSE_REGS, SSE_REGS, SSE_REGS, SSE_REGS, SSE_REGS,
  SSE_REGS, SSE_REGS,
};

/* avm2_regparm_string as a number */
/* LLVM LOCAL begin */
#ifndef ENABLE_LLVM
static
#endif
int avm2_regparm = 0;
/* LLVM LOCAL end */

char *SetFlasccSDKLocation(const char *suffix);

char *avm2_build_sysroot_path(const char *sysroot, const char *path)
{
    return concat(SetFlasccSDKLocation("/../../../.."), path, NULL);
}

static int avm2_function_regparm (tree, tree);

/* Handle "cdecl", "stdcall", "fastcall", "regparm" and "sseregparm"
   calling convention attributes;
   arguments as in struct attribute_spec.handler.  */

static tree
avm2_handle_cconv_attribute (tree *node, tree name,
				   tree args,
				   int flags ATTRIBUTE_UNUSED,
				   bool *no_add_attrs)
{
  if (TREE_CODE (*node) != FUNCTION_TYPE
      && TREE_CODE (*node) != METHOD_TYPE
      && TREE_CODE (*node) != FIELD_DECL
      && TREE_CODE (*node) != TYPE_DECL)
    {
      warning (OPT_Wattributes, "%qs attribute only applies to functions",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
      return NULL_TREE;
    }

  /* Can combine regparm with all attributes but fastcall.  */
  if (is_attribute_p ("regparm", name))
    {
      tree cst;

      if (lookup_attribute ("fastcall", TYPE_ATTRIBUTES (*node)))
        {
	  error ("fastcall and regparm attributes are not compatible");
	}

      cst = TREE_VALUE (args);
      if (TREE_CODE (cst) != INTEGER_CST)
	{
	  warning (OPT_Wattributes,
		   "%qs attribute requires an integer constant argument",
		   IDENTIFIER_POINTER (name));
	  *no_add_attrs = true;
	}
      else if (compare_tree_int (cst, REGPARM_MAX) > 0)
	{
	  warning (OPT_Wattributes, "argument to %qs attribute larger than %d",
		   IDENTIFIER_POINTER (name), REGPARM_MAX);
	  *no_add_attrs = true;
	}
      return NULL_TREE;
    }

  /* Can combine fastcall with stdcall (redundant) and sseregparm.  */
  /* APPLE LOCAL begin regparmandstackparm */
  if (is_attribute_p ("fastcall", name)
      || is_attribute_p ("regparmandstackparm", name))
  /* APPLE LOCAL end regparmandstackparm */
    {
      if (lookup_attribute ("cdecl", TYPE_ATTRIBUTES (*node)))
        {
	  error ("fastcall and cdecl attributes are not compatible");
	}
      if (lookup_attribute ("stdcall", TYPE_ATTRIBUTES (*node)))
        {
	  error ("fastcall and stdcall attributes are not compatible");
	}
      if (lookup_attribute ("regparm", TYPE_ATTRIBUTES (*node)))
        {
	  error ("fastcall and regparm attributes are not compatible");
	}
    }

  /* Can combine stdcall with fastcall (redundant), regparm and
     sseregparm.  */
  else if (is_attribute_p ("stdcall", name))
    {
      if (lookup_attribute ("cdecl", TYPE_ATTRIBUTES (*node)))
        {
	  error ("stdcall and cdecl attributes are not compatible");
	}
      /* APPLE LOCAL begin regparmandstackparm */
      if (lookup_attribute ("fastcall", TYPE_ATTRIBUTES (*node))
	  || lookup_attribute ("regparmandstackparm", TYPE_ATTRIBUTES (*node)))
      /* APPLE LOCAL end regparmandstackparm */
        {
	  error ("stdcall and fastcall attributes are not compatible");
	}
    }

  /* Can combine cdecl with regparm and sseregparm.  */
  else if (is_attribute_p ("cdecl", name))
    {
      if (lookup_attribute ("stdcall", TYPE_ATTRIBUTES (*node)))
        {
	  error ("stdcall and cdecl attributes are not compatible");
	}
      if (lookup_attribute ("fastcall", TYPE_ATTRIBUTES (*node)))
        {
	  error ("fastcall and cdecl attributes are not compatible");
	}
    }

  /* Can combine sseregparm with all attributes.  */

  return NULL_TREE;
}

/* Handle a "ms_struct" or "gcc_struct" attribute; arguments as in
   struct attribute_spec.handler.  */
static tree
avm2_handle_struct_attribute (tree *node, tree name,
			      tree args ATTRIBUTE_UNUSED,
			      int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
  tree *type = NULL;
  if (DECL_P (*node))
    {
      if (TREE_CODE (*node) == TYPE_DECL)
	type = &TREE_TYPE (*node);
    }
  else
    type = node;

  if (!(type && (TREE_CODE (*type) == RECORD_TYPE
		 || TREE_CODE (*type) == UNION_TYPE)))
    {
      warning (OPT_Wattributes, "%qs attribute ignored",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }

  else if ((is_attribute_p ("ms_struct", name)
	    && lookup_attribute ("gcc_struct", TYPE_ATTRIBUTES (*type)))
	   || ((is_attribute_p ("gcc_struct", name)
		&& lookup_attribute ("ms_struct", TYPE_ATTRIBUTES (*type)))))
    {
      warning (OPT_Wattributes, "%qs incompatible attribute ignored",
               IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }

  return NULL_TREE;
}

/* Handle an attribute requiring a FUNCTION_DECL;
   arguments as in struct attribute_spec.handler.  */
static tree
avm2_handle_fndecl_attribute (tree *node, tree name,
		tree args,
		int flags ATTRIBUTE_UNUSED,
		bool *no_add_attrs)
{
  if (TREE_CODE (*node) != FUNCTION_DECL)
  {
    warning (OPT_Wattributes, "%qs attribute only applies to functions",
	   IDENTIFIER_POINTER (name));
    *no_add_attrs = true;
  }
  return NULL_TREE;
}

/* Extract the parts of an RTL expression that is a valid memory address
   for an instruction.  Return 0 if the structure of the address is
   grossly off.  Return -1 if the address contains ASHIFT, so it is not
   strictly valid, but still used for computing length of lea instruction.  */

int
avm2_decompose_address (rtx addr, struct avm2_address *out)
{
  rtx base = NULL_RTX, index = NULL_RTX, disp = NULL_RTX;
  rtx base_reg, index_reg;
  HOST_WIDE_INT scale = 1;
  rtx scale_rtx = NULL_RTX;
  int retval = 1;
  enum avm2_address_seg seg = SEG_DEFAULT;

  if (GET_CODE (addr) == REG || GET_CODE (addr) == SUBREG)
    base = addr;
  else if (GET_CODE (addr) == PLUS)
    {
      rtx addends[4], op;
      int n = 0, i;

      op = addr;
      do
	{
	  if (n >= 4)
	    return 0;
	  addends[n++] = XEXP (op, 1);
	  op = XEXP (op, 0);
	}
      while (GET_CODE (op) == PLUS);
      if (n >= 4)
	return 0;
      addends[n] = op;

      for (i = n; i >= 0; --i)
	{
	  op = addends[i];
	  switch (GET_CODE (op))
	    {
	    case MULT:
	      if (index)
		return 0;
	      index = XEXP (op, 0);
	      scale_rtx = XEXP (op, 1);
	      break;

	    case UNSPEC:
		  return 0;
	      break;

	    case REG:
	    case SUBREG:
	      if (!base)
		base = op;
	      else if (!index)
		index = op;
	      else
		return 0;
	      break;

	    case CONST:
	    case CONST_INT:
	    case SYMBOL_REF:
	    case LABEL_REF:
	      if (disp)
		return 0;
	      disp = op;
	      break;

	    default:
	      return 0;
	    }
	}
    }
  else if (GET_CODE (addr) == MULT)
    {
      index = XEXP (addr, 0);		/* index*scale */
      scale_rtx = XEXP (addr, 1);
    }
  else if (GET_CODE (addr) == ASHIFT)
    {
      rtx tmp;

      /* We're called for lea too, which implements ashift on occasion.  */
      index = XEXP (addr, 0);
      tmp = XEXP (addr, 1);
      if (GET_CODE (tmp) != CONST_INT)
	return 0;
      scale = INTVAL (tmp);
      if ((unsigned HOST_WIDE_INT) scale > 3)
	return 0;
      scale = 1 << scale;
      retval = -1;
    }
  else
    disp = addr;			/* displacement */

  /* Extract the integral value of scale.  */
  if (scale_rtx)
    {
      if (GET_CODE (scale_rtx) != CONST_INT)
	return 0;
      scale = INTVAL (scale_rtx);
    }

  base_reg = base && GET_CODE (base) == SUBREG ? SUBREG_REG (base) : base;
  index_reg = index && GET_CODE (index) == SUBREG ? SUBREG_REG (index) : index;

  /* Allow arg pointer and stack pointer as index if there is not scaling.  */
  if (base_reg && index_reg && scale == 1
      && (index_reg == arg_pointer_rtx
	  || index_reg == frame_pointer_rtx
	  || (REG_P (index_reg) && REGNO (index_reg) == STACK_POINTER_REGNUM)))
    {
      rtx tmp;
      tmp = base, base = index, index = tmp;
      tmp = base_reg, base_reg = index_reg, index_reg = tmp;
    }

  /* Special case: %ebp cannot be encoded as a base without a displacement.  */
  if ((base_reg == hard_frame_pointer_rtx
       || base_reg == frame_pointer_rtx
       || base_reg == arg_pointer_rtx) && !disp)
    disp = const0_rtx;

  /* Special case: on K6, [%esi] makes the instruction vector decoded.
     Avoid this by transforming to [%esi+0].  */
  if (false)
    disp = const0_rtx;

  /* Special case: encode reg+reg instead of reg*2.  */
  if (!base && index && scale && scale == 2)
    base = index, base_reg = index_reg, scale = 1;

  /* Special case: scaling cannot be encoded without base or displacement.  */
  if (!base && !disp && index && scale != 1)
    disp = const0_rtx;

  out->base = base;
  out->index = index;
  out->disp = disp;
  out->scale = scale;
  out->seg = seg;

  return retval;
}

/* Determine if a given RTX is a valid constant.  We already know this
   satisfies CONSTANT_P.  */

bool
legitimate_constant_p (rtx x)
{
  switch (GET_CODE (x))
    {
    case CONST:
      x = XEXP (x, 0);

      if (GET_CODE (x) == PLUS)
	{
	  if (GET_CODE (XEXP (x, 1)) != CONST_INT)
	    return false;
	  x = XEXP (x, 0);
	}

      /* Only some unspecs are valid as "constants".  */
      if (GET_CODE (x) == UNSPEC)
	switch (XINT (x, 1))
	  {
	  case UNSPEC_GOTOFF:
	    return false;
	  case UNSPEC_TPOFF:
	  case UNSPEC_NTPOFF:
	    x = XVECEXP (x, 0, 0);
	    return false;
	  case UNSPEC_DTPOFF:
	    x = XVECEXP (x, 0, 0);
	    return false;
	  default:
	    return false;
	  }

      /* We must have drilled down to a symbol.  */
      if (GET_CODE (x) == LABEL_REF)
	return true;
      if (GET_CODE (x) != SYMBOL_REF)
	return false;
      /* FALLTHRU */

    case SYMBOL_REF:
      break;

    case PLUS:
      {
	rtx left = XEXP (x, 0);
	rtx right = XEXP (x, 1);
	bool left_is_constant = legitimate_constant_p (left);
	bool right_is_constant = legitimate_constant_p (right);
	return left_is_constant && right_is_constant;
      }
      break;
      /* APPLE LOCAL end dynamic-no-pic */

    case CONST_DOUBLE:
      if (GET_MODE (x) == TImode
	  && x != CONST0_RTX (TImode))
	return false;
      break;

    case CONST_VECTOR:
      return false;

    default:
      break;
    }

  /* Otherwise we handle everything else in the move patterns.  */
  return true;
}


/* GO_IF_LEGITIMATE_ADDRESS recognizes an RTL expression that is a valid
   memory address for an instruction.  The MODE argument is the machine mode
   for the MEM expression that wants to use this address.

   It only recognizes address in canonical form.  LEGITIMIZE_ADDRESS should
   convert common non-canonical forms to canonical form so that they will
   be recognized.  */

int
legitimate_address_p (enum machine_mode mode, rtx addr, int strict)
{
  struct avm2_address parts;
  rtx base, index, disp;
  HOST_WIDE_INT scale;
  const char *reason = NULL;
  rtx reason_rtx = NULL_RTX;

  if (avm2_decompose_address (addr, &parts) <= 0)
    {
      reason = "decomposition failed";
      goto report_error;
    }

  base = parts.base;
  index = parts.index;
  disp = parts.disp;
  scale = parts.scale;

  /* Validate base register.

     Don't allow SUBREG's that span more than a word here.  It can lead to spill
     failures when the base is one word out of a two word structure, which is
     represented internally as a DImode int.  */

  if (base)
    {
      rtx reg;
      reason_rtx = base;

      if (REG_P (base))
  	reg = base;
      else if (GET_CODE (base) == SUBREG
	       && REG_P (SUBREG_REG (base))
	       && GET_MODE_SIZE (GET_MODE (SUBREG_REG (base)))
		  <= UNITS_PER_WORD)
  	reg = SUBREG_REG (base);
      else
	{
	  reason = "base is not a register";
	  goto report_error;
	}

      if (GET_MODE (base) != Pmode)
	{
	  reason = "base is not in Pmode";
	  goto report_error;
	}

      if ((strict && ! REG_OK_FOR_BASE_STRICT_P (reg))
	  || (! strict && ! REG_OK_FOR_BASE_NONSTRICT_P (reg)))
	{
	  reason = "base is not valid";
	  goto report_error;
	}
    }

  /* Validate index register.

     Don't allow SUBREG's that span more than a word here -- same as above.  */

  if (index)
    {
      rtx reg;
      reason_rtx = index;

      if (REG_P (index))
  	reg = index;
      else if (GET_CODE (index) == SUBREG
	       && REG_P (SUBREG_REG (index))
	       && GET_MODE_SIZE (GET_MODE (SUBREG_REG (index)))
		  <= UNITS_PER_WORD)
  	reg = SUBREG_REG (index);
      else
	{
	  reason = "index is not a register";
	  goto report_error;
	}

      if (GET_MODE (index) != Pmode)
	{
	  reason = "index is not in Pmode";
	  goto report_error;
	}

      if ((strict && ! REG_OK_FOR_INDEX_STRICT_P (reg))
	  || (! strict && ! REG_OK_FOR_INDEX_NONSTRICT_P (reg)))
	{
	  reason = "index is not valid";
	  goto report_error;
	}
    }

  /* Validate scale factor.  */
  if (scale != 1)
    {
      reason_rtx = GEN_INT (scale);
      if (!index)
	{
	  reason = "scale without index";
	  goto report_error;
	}

      if (scale != 2 && scale != 4 && scale != 8)
	{
	  reason = "scale is not a valid multiplier";
	  goto report_error;
	}
    }

  /* Validate displacement.  */
  if (disp)
    {
      reason_rtx = disp;

      if (GET_CODE (disp) == CONST
	  && GET_CODE (XEXP (disp, 0)) == UNSPEC)
	switch (XINT (XEXP (disp, 0), 1))
	  {
	  default:
	    reason = "invalid address unspec";
	    goto report_error;
	  }

      else if (SYMBOLIC_CONST (disp)
	       && (flag_pic))
	{
	  /* APPLE LOCAL end dynamic-no-pic */

          /* This code used to verify that a symbolic pic displacement
	     includes the pic_offset_table_rtx register.

	     While this is good idea, unfortunately these constructs may
	     be created by "adds using lea" optimization for incorrect
	     code like:

	     int a;
	     int foo(int i)
	       {
	         return *(&a+i);
	       }

	     This code is nonsensical, but results in addressing
	     GOT table with pic_offset_table_rtx base.  We can't
	     just refuse it easily, since it gets matched by
	     "addsi3" pattern, that later gets split to lea in the
	     case output register differs from input.  While this
	     can be handled by separate addsi pattern for this case
	     that never results in lea, this seems to be easier and
	     correct fix for crash to disable this test.  */
	}
      else if (GET_CODE (disp) != LABEL_REF
	       && GET_CODE (disp) != CONST_INT
	       && (GET_CODE (disp) != CONST
		   || !legitimate_constant_p (disp))
	       && (GET_CODE (disp) != SYMBOL_REF
		   || !legitimate_constant_p (disp)))
	{
	  reason = "displacement is not constant";
	  goto report_error;
	}
    }
  return TRUE;

 report_error:
  return FALSE;
}

/* Value is the number of bytes of arguments automatically
   popped when returning from a subroutine call.
   FUNDECL is the declaration node of the function (as a tree),
   FUNTYPE is the data type of the function (as a tree),
   or for a library call it is an identifier node for the subroutine name.
   SIZE is the number of bytes of arguments passed on the stack.

   On the 80386, the RTD insn may be used to pop them if the number
     of args is fixed, but if the number is variable then the caller
     must pop them all.  RTD can't be used for library calls now
     because the library is compiled with the Unix compiler.
   Use of RTD is a selectable option, since it is incompatible with
   standard Unix calling sequences.  If the option is not selected,
   the caller must always pop the args.

   The attribute stdcall is equivalent to RTD on a per module basis.  */

int
avm2_return_pops_args (tree fundecl, tree funtype, int size)
{
  int rtd = TARGET_RTD && (!fundecl || TREE_CODE (fundecl) != IDENTIFIER_NODE);

  /* Cdecl functions override -mrtd, and never pop the stack.  */
  if (! lookup_attribute ("cdecl", TYPE_ATTRIBUTES (funtype))) {

    /* Stdcall and fastcall functions will pop the stack if not
       variable args.  */
    if (lookup_attribute ("stdcall", TYPE_ATTRIBUTES (funtype))
        || lookup_attribute ("fastcall", TYPE_ATTRIBUTES (funtype)))
      rtd = 1;

    if (rtd
        && (TYPE_ARG_TYPES (funtype) == NULL_TREE
            || (TREE_VALUE (tree_last (TYPE_ARG_TYPES (funtype)))
                == void_type_node)))
      return size;
  }

  /* Lose any fake structure return argument if it is passed on the stack.  */
  if (aggregate_value_p (TREE_TYPE (funtype), fundecl)
      && !KEEP_AGGREGATE_RETURN_POINTER)
    {
      int nregs = avm2_function_regparm (funtype, fundecl);

      if (!nregs)
        return GET_MODE_SIZE (Pmode);
    }

  return 0;
}

/* Return the regparm value for a function with the indicated TYPE and DECL.
   DECL may be NULL when calling function indirectly
   or considering a libcall.  */

static int
avm2_function_regparm (tree type, tree decl)
{
  tree attr;
  /* APPLE LOCAL begin MERGE FIXME audit to ensure that it's ok

     We had local_regparm but the FSF didn't and there didn't seem to
     be a merge conflict some something is strange. These seem to be just
     normal apple local changes.  I asked Stuart about them in email.  */
  int regparm = avm2_regparm;
  bool user_convention = false;

  if (1)
    {
      attr = lookup_attribute ("regparm", TYPE_ATTRIBUTES (type));
      if (attr)
	{
	  regparm = TREE_INT_CST_LOW (TREE_VALUE (TREE_VALUE (attr)));
	  user_convention = true;
	}

      /* APPLE LOCAL begin regparmandstackparm */
      if (lookup_attribute ("fastcall", TYPE_ATTRIBUTES (type))
	  || lookup_attribute ("regparmandstackparmee", TYPE_ATTRIBUTES (type)))
      /* APPLE LOCAL end regparmandstackparm */
	{
	  regparm = 2;
	  user_convention = true;
	}

      /* Use register calling convention for local functions when possible.  */
      if (!user_convention && decl
	  && flag_unit_at_a_time && !profile_flag)
	{
	  struct cgraph_local_info *i = cgraph_local_info (decl);
	  if (i && i->local)
	    {
	      int local_regparm, globals = 0, regno;

	      /* Make sure no regparm register is taken by a global register
		 variable.  */
	      for (local_regparm = 0; local_regparm < 3; local_regparm++)
		if (global_regs[local_regparm])
		  break;
	      /* We can't use regparm(3) for nested functions as these use
		 static chain pointer in third argument.  */
	      if (local_regparm == 3
                  /* APPLE LOCAL begin mainline */
		  && (decl_function_context (decl))
                  /* APPLE LOCAL end mainline */
		  && !DECL_NO_STATIC_CHAIN (decl))
		local_regparm = 2;
	      /* If the function realigns its stackpointer, the
		 prologue will clobber %ecx.  If we've already
		 generated code for the callee, the callee
		 DECL_STRUCT_FUNCTION is gone, so we fall back to
		 scanning the attributes for the self-realigning
		 property.  */
	      if (false)
		    local_regparm = 2;
	      /* Each global register variable increases register preassure,
		 so the more global reg vars there are, the smaller regparm
		 optimization use, unless requested by the user explicitly.  */
	      for (regno = 0; regno < 6; regno++)
		if (global_regs[regno])
		  globals++;
	      local_regparm
		= globals < local_regparm ? local_regparm - globals : 0;

	      if (local_regparm > regparm)
		regparm = local_regparm;
	    }
	}
    }
  /* APPLE LOCAL end MERGE FIXME audit to ensure that it's ok */
  return regparm;
}

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.  */

void
init_cumulative_args (CUMULATIVE_ARGS *cum,  /* Argument info to initialize */
		      tree fntype,	/* tree ptr for function decl */
		      rtx libname,	/* SYMBOL_REF of library name or 0 */
		      tree fndecl)
{
  static CUMULATIVE_ARGS zero_cum;
  tree param, next_param;

  *cum = zero_cum;

  /* Set up the number of registers to use for passing arguments.  */
  cum->nregs = avm2_regparm;
  cum->warn_sse = true;
  cum->warn_mmx = true;
  cum->maybe_vaarg = false;

  /* Use ecx and edx registers if function has fastcall attribute,
     else look for regparm information.  */
  if (fntype)
    {
      if (lookup_attribute ("fastcall", TYPE_ATTRIBUTES (fntype)))
	{
	  cum->nregs = 2;
	  cum->fastcall = 1;
	}
      else
	cum->nregs = avm2_function_regparm (fntype, fndecl);
    }

  /* Set up the number of SSE registers used for passing SFmode
     and DFmode arguments.  Warn for mismatching ABI.  */
  cum->float_in_sse = 0;

  /* Determine if this function has variable arguments.  This is
     indicated by the last argument being 'void_type_mode' if there
     are no variable arguments.  If there are variable arguments, then
     we won't pass anything in registers in 32-bit mode. */

  if (cum->nregs || cum->mmx_nregs || cum->sse_nregs)
    {
      for (param = (fntype) ? TYPE_ARG_TYPES (fntype) : 0;
	   param != 0; param = next_param)
	{
	  next_param = TREE_CHAIN (param);
	  if (next_param == 0 && TREE_VALUE (param) != void_type_node)
	    {
	      if (1)
		{
		  cum->nregs = 0;
		  cum->sse_nregs = 0;
		  cum->mmx_nregs = 0;
		  cum->warn_sse = 0;
		  cum->warn_mmx = 0;
		  cum->fastcall = 0;
		  cum->float_in_sse = 0;
		}
	      cum->maybe_vaarg = true;
	    }
	}
    }
  if ((!fntype && !libname)
      || (fntype && !TYPE_ARG_TYPES (fntype)))
    cum->maybe_vaarg = true;

  return;
}

/* Return the "natural" mode for TYPE.  In most cases, this is just TYPE_MODE.
   But in the case of vector types, it is some vector mode.

   When we have only some of our vector isa extensions enabled, then there
   are some modes for which vector_mode_supported_p is false.  For these
   modes, the generic vector support in gcc will choose some non-vector mode
   in order to implement the type.  By computing the natural mode, we'll
   select the proper ABI location for the operand and not depend on whatever
   the middle-end decides to do with these vector types.  */

static enum machine_mode
type_natural_mode (tree type)
{
  enum machine_mode mode = TYPE_MODE (type);

  if (TREE_CODE (type) == VECTOR_TYPE && !VECTOR_MODE_P (mode))
    {
      HOST_WIDE_INT size = int_size_in_bytes (type);
      if ((size == 8 || size == 16)
	  /* ??? Generic code allows us to create width 1 vectors.  Ignore.  */
	  && TYPE_VECTOR_SUBPARTS (type) > 1)
	{
	  enum machine_mode innermode = TYPE_MODE (TREE_TYPE (type));

	  if (TREE_CODE (TREE_TYPE (type)) == REAL_TYPE)
	    mode = MIN_MODE_VECTOR_FLOAT;
	  else
	    mode = MIN_MODE_VECTOR_INT;

	  /* Get the mode which has this inner mode and number of units.  */
	  for (; mode != VOIDmode; mode = GET_MODE_WIDER_MODE (mode))
	    if (GET_MODE_NUNITS (mode) == TYPE_VECTOR_SUBPARTS (type)
		&& GET_MODE_INNER (mode) == innermode)
	      return mode;

	  gcc_unreachable ();
	}
    }
  return mode;
}

/* Define where to put the arguments to a function.
   Value is zero to push the argument on the stack,
   or a hard register in which to store the argument.

   MODE is the argument's machine mode.
   TYPE is the data type of the argument (as a tree).
    This is null for libcalls where that information may
    not be available.
   CUM is a variable of type CUMULATIVE_ARGS which gives info about
    the preceding args and about the function being called.
   NAMED is nonzero if this argument is a named parameter
    (otherwise it is an extra parameter matching an ellipsis).  */

rtx
function_arg (CUMULATIVE_ARGS *cum, enum machine_mode orig_mode,
	      tree type, int named)
{
  enum machine_mode mode = orig_mode;
  rtx ret = NULL_RTX;
  int bytes =
    (mode == BLKmode) ? int_size_in_bytes (type) : (int) GET_MODE_SIZE (mode);
  int words = (bytes + UNITS_PER_WORD - 1) / UNITS_PER_WORD;

  /* To simplify the code below, represent vector types with a vector mode
     even if MMX/SSE are not active.  */
  if (type && TREE_CODE (type) == VECTOR_TYPE)
    mode = type_natural_mode (type);

  /* Handle a hidden AL argument containing number of registers for varargs
     x86-64 functions.  For i386 ABI just return constm1_rtx to avoid
     any AL settings.  */
  if (mode == VOIDmode)
    {
	return constm1_rtx;
    }
    switch (mode)
      {
	/* For now, pass fp/complex values on the stack.  */
      default:
	break;

      case BLKmode:
	if (bytes < 0)
	  break;
	/* FALLTHRU */
      case DImode:
      case SImode:
      case HImode:
      case QImode:
	if (words <= cum->nregs)
	  {
	    int regno = cum->regno;

	    /* Fastcall allocates the first two DWORD (SImode) or
	       smaller arguments to ECX and EDX.  */
	    if (cum->fastcall)
	      {
	        if (mode == BLKmode || mode == DImode)
	          break;

	        /* ECX not EAX is the first allocated register.  */
	        if (regno == 0)
		  regno = 2;
	      }
	    ret = gen_rtx_REG (mode, regno);
	  }
	break;
      case DFmode:
	  break;
      case SFmode:
	
	  break;
	/* FALLTHRU */
      case TImode:
	if (!type || !AGGREGATE_TYPE_P (type))
	  {
	    
	  }
	break;
      }

  
  return ret;
}

/* Returns 1 if OP contains a symbol reference */

int
symbolic_reference_mentioned_p (rtx op)
{
  const char *fmt;
  int i;

  if (GET_CODE (op) == SYMBOL_REF || GET_CODE (op) == LABEL_REF)
    return 1;

  fmt = GET_RTX_FORMAT (GET_CODE (op));
  for (i = GET_RTX_LENGTH (GET_CODE (op)) - 1; i >= 0; i--)
    {
      if (fmt[i] == 'E')
	{
	  int j;

	  for (j = XVECLEN (op, i) - 1; j >= 0; j--)
	    if (symbolic_reference_mentioned_p (XVECEXP (op, i, j)))
	      return 1;
	}

      else if (fmt[i] == 'e' && symbolic_reference_mentioned_p (XEXP (op, i)))
	return 1;
    }

  return 0;
}

/* LLVM LOCAL end - AVM2 target */
