/* Functions for generic Darwin as target machine for GNU C compiler.
   Copyright (C) 1989, 1990, 1991, 1992, 1993, 2000, 2001, 2002, 2003, 2004,
   2005
   Free Software Foundation, Inc.
   Contributed by Apple Computer Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

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
#include "insn-flags.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "tree.h"
#include "expr.h"
#include "reload.h"
#include "function.h"
#include "ggc.h"
#include "langhooks.h"
#include "target.h"
#include "tm_p.h"
#include "toplev.h"
#include "hashtab.h"
/* APPLE LOCAL begin constant cfstrings */
#include "toplev.h"

static tree darwin_build_constant_cfstring (tree);

enum darwin_builtins
{
  DARWIN_BUILTIN_MIN = (int)END_BUILTINS,

  DARWIN_BUILTIN_CFSTRINGMAKECONSTANTSTRING,
  DARWIN_BUILTIN_MAX
};
/* APPLE LOCAL end constant cfstrings */

/* Darwin supports a feature called fix-and-continue, which is used
   for rapid turn around debugging.  When code is compiled with the
   -mfix-and-continue flag, two changes are made to the generated code
   that allow the system to do things that it would normally not be
   able to do easily.  These changes allow gdb to load in
   recompilation of a translation unit that has been changed into a
   running program and replace existing functions and methods of that
   translation unit with versions of those functions and methods
   from the newly compiled translation unit.  The new functions access
   the existing static symbols from the old translation unit, if the
   symbol existed in the unit to be replaced, and from the new
   translation unit, otherwise.

   The changes are to insert 5 nops at the beginning of all functions
   and to use indirection to get at static symbols.  The 5 nops
   are required by consumers of the generated code.  Currently, gdb
   uses this to patch in a jump to the overriding function, this
   allows all uses of the old name to forward to the replacement,
   including existing function pointers and virtual methods.  See
   rs6000_emit_prologue for the code that handles the nop insertions.

   The added indirection allows gdb to redirect accesses to static
   symbols from the newly loaded translation unit to the existing
   symbol, if any.  @code{static} symbols are special and are handled by
   setting the second word in the .non_lazy_symbol_pointer data
   structure to symbol.  See indirect_data for the code that handles
   the extra indirection, and machopic_output_indirection and its use
   of MACHO_SYMBOL_STATIC for the code that handles @code{static}
   symbol indirection.  */

/* APPLE LOCAL begin pragma reverse_bitfields */
/* Shouldn't there be a comment here?  */
int darwin_reverse_bitfields = 0;
/* APPLE LOCAL end pragma reverse_bitfields */

/* APPLE LOCAL begin axe stubs 5571540 */
int darwin_stubs = false;
/* APPLE LOCAL end axe stubs 5571540 */

/* Section names.  */
section * darwin_sections[NUM_DARWIN_SECTIONS];

/* True if we're setting __attribute__ ((ms_struct)).  */
int darwin_ms_struct = false;

/* A get_unnamed_section callback used to switch to an ObjC section.
   DIRECTIVE is as for output_section_asm_op.  */

static void
output_objc_section_asm_op (const void *directive)
{
  static bool been_here = false;

  if (! been_here)
    {
      static const enum darwin_section_enum tomark[] =
	{
	  /* written, cold -> hot */
	  objc_cat_cls_meth_section,
	  objc_cat_inst_meth_section,
	  objc_string_object_section,
	  objc_constant_string_object_section,
	  objc_selector_refs_section,
	  objc_selector_fixup_section,
	  objc_cls_refs_section,
	  objc_class_section,
	  objc_meta_class_section,
	  /* shared, hot -> cold */
	  objc_cls_meth_section,
	  objc_inst_meth_section,
	  objc_protocol_section,
	  objc_class_names_section,
	  objc_meth_var_types_section,
	  objc_meth_var_names_section,
	  objc_category_section,
	  objc_class_vars_section,
	  objc_instance_vars_section,
	  objc_module_info_section,
	  /* APPLE LOCAL begin objc2 */
	  objc_symbols_section,
	  objc_protocol_ext_section,
	  objc_class_ext_section,
	  objc_prop_list_section
	  /* APPLE LOCAL end objc2 */
	};
      /* APPLE LOCAL begin ObjC abi v2 - radar 4792158 */
      static const enum darwin_section_enum tomarkv2[] =
	{
	  objc_v2_message_refs_section,
	  /* APPLE LOCAL begin radar 6255595 */
	  objc_v2_classdefs_section,
	  objc_v2_metadata_section,
	  /* APPLE LOCAL end radar 6255595 */
	  objc_v2_classrefs_section,
	  objc_v2_classlist_section,
	  objc_v2_categorylist_section,
	  objc_v2_selector_refs_section,
	  objc_v2_nonlazy_class_section,
	  objc_v2_nonlazy_category_section,
	  objc_v2_protocollist_section,
	  objc_v2_protocolrefs_section,
	  objc_v2_super_classrefs_section,
	  objc_v2_image_info_section,
	  objc_v2_constant_string_object_section
	} ;
      /* APPLE LOCAL end ObjC abi v2 - radar 4792158 */
      size_t i;

      been_here = true;
      /* APPLE LOCAL begin radar 4792158 */
      /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
      if (flag_objc_abi == 1)
        for (i = 0; i < ARRAY_SIZE (tomark); i++)
          switch_to_section (darwin_sections[tomark[i]]);
      /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */ \
      else if (flag_objc_abi == 2)
	for (i = 0; i < ARRAY_SIZE (tomarkv2); i++)
	  switch_to_section (darwin_sections[tomarkv2[i]]);
      /* APPLE LOCAL end radar 4792158 */
    }
  output_section_asm_op (directive);
}

/* Implement TARGET_ASM_INIT_SECTIONS.  */

void
darwin_init_sections (void)
{
#define DEF_SECTION(NAME, FLAGS, DIRECTIVE, OBJC)		\
  darwin_sections[NAME] =					\
    get_unnamed_section (FLAGS, (OBJC				\
				 ? output_objc_section_asm_op	\
				 : output_section_asm_op),	\
			 "\t" DIRECTIVE);
#include "config/darwin-sections.def"
#undef DEF_SECTION

  readonly_data_section = darwin_sections[const_section];
  exception_section = darwin_sections[darwin_exception_section];
  eh_frame_section = darwin_sections[darwin_eh_frame_section];
}

int
name_needs_quotes (const char *name)
{
  int c;
  while ((c = *name++) != '\0')
    if (! ISIDNUM (c) && c != '.' && c != '$')
      return 1;
  return 0;
}

/* Return true if SYM_REF can be used without an indirection.  */
/* APPLE LOCAL dynamic-no-pic */
int
machopic_symbol_defined_p (rtx sym_ref)
{
  if (SYMBOL_REF_FLAGS (sym_ref) & MACHO_SYMBOL_FLAG_DEFINED)
    return true;

  /* If a symbol references local and is not an extern to this
     file, then the symbol might be able to declared as defined.  */
  if (SYMBOL_REF_LOCAL_P (sym_ref) && ! SYMBOL_REF_EXTERNAL_P (sym_ref))
    {
      /* If the symbol references a variable and the variable is a
	 common symbol, then this symbol is not defined.  */
      if (SYMBOL_REF_FLAGS (sym_ref) & MACHO_SYMBOL_FLAG_VARIABLE)
	{
	  tree decl = SYMBOL_REF_DECL (sym_ref);
	  if (!decl)
	    return true;
	  if (DECL_COMMON (decl))
	    return false;
	}
      /* APPLE LOCAL begin 6077274 */
      /* Weak functions should always be indirected.  */
      else if (SYMBOL_REF_FLAGS (sym_ref) & SYMBOL_FLAG_FUNCTION)
	{
	  tree decl = SYMBOL_REF_DECL (sym_ref);
	  if (decl && DECL_WEAK (decl))
	    return false;
	}
      /* APPLE LOCAL end 6077274 */
      return true;
    }
  return false;
}

/* This module assumes that (const (symbol_ref "foo")) is a legal pic
   reference, which will not be changed.  */

enum machopic_addr_class
machopic_classify_symbol (rtx sym_ref)
{
  int flags;
  bool function_p;

  flags = SYMBOL_REF_FLAGS (sym_ref);
  function_p = SYMBOL_REF_FUNCTION_P (sym_ref);
  if (machopic_symbol_defined_p (sym_ref))
    return (function_p
	    ? MACHOPIC_DEFINED_FUNCTION : MACHOPIC_DEFINED_DATA);
  else
    return (function_p
	    ? MACHOPIC_UNDEFINED_FUNCTION : MACHOPIC_UNDEFINED_DATA);
}

#ifndef TARGET_FIX_AND_CONTINUE
#define TARGET_FIX_AND_CONTINUE 0
#endif

/* Indicate when fix-and-continue style code generation is being used
   and when a reference to data should be indirected so that it can be
   rebound in a new translation unit to reference the original instance
   of that data.  Symbol names that are for code generation local to
   the translation unit are bound to the new translation unit;
   currently this means symbols that begin with L or _OBJC_;
   otherwise, we indicate that an indirect reference should be made to
   permit the runtime to rebind new instances of the translation unit
   to the original instance of the data.  */

/* APPLE LOCAL fix-and-continue 6227434 */
int
indirect_data (rtx sym_ref)
{
  int lprefix;
  const char *name;

  /* If we aren't generating fix-and-continue code, don't do anything special.  */
  if (TARGET_FIX_AND_CONTINUE == 0)
    return 0;

  /* Otherwise, all symbol except symbols that begin with L or _OBJC_
     are indirected.  Symbols that begin with L and _OBJC_ are always
     bound to the current translation unit as they are used for
     generated local data of the translation unit.  */

  name = XSTR (sym_ref, 0);

  lprefix = (((name[0] == '*' || name[0] == '&')
	      /* APPLE LOCAL begin fix-and-continue 6227434 */
              && (name[1] == 'L'
		  || (name[1] == '"' && name[2] == 'L')
		  /* Don't indirect writable strings.  */
		  || (name[1] == 'l' && name[2] == 'C')))
             || (strncmp (name, "_OBJC_", 6) == 0)
	     || objc_anonymous_local_objc_name (name));
	      /* APPLE LOCAL end fix-and-continue 6227434 */

  return ! lprefix;
}


/* APPLE LOCAL ARM pic support */
int
machopic_data_defined_p (rtx sym_ref)
{
  if (indirect_data (sym_ref))
    return 0;

  switch (machopic_classify_symbol (sym_ref))
    {
    case MACHOPIC_DEFINED_DATA:
    case MACHOPIC_DEFINED_FUNCTION:
      return 1;
    default:
      return 0;
    }
}

void
machopic_define_symbol (rtx mem)
{
  rtx sym_ref;

  gcc_assert (GET_CODE (mem) == MEM);
  sym_ref = XEXP (mem, 0);
  SYMBOL_REF_FLAGS (sym_ref) |= MACHO_SYMBOL_FLAG_DEFINED;
}

static GTY(()) char * function_base;

const char *
machopic_function_base_name (void)
{
  /* if dynamic-no-pic is on, we should not get here */
  gcc_assert (!MACHO_DYNAMIC_NO_PIC_P);

  if (function_base == NULL)
    function_base =
      (char *) ggc_alloc_string ("<pic base>", sizeof ("<pic base>"));

  current_function_uses_pic_offset_table = 1;

  return function_base;
}

/* Return a SYMBOL_REF for the PIC function base.  */

rtx
machopic_function_base_sym (void)
{
  rtx sym_ref;

  sym_ref = gen_rtx_SYMBOL_REF (Pmode, machopic_function_base_name ());
  SYMBOL_REF_FLAGS (sym_ref)
    |= (MACHO_SYMBOL_FLAG_VARIABLE | MACHO_SYMBOL_FLAG_DEFINED);
  return sym_ref;
}

/* Return either ORIG or (const:P (minus:P ORIG PIC_BASE)), depending
   on whether pic_base is NULL or not.  */
static inline rtx
gen_pic_offset (rtx orig, rtx pic_base)
{
  if (!pic_base)
    return orig;
  else
    return gen_rtx_CONST (Pmode, gen_rtx_MINUS (Pmode, orig, pic_base));
}

static GTY(()) const char * function_base_func_name;
static GTY(()) int current_pic_label_num;

void
machopic_output_function_base_name (FILE *file)
{
  const char *current_name;

  /* If dynamic-no-pic is on, we should not get here.  */
  gcc_assert (!MACHO_DYNAMIC_NO_PIC_P);
  current_name =
    IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (current_function_decl));
  if (function_base_func_name != current_name)
    {
      ++current_pic_label_num;
      function_base_func_name = current_name;
    }
  fprintf (file, "\"L%011d$pb\"", current_pic_label_num);
}

/* The suffix attached to non-lazy pointer symbols.  */
#define NON_LAZY_POINTER_SUFFIX "$non_lazy_ptr"
/* The suffix attached to stub symbols.  */
#define STUB_SUFFIX "$stub"

typedef struct machopic_indirection GTY (())
{
  /* The SYMBOL_REF for the entity referenced.  */
  rtx symbol;
  /* The name of the stub or non-lazy pointer.  */
  const char * ptr_name;
  /* True iff this entry is for a stub (as opposed to a non-lazy
     pointer).  */
  bool stub_p;
  /* True iff this stub or pointer pointer has been referenced.  */
  bool used;
  /* APPLE LOCAL begin ARM 5440570 */
  /* True iff this stub or pointer pointer has been outputted.  */
  bool emitted;
  /* APPLE LOCAL end ARM 5440570 */
} machopic_indirection;

/* A table mapping stub names and non-lazy pointer names to
   SYMBOL_REFs for the stubbed-to and pointed-to entities.  */

static GTY ((param_is (struct machopic_indirection))) htab_t
  machopic_indirections;

/* APPLE LOCAL begin 5440570 */
/* Used to identify that usage information has changed while
   outputting the stubs.  */
static GTY (()) bool indirection_uses_changed;
/* APPLE LOCAL end 5440570 */

/* Return a hash value for a SLOT in the indirections hash table.  */

static hashval_t
machopic_indirection_hash (const void *slot)
{
  const machopic_indirection *p = (const machopic_indirection *) slot;
  return htab_hash_string (p->ptr_name);
}

/* Returns true if the KEY is the same as that associated with
   SLOT.  */

static int
machopic_indirection_eq (const void *slot, const void *key)
{
  return strcmp (((const machopic_indirection *) slot)->ptr_name, key) == 0;
}

/* Return the name of the non-lazy pointer (if STUB_P is false) or
   stub (if STUB_B is true) corresponding to the given name.  */

const char *
machopic_indirection_name (rtx sym_ref, bool stub_p)
{
  char *buffer;
  const char *name = XSTR (sym_ref, 0);
  size_t namelen = strlen (name);
  machopic_indirection *p;
  void ** slot;
  bool saw_star = false;
  bool needs_quotes;
  const char *suffix;
  const char *prefix = user_label_prefix;
  const char *quote = "";
  tree id;

  id = maybe_get_identifier (name);
  if (id)
    {
      tree id_orig = id;

      while (IDENTIFIER_TRANSPARENT_ALIAS (id))
	id = TREE_CHAIN (id);
      if (id != id_orig)
	{
	  name = IDENTIFIER_POINTER (id);
	  namelen = strlen (name);
	}
    }

  if (name[0] == '*')
    {
      saw_star = true;
      prefix = "";
      ++name;
      --namelen;
    }

  needs_quotes = name_needs_quotes (name);
  if (needs_quotes)
    {
      quote = "\"";
    }

  if (stub_p)
    suffix = STUB_SUFFIX;
  else
    suffix = NON_LAZY_POINTER_SUFFIX;

  buffer = alloca (strlen ("&L")
		   + strlen (prefix)
		   + namelen
		   + strlen (suffix)
		   + 2 * strlen (quote)
		   + 1 /* '\0' */);

  /* Construct the name of the non-lazy pointer or stub.  */
  sprintf (buffer, "&%sL%s%s%s%s", quote, prefix, name, suffix, quote);

  if (!machopic_indirections)
    machopic_indirections = htab_create_ggc (37,
					     machopic_indirection_hash,
					     machopic_indirection_eq,
					     /*htab_del=*/NULL);

  slot = htab_find_slot_with_hash (machopic_indirections, buffer,
				   htab_hash_string (buffer), INSERT);
  if (*slot)
    {
      p = (machopic_indirection *) *slot;
    }
  else
    {
      p = (machopic_indirection *) ggc_alloc (sizeof (machopic_indirection));
      p->symbol = sym_ref;
      p->ptr_name = xstrdup (buffer);
      p->stub_p = stub_p;
      p->used = false;
      /* APPLE LOCAL ARM 5440570 */
      p->emitted = false;
      *slot = p;
    }

  return p->ptr_name;
}

/* Return the name of the stub for the mcount function.  */

const char*
machopic_mcount_stub_name (void)
{
  rtx symbol = gen_rtx_SYMBOL_REF (Pmode, "*mcount");
  return machopic_indirection_name (symbol, /*stub_p=*/true);
}

/* APPLE LOCAL begin ARM pic support */
/* Determine whether the specified symbol is in the indirections table.  */
int
machopic_lookup_stub_or_non_lazy_ptr (const char *name)
{
  machopic_indirection *p;

  if (! machopic_indirections)
    return 0;

  p = (machopic_indirection *)
       htab_find_with_hash (machopic_indirections, name,
			    htab_hash_string (name));
  if (p)
    return 1;
  else
    return 0;
}
/* APPLE LOCAL end ARM pic support */

/* If NAME is the name of a stub or a non-lazy pointer , mark the stub
   or non-lazy pointer as used -- and mark the object to which the
   pointer/stub refers as used as well, since the pointer/stub will
   emit a reference to it.  */

void
machopic_validate_stub_or_non_lazy_ptr (const char *name)
{
  machopic_indirection *p;

  p = ((machopic_indirection *)
       (htab_find_with_hash (machopic_indirections, name,
			     htab_hash_string (name))));
  if (p && ! p->used)
    {
      const char *real_name;
      tree id;

      p->used = true;
      /* APPLE LOCAL ARM 5440570 */
      indirection_uses_changed = true;

      /* Do what output_addr_const will do when we actually call it.  */
      if (SYMBOL_REF_DECL (p->symbol))
	mark_decl_referenced (SYMBOL_REF_DECL (p->symbol));

      real_name = targetm.strip_name_encoding (XSTR (p->symbol, 0));

      id = maybe_get_identifier (real_name);
      if (id)
	mark_referenced (id);
    }
}

/* Transform ORIG, which may be any data source, to the corresponding
   source using indirections.  */

rtx
machopic_indirect_data_reference (rtx orig, rtx reg)
{
  rtx ptr_ref = orig;

  if (! MACHOPIC_INDIRECT)
    return orig;

  /* APPLE LOCAL begin dynamic-no-pic  */
  switch (GET_CODE (orig))
    {
    case SYMBOL_REF:
      {
	int defined = machopic_data_defined_p (orig);

      if (defined && MACHO_DYNAMIC_NO_PIC_P)
	{
#if defined (TARGET_TOC)
	  /* Create a new register for CSE opportunities.  */
	  rtx hi_reg = (no_new_pseudos ? reg : gen_reg_rtx (Pmode));
 	  emit_insn (gen_macho_high (hi_reg, orig));
 	  emit_insn (gen_macho_low (reg, hi_reg, orig));
#else
#if defined (TARGET_386)
	    return orig;
#else /* defined (TARGET_386) */
	   /* some other cpu -- writeme!  */
	   gcc_unreachable ();
#endif /* defined (TARGET_386) */
#endif
	   return reg;
	}
      else if (defined)
	{
#if defined (TARGET_TOC) || defined (HAVE_lo_sum)
	  rtx pic_base = machopic_function_base_sym ();
	  rtx offset = gen_pic_offset (orig, pic_base);
#endif

#if defined (TARGET_TOC) /* i.e., PowerPC */
	  rtx hi_sum_reg = (no_new_pseudos ? reg : gen_reg_rtx (Pmode));

	  gcc_assert (reg);

	  emit_insn (gen_rtx_SET (Pmode, hi_sum_reg,
			      gen_rtx_PLUS (Pmode, pic_offset_table_rtx,
				       gen_rtx_HIGH (Pmode, offset))));
	  emit_insn (gen_rtx_SET (Pmode, reg,
				  gen_rtx_LO_SUM (Pmode, hi_sum_reg, offset)));

	  orig = reg;
#else
#if defined (HAVE_lo_sum)
	  gcc_assert (reg);

	  emit_insn (gen_rtx_SET (VOIDmode, reg,
				  gen_rtx_HIGH (Pmode, offset)));
	  emit_insn (gen_rtx_SET (VOIDmode, reg,
				  gen_rtx_LO_SUM (Pmode, reg, offset)));
	  emit_insn (gen_rtx_USE (VOIDmode, pic_offset_table_rtx));

	  orig = gen_rtx_PLUS (Pmode, pic_offset_table_rtx, reg);
#endif
#endif
	  return orig;
	}

      ptr_ref = (gen_rtx_SYMBOL_REF
		 (Pmode,
		  machopic_indirection_name (orig, /*stub_p=*/false)));

      SYMBOL_REF_DATA (ptr_ref) = SYMBOL_REF_DATA (orig);

      ptr_ref = gen_const_mem (Pmode, ptr_ref);
      machopic_define_symbol (ptr_ref);

#ifdef TARGET_386
	if (reg && MACHO_DYNAMIC_NO_PIC_P)
	  {
	    emit_insn (gen_rtx_SET (Pmode, reg, ptr_ref));
	    ptr_ref = reg;
	  }
#endif	/* TARGET_386 */

      return ptr_ref;
    }
      break;
  
    case CONST:
      {
	/* If "(const (plus ...", walk the PLUS and return that result.
	   PLUS processing (below) will restore the "(const ..." if
	   appropriate.  */
	if (GET_CODE (XEXP (orig, 0)) == PLUS)
	  return machopic_indirect_data_reference (XEXP (orig, 0), reg);
	else 
	  return orig;
      }
      break;
  
    case MEM:
      {
	XEXP (ptr_ref, 0) = machopic_indirect_data_reference (XEXP (orig, 0), reg);
	return ptr_ref;
      }
      break;
  
    case PLUS:
      {
	rtx base, result;

	/* When the target is i386, this code prevents crashes due to the
	   compiler's ignorance on how to move the PIC base register to
	   other registers.  (The reload phase sometimes introduces such
	   insns.)  */
	if (GET_CODE (XEXP (orig, 0)) == REG
	    && REGNO (XEXP (orig, 0)) == PIC_OFFSET_TABLE_REGNUM
#ifdef TARGET_386
	    /* Prevent the same register from being erroneously used
	       as both the base and index registers.  */
	    && GET_CODE (XEXP (orig, 1)) == CONST
#endif
	    && reg)
	  {
	    emit_move_insn (reg, XEXP (orig, 0));
	    XEXP (ptr_ref, 0) = reg;
	    return ptr_ref;
	  }

	/* Legitimize both operands of the PLUS.  */
	base = machopic_indirect_data_reference (XEXP (orig, 0), reg);
	orig = machopic_indirect_data_reference (XEXP (orig, 1),
						 (base == reg ? 0 : reg));
	if (MACHOPIC_INDIRECT && GET_CODE (orig) == CONST_INT)
	  result = plus_constant (base, INTVAL (orig));
	else
	  result = gen_rtx_PLUS (Pmode, base, orig);

	if (MACHOPIC_JUST_INDIRECT && GET_CODE (base) == MEM)
	  {
	    if (reg)
	      {
		emit_move_insn (reg, result);
		result = reg;
	      }
	    else
	      result = force_reg (GET_MODE (result), result);
	  }
	return result;
      }
      break;

    default:
      break;
    }	/* End switch (GET_CODE (orig)) */
  /* APPLE LOCAL end dynamic-no-pic */
  return ptr_ref;
}

/* APPLE LOCAL begin 4380289 */
/* Force a Mach-O stub.  Expects MEM(SYM_REF(foo)).  No sanity
   checking.  */
static inline rtx
machopic_force_stub (rtx target)
{
  rtx sym_ref = XEXP (target, 0);
  rtx new_target;
  enum machine_mode mem_mode = GET_MODE (target);
  enum machine_mode sym_mode = GET_MODE (XEXP (target, 0));
  const char *stub_name = XSTR (sym_ref, 0);

  stub_name = machopic_indirection_name (sym_ref, /*stub_p=*/true);

  new_target = gen_rtx_MEM (mem_mode, gen_rtx_SYMBOL_REF (sym_mode, stub_name));
  SYMBOL_REF_DATA (XEXP (new_target, 0)) = SYMBOL_REF_DATA (sym_ref);
  MEM_READONLY_P (new_target) = 1;
  MEM_NOTRAP_P (new_target) = 1;
  return new_target;
}

/* Like machopic_indirect_call_target, but always stubify,
   and don't re-stubify anything already stubified.  */
rtx
machopic_force_indirect_call_target (rtx target)
{
  if (MEM_P (target))
  {
    rtx sym_ref = XEXP (target, 0);
    const char *stub_name = XSTR (sym_ref, 0);
    unsigned int stub_name_length = strlen (stub_name);
      
    /* If "$stub" suffix absent, add it.  */
    if (stub_name_length < 6 || strcmp ("$stub", stub_name + stub_name_length - 5))
      target = machopic_force_stub (target);
  }

  return target;
}
/* APPLE LOCAL end 4380289 */

/* Transform TARGET (a MEM), which is a function call target, to the
   corresponding symbol_stub if necessary.  Return a new MEM.  */

rtx
machopic_indirect_call_target (rtx target)
{
  /* APPLE LOCAL begin axe stubs 5571540 */
  if (! darwin_stubs)
    return target;
  /* APPLE LOCAL end axe stubs 5571540 */

  if (GET_CODE (target) != MEM)
    return target;

  if (MACHOPIC_INDIRECT
      && GET_CODE (XEXP (target, 0)) == SYMBOL_REF
      && !(SYMBOL_REF_FLAGS (XEXP (target, 0))
	   & MACHO_SYMBOL_FLAG_DEFINED))
    /* APPLE LOCAL begin 4380289 */
    target = machopic_force_stub (target);
    /* APPLE LOCAL end 4380289 */

  return target;
}

rtx
machopic_legitimize_pic_address (rtx orig, enum machine_mode mode, rtx reg)
{
  rtx pic_ref = orig;

  if (! MACHOPIC_INDIRECT)
    return orig;

  /* First handle a simple SYMBOL_REF or LABEL_REF */
  if (GET_CODE (orig) == LABEL_REF
      || (GET_CODE (orig) == SYMBOL_REF
	  ))
    {
      /* addr(foo) = &func+(foo-func) */
      rtx pic_base;

      orig = machopic_indirect_data_reference (orig, reg);

      if (GET_CODE (orig) == PLUS
	  && GET_CODE (XEXP (orig, 0)) == REG)
	{
	  if (reg == 0)
	    return force_reg (mode, orig);

	  emit_move_insn (reg, orig);
	  return reg;
	}

      /* if dynamic-no-pic we don't have a pic base  */
      if (MACHO_DYNAMIC_NO_PIC_P)
	pic_base = NULL;
      else
	pic_base = machopic_function_base_sym ();

      if (GET_CODE (orig) == MEM)
	{
	  if (reg == 0)
	    {
	      gcc_assert (!reload_in_progress);
	      reg = gen_reg_rtx (Pmode);
	    }

#ifdef HAVE_lo_sum
	  if (MACHO_DYNAMIC_NO_PIC_P
	      && (GET_CODE (XEXP (orig, 0)) == SYMBOL_REF
		  || GET_CODE (XEXP (orig, 0)) == LABEL_REF))
	    {
#if defined (TARGET_TOC)	/* ppc  */
	      rtx temp_reg = (no_new_pseudos) ? reg : gen_reg_rtx (Pmode);
	      rtx asym = XEXP (orig, 0);
	      rtx mem;

	      emit_insn (gen_macho_high (temp_reg, asym));
	      mem = gen_const_mem (GET_MODE (orig),
				   gen_rtx_LO_SUM (Pmode, temp_reg, asym));
	      emit_insn (gen_rtx_SET (VOIDmode, reg, mem));
#else
	      /* Some other CPU -- WriteMe! but right now there are no other platform that can use dynamic-no-pic  */
	      gcc_unreachable ();
#endif
	      pic_ref = reg;
	    }
	  else
	  if (GET_CODE (XEXP (orig, 0)) == SYMBOL_REF
	      || GET_CODE (XEXP (orig, 0)) == LABEL_REF)
	    {
	      rtx offset = gen_pic_offset (XEXP (orig, 0), pic_base);
#if defined (TARGET_TOC) /* i.e., PowerPC */
	      /* Generating a new reg may expose opportunities for
		 common subexpression elimination.  */
              rtx hi_sum_reg = no_new_pseudos ? reg : gen_reg_rtx (Pmode);
	      rtx mem;
	      rtx insn;
	      rtx sum;

	      sum = gen_rtx_HIGH (Pmode, offset);
	      if (! MACHO_DYNAMIC_NO_PIC_P)
		sum = gen_rtx_PLUS (Pmode, pic_offset_table_rtx, sum);

	      emit_insn (gen_rtx_SET (Pmode, hi_sum_reg, sum));

	      mem = gen_const_mem (GET_MODE (orig),
				  gen_rtx_LO_SUM (Pmode,
						  hi_sum_reg, offset));
	      insn = emit_insn (gen_rtx_SET (VOIDmode, reg, mem));
	      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_EQUAL, pic_ref,
						    REG_NOTES (insn));

	      pic_ref = reg;
#else
	      emit_insn (gen_rtx_USE (VOIDmode,
				      gen_rtx_REG (Pmode,
						   PIC_OFFSET_TABLE_REGNUM)));

	      emit_insn (gen_rtx_SET (VOIDmode, reg,
				      gen_rtx_HIGH (Pmode,
						    gen_rtx_CONST (Pmode,
								   offset))));
	      emit_insn (gen_rtx_SET (VOIDmode, reg,
				  gen_rtx_LO_SUM (Pmode, reg,
					   gen_rtx_CONST (Pmode, offset))));
	      pic_ref = gen_rtx_PLUS (Pmode,
				      pic_offset_table_rtx, reg);
#endif
	    }
	  else
#endif  /* HAVE_lo_sum */
	    {
	      rtx pic = pic_offset_table_rtx;
	      if (GET_CODE (pic) != REG)
		{
		  emit_move_insn (reg, pic);
		  pic = reg;
		}
#if 0
	      emit_insn (gen_rtx_USE (VOIDmode,
				      gen_rtx_REG (Pmode,
						   PIC_OFFSET_TABLE_REGNUM)));
#endif

	      if (reload_in_progress)
		regs_ever_live[REGNO (pic)] = 1;
	      pic_ref = gen_rtx_PLUS (Pmode, pic,
				      gen_pic_offset (XEXP (orig, 0),
						      pic_base));
	    }

#if !defined (TARGET_TOC)
	  emit_move_insn (reg, pic_ref);
	  pic_ref = gen_const_mem (GET_MODE (orig), reg);
#endif
	}
      else
	{

#ifdef HAVE_lo_sum
	  if (GET_CODE (orig) == SYMBOL_REF
	      || GET_CODE (orig) == LABEL_REF)
	    {
	      rtx offset = gen_pic_offset (orig, pic_base);
#if defined (TARGET_TOC) /* i.e., PowerPC */
              rtx hi_sum_reg;

	      if (reg == 0)
		{
		  gcc_assert (!reload_in_progress);
		  reg = gen_reg_rtx (Pmode);
		}

	      hi_sum_reg = reg;

	      emit_insn (gen_rtx_SET (Pmode, hi_sum_reg,
				      (MACHO_DYNAMIC_NO_PIC_P)
				      ? gen_rtx_HIGH (Pmode, offset)
				      : gen_rtx_PLUS (Pmode,
						      pic_offset_table_rtx,
						      gen_rtx_HIGH (Pmode,
								    offset))));
	      emit_insn (gen_rtx_SET (VOIDmode, reg,
				      gen_rtx_LO_SUM (Pmode,
						      hi_sum_reg, offset)));
	      pic_ref = reg;
#else
	      emit_insn (gen_rtx_SET (VOIDmode, reg,
				      gen_rtx_HIGH (Pmode, offset)));
	      emit_insn (gen_rtx_SET (VOIDmode, reg,
				      gen_rtx_LO_SUM (Pmode, reg, offset)));
	      pic_ref = gen_rtx_PLUS (Pmode,
				      pic_offset_table_rtx, reg);
#endif
	    }
	  else
#endif  /*  HAVE_lo_sum  */
	    {
	      if (REG_P (orig)
	          || GET_CODE (orig) == SUBREG)
		{
		  return orig;
		}
	      else
		{
		  rtx pic = pic_offset_table_rtx;
		  if (GET_CODE (pic) != REG)
		    {
		      emit_move_insn (reg, pic);
		      pic = reg;
		    }
#if 0
		  emit_insn (gen_rtx_USE (VOIDmode,
					  pic_offset_table_rtx));
#endif
		  if (reload_in_progress)
		    regs_ever_live[REGNO (pic)] = 1;
		  pic_ref = gen_rtx_PLUS (Pmode,
					  pic,
					  gen_pic_offset (orig, pic_base));
		}
	    }
	}

      if (GET_CODE (pic_ref) != REG)
        {
          if (reg != 0)
            {
              emit_move_insn (reg, pic_ref);
              return reg;
            }
          else
            {
              return force_reg (mode, pic_ref);
            }
        }
      else
        {
          return pic_ref;
        }
    }

  else if (GET_CODE (orig) == SYMBOL_REF)
    return orig;

  else if (GET_CODE (orig) == PLUS
	   && (GET_CODE (XEXP (orig, 0)) == MEM
	       || GET_CODE (XEXP (orig, 0)) == SYMBOL_REF
	       || GET_CODE (XEXP (orig, 0)) == LABEL_REF)
	   && XEXP (orig, 0) != pic_offset_table_rtx
	   && GET_CODE (XEXP (orig, 1)) != REG)

    {
      rtx base;
      int is_complex = (GET_CODE (XEXP (orig, 0)) == MEM);

      base = machopic_legitimize_pic_address (XEXP (orig, 0), Pmode, reg);
      orig = machopic_legitimize_pic_address (XEXP (orig, 1),
					      Pmode, (base == reg ? 0 : reg));
      if (GET_CODE (orig) == CONST_INT)
	{
	  pic_ref = plus_constant (base, INTVAL (orig));
	  is_complex = 1;
	}
      else
	pic_ref = gen_rtx_PLUS (Pmode, base, orig);

      /* APPLE LOCAL begin gen ADD */
#ifdef MASK_80387
      {
	rtx mem, other;

	if (GET_CODE (orig) == MEM) {
	    mem = orig; other = base;
	    /* Swap the kids only if there is only one MEM, and it's on the right.  */
	    if (GET_CODE (base) != MEM) {
		XEXP (pic_ref, 0) = orig;
		XEXP (pic_ref, 1) = base;
	      }
	  }
	else if (GET_CODE (base) == MEM) {
	    mem = base; other = orig;
	  } else
	    mem = other = NULL_RTX;
     
	/* Both kids are MEMs.  */
	if (other && GET_CODE (other) == MEM)
	  other = force_reg (GET_MODE (other), other);

	/* The x86 can't post-index a MEM; emit an ADD instruction to handle this.  */
	if (mem && GET_CODE (mem) == MEM) {
	  if ( ! reload_in_progress) {
	    rtx set = gen_rtx_SET (VOIDmode, reg, pic_ref);
	    rtx clobber_cc = gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (CCmode, FLAGS_REG));
	    pic_ref = gen_rtx_PARALLEL (VOIDmode, gen_rtvec (2, set, clobber_cc));
	    emit_insn (pic_ref);
	    pic_ref = reg;
	    is_complex = 0;
	  }
	}
      }
#endif
      /* APPLE LOCAL end gen ADD */

      if (reg && is_complex)
	{
	  emit_move_insn (reg, pic_ref);
	  pic_ref = reg;
	}
      /* Likewise, should we set special REG_NOTEs here?  */
    }

  else if (GET_CODE (orig) == CONST)
    {
      return machopic_legitimize_pic_address (XEXP (orig, 0), Pmode, reg);
    }

  else if (GET_CODE (orig) == MEM
	   && GET_CODE (XEXP (orig, 0)) == SYMBOL_REF)
    {
      /* APPLE LOCAL begin use new pseudo for temp; reusing reg confuses PRE */
      rtx tempreg = reg;
      rtx addr;
      if ( !no_new_pseudos )
	tempreg = gen_reg_rtx (Pmode);
      addr = machopic_legitimize_pic_address (XEXP (orig, 0), Pmode, tempreg);
      /* APPLE LOCAL end use new pseudo for temp; reusing reg confuses PRE */
      addr = replace_equiv_address (orig, addr);
      emit_move_insn (reg, addr);
      pic_ref = reg;
    }

  return pic_ref;
}

/* Output the stub or non-lazy pointer in *SLOT, if it has been used.
   DATA is the FILE* for assembly output.  Called from
   htab_traverse.  */

static int
machopic_output_indirection (void **slot, void *data)
{
  machopic_indirection *p = *((machopic_indirection **) slot);
  FILE *asm_out_file = (FILE *) data;
  rtx symbol;
  const char *sym_name;
  const char *ptr_name;

  /* APPLE LOCAL ARM 5440570 */
  if (!p->used || p->emitted)
    return 1;

  symbol = p->symbol;
  sym_name = XSTR (symbol, 0);
  ptr_name = p->ptr_name;

  if (p->stub_p)
    {
      char *sym;
      char *stub;
      tree id;

      id = maybe_get_identifier (sym_name);
      if (id)
	{
	  tree id_orig = id;

	  while (IDENTIFIER_TRANSPARENT_ALIAS (id))
	    id = TREE_CHAIN (id);
	  if (id != id_orig)
	    sym_name = IDENTIFIER_POINTER (id);
	}

      sym = alloca (strlen (sym_name) + 2);
      if (sym_name[0] == '*' || sym_name[0] == '&')
	strcpy (sym, sym_name + 1);
      else if (sym_name[0] == '-' || sym_name[0] == '+')
	strcpy (sym, sym_name);
      else
	sprintf (sym, "%s%s", user_label_prefix, sym_name);

      stub = alloca (strlen (ptr_name) + 2);
      if (ptr_name[0] == '*' || ptr_name[0] == '&')
	strcpy (stub, ptr_name + 1);
      else
	sprintf (stub, "%s%s", user_label_prefix, ptr_name);

      machopic_output_stub (asm_out_file, sym, stub);
    }
  else if (! indirect_data (symbol)
	   && (machopic_symbol_defined_p (symbol)
	       || SYMBOL_REF_LOCAL_P (symbol)))
    {
      switch_to_section (data_section);
      assemble_align (GET_MODE_ALIGNMENT (Pmode));
      assemble_label (ptr_name);
      assemble_integer (gen_rtx_SYMBOL_REF (Pmode, sym_name),
			GET_MODE_SIZE (Pmode),
			GET_MODE_ALIGNMENT (Pmode), 1);
    }
  else
    {
      rtx init = const0_rtx;

      switch_to_section (darwin_sections[machopic_nl_symbol_ptr_section]);
      assemble_name (asm_out_file, ptr_name);
      fprintf (asm_out_file, ":\n");

      fprintf (asm_out_file, "\t.indirect_symbol ");
      assemble_name (asm_out_file, sym_name);
      fprintf (asm_out_file, "\n");

      /* Variables that are marked with MACHO_SYMBOL_STATIC need to
	 have their symbol name instead of 0 in the second entry of
	 the non-lazy symbol pointer data structure when they are
	 defined.  This allows the runtime to rebind newer instances
	 of the translation unit with the original instance of the
	 symbol.  */

      if ((SYMBOL_REF_FLAGS (symbol) & MACHO_SYMBOL_STATIC)
	  && machopic_symbol_defined_p (symbol))
	init = gen_rtx_SYMBOL_REF (Pmode, sym_name);

      assemble_integer (init, GET_MODE_SIZE (Pmode),
			GET_MODE_ALIGNMENT (Pmode), 1);
    }
  /* APPLE LOCAL ARM 5440570 */
  p->emitted = true;

  return 1;
}

void
machopic_finish (FILE *asm_out_file)
{
  if (machopic_indirections)
    /* APPLE LOCAL begin 5440570 */
    do
      {
	indirection_uses_changed = false;
	htab_traverse_noresize (machopic_indirections,
				machopic_output_indirection,
				asm_out_file);
      }
    while (indirection_uses_changed == true);
    /* APPLE LOCAL end 5440570 */
}

int
machopic_operand_p (rtx op)
{
  if (MACHOPIC_JUST_INDIRECT)
    {
      while (GET_CODE (op) == CONST)
	op = XEXP (op, 0);

      if (GET_CODE (op) == SYMBOL_REF)
	return machopic_symbol_defined_p (op);
      else
	return 0;
    }

  while (GET_CODE (op) == CONST)
    op = XEXP (op, 0);

  if (GET_CODE (op) == MINUS
      && GET_CODE (XEXP (op, 0)) == SYMBOL_REF
      && GET_CODE (XEXP (op, 1)) == SYMBOL_REF
      && machopic_symbol_defined_p (XEXP (op, 0))
      && machopic_symbol_defined_p (XEXP (op, 1)))
      return 1;

  return 0;
}

/* This function records whether a given name corresponds to a defined
   or undefined function or variable, for machopic_classify_ident to
   use later.  */

void
darwin_encode_section_info (tree decl, rtx rtl, int first ATTRIBUTE_UNUSED)
{
  rtx sym_ref;

  /* Do the standard encoding things first.  */
  default_encode_section_info (decl, rtl, first);

  if (TREE_CODE (decl) != FUNCTION_DECL && TREE_CODE (decl) != VAR_DECL)
    return;

  sym_ref = XEXP (rtl, 0);
  if (TREE_CODE (decl) == VAR_DECL)
    SYMBOL_REF_FLAGS (sym_ref) |= MACHO_SYMBOL_FLAG_VARIABLE;

  if (!DECL_EXTERNAL (decl)
      && (!TREE_PUBLIC (decl) || !DECL_WEAK (decl))
      && ! lookup_attribute ("weakref", DECL_ATTRIBUTES (decl))
      && ((TREE_STATIC (decl)
	   && (!DECL_COMMON (decl) || !TREE_PUBLIC (decl)))
	  || (!DECL_COMMON (decl) && DECL_INITIAL (decl)
	      && DECL_INITIAL (decl) != error_mark_node)))
    SYMBOL_REF_FLAGS (sym_ref) |= MACHO_SYMBOL_FLAG_DEFINED;

  if (! TREE_PUBLIC (decl))
    SYMBOL_REF_FLAGS (sym_ref) |= MACHO_SYMBOL_STATIC;

  /* APPLE LOCAL begin fix OBJC codegen */
  if (TREE_CODE (decl) == VAR_DECL)
    {
      if (strncmp (XSTR (sym_ref, 0), "_OBJC_", 6) == 0)
	SYMBOL_REF_FLAGS (sym_ref) |= MACHO_SYMBOL_FLAG_DEFINED;
    }
  /* APPLE LOCAL end fix OBJC codegen */
}

void
darwin_mark_decl_preserved (const char *name)
{
  fprintf (asm_out_file, ".no_dead_strip ");
  assemble_name (asm_out_file, name);
  fputc ('\n', asm_out_file);
}

int
machopic_reloc_rw_mask (void)
{
  return MACHOPIC_INDIRECT ? 3 : 0;
}

/* APPLE LOCAL begin radar 5575115, 6255595 */
/* This routine returns TRUE if EXP is a variable representing
   on objective C meta data. */
static inline bool
objc_internal_variable_name (tree exp)
{
  if (TREE_CODE (exp) == VAR_DECL)
    {
      tree decl_name = DECL_NAME (exp);
      if (decl_name && TREE_CODE (decl_name) == IDENTIFIER_NODE
	  && IDENTIFIER_POINTER (decl_name))
	{
	  const char* name = IDENTIFIER_POINTER (decl_name);
	  return 
	   (!strncmp (name, "_OBJC_", 6)
	    || !strncmp (name, "OBJC_", 5)
	    || !strncmp (name, "l_OBJC_", 7)
            || !strncmp (name, "l_objc_", 7));
	}
    }
  return false;
}
/* APPLE LOCAL end radar 5575115, 6255595 */

section *
machopic_select_section (tree exp, int reloc,
			 unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED)
{
  section *base_section;
  bool weak_p = (DECL_P (exp) && DECL_WEAK (exp)
		 && (lookup_attribute ("weak", DECL_ATTRIBUTES (exp))
		     || ! lookup_attribute ("weak_import",
					    DECL_ATTRIBUTES (exp))));

  if (TREE_CODE (exp) == FUNCTION_DECL)
    {
      if (reloc == 1)
	base_section = (weak_p
			? darwin_sections[text_unlikely_coal_section]
			: unlikely_text_section ());
      else
	base_section = weak_p ? darwin_sections[text_coal_section] : text_section;
    }
  else if (decl_readonly_section (exp, reloc))
    base_section = weak_p ? darwin_sections[const_coal_section] : darwin_sections[const_section];
  else if (TREE_READONLY (exp) || TREE_CONSTANT (exp))
    base_section = weak_p ? darwin_sections[const_data_coal_section] : darwin_sections[const_data_section];
  else
    base_section = weak_p ? darwin_sections[data_coal_section] : data_section;

  /* APPLE LOCAL begin fwritable strings  */
  if (TREE_CODE (exp) == STRING_CST
      /* APPLE LOCAL begin 5612787 mainline sse4 */
      /* deletion */
      /* Copied from varasm.c:output_constant_def_contents().  5346453 */
      && (MAX ((HOST_WIDE_INT)TREE_STRING_LENGTH (exp),
	       int_size_in_bytes (TREE_TYPE (exp)))
	  /* APPLE LOCAL ARM signedness mismatch */
	  == (HOST_WIDE_INT) strlen (TREE_STRING_POINTER (exp)) + 1)
      /* APPLE LOCAL end 5612787 mainline sse4 */
      && ! flag_writable_strings)
    return darwin_sections[cstring_section];
  /* APPLE LOCAL end fwritable strings, 5346453 */
  else if ((TREE_CODE (exp) == INTEGER_CST || TREE_CODE (exp) == REAL_CST)
	   && flag_merge_constants)
    {
      tree size = TYPE_SIZE_UNIT (TREE_TYPE (exp));

      if (TREE_CODE (size) == INTEGER_CST &&
	  TREE_INT_CST_LOW (size) == 4 &&
	  TREE_INT_CST_HIGH (size) == 0)
	return darwin_sections[literal4_section];
      else if (TREE_CODE (size) == INTEGER_CST &&
	       TREE_INT_CST_LOW (size) == 8 &&
	       TREE_INT_CST_HIGH (size) == 0)
	return darwin_sections[literal8_section];
      /* APPLE LOCAL begin mainline x86_64 literal16 */
#ifndef HAVE_GAS_LITERAL16
#define HAVE_GAS_LITERAL16 0
#endif
      else if (HAVE_GAS_LITERAL16
	       && TARGET_64BIT
      /* APPLE LOCAL end mainline x86_64 literal16 */
	       && TREE_CODE (size) == INTEGER_CST
	       && TREE_INT_CST_LOW (size) == 16
	       && TREE_INT_CST_HIGH (size) == 0)
	return darwin_sections[literal16_section];
      else
	return base_section;
    }
  else if (TREE_CODE (exp) == CONSTRUCTOR
	   && TREE_TYPE (exp)
	   && TREE_CODE (TREE_TYPE (exp)) == RECORD_TYPE
	   && TYPE_NAME (TREE_TYPE (exp)))
    {
      /* APPLE LOCAL constant strings */
      extern int flag_next_runtime;
      tree name = TYPE_NAME (TREE_TYPE (exp));
      if (TREE_CODE (name) == TYPE_DECL)
	name = DECL_NAME (name);

      if (!strcmp (IDENTIFIER_POINTER (name), "__builtin_ObjCString"))
	{
	  /* APPLE LOCAL begin radar 4792158 */
	  if (flag_next_runtime)
	    {
	      if (flag_objc_abi == 2)
		return darwin_sections[objc_v2_constant_string_object_section];
	      else
		return darwin_sections[objc_constant_string_object_section];
	    }
	  /* APPLE LOCAL end radar 4792158 */
	  else
	    return darwin_sections[objc_string_object_section];
	}
      /* APPLE LOCAL begin constant strings */
      else if (!strcmp (IDENTIFIER_POINTER (name), "__builtin_CFString"))
	return darwin_sections[cfstring_constant_object_section];
      /* APPLE LOCAL end constant strings */
      else
	return base_section;
    }
  /* APPLE LOCAL begin radar 5575115, 6255595 */
  else if (objc_internal_variable_name (exp))
  /* APPLE LOCAL end radar 5575115, 6255595 */
    {
      const char *name = IDENTIFIER_POINTER (DECL_NAME (exp));
      /* APPLE LOCAL begin radar 4792158 */
      if (flag_objc_abi == 1)
	{
      	  if (!strncmp (name, "_OBJC_CLASS_METHODS_", 20))
	    return darwin_sections[objc_cls_meth_section];
          else if (!strncmp (name, "_OBJC_INSTANCE_METHODS_", 23))
	    return darwin_sections[objc_inst_meth_section];
          else if (!strncmp (name, "_OBJC_CATEGORY_CLASS_METHODS_", 29))
	    return darwin_sections[objc_cat_cls_meth_section];
          else if (!strncmp (name, "_OBJC_CATEGORY_INSTANCE_METHODS_", 32))
	    return darwin_sections[objc_cat_inst_meth_section];
          else if (!strncmp (name, "_OBJC_CLASS_VARIABLES_", 22))
	    return darwin_sections[objc_class_vars_section];
          else if (!strncmp (name, "_OBJC_INSTANCE_VARIABLES_", 25))
	    return darwin_sections[objc_instance_vars_section];
          else if (!strncmp (name, "_OBJC_CLASS_PROTOCOLS_", 22))
	    return darwin_sections[objc_cat_cls_meth_section];
          else if (!strncmp (name, "_OBJC_CLASS_NAME_", 17))
	    return darwin_sections[objc_class_names_section];
          else if (!strncmp (name, "_OBJC_METH_VAR_NAME_", 20))
	    return darwin_sections[objc_meth_var_names_section];
          else if (!strncmp (name, "_OBJC_METH_VAR_TYPE_", 20))
	    return darwin_sections[objc_meth_var_types_section];
          else if (!strncmp (name, "_OBJC_CLASS_REFERENCES", 22))
	    return darwin_sections[objc_cls_refs_section];
          else if (!strncmp (name, "_OBJC_CLASS_", 12))
	    return darwin_sections[objc_class_section];
          else if (!strncmp (name, "_OBJC_METACLASS_", 16))
	    return darwin_sections[objc_meta_class_section];
          else if (!strncmp (name, "_OBJC_CATEGORY_", 15))
	    return darwin_sections[objc_category_section];
          else if (!strncmp (name, "_OBJC_SELECTOR_REFERENCES", 25))
	    return darwin_sections[objc_selector_refs_section];
          else if (!strncmp (name, "_OBJC_SELECTOR_FIXUP", 20))
	    return darwin_sections[objc_selector_fixup_section];
          else if (!strncmp (name, "_OBJC_SYMBOLS", 13))
	    return darwin_sections[objc_symbols_section];
          else if (!strncmp (name, "_OBJC_MODULES", 13))
	    return darwin_sections[objc_module_info_section];
          else if (!strncmp (name, "_OBJC_IMAGE_INFO", 16))
	    return darwin_sections[objc_image_info_section];
          else if (!strncmp (name, "_OBJC_PROTOCOL_INSTANCE_METHODS_", 32))
	    return darwin_sections[objc_cat_inst_meth_section];
          else if (!strncmp (name, "_OBJC_PROTOCOL_CLASS_METHODS_", 29))
	    return darwin_sections[objc_cat_cls_meth_section];
          else if (!strncmp (name, "_OBJC_PROTOCOL_REFS_", 20))
	    return darwin_sections[objc_cat_cls_meth_section];
          else if (!strncmp (name, "_OBJC_PROTOCOL_", 15))
	    return darwin_sections[objc_protocol_section];
          else if (!strncmp (name, "_OBJC_CLASSEXT_", 15))
	    return darwin_sections[objc_class_ext_section];
          else if (!strncmp (name, "_OBJC_$_PROP_LIST", 17)
		   || !strncmp (name, "_OBJC_$_PROP_PROTO", 18))
	    return darwin_sections[objc_prop_list_section];
          else if (!strncmp (name, "_OBJC_PROTOCOLEXT", 17))
            return darwin_sections[objc_protocol_ext_section];
	  else if (!strncmp (name, "_OBJC_PROP_NAME_ATTR_", 21))
	    return darwin_sections[cstring_section];
          else
	return base_section;
	}
      else /* flag_objc_abi == 2 */
        {
          if (!strncmp (name, "_OBJC_PROP_NAME_ATTR_", 21)
              || !strncmp (name, "_OBJC_CLASS_NAME_", 17)
              || !strncmp (name, "_OBJC_METH_VAR_NAME_", 20)
              || !strncmp (name, "_OBJC_METH_VAR_TYPE_", 20))
	    return darwin_sections[cstring_section];
          else if (!strncmp (name, "_OBJC_CLASSLIST_REFERENCES_", 27))
            return darwin_sections[objc_v2_classrefs_section];
          else if (!strncmp (name, "_OBJC_CLASSLIST_SUP_REFS_", 25))
            return darwin_sections[objc_v2_super_classrefs_section];
          /* APPLE LOCAL radar 5575115 - radar 6252174 */
          else if (!strncmp (name, "l_objc_msgSend", 14))
            return darwin_sections[objc_v2_message_refs_section];
          else if (!strncmp (name, "_OBJC_LABEL_CLASS_", 18))
            return darwin_sections[objc_v2_classlist_section];
          /* APPLE LOCAL radar 6351990 */
          else if (!strncmp (name, "l_OBJC_LABEL_PROTOCOL_", 22))
            return darwin_sections[objc_v2_protocollist_section];
          else if (!strncmp (name, "_OBJC_LABEL_CATEGORY_", 21))
            return darwin_sections[objc_v2_categorylist_section];
          else if (!strncmp (name, "_OBJC_LABEL_NONLAZY_CLASS_", 26))
            return darwin_sections[objc_v2_nonlazy_class_section];
          else if (!strncmp (name, "_OBJC_LABEL_NONLAZY_CATEGORY_", 29))
            return darwin_sections[objc_v2_nonlazy_category_section];
          /* APPLE LOCAL radar 6351990 */
          else if (!strncmp (name, "l_OBJC_PROTOCOL_REFERENCE_", 26))
            return darwin_sections[objc_v2_protocolrefs_section];
          else if (!strncmp (name, "_OBJC_SELECTOR_REFERENCES", 25))
            return darwin_sections[objc_v2_selector_refs_section];
          else if (!strncmp (name, "_OBJC_IMAGE_INFO", 16))
            return darwin_sections[objc_v2_image_info_section];
	    /* APPLE LOCAL begin radar 6255595 */
	  else if (!strncmp (name, "OBJC_CLASS_$_", 13)
		   || !strncmp (name, "OBJC_METACLASS_$_", 17))
	    return darwin_sections[objc_v2_classdefs_section];
          else
            return  (base_section == data_section) ? 
		      darwin_sections[objc_v2_metadata_section] : base_section;
    	  /* APPLE LOCAL end radar 6255595 */
	}
      /* APPLE LOCAL end radar 4792158 */
    }
  /* APPLE LOCAL coalescing */
  /* Removed special handling of '::operator new' and '::operator delete'.  */
  /* APPLE LOCAL begin darwin_set_section_for_var_p  */
  else
    return darwin_set_section_for_var_p (exp, reloc, align, base_section);
  /* APPLE LOCAL end darwin_set_section_for_var_p  */
}

/* LLVM LOCAL begin */
extern char * mempcpy (char *dst, const char *src, size_t len);
char *darwin_build_sysroot_path(const char *sysroot, const char *path) {
  char *str = NULL;
  char *str1 = NULL;
  char *darwin = NULL;
#ifndef ENABLE_LLVM
  return concat (sysroot, path, NULL);
#endif

  /* FIXME : When time is appropriate, handle other sdks.  */
  if (sysroot && strstr(sysroot, "MacOSX10.5.sdk") == NULL)
    return concat (sysroot, path, NULL);

  /* libstdc++ headers are fixed magically through sym link jungle.  */
  if (strstr(path, "c++") != NULL)
    return concat (sysroot, path, NULL);

  darwin = strstr(path, "apple-darwin");
  if (!darwin)
    return concat (sysroot, path, NULL);

  /* Released 10.5 SDK uses header paths that include OS version
     number, for example 9 in 
     .../MacOSX10.5.sdk/.../lib/gcc/i686-apple-darwin9/4.2.1/include
     However the 9 is constructed based on the host OS version on
     which the compiler is built. This means, the compiler will
     not be able to use 10.5 SDK unless it is built on 10.5 system.
     Fix header path here to make it work.
     
     Path includes "apple-darwinXYZ/" substring. Replace
     this substring with "apple-darwin9/". */
  str = XNEWVEC(char, strlen(sysroot) + strlen(path) + 2);
  str1 = mempcpy(str, sysroot, strlen(sysroot));
  str1 = mempcpy(str1, path, darwin - path);
  str1 = mempcpy(str1, "apple-darwin9", strlen("apple-darwin9"));
  darwin = strchr(darwin, '/');
  str1 = mempcpy(str1, darwin, strlen(darwin));
  return str;
}

#ifdef ENABLE_LLVM
static const char *skip_objc_prefix(const char *name)
{
  if (!strncmp (name, "_OBJC_", 6))
    return name + 6;
  else if (!strncmp (name, "OBJC_", 5))
    return name + 5;

  return name + 7;
}

static const char *
darwin_objc_llvm_special_name_section_help(tree decl) {
  /* Get a pointer to the name, past the L_OBJC_ prefix. */
  const char *name = IDENTIFIER_POINTER (DECL_NAME (decl));
  const char *base_section = 0;
  const char *section = 0;
  bool weak_p = (DECL_P (decl) && DECL_WEAK (decl)
		 && (lookup_attribute ("weak", DECL_ATTRIBUTES (decl))
		     || ! lookup_attribute ("weak_import",
					    DECL_ATTRIBUTES (decl))));

  if (TREE_READONLY (decl) || TREE_CONSTANT (decl))
    base_section = weak_p ? "__DATA,__const_coal,coalesced" : "__DATA,__const";
  else
    base_section = weak_p ? "__DATA,__datacoal_nt,coalesced" : "__DATA,__data";

  name = skip_objc_prefix(name);
  section = darwin_objc_llvm_special_name_section(name);

  if (!section && flag_objc_abi == 2)
    section = strcmp(base_section, "__DATA,__data") == 0 ?
      "__DATA, __objc_const" : base_section;

  return section ? section : base_section;
}

const char *darwin_objc_llvm_special_name_section(const char *name) {
  if (flag_objc_abi == 1) {
    if (!strncmp (name, "CLASS_METHODS_", 14))
      return "__OBJC,__cls_meth,regular,no_dead_strip";
    else if (!strncmp (name, "INSTANCE_METHODS_", 17))
      return "__OBJC,__inst_meth,regular,no_dead_strip";
    else if (!strncmp (name, "CATEGORY_CLASS_METHODS_", 23))
      return "__OBJC,__cat_cls_meth,regular,no_dead_strip";
    else if (!strncmp (name, "CATEGORY_INSTANCE_METHODS_", 26))
      return "__OBJC,__cat_inst_meth,regular,no_dead_strip";
    else if (!strncmp (name, "CLASS_VARIABLES_", 16))
      return "__OBJC,__class_vars,regular,no_dead_strip";
    else if (!strncmp (name, "INSTANCE_VARIABLES_", 19))
      return "__OBJC,__instance_vars,regular,no_dead_strip";
    else if (!strncmp (name, "CLASS_PROTOCOLS_", 16))
      return "__OBJC,__cat_cls_meth,regular,no_dead_strip";
    else if (!strncmp (name, "CLASS_NAME_", 11))
      return "__TEXT,__cstring,cstring_literals";
    else if (!strncmp (name, "METH_VAR_NAME_", 14))
      return "__TEXT,__cstring,cstring_literals";
    else if (!strncmp (name, "METH_VAR_TYPE_", 14))
      return "__TEXT,__cstring,cstring_literals";
    else if (!strncmp (name, "PROP_NAME_ATTR_", 15))
      return "__TEXT,__cstring,cstring_literals";
    else if (!strncmp (name, "CLASS_REFERENCES", 16))
      return "__OBJC,__cls_refs,literal_pointers,no_dead_strip";
    else if (!strncmp (name, "CLASS_", 6))
      return "__OBJC,__class,regular,no_dead_strip";
    else if (!strncmp (name, "METACLASS_", 10))
      return "__OBJC,__meta_class,regular,no_dead_strip";
    else if (!strncmp (name, "CATEGORY_", 9))
      return "__OBJC,__category,regular,no_dead_strip";
    else if (!strncmp (name, "SELECTOR_REFERENCES", 19))
      return "__OBJC,__message_refs,literal_pointers,no_dead_strip";
    else if (!strncmp (name, "SELECTOR_FIXUP", 14))
      return "__OBJC,__sel_fixup,regular";/*,no_dead_strip";*/
    else if (!strncmp (name, "SYMBOLS", 7))
      return "__OBJC,__symbols,regular,no_dead_strip";
    else if (!strncmp (name, "MODULES", 7))
      return "__OBJC,__module_info,regular,no_dead_strip";
    else if (!strncmp (name, "IMAGE_INFO", 10))
      return "__OBJC, __image_info,regular" /*,no_dead_strip";*/;
    else if (!strncmp (name, "PROTOCOL_INSTANCE_METHODS_", 26))
      return "__OBJC,__cat_inst_meth,regular,no_dead_strip";
    else if (!strncmp (name, "PROTOCOL_CLASS_METHODS_", 23))
      return "__OBJC,__cat_cls_meth,regular,no_dead_strip";
    else if (!strncmp (name, "PROTOCOL_REFS_", 14))
      return "__OBJC,__cat_cls_meth,regular,no_dead_strip";
    else if (!strncmp (name, "PROTOCOL_", 9))
      return "__OBJC,__protocol,regular,no_dead_strip";
    else if (!strncmp (name, "CLASSEXT_", 9))
      return "__OBJC,__class_ext,regular,no_dead_strip";
    else if (!strncmp (name, "$_PROP_LIST", 11)
             || !strncmp (name, "$_PROP_PROTO", 12))
      return "__OBJC,__property,regular,no_dead_strip";
    else if (!strncmp (name, "PROTOCOLEXT", 11))
      return "__OBJC,__protocol_ext,regular,no_dead_strip";
    else if (!strncmp (name, "PROP_NAME_ATTR_", 15))
      return "__TEXT,__cstring,cstring_literals";
  } else if (flag_objc_abi == 2) {
    if (!strncmp (name, "PROP_NAME_ATTR_", 15))
      return "__TEXT,__cstring,cstring_literals";
    else if (!strncmp (name, "CLASSLIST_REFERENCES_", 21))
      return "__DATA, __objc_classrefs, regular, no_dead_strip";
    else if (!strncmp (name, "CLASSLIST_SUP_REFS_", 19))
      return "__DATA, __objc_superrefs, regular, no_dead_strip"; 
    else if (!strncmp (name, "msgSend", 7))
      return "__DATA, __objc_msgrefs, coalesced";
    else if (!strncmp (name, "LABEL_CLASS_", 12))
      return "__DATA, __objc_classlist, regular, no_dead_strip"; 
    else if (!strncmp (name, "LABEL_PROTOCOL_", 15))
      return "__DATA, __objc_protolist, coalesced, no_dead_strip"; 
    else if (!strncmp (name, "LABEL_CATEGORY_", 15))
      return "__DATA, __objc_catlist, regular, no_dead_strip"; 
    else if (!strncmp (name, "LABEL_NONLAZY_CLASS_", 20))
      return "__DATA, __objc_nlclslist, regular, no_dead_strip";
    else if (!strncmp (name, "LABEL_NONLAZY_CATEGORY_", 23))
      return "__DATA, __objc_nlcatlist, regular, no_dead_strip";
    else if (!strncmp (name, "PROTOCOL_REFERENCE_", 19))
      return "__DATA, __objc_protorefs, coalesced, no_dead_strip";
    else if (!strncmp (name, "SELECTOR_REFERENCES", 19))
      return "__DATA, __objc_selrefs, literal_pointers, no_dead_strip";
    else if (!strncmp (name, "IMAGE_INFO", 10))
      return "__DATA, __objc_imageinfo, regular, no_dead_strip";
    else if (!strncmp (name, "CLASS_$_", 8)
             || !strncmp (name, "METACLASS_$_", 12))
      return "__DATA, __objc_data";
    else if (!strncmp (name, "METH_VAR_NAME_", 14))
      return "__TEXT, __objc_methname, cstring_literals";
    else if (!strncmp (name, "METH_VAR_TYPE_", 14))
      return "__TEXT, __objc_methtype, cstring_literals";
    else if (!strncmp (name, "CLASS_NAME_", 11))
      return "__TEXT, __objc_classname, cstring_literals";
  }
  return 0;
}

const char *darwin_objc_llvm_implicit_target_global_var_section(tree decl) {
  if (TREE_CODE(decl) == CONST_DECL) {
    extern int flag_next_runtime;
    tree typename = TYPE_NAME(TREE_TYPE(decl));
    if (TREE_CODE(typename) == TYPE_DECL)
      typename = DECL_NAME(typename);
    
    if (!strcmp(IDENTIFIER_POINTER(typename), "__builtin_ObjCString")) {
      if (flag_next_runtime)
        return "__OBJC, __cstring_object,regular,no_dead_strip";
      else
        return "__OBJC, __string_object,no_dead_strip";
    } else if (!strcmp(IDENTIFIER_POINTER(typename), "__builtin_CFString")) {
      return
#ifdef LLVM_CONST_DATA_SECTION
	(flag_writable_strings) ? LLVM_CONST_DATA_SECTION :
#endif
	"__DATA, __cfstring";
    } else {
      return 0;
    }
  }
  
  return darwin_objc_llvm_special_name_section_help(decl);
}
#endif
/* LLVM LOCAL end */

/* This can be called with address expressions as "rtx".
   They must go in "const".  */

section *
machopic_select_rtx_section (enum machine_mode mode, rtx x,
			     unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED)
{
  if (GET_MODE_SIZE (mode) == 8
      && (GET_CODE (x) == CONST_INT
	  || GET_CODE (x) == CONST_DOUBLE))
    return darwin_sections[literal8_section];
  else if (GET_MODE_SIZE (mode) == 4
	   && (GET_CODE (x) == CONST_INT
	       || GET_CODE (x) == CONST_DOUBLE))
    return darwin_sections[literal4_section];
  /* APPLE LOCAL begin mainline x86_64 literal16 */
  else if (HAVE_GAS_LITERAL16
	   && TARGET_64BIT
  /* APPLE LOCAL end mainline x86_64 literal16 */
	   && GET_MODE_SIZE (mode) == 16
	   && (GET_CODE (x) == CONST_INT
	       || GET_CODE (x) == CONST_DOUBLE
	       || GET_CODE (x) == CONST_VECTOR))
    return darwin_sections[literal16_section];
  else if (MACHOPIC_INDIRECT
	   && (GET_CODE (x) == SYMBOL_REF
	       || GET_CODE (x) == CONST
	       || GET_CODE (x) == LABEL_REF))
    return darwin_sections[const_data_section];
  else
    return darwin_sections[const_section];
}

void
machopic_asm_out_constructor (rtx symbol, int priority ATTRIBUTE_UNUSED)
{
  if (MACHOPIC_INDIRECT)
    switch_to_section (darwin_sections[mod_init_section]);
  else
    switch_to_section (darwin_sections[constructor_section]);
  assemble_align (POINTER_SIZE);
  assemble_integer (symbol, POINTER_SIZE / BITS_PER_UNIT, POINTER_SIZE, 1);

  if (! MACHOPIC_INDIRECT)
    fprintf (asm_out_file, ".reference .constructors_used\n");
}

void
machopic_asm_out_destructor (rtx symbol, int priority ATTRIBUTE_UNUSED)
{
  if (MACHOPIC_INDIRECT)
    switch_to_section (darwin_sections[mod_term_section]);
  else
    switch_to_section (darwin_sections[destructor_section]);
  assemble_align (POINTER_SIZE);
  assemble_integer (symbol, POINTER_SIZE / BITS_PER_UNIT, POINTER_SIZE, 1);

  if (! MACHOPIC_INDIRECT)
    fprintf (asm_out_file, ".reference .destructors_used\n");
}

void
darwin_globalize_label (FILE *stream, const char *name)
{
  if (!!strncmp (name, "_OBJC_", 6))
    default_globalize_label (stream, name);
}

/* APPLE LOCAL begin assembly "abort" directive  */
/* This can be called instead of EXIT.  It will emit a '.abort' directive
   into any existing assembly file, causing assembly to immediately abort,
   thus preventing the assembler from spewing out numerous, irrelevant
   error messages.  */

void
abort_assembly_and_exit (int status)
{
  /* If we're aborting, get the assembler to abort, too.  */
  if (status == FATAL_EXIT_CODE && asm_out_file != 0)
    fprintf (asm_out_file, "\n.abort\n");

  exit (status);
}
/* APPLE LOCAL end assembly "abort" directive  */

/* APPLE LOCAL begin ObjC GC */
tree
darwin_handle_objc_gc_attribute (tree *node,
				 tree name,
				 tree args,
				 int flags ATTRIBUTE_UNUSED,
				 bool *no_add_attrs)
{
  tree orig = *node, type;

  /* Propagate GC-ness to the innermost pointee.  */
  while (POINTER_TYPE_P (orig)
	 || TREE_CODE (orig) == FUNCTION_TYPE
	 || TREE_CODE (orig) == METHOD_TYPE
	 || TREE_CODE (orig) == ARRAY_TYPE)
    orig = TREE_TYPE (orig);

  type = build_type_attribute_variant (orig,
				       tree_cons (name, args,
				       TYPE_ATTRIBUTES (orig)));

  /* For some reason, build_type_attribute_variant() creates a distinct
     type instead of a true variant!  We make up for this here.  */
  /* APPLE LOCAL begin radar 4600999 */
  /* The main variant must be preserved no matter what. What ever
     main variant comes out of the call to build_type_attribute_variant
     is bogus here. */
  if (TYPE_MAIN_VARIANT (orig) != TYPE_MAIN_VARIANT (type))
    {
      TYPE_MAIN_VARIANT (type) = TYPE_MAIN_VARIANT (orig);
  /* APPLE LOCAL end radar 4600999 */
      TYPE_NEXT_VARIANT (type) = TYPE_NEXT_VARIANT (orig);
      TYPE_NEXT_VARIANT (orig) = type;
    }

  *node = reconstruct_complex_type (*node, type);
  /* No need to hang on to the attribute any longer.  */
  *no_add_attrs = true;

  return NULL_TREE;
}
/* APPLE LOCAL end ObjC GC */

/* APPLE LOCAL begin radar 5595352 */
tree
darwin_handle_nsobject_attribute (tree *node,
                                  tree name,
                                  tree args ATTRIBUTE_UNUSED,
                                  int flags ATTRIBUTE_UNUSED,
                                  bool *no_add_attrs)
{
  tree orig = *node, type;
  if (!POINTER_TYPE_P (orig) || TREE_CODE (TREE_TYPE (orig)) != RECORD_TYPE)
    {
      error ("__attribute ((NSObject)) is for pointer types only");
      return NULL_TREE;
    }
  type = build_type_attribute_variant (orig,
				       tree_cons (name, NULL_TREE,
				       TYPE_ATTRIBUTES (orig)));
  /* The main variant must be preserved no matter what. What ever
     main variant comes out of the call to build_type_attribute_variant
     is bogus here. */
  if (TYPE_MAIN_VARIANT (orig) != TYPE_MAIN_VARIANT (type))
    {
      TYPE_MAIN_VARIANT (type) = TYPE_MAIN_VARIANT (orig);
      TYPE_NEXT_VARIANT (type) = TYPE_NEXT_VARIANT (orig);
      TYPE_NEXT_VARIANT (orig) = type;
    }

  *node = type;
  /* No need to hang on to the attribute any longer.  */
  *no_add_attrs = true;
  return NULL_TREE;
}
/* APPLE LOCAL end radar 5595352 */

/* APPLE LOCAL begin darwin_set_section_for_var_p  20020226 --turly  */

/* This is specifically for any initialised static class constants
   which may be output by the C++ front end at the end of compilation. 
   SELECT_SECTION () macro won't do because these are VAR_DECLs, not
   STRING_CSTs or INTEGER_CSTs.  And by putting 'em in appropriate
   sections, we save space.  

   FIXME: does this really do anything?  Won't the DECL_WEAK test be
   true 99% (or 100%) of the time?  In the other 1% of the time,
   shouldn't select_section be fixed instead of this hackery?  */

section*
darwin_set_section_for_var_p (tree exp, int reloc, int align, section* base_section)
{
  if (!reloc && TREE_CODE (exp) == VAR_DECL
      && DECL_ALIGN (exp) == align 
      && TREE_READONLY (exp) && DECL_INITIAL (exp)
      && ! DECL_WEAK (exp))
    {
      /* Put constant string vars in ".cstring" section.  */

      if (TREE_CODE (TREE_TYPE (exp)) == ARRAY_TYPE
	  && TREE_CODE (TREE_TYPE (TREE_TYPE (exp))) == INTEGER_TYPE
	  && integer_onep (TYPE_SIZE_UNIT (TREE_TYPE (TREE_TYPE (exp))))
	  && TREE_CODE (DECL_INITIAL (exp)) == STRING_CST)
	{

	  /* Compare string length with actual number of characters
	     the compiler will write out (which is not necessarily
	     TREE_STRING_LENGTH, in the case of a constant array of
	     characters that is not null-terminated).   Select appropriate
	     section accordingly. */

	  if (MIN ( TREE_STRING_LENGTH (DECL_INITIAL(exp)),
		    int_size_in_bytes (TREE_TYPE (exp)))
	      == (long) strlen (TREE_STRING_POINTER (DECL_INITIAL (exp))) + 1)
	    return darwin_sections[cstring_section];
	  else
	    return darwin_sections[const_section];
	}
     else
      if (TREE_READONLY (exp) 
	  && ((TREE_CODE (TREE_TYPE (exp)) == INTEGER_TYPE
	       && TREE_CODE (DECL_INITIAL (exp)) == INTEGER_CST)
	      || (TREE_CODE (TREE_TYPE (exp)) == REAL_TYPE
	  	  && TREE_CODE (DECL_INITIAL (exp)) == REAL_CST))
	  && TREE_CODE (TYPE_SIZE_UNIT (TREE_TYPE (DECL_INITIAL (exp))))
		== INTEGER_CST)
	{
	  tree size = TYPE_SIZE_UNIT (TREE_TYPE (DECL_INITIAL (exp)));
	  if (TREE_INT_CST_HIGH (size) != 0)
	    return base_section;

	  /* Put integer and float consts in the literal4|8|16 sections.  */

	  if (TREE_INT_CST_LOW (size) == 4)
	    return darwin_sections[literal4_section];
	  else if (TREE_INT_CST_LOW (size) == 8)
	    return darwin_sections[literal8_section];                                
	  else if (HAVE_GAS_LITERAL16
		   && TARGET_64BIT
		   && TREE_INT_CST_LOW (size) == 16)
	    return darwin_sections[literal16_section];
	}
    }

  return base_section;
}
/* APPLE LOCAL end darwin_set_section_for_var_p  20020226 --turly  */

void
darwin_asm_named_section (const char *name,
			  unsigned int flags ATTRIBUTE_UNUSED,
			  tree decl ATTRIBUTE_UNUSED)
{
  fprintf (asm_out_file, "\t.section %s\n", name);
}

void
darwin_unique_section (tree decl ATTRIBUTE_UNUSED, int reloc ATTRIBUTE_UNUSED)
{
  /* Darwin does not use unique sections.  */
}

/* Handle __attribute__ ((apple_kext_compatibility)).
   This only applies to darwin kexts for 2.95 compatibility -- it shrinks the
   vtable for classes with this attribute (and their descendants) by not
   outputting the new 3.0 nondeleting destructor.  This means that such
   objects CANNOT be allocated on the stack or as globals UNLESS they have
   a completely empty `operator delete'.
   Luckily, this fits in with the Darwin kext model.

   This attribute also disables gcc3's potential overlaying of derived
   class data members on the padding at the end of the base class.  */

tree
darwin_handle_kext_attribute (tree *node, tree name,
			      tree args ATTRIBUTE_UNUSED,
			      int flags ATTRIBUTE_UNUSED,
			      bool *no_add_attrs)
{
  /* APPLE KEXT stuff -- only applies with pure static C++ code.  */
  if (! TARGET_KEXTABI)
    {
      warning (0, "%<%s%> 2.95 vtable-compatability attribute applies "
	       "only when compiling a kext", IDENTIFIER_POINTER (name));

      *no_add_attrs = true;
    }
  else if (TREE_CODE (*node) != RECORD_TYPE)
    {
      warning (0, "%<%s%> 2.95 vtable-compatability attribute applies "
	       "only to C++ classes", IDENTIFIER_POINTER (name));

      *no_add_attrs = true;
    }

  return NULL_TREE;
}

/* APPLE LOCAL begin radar 4733555 */
/* Ick, this probably will cause other languages to die.  */
extern bool objc_method_decl (enum tree_code ARG_UNUSED (opcode));
  /* APPLE LOCAL end radar 4733555 */

/* Handle a "weak_import" attribute; arguments as in
   struct attribute_spec.handler.  */

tree
darwin_handle_weak_import_attribute (tree *node, tree name,
				     tree ARG_UNUSED (args),
				     int ARG_UNUSED (flags),
				     bool * no_add_attrs)
{
  /* APPLE LOCAL begin radar 4733555 */
  /* The compiler should silently ignore weak_import when specified on a method. All 
     Objective-C methods are "weak" in the sense that the availability macros want. */
  if (objc_method_decl (TREE_CODE (*node)))
    return NULL_TREE;
  /* APPLE LOCAL end radar 4733555 */
  /* APPLE LOCAL begin weak_import on property 6676828 */
  if (in_objc_property_decl_context ())
    {
      *no_add_attrs = true;
      return NULL_TREE;
    }
  /* APPLE LOCAL end weak_import on property 6676828 */
  /* LLVM LOCAL 6652529 begin */
  if ((TREE_CODE (*node) != FUNCTION_DECL && TREE_CODE (*node) != VAR_DECL)
      || !DECL_EXTERNAL (*node))
  /* LLVM LOCAL 6652529 end */
    {
      warning (OPT_Wattributes, "%qs attribute ignored",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }
  else
    declare_weak (*node);

  return NULL_TREE;
}

/* APPLE LOCAL begin for-fsf-4_4 5480287 */ \
/* APPLE LOCAL end for-fsf-4_4 5480287 */ \
/* Emit a label for an FDE, making it global and/or weak if appropriate.
   The third parameter is nonzero if this is for exception handling.
   The fourth parameter is nonzero if this is just a placeholder for an
   FDE that we are omitting. */

void
darwin_emit_unwind_label (FILE *file, tree decl, int for_eh, int empty)
{
/* APPLE LOCAL begin for-fsf-4_4 5480287 */ \
  char *lab;

  if (! for_eh)
    return;

  lab = concat (IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl)), ".eh", NULL);

  if (TREE_PUBLIC (decl))
    {
      targetm.asm_out.globalize_label (file, lab);
      if (DECL_VISIBILITY (decl) == VISIBILITY_HIDDEN)
	{
	  fputs ("\t.private_extern ", file);
	  assemble_name (file, lab);
	  fputc ('\n', file);
	}
    }

  if (DECL_WEAK (decl))
    {
      fputs ("\t.weak_definition ", file);
      assemble_name (file, lab);
      fputc ('\n', file);
    }

  assemble_name (file, lab);
  if (empty)
    {
      fputs (" = 0\n", file);

      /* Mark the absolute .eh and .eh1 style labels as needed to
	 ensure that we don't dead code strip them and keep such
	 labels from another instantiation point until we can fix this
	 properly with group comdat support.  */
      darwin_mark_decl_preserved (lab);
    }
  else
    fputs (":\n", file);

/* APPLE LOCAL end for-fsf-4_4 5480287 */ \
  free (lab);
}

static GTY(()) unsigned long except_table_label_num;

void
darwin_emit_except_table_label (FILE *file)
{
  char section_start_label[30];

  ASM_GENERATE_INTERNAL_LABEL (section_start_label, "GCC_except_table",
			       except_table_label_num++);
  ASM_OUTPUT_LABEL (file, section_start_label);
}
/* Generate a PC-relative reference to a Mach-O non-lazy-symbol.  */

void
darwin_non_lazy_pcrel (FILE *file, rtx addr)
{
  const char *nlp_name;

  gcc_assert (GET_CODE (addr) == SYMBOL_REF);

  nlp_name = machopic_indirection_name (addr, /*stub_p=*/false);
  fputs ("\t.long\t", file);
  ASM_OUTPUT_LABELREF (file, nlp_name);
  fputs ("-.", file);
}

/* Emit an assembler directive to set visibility for a symbol.  The
   only supported visibilities are VISIBILITY_DEFAULT and
   VISIBILITY_HIDDEN; the latter corresponds to Darwin's "private
   extern".  There is no MACH-O equivalent of ELF's
   VISIBILITY_INTERNAL or VISIBILITY_PROTECTED. */

void
/* LLVM LOCAL begin */
darwin_assemble_visibility (tree decl ATTRIBUTE_UNUSED, int vis)
{
  if (vis == VISIBILITY_DEFAULT)
    ;
  else if (vis == VISIBILITY_HIDDEN)
    {
/* LLVM LOCAL */
#ifndef ENABLE_LLVM
      fputs ("\t.private_extern ", asm_out_file);
      assemble_name (asm_out_file,
		     (IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl))));
      fputs ("\n", asm_out_file);
/* LLVM LOCAL */
#endif
    }
  else
    warning (OPT_Wattributes, "internal and protected visibility attributes "
	     "not supported in this configuration; ignored");
}

/* Output a difference of two labels that will be an assembly time
   constant if the two labels are local.  (.long lab1-lab2 will be
   very different if lab1 is at the boundary between two sections; it
   will be relocated according to the second section, not the first,
   so one ends up with a difference between labels in different
   sections, which is bad in the dwarf2 eh context for instance.)  */

static int darwin_dwarf_label_counter;

void
darwin_asm_output_dwarf_delta (FILE *file, int size,
			       const char *lab1, const char *lab2)
{
  int islocaldiff = (lab1[0] == '*' && lab1[1] == 'L'
		     && lab2[0] == '*' && lab2[1] == 'L');
  const char *directive = (size == 8 ? ".quad" : ".long");

  if (islocaldiff)
    fprintf (file, "\t.set L$set$%d,", darwin_dwarf_label_counter);
  else
    fprintf (file, "\t%s\t", directive);
  assemble_name_raw (file, lab1);
  fprintf (file, "-");
  assemble_name_raw (file, lab2);
  if (islocaldiff)
    fprintf (file, "\n\t%s L$set$%d", directive, darwin_dwarf_label_counter++);
}

/* Output labels for the start of the DWARF sections if necessary.  */
void
darwin_file_start (void)
{
  if (write_symbols == DWARF2_DEBUG)
    {
      static const char * const debugnames[] =
	{
	  DEBUG_FRAME_SECTION,
	  DEBUG_INFO_SECTION,
	  DEBUG_ABBREV_SECTION,
	  DEBUG_ARANGES_SECTION,
	  DEBUG_MACINFO_SECTION,
	  DEBUG_LINE_SECTION,
	  DEBUG_LOC_SECTION,
	  DEBUG_PUBNAMES_SECTION,
	  /* APPLE LOCAL begin pubtypes, approved for 4.3 4535968  */
	  DEBUG_PUBTYPES_SECTION,
	  /* APPLE LOCAL end pubtypes, approved for 4.3 4535968  */
	  /* APPLE LOCAL radar 6275985 debug inlined section  */
	  DEBUG_INLINED_SECTION,
	  DEBUG_STR_SECTION,
	  DEBUG_RANGES_SECTION
	};
      size_t i;

      for (i = 0; i < ARRAY_SIZE (debugnames); i++)
	{
	  int namelen;

	  switch_to_section (get_section (debugnames[i], SECTION_DEBUG, NULL));

	  gcc_assert (strncmp (debugnames[i], "__DWARF,", 8) == 0);
	  gcc_assert (strchr (debugnames[i] + 8, ','));

	  namelen = strchr (debugnames[i] + 8, ',') - (debugnames[i] + 8);
	  fprintf (asm_out_file, "Lsection%.*s:\n", namelen, debugnames[i] + 8);
	}
    }
}

/* Output an offset in a DWARF section on Darwin.  On Darwin, DWARF section
   offsets are not represented using relocs in .o files; either the
   section never leaves the .o file, or the linker or other tool is
   responsible for parsing the DWARF and updating the offsets.  */

void
darwin_asm_output_dwarf_offset (FILE *file, int size, const char * lab,
				section *base)
{
  char sname[64];
  int namelen;

  gcc_assert (base->common.flags & SECTION_NAMED);
  gcc_assert (strncmp (base->named.name, "__DWARF,", 8) == 0);
  gcc_assert (strchr (base->named.name + 8, ','));

  namelen = strchr (base->named.name + 8, ',') - (base->named.name + 8);
  sprintf (sname, "*Lsection%.*s", namelen, base->named.name + 8);
  darwin_asm_output_dwarf_delta (file, size, lab, sname);
}

void
darwin_file_end (void)
{
  machopic_finish (asm_out_file);
  /* APPLE LOCAL constant cfstrings */
  if (darwin_running_cxx)
    {
      switch_to_section (darwin_sections[constructor_section]);
      switch_to_section (darwin_sections[destructor_section]);
      ASM_OUTPUT_ALIGN (asm_out_file, 1);
    }
  /* APPLE LOCAL begin CW asm blocks */
  if (! has_alternative_entry_points ())
    fprintf (asm_out_file, "\t.subsections_via_symbols\n");
  /* APPLE LOCAL end CW asm blocks */
}

/* APPLE LOCAL KEXT treat vtables as overridable */
#define DARWIN_VTABLE_P(DECL) lang_hooks.vtable_p (DECL)

/* Cross-module name binding.  Darwin does not support overriding
   functions at dynamic-link time, except for vtables in kexts.  */

bool
darwin_binds_local_p (tree decl)
{
  return default_binds_local_p_1 (decl,
				  TARGET_KEXTABI && DARWIN_VTABLE_P (decl));
}

/* APPLE LOCAL begin constant cfstrings */
int darwin_running_cxx;

static GTY(()) tree cfstring_class_reference = NULL_TREE;
static GTY(()) tree cfstring_type_node = NULL_TREE;
static GTY(()) tree ccfstring_type_node = NULL_TREE;
static GTY(()) tree pccfstring_type_node = NULL_TREE;
static GTY(()) tree pcint_type_node = NULL_TREE;
static GTY(()) tree pcchar_type_node = NULL_TREE;

/* Store all constructed constant CFStrings in a hash table so that
   they get uniqued properly.  */

struct cfstring_descriptor GTY(())
{
  /* The literal argument .  */
  tree literal;

  /* The resulting constant CFString.  */
  tree constructor;
};

static GTY((param_is (struct cfstring_descriptor))) htab_t cfstring_htab;

static hashval_t cfstring_hash (const void *);
static int cfstring_eq (const void *, const void *);

void
darwin_init_cfstring_builtins (void)
{
  tree field, fields, pccfstring_ftype_pcchar;
  /* APPLE LOCAL begin 3996036 */
  int save_warn_padded;
  /* APPLE LOCAL end 3996036 */

  /* struct __builtin_CFString {
       const int *isa;		(will point at
       int flags;		 __CFConstantStringClassReference)
       const char *str;
       long length;
     };  */

  pcint_type_node
    = build_pointer_type (build_qualified_type (integer_type_node,
			  TYPE_QUAL_CONST));
  pcchar_type_node
    = build_pointer_type (build_qualified_type (char_type_node,
			  TYPE_QUAL_CONST));
  cfstring_type_node = (*lang_hooks.types.make_type) (RECORD_TYPE);
  fields = build_decl (FIELD_DECL, NULL_TREE, pcint_type_node);
  field = build_decl (FIELD_DECL, NULL_TREE, integer_type_node);
  TREE_CHAIN (field) = fields; fields = field;
  field = build_decl (FIELD_DECL, NULL_TREE, pcchar_type_node);
  TREE_CHAIN (field) = fields; fields = field;
  /* APPLE LOCAL radar 4493912 */
  field = build_decl (FIELD_DECL, NULL_TREE, long_integer_type_node);
  TREE_CHAIN (field) = fields; fields = field;
  /* NB: The finish_builtin_struct() routine expects FIELD_DECLs in
     reverse order!  */
  /* APPLE LOCAL begin 3996036 */
  save_warn_padded = warn_padded;
  warn_padded = 0;
  /* APPLE LOCAL end 3996036 */
  finish_builtin_struct (cfstring_type_node, "__builtin_CFString",
			 fields, NULL_TREE);
  /* APPLE LOCAL begin 3996036 */
  warn_padded = save_warn_padded; 
  /* APPLE LOCAL end 3996036 */

  /* const struct __builtin_CFstring *
     __builtin___CFStringMakeConstantString (const char *); */

  ccfstring_type_node
    = build_qualified_type (cfstring_type_node, TYPE_QUAL_CONST);
  pccfstring_type_node
    = build_pointer_type (ccfstring_type_node);
  pccfstring_ftype_pcchar
    = build_function_type_list (pccfstring_type_node,
				pcchar_type_node, NULL_TREE);
  lang_hooks.builtin_function ("__builtin___CFStringMakeConstantString",
			       pccfstring_ftype_pcchar,
			       DARWIN_BUILTIN_CFSTRINGMAKECONSTANTSTRING,
			       BUILT_IN_NORMAL, NULL, NULL_TREE);

  /* extern int __CFConstantStringClassReference[];  */
  cfstring_class_reference
   = build_decl (VAR_DECL,
		 get_identifier ("__CFConstantStringClassReference"),
		 build_array_type (integer_type_node, NULL_TREE));
  TREE_PUBLIC (cfstring_class_reference) = 1;
  TREE_USED (cfstring_class_reference) = 1;
  DECL_ARTIFICIAL (cfstring_class_reference) = 1;
  (*lang_hooks.decls.pushdecl) (cfstring_class_reference);
  DECL_EXTERNAL (cfstring_class_reference) = 1;
  rest_of_decl_compilation (cfstring_class_reference, 0, 0);
  
  /* Initialize the hash table used to hold the constant CFString objects.  */
  cfstring_htab = htab_create_ggc (31, cfstring_hash,
				   cfstring_eq, NULL);
}

tree
darwin_expand_tree_builtin (tree function, tree params,
			    tree coerced_params ATTRIBUTE_UNUSED)
{
  unsigned int fcode = DECL_FUNCTION_CODE (function);

  switch (fcode)
    {
    case DARWIN_BUILTIN_CFSTRINGMAKECONSTANTSTRING:
      if (!darwin_constant_cfstrings)
	{
	  error ("built-in function `%s' requires `-fconstant-cfstrings' flag",
		 IDENTIFIER_POINTER (DECL_NAME (function)));
	  return error_mark_node;
	}

      return darwin_build_constant_cfstring (TREE_VALUE (params));
    default:
      break;
    }

  return NULL_TREE;
}

static hashval_t
cfstring_hash (const void *ptr)
{
  tree str = ((struct cfstring_descriptor *)ptr)->literal;
  const unsigned char *p = (const unsigned char *) TREE_STRING_POINTER (str);
  int i, len = TREE_STRING_LENGTH (str);
  hashval_t h = len;

  for (i = 0; i < len; i++)
    h = ((h * 613) + p[i]);

  return h;
}

static int
cfstring_eq (const void *ptr1, const void *ptr2)
{
  tree str1 = ((struct cfstring_descriptor *)ptr1)->literal;
  tree str2 = ((struct cfstring_descriptor *)ptr2)->literal;
  int len1 = TREE_STRING_LENGTH (str1);

  return (len1 == TREE_STRING_LENGTH (str2)
	  && !memcmp (TREE_STRING_POINTER (str1), TREE_STRING_POINTER (str2),
		      len1));
}

tree
darwin_construct_objc_string (tree str)
{
  if (!darwin_constant_cfstrings)
  /* APPLE LOCAL begin 4080358 */
    {
      /* Even though we are not using CFStrings, place our literal
	 into the cfstring_htab hash table, so that the
	 darwin_constant_cfstring_p() function below will see it.  */
      struct cfstring_descriptor key;
      void **loc;

      key.literal = str;
      loc = htab_find_slot (cfstring_htab, &key, INSERT);

      if (!*loc)
	{
	  /* APPLE LOCAL radar 4563012 */
	  *loc = ggc_alloc_cleared (sizeof (struct cfstring_descriptor));
	  ((struct cfstring_descriptor *)*loc)->literal = str;
	}

      return NULL_TREE;  /* Fall back to NSConstantString.  */
    }

  /* APPLE LOCAL end 4080358 */
  return darwin_build_constant_cfstring (str);
}

bool
darwin_constant_cfstring_p (tree str)
{
  struct cfstring_descriptor key;
  void **loc;

  if (!str)
    return false;

  STRIP_NOPS (str);

  if (TREE_CODE (str) == ADDR_EXPR)
    str = TREE_OPERAND (str, 0);

  if (TREE_CODE (str) != STRING_CST)
    return false;

  key.literal = str;
  loc = htab_find_slot (cfstring_htab, &key, NO_INSERT);
  
  if (loc)
    return true;

  return false;
}

static tree
darwin_build_constant_cfstring (tree str)
{
  struct cfstring_descriptor *desc, key;
  void **loc;
  tree addr;

  if (!str)
    goto invalid_string;

  STRIP_NOPS (str);

  if (TREE_CODE (str) == ADDR_EXPR)
    str = TREE_OPERAND (str, 0);

  if (TREE_CODE (str) != STRING_CST)
    {
     invalid_string:
      error ("CFString literal expression is not constant");
      return error_mark_node;
    }

  /* Perhaps we already constructed a constant CFString just like this one? */
  key.literal = str;
  loc = htab_find_slot (cfstring_htab, &key, INSERT);
  desc = *loc;

  if (!desc)
    {
      tree initlist, constructor, field = TYPE_FIELDS (ccfstring_type_node);
      tree var;
      int length = TREE_STRING_LENGTH (str) - 1;
      /* FIXME: The CFString functionality should probably reside
	 in darwin-c.c.  */
      extern tree pushdecl_top_level (tree);
      /* APPLE LOCAL begin radar 2996215 */
      extern int isascii (int);
      bool cvt_utf = false;
      tree utf16_str = NULL_TREE;
      const char *s = TREE_STRING_POINTER (str);
      int l;
      for (l = 0; l < length; l++)
        if (!s[l] || !isascii (s[l]))
          {
            cvt_utf = true;
            break;
          }
      if (cvt_utf)
        {
          size_t numUniChars;
          const unsigned char *inbuf = (unsigned char *)TREE_STRING_POINTER (str);
          utf16_str = create_init_utf16_var (inbuf, length, &numUniChars);
          if (!utf16_str)
            {
              warning (0, "input conversion stopped due to an input byte "
                          "that does not belong to the input codeset UTF-8");
              cvt_utf = false; /* fall thru */
            }
          else
            length = (numUniChars >> 1);
        }
      /* APPLE LOCAL end radar 2996215 */
      *loc = desc = ggc_alloc (sizeof (*desc));
      desc->literal = str;

      initlist = build_tree_list
		 (field, build1 (ADDR_EXPR, pcint_type_node, 
				 cfstring_class_reference));
      field = TREE_CHAIN (field);
      /* APPLE LOCAL radar 2996215 */
      initlist = tree_cons (field, build_int_cst (NULL_TREE, utf16_str ? 0x000007d0 : 0x000007c8),
			    initlist);
      field = TREE_CHAIN (field);
      initlist = tree_cons (field,
			    build1 (ADDR_EXPR, pcchar_type_node,
				    /* APPLE LOCAL radar 2996215 */
				    utf16_str ? utf16_str : str), initlist);
      field = TREE_CHAIN (field);
      /* APPLE LOCAL radar 4493912 */
      initlist = tree_cons (field, build_int_cst (TREE_TYPE (field), length),
			    initlist);

      constructor = build_constructor_from_list (ccfstring_type_node,
						 nreverse (initlist));
      TREE_READONLY (constructor) = 1;
      TREE_CONSTANT (constructor) = 1;
      TREE_STATIC (constructor) = 1;

      /* Fromage: The C++ flavor of 'build_unary_op' expects constructor nodes
	 to have the TREE_HAS_CONSTRUCTOR (...) bit set.  However, this file is
	 being built without any knowledge of C++ tree accessors; hence, we shall
	 use the generic accessor that TREE_HAS_CONSTRUCTOR actually maps to!  */
      if (darwin_running_cxx)
	TREE_LANG_FLAG_4 (constructor) = 1;   /* TREE_HAS_CONSTRUCTOR  */

      /* Create an anonymous global variable for this CFString.  */
      var = build_decl (CONST_DECL, NULL, TREE_TYPE (constructor));
      DECL_INITIAL (var) = constructor;
      TREE_STATIC (var) = 1;
      DECL_IGNORED_P (var) = 1;
      pushdecl_top_level (var);
      desc->constructor = var;
    }

  addr = build1 (ADDR_EXPR, pccfstring_type_node, desc->constructor);
  TREE_CONSTANT (addr) = 1;

  return addr;
}
/* APPLE LOCAL end constant cfstrings */

/* APPLE LOCAL begin CW asm blocks */
/* Assume labels like L_foo$stub etc in CW-style inline code are
   intended to be taken as literal labels, and return the identifier,
   otherwise return NULL signifying that we have no special
   knowledge.  */
tree
darwin_iasm_special_label (tree id)
{
  const char *name = IDENTIFIER_POINTER (id);

  if (name[0] == 'L')
    {
      int len = strlen (name);

      if ((len > 5 && strcmp (name + len - 5, "$stub") == 0)
	  || (len > 9 && strcmp (name + len - 9, "$lazy_ptr") == 0)
	  || (len > 13 && strcmp (name + len - 13, "$non_lazy_ptr") == 0))
	return id;
    }

  return NULL_TREE;
}
/* APPLE LOCAL end CW asm blocks */

#if 0
/* See TARGET_ASM_OUTPUT_ANCHOR for why we can't do this yet.  */
/* The Darwin's implementation of TARGET_ASM_OUTPUT_ANCHOR.  Define the
   anchor relative to ".", the current section position.  We cannot use
   the default one because ASM_OUTPUT_DEF is wrong for Darwin.  */

void
darwin_asm_output_anchor (rtx symbol)
{
  fprintf (asm_out_file, "\t.set\t");
  assemble_name (asm_out_file, XSTR (symbol, 0));
  fprintf (asm_out_file, ", . + " HOST_WIDE_INT_PRINT_DEC "\n",
	   SYMBOL_REF_BLOCK_OFFSET (symbol));
}
#endif

/* Set the darwin specific attributes on TYPE.  */
void
darwin_set_default_type_attributes (tree type)
{
  if (darwin_ms_struct
      && TREE_CODE (type) == RECORD_TYPE)
    TYPE_ATTRIBUTES (type) = tree_cons (get_identifier ("ms_struct"),
                                        NULL_TREE,
                                        TYPE_ATTRIBUTES (type));
}

/* True, iff we're generating code for loadable kernel extentions.  */

bool
darwin_kextabi_p (void) {
  /* APPLE LOCAL kext v2 */
  return TARGET_KEXTABI == 1;
}

/* APPLE LOCAL begin kext v2 */
#ifndef TARGET_SUPPORTS_KEXTABI1
#define TARGET_SUPPORTS_KEXTABI1 0
#endif
/* APPLE LOCAL end kext v2 */

void
darwin_override_options (void)
{
  /* APPLE LOCAL begin for iframework for 4.3 4094959 */
  /* Remove this: */
#if 0
  if (flag_apple_kext && strcmp (lang_hooks.name, "GNU C++") != 0)
    {
      warning (0, "command line option %<-fapple-kext%> is only valid for C++");
      flag_apple_kext = 0;
    }
#endif
  /* APPLE LOCAL end for iframework for 4.3 4094959 */
  if (flag_mkernel || flag_apple_kext)
    {
      /* -mkernel implies -fapple-kext for C++ */
      if (strcmp (lang_hooks.name, "GNU C++") == 0)
	flag_apple_kext = 1;

      flag_no_common = 1;

      /* No EH in kexts.  */
      flag_exceptions = 0;
      /* APPLE LOCAL 5628030 */
      flag_asynchronous_unwind_tables = 0;
      /* No -fnon-call-exceptions data in kexts.  */
      flag_non_call_exceptions = 0;
      /* APPLE LOCAL begin kext v2 */
      if (flag_apple_kext &&
	  ! TARGET_SUPPORTS_KEXTABI1)
	flag_apple_kext = 2;
      /* APPLE LOCAL end kext v2 */
    }
  /* APPLE LOCAL begin axe stubs 5571540 */
  /* APPLE LOCAL begin ARM 5683689 */

  /* Go ahead and generate stubs for old systems, just in case.  */
  if (darwin_macosx_version_min
      && strverscmp (darwin_macosx_version_min, "10.5") < 0)
    darwin_stubs = true;
  /* APPLE LOCAL end ARM 5683689 */
  /* APPLE LOCAL end axe stubs 5571540 */
  /* APPLE LOCAL begin stack-protector default 5095227 */
  /* Default flag_stack_protect to 1 if on 10.5 or later for user code,
     or 10.6 or later for code identified as part of the kernel.  */
  /* LLVM LOCAL begin - Don't enable stack protectors by default for Leopard. */
#ifndef ENABLE_LLVM
  if (flag_stack_protect == -1
      && darwin_macosx_version_min
      && ((! flag_mkernel && ! flag_apple_kext
	   && strverscmp (darwin_macosx_version_min, "10.5") >= 0)
	  || strverscmp (darwin_macosx_version_min, "10.6") >= 0))
    flag_stack_protect = 1;
#else
  if (flag_stack_protect == -1 && darwin_macosx_version_min)
    {
      if (strverscmp (darwin_macosx_version_min, "10.6") >= 0)
        flag_stack_protect = 1;
      else if (strverscmp (darwin_macosx_version_min, "10.5") >= 0)
        flag_stack_protect = 0;
    }
#endif
  /* LLVM LOCAL end - Don't enable stack protectors by default for Leopard. */
  /* APPLE LOCAL end stack-protector default 5095227 */
/* APPLE LOCAL diff confuses me */
}
/* APPLE LOCAL begin radar 4985544 */
bool
darwin_cfstring_type_node (tree type_node)
{
  return type_node == ccfstring_type_node;
}
/* APPLE LOCAL end radar 4985544 */

/* LLVM LOCAL begin radar 6230142 */
unsigned darwin_llvm_override_target_version(const char *triple, char **new_triple) {
  int len = 0, pad1 = 0, pad2 = 0, version = 0;
  char *substr;

  if (!darwin_macosx_version_min)
    return 0;
  
  /* Triple string is expected to look something like 'i386-*-darwin?' or
     'i386-*-darwin9.5.0'  */
  substr = strstr(triple, "darwin");
  if (!substr)
    return 0;
  len = substr + 6 - triple;

  /* llvm-gcc doesn't support pre-10.0 systems. */
  version = strverscmp (darwin_macosx_version_min, "10.0");
  if (version < 0)
    return 0;

  /* 10.0 is darwin4. */
  version += 4;

  /* Darwin version number will be 2 digits for 10.6 and up.  */
  if (version >= 10)
    pad1 = 1;

  /* If darwin_macosx_version_min is something like 10.4.9, we need to append
     the .9 to the new triple. */
  substr = strchr(darwin_macosx_version_min, '.');
  if (!substr)
    return 0;
  substr = strchr(substr+1, '.');
  if (substr)
    pad2 = strlen(substr);
  
  *new_triple = ggc_alloc (len+pad1+pad2+1);
  strncpy (*new_triple, triple, len);
  if (version >= 10)
    {
      (*new_triple)[len] = '1';
      version -= 10;
      ++len;
    }
  (*new_triple)[len] = '0' + version;
  if (substr)
    {
      int i;
      for (i = 0; i < pad2; ++i)
        (*new_triple)[len+1+i] = substr[i];
      len += pad2;
    }
  (*new_triple)[len+1] = '\0';
  
  return 1;
}
/* LLVM LOCAL end radar 6230142 */
#include "gt-darwin.h"
