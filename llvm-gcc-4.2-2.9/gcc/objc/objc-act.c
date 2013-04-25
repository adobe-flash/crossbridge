/* Implement classes and message passing for Objective C.
   Copyright (C) 1992, 1993, 1994, 1995, 1997, 1998, 1999, 2000,
   2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.
   Contributed by Steve Naroff.

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

/* Purpose: This module implements the Objective-C 4.0 language.

   compatibility issues (with the Stepstone translator):

   - does not recognize the following 3.3 constructs.
     @requires, @classes, @messages, = (...)
   - methods with variable arguments must conform to ANSI standard.
   - tagged structure definitions that appear in BOTH the interface
     and implementation are not allowed.
   - public/private: all instance variables are public within the
     context of the implementation...I consider this to be a bug in
     the translator.
   - statically allocated objects are not supported. the user will
     receive an error if this service is requested.

   code generation `options':

   */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "rtl.h"
#include "tm_p.h"
#include "expr.h"

#ifdef OBJCPLUS
#include "cp-tree.h"
#else
#include "c-tree.h"
#endif

#include "c-common.h"
#include "c-pragma.h"
#include "flags.h"
#include "langhooks.h"
#include "objc-act.h"
#include "input.h"
#include "except.h"
#include "function.h"
#include "output.h"
#include "toplev.h"
#include "ggc.h"
#include "varray.h"
#include "debug.h"
#include "target.h"
#include "diagnostic.h"
#include "cgraph.h"
#include "tree-iterator.h"
#include "libfuncs.h"
#include "hashtab.h"
#include "langhooks-def.h"
/* APPLE LOCAL optimization pragmas 3124235/3420242 */
#include "opts.h"
/* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
#include "llvm.h"               /* for reset_initializer_llvm */
#endif
/* LLVM LOCAL end */

#define OBJC_VOID_AT_END	void_list_node

/* APPLE LOCAL radar 4506893 */
static bool in_objc_property_setter_name_context = false;
static unsigned int should_call_super_dealloc = 0;
/* APPLE LOCAL begin radar 4757423 */
static bool should_call_super_finalize = false;
/* APPLE LOCAL end radar 4757423 */

/* When building Objective-C++, we are not linking against the C front-end
   and so need to replicate the C tree-construction functions in some way.  */
#ifdef OBJCPLUS
#define OBJCP_REMAP_FUNCTIONS
#include "objcp-decl.h"
#endif  /* OBJCPLUS */

/* This is the default way of generating a method name.  */
/* I am not sure it is really correct.
   Perhaps there's a danger that it will make name conflicts
   if method names contain underscores. -- rms.  */
#ifndef OBJC_GEN_METHOD_LABEL
#define OBJC_GEN_METHOD_LABEL(BUF, IS_INST, CLASS_NAME, CAT_NAME, SEL_NAME, NUM) \
  do {					    \
    char *temp;				    \
    sprintf ((BUF), "_%s_%s_%s_%s",	    \
	     ((IS_INST) ? "i" : "c"),	    \
	     (CLASS_NAME),		    \
	     ((CAT_NAME)? (CAT_NAME) : ""), \
	     (SEL_NAME));		    \
    for (temp = (BUF); *temp; temp++)	    \
      if (*temp == ':') *temp = '_';	    \
  } while (0)
#endif
/* APPLE LOCAL begin radar 4862848 */
#ifndef OBJC_FLAG_OBJC_ABI
#define OBJC_FLAG_OBJC_ABI \
  do { if (flag_objc_abi == -1)   \
         flag_objc_abi = 1;   \
  } while (0)
#endif
/* APPLE LOCAL end radar 4862848 */
/* APPLE LOCAL begin radar 5023725 */
#ifndef OBJC_FLAG_ZEROCOST_EXCEPTIONS
#define OBJC_FLAG_ZEROCOST_EXCEPTIONS
#endif
/* APPLE LOCAL end radar 5023725 */
/* LLVM LOCAL begin */
/* APPLE LOCAL begin radar 4590191 */
#ifndef OBJC_FLAG_SJLJ_EXCEPTIONS
#define OBJC_FLAG_SJLJ_EXCEPTIONS
#endif
/* APPLE LOCAL end radar 4590191 */
/* LLVM LOCAL end */
/* APPLE LOCAL begin radar 4531086 */
#ifndef OBJC_WARN_OBJC2_FEATURES
#define OBJC_WARN_OBJC2_FEATURES(MESSAGE)
#endif
/* APPLE LOCAL end radar 4531086 */

/* APPLE LOCAL begin radar 6307941 */
#ifndef OBJC2_ABI_DISPATCH
#define OBJC2_ABI_DISPATCH 0
#endif
/* APPLE LOCAL end radar 6307941 */

/* These need specifying.  */
#ifndef OBJC_FORWARDING_STACK_OFFSET
#define OBJC_FORWARDING_STACK_OFFSET 0
#endif

#ifndef OBJC_FORWARDING_MIN_OFFSET
#define OBJC_FORWARDING_MIN_OFFSET 0
#endif

/* Set up for use of obstacks.  */

#include "obstack.h"

/* This obstack is used to accumulate the encoding of a data type.  */
static struct obstack util_obstack;

/* This points to the beginning of obstack contents, so we can free
   the whole contents.  */
char *util_firstobj;

/* The version identifies which language generation and runtime
   the module (file) was compiled for, and is recorded in the
   module descriptor.  */

/* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
/* Bump version number to 7 */
/* APPLE LOCAL mainline */
#define OBJC_VERSION    (flag_next_runtime ? 7 : 8)
/* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */
#define PROTOCOL_VERSION 2
/* APPLE LOCAL radar 4533974 - ObjC newprotocol */
#define NEW_PROTOCOL_VERSION 3

/* (Decide if these can ever be validly changed.) */
#define OBJC_ENCODE_INLINE_DEFS 	0
#define OBJC_ENCODE_DONT_INLINE_DEFS	1

/*** Private Interface (procedures) ***/

/* Used by compile_file.  */

static void init_objc (void);
static void finish_objc (void);

/* Code generation.  */

static tree objc_build_constructor (tree, tree);
/* APPLE LOCAL radar 6307941 */
static tree build_objc_method_call (int, tree, tree, tree, tree, tree);
static tree get_proto_encoding (tree);
static tree lookup_interface (tree);
static tree objc_add_static_instance (tree, tree);
/* APPLE LOCAL radar 5168496 */
static bool objc_is_object_id (tree);

static tree start_class (enum tree_code, tree, tree, tree);
static tree continue_class (tree);
static void finish_class (tree);
static void start_method_def (tree);
/* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
#ifdef OBJCPLUS
static void objc_start_function (tree, tree, tree, tree, tree);
#else
static void objc_start_function (tree, tree, tree, struct c_arg_info *, tree);
#endif
/* APPLE LOCAL end radar 5839812 - location for synthesized methods  */
/* APPLE LOCAL radar 4947311 - protocol attributes */
static tree start_protocol (tree, tree, tree);
static tree build_method_decl (enum tree_code, tree, tree, tree, bool);
/* APPLE LOCAL C* language */
static tree objc_add_method (tree, tree, int, int);
static tree add_instance_variable (tree, int, tree);
static tree build_ivar_reference (tree);
static tree is_ivar (tree, tree);

/* APPLE LOCAL begin objc new property */
static bool objc_lookup_protocol (tree, tree, tree, bool);
/* APPLE LOCAL radar 5277239 */
static tree lookup_method_static (tree, tree, int, bool, bool);
/* APPLE LOCAL radar 4817072 */
static void objc_lookup_property_ivar (tree, tree);
/* APPLE LOCAL end objc new property */
static void build_objc_exception_stuff (void);
static void build_next_objc_exception_stuff (void);
/* APPLE LOCAL radar 5285911 */
static tree nested_ivar_lookup (tree, tree);

/* APPLE LOCAL radar 4431864 */
static void objc_set_global_decl_fields (tree);
/* APPLE LOCAL begin ObjC GC */
static int objc_is_strong_p (tree);
static int objc_is_gcable_p (tree);
/* APPLE LOCAL end ObjC GC */

/* We only need the following for ObjC; ObjC++ will use C++'s definition
   of DERIVED_FROM_P.  */
#ifndef OBJCPLUS
static bool objc_derived_from_p (tree, tree);
#define DERIVED_FROM_P(PARENT, CHILD) objc_derived_from_p (PARENT, CHILD)
#endif
/* APPLE LOCAL begin C* language */
/* code removed */
/* APPLE LOCAL end C* language */
/* APPLE LOCAL begin C* property (Radar 4436866) */
static char *objc_build_property_setter_name (tree, bool);
static int match_proto_with_proto (tree, tree, int);
static char * objc_build_property_ivar_name (tree);
/* APPLE LOCAL end C* property (Radar 4436866) */
static void objc_xref_basetypes (tree, tree);
/* APPLE LOCAL radar 5376125 */
static void objc_warn_direct_ivar_access (tree, tree);

/* APPLE LOCAL begin ObjC new abi */
static void build_v2_class_template (void);
static void build_v2_protocol_template (void);
static void build_message_ref_template (void);
/* APPLE LOCAL radar 6307941 */
static tree build_v2_build_objc_method_call (int, tree, tree, tree, tree, tree, bool);
/* APPLE LOCAL radar 4695109 */
static void build_v2_protocol_reference (tree);
/* APPLE LOCAL radar 5153561 */
static void init_UOBJC2_EHTYPE_decls (void);
static void create_ivar_offset_name (char *, tree, tree);
static void build_v2_category_template (void);
/* APPLE LOCAL radar 4533974 - ObjC new protocol - radar 6351990 */
static tree build_protocollist_reference_decl (tree);
static void objc_add_to_category_list_chain (tree);
static void objc_add_to_nonlazy_category_list_chain (tree);
/* APPLE LOCAL begin radar 4441049 */
static void hash_name_enter (hash *, tree);
static tree hash_name_lookup (hash *, tree);
/* APPLE LOCAL end radar 4441049 */
/* APPLE LOCAL end ObjC new abi */
/* APPLE LOCAL C* language */
static void build_objc_fast_enum_state_type (void);

static void build_class_template (void);
static void build_selector_template (void);
static void build_category_template (void);
static void build_super_template (void);
/* APPLE LOCAL begin ObjC new abi - radar 4695109 */
static tree build_protocol_initializer (tree, tree, tree, tree, tree, bool, tree, tree, tree);
/* APPLE LOCAL end ObjC new abi - radar 4695109 */

static tree get_class_ivars (tree, bool);
static tree generate_protocol_list (tree);
static void build_protocol_reference (tree);

#ifdef OBJCPLUS
static void objc_generate_cxx_cdtors (void);
#endif
/* APPLE LOCAL begin radar 4359757 */
static size_t hash_func (tree);
static void hash_add_attr (hash, tree);
static hash hash_ident_lookup (hash *, tree);
static hash hash_ident_enter (hash *, tree);
/* APPLE LOCAL end radar 4359757 */

/* APPLE LOCAL radar 3803157 - objc attribute */
static void objc_decl_method_attributes (tree*, tree, int);
static const char *synth_id_with_class_suffix (const char *, tree);

/* APPLE LOCAL begin radar 4548636 */
static void objc_warn_on_class_attributes (tree, bool);
static void objc_attach_attrs_to_type (tree, tree);
/* APPLE LOCAL end radar 4548636 */
/* APPLE LOCAL radar 4664707 */
static tree objc_create_named_tmp_var (tree, const char *);
/* APPLE LOCAL radar 2848255 */
static tree objc2_eh_runtime_type (tree);
/* APPLE LOCAL radar 4982951 */
static tree objc_v2_ivar_reference (tree);
/* APPLE LOCAL radar 5277239 */
static tree receiver_is_class_object (tree, int, int);

/* APPLE LOCAL radar 5040740 */
static tree lookup_nested_method (tree, tree);

/* APPLE LOCAL begin radar 5607453 */
static bool objc_is_object_id (tree);
static bool objc_is_class_id (tree);
/* APPLE LOCAL end radar 5607453 */
/* Hash tables to manage the global pool of method prototypes.  */

hash *nst_method_hash_list = 0;
hash *cls_method_hash_list = 0;
/* APPLE LOCAL begin radar 4359757 */
hash *class_nst_method_hash_list = 0;
hash *class_cls_method_hash_list = 0;
hash *class_names_hash_list = 0;
hash *meth_var_names_hash_list = 0;
hash *meth_var_types_hash_list = 0;
hash *prop_names_attr_hash_list = 0;
hash *sel_ref_hash_list = 0;
/* APPLE LOCAL end radar 4359757 */

/* APPLE LOCAL begin radar 4345837 */
hash *cls_name_hash_list = 0;
hash *als_name_hash_list = 0;
/* APPLE LOCAL end radar 4345837 */

/* APPLE LOCAL radar 4441049 */
hash *ivar_offset_hash_list = 0;

/* APPLE LOCAL begin radar 4345837 */
static void hash_class_name_enter (hash *, tree, tree);
static hash hash_class_name_lookup (hash *, tree);
/* APPLE LOCAL end radar 4345837 */
static hash hash_lookup (hash *, tree);
static tree lookup_method (tree, tree);
/* APPLE LOCAL C* property (Radar 4436866) */
static inline tree lookup_category (tree, tree);

enum string_section
{
  class_names,		/* class, category, protocol, module names */
  meth_var_names,	/* method and variable names */
  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
  meth_var_types,	/* method and variable type descriptors */
  prop_names_attr	/* property names and their attributes. */
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
};

static tree add_objc_string (tree, enum string_section);
/* APPLE LOCAL do not create an unreferenced decl */
static tree build_objc_string_ident (enum string_section);
static void build_selector_table_decl (void);

/* Protocol additions.  */

static tree lookup_protocol (tree);
/* APPLE LOCAL radar 4398221 */
static tree lookup_and_install_protocols (tree, bool);
/* APPLE LOCAL radar 4869979 */
static int conforms_to_protocol (tree, tree);

/* Type encoding.  */

static void encode_type_qualifiers (tree);
static void encode_type (tree, int, int);
static void encode_field_decl (tree, int, int);

#ifdef OBJCPLUS
static void really_start_method (tree, tree);
#else
static void really_start_method (tree, struct c_arg_info *);
#endif
static int comp_proto_with_proto (tree, tree, int);
/* APPLE LOCAL begin 4209854 */
/* APPLE LOCAL radar 6307941 */
static tree get_arg_type_list (tree, tree, int, int);
static tree objc_decay_parm_type (tree);
/* APPLE LOCAL end 4209854 */
static void objc_push_parm (tree);
#ifdef OBJCPLUS
static tree objc_get_parm_info (int);
#else
static struct c_arg_info *objc_get_parm_info (int);
#endif

/* Utilities for debugging and error diagnostics.  */

static void warn_with_method (const char *, int, tree);
static char *gen_type_name (tree);
static char *gen_type_name_0 (tree);
static char *gen_method_decl (tree);
static char *gen_declaration (tree);

/* Everything else.  */

/* APPLE LOCAL begin C* property metadata (Radar 4498373) */
static tree start_var_decl (tree, const char *);
static void finish_var_decl (tree, tree);
/* APPLE LOCAL end C* property metadata (Radar 4498373) */
static tree create_field_decl (tree, const char *);
static void add_class_reference (tree);
static void build_protocol_template (void);
static tree encode_method_prototype (tree);
static void generate_classref_translation_entry (tree);
static void handle_class_ref (tree);
static void generate_struct_by_value_array (void)
     ATTRIBUTE_NORETURN;
static void mark_referenced_methods (void);
static void generate_objc_image_info (void);

/*** Private Interface (data) ***/

/* Reserved tag definitions.  */

#define OBJECT_TYPEDEF_NAME		"id"
#define CLASS_TYPEDEF_NAME		"Class"

#define TAG_OBJECT			"objc_object"
#define TAG_CLASS			"objc_class"
#define TAG_SUPER			"objc_super"
#define TAG_SELECTOR			"objc_selector"

#define UTAG_CLASS			"_objc_class"
#define UTAG_IVAR			"_objc_ivar"
#define UTAG_IVAR_LIST			"_objc_ivar_list"
#define UTAG_METHOD			"_objc_method"
#define UTAG_METHOD_LIST		"_objc_method_list"
#define UTAG_CATEGORY			"_objc_category"
#define UTAG_MODULE			"_objc_module"
#define UTAG_SYMTAB			"_objc_symtab"
#define UTAG_SUPER			"_objc_super"
#define UTAG_SELECTOR			"_objc_selector"

#define UTAG_PROTOCOL			"_objc_protocol"
#define UTAG_METHOD_PROTOTYPE		"_objc_method_prototype"
#define UTAG_METHOD_PROTOTYPE_LIST	"_objc__method_prototype_list"

/* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
#define UTAG_CLASS_EXT			"_objc_class_ext"
#define UTAG_PROPERTY_LIST		"_prop_list_t"
#define UTAG_PROTOCOL_EXT		"_objc_protocol_extension"
/* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */

/* Note that the string object global name is only needed for the
   NeXT runtime.  */
#define STRING_OBJECT_GLOBAL_FORMAT	"_%sClassReference"
/* APPLE LOCAL radar 4719165 */
#define STRING_V2_OBJECT_GLOBAL_FORMAT     "OBJC_CLASS_$_%s"
/* APPLE LOCAL radar 4894756 */
#define STRING_V2_IVAR_OFFSET_PREFIX	   "OBJC_IVAR_$_"
/* APPLE LOCAL radar 5008110 */
#define STRING_V2_OBJC_EHTYPE_PREFIX    "OBJC_EHTYPE_$"

#define PROTOCOL_OBJECT_CLASS_NAME	"Protocol"

/* APPLE LOCAL begin ObjC new abi */
#define UTAG_V2_CLASS			"_class_t"
#define UTAG_V2_CLASS_RO			"_class_ro_t"
#define UTAG_V2_PROTOCOL_LIST		"_protocol_list_t"
#define UTAG_V2_PROTOCOL			"_protocol_t"
/* APPLE LOCAL end ObjC new abi */

static const char *TAG_GETCLASS;
static const char *TAG_GETMETACLASS;
static const char *TAG_MSGSEND;
static const char *TAG_MSGSENDSUPER;
/* The NeXT Objective-C messenger may have two extra entry points, for use
   when returning a structure. */
static const char *TAG_MSGSEND_STRET;
static const char *TAG_MSGSENDSUPER_STRET;
static const char *default_constant_string_class_name;
/* APPLE LOCAL radar 4666559 */
static const struct gcc_debug_hooks *save_default_debug_hooks;
/* APPLE LOCAL radar 5932809 - copyable byref blocks */
static int donot_warn_missing_methods;

/* Runtime metadata flags.  */
#define CLS_FACTORY			0x0001L
#define CLS_META			0x0002L
/* APPLE LOCAL radar 5142207 */
#define IS_CLS_META(X)			((X & 0x0003) == CLS_META)
#define CLS_HAS_CXX_STRUCTORS		0x2000L
/* APPLE LOCAL radar 4923634 */
#define OBJC2_CLS_HAS_CXX_STRUCTORS	0x0004L
/* APPLE LOCAL begin radar 5142207 */
#define CLS_HIDDEN			0x20000
#define OBJC2_CLS_HIDDEN		0x10
/* APPLE LOCAL end radar 5142207 */

#define OBJC_MODIFIER_STATIC		0x00000001
#define OBJC_MODIFIER_FINAL		0x00000002
#define OBJC_MODIFIER_PUBLIC		0x00000004
#define OBJC_MODIFIER_PRIVATE		0x00000008
#define OBJC_MODIFIER_PROTECTED		0x00000010
#define OBJC_MODIFIER_NATIVE		0x00000020
#define OBJC_MODIFIER_SYNCHRONIZED	0x00000040
#define OBJC_MODIFIER_ABSTRACT		0x00000080
#define OBJC_MODIFIER_VOLATILE		0x00000100
#define OBJC_MODIFIER_TRANSIENT		0x00000200
#define OBJC_MODIFIER_NONE_SPECIFIED	0x80000000

/* NeXT-specific tags.  */

#define TAG_MSGSEND_NONNIL		"objc_msgSendNonNil"
#define TAG_MSGSEND_NONNIL_STRET	"objc_msgSendNonNil_stret"
#define TAG_EXCEPTIONEXTRACT		"objc_exception_extract"
#define TAG_EXCEPTIONTRYENTER		"objc_exception_try_enter"
#define TAG_EXCEPTIONTRYEXIT		"objc_exception_try_exit"
#define TAG_EXCEPTIONMATCH		"objc_exception_match"
#define TAG_EXCEPTIONTHROW		"objc_exception_throw"
#define TAG_SYNCENTER			"objc_sync_enter"
#define TAG_SYNCEXIT			"objc_sync_exit"
#define TAG_SETJMP			"_setjmp"
#define UTAG_EXCDATA			"_objc_exception_data"
/* APPLE LOCAL radar 4280641 */
#define TAG_MSGSEND_FPRET		"objc_msgSend_fpret"

/* APPLE LOCAL begin radar 4426814 */
#define TAG_ASSIGN_WEAK			"objc_assign_weak"
#define TAG_READ_WEAK			"objc_read_weak"
/* APPLE LOCAL end radar 4426814 */
/* APPLE LOCAL radar 3742561 */
#define TAG_MEMMOVE_COLLECTABLE		"objc_memmove_collectable"

#define TAG_ASSIGNIVAR			"objc_assign_ivar"
#define TAG_ASSIGNGLOBAL		"objc_assign_global"
#define TAG_ASSIGNSTRONGCAST		"objc_assign_strongCast"

/* Branch entry points.  All that matters here are the addresses;
   functions with these names do not really exist in libobjc.  */

#define TAG_MSGSEND_FAST		"objc_msgSend_Fast"
#define TAG_ASSIGNIVAR_FAST		"objc_assign_ivar_Fast"

/* APPLE LOCAL begin radar 4590221 */
#ifndef OFFS_MSGSEND_FAST
#define OFFS_MSGSEND_FAST 0
#endif
#ifndef OFFS_ASSIGNIVAR_FAST
#define OFFS_ASSIGNIVAR_FAST 0
#endif
/* APPLE LOCAL end radar 4590221 */

#define TAG_CXX_CONSTRUCT		".cxx_construct"
#define TAG_CXX_DESTRUCT		".cxx_destruct"

/* GNU-specific tags.  */

#define TAG_EXECCLASS			"__objc_exec_class"
#define TAG_GNUINIT			"__objc_gnu_init"

/* Flags for lookup_method_static().  */
#define OBJC_LOOKUP_CLASS	1	/* Look for class methods.  */
#define OBJC_LOOKUP_NO_SUPER	2	/* Do not examine superclasses.  */

/* The OCTI_... enumeration itself is in objc/objc-act.h.  */
tree objc_global_trees[OCTI_MAX];

static void handle_impent (struct imp_entry *);

struct imp_entry *imp_list = 0;
int imp_count = 0;	/* `@implementation' */
int cat_count = 0;	/* `@category' */

enum tree_code objc_inherit_code;
int objc_public_flag;

/* APPLE LOCAL C* language */
static int objc_method_optional_flag = 0;
/* APPLE LOCAL begin C* property (Radar 4436866, 4591909) */
static bool property_readonly;
static tree property_getter;
static tree property_setter;
/* APPLE LOCAL end C* property (Radar 4436866, 4591909) */
/* APPLE LOCAL begin objc new property */
static bool property_readwrite;
static bool property_assign;
static bool property_retain;
static bool property_copy;
/* APPLE LOCAL end objc new property */
/* APPLE LOCAL radar 4947014 - objc atomic property */
static bool property_atomic;
/* Use to generate method labels.  */
static int method_slot = 0;

/* APPLE LOCAL begin radar 4291785 */
static int objc_collecting_ivars = 0;
/* APPLE LOCAL end radar 4291785 */

#define BUFSIZE		1024

static char *errbuf;	/* Buffer for error diagnostics */

/* Data imported from tree.c.  */

extern enum debug_info_type write_symbols;

/* Data imported from toplev.c.  */

extern const char *dump_base_name;
/* APPLE LOCAL radar 5130983 */
int lvalue_or_else (tree*, enum lvalue_use);

static int flag_typed_selectors;

/* Store all constructed constant strings in a hash table so that
   they get uniqued properly.  */

struct string_descriptor GTY(())
{
  /* The literal argument .  */
  tree literal;

  /* The resulting constant string.  */
  tree constructor;
};

static GTY((param_is (struct string_descriptor))) htab_t string_htab;

/* APPLE LOCAL begin radar 4204796 */
/* struct volatilized_type and volatilized_htab removed */
/* APPLE LOCAL end radar 4204796 */

FILE *gen_declaration_file;

/* APPLE LOCAL begin radar 5002848 */
/* Tells "encode_pointer" that we are encoding for a property type. */
static int generating_property_type_encoding = 0;
/* APPLE LOCAL end radar 5002848 */

/* Tells "encode_pointer/encode_aggregate" whether we are generating
   type descriptors for instance variables (as opposed to methods).
   Type descriptors for instance variables contain more information
   than methods (for static typing and embedded structures).  */

static int generating_instance_variables = 0;

/* Some platforms pass small structures through registers versus
   through an invisible pointer.  Determine at what size structure is
   the transition point between the two possibilities.  */

static void
generate_struct_by_value_array (void)
{
  tree type;
  tree field_decl, field_decl_chain;
  int i, j;
  int aggregate_in_mem[32];
  int found = 0;

  /* Presumably no platform passes 32 byte structures in a register.  */
  for (i = 1; i < 32; i++)
    {
      char buffer[5];

      /* Create an unnamed struct that has `i' character components */
      type = start_struct (RECORD_TYPE, NULL_TREE);

      strcpy (buffer, "c1");
      field_decl = create_field_decl (char_type_node,
				      buffer);
      field_decl_chain = field_decl;

      for (j = 1; j < i; j++)
	{
	  sprintf (buffer, "c%d", j + 1);
	  field_decl = create_field_decl (char_type_node,
					  buffer);
	  chainon (field_decl_chain, field_decl);
	}
      finish_struct (type, field_decl_chain, NULL_TREE);

      aggregate_in_mem[i] = aggregate_value_p (type, 0);
      if (!aggregate_in_mem[i])
	found = 1;
    }

  /* We found some structures that are returned in registers instead of memory
     so output the necessary data.  */
  if (found)
    {
      for (i = 31; i >= 0;  i--)
	if (!aggregate_in_mem[i])
	  break;
      printf ("#define OBJC_MAX_STRUCT_BY_VALUE %d\n\n", i);

      /* The first member of the structure is always 0 because we don't handle
	 structures with 0 members */
      printf ("static int struct_forward_array[] = {\n  0");

      for (j = 1; j <= i; j++)
	printf (", %d", aggregate_in_mem[j]);
      printf ("\n};\n");
    }

  exit (0);
}

bool
objc_init (void)
{
/* APPLE LOCAL begin ARM 5726269 */
#ifdef OBJC_TARGET_FLAG_OBJC_ABI
  OBJC_TARGET_FLAG_OBJC_ABI;
#endif
/* APPLE LOCAL end ARM 5726269 */
  /* APPLE LOCAL radar 4862848 */
  OBJC_FLAG_OBJC_ABI;
  /* APPLE LOCAL begin radar 4531086 */
  if (flag_objc_abi == 2)
    OBJC_WARN_OBJC2_FEATURES ("the new objc abi");
  /* APPLE LOCAL end radar 4531086 */
#ifdef OBJCPLUS
  if (cxx_init () == false)
#else
  if (c_objc_common_init () == false)
#endif
    return false;

#ifndef USE_MAPPED_LOCATION
  /* Force the line number back to 0; check_newline will have
     raised it to 1, which will make the builtin functions appear
     not to be built in.  */
  input_line = 0;
#endif

  /* If gen_declaration desired, open the output file.  */
  if (flag_gen_declaration)
    {
      register char * const dumpname = concat (dump_base_name, ".decl", NULL);
      gen_declaration_file = fopen (dumpname, "w");
      if (gen_declaration_file == 0)
	fatal_error ("can't open %s: %m", dumpname);
      free (dumpname);
    }

  if (flag_next_runtime)
    {
      /* APPLE LOCAL begin ARM hybrid objc-2.0 */
      /* APPLE LOCAL radar 6307941 */
      /* code removed. */
      TAG_GETCLASS = "objc_getClass";
      TAG_GETMETACLASS = "objc_getMetaClass";
      TAG_MSGSEND = "objc_msgSend";
      /* APPLE LOCAL begin radar 6307941 */
      TAG_MSGSENDSUPER = flag_objc_abi==2 ? "objc_msgSendSuper2"
					  : "objc_msgSendSuper";
      TAG_MSGSEND_STRET = "objc_msgSend_stret";
      TAG_MSGSENDSUPER_STRET = flag_objc_abi==2 ? "objc_msgSendSuper2_stret"
						: "objc_msgSendSuper_stret";
      /* APPLE LOCAL end radar 6307941 */
      /* APPLE LOCAL end ARM hybrid objc-2.0 */
      default_constant_string_class_name = "NSConstantString";
      /* APPLE LOCAL begin radar 4810609 */
      if (flag_objc_gc_only)
        flag_objc_gc = 1;
      /* APPLE LOCAL end radar 4810609 */
      /* APPLE LOCAL begin radar 4949034 */
#ifdef OBJCPLUS
      /* APPLE LOCAL begin radar 5809596 */
      /* For all objc ABIs -fobjc-call-cxx-cdtors is on by default. */
      if (flag_objc_call_cxx_cdtors == -1)
        flag_objc_call_cxx_cdtors = 1;
      /* APPLE LOCAL end radar 5809596 */
#endif
      /* APPLE LOCAL end radar 4949034 */
    }
  else
    {
      TAG_GETCLASS = "objc_get_class";
      TAG_GETMETACLASS = "objc_get_meta_class";
      TAG_MSGSEND = "objc_msg_lookup";
      TAG_MSGSENDSUPER = "objc_msg_lookup_super";
      /* GNU runtime does not provide special functions to support
	 structure-returning methods.  */
      default_constant_string_class_name = "NXConstantString";
      flag_typed_selectors = 1;
    }

  init_objc ();

/* APPLE LOCAL begin radar 2848255 - radar 5153561 */
#ifdef OBJCPLUS
  if (flag_objc_zerocost_exceptions)
    {
      lang_eh_runtime_type = objc2_eh_runtime_type;
      init_UOBJC2_EHTYPE_decls ();
    }
#endif
/* APPLE LOCAL end radar 2848255 - radar 5153561 */
  if (print_struct_values)
    generate_struct_by_value_array ();

  return true;
}

void
objc_finish_file (void)
{
  mark_referenced_methods ();

#ifdef OBJCPLUS
  /* We need to instantiate templates _before_ we emit ObjC metadata;
     if we do not, some metadata (such as selectors) may go missing.  */
  /* APPLE LOCAL begin radar 4721858 */
  {
    /* APPLE LOCAL radar 4874626 */
    location_t locus = input_location;
    emit_instantiate_pending_templates (&locus);
  }
  /* APPLE LOCAL end radar 4721858 */
#endif

  /* Finalize Objective-C runtime data.  No need to generate tables
     and code if only checking syntax, or if generating a PCH file.  */
  if (!flag_syntax_only && !pch_file)
    finish_objc ();

  if (gen_declaration_file)
    fclose (gen_declaration_file);

#ifdef OBJCPLUS
  cp_finish_file ();
#endif
}

/* Return the first occurrence of a method declaration corresponding
   to sel_name in rproto_list.  Search rproto_list recursively.
   If is_class is 0, search for instance methods, otherwise for class
   methods.  */
static tree
lookup_method_in_protocol_list (tree rproto_list, tree sel_name,
				int is_class)
{
   tree rproto, p;
   tree fnd = 0;

   for (rproto = rproto_list; rproto; rproto = TREE_CHAIN (rproto))
     {
        p = TREE_VALUE (rproto);

	if (TREE_CODE (p) == PROTOCOL_INTERFACE_TYPE)
	  {
	    /* APPLE LOCAL begin radar 4550413 */
	    fnd = lookup_method (is_class
				 ? PROTOCOL_CLS_METHODS (p)
				 : PROTOCOL_NST_METHODS (p), sel_name);
	    if (!fnd)
	      fnd = lookup_method (is_class
				   ? PROTOCOL_OPTIONAL_CLS_METHODS (p)
				   : PROTOCOL_OPTIONAL_NST_METHODS (p), sel_name);	
	    if (!fnd && PROTOCOL_LIST (p))
	    /* APPLE LOCAL end radar 4550413 */
	      fnd = lookup_method_in_protocol_list (PROTOCOL_LIST (p),
						    sel_name, is_class);
	  }
	else
          {
	    ; /* An identifier...if we could not find a protocol.  */
          }

	if (fnd)
	  return fnd;
     }

   return 0;
}

static tree
lookup_protocol_in_reflist (tree rproto_list, tree lproto)
{
  tree rproto, p;

  /* Make sure the protocol is supported by the object on the rhs.  */
  if (TREE_CODE (lproto) == PROTOCOL_INTERFACE_TYPE)
    {
      tree fnd = 0;
      for (rproto = rproto_list; rproto; rproto = TREE_CHAIN (rproto))
	{
	  p = TREE_VALUE (rproto);

	  if (TREE_CODE (p) == PROTOCOL_INTERFACE_TYPE)
	    {
	      if (lproto == p)
		fnd = lproto;

	      else if (PROTOCOL_LIST (p))
		fnd = lookup_protocol_in_reflist (PROTOCOL_LIST (p), lproto);
	    }

	  if (fnd)
	    return fnd;
	}
    }
  else
    {
      ; /* An identifier...if we could not find a protocol.  */
    }

  return 0;
}

/* APPLE LOCAL begin radar 7865106 */
static bool objc_class_weak_import(tree class)
{
  tree chain;
  gcc_assert (class && TREE_CODE (class) == CLASS_INTERFACE_TYPE);
  for (chain = CLASS_ATTRIBUTES (class); chain; chain = TREE_CHAIN (chain))
    if (is_attribute_p ("weak_import", TREE_PURPOSE (chain)))
      return true;
  return false;
}

static char*
objc_build_weak_reference_internal_classname (tree ident, bool metaclass)
{
  static char string[BUFSIZE];
  sprintf (string, ".weak_reference %s_%s", !metaclass ? "_OBJC_CLASS_$" 
				       : "_OBJC_METACLASS_$", 
			    IDENTIFIER_POINTER (ident));
  return string;
}
/* APPLE LOCAL end radar 7865106 */

void
/* APPLE LOCAL radar 4548636 */
objc_start_class_interface (tree class, tree super_class, tree protos, tree attributes)
{
  objc_interface_context
    = objc_ivar_context
    = start_class (CLASS_INTERFACE_TYPE, class, super_class, protos);
/* APPLE LOCAL begin radar 4548636 */
  CLASS_ATTRIBUTES (objc_interface_context) = attributes;
  objc_warn_on_class_attributes (objc_interface_context, false);
/* APPLE LOCAL end radar 4548636 */
  /* APPLE LOCAL begin radar 7865106 */
  if (flag_objc_abi == 2 && objc_class_weak_import(objc_interface_context)) {
    const char * name = 
      objc_build_weak_reference_internal_classname(CLASS_NAME(objc_interface_context) , 0);
    tree asm_str = build_string(strlen(name), name);
    cgraph_add_asm_node(asm_str);
    name = objc_build_weak_reference_internal_classname(CLASS_NAME(objc_interface_context) , 1);
    asm_str = build_string(strlen(name), name);
    cgraph_add_asm_node(asm_str);
  }
  /* APPLE LOCAL end radar 7865106 */
  objc_public_flag = 0;
}

void
objc_start_category_interface (tree class, tree categ, tree protos)
{
  /* APPLE LOCAL begin radar 4994854 */
  if (categ == NULL_TREE)
    {
      /* Anonymous category. Check if it is a continuation anonymous category? */
      tree class_category_is_assoc_with = lookup_interface (class);
      if (class_category_is_assoc_with)
        {
          tree cat = lookup_category (class_category_is_assoc_with, NULL_TREE);
          if (cat)
            {
              /* This is a continuation anonymous category. Use the first declaration. */
              objc_interface_context = cat;
              objc_ivar_chain = continue_class (objc_interface_context);
              return;
            }
        }
    }
  /* APPLE LOCAL end radar 4994854 */
  objc_interface_context
    = start_class (CATEGORY_INTERFACE_TYPE, class, categ, protos);
  objc_ivar_chain
    = continue_class (objc_interface_context);
}

void
/* APPLE LOCAL radar 4947311 - protocol attributes */
objc_start_protocol (tree name, tree protos, tree prefix_attrs)
{
  objc_interface_context
    /* APPLE LOCAL radar 4947311 - protocol attributes */
    = start_protocol (name, protos, prefix_attrs);
  /* APPLE LOCAL C* language */
  objc_method_optional_flag = 0;
}

void
objc_continue_interface (void)
{
  /* APPLE LOCAL begin radar 4666559 */
  /* Since we may add additional ivars for @property declarations,
     do not output any debug info. for class type as yet. */
  gcc_assert (TREE_CODE (objc_interface_context) == CLASS_INTERFACE_TYPE);
  save_default_debug_hooks = debug_hooks;
  debug_hooks = &do_nothing_debug_hooks;
  /* APPLE LOCAL end radar 4666559 */
  objc_ivar_chain
    = continue_class (objc_interface_context);
  /* APPLE LOCAL radar 4666559 */
  debug_hooks = save_default_debug_hooks;
}

void
objc_finish_interface (void)
{
  finish_class (objc_interface_context);
  /* APPLE LOCAL begin radar 4666559 */
  if (TREE_CODE (objc_interface_context) == CLASS_INTERFACE_TYPE)
    {
      tree record = CLASS_STATIC_TEMPLATE (objc_interface_context);
      /* Now output debug info. Since ivar list is now complete. */
      gcc_assert (record);
#ifdef OBJCPLUS
      push_lang_context (lang_name_c);
#endif /* OBJCPLUS */
      rest_of_type_compilation (record, 1);
      if (flag_debug_only_used_symbols && TYPE_STUB_DECL (record))
	TREE_USED (TYPE_STUB_DECL (record)) = 1;
#ifdef OBJCPLUS
      pop_lang_context ();
#endif /* OBJCPLUS */
    }
  /* APPLE LOCAL end radar 4666559 */
  objc_interface_context = NULL_TREE;
  /* APPLE LOCAL C* language */
  objc_method_optional_flag = 0;
}

#ifdef ENABLE_LLVM
/* Return size in bits this class occupies when used as a base class. */
static int realClassSize(tree class)
{
  unsigned int instanceSize = 0;
  tree field = TYPE_FIELDS (class);
  while (field && TREE_CHAIN (field)
         && TREE_CODE (TREE_CHAIN (field)) == FIELD_DECL)
    field = TREE_CHAIN (field);
  
  if (field && TREE_CODE (field) == FIELD_DECL)
    instanceSize = int_byte_position (field) * BITS_PER_UNIT + 
                          tree_low_cst (DECL_SIZE (field), 0);
  return instanceSize;
}
#endif

void
objc_start_class_implementation (tree class, tree super_class)
{
  objc_implementation_context
    = objc_ivar_context
    = start_class (CLASS_IMPLEMENTATION_TYPE, class, super_class, NULL_TREE);
  objc_public_flag = 0;
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  if (flag_objc_abi == 2) 
    {
      tree record = CLASS_STATIC_TEMPLATE(implementation_template);
      if (record) 
        {
          bool changed = false;
          tree field;
          for (field = TYPE_FIELDS(record); field; field = TREE_CHAIN (field))
            {
#ifdef OBJCPLUS
              /* Skip C++ static members, and things that are not fields at all. */
              if (TREE_CODE (field) != FIELD_DECL || TREE_STATIC (field))
                continue;
#endif
              /* If we have an embedded base class, and its size doesn't match the
                 size in the field node, that's because ivars were added to the base
                 class after the field node was built.  We need to update the field
                 node and re-layout the outer record. 
                 Note that we can't rely on the size in the TYPE_SIZE node of
                 the embedded base class type, it is wrong for some cases
                 involving bitfields (!) */
              if (DECL_ARTIFICIAL (field) && !DECL_NAME (field)
                  && TREE_CODE (TREE_TYPE (field)) == RECORD_TYPE
                  && DECL_SIZE (field) 
                  && TREE_CODE (DECL_SIZE (field)) == INTEGER_CST)
                {
                  unsigned int realSize = realClassSize(TREE_TYPE(field));
                  if (realSize && 
                      TREE_INT_CST_LOW (DECL_SIZE (field)) != realSize)
                  {
                    DECL_SIZE (field) = build_int_cst(bitsizetype, realSize);
                    DECL_SIZE_UNIT (field) = 
                        build_int_cst(sizetype, realSize / BITS_PER_UNIT);
                    changed = true;
                  }
                }
            }
          if (changed) 
            {
              TYPE_SIZE (record) = 0;
              layout_type (record);
            }
        }
    }
#endif
  /* LLVM LOCAL end */
}

void
objc_start_category_implementation (tree class, tree categ)
{
  objc_implementation_context
    = start_class (CATEGORY_IMPLEMENTATION_TYPE, class, categ, NULL_TREE);
  objc_ivar_chain
    = continue_class (objc_implementation_context);
}

void
objc_continue_implementation (void)
{
  objc_ivar_chain
    = continue_class (objc_implementation_context);
}

void
objc_finish_implementation (void)
{
#ifdef OBJCPLUS
  if (flag_objc_call_cxx_cdtors)
    objc_generate_cxx_cdtors ();
#endif

  if (objc_implementation_context)
    {
      finish_class (objc_implementation_context);
      objc_ivar_chain = NULL_TREE;
      objc_implementation_context = NULL_TREE;
    }
  else
    warning (0, "%<@end%> must appear in an @implementation context");
}

void
objc_set_visibility (int visibility)
{
  objc_public_flag = visibility;
}

/* APPLE LOCAL begin C* language */
void
objc_set_method_opt (int optional)
{
  objc_method_optional_flag = optional;
  if (!objc_interface_context
      || TREE_CODE (objc_interface_context) != PROTOCOL_INTERFACE_TYPE)
    {
      error ("@optional/@required is allowed in @protocol context only.");
      objc_method_optional_flag = 0;
    }
}

/* This routine gather property attribute information from attribute
   portion of a property declaration. */

void
objc_set_property_attr (int attr, tree ident)
{
  static char string[BUFSIZE];
  switch (attr)
    {
    case 0: /* init */
	property_readonly = false;
	property_setter = property_getter = NULL_TREE;
 	property_readwrite = property_assign = property_retain = property_copy = false;
	/* APPLE LOCAL radar 4947014 - objc atomic property */
	property_atomic = true;
	break;
    case 1: /* readonly */
	property_readonly = true;
	break;
    case 2: /* getter = ident */
	if (property_getter != NULL_TREE)
	  error("getter attribute can be specified only once");
        property_getter = ident;
	break;
    case 3: /* setter = ident */
	if (property_setter != NULL_TREE)
	  error("setter attribute can be specified only once");
	/* setters always have a trailing ':' in their name. In fact, this is the
	   only syntax that parser recognizes for a setter name. Must add a trailing
	   ':' here so name matches that of the declaration of user instance method
	   for the setter. */
	sprintf (string, "%s:", IDENTIFIER_POINTER (ident));
	property_setter = get_identifier (string);;
	break;
    case 9: /* readwrite */
	property_readwrite = true;
	break;
    case 10: /* assign */
	property_assign = true;
	break;
    case 11: /* retain */
	property_retain = true;
	break;
    case 12: /* copy */
	property_copy = true;
	break;
    /* APPLE LOCAL begin radar 4947014 - objc atomic property */
    case 13: /* nonatomic */
	property_atomic = false;
	break;
    /* APPLE LOCAL end radar 4947014 - objc atomic property */
    default:
	break;
    }
}

/* APPLE LOCAL begin radar 5168496 */
/* Return TRUE if object of TYPE pointer type requires explicit memory management. 
   This is reserved for objctivec-c object pointers; such as 'id'. */
static bool
managed_objc_object_pointer (tree type)
{
  return type && type != error_mark_node && POINTER_TYPE_P (type)
	 && (objc_is_object_id (TREE_TYPE (type)) || TYPED_OBJECT (TREE_TYPE (type))
             /* APPLE LOCAL radar 6255671 */
             || TREE_CODE (type) == BLOCK_POINTER_TYPE);
}
/* APPLE LOCAL end radar 5168496 */

/* This routine declares a 'setter' method for property X in class CLASS. */
static void
objc_add_property_setter_method (tree class, tree x)
{
  tree type = TREE_TYPE (x);
  tree key_name, arg_type, arg_name;
  tree setter_decl, selector;
  tree ret_type = build_tree_list (NULL_TREE, void_type_node);
  tree mth;
  /* setter name. */
  key_name = get_identifier (objc_build_property_setter_name (
                             PROPERTY_NAME (x), false));
  arg_type = build_tree_list (NULL_TREE, type);
  arg_name = get_identifier ("_value");
  /* APPLE LOCAL radar 4157812 */
  selector = objc_build_keyword_decl (key_name, arg_type, arg_name, NULL_TREE);
  /* APPLE LOCAL radar 5839812 - location for synthesized methods  */
  DECL_SOURCE_LOCATION (selector) = DECL_SOURCE_LOCATION (x);
  setter_decl = build_method_decl (INSTANCE_METHOD_DECL,
                                   ret_type, selector,
                                   build_tree_list (NULL_TREE, NULL_TREE),
				   false);
  /* APPLE LOCAL radar 4712415 */
  TREE_DEPRECATED (setter_decl) = TREE_DEPRECATED (x);
  mth = lookup_method (CLASS_NST_METHODS (class),
                       METHOD_SEL_NAME (setter_decl));
  if (PROPERTY_IVAR_NAME (x)
      && (mth = lookup_method (CLASS_NST_METHODS (class),
                               METHOD_SEL_NAME (setter_decl))))
    {
      /* user declared accessor is encounterred. This is error. */
      error ("user accessor %<%c%s%> not allowed for property %qs",
             '-', IDENTIFIER_POINTER (METHOD_SEL_NAME (mth)),
             IDENTIFIER_POINTER (PROPERTY_NAME (x)));
    }
  else
    /* APPLE LOCAL radar 4653422 */
    objc_add_method (class, setter_decl, false, OPTIONAL_PROPERTY (x));
  METHOD_PROPERTY_CONTEXT (setter_decl) = x;
  return;
}

/* This routine builds a 'property_decl' tree node and adds it to the list
   of such properties in the current class. It also checks for duplicates.  */

void
objc_add_property_variable (tree decl)
{
  tree property_decl;
  tree x;

  /* APPLE LOCAL radar 4531086 */
  OBJC_WARN_OBJC2_FEATURES ("property");
  if (DECL_INITIAL (decl) || TREE_CODE (TREE_TYPE (decl)) == ARRAY_TYPE)
    {
      error ("bad property declaration");
      return;
    }
  if (objc_implementation_context)
    {
      error ("property cannot be declared inside @implementation context");
      return;
    }
  else if (!objc_interface_context)
    {
      fatal_error ("property declaration not in @interface or @implementation context");
      return;
    }

  property_decl = make_node (PROPERTY_DECL);
  TREE_TYPE (property_decl) = TREE_TYPE (decl);

  PROPERTY_NAME (property_decl) = DECL_NAME (decl);
  PROPERTY_GETTER_NAME (property_decl) = property_getter;
  PROPERTY_SETTER_NAME (property_decl) = property_setter;
  PROPERTY_READONLY (property_decl) = property_readonly
					? boolean_true_node
					: boolean_false_node;
  PROPERTY_IVAR_NAME (property_decl) = NULL_TREE;
  PROPERTY_DYNAMIC (property_decl) = NULL_TREE;
  /* APPLE LOCAL radar 4712415 */
  TREE_DEPRECATED (property_decl) = TREE_DEPRECATED (decl);
  /* APPLE LOCAL radar 4653422 */
  OPTIONAL_PROPERTY (property_decl) = objc_method_optional_flag;
  /* assign, retain and copy are mutually exclusive. */
  if ((property_assign && property_retain)
      || (property_assign && property_copy) 
      || (property_retain && property_copy))
    {
      error ("assign, retain and copy are mutually exclusive");
      property_copy = property_retain = false;
    }
 else if (!property_assign && !property_retain && !property_copy
          && !property_readonly
          /* APPLE LOCAL radar 5168496 */
	  && managed_objc_object_pointer (TREE_TYPE (property_decl)))
        {
          /* For -fobjc-gc-only, a warning is issued under more restricted condition. See below. */
          if (!flag_objc_gc_only && warn_property_assign_default)
            warning (0, "no 'assign', 'retain', or 'copy' attribute is specified - 'assign' is assumed");
        }

  /* readwrite is the default */
  if (!property_readonly)
    property_readwrite = true;
  /* readonly and readwrite are mutually exclusive. */
  else if (property_readwrite)
    {
      error ("readonly and readwrite are mutually exclusive");
      property_readwrite = false;
      PROPERTY_READONLY (property_decl) = boolean_false_node;
    }
  PROPERTY_COPY (property_decl) = property_copy 
			          ? boolean_true_node 
			          : boolean_false_node;
  PROPERTY_RETAIN (property_decl) = property_retain 
				    ? boolean_true_node 
				    : boolean_false_node;
  PROPERTY_READWRITE (property_decl) = property_readwrite
				       ? boolean_true_node 
				       : boolean_false_node;
  /* APPLE LOCAL radar 4947014 - objc atomic property */
  ATOMIC_PROPERTY (property_decl) = property_atomic;

  if (objc_interface_context)
    {
      /* Doing the property in interface declaration. */
      if (PROPERTY_ASSIGN (property_decl))
	{
	  /* APPLE LOCAL radar 5168496 */
	  if (managed_objc_object_pointer (TREE_TYPE (property_decl)))
	    {
	      /* APPLE LOCAL begin radar 4855821 */
	      if (!flag_objc_gc)
		{
                  /* APPLE LOCAL begin radar 6255671 */
		  if (!property_assign)
                  {
                    if (TREE_CODE (TREE_TYPE (property_decl)) == BLOCK_POINTER_TYPE)
                      error ("assign attribute (default) not appropriate for non-gc object property %qs",
                             IDENTIFIER_POINTER (PROPERTY_NAME (property_decl)));
                    else
                      warning (0, "assign attribute (default) not appropriate for non-gc object property %qs",
                               IDENTIFIER_POINTER (PROPERTY_NAME (property_decl)));
                  }
                  /* APPLE LOCAL end radar 6255671 */
		}
	      /* APPLE LOCAL end radar 4855821 */
              else if (!property_assign)
                {
                  /* under -fobjc-gc[-only], a warning should be issued if none of those were specified
                     AND the object type conforms to <NSCopying>. */
                  tree ltyp = TREE_TYPE (property_decl);
                  /* APPLE LOCAL begin radar 6379842 */
                  /* under -fobjc-gc-only flag, a warning to be issued if 
                   block pointer property does not have 'copy' attribute. */
                  if (flag_objc_gc_only 
                      && TREE_CODE (ltyp) == BLOCK_POINTER_TYPE
                      && !property_copy)
                    warning (0, "'copy' attribute must be specified for the block"
                           " property %qs when -fobjc-gc-only is specified",
                           IDENTIFIER_POINTER (PROPERTY_NAME (property_decl)));
                  /* APPLE LOCAL end radar 6379842 */
                  do
                    ltyp = TREE_TYPE (ltyp);
                  while (POINTER_TYPE_P (ltyp));
		  /* APPLE LOCAL begin radar 5607453 */
                  /* APPLE LOCAL radar 5096644 */
                  if (TYPE_HAS_OBJC_INFO (ltyp) 
		      && !objc_is_object_id (ltyp) && !objc_is_class_id (ltyp)
		      && TREE_CODE (TYPE_OBJC_INTERFACE (ltyp)) != IDENTIFIER_NODE)
		  /* APPLE LOCAL end radar 5607453 */
                    {
                      tree id_NSCopying = get_identifier ("NSCopying");
                      tree lproto = lookup_protocol (id_NSCopying);
		      /* APPLE LOCAL radar 4947311 - protocol attributes */
                      objc_declare_protocols (tree_cons (NULL_TREE, id_NSCopying, NULL_TREE), NULL_TREE);
                      /* APPLE LOCAL begin radar 5096644 */
                      if (lproto && TREE_CODE (lproto) == PROTOCOL_INTERFACE_TYPE
                          && objc_lookup_protocol (lproto , TYPE_OBJC_INTERFACE (ltyp), ltyp, false))
                        /* APPLE LOCAL begin radar 5698469 */
                        warning (0, "%s'assign' attribute on property %qs which implements 'NSCopying' "
                                 "protocol not appropriate with %s",
                                 !property_assign ? "default " :  "",
                                 IDENTIFIER_POINTER (PROPERTY_NAME (property_decl)),
                                 flag_objc_gc_only ? "-fobjc-gc-only" : "-fobjc-gc");
                      /* APPLE LOCAL end radar 5698469 */
                      /* APPLE LOCAL end radar 5096644 */
                    }
                }
	    }
	}

      /* must check for duplicate property declarations. */
      for (x = CLASS_PROPERTY_DECL (objc_interface_context); x; x = TREE_CHAIN (x))
	{
	  if (PROPERTY_NAME (x) == DECL_NAME (decl))
	    {
	      error ("%Jduplicate property declaration %qD", decl, decl);
	      return;
	    }
	}

      /* APPLE LOCAL begin radar 4965989 */
      /* New property's anonymous category */
      if (ANONYMOUS_CATEGORY (objc_interface_context))
        {
          /* @interface INTF() ... @end */
          tree class = lookup_interface (CLASS_NAME (objc_interface_context));
          if (class)
            {
              /* Find the property in the class. */
              for (x = CLASS_PROPERTY_DECL (class); x; x = TREE_CHAIN (x))
                if (PROPERTY_NAME (x) == DECL_NAME (decl))
                  break;
              if (x)
                {
                  if (PROPERTY_READONLY (x) == boolean_true_node
                      && PROPERTY_READWRITE (property_decl) == boolean_true_node)
                    {
                      /* APPLE LOCAL begin radar 5159695 */
                      if ((PROPERTY_ASSIGN (x) != PROPERTY_ASSIGN (property_decl))
                          || (PROPERTY_RETAIN (x) != PROPERTY_RETAIN (property_decl))
                          || (PROPERTY_COPY (x) != PROPERTY_COPY (property_decl))
                          || (IS_ATOMIC (x) != IS_ATOMIC (property_decl)))
                        warning (0, "property %qs attribute in %qs class continuation"
                                 " does not match class %qs property",
                                 IDENTIFIER_POINTER (PROPERTY_NAME (property_decl)),
                                 IDENTIFIER_POINTER (CLASS_NAME (class)),
                                 IDENTIFIER_POINTER (CLASS_NAME (class)));
                      /* APPLE LOCAL end radar 5159695 */

                      /* class property is now 'readwrite'. */
                      PROPERTY_READONLY (x) = boolean_false_node;
                      PROPERTY_READWRITE (x) = boolean_true_node;
                      PROPERTY_COPY (x) = PROPERTY_COPY (property_decl);
                      PROPERTY_RETAIN (x) = PROPERTY_RETAIN (property_decl);
                      PROPERTY_SETTER_NAME (x) = PROPERTY_SETTER_NAME (property_decl);
                    }
                  else
		    error ("use class continuation to override %<readonly%> property %qs with %<readwrite%> in class %qs",
                           IDENTIFIER_POINTER (PROPERTY_NAME (property_decl)),
                           IDENTIFIER_POINTER (CLASS_NAME (class)));

 		  /* APPLE LOCAL begin radar 5159707 */
 		  if (!comptypes (TREE_TYPE (x), TREE_TYPE (property_decl)))
 		    {
 		      warning (0, "property %qs type in %qs class continuation does not match class %qs property",
 			        IDENTIFIER_POINTER (PROPERTY_NAME (property_decl)),
 			        IDENTIFIER_POINTER (CLASS_NAME (class)),
 			        IDENTIFIER_POINTER (CLASS_NAME (class)));
 		      TREE_TYPE (property_decl) = TREE_TYPE (x);
 		    }
 		  /* APPLE LOCAL end radar 5159707 */
  		}
              /* APPLE LOCAL begin radar 4994854 */
              else
                {
                  /* Inject anonymous categories property declaration into promary class. */
                  tree save_objc_interface_context = objc_interface_context;
                  objc_interface_context = class;
                  objc_add_property_variable (decl);
                  objc_interface_context = save_objc_interface_context;
                }
              /* APPLE LOCAL end radar 4994854 */
            }
          else
            error ("anonymous category %qs has no primary class", IDENTIFIER_POINTER (CLASS_NAME (class)));
        }
      /* APPLE LOCAL end radar 4965989 */
      TREE_CHAIN (property_decl) = CLASS_PROPERTY_DECL (objc_interface_context);
      CLASS_PROPERTY_DECL (objc_interface_context) = property_decl;
    }
}

/* This routine looks for a given PROPERTY in a list of CLASS, CATEGORY, or
   PROTOCOL.  */

static tree
lookup_property_in_list (tree chain, tree property)
{
  tree x;
  for (x = CLASS_PROPERTY_DECL (chain); x; x = TREE_CHAIN (x))
    if (PROPERTY_NAME (x) == property)
      return x;
  return NULL_TREE;
}

/* This routine looks for a given PROPERTY in the tree chain of RPROTO_LIST. */

static tree
lookup_property_in_protocol_list (tree rproto_list, tree property)
{
  tree rproto, x;
  for (rproto = rproto_list; rproto; rproto = TREE_CHAIN (rproto))
    {
      tree p = TREE_VALUE (rproto);
      if (TREE_CODE (p) == PROTOCOL_INTERFACE_TYPE)
	{
	  if ((x = lookup_property_in_list (p, property)))
	    return x;
	  if (PROTOCOL_LIST (p)
	      && (x = lookup_property_in_protocol_list (PROTOCOL_LIST (p), property)))
	    return x;
	}
      else
	{
	  ; /* An identifier...if we could not find a protocol.  */
	}
    }
  return NULL_TREE;
}

/* This routine looks up the PROPERTY in current INTERFACE, its categories and up the
   chain of interface hierarchy.  */

static tree
lookup_property (tree interface_type, tree property)
{
  tree inter = interface_type;
  while (inter)
    {
      tree x, category;
      if (TREE_CODE (inter) != CLASS_INTERFACE_TYPE)
	  return NULL_TREE;  /* error - caller will issue it */
      if ((x = lookup_property_in_list (inter, property)))
	return x;
      /* Failing that, look for the property in each category of the class.  */
      category = inter;
      while ((category = CLASS_CATEGORY_LIST (category)))

	if ((x = lookup_property_in_list (category, property)))
	  return x;

      /*  Failing to find in categories, look for property in protocol list. */
      if (CLASS_PROTOCOL_LIST (inter)
	  && (x = lookup_property_in_protocol_list (
		    CLASS_PROTOCOL_LIST (inter), property)))
	return x;

      /* Failing that, climb up the inheritance hierarchy.  */
      inter = lookup_interface (CLASS_SUPER_NAME (inter));
    }
  return inter;
}

static tree
objc_get_inner_exp (tree t)
{
  while (TREE_CODE (t) == COMPOUND_EXPR
         || TREE_CODE (t) == MODIFY_EXPR
         || TREE_CODE (t) == NOP_EXPR
         || TREE_CODE (t) == CONVERT_EXPR
         || TREE_CODE (t) == COMPONENT_REF)
    t = TREE_OPERAND (t, 0);
  return t;
}

/* APPLE LOCAL begin radar 5285911 */
/* This routine, given RECEIVER.COMPONENT, RTYPE for type of the class for the
   receiver, returns in PROP the property declaration and in INTERFACE_TYPE class
   declaration. Furthermore, if CHECK_PROP_COND is true, it checks for correctness of
   property use in dot syntax and issues diagnostic. If CHECK_PROP_COND is false, it assumes
   that tree is a correct property access. On sucessess it returns RECEIVER.
   Finally, if RECEIVER is a class object, true is returned in RECEIVER_IS_CLASS.
*/
static tree
objc_property_access_info (tree receiver, tree component, tree rtype, 
			   bool check_prop_cond, 
			   tree *interface_type, tree *prop, bool *receiver_is_class)
{
  *prop = NULL_TREE;
  *interface_type = NULL_TREE;
  *receiver_is_class = false;
  if (objc_is_id (rtype))
    {
      tree rprotos = (TYPE_HAS_OBJC_INFO (TREE_TYPE (rtype))
                      ? TYPE_OBJC_PROTOCOL_LIST (TREE_TYPE (rtype))
                      : NULL_TREE);
      if (rprotos)
        *prop = lookup_property_in_protocol_list (rprotos, component);
      else
      /* Could be super.property */
	{
      	  tree t = objc_get_inner_exp (receiver);
	  /* APPLE LOCAL begin radar 5277239 */
	  int super = (t == UOBJC_SUPER_decl);
	  tree class_tree = receiver_is_class_object (receiver, (t == self_decl), super);
	  /* CLASSNAME.class_method ? */
	  if (class_tree)
	    {
	      tree class_type = lookup_interface (class_tree);
	      if (class_type)
	      {
	        *interface_type = class_type;
	        *receiver_is_class = true;
	      }
	    }
	  else if (super)
	  /* APPLE LOCAL end radar 5277239 */
            {
              /* super.property */
              if (implementation_template && CLASS_SUPER_NAME (implementation_template))
		{
                  *interface_type = lookup_interface (CLASS_SUPER_NAME (implementation_template));
		  if (*interface_type)
		    *prop = lookup_property (*interface_type, component);
		}
            }
	}
    }
  else
    {
      tree basetype = TYPE_MAIN_VARIANT (rtype);
      if (basetype != NULL_TREE && TREE_CODE (basetype) == POINTER_TYPE)
        basetype = TREE_TYPE (basetype);
      else if (check_prop_cond)
	/* APPLE LOCAL begin radar 5376125 */
	{
	  /* Note that for objc2, this warning is issued when ivar reference
	     tree is generated in call to objc_v2_build_ivar_ref */
	  if (!flag_objc_gc && warn_direct_ivar_access && flag_objc_abi != 2)
            objc_warn_direct_ivar_access (basetype, component);
	  return NULL_TREE;
	}
	/* APPLE LOCAL end radar 5376125 */

      while (basetype != NULL_TREE
             && TREE_CODE (basetype) == RECORD_TYPE && OBJC_TYPE_NAME (basetype)
             && TREE_CODE (OBJC_TYPE_NAME (basetype)) == TYPE_DECL
             && DECL_ORIGINAL_TYPE (OBJC_TYPE_NAME (basetype)))
      basetype = DECL_ORIGINAL_TYPE (OBJC_TYPE_NAME (basetype));
      if (basetype != NULL_TREE && TYPED_OBJECT (basetype))
        {
          *interface_type = TYPE_OBJC_INTERFACE (basetype);
          /* Handle more user error conditions */
          if (check_prop_cond && 
	      (!*interface_type ||
              (TREE_CODE (*interface_type) != CLASS_INTERFACE_TYPE
               &&  TREE_CODE (*interface_type) != PROTOCOL_INTERFACE_TYPE
               && TREE_CODE (*interface_type) != CATEGORY_INTERFACE_TYPE)))
	    {
	      /* Better error recovery */
	      if (TREE_CODE (receiver) == OBJC_PROPERTY_REFERENCE_EXPR)
		{
		  error ("accessing unknown %qs component of a property",
                         IDENTIFIER_POINTER (component));
                  return error_mark_node;
                }
	      return NULL_TREE;
	    }
          *prop = lookup_property (*interface_type, component);
          /* APPLE LOCAL begin radar 4959107 */
          if (!*prop && TYPE_OBJC_PROTOCOL_LIST (basetype))
            *prop = lookup_property_in_protocol_list (TYPE_OBJC_PROTOCOL_LIST (basetype), 
						      component);
          /* APPLE LOCAL end radar 4959107 */
        }
    }
  return receiver;
}

/*
  This routine builds the OBJC_PROPERTY_REFERENCE_EXPR for 
  RECEIVER.COMPONENT which later on gets transformed into a 'getter' or 
  'setter' call depending on its use.
*/

tree
objc_build_property_reference_expr (tree receiver, tree component)
{
  tree interface_type;
  tree rtype;
  tree prop, prop_type, res;
  bool receiver_is_class;
  /* APPLE LOCAL radar 6083666 */
  tree ivar;

  if (component == error_mark_node || component == NULL_TREE
      || TREE_CODE (component) != IDENTIFIER_NODE)
    return NULL_TREE;
  if (receiver == error_mark_node || receiver == NULL_TREE
      || (rtype = TREE_TYPE (receiver)) == NULL_TREE)
    return NULL_TREE;
  res = objc_property_access_info (receiver, component, rtype, true, 
				   &interface_type, &prop, &receiver_is_class);
  /* These are conditions for errornous property access or not a property
     access syntax. */
  if (res == NULL_TREE || res == error_mark_node)
    return res;

  /* APPLE LOCAL radar 6083666 */
  prop_type = ivar = NULL_TREE;
  /* APPLE LOCAL begin objc2 5512183 */
  if (interface_type && !receiver_is_class)
  /* APPLE LOCAL end objc2 5512183 */
    {
      /* type of the expression is either the property type or, if no property declared,
	 then ivar type used in receiver.ivar expression. */
      /* APPLE LOCAL radar 6083666 */
      ivar = nested_ivar_lookup (interface_type, component);
      if (ivar)
	prop_type = TREE_TYPE (ivar);
      else
        {
	  /* Try to get the type for the 'getter' declaration. */
          tree getter = lookup_method_static (interface_type, component, 0, false, false);
	  if (getter)
	    prop_type = TREE_VALUE (TREE_TYPE (getter));
        }
    }
  /* APPLE LOCAL begin radar 5277239 */
  else if (interface_type)
    {
      /* CLASS_NAME.getter expression. Try finding a class method and, if one not found, 
	 attach a dummy 'int' type to the expression tree (presumably because we end up 
	 using it on the LHS of assignment only. */
      tree getter = lookup_method_static (interface_type, component, 1, false, true);
      prop_type = getter ? TREE_VALUE (TREE_TYPE (getter)) : integer_type_node;
    }
  /* APPLE LOCAL end radar 5277239 */
  /* APPLE LOCAL begin radar 5893391 */
  else if (!prop && objc_is_id (rtype))
  {
    tree rprotos = (TYPE_HAS_OBJC_INFO (TREE_TYPE (rtype))
                    ? TYPE_OBJC_PROTOCOL_LIST (TREE_TYPE (rtype))
                    : NULL_TREE);
    if (rprotos) {
        tree getter = lookup_method_in_protocol_list (rprotos, component, 0);
        prop_type = getter ? TREE_VALUE (TREE_TYPE (getter)) : integer_type_node;
    }
  }
  /* APPLE LOCAL end radar 5893391 */
  
  /* APPLE LOCAL begin objc2 5512183 */
  if (prop)
    {
      /* APPLE LOCAL begin radar 6029577 */
      tree property_type = TREE_TYPE (prop);
      bool comparison_result = false;
      if (prop_type)
      {
#ifdef OBJCPLUS
        if (TREE_CODE (property_type) == REFERENCE_TYPE
            || TREE_CODE (prop_type) == REFERENCE_TYPE)
        {
          if (TREE_CODE (property_type) == REFERENCE_TYPE)
            property_type = TREE_TYPE (property_type);
          if (TREE_CODE (prop_type) == REFERENCE_TYPE)
            prop_type = TREE_TYPE (prop_type);
          comparison_result =
            !objcp_reference_related_p (prop_type, property_type);
        }
        else
#endif
          comparison_result = comptypes (prop_type, property_type) != 1;
      }
      /* APPLE LOCAL begin radar 6083666 */
      if (!ivar && prop_type && comparison_result
          && !objc_compare_types(property_type, prop_type, -6, NULL_TREE, NULL))
	warning (0, "type of accessor does not match the type of property %qs",
	        IDENTIFIER_POINTER (PROPERTY_NAME (prop)));
      else
        prop_type = property_type;
      /* APPLE LOCAL end radar 6083666 */
      /* APPLE LOCAL end radar 6029577 */
    }
  /* APPLE LOCAL end objc2 5512183 */

  if (prop_type)
    {
      /* APPLE LOCAL begin 5494488 */
      tree prop_expr = build3 (OBJC_PROPERTY_REFERENCE_EXPR, prop_type,
			       receiver, component, UOBJC_SUPER_decl);
      /* APPLE LOCAL end 5494488 */
      TREE_SIDE_EFFECTS (prop_expr) = 1;
      return prop_expr;
    }
  else
    return NULL_TREE;
}

/* This routine generates a 'getter' call tree, using the input parameters. It returns 
   the CALL_EXPR tree or error_mark_node */

static tree
objc_build_getter_call (tree receiver, tree component)
{
  tree interface_type;
  tree x;
  bool receiver_is_class;
  tree getter = error_mark_node;
  tree rtype = TREE_TYPE (receiver);
  (void)objc_property_access_info (receiver, component, rtype, false, 
				   &interface_type, &x, &receiver_is_class);

  if (x)
    {
      /* Get the getter name. */
      gcc_assert (PROPERTY_NAME (x));
      if (PROPERTY_GETTER_NAME (x))
	/* If 'getter=getter_name' attribute is specified, must use the user provided
	   getter name in generating messaging call. */
        getter = objc_finish_message_expr (receiver, PROPERTY_GETTER_NAME (x), NULL_TREE);
      else
        getter = objc_finish_message_expr (receiver, PROPERTY_NAME (x), NULL_TREE);
    }
  else if (interface_type)
    {
      /* APPLE LOCAL begin radar 5277239 */
      getter = lookup_method_static (interface_type, component, receiver_is_class, 
				     false, receiver_is_class);
      /* APPLE LOCAL end radar 5277239 */
      if (getter)
#ifdef OBJCPLUS
	getter = objc_build_message_expr (
		   build_tree_list (receiver, 
				    build_tree_list (component, NULL_TREE)));
#else
	getter = objc_build_message_expr (build_tree_list (receiver, component));
#endif
      else
	{
	  error ("accessing unknown %qs %s method",
		  IDENTIFIER_POINTER (component), receiver_is_class ? "class" : "getter");
	  getter = receiver;	/* recover */
	}
    }
  /* APPLE LOCAL begin radar 5893391 */
  else if (objc_is_id (rtype))
  {
    tree rprotos = (TYPE_HAS_OBJC_INFO (TREE_TYPE (rtype))
                    ? TYPE_OBJC_PROTOCOL_LIST (TREE_TYPE (rtype))
                    : NULL_TREE);
    if (rprotos) {
      getter = lookup_method_in_protocol_list (rprotos, component, 0);
      if (getter)
#ifdef OBJCPLUS
	getter = objc_build_message_expr (
          build_tree_list (receiver, build_tree_list (component, NULL_TREE)));
#else
      getter = objc_build_message_expr (build_tree_list (receiver, component));
#endif
      else
      {
        error ("accessing unknown %qs getter method",
               IDENTIFIER_POINTER (component));
        getter = receiver;	/* recover */
      }      
    }
  }
  /* APPLE LOCAL end radar 5893391 */
  return getter;
}
/* APPLE LOCAL end radar 5285911 */

/* This routine is called with PROPERTY_IDENT being the name of the getter method,
   ; user attribute specified or the default. From the getter name, it first finds 
   the property name and from property name it finds the setter_name; one specified
   by user or the default. */

static const char *
objc_resolve_build_property_setter_name (tree receiver, tree property_ident)
{
  tree rtype, basetype,x,class=NULL_TREE;
  bool receiver_is_class = false;
  gcc_assert (receiver && TREE_TYPE (receiver));
  rtype = TREE_TYPE (receiver);
  if (objc_is_id (rtype))
    {
      tree t = objc_get_inner_exp (receiver);
      /* APPLE LOCAL begin radar 5277239 */
      int super = (t == UOBJC_SUPER_decl);
      tree class_tree = receiver_is_class_object (receiver, (t == self_decl), super);
      /* CLASSNAME.class_method = rhs ? */
      if (class_tree)
        {
          class = lookup_interface (class_tree);
          receiver_is_class = true;
        }
      else if (super)
      /* APPLE LOCAL end radar 5277239 */
	{
	  /* super.property */
	  if (implementation_template
                && CLASS_SUPER_NAME (implementation_template))
	    class = lookup_interface (CLASS_SUPER_NAME (implementation_template));
	}
      /* APPLE LOCAL begin radar 8290584 */
      else {
        /* receiver could be of type id<protocol,...> find property in 
           protocol list. */
        tree rprotos = (TYPE_HAS_OBJC_INFO (TREE_TYPE (rtype))
                        ? TYPE_OBJC_PROTOCOL_LIST (TREE_TYPE (rtype))
                        : NULL_TREE);
        if (rprotos) {
          x = lookup_property_in_protocol_list (rprotos, property_ident);
          if (x) {
            return PROPERTY_SETTER_NAME (x) ? 
                     IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (x)) :
                     objc_build_property_setter_name(property_ident, true);
          }
        }
      }
      /* APPLE LOCAL end radar 8290584 */
      /* APPLE LOCAL begin radar 8351204 */
      if (!class) {
        error ("expression is not assignable - type of receiver is unknown");
        /* recover */
        return objc_build_property_setter_name (property_ident, true);
      }
      /* APPLE LOCAL end radar 8351204 */
    }
  else
    {
      basetype = TYPE_MAIN_VARIANT (rtype);
      gcc_assert (basetype && TREE_CODE (basetype) == POINTER_TYPE);
      basetype = TREE_TYPE (basetype);
      while (basetype != NULL_TREE
             && TREE_CODE (basetype) == RECORD_TYPE && OBJC_TYPE_NAME (basetype)
             && TREE_CODE (OBJC_TYPE_NAME (basetype)) == TYPE_DECL
             && DECL_ORIGINAL_TYPE (OBJC_TYPE_NAME (basetype)))
        basetype = DECL_ORIGINAL_TYPE (OBJC_TYPE_NAME (basetype));
      gcc_assert (basetype && TYPED_OBJECT (basetype));
      class = TYPE_OBJC_INTERFACE (basetype);
    }
  gcc_assert (class);
  /* APPLE LOCAL begin radar 5277239 */
  if (receiver_is_class)
    {
      /* Direct access of a class method using the dot syntax. 
         Make up a 'setter' method name. */
      char *setter_name = objc_build_property_setter_name (property_ident, true);
      /* A class method of the given name must exist or it is error. */
      if (!lookup_method_static (class, get_identifier (setter_name), 1, false, true))
        error ("accessing unknown %qs class method", setter_name);
      return setter_name; 
    }
  /* APPLE LOCAL end radar 5277239 */
  /* APPLE LOCAL begin radar 5390587 */
  x = lookup_property (class, property_ident);
  return (x && PROPERTY_SETTER_NAME (x)) ? IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (x))
					 : objc_build_property_setter_name 
					     (property_ident, true);
  /* APPLE LOCAL end radar 5390587 */
}

/* This routine builds a call to property's 'setter' function. RECEIVER is the 
   receiving object for 'setter'. PROPERTY_IDENT is name of the property and
   RHS is the argument passed to the 'setter' function.  */

static tree
objc_setter_func_call (tree receiver, tree property_ident, tree rhs)
{
  tree setter_argument = build_tree_list (NULL_TREE, rhs);
  const char *setter_name = objc_resolve_build_property_setter_name (receiver, property_ident); 
  tree setter;
  in_objc_property_setter_name_context = true;
  setter = objc_finish_message_expr (receiver, 
				     get_identifier (setter_name),
                                     setter_argument);
  in_objc_property_setter_name_context = false;
  /* APPLE LOCAL begin radar 4838528 */
  if (TREE_CODE (setter) == CALL_EXPR && TREE_TYPE (setter) != void_type_node)
    warning (0, "type of setter %qs must be 'void' instead of '%T'", 
	     setter_name, TREE_TYPE (setter) );
  /* APPLE LOCAL end radar 4838528 */
  return setter;
}

/* This routine builds a compound expression which implements semantics
   of property's setter expression:
   typeof (rhs) temp; (temp = rhs, [receiver prop_setter:temp], temp)
   if rhs has no side-effect, simply generate:
   ([receiver prop_setter:rhs], rhs)
*/
static tree
objc_build_compound_setter_call (tree receiver, tree prop_ident, tree rhs)
{
  tree temp, bind, comma_exp;
  /* LLVM LOCAL begin 8246180 */
#ifdef OBJCPLUS
  if (TYPE_NEEDS_CONSTRUCTING (TREE_TYPE(rhs)))
    error("setting a C++ non-POD object value is not implemented - assign the value to a temporary and use the temporary.");
#endif
  if (TREE_SIDE_EFFECTS (rhs)
#ifdef OBJCPLUS
      || TYPE_NEEDS_CONSTRUCTING (TREE_TYPE(rhs))
#endif
  /* LLVM LOCAL end 8246180 */
      )
    {
      /* To allow for correct property assignment semantics
         and in accordance with C99 rules we generate: type temp;
         (temp = rhs, [lhs Setter:temp], temp) */
      /* APPLE LOCAL begin radar 5279122 */
      /* APPLE LOCAL radar 6264448 */
      /* rhs = default_conversion (rhs);  removed */
      temp = objc_create_named_tmp_var (TREE_TYPE (rhs), "prop");
      /* APPLE LOCAL end radar 5279122 */
      bind = build3 (BIND_EXPR, void_type_node, temp, NULL, NULL);
      TREE_SIDE_EFFECTS (bind) = 1;
      add_stmt (bind);
      /* APPLE LOCAL begin radar 7591784 */
#ifdef OBJCPLUS
      {
	tree type = TREE_TYPE (rhs);
	if (TYPE_NEEDS_CONSTRUCTING (type))
          comma_exp = temp;
	else
      	  comma_exp = build_modify_expr (temp, NOP_EXPR, rhs);
      }
#else
      comma_exp = build_modify_expr (temp, NOP_EXPR, rhs);
#endif
      /* APPLE LOCAL end radar 7591784 */
      comma_exp = build_compound_expr (comma_exp,
		    objc_setter_func_call (receiver, prop_ident, temp));
      /* APPLE LOCAL begin radar 6264448 */
      /* conversion of 'temp' moved *after* it is passed ar argument to build
         setter call. This is because its type must match the setter's argument
         type. */
      temp = default_conversion (temp);
      /* APPLE LOCAL end radar 6264448 */
    }
  else
    {
      comma_exp = objc_setter_func_call (receiver, prop_ident, rhs);
      /* APPLE LOCAL radar 5279122 */
      rhs = default_conversion (rhs);
      /* APPLE LOCAL radar 5140757 */
      temp = save_expr (rhs);
      /* APPLE LOCAL begin radar 4727191 */
      if (TREE_CODE (temp) == VAR_DECL)
        DECL_SEEN_IN_BIND_EXPR_P (temp) = 1;
      /* APPLE LOCAL end radar 4727191 */
    }
  /* Suppress C++'s warning of comma-expr with no side-effect. */
  TREE_NO_WARNING (temp) = 1;
  /* Suppress warning on value computed is not used */
  comma_exp = build_compound_expr (comma_exp, temp);
  TREE_NO_WARNING (comma_exp) = 1;
  return comma_exp;
}

/* APPLE LOCAL begin radar 5285911 */
bool
objc_property_reference_expr (tree expr)
{
  return (expr 
	  && TREE_CODE (expr) == OBJC_PROPERTY_REFERENCE_EXPR);
}

/* This routine converts a previously synthesized 'getter' function call for
   a property and converts it to a 'setter' function call for the same
   property.  */

tree
objc_build_setter_call (tree lhs, tree rhs)
{
  if (objc_property_reference_expr (lhs))
    return objc_build_compound_setter_call (TREE_OPERAND (lhs, 0), 
					    TREE_OPERAND (lhs, 1), rhs);
  return NULL_TREE;
}
/* APPLE LOCAL end radar 5285911 */

/* This routine builds the following type:
   struct _prop_t {
     const char * const name;			// property name
     const char * const attributes;		// comma-delimited, encoded,
						// property attributes
   };  */

static tree
build_v2_property_template (void)
{
  tree prop_record;
  tree field_decl, field_decl_chain;

  prop_record = start_struct (RECORD_TYPE, get_identifier ("_prop_t"));
  /* const char * name */
  field_decl = create_field_decl (string_type_node, "name");
  field_decl_chain = field_decl;
  /* const char * attribute */
  field_decl = create_field_decl (string_type_node, "attribute");
  chainon (field_decl_chain, field_decl);
  finish_struct (prop_record, field_decl_chain, NULL_TREE);
  return prop_record;
}

/* This routine builds the following type:
   struct _prop_list_t {
     uint32_t entsize;			// sizeof (struct _prop_t)
     uint32_t prop_count;
     struct _prop_t prop_list [prop_count];
   }  */

static tree
build_v2_property_list_template (tree list_type, int size)
{
  tree property_list_t_record;
  tree field_decl, field_decl_chain;

  property_list_t_record = start_struct (RECORD_TYPE, NULL_TREE);

  /* uint32_t const entsize */
  field_decl = create_field_decl (integer_type_node, "entsize");
  field_decl_chain = field_decl;

  /* int prop_count */
  field_decl = create_field_decl (integer_type_node, "prop_count");
  chainon (field_decl_chain, field_decl);

  /* struct _prop_t prop_list[]; */
  field_decl = create_field_decl (build_array_type
				  (list_type,
				   build_index_type
				   (build_int_cst (NULL_TREE, size - 1))),
				   "prop_list");
  chainon (field_decl_chain, field_decl);

  finish_struct (property_list_t_record, field_decl_chain, NULL_TREE);

  return property_list_t_record;
}

/* This routine encodes the attribute of the input PROPERTY according to following
   formula:

Property attributes are stored as a comma-delimited C string. The simple attributes
readonly and bycopy are encoded as single characters. The parametrized attributes,
getter=name, setter=name, and ivar=name, are encoded as single characters, followed
by an identifier. Property types are also encoded as a parametrized attribute. The
characters used to encode these attributes are defined by the following enumeration:

enum PropertyAttributes {
    kPropertyReadOnly = 'R',                    // property is read-only.
    kPropertyBycopy = 'C',                      // property is a copy of the value last assigned
    kPropertyByref = '&',                      // property is a reference to the value last assigned
    kPropertyDynamic = 'D',			// property is dynamic
    kPropertyGetter = 'G',                      // followed by getter selector name
    kPropertySetter = 'S',                      // followed by setter selector name
    kPropertyInstanceVariable = 'V'     	// followed by instance variable  name
    kPropertyType = 't'                         // followed by old-style type encoding.
    kPropertyWeak = 'W'				// 'weak' property
    kPropertyStrong = 'P'			// property GC'able
    kPropertyNonAtomic = 'N'			// property non-atomic
};  */

static tree
objc_v2_encode_prop_attr (tree property)
{
  const char *string;
  tree type = TREE_TYPE (property);
  obstack_1grow (&util_obstack, 'T');
  /* APPLE LOCAL radar 5002848 */
  generating_property_type_encoding = 1;
  encode_type (type, obstack_object_size (&util_obstack),
	       OBJC_ENCODE_INLINE_DEFS);
  /* APPLE LOCAL radar 5002848 */
  generating_property_type_encoding = 0;

  if (PROPERTY_READONLY (property) == boolean_true_node)
    obstack_grow (&util_obstack, ",R", 2);

  if (PROPERTY_COPY (property) == boolean_true_node)
    obstack_grow (&util_obstack, ",C", 2);

  if (PROPERTY_RETAIN (property) == boolean_true_node)
    obstack_grow (&util_obstack, ",&", 2);

  if (PROPERTY_DYNAMIC (property) == boolean_true_node)
    obstack_grow (&util_obstack, ",D", 2);

  /* APPLE LOCAL begin radar 5407792 */
  if (!IS_ATOMIC (property))
    obstack_grow (&util_obstack, ",N", 2);
  /* APPLE LOCAL end radar 5407792 */

  if (PROPERTY_GETTER_NAME (property))
    {
      obstack_grow (&util_obstack, ",G", 2);
      string = IDENTIFIER_POINTER (PROPERTY_GETTER_NAME (property));
      obstack_grow (&util_obstack, string, strlen (string));
    }
  if (PROPERTY_SETTER_NAME (property))
    {
      obstack_grow (&util_obstack, ",S", 2);
      string = IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (property));
      obstack_grow (&util_obstack, string, strlen (string));
    }
  if (PROPERTY_IVAR_NAME (property))
    {
      obstack_grow (&util_obstack, ",V", 2);
      if (PROPERTY_IVAR_NAME (property) != boolean_true_node)
	string = IDENTIFIER_POINTER (PROPERTY_IVAR_NAME (property));
      else
	string = objc_build_property_ivar_name (property);
      obstack_grow (&util_obstack, string, strlen (string));
    }
    if (flag_objc_gc)
      {
	int g = objc_is_gcable_type (type);
	if (g == -1)
	  obstack_grow (&util_obstack, ",W", 2);
	else if (g == 1)
	  obstack_grow (&util_obstack, ",P", 2);
      }

  obstack_1grow (&util_obstack, 0);    /* null terminate string */
  string = obstack_finish (&util_obstack);
  obstack_free (&util_obstack, util_firstobj);
  return get_identifier (string);
}

/* This routine builds the initializer list to initlize 'struct _prop_t prop_list[]'
   field of 'struct _prop_list_t' meta-data. */

static tree
build_v2_property_table_initializer (tree type)
{
  tree x;
  tree initlist = NULL_TREE;
  generating_instance_variables = 1;
  for (x = IMPL_PROPERTY_DECL (objc_implementation_context); x; x = TREE_CHAIN (x))
    {
      /* NOTE! sections where property name/attribute go MUST change later. */
      tree name_ident = PROPERTY_NAME (x);
      tree elemlist = tree_cons (NULL_TREE,
				 add_objc_string (name_ident, prop_names_attr),
				 NULL_TREE);
      tree attribute = objc_v2_encode_prop_attr (x);
      elemlist = tree_cons (NULL_TREE, add_objc_string (attribute, prop_names_attr),
			    elemlist);

      initlist = tree_cons (NULL_TREE,
			    objc_build_constructor (type, nreverse (elemlist)),
			    initlist);
    }
    generating_instance_variables = 0;
    return objc_build_constructor (build_array_type (type, 0),
				   nreverse (initlist));
}

/* This routine builds the 'struct _prop_list_t' variable declaration and initializes
   it to its initializer list. TYPE is 'struct _prop_list_t', NAME is internal name
   of this variable, SIZE is number of properties for this class and LIST is the
   initializer list for its 'prop_list' field. */

static tree
generate_v2_property_list (tree type, const char *name, int size, tree list)
{
  tree decl, initlist;
  int init_val = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_v2_property_template));

  /* APPLE LOCAL begin radar 6064186 */
  const char * ref_name = synth_id_with_class_suffix (name, objc_implementation_context);
  decl = start_var_decl (type, ref_name);
  set_user_assembler_name (decl, ref_name);
  /* APPLE LOCAL end radar 6064186 */

  initlist = build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, init_val));
  initlist = tree_cons (NULL_TREE, build_int_cst (NULL_TREE, size), initlist);
  initlist = tree_cons (NULL_TREE, list, initlist);

  finish_var_decl (decl,
		   objc_build_constructor (TREE_TYPE (decl),
					   nreverse (initlist)));
  return decl;
}

/* APPLE LOCAL begin radar 5082000 */
#define OUTPUT_LAYOUT_BYTE(BYTE)		  \
	do {					  \
	    obstack_1grow (&util_obstack, BYTE);  \
	    if (print_objc_ivar_layout)		  \
	      {					  \
		if (first_nibble)		  \
		  {		  		  \
		    first_nibble = false;	  \
		    printf ("\n%s for class \'%s\': ", \
			    strong_ivar_layout ?  \
			    "strong ivar layout" :\
			    "weak ivar layout",   \
			    IDENTIFIER_POINTER (CLASS_NAME (implementation_template)));  \
		  }				  \
		if (!(BYTE & 0xf0))		  \
	          printf ("0x0%x%s", BYTE, BYTE != 0 ? ", " : ""); \
		else				  \
	          printf ("0x%x%s", BYTE, BYTE != 0 ? ", " : ""); \
		if (++num_nibbles > 20 		  \
		    || BYTE == 0)		  \
		  { printf("\n"); num_nibbles=1; }\
	      }					  \
	} while (0);				

#define GC_IVAR_BUFSIZE_INCR	256

struct GC_IVAR {
  unsigned int ivar_bytepos;
  unsigned int ivar_size;
};

static struct GC_IVAR *ivar;
static struct GC_IVAR *sk_ivar;
struct skip_scan
  {
    unsigned int skip;
    unsigned int scan;
  };
static struct skip_scan *skip_scan;
static int max_layout_buf_size = 0;

static void
/* Routine for dynamic allocation of various byffers used by GC layout mapper. */
adjust_max_layout_buf_size (int index)
{
  if (index < max_layout_buf_size-1)
    return;
  max_layout_buf_size += GC_IVAR_BUFSIZE_INCR;
  ivar = (struct GC_IVAR*)xrealloc (ivar, sizeof(struct GC_IVAR) * max_layout_buf_size); 
  sk_ivar = (struct GC_IVAR*)xrealloc  (sk_ivar, sizeof(struct GC_IVAR) * max_layout_buf_size); 
  skip_scan = (struct skip_scan*)xrealloc (skip_scan, sizeof(struct GC_IVAR) * max_layout_buf_size); 
  return;
}

static int
ivar_bytepos_cmp (const void *a, const void *b)
{
  HOST_WIDE_INT sa = ((struct GC_IVAR *)a)->ivar_bytepos;
  HOST_WIDE_INT sb = ((struct GC_IVAR *)b)->ivar_bytepos;

  if (sa < sb)
    return -1;
  if (sa > sb)
    return 1;
  return 0;
}

/* This routine returns total count of array elements. Returns 0 if
   there was an error. */
static HOST_WIDE_INT
total_type_elements (tree type)
{
  HOST_WIDE_INT el_count = 1;
  do {
    tree telts = array_type_nelts (type);
    if (telts && host_integerp (telts, 1))
      el_count *= (tree_low_cst (telts, 1) + 1);
    else
      {
        /* error case, reported elsewhere. */
        el_count = 0;
        break;
      }
    type = TREE_TYPE (type);
  } while (TREE_CODE (type) == ARRAY_TYPE);
  return el_count;
}
/* APPLE LOCAL end radar 5082000 */

/* APPLE LOCAL begin radar 6133042 */
/* This routine checks block pointers and either returns
   1 for GCable, or -1 if they are tagged as __weak. */
static int
block_pointer_strong_value (tree type)
{
  gcc_assert (type && TREE_CODE (type) == BLOCK_POINTER_TYPE);
  type = TREE_TYPE (TREE_TYPE (type));
  while (POINTER_TYPE_P (type))
     type = TREE_TYPE (type);
  return (objc_is_strong_p (type) >= 0 ? 1 : -1);
}
/* APPLE LOCAL end radar 6133042 */

static void
objc_build_aggregate_ivar_layout (tree aggr_type, tree field_decl,
				  unsigned int base_byte_pos,
				  bool strong_ivar_layout,
				  int *index,
				  int *sk_index,
				  bool *hasUnion)
{
  int iIndex = *index;
  int iSkIndex = *sk_index;
  unsigned int max_gc_union_ivar_size = 0;
  tree max_field_decl = NULL_TREE;
  unsigned int max_sk_union_ivar_size = 0;
  tree max_sk_field_decl = NULL_TREE;
  bool is_union = (aggr_type && TREE_CODE (aggr_type) == UNION_TYPE);
  unsigned int base = 0;
  /* APPLE LOCAL begin radar 5781140 */
  if (!field_decl)
    return;
  /* APPLE LOCAL end radar 5781140 */
  if (is_union)
    base = base_byte_pos + int_byte_position (field_decl);
 
  do {
    tree type;
    int strong;
    /* APPLE LOCAL objc gc 5547128 */

    /* Unnamed bitfields are ignored. */
    if (!DECL_NAME (field_decl) || DECL_BIT_FIELD_TYPE (field_decl))
      {
        /* APPLE LOCAL radar 5791701 */
        tree last_field_decl = field_decl;
	do
	  field_decl = TREE_CHAIN (field_decl);
	while (field_decl && TREE_CODE (field_decl) != FIELD_DECL);
        /* APPLE LOCAL begin radar 5791701 */
        if (field_decl)
          continue;
        /* last field was a bitfield. Must update the skip info. */
        field_decl = last_field_decl;
        /* APPLE LOCAL end radar 5791701 */
      }
#ifdef OBJCPLUS
    if (TREE_CODE (field_decl) != FIELD_DECL || TREE_STATIC (field_decl))
      {
	/* C++ static members, and things that are not field at all, 
	   are not in the ivar map. */
	do
	  field_decl = TREE_CHAIN (field_decl);
	while (field_decl && TREE_CODE (field_decl) != FIELD_DECL);
	continue;
      }
#endif
    type = TREE_TYPE (field_decl); 
    if (TREE_CODE (type) == RECORD_TYPE || TREE_CODE (type) == UNION_TYPE)
      {
	if (TREE_CODE (type) == UNION_TYPE)
	  *hasUnion = true;
	objc_build_aggregate_ivar_layout (type, TYPE_FIELDS (type), 
					  base_byte_pos + int_byte_position (field_decl),
					  strong_ivar_layout,
					  &iIndex, 
					  &iSkIndex, 
					  hasUnion);
	do
	  field_decl = TREE_CHAIN (field_decl);
	while (field_decl && TREE_CODE (field_decl) != FIELD_DECL);
	continue;
      }
    /* APPLE LOCAL begin radar 5082000 */
    else if (TREE_CODE (type) == ARRAY_TYPE)
      {
        tree inner = TREE_TYPE (type);
	while (TREE_CODE (inner) == ARRAY_TYPE)
	  inner = TREE_TYPE (inner);
        if (TREE_CODE (inner) == UNION_TYPE)
	  error ("Layout for array of unions not supported");
        else if (TREE_CODE (inner) == RECORD_TYPE)
	  {
	    HOST_WIDE_INT el_count = total_type_elements (type);
	    HOST_WIDE_INT el_ix;
	    int old_iIndex = iIndex, old_iSkIndex = iSkIndex;
	    int first_iIndex, first_iSkIndex;
	    objc_build_aggregate_ivar_layout (inner, TYPE_FIELDS (inner), 
					      base_byte_pos + int_byte_position (field_decl),
					      strong_ivar_layout,
					      &iIndex, 
					      &iSkIndex, 
					      hasUnion);
	    /* Replicate layout information for each array element. Note that
	       one element is already done. */
	    first_iIndex = iIndex;
	    first_iSkIndex = iSkIndex;
	    for (el_ix = 1; el_ix < el_count; el_ix++)
	      {
		int i;
		int size = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (inner));
		for (i = old_iIndex+1; i <= first_iIndex; ++i)
		  {
		    adjust_max_layout_buf_size (iIndex);
		    ivar[++iIndex].ivar_bytepos = ivar[i].ivar_bytepos + size*el_ix;
		    ivar[iIndex].ivar_size = ivar[i].ivar_size;
		  }
		for (i = old_iSkIndex+1; i <= first_iSkIndex; ++i)
		  {
		    adjust_max_layout_buf_size (iSkIndex);
		    sk_ivar[++iSkIndex].ivar_bytepos = sk_ivar[i].ivar_bytepos + size*el_ix;
		    sk_ivar[iSkIndex].ivar_size = sk_ivar[i].ivar_size;
		  }
	      }
	    do
	      field_decl = TREE_CHAIN (field_decl);
	    while (field_decl && TREE_CODE (field_decl) != FIELD_DECL);
	    continue;
	  }	
      }
    /* APPLE LOCAL end radar 5082000 */

    strong = 0;
    do
      {
        strong = objc_is_strong_p (type);
        if (strong)
          break;

	/* APPLE LOCAL begin objc gc 5547128 */
        if (TREE_CODE (type) == FUNCTION_TYPE
	    || TREE_CODE (type) == METHOD_TYPE)
          break;
	/* APPLE LOCAL end objc gc 5547128 */

        if (objc_is_object_ptr (type))
	/* APPLE LOCAL begin objc gc 5547128 */
	  {
	    strong = ((objc_is_strong_p (TREE_TYPE (type)) >= 0) ? 1 : -1);
	    break;
	  }
	/* APPLE LOCAL end objc gc 5547128 */
	/* APPLE LOCAL begin radar 6133042 */
        if (TREE_CODE (type) == BLOCK_POINTER_TYPE)
          {
            strong = block_pointer_strong_value (type);
            break;
          }
	/* APPLE LOCAL end radar 6133042 */
        type = TREE_TYPE (type);
      }
    while (!strong && type);

    if ((strong_ivar_layout && strong == 1)
        || (!strong_ivar_layout && strong == -1))
      {
	if (is_union)
	  {
	    unsigned int u_ivar_size = TREE_INT_CST_LOW (DECL_SIZE_UNIT (field_decl))
				       / GET_MODE_SIZE (Pmode);
	    if (u_ivar_size > max_gc_union_ivar_size)
	      {
	        max_gc_union_ivar_size = u_ivar_size;	    
	        max_field_decl = field_decl;
	      }
	  }
	else
	  {  
	    adjust_max_layout_buf_size (iIndex);
            ivar[++iIndex].ivar_bytepos = base_byte_pos + int_byte_position (field_decl);
            ivar[iIndex].ivar_size = TREE_INT_CST_LOW (DECL_SIZE_UNIT (field_decl))
                                     / GET_MODE_SIZE (Pmode);
	  }
      }
    else if ((strong_ivar_layout && strong <= 0)
	     || (!strong_ivar_layout && strong != -1))
      {
	if (is_union)
	  {
	    unsigned int u_ivar_size = TREE_INT_CST_LOW (DECL_SIZE_UNIT (field_decl));
	    if (u_ivar_size > max_sk_union_ivar_size)
	      {
	        max_sk_union_ivar_size = u_ivar_size;	    
	        max_sk_field_decl = field_decl;
	      }
	  }
	else
	  {  
	    adjust_max_layout_buf_size (iSkIndex);
            sk_ivar[++iSkIndex].ivar_bytepos = base_byte_pos + int_byte_position (field_decl);
            sk_ivar[iSkIndex].ivar_size = TREE_INT_CST_LOW (DECL_SIZE_UNIT (field_decl));
	  }
      }

    do
      field_decl = TREE_CHAIN (field_decl);
    while (field_decl && TREE_CODE (field_decl) != FIELD_DECL);
  }
  while (field_decl);
  if (max_field_decl)
    {
      adjust_max_layout_buf_size (iIndex);
      ivar[++iIndex].ivar_bytepos = base_byte_pos + int_byte_position (max_field_decl);
      ivar[iIndex].ivar_size = max_gc_union_ivar_size;
    }
  if (max_sk_field_decl)
    {
      adjust_max_layout_buf_size (iSkIndex);
      sk_ivar[++iSkIndex].ivar_bytepos = base_byte_pos + int_byte_position (max_sk_field_decl);
      sk_ivar[iSkIndex].ivar_size = max_sk_union_ivar_size;
    }
  *index = iIndex;
  *sk_index = iSkIndex;
}


/* This routine builds the ivar_layout (when STRONG_IVAR_LAYOUT is true), or otherwise
   weak_ivar_layout and returns it in a c-string. The layout map displays which words
   in ivar list must be skipped and which must be scanned by GC (see below). 
   String is built of bytes. Each byte is divided up in two nibbles (4-bit each). Left
   nibble is count of words to skip and right nibble is count of words to scan. So, each
   nibble represents up to 15 workds to skip or scan. Skipping the rest is represented
   by a 0x00 byte which also ends the string.

   1. when STRONG_IVAR_LAYOUT is true, following ivars are scanned:
	- id, Class
	- object *
	- __strong anything

   2. When STRONG_IVAR_LAYOUT is false, following ivars are scanned:
	- __weak anything
*/

static tree 
objc_build_ivar_layout (bool strong_ivar_layout)
{
  tree field_decl;
  unsigned int words_to_skip, words_to_scan;
  int i;
  int iIndex = -1;
  int iSkIndex = -1;
  int iSkip_scan;
  unsigned char byte;
  const char *string;
  bool hasUnion, bytesSkipped;
  int num_nibbles = 0;
  bool first_nibble = true;
  /* APPLE LOCAL radar 6003871 */
  tree retval;

  if (!flag_objc_gc || !implementation_template)
    return NULL_TREE;

  field_decl = CLASS_IVARS (implementation_template);
  if (CLASS_SUPER_NAME (implementation_template))
    {
      /* Must build an ivar-list of all ivars in the inheritiance hierarchy. */
      tree interface = implementation_template;
      field_decl = copy_list (field_decl);
      while (CLASS_SUPER_NAME (interface))
	{
	  interface = lookup_interface (CLASS_SUPER_NAME (interface));
	  field_decl = chainon (copy_list (CLASS_IVARS (interface)),
				field_decl);
	}
    }
  if (!field_decl)
    return NULL_TREE;

  hasUnion = false;
  if (max_layout_buf_size == 0)
    {
      gcc_assert (ivar == NULL && sk_ivar == NULL && skip_scan == NULL);
      adjust_max_layout_buf_size (GC_IVAR_BUFSIZE_INCR);
    }
  objc_build_aggregate_ivar_layout (NULL_TREE, field_decl, 0, strong_ivar_layout, 
				    &iIndex, &iSkIndex, &hasUnion);

  if (iIndex == -1)
    {
      /* Skip everything */
      return NULL_TREE;
    }

  /* Must sort on byte position in case we encounterred a union nested in
     the ivar list. */
  if (hasUnion && iIndex > 0)
    qsort (ivar, iIndex+1, sizeof (struct GC_IVAR), ivar_bytepos_cmp);

  if (hasUnion && iSkIndex > 0)
    qsort (sk_ivar, iSkIndex+1, sizeof (struct GC_IVAR), ivar_bytepos_cmp);

  /* Now build the string of skip/scan nibbles */
  /* Initialization */
  iSkip_scan = -1;

  if (ivar[0].ivar_bytepos == 0)
    {
      words_to_skip = 0;
      words_to_scan = ivar[0].ivar_size;
    }
  else
    {
      words_to_skip = ivar[0].ivar_bytepos / GET_MODE_SIZE (Pmode);
      words_to_scan = ivar[0].ivar_size;
    }

  for (i=1; i <= iIndex; i++)
    {
      unsigned int tail_prev_gc_obj = ivar[i-1].ivar_bytepos + 
				      ivar[i-1].ivar_size * GET_MODE_SIZE (Pmode);
      if (ivar[i].ivar_bytepos == tail_prev_gc_obj)
	{
	  /* consecutive 'scanned' object pointers. */
	  words_to_scan += ivar[i].ivar_size;
	}
      else
	{
	  /* Skip over 'gc'able object pointer which lay over each other. */
	  if (tail_prev_gc_obj > ivar[i].ivar_bytepos)
	    continue;
	  /* Must skip over 1 or more words. We save current skip/scan values
	     and start a new pair. */
	  adjust_max_layout_buf_size (iSkip_scan);
	  skip_scan[++iSkip_scan].skip = words_to_skip;
	  skip_scan[iSkip_scan].scan = words_to_scan;
	  /* APPLE LOCAL begin radar 5109807 */
	  /* Skip the hole */
	  skip_scan[++iSkip_scan].skip = (ivar[i].ivar_bytepos - tail_prev_gc_obj)
			  		 / GET_MODE_SIZE (Pmode);
	  skip_scan[iSkip_scan].scan = 0;
	  words_to_skip = 0;
	  /* APPLE LOCAL end radar 5109807 */
	  words_to_scan = ivar[i].ivar_size;
	}
    }
  if (words_to_scan > 0)
    {
      adjust_max_layout_buf_size (iSkip_scan);
      skip_scan[++iSkip_scan].skip = words_to_skip;
      skip_scan[iSkip_scan].scan = words_to_scan;
    }
  /* APPLE LOCAL begin radar 5251019 */
  bytesSkipped = false;
  if (iSkIndex >= 0)
    {
      int lastByteSkipped = sk_ivar[iSkIndex].ivar_bytepos + sk_ivar[iSkIndex].ivar_size;
      int lastByteScanned = ivar[iIndex].ivar_bytepos + ivar[iIndex].ivar_size * GET_MODE_SIZE (Pmode);
      bytesSkipped = (lastByteSkipped > lastByteScanned);
      /* Compute number of bytes to skip at the tail end of the last ivar scanned. */
      if (bytesSkipped)
        {
          unsigned int totalWord = (lastByteSkipped + (GET_MODE_SIZE (Pmode) -1))
                                   / GET_MODE_SIZE (Pmode);
          adjust_max_layout_buf_size (iSkip_scan);
          skip_scan[++iSkip_scan].skip = totalWord - (lastByteScanned / GET_MODE_SIZE (Pmode));
          skip_scan[iSkip_scan].scan = 0;
        }
    }
  /* APPLE LOCAL end radar 5251019 */
 
  /* APPLE LOCAL begin radar 5259868 */
  /* Mini optimization of nibbles such that an 0xM0 followed by 0x0N is produced
     as 0xMN. */
  for (i = 0; i <= iSkip_scan; i++)
    {
      if (i < iSkip_scan)
	{
	  if (skip_scan[i].skip && skip_scan[i].scan == 0
	      && skip_scan[i+1].skip == 0 && skip_scan[i+1].scan)
	    {
	      /* 0xM0 followed by 0x0N detected. */
	      int j;
	      skip_scan[i].scan = skip_scan[i+1].scan;
	      for (j = i+1; j < iSkip_scan; j++)
		skip_scan[j] = skip_scan[j+1];
	      --iSkip_scan;
	    }
	} 
    }
  /* APPLE LOCAL end radar 5259868 */
  /* Now generate the string */
  for (i = 0; i <= iSkip_scan; i++)
    {
      unsigned int skip_small = skip_scan[i].skip % 0xf;
      unsigned int scan_small = skip_scan[i].scan % 0xf;
      unsigned int skip_big  = skip_scan[i].skip / 0xf;
      unsigned int scan_big  = skip_scan[i].scan / 0xf;
      unsigned int ix;
      if (skip_small > 0 || skip_big > 0)
	bytesSkipped = true;
      /* APPLE LOCAL begin radar 5217964 */
      /* first skip big. */
      for (ix = 0; ix < skip_big; ix++)
        OUTPUT_LAYOUT_BYTE (0xf0);

      /* next (skip small, scan) */
      if (skip_small)
	{
	  byte = skip_small << 4;
	  if (scan_big > 0)
	    {
	      byte |= 0xf;
	      --scan_big;
	    }
	  else if (scan_small)
	    {
	      byte |= scan_small;
	      scan_small = 0;
	    }
	  OUTPUT_LAYOUT_BYTE (byte);
	}
      /* next scan big */
      for (ix = 0; ix < scan_big; ix++)
	OUTPUT_LAYOUT_BYTE (0x0f);
      /* last scan small */
      if (scan_small)
	{
	  byte = scan_small; 
	  OUTPUT_LAYOUT_BYTE (byte);
	}
       /* APPLE LOCAL end radar 5217964 */
    }
  OUTPUT_LAYOUT_BYTE (0);  /* null terminate string */
  string = obstack_finish (&util_obstack);
  /* APPLE LOCAL begin radar 6003871 */
  /* if ivar_layout bitmap is all 1 bits (nothing skipped) then use NULL as
     final layout. */
  retval =  (strong_ivar_layout && !bytesSkipped) 
	     ? NULL_TREE 
	     : add_objc_string (get_identifier (string), class_names);
  obstack_free (&util_obstack, util_firstobj);
  return retval;
  /* APPLE LOCAL end radar 6003871 */
}

/**
  This routine build following type:

  struct _objc_protocol_extension
    {
      uint32_t size;	// sizeof (struct _objc_protocol_extension)
      struct objc_method_list	*optional_instance_methods;
      struct objc_method_list   *optional_class_methods;
      struct objc_prop_list	*instance_properties;
    }
*/

static void build_objc_protocol_extension_template (void)
{
  tree field_decl, field_decl_chain;
  
  objc_protocol_extension_template = start_struct (RECORD_TYPE,
						   get_identifier (UTAG_PROTOCOL_EXT));
  /*  uint32_t size; */
  field_decl = create_field_decl (integer_type_node, "size");
  field_decl_chain = field_decl;

  /* struct objc_method_list   *optional_instance_methods; */
  field_decl = create_field_decl (objc_method_list_ptr, "optional_instance_methods");
  chainon (field_decl_chain, field_decl);

  /* struct objc_method_list   *optional_class_methods; */
  field_decl = create_field_decl (objc_method_list_ptr, "optional_class_methods");
  chainon (field_decl_chain, field_decl);

  /* struct objc_prop_list     *instance_properties; */
  field_decl = create_field_decl (objc_prop_list_ptr, "instance_properties");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_protocol_extension_template, field_decl_chain, NULL_TREE);
}

static tree
generate_objc_protocol_extension (tree proto_interface, tree optional_instance_meth, 
				  tree optional_class_meth, tree instance_prop)
{
  int size;
  tree decl, initlist, expr;

  if (!optional_instance_meth && !optional_class_meth && !instance_prop)
    return NULL_TREE;

  if (!objc_protocol_extension_template)
    build_objc_protocol_extension_template ();    

  /* uint32_t size */
  size = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_protocol_extension_template));
  initlist = build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, size));

  /* struct objc_method_list   *optional_instance_methods; */
  if (!optional_instance_meth)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_method_list_ptr, integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_method_list_ptr, build_unary_op (ADDR_EXPR, 
					      optional_instance_meth, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  /* struct objc_method_list   *optional_class_methods; */
  if (!optional_class_meth)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_method_list_ptr, integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_method_list_ptr, build_unary_op (ADDR_EXPR, 
					      optional_class_meth, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  /* struct objc_prop_list     *instance_properties; */
  if (!instance_prop)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_prop_list_ptr, integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_prop_list_ptr, build_unary_op (ADDR_EXPR, 
							  instance_prop, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  decl = start_var_decl (objc_protocol_extension_template, 
                         synth_id_with_class_suffix 
                           ("_OBJC_PROTOCOLEXT", proto_interface));
  expr = objc_build_constructor (TREE_TYPE (decl), nreverse (initlist));
  finish_var_decl (decl, expr);
  return decl;
}

/* This routine build following struct type:
   struct _objc_class_ext 
     {
       uint32_t size;	// sizeof(struct _objc_class_ext)
       const char *weak_ivar_layout;
       struct _prop_list_t *properties;
     }
*/

static void
build_objc_class_ext_template (void)
{
  tree field_decl, field_decl_chain;

  objc_class_ext_template = start_struct (RECORD_TYPE,
					  get_identifier (UTAG_CLASS_EXT));

  /* uint32_t size; */
  field_decl = create_field_decl (integer_type_node, "size");
  field_decl_chain = field_decl;

  /* const char *weak_ivar_layout; */
  field_decl = create_field_decl (string_type_node, "weak_ivar_layout");
  chainon (field_decl_chain, field_decl);

  /* struct _prop_list_t *properties; */
  field_decl 
   = create_field_decl (build_pointer_type (xref_tag (RECORD_TYPE,
                                            get_identifier(UTAG_PROPERTY_LIST))),
                                  	    "properties");
  chainon (field_decl_chain, field_decl);
  finish_struct (objc_class_ext_template, field_decl_chain, NULL_TREE);
}

/* This routine declares UOBJC_CLASS_EXT_decl and initializes its 'size',
   'weak_ivar_layout' (with WEAK_IVAR_LAYOUT) and 'properties' (with
   PROPERTY_LIST).
*/

static tree
generate_objc_class_ext (tree property_list)
{
  tree decl, initlist, expr;
  int size;
  tree weak_ivar_layout_tree;

  /* const char *weak_ivar_layout */
  weak_ivar_layout_tree = objc_build_ivar_layout (false);
  if (!property_list && !weak_ivar_layout_tree)
    return NULL_TREE;

  if (!objc_class_ext_template)
    build_objc_class_ext_template ();

  /* uint32_t size */
  size = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_class_ext_template));
  initlist = build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, size));

  initlist = tree_cons (NULL_TREE,
			(!weak_ivar_layout_tree ? integer_zero_node
					        : weak_ivar_layout_tree),
			initlist);

  /* struct _prop_list_t *properties; */
  if (!property_list)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_prop_list_ptr, integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_prop_list_ptr,
                      build_unary_op (ADDR_EXPR, property_list, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  decl = start_var_decl (objc_class_ext_template,
		         synth_id_with_class_suffix
			   ("_OBJC_CLASSEXT", objc_implementation_context));
  expr = objc_build_constructor (TREE_TYPE (decl), nreverse (initlist));
  finish_var_decl (decl, expr);
  return decl;
}

/* Top-level routine to generate property tables for each implementation.
   PROTOCOL, if not NULL, is the PROTOCOL_INTERFACE_TYPE of the protocol
   whose property meta-data must be generated, via @implementation <protoname>
   syntax.  */

static void
generate_v2_property_tables (tree protocol)
{
  tree x, initlist, property_list_template;
  int size = 0;
  x = protocol ? CLASS_PROPERTY_DECL (protocol)
	       : IMPL_PROPERTY_DECL (objc_implementation_context);
  for (; x; x = TREE_CHAIN (x))
    size++;

  if (size == 0)
    {
      UOBJC_V2_PROPERTY_decl = NULL_TREE;
      return;
    }

  if (!objc_v2_property_template)
    objc_v2_property_template = build_v2_property_template ();
  property_list_template = build_v2_property_list_template (objc_v2_property_template,
							    size);
  initlist
    = build_v2_property_table_initializer (objc_v2_property_template);
  /* A property meta-data can be generated via @implementation <protoname> or
     via implementation of an interface where properties are declaraed. Make sure
     name of property meta-data is unique for each case. */
  UOBJC_V2_PROPERTY_decl = generate_v2_property_list (
			     property_list_template,
			     /* APPLE LOCAL begin radar 6064186 */
			     protocol ? "l_OBJC_$_PROP_PROTO_LIST"
				      : "l_OBJC_$_PROP_LIST",
			     /* APPLE LOCAL end radar 6064186 */
			     size, initlist);
}

/* This routine merges properties undeclared in implementation from its interface 
   declaration.  */

static void
objc_v2_merge_property (void)
{
  tree property_decl;
  tree interface = NULL_TREE;
  tree x, y;

  gcc_assert (objc_implementation_context);
  interface = lookup_interface (CLASS_NAME (objc_implementation_context));
  /* May be null due to some previously declared diagnostic. */
  if (!interface)
    return;

  if (TREE_CODE (objc_implementation_context) == CATEGORY_IMPLEMENTATION_TYPE)
    {
      interface = lookup_category (interface,
				   CLASS_SUPER_NAME (objc_implementation_context));
      if (!interface)
        return;
    }

  /* Check for properties declared in interface but not in implementations.
     These are 'dynamic' properties. */
  for (x = CLASS_PROPERTY_DECL (interface); x; x = TREE_CHAIN (x))
    {
      for (y = IMPL_PROPERTY_DECL (objc_implementation_context); y; y = TREE_CHAIN (y))
	 if (PROPERTY_NAME (x) == PROPERTY_NAME (y))
	   break;
      if (y)
	continue;
      else 
	{
	  property_decl = copy_node (x);
	  /* Add the property to the list of properties for current implementation. */
	  TREE_CHAIN (property_decl) = IMPL_PROPERTY_DECL (objc_implementation_context);
	  IMPL_PROPERTY_DECL (objc_implementation_context) = property_decl;
	  continue;
	}
      /* 'x' is dynamic propeperty. Add it to the list of properties for this
	  implementation. */	
      property_decl = make_node (PROPERTY_DECL);
      TREE_TYPE (property_decl) = TREE_TYPE (x);
      PROPERTY_NAME (property_decl) = PROPERTY_NAME (x);
      PROPERTY_READONLY (property_decl) = PROPERTY_READONLY (x);
      PROPERTY_DYNAMIC (property_decl) = PROPERTY_DYNAMIC (x);
      PROPERTY_GETTER_NAME (property_decl) = NULL_TREE;
      PROPERTY_SETTER_NAME (property_decl) = NULL_TREE;
      PROPERTY_IVAR_NAME (property_decl) = PROPERTY_IVAR_NAME (x);
      /* Add the property to the list of properties for current implementation. */
      TREE_CHAIN (property_decl) = IMPL_PROPERTY_DECL (objc_implementation_context);
      IMPL_PROPERTY_DECL (objc_implementation_context) = property_decl;
    }
  return;
}
/* APPLE LOCAL end C* language */

void
objc_set_method_type (enum tree_code type)
{
  objc_inherit_code = (type == PLUS_EXPR
		       ? CLASS_METHOD_DECL
		       : INSTANCE_METHOD_DECL);
}


/* APPLE LOCAL begin radar 6307941 */
/* Non-legacy dispatch is a performance optimization only. 
   Using the "wrong" dispatch is not a correctness problem, 
   assuming non-legacy dispatch is supported at all. 

   SnowLeopard: only the selectors below are optimized by the runtime. 
     Other selectors should use legacy dispatch to save memory.
*/
static const char * const nonlegacy_dispatched_selectors[] = {
  "allocWithZone:",
  "alloc",
  "class",
  "self",
  "isKindOfClass:",
  "respondsToSelector:",
  "isFlipped",
  "length",
  "objectForKey:",
  "count",
  "objectAtIndex:",
  "isEqualToString:",
  "isEqual:",
  "retain",
  "release",
  "autorelease",
  "hash",
  "addObject:",
  "countByEnumeratingWithState:objects:count:",
};

static int
objc_legacy_dispatched_selector_p (tree sel_name)
{
  gcc_assert (flag_objc_abi == 2);
  gcc_assert (sel_name);
  if (flag_objc_legacy_dispatch) 
    return true;
  if (OBJC2_ABI_DISPATCH)
    /* Do this for Snow Leopard ot later only. */
    return false;
  else {
    size_t i, count;
    const char *sel_cname = IDENTIFIER_POINTER (sel_name);
    count = sizeof(nonlegacy_dispatched_selectors) / sizeof(nonlegacy_dispatched_selectors[0]);
    for (i = 0; i < count; i++) 
    {
      if (!strcmp (sel_cname, nonlegacy_dispatched_selectors[i]))
        return false;
    }
  } 
  return true;
}
/* APPLE LOCAL end radar 6307941 */

tree
objc_build_method_signature (tree rettype, tree selector,
			     tree optparms, bool ellipsis)
{
  return build_method_decl (objc_inherit_code, rettype, selector,
			    optparms, ellipsis);
}

/* APPLE LOCAL begin radar 4869979 */
/* Return 'true' if chain of attributes includes 'bycopy' */
static bool
objc_type_has_bycopy_attr (tree list)
{
  tree p;
  for (p = list; p; p = TREE_CHAIN (p))
    {
      tree v = TREE_VALUE (p);
      if (v && TREE_CODE (v) == IDENTIFIER_NODE
	  && !strcmp (IDENTIFIER_POINTER (v), "bycopy"))
        return true;
    }
  return false;
}

/* This routine issues a warning if instance method declaration has a 'bycopy' 
   attribute of an object which does not include 'NSCoding' protocol. */
static void
objc_diagnose_bycopy_type (tree decl)
{
  tree type = TREE_TYPE (decl);
  if (type && TREE_CODE (type) == TREE_LIST)
    {
      tree p = TREE_PURPOSE (type);
      if (p && TREE_CODE (p) == TREE_LIST)
	{
	  if (objc_type_has_bycopy_attr (p))
	    {
	      tree intf;
	      tree objc_class = objc_is_object_ptr (TREE_VALUE (type));
	      if (objc_class && objc_is_class_name (objc_class)
		  && (intf = lookup_interface (objc_class)))
		{
		  tree proto = lookup_protocol (get_identifier ("NSCoding"));
		  if (proto && !conforms_to_protocol (intf, proto))
		    warning (0, "\"bycopy\" specified on instance method type of '%T' that does not conform to \"NSCoding\" protocol",
			     TREE_VALUE (type));
		}
	    } 
	}
    }
}
/* APPLE LOCAL end radar 4869979 */

void
/* APPLE LOCAL radar 3803157 - objc attribute */
objc_add_method_declaration (tree decl, tree attributes)
{
  /* APPLE LOCAL begin radar 4157812 */
  if (decl == error_mark_node)
    return;
  /* APPLE LOCAL end radar 4157812 */

  if (!objc_interface_context)
    fatal_error ("method declaration not in @interface context");

  /* APPLE LOCAL radar 3803157 - objc attribute */
  objc_decl_method_attributes (&decl, attributes, 0);
  /* APPLE LOCAL begin C* language */
  objc_add_method (objc_interface_context,
		   decl,
		   objc_inherit_code == CLASS_METHOD_DECL,
		   objc_method_optional_flag);
  /* APPLE LOCAL end C* language */
  /* APPLE LOCAL begin radar 4869979 */
  if (objc_interface_context &&
      TREE_CODE (objc_interface_context) == CLASS_INTERFACE_TYPE
      && objc_inherit_code == INSTANCE_METHOD_DECL)
    objc_diagnose_bycopy_type (decl);
  /* APPLE LOCAL end radar 4869979 */
}

/* APPLE LOCAL begin deprecated use in deprecated 6425499 */
static tree fast_lookup_method (int is_class, tree class, tree method_ident);
/* APPLE LOCAL end deprecated use in deprecated 6425499 */

void
/* APPLE LOCAL radar 3803157 - objc attribute */
objc_start_method_definition (tree decl, tree attributes)
{
  /* APPLE LOCAL begin deprecated use in deprecated 6425499 */
  tree class;
  /* APPLE LOCAL end deprecated use in deprecated 6425499 */

  if (!objc_implementation_context)
    fatal_error ("method definition not in @implementation context");

  /* APPLE LOCAL begin radar 4290840 */
  if (decl != NULL_TREE  && METHOD_SEL_NAME (decl) == error_mark_node)
    return;
  /* APPLE LOCAL end radar 4290840 */

  /* APPLE LOCAL begin radar 4219590 */
#ifndef OBJCPLUS
  /* Indicate no valid break/continue context by setting these variables
     to some non-null, non-label value.  We'll notice and emit the proper
     error message in c_finish_bc_stmt.  */
  c_break_label = c_cont_label = size_zero_node;
#endif
  /* APPLE LOCAL end radar 4219590 */

  /* APPLE LOCAL begin radar 4529765 */
  if (attributes)
    warning (0, "method attribute may be specified on method declarations only");
  /* APPLE LOCAL end radar 4529765 */
  /* APPLE LOCAL begin radar 5424416 */
  if (TREE_TYPE (decl) && TREE_CODE (TREE_TYPE (decl)) == TREE_LIST)
  {
    tree type = TREE_VALUE (TREE_TYPE (decl));
    if (type)
    {
      if (TREE_CODE (type) == FUNCTION_TYPE)
        {
          error ("%qs declared as method returning a function", 
                 IDENTIFIER_POINTER (DECL_NAME (decl)));
	}
      else if (TREE_CODE (type) == ARRAY_TYPE)
	{
          error ("%qs declared as method returning an array", 
                 IDENTIFIER_POINTER (DECL_NAME (decl)));
	}
    }
  }
  /* APPLE LOCAL end radar 5424416 */
  /* APPLE LOCAL radar 3803157 - objc attribute */
  objc_decl_method_attributes (&decl, attributes, 0);
  objc_add_method (objc_implementation_context,
		   decl,
		   /* APPLE LOCAL C* language */
		   objc_inherit_code == CLASS_METHOD_DECL, 0);
  /* APPLE LOCAL begin deprecated use in deprecated 6425499 */
  /* We have to copy the TREE_DEPRECATED bit from the interface.  */
  class = lookup_interface (CLASS_NAME (objc_implementation_context));
  if (class)
    {
      tree decl2 = fast_lookup_method (objc_inherit_code == CLASS_METHOD_DECL,
				       class,
				       METHOD_SEL_NAME (decl));
      if (decl2)
	TREE_DEPRECATED (decl) = TREE_DEPRECATED (decl2);
  }
  /* APPLE LOCAL end deprecated use in deprecated 6425499 */
  start_method_def (decl);

  /* APPLE LOCAL begin ObjC abi v2 */
  /* APPLE LOCAL radar 4581680 */
  if (flag_objc_abi == 2 && objc_inherit_code == CLASS_METHOD_DECL)
    {
      /* Insert declaration of class method "load" in one of the __nonlazy_class
         or __nonlazy_category lists. */
      tree id = DECL_NAME (decl);
      if (id && TREE_CODE (id) == IDENTIFIER_NODE)
        {
          const char *name = IDENTIFIER_POINTER (id);
	  if (!strcmp (name, "load"))
	    CLASS_OR_CATEGORY_HAS_LOAD_IMPL (objc_implementation_context) = decl;
	}
    }
  /* APPLE LOCAL end ObjC abi v2 */
}

void
objc_add_instance_variable (tree decl)
{
  (void) add_instance_variable (objc_ivar_context,
				objc_public_flag,
				decl);
}

/* Return 1 if IDENT is an ObjC/ObjC++ reserved keyword in the context of
   an '@'.  */

int
objc_is_reserved_word (tree ident)
{
  unsigned char code = C_RID_CODE (ident);

  return (OBJC_IS_AT_KEYWORD (code)
#ifdef OBJCPLUS
          /* APPLE LOCAL radar 4564694 */
          || code == RID_AT_PACKAGE
	  || code == RID_CLASS || code == RID_PUBLIC
	  || code == RID_PROTECTED || code == RID_PRIVATE
	  || code == RID_TRY || code == RID_THROW || code == RID_CATCH
#endif
	    );
}

/* Return true if TYPE is 'id'.  */

static bool
objc_is_object_id (tree type)
{
  return OBJC_TYPE_NAME (type) == objc_object_id;
}

static bool
objc_is_class_id (tree type)
{
  return OBJC_TYPE_NAME (type) == objc_class_id;
}

/* Construct a C struct with same name as CLASS, a base struct with tag
   SUPER_NAME (if any), and FIELDS indicated.  */

static tree
objc_build_struct (tree class, tree fields, tree super_name)
{
  tree name = CLASS_NAME (class);
  tree s = start_struct (RECORD_TYPE, name);
  tree super = (super_name ? xref_tag (RECORD_TYPE, super_name) : NULL_TREE);
  /* APPLE LOCAL begin radar 5676962 */
  tree t;
  int cv = 0;
  /* APPLE LOCAL radar 6285794 */
  struct lang_type ** pTypeLang = 0;
  /* APPLE LOCAL end radar 5676962 */

  /* APPLE LOCAL begin radar 5025001 */
  if (flag_objc_abi == 2 && TREE_CODE (class) == CLASS_INTERFACE_TYPE
      && fields != NULL_TREE)
    {
      /* append char :0; field if last field is a bit field. */
      tree last = fields;
      while (TREE_CHAIN (last))
        last = TREE_CHAIN (last);
      gcc_assert (TREE_CODE (last) == FIELD_DECL);
      if (DECL_INITIAL (last) && DECL_INITIAL (last) != error_mark_node)
        {
          tree bf_zero = build_decl (FIELD_DECL, NULL_TREE, char_type_node);
          DECL_INITIAL (bf_zero) = size_zero_node;
          SET_DECL_C_BIT_FIELD (bf_zero);
          DECL_NONADDRESSABLE_P (bf_zero) = 1;
          TREE_CHAIN (last) = bf_zero;
        }
    }
  /* APPLE LOCAL end radar 5025001 */

  if (super)
    {
      /* Prepend a packed variant of the base class into the layout.  This
	 is necessary to preserve ObjC ABI compatibility.  */
      tree base = build_decl (FIELD_DECL, NULL_TREE, super);
      tree field = TYPE_FIELDS (super);

      while (field && TREE_CHAIN (field)
	     && TREE_CODE (TREE_CHAIN (field)) == FIELD_DECL)
	field = TREE_CHAIN (field);

      /* For ObjC ABI purposes, the "packed" size of a base class is the
	 the sum of the offset and the size (in bits) of the last field
	 in the class.  */
      DECL_SIZE (base)
	= (field && TREE_CODE (field) == FIELD_DECL
	   ? size_binop (PLUS_EXPR,
			 size_binop (PLUS_EXPR,
				     size_binop
				     (MULT_EXPR,
				      convert (bitsizetype,
					       DECL_FIELD_OFFSET (field)),
				      bitsize_int (BITS_PER_UNIT)),
				     DECL_FIELD_BIT_OFFSET (field)),
			 DECL_SIZE (field))
	   : bitsize_zero_node);
      DECL_SIZE_UNIT (base)
	= size_binop (FLOOR_DIV_EXPR, convert (sizetype, DECL_SIZE (base)),
		      size_int (BITS_PER_UNIT));
      DECL_ARTIFICIAL (base) = 1;
      DECL_ALIGN (base) = 1;
      DECL_FIELD_CONTEXT (base) = s;
      /* APPLE LOCAL begin radar 4477797 */
      /* LLVM LOCAL begin */
      /* Do not check write-symbols in llvm-gcc. */
	 DECL_IGNORED_P (base) = 1;
      /* LLVM LOCAL end */
      /* APPLE LOCAL end radar 4477797 */
#ifdef OBJCPLUS
      DECL_FIELD_IS_BASE (base) = 1;

      if (fields)
	TREE_NO_WARNING (fields) = 1;	/* Suppress C++ ABI warnings -- we   */
#endif					/* are following the ObjC ABI here.  */
      TREE_CHAIN (base) = fields;
      fields = base;
    }

  /* NB: Calling finish_struct() may cause type TYPE_LANG_SPECIFIC fields
     in all variants of this RECORD_TYPE to be clobbered, but it is therein
     that we store protocol conformance info (e.g., 'NSObject <MyProtocol>').
     Hence, we must squirrel away the ObjC-specific information before calling
     finish_struct(), and then reinstate it afterwards.  */

  /* APPLE LOCAL begin radar 5676962 */
#if 0
     removed:
  for (t = TYPE_NEXT_VARIANT (s); t; t = TYPE_NEXT_VARIANT (t))
    objc_info
      = chainon (objc_info,
		 build_tree_list (NULL_TREE, TYPE_OBJC_INFO (t)));
#endif
  /* APPLE LOCAL end radar 5676962 */

  /* Point the struct at its related Objective-C class.  */
  INIT_TYPE_OBJC_INFO (s);
  TYPE_OBJC_INTERFACE (s) = class;

  /* APPLE LOCAL begin radar 5676962, 4310884, 4945770 */
  /* Call to finish_struct has the side-effect of unifying TYPE_LANG_SPECIFIC
     of all variants to be same as the main variant's. We must preseve objc-c's
     TYPE_LANG_SPECIFIC objects as they have unique information in them; such as
     TYPE_OBJC_INFO. So, we save and restore TYPE_LANG_SPECIFIC objects. */
  for (t = TYPE_MAIN_VARIANT (s); t && TYPE_LANG_SPECIFIC (t) && TYPE_OBJC_INFO (t); 
       t = TYPE_NEXT_VARIANT (t))
    ++cv;
  if (cv > 1) {
    int i=0;
    pTypeLang = (struct lang_type **) xmalloc (sizeof (struct lang_type *) * cv);
    for (t = TYPE_MAIN_VARIANT (s); t && TYPE_LANG_SPECIFIC (t) && TYPE_OBJC_INFO (t);
	 t = TYPE_NEXT_VARIANT (t))
      pTypeLang[i++] = TYPE_LANG_SPECIFIC(t);

  }
  /* APPLE LOCAL end radar 5676962, 4310884, 4945770 */

  s = finish_struct (s, fields, NULL_TREE);

  /* APPLE LOCAL begin radar 5676962, 4310884, 4945770 */
  if (cv > 1) {
    int i=0;
    for (t = TYPE_MAIN_VARIANT (s); t && TYPE_LANG_SPECIFIC (t) && TYPE_OBJC_INFO (t);
         t = TYPE_NEXT_VARIANT (t))
      TYPE_LANG_SPECIFIC(t) = pTypeLang[i++];
    free (pTypeLang);
  }
  /* APPLE LOCAL end radar 5676962, 4310884, 4945770 */

  /* Use TYPE_BINFO structures to point at the super class, if any.  */
  objc_xref_basetypes (s, super);

  /* Mark this struct as a class template.  */
  CLASS_STATIC_TEMPLATE (class) = s;

  return s;
}

/* Build a type differing from TYPE only in that TYPE_VOLATILE is set.
   Unlike tree.c:build_qualified_type(), preserve TYPE_LANG_SPECIFIC in the
   process.  */
/* LLVM LOCAL begin rdar 6551276 */
#ifndef ENABLE_LLVM
static
#endif
tree
/* LLVM LOCAL end rdar 6551276 */
objc_build_volatilized_type (tree type)
{
  tree t;

  /* Check if we have not constructed the desired variant already.  */
  for (t = TYPE_MAIN_VARIANT (type); t; t = TYPE_NEXT_VARIANT (t))
    {
      /* The type qualifiers must (obviously) match up.  */
      if (!TYPE_VOLATILE (t)
	  || (TYPE_READONLY (t) != TYPE_READONLY (type))
	  || (TYPE_RESTRICT (t) != TYPE_RESTRICT (type)))
	continue;

      /* For pointer types, the pointees (and hence their TYPE_LANG_SPECIFIC
	 info, if any) must match up.  */
      if (POINTER_TYPE_P (t)
	  && (TREE_TYPE (t) != TREE_TYPE (type)))
	continue;

      /* LLVM LOCAL begin 7176678 */
      /* If this is a POINTER_TYPE, the name field should match too;
	 if one is set and another is cleared, encode_type will treat
	 them differently.  */
      if (TREE_CODE (type) == POINTER_TYPE
	  && TYPE_NAME (type) != TYPE_NAME (t))
	continue;
      /* LLVM LOCAL end 7176678 */

      /* APPLE LOCAL begin radar 4204796 */
      /* Only match up the types which were previously volatilized in similar fashion and not
	 because they were declared as such. */
      if (!lookup_attribute ("objc_volatilized", TYPE_ATTRIBUTES (t)))
	continue;
      /* APPLE LOCAL end radar 4204796 */
      
      /* Everything matches up!  */
      return t;
    }

  /* Ok, we could not re-use any of the pre-existing variants.  Create
     a new one.  */
  t = build_variant_type_copy (type);
  TYPE_VOLATILE (t) = 1;
  /* APPLE LOCAL begin radar 4204796 */
  TYPE_ATTRIBUTES (t) = merge_attributes (TYPE_ATTRIBUTES (type),
                      			  tree_cons (get_identifier ("objc_volatilized"),
                                 	  NULL_TREE,
                                 	  NULL_TREE));
  if (TREE_CODE (t) == ARRAY_TYPE)
    TREE_TYPE (t) = objc_build_volatilized_type (TREE_TYPE (t));
  /* APPLE LOCAL end radar 4204796 */

  return t;
}

/* Mark DECL as being 'volatile' for purposes of Darwin
   _setjmp()/_longjmp() exception handling.  Called from
   objc_mark_locals_volatile().  */
void
objc_volatilize_decl (tree decl)
{
  /* Do not mess with variables that are 'static' or (already)
     'volatile'.  */
  if (!TREE_THIS_VOLATILE (decl) && !TREE_STATIC (decl)
      && (TREE_CODE (decl) == VAR_DECL
	  || TREE_CODE (decl) == PARM_DECL))
    {
      tree t = TREE_TYPE (decl);
      /* APPLE LOCAL begin radar 4204796 */
      /* code removed */
      /* APPLE LOCAL end radar 4204796 */

      t = objc_build_volatilized_type (t);
      /* APPLE LOCAL begin radar 4204796 */
      /* code removed */
      /* APPLE LOCAL end radar 4204796 */

      TREE_TYPE (decl) = t;
      TREE_THIS_VOLATILE (decl) = 1;
      TREE_SIDE_EFFECTS (decl) = 1;
      DECL_REGISTER (decl) = 0;
#ifndef OBJCPLUS
      C_DECL_REGISTER (decl) = 0;
#endif
    }
}

/* APPLE LOCAL begin radar 4697411 */
/* This routine sets the "objc_volatilized" attribute on COMPONENT_REF
   node if 1) it is not already 'volatile' and 2) datum node
   has its "objc_volatilized" attribute set.
*/
void
objc_volatilize_component_ref (tree cref, tree member_type)
{
  tree object, type;

  if (TYPE_VOLATILE (member_type))
    return;
  object = TREE_OPERAND (cref, 0);
  if (TREE_THIS_VOLATILE (object))
    {
      type = TREE_TYPE (object);
      if (lookup_attribute ("objc_volatilized", TYPE_ATTRIBUTES (type)))
	{
	  member_type = objc_build_volatilized_type (member_type);
	  TREE_TYPE (cref) = member_type;
	  TREE_THIS_VOLATILE (cref) = 1;
	  TREE_SIDE_EFFECTS (cref) = 1;
	}
    }
  return;
}
/* APPLE LOCAL end radar 4697411 */

/* Check if protocol PROTO is adopted (directly or indirectly) by class CLS
   (including its categoreis and superclasses) or by object type TYP.
   Issue a warning if PROTO is not adopted anywhere and WARN is set.  */

static bool
objc_lookup_protocol (tree proto, tree cls, tree typ, bool warn)
{
  bool class_type = (cls != NULL_TREE);

  while (cls)
    {
      tree c;

      /* Check protocols adopted by the class and its categories.  */
      for (c = cls; c; c = CLASS_CATEGORY_LIST (c))
	{
	  if (lookup_protocol_in_reflist (CLASS_PROTOCOL_LIST (c), proto))
	    return true;
	}

      /* Repeat for superclasses.  */
      cls = lookup_interface (CLASS_SUPER_NAME (cls));
    }

  /* Check for any protocols attached directly to the object type.  */
  if (TYPE_HAS_OBJC_INFO (typ))
    {
      if (lookup_protocol_in_reflist (TYPE_OBJC_PROTOCOL_LIST (typ), proto))
	return true;
    }

  if (warn)
    {
      strcpy (errbuf, class_type ? "class \'" : "type \'");
      gen_type_name_0 (class_type ? typ : TYPE_POINTER_TO (typ));
      strcat (errbuf, "\' does not ");
      /* NB: Types 'id' and 'Class' cannot reasonably be described as
	 "implementing" a given protocol, since they do not have an
	 implementation.  */
      strcat (errbuf, class_type ? "implement" : "conform to");
      strcat (errbuf, " the \'");
      strcat (errbuf, IDENTIFIER_POINTER (PROTOCOL_NAME (proto)));
      strcat (errbuf, "\' protocol");
      /* APPLE LOCAL default to Wformat-security 5764921 */
      warning (0, "%s", errbuf);
    }

  return false;
}

/* Check if class RCLS and instance struct type RTYP conform to at least the
   same protocols that LCLS and LTYP conform to.  */

static bool
objc_compare_protocols (tree lcls, tree ltyp, tree rcls, tree rtyp, bool warn)
{
  tree p;
  bool have_lproto = false;

  while (lcls)
    {
      /* NB: We do _not_ look at categories defined for LCLS; these may or
	 may not get loaded in, and therefore it is unreasonable to require
	 that RCLS/RTYP must implement any of their protocols.  */
      for (p = CLASS_PROTOCOL_LIST (lcls); p; p = TREE_CHAIN (p))
	{
	  have_lproto = true;

	  if (!objc_lookup_protocol (TREE_VALUE (p), rcls, rtyp, warn))
	    return warn;
	}

      /* Repeat for superclasses.  */
      lcls = lookup_interface (CLASS_SUPER_NAME (lcls));
    }

  /* Check for any protocols attached directly to the object type.  */
  if (TYPE_HAS_OBJC_INFO (ltyp))
    {
      for (p = TYPE_OBJC_PROTOCOL_LIST (ltyp); p; p = TREE_CHAIN (p))
	{
	  have_lproto = true;

	  if (!objc_lookup_protocol (TREE_VALUE (p), rcls, rtyp, warn))
	    return warn;
	}
    }

  /* NB: If LTYP and LCLS have no protocols to search for, return 'true'
     vacuously, _unless_ RTYP is a protocol-qualified 'id'.  We can get
     away with simply checking for 'id' or 'Class' (!RCLS), since this
     routine will not get called in other cases.  */
  return have_lproto || (rcls != NULL_TREE);
}

/* APPLE LOCAL begin 4154928 */
/* Given two types TYPE1 and TYPE2, return their least common ancestor.
   Both TYPE1 and TYPE2 must be pointers, and already determined to be
   compatible by objc_compare_types() below.  */

tree
objc_common_type (tree type1, tree type2)
{
  tree inner1 = TREE_TYPE (type1), inner2 = TREE_TYPE (type2);

  while (POINTER_TYPE_P (inner1))
    {
      inner1 = TREE_TYPE (inner1);
      inner2 = TREE_TYPE (inner2);
    }

  /* If one type is derived from another, return the base type.  */
  if (DERIVED_FROM_P (inner1, inner2))
    return type1;
  else if (DERIVED_FROM_P (inner2, inner1))
    return type2;

  /* If both types are 'Class', return 'Class'.  */
  if (objc_is_class_id (inner1) && objc_is_class_id (inner2))
    return objc_class_type;

  /* Otherwise, return 'id'.  */
  return objc_object_type;
}
/* APPLE LOCAL end 4154928 */

/* APPLE LOCAL begin radar 5595325 */
/* This routine checks that TYPE has __attribute__((NSObject)) */
static bool
objc_nsobject_attribute_type (tree type)
{
  return POINTER_TYPE_P (type) && 
  lookup_attribute ("NSObject", TYPE_ATTRIBUTES (type));
}
/* APPLE LOCAL end radar 5595325 */

/* Determine if it is permissible to assign (if ARGNO is greater than -3)
   an instance of RTYP to an instance of LTYP or to compare the two
   (if ARGNO is equal to -3), per ObjC type system rules.  Before
   returning 'true', this routine may issue warnings related to, e.g.,
   protocol conformance.  When returning 'false', the routine must
   produce absolutely no warnings; the C or C++ front-end will do so
   instead, if needed.  If either LTYP or RTYP is not an Objective-C type,
   the routine must return 'false'.

   The ARGNO parameter is encoded as follows:
     >= 1	Parameter number (CALLEE contains function being called);
     0		Return value;
     -1		Assignment;
     -2		Initialization;
     APPLE LOCAL begin 4175534
     -3		Comparison (LTYP and RTYP may match in either direction);
     -4		Silent comparison (for C++ overload resolution).
     -5         Comparison of ivar and @synthesized property type
     // APPLE LOCAL begin radar 5218071
     -6         Comparison of two property types; RTYP is type of property
                in 'base' and LTYP is property type in derived class. They
                match if LTYP is more specialized than RTYP; this includes
                when RTYP is 'id' or LTYP is an object derived from an object 
                of RTYP.
    // APPLE LOCAL end radar 5218071
     APPLE LOCAL end 4175534  */

bool
/* APPLE LOCAL radar 6231433 */
objc_compare_types (tree ltyp, tree rtyp, int argno, tree callee, const char *message)
{
  tree lcls, rcls, lproto, rproto;
  bool pointers_compatible;

  /* APPLE LOCAL begin radar 5595325 */
  tree orig_ltyp, orig_rtyp;
  /* APPLE LOCAL end radar 5595325 */

  /* We must be dealing with pointer types */
  if (!POINTER_TYPE_P (ltyp) || !POINTER_TYPE_P (rtyp))
    return false;

  /* APPLE LOCAL begin radar 5595325 */
  orig_ltyp = ltyp;
  orig_rtyp = rtyp;
  /* APPLE LOCAL end radar 5595325 */
  do
    {
      ltyp = TREE_TYPE (ltyp);  /* Remove indirections.  */
      rtyp = TREE_TYPE (rtyp);
    }
  while (POINTER_TYPE_P (ltyp) && POINTER_TYPE_P (rtyp));

  /* APPLE LOCAL begin 4174166 */
  /* We must also handle function pointers, since ObjC is a bit more
     lenient than C or C++ on this.  */
  if (TREE_CODE (ltyp) == FUNCTION_TYPE && TREE_CODE (rtyp) == FUNCTION_TYPE)
    {
      /* Return types must be covariant.  */
      if (!comptypes (TREE_TYPE (ltyp), TREE_TYPE (rtyp))
	  && !objc_compare_types (TREE_TYPE (ltyp), TREE_TYPE (rtyp),
				  /* APPLE LOCAL radar 6231433 */
				  argno, callee, message))
      return false;

      /* Argument types must be contravariant.  */
      for (ltyp = TYPE_ARG_TYPES (ltyp), rtyp = TYPE_ARG_TYPES (rtyp);
	   ltyp && rtyp; ltyp = TREE_CHAIN (ltyp), rtyp = TREE_CHAIN (rtyp))
	{
	  if (!comptypes (TREE_VALUE (rtyp), TREE_VALUE (ltyp))
	      && !objc_compare_types (TREE_VALUE (rtyp), TREE_VALUE (ltyp),
				      /* APPLE LOCAL radar 6231433 */
				      argno, callee, message))
	    return false;
      }

      return (ltyp == rtyp);
    }

  /* APPLE LOCAL end 4174166 */
  /* Past this point, we are only interested in ObjC class instances,
     or 'id' or 'Class'.  */
  if (TREE_CODE (ltyp) != RECORD_TYPE || TREE_CODE (rtyp) != RECORD_TYPE)
    return false;

  if (!objc_is_object_id (ltyp) && !objc_is_class_id (ltyp)
  /* APPLE LOCAL begin radar 5595325 */
      && !TYPE_HAS_OBJC_INFO (ltyp)) {
    /* Allow NSObject pointers to match 'id' */
    return (objc_nsobject_attribute_type (orig_ltyp) && objc_is_object_id (rtyp)) 
           ? true : false;
  }
  /* APPLE LOCAL end radar 5595325 */

  if (!objc_is_object_id (rtyp) && !objc_is_class_id (rtyp)
  /* APPLE LOCAL begin radar 5595325 */
      && !TYPE_HAS_OBJC_INFO (rtyp)) {
    /* Allow NSObject pointers to match 'id' */
    return (objc_nsobject_attribute_type (orig_rtyp) && objc_is_object_id (ltyp)) 
           ? true : false;
  }
  /* APPLE LOCAL end radar 5595325 */

  /* APPLE LOCAL begin 4175534 */
  /* Past this point, we are committed to returning 'true' to the caller
     (unless performing a silent comparison; see below).  However, we can
     still warn about type and/or protocol mismatches.  */
  /* APPLE LOCAL end 4175534 */

  if (TYPE_HAS_OBJC_INFO (ltyp))
    {
      lcls = TYPE_OBJC_INTERFACE (ltyp);
      lproto = TYPE_OBJC_PROTOCOL_LIST (ltyp);
    }
  else
    lcls = lproto = NULL_TREE;

  if (TYPE_HAS_OBJC_INFO (rtyp))
    {
      rcls = TYPE_OBJC_INTERFACE (rtyp);
      rproto = TYPE_OBJC_PROTOCOL_LIST (rtyp);
    }
  else
    rcls = rproto = NULL_TREE;

  /* If we could not find an @interface declaration, we must have
     only seen a @class declaration; for purposes of type comparison,
     treat it as a stand-alone (root) class.  */

  /* APPLE LOCAL begin radar 6061276 */
  if (lcls && TREE_CODE (lcls) == IDENTIFIER_NODE)
    /* A previously declared @class may have its @interface declared at
       this point. Find it. */
    lcls = lookup_interface (lcls);

  if (rcls && TREE_CODE (rcls) == IDENTIFIER_NODE)
    /* A previously declared @class may have its @interface declared at
       this point. Find it. */
    rcls = lookup_interface (rcls);
  /* APPLE LOCAL end radar 6061276 */

  /* If either type is an unqualified 'id', we're done.  */
  /* APPLE LOCAL radar 5218071 */
  if ((argno != -6 && !lproto && objc_is_object_id (ltyp))
      || (!rproto && objc_is_object_id (rtyp)))
    return true;

  pointers_compatible = (TYPE_MAIN_VARIANT (ltyp) == TYPE_MAIN_VARIANT (rtyp));

  /* If the underlying types are the same, and at most one of them has
     a protocol list, we do not need to issue any diagnostics.  */
  if (pointers_compatible && (!lproto || !rproto))
    return true;

  /* If exactly one of the types is 'Class', issue a diagnostic; any
     exceptions of this rule have already been handled.  */
  if (objc_is_class_id (ltyp) ^ objc_is_class_id (rtyp))
    pointers_compatible = false;
  /* Otherwise, check for inheritance relations.  */
  else
    {
      if (!pointers_compatible)
	pointers_compatible
        /* APPLE LOCAL radar 5218071 */
	  = ((argno != -6 && objc_is_object_id (ltyp)) || objc_is_object_id (rtyp));
      /* APPLE LOCAL radar 5218071 */
      if (!pointers_compatible && argno != -6)
	pointers_compatible = DERIVED_FROM_P (ltyp, rtyp);

      /* APPLE LOCAL 4175534 */
      if (!pointers_compatible && argno <= -3 && argno != -5)
	pointers_compatible = DERIVED_FROM_P (rtyp, ltyp);
    }

  /* If the pointers match modulo protocols, check for protocol conformance
     mismatches.  */
  if (pointers_compatible)
    {
      pointers_compatible = objc_compare_protocols (lcls, ltyp, rcls, rtyp,
						    /* APPLE LOCAL 4175534 */
						    (argno > -3 || argno == -5));

      if (!pointers_compatible && argno == -3)
	pointers_compatible = objc_compare_protocols (rcls, rtyp, lcls, ltyp,
						      /* APPLE LOCAL 4175534 */
						      false);
    }

  if (!pointers_compatible)
    {
      /* APPLE LOCAL begin 4175534 */
      /* The two pointers are not exactly compatible.  Issue a warning, unless
	 we are performing a silent comparison, in which case return 'false'
	 instead.  */
      /* APPLE LOCAL end 4175534 */
      switch (argno)
	{
	/* APPLE LOCAL begin 4175534 */
        /* APPLE LOCAL radar 5218071 */
        case -6:
	case -5:
	case -4:
	  return false;

	/* APPLE LOCAL end 4175534 */
        /* APPLE LOCAL begin radar 5839123 */
	case -3:
	  /* APPLE LOCAL begin radar 6231433 */
	  warning (0, "%s of distinct Objective-C types %qT and %qT lacks a cast",
                   message, orig_rtyp, orig_ltyp);
	  /* APPLE LOCAL end radar 6231433 */
	  break;

	case -2:
	  warning (0, "incompatible Objective-C types initializing %qT, expected %qT",
                   orig_rtyp, orig_ltyp);
	  break;

	case -1:
	  warning (0, "incompatible Objective-C types assigning %qT, expected %qT", 
		   orig_rtyp, orig_ltyp);
	  break;

	case 0:
	  warning (0, "incompatible Objective-C types returning %qT, expected %qT",
                   orig_rtyp, orig_ltyp);
	  break;

	default:
	  warning (0, "incompatible Objective-C types %qT, expected %qT when"
                       " passing argument %d of %qE from distinct "
		       "Objective-C type"
                        , orig_rtyp, orig_ltyp, argno, callee);
          /* APPLE LOCAL end radar 5839123 */
	  break;
	}
    }

  return true;
}

/* APPLE LOCAL begin radar 4229905 - radar 6131433 */
/* This routine is similar to objc_compare_types except that function-pointers are
   excluded. This is because, caller assumes that common types are of (id, Object*)
   variety and calls objc_common_type to obtain a common type. There is no commonolty
   between two function-pointers in this regard. */

bool
objc_have_common_type (tree ltyp, tree rtyp, int argno, tree callee, const char *message)
{
  if (objc_compare_types (ltyp, rtyp, argno, callee, message))
    {
      /* exclude function-pointer types. */
      do
        {
          ltyp = TREE_TYPE (ltyp);  /* Remove indirections.  */
          rtyp = TREE_TYPE (rtyp);
        }
      while (POINTER_TYPE_P (ltyp) && POINTER_TYPE_P (rtyp));
      return !(TREE_CODE (ltyp) == FUNCTION_TYPE && TREE_CODE (rtyp) == FUNCTION_TYPE);
    }
  return false;
}
/* APPLE LOCAL end radar 4229905 - radar 6131433 */

/* Check if LTYP and RTYP have the same type qualifiers.  If either type
   lives in the volatilized hash table, ignore the 'volatile' bit when
   making the comparison.  */

bool
objc_type_quals_match (tree ltyp, tree rtyp)
{
  int lquals = TYPE_QUALS (ltyp), rquals = TYPE_QUALS (rtyp);
  
  /* APPLE LOCAL begin radar 4204796 */
  if (lookup_attribute ("objc_volatilized", TYPE_ATTRIBUTES (ltyp)))
    lquals &= ~TYPE_QUAL_VOLATILE;

  if (lookup_attribute ("objc_volatilized", TYPE_ATTRIBUTES (rtyp)))
    rquals &= ~TYPE_QUAL_VOLATILE;
  /* APPLE LOCAL end radar 4204796 */

  return (lquals == rquals);
}

#ifndef OBJCPLUS
/* Determine if CHILD is derived from PARENT.  The routine assumes that
   both parameters are RECORD_TYPEs, and is non-reflexive.  */

static bool
objc_derived_from_p (tree parent, tree child)
{
  parent = TYPE_MAIN_VARIANT (parent);

  for (child = TYPE_MAIN_VARIANT (child);
       TYPE_BINFO (child) && BINFO_N_BASE_BINFOS (TYPE_BINFO (child));)
    {
      child = TYPE_MAIN_VARIANT (BINFO_TYPE (BINFO_BASE_BINFO
					     (TYPE_BINFO (child),
					      0)));

      if (child == parent)
	return true;
    }

  return false;
}
#endif

/* APPLE LOCAL C* language */
tree
objc_build_component_ref (tree datum, tree component)
{
  /* If COMPONENT is NULL, the caller is referring to the anonymous
     base class field.  */
  if (!component)
    {
      tree base = TYPE_FIELDS (TREE_TYPE (datum));

      return build3 (COMPONENT_REF, TREE_TYPE (base), datum, base, NULL_TREE);
    }

  /* The 'build_component_ref' routine has been removed from the C++
     front-end, but 'finish_class_member_access_expr' seems to be
     a worthy substitute.  */
#ifdef OBJCPLUS
  return finish_class_member_access_expr (datum, component, false);
#else
  return build_component_ref (datum, component);
#endif
}

/* Recursively copy inheritance information rooted at BINFO.  To do this,
   we emulate the song and dance performed by cp/tree.c:copy_binfo().  */

static tree
objc_copy_binfo (tree binfo)
{
  tree btype = BINFO_TYPE (binfo);
  tree binfo2 = make_tree_binfo (BINFO_N_BASE_BINFOS (binfo));
  tree base_binfo;
  int ix;

  BINFO_TYPE (binfo2) = btype;
  BINFO_OFFSET (binfo2) = BINFO_OFFSET (binfo);
  BINFO_BASE_ACCESSES (binfo2) = BINFO_BASE_ACCESSES (binfo);

  /* Recursively copy base binfos of BINFO.  */
  for (ix = 0; BINFO_BASE_ITERATE (binfo, ix, base_binfo); ix++)
    {
      tree base_binfo2 = objc_copy_binfo (base_binfo);

      BINFO_INHERITANCE_CHAIN (base_binfo2) = binfo2;
      BINFO_BASE_APPEND (binfo2, base_binfo2);
    }

  return binfo2;
}

/* Record superclass information provided in BASETYPE for ObjC class REF.
   This is loosely based on cp/decl.c:xref_basetypes().  */

static void
objc_xref_basetypes (tree ref, tree basetype)
{
  tree binfo = make_tree_binfo (basetype ? 1 : 0);

  TYPE_BINFO (ref) = binfo;
  BINFO_OFFSET (binfo) = size_zero_node;
  BINFO_TYPE (binfo) = ref;

  if (basetype)
    {
      tree base_binfo = objc_copy_binfo (TYPE_BINFO (basetype));

      BINFO_INHERITANCE_CHAIN (base_binfo) = binfo;
      BINFO_BASE_ACCESSES (binfo) = VEC_alloc (tree, gc, 1);
      BINFO_BASE_APPEND (binfo, base_binfo);
      BINFO_BASE_ACCESS_APPEND (binfo, access_public_node);
    }
}

/* APPLE LOCAL begin radar 4204796 */
/* volatilized_hash and volatilized_eq removed */
/* APPLE LOCAL end radar 4204796 */
/* Called from finish_decl.  */

void
objc_check_decl (tree decl)
{
  tree type = TREE_TYPE (decl);

  /* APPLE LOCAL begin 6393374 */
  while (TREE_CODE (type) == ARRAY_TYPE)
    type = TREE_TYPE (type);
  /* APPLE LOCAL end 6393374 */
  if (TREE_CODE (type) != RECORD_TYPE)
    return;
  if (OBJC_TYPE_NAME (type) && (type = objc_is_class_name (OBJC_TYPE_NAME (type))))
    error ("statically allocated instance of Objective-C class %qs",
	   IDENTIFIER_POINTER (type));
}

/* APPLE LOCAL begin radar 4281748 */
void
objc_check_global_decl (tree decl)
{
  tree id = DECL_NAME (decl);
  if (objc_is_class_name (id) && global_bindings_p())
    error ("redeclaration of Objective-C class %qs", IDENTIFIER_POINTER (id));
}
/* APPLE LOCAL end radar 4281748 */

/* APPLE LOCAL begin radar 4330422 */
/* Return a non-volatalized version of TYPE. */

tree
objc_non_volatilized_type (tree type)
{
  if (lookup_attribute ("objc_volatilized", TYPE_ATTRIBUTES (type)))
    type = build_qualified_type (type, (TYPE_QUALS (type) & ~TYPE_QUAL_VOLATILE));
  return type;
}
/* APPLE LOCAL end radar 4330422 */

/* Construct a PROTOCOLS-qualified variant of INTERFACE, where INTERFACE may
   either name an Objective-C class, or refer to the special 'id' or 'Class'
   types.  If INTERFACE is not a valid ObjC type, just return it unchanged.  */

tree
objc_get_protocol_qualified_type (tree interface, tree protocols)
{
  /* If INTERFACE is not provided, default to 'id'.  */
  tree type = (interface ? objc_is_id (interface) : objc_object_type);
  bool is_ptr = (type != NULL_TREE);

  if (!is_ptr)
    {
      type = objc_is_class_name (interface);

      if (type)
	/* APPLE LOCAL begin 4216500 */
	{
	  /* If looking at a typedef, retrieve the precise type it
	     describes.  */
	  if (TREE_CODE (interface) == IDENTIFIER_NODE)
	    interface = identifier_global_value (interface);

	  type = ((interface && TREE_CODE (interface) == TYPE_DECL
		   && DECL_ORIGINAL_TYPE (interface))
		  ? DECL_ORIGINAL_TYPE (interface)
		  : xref_tag (RECORD_TYPE, type));
	}
	/* APPLE LOCAL end 4216500 */
      /* APPLE LOCAL begin radar 6267049 */
      else {
        /* This can only happen because of bogus code. Note that in this
	   path, protocols is not even considered. */
	error ("qualified type is not a valid object");
        /* Check further to see if protocol is valid. */
        if (protocols)
	  (void) lookup_and_install_protocols (protocols, false);
        return interface;
      }
      /* APPLE LOCAL end radar 6267049 */
    }

  if (protocols)
    {
      type = build_variant_type_copy (type);

      /* For pointers (i.e., 'id' or 'Class'), attach the protocol(s)
	 to the pointee.  */
      if (is_ptr)
	{
	  TREE_TYPE (type) = build_variant_type_copy (TREE_TYPE (type));
	  TYPE_POINTER_TO (TREE_TYPE (type)) = type;
	  type = TREE_TYPE (type);
	}

      /* Look up protocols and install in lang specific list.  */
      DUP_TYPE_OBJC_INFO (type, TYPE_MAIN_VARIANT (type));
      /* APPLE LOCAL radar 4398221 */
      TYPE_OBJC_PROTOCOL_LIST (type) = lookup_and_install_protocols (protocols, false);

      /* For RECORD_TYPEs, point to the @interface; for 'id' and 'Class',
	 return the pointer to the new pointee variant.  */
      if (is_ptr)
	type = TYPE_POINTER_TO (type);
      else
	TYPE_OBJC_INTERFACE (type)
	  = TYPE_OBJC_INTERFACE (TYPE_MAIN_VARIANT (type));
    }

  return type;
}

/* Check for circular dependencies in protocols.  The arguments are
   PROTO, the protocol to check, and LIST, a list of protocol it
   conforms to.  */

static void
check_protocol_recursively (tree proto, tree list)
{
  tree p;

  for (p = list; p; p = TREE_CHAIN (p))
    {
      tree pp = TREE_VALUE (p);

      if (TREE_CODE (pp) == IDENTIFIER_NODE)
	pp = lookup_protocol (pp);

      if (pp == proto)
	fatal_error ("protocol %qs has circular dependency",
		     IDENTIFIER_POINTER (PROTOCOL_NAME (pp)));
      if (pp)
	check_protocol_recursively (proto, PROTOCOL_LIST (pp));
    }
}

/* APPLE LOCAL begin radar 4947311 - protocol attributes */
static void
objc_handle_protocol_attrs (tree proto)
{
  tree chain;

  for (chain = PROTOCOL_ATTRIBUTES (proto); chain; chain = TREE_CHAIN (chain))
    {
      if (is_attribute_p ("deprecated", TREE_PURPOSE (chain)))
	warning (0, "protocol %qs is deprecated",
		 IDENTIFIER_POINTER (CLASS_NAME (proto)));
      if (is_attribute_p ("unavailable", TREE_PURPOSE (chain)))
	warning (0, "protocol %qs is unavailable",
	         IDENTIFIER_POINTER (CLASS_NAME (proto)));
    }
  return;
}

/* APPLE LOCAL end radar 4947311 - protocol attributes */

/* Look up PROTOCOLS, and return a list of those that are found.
   If none are found, return NULL.  */

static tree
/* APPLE LOCAL radar 4398221 */
lookup_and_install_protocols (tree protocols, bool defNeeded)
{
  tree proto;
  tree return_value = NULL_TREE;

  for (proto = protocols; proto; proto = TREE_CHAIN (proto))
    {
      tree ident = TREE_VALUE (proto);
      tree p = lookup_protocol (ident);

      if (p)
	/* APPLE LOCAL begin radar 4398221 */
	{
	  if (defNeeded && !PROTOCOL_DEFINED (p))
	    warning (0, "no definition of protocol %qs is found",
		     IDENTIFIER_POINTER (ident));
	  /* APPLE LOCAL radar 4947311 - protocol attributes */
	  objc_handle_protocol_attrs (p);
	  return_value = chainon (return_value,
				  build_tree_list (NULL_TREE, p));
	}
      /* APPLE LOCAL end radar 4398221 */
      else if (ident != error_mark_node)
	error ("cannot find protocol declaration for %qs",
	       IDENTIFIER_POINTER (ident));
    }

  return return_value;
}

/* Create a declaration for field NAME of a given TYPE.  */

static tree
create_field_decl (tree type, const char *name)
{
  return build_decl (FIELD_DECL, get_identifier (name), type);
}

/* Create a global, static declaration for variable NAME of a given TYPE.  The
   finish_var_decl() routine will need to be called on it afterwards.  */

/* APPLE LOCAL begin radar 4431864 */
static tree
start_var_decl (tree type, const char *name)
{
  /* LLVM LOCAL begin */
  tree var = NULL_TREE;
#ifdef ENABLE_LLVM
  /* Prefer to use 'L' as a prefix so symbols can be stripped at assembly
     time. GCC codegen handles this later while emitting symbols, but fix
     it here for llvm.  */
  /* Special ObjC symbols in __data should use 'l' so the assembler
     will not strip the symbol; the linker needs to see these, but
     strip them after use. */
  char *new_name;
  if (name && strncmp (name, "_OBJC_", 6) == 0) {
    new_name = alloca (strlen (name) + 2);
#if TARGET_MACHO
    {
      const char* section = darwin_objc_llvm_special_name_section(name+6);
      new_name[0] = (section==0 || strcmp(section, "__DATA,__data")==0) 
                    ? 'l' : 'L';
    }
#else
    new_name[0] = 'L';
#endif
    strcpy (new_name + 1, name);
    var = build_decl (VAR_DECL, get_identifier (new_name), type);
    set_user_assembler_name (var, IDENTIFIER_POINTER (DECL_NAME (var)));
    /* Let optimizer know that this var is not removable.  */
    DECL_PRESERVE_P (var) = 1;
    TREE_USED(var) = 1;
  } 
  else
    /* Fall through. Build using 'name' */
#endif
  var = build_decl (VAR_DECL, get_identifier (name), type);
  /* LLVM LOCAL end */
  objc_set_global_decl_fields (var);
  return var;
}

/* Utility routine to set global flags for a global, static declaration. */

static void 
objc_set_global_decl_fields (tree var)
{
  TREE_STATIC (var) = 1;
  DECL_INITIAL (var) = error_mark_node;  /* A real initializer is coming... */
  DECL_IGNORED_P (var) = 1;
  DECL_ARTIFICIAL (var) = 1;
  DECL_CONTEXT (var) = NULL_TREE;
#ifdef OBJCPLUS
  DECL_THIS_STATIC (var) = 1; /* squash redeclaration errors */
#endif
  /* APPLE LOCAL begin 6255801 */
  /* Keep word alignment for metadata records.  */
  DECL_USER_ALIGN (var) = 1;
  /* APPLE LOCAL end 6255801 */
}
/* APPLE LOCAL end radar 4431864 */

/* APPLE LOCAL begin ObjC new abi */
/* Create a globally visible definition for variable NAME of a given TYPE. The
   finish_var_decl() routine will need to be called on it afterwards.  */

static tree
create_global_decl (tree type, const char *name)
{
  /* APPLE LOCAL begin radar 4431864 */
  tree id = get_identifier (name);
  /* APPLE LOCAL radar 4441049 */
  tree var = hash_name_lookup (ivar_offset_hash_list, id);
  if (var)
    {
      DECL_EXTERNAL (var) = 0;
      /* APPLE LOCAL radar 4431864 */
      objc_set_global_decl_fields (var);
    }
  else
  /* APPLE LOCAL begin radar 4441049 */
    {
      var = start_var_decl (type, name);
      hash_name_enter (ivar_offset_hash_list, var);
    }
  /* APPLE LOCAL end radar 4441049 */
  /* APPLE LOCAL end radar 4431864 */
  TREE_PUBLIC (var) = 1;
  return var;
}

/* Create a symbol with __attribute__ ((visibility ("hidden"))) 
   attribute (private extern) */

static tree
create_hidden_decl (tree type, const char *name)
{
    tree decl = create_global_decl (type, name);
    DECL_VISIBILITY (decl) = VISIBILITY_HIDDEN;
    DECL_VISIBILITY_SPECIFIED (decl) = 1;
    return decl;
}

/* Create an extern declaration for variable NAME of a given TYPE. The
   finish_var_decl() routine will need to be called on it afterwards.  */

static tree
create_extern_decl (tree type, const char *name)
{
  tree id = get_identifier (name);
  /* APPLE LOCAL radar 4441049 */
  tree var = hash_name_lookup (ivar_offset_hash_list, id);
  if (var)
    return var;
  /* Name not already declared. */
  var = build_decl (VAR_DECL, id, type); 
  DECL_EXTERNAL (var) = 1;
  TREE_PUBLIC (var) = 1;
  /* All external declarations are at file_scope to prevent 
     duplication of declarations; a common occurance for external
     variables holding ivar offsets in the new abi. */
  pushdecl_top_level (var);
  rest_of_decl_compilation (var, 0, 0);
  /* APPLE LOCAL radar 4441049 */
  hash_name_enter (ivar_offset_hash_list, var);
  return var;
}

/* APPLE LOCAL begin radar 4947014 - objc atomic property */
/* This routine declares prototypes:
   id objc_getProperty (id, SEL, ptrdiff_t, bool)
   void objc_setProperty (id, SEL, ptrdiff_t, id, bool, bool)
   void objc_copyStruct (void *, const void *, size_t, bool, bool)
*/
static void
declare_atomic_property_api (void)
{
  tree func_type;

  umsg_GetAtomicProperty = lookup_name (get_identifier ("objc_getProperty"));
  umsg_SetAtomicProperty = lookup_name (get_identifier ("objc_setProperty"));
  umsg_CopyAtomicStruct = lookup_name (get_identifier ("objc_copyStruct"));
  if (umsg_GetAtomicProperty && umsg_SetAtomicProperty && umsg_CopyAtomicStruct)
    return;

  if (!umsg_GetAtomicProperty)
    {
      /* id objc_getProperty (id, SEL, ptrdiff_t, bool) */
      func_type
        = build_function_type (objc_object_type,
		           tree_cons (NULL_TREE, objc_object_type,
	              	     tree_cons (NULL_TREE, objc_selector_type,
			       tree_cons (NULL_TREE, long_integer_type_node, 
			    	 tree_cons (NULL_TREE, boolean_type_node, NULL_TREE)))));
      umsg_GetAtomicProperty = builtin_function (
			        "objc_getProperty",
			        func_type, 0, NOT_BUILT_IN, 0, NULL_TREE);
      TREE_NOTHROW (umsg_GetAtomicProperty) = 0;
    }

  if (!umsg_SetAtomicProperty)
    {
      /* void objc_setProperty (id, SEL, ptrdiff_t, id, bool, bool) */
      func_type
        = build_function_type (void_type_node,
		           tree_cons (NULL_TREE, objc_object_type,
			     tree_cons (NULL_TREE, objc_selector_type,
			       tree_cons (NULL_TREE, long_integer_type_node, 
			         tree_cons (NULL_TREE, objc_object_type, 
				   tree_cons (NULL_TREE, boolean_type_node, 
				     tree_cons (NULL_TREE, boolean_type_node, NULL_TREE)))))));
      umsg_SetAtomicProperty = builtin_function (
			        "objc_setProperty",
			        func_type, 0, NOT_BUILT_IN, 0, NULL_TREE);
      TREE_NOTHROW (umsg_SetAtomicProperty) = 0;
    }

  if (!umsg_CopyAtomicStruct)
    {
      /* void objc_copyStruct (void *, const void *, size_t, bool, bool) */
      func_type
        = build_function_type (void_type_node,
		           tree_cons (NULL_TREE, ptr_type_node,
			     tree_cons (NULL_TREE, ptr_type_node,
			       tree_cons (NULL_TREE, long_unsigned_type_node, 
				   tree_cons (NULL_TREE, boolean_type_node, 
				     tree_cons (NULL_TREE, boolean_type_node, NULL_TREE))))));
      umsg_CopyAtomicStruct = builtin_function (
			        "objc_copyStruct",
			        func_type, 0, NOT_BUILT_IN, 0, NULL_TREE);
      TREE_NOTHROW (umsg_CopyAtomicStruct) = 0;
    }
  return; 
}

/* This routine returns true if struct/union contains an object pointer or a __strong pointer. */
static bool
aggregate_contains_objc_pointer (tree type)
{
  tree field;
  field = TYPE_FIELDS (type);

  for (; field; field = TREE_CHAIN (field))
    {
#ifdef OBJCPLUS
      if (TREE_CODE (field) != FIELD_DECL || TREE_STATIC (field))
        continue;
#endif
      type = TREE_TYPE (field);
      if (TREE_CODE (type) == RECORD_TYPE || TREE_CODE (type) == UNION_TYPE)
	if (aggregate_contains_objc_pointer (type))
	  return true;
      if (objc_is_gcable_type (type) == 1)
	return true; 
    }
  return false;
}

/* APPLE LOCAL end radar 4947014 - objc atomic property */

/* APPLE LOCAL end ObjC new abi */

/* Finish off the variable declaration created by start_var_decl().  */

static void
finish_var_decl (tree var, tree initializer)
{
  finish_decl (var, initializer, NULL_TREE);
  /* Ensure that the variable actually gets output.  */
  mark_decl_referenced (var);
  /* Mark the decl to avoid "defined but not used" warning.  */
  TREE_USED (var) = 1;
}

/* Find the decl for the constant string class reference.  This is only
   used for the NeXT runtime.  */

static tree
setup_string_decl (void)
{
  char *name;
  size_t length;
  /* APPLE LOCAL begin radar 4719165 */
  const char *prefix = (flag_objc_abi == 2) 
			? STRING_V2_OBJECT_GLOBAL_FORMAT 
			: STRING_OBJECT_GLOBAL_FORMAT;

  /* %s in format will provide room for terminating null */
  length = strlen (prefix)
	   + strlen (constant_string_class_name);
  name = xmalloc (length);
  sprintf (name, prefix,
	   constant_string_class_name);
  constant_string_global_id = get_identifier (name);
  string_class_decl = lookup_name (constant_string_global_id);
  /* In OBJC2 abi constant string class reference refers to 
     class name for NSConstantString class. This declaration may not be
     available yet (in fact it is not in most cases). So, declare an extern 
     OBJC_CLASS_$_NSConstantString in its place. */
  if (flag_objc_abi == 2 && !string_class_decl)
    string_class_decl = create_extern_decl (objc_v2_class_template, name);
  /* APPLE LOCAL end radar 4719165 */
  return string_class_decl;
}

/* Purpose: "play" parser, creating/installing representations
   of the declarations that are required by Objective-C.

   Model:

	type_spec--------->sc_spec
	(tree_list)        (tree_list)
	    |                  |
	    |                  |
	identifier_node    identifier_node  */

static void
synth_module_prologue (void)
{
  tree type;
  enum debug_info_type save_write_symbols = write_symbols;
  const struct gcc_debug_hooks *const save_hooks = debug_hooks;
  /* APPLE LOCAL 6348516 */
  int save_warn_padded;

  /* Suppress outputting debug symbols, because
     dbxout_init hasn'r been called yet.  */
  write_symbols = NO_DEBUG;
  debug_hooks = &do_nothing_debug_hooks;

#ifdef OBJCPLUS
  push_lang_context (lang_name_c); /* extern "C" */
#endif

  /* The following are also defined in <objc/objc.h> and friends.  */

  objc_object_id = get_identifier (TAG_OBJECT);
  objc_class_id = get_identifier (TAG_CLASS);

  objc_object_reference = xref_tag (RECORD_TYPE, objc_object_id);
  objc_class_reference = xref_tag (RECORD_TYPE, objc_class_id);

  objc_object_type = build_pointer_type (objc_object_reference);
  objc_class_type = build_pointer_type (objc_class_reference);

  objc_object_name = get_identifier (OBJECT_TYPEDEF_NAME);
  objc_class_name = get_identifier (CLASS_TYPEDEF_NAME);

  /* Declare the 'id' and 'Class' typedefs.  */

  type = lang_hooks.decls.pushdecl (build_decl (TYPE_DECL,
						objc_object_name,
						objc_object_type));
  DECL_IN_SYSTEM_HEADER (type) = 1;
  type = lang_hooks.decls.pushdecl (build_decl (TYPE_DECL,
						objc_class_name,
						objc_class_type));
  DECL_IN_SYSTEM_HEADER (type) = 1;

  /* Forward-declare '@interface Protocol'.  */

  type = get_identifier (PROTOCOL_OBJECT_CLASS_NAME);
  objc_declare_class (tree_cons (NULL_TREE, type, NULL_TREE));
  objc_protocol_type = build_pointer_type (xref_tag (RECORD_TYPE,
                                type));

  /* Declare type of selector-objects that represent an operation name.  */

  if (flag_next_runtime)
    /* `struct objc_selector *' */
    objc_selector_type
      = build_pointer_type (xref_tag (RECORD_TYPE,
				      get_identifier (TAG_SELECTOR)));
  else
    /* `const struct objc_selector *' */
    objc_selector_type
      = build_pointer_type
	(build_qualified_type (xref_tag (RECORD_TYPE,
					 get_identifier (TAG_SELECTOR)),
			       TYPE_QUAL_CONST));

  /* Declare receiver type used for dispatching messages to 'super'.  */

  /* `struct objc_super *' */
  objc_super_type = build_pointer_type (xref_tag (RECORD_TYPE,
						  get_identifier (TAG_SUPER)));

  /* Declare pointers to method and ivar lists.  */
  objc_method_list_ptr = build_pointer_type
			 (xref_tag (RECORD_TYPE,
				    get_identifier (UTAG_METHOD_LIST)));
  objc_method_proto_list_ptr
    = build_pointer_type (xref_tag (RECORD_TYPE,
				    get_identifier (UTAG_METHOD_PROTOTYPE_LIST)));
  objc_ivar_list_ptr = build_pointer_type
		       (xref_tag (RECORD_TYPE,
				  get_identifier (UTAG_IVAR_LIST)));

  /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
  objc_prop_list_ptr 
   = build_pointer_type (xref_tag (RECORD_TYPE,
				     get_identifier ("_prop_list_t")));
  /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */

  /* APPLE LOCAL begin ObjC new abi */
  if (flag_objc_abi == 2)
    {
      objc_v2_ivar_list_ptr = build_pointer_type (
				    xref_tag (RECORD_TYPE, 
					      get_identifier ("_ivar_list_t")));
    }

  /* typedef id (*IMP)(id, SEL, ...); */
  objc_imp_type
    = build_pointer_type
	  (build_function_type (objc_object_type,      
				tree_cons (NULL_TREE, objc_object_type,      
					   tree_cons (NULL_TREE, objc_selector_type,      
						      NULL_TREE))));      
  /* APPLE LOCAL end ObjC new abi */

  /* TREE_NOTHROW is cleared for the message-sending functions,
     because the function that gets called can throw in Obj-C++, or
     could itself call something that can throw even in Obj-C.  */

  if (flag_next_runtime)
    {
      /* NB: In order to call one of the ..._stret (struct-returning)
      functions, the function *MUST* first be cast to a signature that
      corresponds to the actual ObjC method being invoked.  This is
      what is done by the build_objc_method_call() routine below.  */

      /* id objc_msgSend (id, SEL, ...); */
      /* id objc_msgSendNonNil (id, SEL, ...); */
      /* id objc_msgSend_stret (id, SEL, ...); */
      /* id objc_msgSendNonNil_stret (id, SEL, ...); */
      type
	= build_function_type (objc_object_type,
			       tree_cons (NULL_TREE, objc_object_type,
					  tree_cons (NULL_TREE, objc_selector_type,
						     NULL_TREE)));
      umsg_decl = builtin_function (TAG_MSGSEND,
				    type, 0, NOT_BUILT_IN,
				    NULL, NULL_TREE);
      umsg_nonnil_decl = builtin_function (TAG_MSGSEND_NONNIL,
					   type, 0, NOT_BUILT_IN,
					   NULL, NULL_TREE);
      umsg_stret_decl = builtin_function (TAG_MSGSEND_STRET,
					  type, 0, NOT_BUILT_IN,
					  NULL, NULL_TREE);
      umsg_nonnil_stret_decl = builtin_function (TAG_MSGSEND_NONNIL_STRET,
						 type, 0, NOT_BUILT_IN,
						 NULL, NULL_TREE);

      /* APPLE LOCAL begin ObjC new abi */
      /* APPLE LOCAL ARM hybrid objc-2.0 */
      if (flag_objc_abi == 2 && !flag_objc_legacy_dispatch)
	{
          /* APPLE LOCAL radar 4699834 */
 	  /* Removed _rtp suffix from objc_msgSend_fixup_rtp and variants */
    	  build_message_ref_template ();
	  /* id objc_msgSend_fixup (id, struct message_ref_t*, ...); */
	  type 
	    = build_function_type (objc_object_type,
				   tree_cons (NULL_TREE, objc_object_type,
					      tree_cons (NULL_TREE, objc_v2_selector_type,
						          NULL_TREE)));
	  umsg_fixup_decl = builtin_function ("objc_msgSend_fixup",
					      type, 0, NOT_BUILT_IN,
					      NULL, NULL_TREE);
	  TREE_NOTHROW (umsg_fixup_decl) = 0;
	  /* APPLE LOCAL begin radar 4557598 */
	  umsg_fpret_fixup_decl = builtin_function ("objc_msgSend_fpret_fixup",
					      type, 0, NOT_BUILT_IN,
					      NULL, NULL_TREE);
	  TREE_NOTHROW (umsg_fpret_fixup_decl) = 0;
	  /* APPLE LOCAL end radar 4557598 */

	  /* id objc_msgSend_stret_fixup (id, struct message_ref_t*, ...); */
	  umsg_stret_fixup_decl = builtin_function ("objc_msgSend_stret_fixup",
					      	    type, 0, NOT_BUILT_IN,
					      	    NULL, NULL_TREE);
	  TREE_NOTHROW (umsg_stret_fixup_decl) = 0;

	  /* id objc_msgSendId_fixup (id, struct message_ref_t*, ...); */
	  umsg_id_fixup_decl = builtin_function ("objc_msgSendId_fixup",
                                              type, 0, NOT_BUILT_IN,
                                              NULL, NULL_TREE);
          TREE_NOTHROW (umsg_id_fixup_decl) = 0;

	  /* id objc_msgSendId_stret_fixup (id, struct message_ref_t*, ...); */
	  umsg_id_stret_fixup_decl = builtin_function ("objc_msgSendId_stret_fixup",
                                              type, 0, NOT_BUILT_IN,
                                              NULL, NULL_TREE);
          TREE_NOTHROW (umsg_id_stret_fixup_decl) = 0;

          /* id objc_msgSendSuper2_fixup (struct objc_super *, struct message_ref_t*, ...); */
          type
	    = build_function_type (objc_object_type,
			           tree_cons (NULL_TREE, objc_super_type,
				   tree_cons (NULL_TREE, objc_v2_super_selector_type,
					      NULL_TREE)));
          umsg_id_super2_fixup_decl = builtin_function ("objc_msgSendSuper2_fixup",
					      		type, 0, NOT_BUILT_IN,
					      		NULL, NULL_TREE);
	  TREE_NOTHROW (umsg_id_super2_fixup_decl) = 0;
          /* id objc_msgSendSuper2_stret_fixup (struct objc_super *, struct message_ref_t*, ...); */
          umsg_id_super2_stret_fixup_decl = builtin_function ("objc_msgSendSuper2_stret_fixup",
						    	      type, 0, NOT_BUILT_IN, 0,
						              NULL_TREE);
	  TREE_NOTHROW (umsg_id_super2_stret_fixup_decl) = 0;

	  /* APPLE LOCAL radar 4533974 - ObjC new protocol */
	  /* code removed */
	}
      /* APPLE LOCAL end ObjC new abi */

      /* APPLE LOCAL begin C* language */
      build_objc_fast_enum_state_type ();

      /* void objc_enumerationMutation (id) */
      type = build_function_type (void_type_node, 
				  tree_cons (NULL_TREE, objc_object_type, NULL_TREE));
      objc_enum_mutation_decl = builtin_function (
				  "objc_enumerationMutation",
				  type, 0, NOT_BUILT_IN, 0, NULL_TREE);
      TREE_NOTHROW (objc_enum_mutation_decl) = 0;

      /* APPLE LOCAL end C* language */

      /* APPLE LOCAL begin radar 4280641 */
      /* Not needed for ppc */
      umsg_fpret_decl = builtin_function (TAG_MSGSEND_FPRET,
				          type, 0, NOT_BUILT_IN,
					  NULL, NULL_TREE);
      TREE_NOTHROW (umsg_fpret_decl) = 0;
      /* APPLE LOCAL end radar 4280641 */

      /* These can throw, because the function that gets called can throw
	 in Obj-C++, or could itself call something that can throw even
	 in Obj-C.  */
      TREE_NOTHROW (umsg_decl) = 0;
      TREE_NOTHROW (umsg_nonnil_decl) = 0;
      TREE_NOTHROW (umsg_stret_decl) = 0;
      TREE_NOTHROW (umsg_nonnil_stret_decl) = 0;

      /* id objc_msgSend_Fast (id, SEL, ...)
	   __attribute__ ((hard_coded_address (OFFS_MSGSEND_FAST))); */
      /* APPLE LOCAL begin radar 4590221 */
      if (OFFS_MSGSEND_FAST)
	{
          /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
          tree umsg_fast_decl;
          umsg_fast_decl = build_int_cst (NULL_TREE, OFFS_MSGSEND_FAST);
          umsg_fast_decl = build_c_cast (build_pointer_type (TREE_TYPE (umsg_decl)),
                                    umsg_fast_decl);
          umsg_fast_decl = build_indirect_ref (umsg_fast_decl, "*"); 
          umsg_decl = umsg_fast_decl;
#else
          umsg_decl = builtin_function (TAG_MSGSEND_FAST,
					type, 0, NOT_BUILT_IN,
					NULL, NULL_TREE);
          TREE_NOTHROW (umsg_decl) = 0;
          DECL_ATTRIBUTES (umsg_decl) 
	    = tree_cons (get_identifier ("hard_coded_address"), 
		         build_int_cst (NULL_TREE, OFFS_MSGSEND_FAST),
		         NULL_TREE);
#endif
          /* LLVM LOCAL end */
	}
      /* APPLE LOCAL end radar 4590221 */

      /* id objc_msgSendSuper (struct objc_super *, SEL, ...); */
      /* id objc_msgSendSuper_stret (struct objc_super *, SEL, ...); */
      type
	= build_function_type (objc_object_type,
			       tree_cons (NULL_TREE, objc_super_type,
					  tree_cons (NULL_TREE, objc_selector_type,
						     NULL_TREE)));
      umsg_super_decl = builtin_function (TAG_MSGSENDSUPER,
					  type, 0, NOT_BUILT_IN,
					  NULL, NULL_TREE);
      umsg_super_stret_decl = builtin_function (TAG_MSGSENDSUPER_STRET,
						type, 0, NOT_BUILT_IN, 0,
						NULL_TREE);
      TREE_NOTHROW (umsg_super_decl) = 0;
      TREE_NOTHROW (umsg_super_stret_decl) = 0;
    }
  else
    {
      /* GNU runtime messenger entry points.  */

      /* APPLE LOCAL begin ObjC new abi */
      /* code removed */
      /* APPLE LOCAL end ObjC new abi */
      /* IMP objc_msg_lookup (id, SEL); */
      type
      /* APPLE LOCAL ObjC new abi */
        = build_function_type (objc_imp_type,
			       tree_cons (NULL_TREE, objc_object_type,
					  tree_cons (NULL_TREE, objc_selector_type,
						     OBJC_VOID_AT_END)));
      umsg_decl = builtin_function (TAG_MSGSEND,
				    type, 0, NOT_BUILT_IN,
				    NULL, NULL_TREE);
      TREE_NOTHROW (umsg_decl) = 0;

      /* IMP objc_msg_lookup_super (struct objc_super *, SEL); */
      type
      /* APPLE LOCAL ObjC new abi */
        = build_function_type (objc_imp_type,
			       tree_cons (NULL_TREE, objc_super_type,
					  tree_cons (NULL_TREE, objc_selector_type,
						     OBJC_VOID_AT_END)));
      umsg_super_decl = builtin_function (TAG_MSGSENDSUPER,
					  type, 0, NOT_BUILT_IN,
					  NULL, NULL_TREE);
      TREE_NOTHROW (umsg_super_decl) = 0;

      /* The following GNU runtime entry point is called to initialize
	 each module:

	 __objc_exec_class (void *); */
      type
	= build_function_type (void_type_node,
			       tree_cons (NULL_TREE, ptr_type_node,
					  OBJC_VOID_AT_END));
      execclass_decl = builtin_function (TAG_EXECCLASS,
					 type, 0, NOT_BUILT_IN,
					 NULL, NULL_TREE);
    }

  /* id objc_getClass (const char *); */

  type = build_function_type (objc_object_type,
				   tree_cons (NULL_TREE,
					      const_string_type_node,
					      OBJC_VOID_AT_END));

  objc_get_class_decl
    = builtin_function (TAG_GETCLASS, type, 0, NOT_BUILT_IN,
			NULL, NULL_TREE);

  /* id objc_getMetaClass (const char *); */

  objc_get_meta_class_decl
    = builtin_function (TAG_GETMETACLASS, type, 0, NOT_BUILT_IN, NULL, NULL_TREE);

  /* APPLE LOCAL begin radar 4698856 */
  if (flag_objc_abi == 2)
    build_v2_class_template ();
  /* APPLE LOCAL end radar 4698856 */

  build_class_template ();
  build_super_template ();
  /* APPLE LOCAL begin ObjC new abi */
  if (flag_objc_abi == 2)
    {
      build_v2_protocol_template ();
      build_v2_category_template ();
    }
  /* APPLE LOCAL end ObjC new abi */
  build_protocol_template ();
  /* APPLE LOCAL begin 6348516 */
  save_warn_padded = warn_padded;
  warn_padded = 0;
  /* APPLE LOCAL end 6348516 */
  build_category_template ();
  build_objc_exception_stuff ();

  if (flag_next_runtime)
    build_next_objc_exception_stuff ();

  /* APPLE LOCAL 6348516 */
  warn_padded = save_warn_padded;
  /* static SEL _OBJC_SELECTOR_TABLE[]; */

  if (! flag_next_runtime)
    build_selector_table_decl ();

  /* Forward declare constant_string_id and constant_string_type.  */
  if (!constant_string_class_name)
    constant_string_class_name = default_constant_string_class_name;

  constant_string_id = get_identifier (constant_string_class_name);
  objc_declare_class (tree_cons (NULL_TREE, constant_string_id, NULL_TREE));

  /* Pre-build the following entities - for speed/convenience.  */
  self_id = get_identifier ("self");
  ucmd_id = get_identifier ("_cmd");

#ifdef OBJCPLUS
  pop_lang_context ();
#endif

  write_symbols = save_write_symbols;
  debug_hooks = save_hooks;
  /* APPLE LOCAL radar 4590221 */
  /* code removed */
}

/* Ensure that the ivar list for NSConstantString/NXConstantString
   (or whatever was specified via `-fconstant-string-class')
   contains fields at least as large as the following three, so that
   the runtime can stomp on them with confidence:

   struct STRING_OBJECT_CLASS_NAME
   {
     Object isa;
     char *cString;
     unsigned int length;
   }; */

static int
check_string_class_template (void)
{
  tree field_decl = objc_get_class_ivars (constant_string_id);

#define AT_LEAST_AS_LARGE_AS(F, T) \
  (F && TREE_CODE (F) == FIELD_DECL \
     && (TREE_INT_CST_LOW (TYPE_SIZE (TREE_TYPE (F))) \
	 >= TREE_INT_CST_LOW (TYPE_SIZE (T))))

  if (!AT_LEAST_AS_LARGE_AS (field_decl, ptr_type_node))
    return 0;

  field_decl = TREE_CHAIN (field_decl);
  if (!AT_LEAST_AS_LARGE_AS (field_decl, ptr_type_node))
    return 0;

  field_decl = TREE_CHAIN (field_decl);
  return AT_LEAST_AS_LARGE_AS (field_decl, unsigned_type_node);

#undef AT_LEAST_AS_LARGE_AS
}

/* Avoid calling `check_string_class_template ()' more than once.  */
static GTY(()) int string_layout_checked;

/* Construct an internal string layout to be used as a template for
   creating NSConstantString/NXConstantString instances.  */

static tree
objc_build_internal_const_str_type (void)
{
  tree type = (*lang_hooks.types.make_type) (RECORD_TYPE);
  tree fields = build_decl (FIELD_DECL, NULL_TREE, ptr_type_node);
  tree field = build_decl (FIELD_DECL, NULL_TREE, ptr_type_node);

  TREE_CHAIN (field) = fields; fields = field;
  field = build_decl (FIELD_DECL, NULL_TREE, unsigned_type_node);
  TREE_CHAIN (field) = fields; fields = field;
  /* NB: The finish_builtin_struct() routine expects FIELD_DECLs in
     reverse order!  */
  finish_builtin_struct (type, "__builtin_ObjCString",
			 fields, NULL_TREE);

  return type;
}

/* Custom build_string which sets TREE_TYPE!  */

static tree
my_build_string (int len, const char *str)
{
  return fix_string_type (build_string (len, str));
}

/* Build a string with contents STR and length LEN and convert it to a
   pointer.  */

static tree
my_build_string_pointer (int len, const char *str)
{
  tree string = my_build_string (len, str);
  tree ptrtype = build_pointer_type (TREE_TYPE (TREE_TYPE (string)));
  return build1 (ADDR_EXPR, ptrtype, string);
}

static hashval_t
string_hash (const void *ptr)
{
  tree str = ((struct string_descriptor *)ptr)->literal;
  const unsigned char *p = (const unsigned char *) TREE_STRING_POINTER (str);
  int i, len = TREE_STRING_LENGTH (str);
  hashval_t h = len;

  for (i = 0; i < len; i++)
    h = ((h * 613) + p[i]);

  return h;
}

static int
string_eq (const void *ptr1, const void *ptr2)
{
  tree str1 = ((struct string_descriptor *)ptr1)->literal;
  tree str2 = ((struct string_descriptor *)ptr2)->literal;
  int len1 = TREE_STRING_LENGTH (str1);

  return (len1 == TREE_STRING_LENGTH (str2)
	  && !memcmp (TREE_STRING_POINTER (str1), TREE_STRING_POINTER (str2),
		      len1));
}

/* APPLE LOCAL begin radar 5982789 */
/* This routine build "NSString" type if "NSString" class is declared
  in scope where it is needed. */
static tree buildNSStringType(void)
{
  tree NSString_decl;

  NSString_decl = objc_is_class_name (get_identifier ("NSString"));
  if (!NSString_decl)
    return NULL_TREE;
  return objc_get_protocol_qualified_type (NSString_decl, NULL_TREE);
}
/* APPLE LOCAL end radar 5982789 */

/* Given a chain of STRING_CST's, build a static instance of
   NXConstantString which points at the concatenation of those
   strings.  We place the string object in the __string_objects
   section of the __OBJC segment.  The Objective-C runtime will
   initialize the isa pointers of the string objects to point at the
   NXConstantString class object.  */

tree
objc_build_string_object (tree string)
{
  tree initlist, constructor, constant_string_class;
  int length;
  tree fields, addr;
  struct string_descriptor *desc, key;
  void **loc;

  /* APPLE LOCAL begin radar 5887355 */
#ifdef OBJCPLUS
  if (processing_template_decl)
    /* Must wait until template instantiation time.  */
    return build_min_nt (OBJC_STRING_REFERENCE, string);
#endif
  /* APPLE LOCAL end radar 5887355 */

  /* Prep the string argument.  */
  string = fix_string_type (string);
  TREE_SET_CODE (string, STRING_CST);
  length = TREE_STRING_LENGTH (string) - 1;

  /* APPLE LOCAL begin constant cfstrings */
  /* The target may have different ideas on how to construct an
     ObjC string literal.  On Darwin (Mac OS X), for example,
     we may wish to obtain a constant CFString reference instead.  */
  constructor = (*targetm.construct_objc_string) (string);
  /* APPLE LOCAL begin radar 4494634, 5982789 */
  if (constructor)
    {
      tree NSStringPtrType = buildNSStringType();
      return build1 (NOP_EXPR, 
                     NSStringPtrType ? build_pointer_type (NSStringPtrType)
                                     : objc_object_type, constructor);
    }
  /* APPLE LOCAL end radar 4494634, 5982789 */
  /* APPLE LOCAL end constant cfstrings */  
    
  /* Check whether the string class being used actually exists and has the
     correct ivar layout.  */
  if (!string_layout_checked)
    {
      string_layout_checked = -1;
      constant_string_class = lookup_interface (constant_string_id);
      internal_const_str_type = objc_build_internal_const_str_type ();

      if (!constant_string_class
	  || !(constant_string_type
	       = CLASS_STATIC_TEMPLATE (constant_string_class)))
	error ("cannot find interface declaration for %qs",
	       IDENTIFIER_POINTER (constant_string_id));
      /* The NSConstantString/NXConstantString ivar layout is now known.  */
      else if (!check_string_class_template ())
	error ("interface %qs does not have valid constant string layout",
	       IDENTIFIER_POINTER (constant_string_id));
      /* For the NeXT runtime, we can generate a literal reference
	 to the string class, don't need to run a constructor.  */
      else if (flag_next_runtime && !setup_string_decl ())
	error ("cannot find reference tag for class %qs",
	       IDENTIFIER_POINTER (constant_string_id));
      else
	{
	  string_layout_checked = 1;  /* Success!  */
	  add_class_reference (constant_string_id);
	}
    }

  if (string_layout_checked == -1)
    return error_mark_node;

  /* Perhaps we already constructed a constant string just like this one? */
  key.literal = string;
  loc = htab_find_slot (string_htab, &key, INSERT);
  desc = *loc;

  if (!desc)
    {
      tree var;
      *loc = desc = ggc_alloc (sizeof (*desc));
      desc->literal = string;

      /* GNU:    (NXConstantString *) & ((__builtin_ObjCString) { NULL, string, length })  */
      /* NeXT:   (NSConstantString *) & ((__builtin_ObjCString) { isa, string, length })   */
      fields = TYPE_FIELDS (internal_const_str_type);
      initlist
	= build_tree_list (fields,
			   flag_next_runtime
			   ? build_unary_op (ADDR_EXPR, string_class_decl, 0)
			   /* APPLE LOCAL radar 6285794 */
			   : integer_zero_node);
      fields = TREE_CHAIN (fields);
      initlist = tree_cons (fields, build_unary_op (ADDR_EXPR, string, 1),
			    initlist);
      fields = TREE_CHAIN (fields);
      initlist = tree_cons (fields, build_int_cst (NULL_TREE, length),
 			    initlist);
      constructor = objc_build_constructor (internal_const_str_type,
					    nreverse (initlist));
      TREE_INVARIANT (constructor) = true;

      if (!flag_next_runtime)
	constructor
	  = objc_add_static_instance (constructor, constant_string_type);
      else
        {
	  var = build_decl (CONST_DECL, NULL, TREE_TYPE (constructor));
	  DECL_INITIAL (var) = constructor;
	  TREE_STATIC (var) = 1;
	  pushdecl_top_level (var);
	  constructor = var;
	}
      desc->constructor = constructor;
    }

  addr = convert (build_pointer_type (constant_string_type),
		  build_unary_op (ADDR_EXPR, desc->constructor, 1));

  return addr;
}

/* Declare a static instance of CLASS_DECL initialized by CONSTRUCTOR.  */

static GTY(()) int num_static_inst;

static tree
objc_add_static_instance (tree constructor, tree class_decl)
{
  tree *chain, decl;
  char buf[256];

  /* Find the list of static instances for the CLASS_DECL.  Create one if
     not found.  */
  for (chain = &objc_static_instances;
       *chain && TREE_VALUE (*chain) != class_decl;
       chain = &TREE_CHAIN (*chain));
  if (!*chain)
    {
      *chain = tree_cons (NULL_TREE, class_decl, NULL_TREE);
      add_objc_string (OBJC_TYPE_NAME (class_decl), class_names);
    }

  sprintf (buf, "_OBJC_INSTANCE_%d", num_static_inst++);
  decl = build_decl (VAR_DECL, get_identifier (buf), class_decl);
  DECL_COMMON (decl) = 1;
  TREE_STATIC (decl) = 1;
  DECL_ARTIFICIAL (decl) = 1;
  TREE_USED (decl) = 1;
  DECL_INITIAL (decl) = constructor;

  /* We may be writing something else just now.
     Postpone till end of input.  */
  DECL_DEFER_OUTPUT (decl) = 1;
  pushdecl_top_level (decl);
  rest_of_decl_compilation (decl, 1, 0);

  /* Add the DECL to the head of this CLASS' list.  */
  TREE_PURPOSE (*chain) = tree_cons (NULL_TREE, decl, TREE_PURPOSE (*chain));

  return decl;
}

/* Build a static constant CONSTRUCTOR
   with type TYPE and elements ELTS.  */

static tree
objc_build_constructor (tree type, tree elts)
{
  tree constructor = build_constructor_from_list (type, elts);

  TREE_CONSTANT (constructor) = 1;
  TREE_STATIC (constructor) = 1;
  TREE_READONLY (constructor) = 1;

#ifdef OBJCPLUS
  /* Adjust for impedance mismatch.  We should figure out how to build
     CONSTRUCTORs that consistently please both the C and C++ gods.  */
  if (!TREE_PURPOSE (elts))
    TREE_TYPE (constructor) = NULL_TREE;
  TREE_HAS_CONSTRUCTOR (constructor) = 1;
#endif

  return constructor;
}

/* Take care of defining and initializing _OBJC_SYMBOLS.  */

/* Predefine the following data type:

   struct _objc_symtab
   {
     long sel_ref_cnt;
     SEL *refs;
     short cls_def_cnt;
     short cat_def_cnt;
     void *defs[cls_def_cnt + cat_def_cnt];
   }; */

static void
build_objc_symtab_template (void)
{
  tree field_decl, field_decl_chain;

  objc_symtab_template
    = start_struct (RECORD_TYPE, get_identifier (UTAG_SYMTAB));

  /* long sel_ref_cnt; */
  field_decl = create_field_decl (long_integer_type_node, "sel_ref_cnt");
  field_decl_chain = field_decl;

  /* SEL *refs; */
  field_decl = create_field_decl (build_pointer_type (objc_selector_type),
				  "refs");
  chainon (field_decl_chain, field_decl);

  /* short cls_def_cnt; */
  field_decl = create_field_decl (short_integer_type_node, "cls_def_cnt");
  chainon (field_decl_chain, field_decl);

  /* short cat_def_cnt; */
  field_decl = create_field_decl (short_integer_type_node,
				  "cat_def_cnt");
  chainon (field_decl_chain, field_decl);

  if (imp_count || cat_count || !flag_next_runtime)
    {
      /* void *defs[imp_count + cat_count (+ 1)]; */
      /* NB: The index is one less than the size of the array.  */
      int index = imp_count + cat_count
		+ (flag_next_runtime? -1: 0);
      field_decl = create_field_decl
		   (build_array_type
		    (ptr_type_node,
		     build_index_type (build_int_cst (NULL_TREE, index))),
		    "defs");
      chainon (field_decl_chain, field_decl);
    }

  finish_struct (objc_symtab_template, field_decl_chain, NULL_TREE);
}

/* Create the initial value for the `defs' field of _objc_symtab.
   This is a CONSTRUCTOR.  */

static tree
init_def_list (tree type)
{
  tree expr, initlist = NULL_TREE;
  struct imp_entry *impent;

  if (imp_count)
    for (impent = imp_list; impent; impent = impent->next)
      {
	if (TREE_CODE (impent->imp_context) == CLASS_IMPLEMENTATION_TYPE)
	  {
	    expr = build_unary_op (ADDR_EXPR, impent->class_decl, 0);
	    initlist = tree_cons (NULL_TREE, expr, initlist);
	  }
      }

  if (cat_count)
    for (impent = imp_list; impent; impent = impent->next)
      {
	if (TREE_CODE (impent->imp_context) == CATEGORY_IMPLEMENTATION_TYPE)
	  {
	    expr = build_unary_op (ADDR_EXPR, impent->class_decl, 0);
	    initlist = tree_cons (NULL_TREE, expr, initlist);
	  }
      }

  if (!flag_next_runtime)
    {
      /* statics = { ..., _OBJC_STATIC_INSTANCES, ... }  */
      tree expr;

      if (static_instances_decl)
	expr = build_unary_op (ADDR_EXPR, static_instances_decl, 0);
      else
	/* APPLE LOCAL radar 6285794 */
	expr = integer_zero_node;

      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  return objc_build_constructor (type, nreverse (initlist));
}

/* Construct the initial value for all of _objc_symtab.  */

static tree
init_objc_symtab (tree type)
{
  tree initlist;

  /* sel_ref_cnt = { ..., 5, ... } */

  initlist = build_tree_list (NULL_TREE,
			      build_int_cst (long_integer_type_node, 0));

  /* refs = { ..., _OBJC_SELECTOR_TABLE, ... } */

  if (flag_next_runtime || ! sel_ref_chain)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (build_pointer_type (objc_selector_type),
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    initlist
      = tree_cons (NULL_TREE,
		   convert (build_pointer_type (objc_selector_type),
			    build_unary_op (ADDR_EXPR,
					    UOBJC_SELECTOR_TABLE_decl, 1)),
		   initlist);

  /* cls_def_cnt = { ..., 5, ... } */

  /* APPLE LOCAL begin radar 4349670 */
  /* NULL_TREE for the type means to use integer_type_node.  However, this should 
     be a short. */
  initlist = tree_cons (NULL_TREE, build_int_cst (short_integer_type_node, 
						  imp_count), initlist);

  /* cat_def_cnt = { ..., 5, ... } */

  initlist = tree_cons (NULL_TREE, build_int_cst (short_integer_type_node, 
						  cat_count), initlist);
  /* APPLE LOCAL end radar 4349670 */

  /* cls_def = { ..., { &Foo, &Bar, ...}, ... } */

  if (imp_count || cat_count || !flag_next_runtime)
    {

      tree field = TYPE_FIELDS (type);
      field = TREE_CHAIN (TREE_CHAIN (TREE_CHAIN (TREE_CHAIN (field))));

      initlist = tree_cons (NULL_TREE, init_def_list (TREE_TYPE (field)),
			    initlist);
    }

  return objc_build_constructor (type, nreverse (initlist));
}

/* Generate forward declarations for metadata such as
  'OBJC_CLASS_...'.  */

static tree
/* APPLE LOCAL radar 5202926 */
build_metadata_decl (const char *name, tree type, bool external_linkage)
{
  tree decl;

  /* struct TYPE NAME_<name>; */
  decl = start_var_decl (type, synth_id_with_class_suffix
			       (name,
				objc_implementation_context));
  /* APPLE LOCAL begin radar 5202926 */
  if (external_linkage)
    TREE_PUBLIC (decl) = 1;
  /* APPLE LOCAL end radar 5202926 */
  return decl;
}

/* Push forward-declarations of all the categories so that
   init_def_list can use them in a CONSTRUCTOR.  */

static void
forward_declare_categories (void)
{
  struct imp_entry *impent;
  tree sav = objc_implementation_context;

  for (impent = imp_list; impent; impent = impent->next)
    {
      if (TREE_CODE (impent->imp_context) == CATEGORY_IMPLEMENTATION_TYPE)
	{
	  /* Set an invisible arg to synth_id_with_class_suffix.  */
	  objc_implementation_context = impent->imp_context;
	  /* APPLE LOCAL begin new ObjC abi v2 */
	  if (flag_objc_abi == 2)
	    {
	      /* APPLE LOCAL begin radar 6064186 */
	      /* extern struct category_t l_OBJC_$_CATEGORY_<name>; */
	      impent->class_v2_decl = build_metadata_decl ("l_OBJC_$_CATEGORY",
							   /* APPLE LOCAL radar 5202926 */
						           objc_v2_category_template, false); 
	      set_user_assembler_name (impent->class_v2_decl, 
				       IDENTIFIER_POINTER (DECL_NAME (impent->class_v2_decl)));
	      /* APPLE LOCAL end radar 6064186 */
	    }
	  else
	    {
	      /* extern struct objc_category _OBJC_CATEGORY_<name>; */
	      impent->class_decl = build_metadata_decl ("_OBJC_CATEGORY",
							/* APPLE LOCAL radar 5202926 */
						        objc_category_template, false);
	    }
	  /* APPLE LOCAL end new ObjC abi v2 */
	}
    }
  objc_implementation_context = sav;
}

/* Create the declaration of _OBJC_SYMBOLS, with type `struct _objc_symtab'
   and initialized appropriately.  */

static void
generate_objc_symtab_decl (void)
{
  /* forward declare categories */
  if (cat_count)
    forward_declare_categories ();

  /* APPLE LOCAL begin new ObjC abi v2 */
  /* APPLE LOCAL radar 4531482 */
  if (flag_objc_abi != 2)
    {
      build_objc_symtab_template ();
      UOBJC_SYMBOLS_decl = start_var_decl (objc_symtab_template, "_OBJC_SYMBOLS");
      finish_var_decl (UOBJC_SYMBOLS_decl,
		       init_objc_symtab (TREE_TYPE (UOBJC_SYMBOLS_decl)));
    }
  /* APPLE LOCAL end new ObjC abi v2 */
}

static tree
init_module_descriptor (tree type)
{
  tree initlist, expr;

  /* version = { 1, ... } */

  expr = build_int_cst (long_integer_type_node, OBJC_VERSION);
  initlist = build_tree_list (NULL_TREE, expr);

  /* size = { ..., sizeof (struct _objc_module), ... } */

  expr = convert (long_integer_type_node,
		  size_in_bytes (objc_module_template));
  initlist = tree_cons (NULL_TREE, expr, initlist);

  /* Don't provide any file name for security reasons. */
  /* name = { ..., "", ... } */

  expr = add_objc_string (get_identifier (""), class_names);
  initlist = tree_cons (NULL_TREE, expr, initlist);

  /* symtab = { ..., _OBJC_SYMBOLS, ... } */

  if (UOBJC_SYMBOLS_decl)
    expr = build_unary_op (ADDR_EXPR, UOBJC_SYMBOLS_decl, 0);
  else
    /* APPLE LOCAL radar 6285794 */
    expr = integer_zero_node;
  initlist = tree_cons (NULL_TREE, expr, initlist);

  return objc_build_constructor (type, nreverse (initlist));
}

/* Write out the data structures to describe Objective C classes defined.

   struct _objc_module { ... } _OBJC_MODULE = { ... };   */

static void
build_module_descriptor (void)
{
  tree field_decl, field_decl_chain;

#ifdef OBJCPLUS
  push_lang_context (lang_name_c); /* extern "C" */
#endif

  objc_module_template
    = start_struct (RECORD_TYPE, get_identifier (UTAG_MODULE));

  /* long version; */
  field_decl = create_field_decl (long_integer_type_node, "version");
  field_decl_chain = field_decl;

  /* long size; */
  field_decl = create_field_decl (long_integer_type_node, "size");
  chainon (field_decl_chain, field_decl);

  /* char *name; */
  field_decl = create_field_decl (string_type_node, "name");
  chainon (field_decl_chain, field_decl);

  /* struct _objc_symtab *symtab; */
  field_decl
    = create_field_decl (build_pointer_type
			 (xref_tag (RECORD_TYPE,
				    get_identifier (UTAG_SYMTAB))),
			 "symtab");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_module_template, field_decl_chain, NULL_TREE);

  /* Create an instance of "_objc_module".  */
  UOBJC_MODULES_decl = start_var_decl (objc_module_template, "_OBJC_MODULES");
  finish_var_decl (UOBJC_MODULES_decl,
		   init_module_descriptor (TREE_TYPE (UOBJC_MODULES_decl)));

#ifdef OBJCPLUS
  pop_lang_context ();
#endif
}

/* The GNU runtime requires us to provide a static initializer function
   for each module:

   static void __objc_gnu_init (void) {
     __objc_exec_class (&L_OBJC_MODULES);
   }  */

static void
build_module_initializer_routine (void)
{
  tree body;

#ifdef OBJCPLUS
  push_lang_context (lang_name_c); /* extern "C" */
#endif

  objc_push_parm (build_decl (PARM_DECL, NULL_TREE, void_type_node));
  /* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
  objc_start_function (get_identifier (TAG_GNUINIT),
		       build_function_type (void_type_node,
					    OBJC_VOID_AT_END),
		       NULL_TREE, objc_get_parm_info (0), NULL_TREE);
  /* APPLE LOCAL end radar 5839812 - location for synthesized methods  */

  body = c_begin_compound_stmt (true);
  add_stmt (build_function_call
	    (execclass_decl,
	     build_tree_list
	     (NULL_TREE,
	      build_unary_op (ADDR_EXPR,
			      UOBJC_MODULES_decl, 0))));
  add_stmt (c_end_compound_stmt (body, true));

  TREE_PUBLIC (current_function_decl) = 0;

#ifndef OBJCPLUS
  /* For Objective-C++, we will need to call __objc_gnu_init
     from objc_generate_static_init_call() below.  */
  DECL_STATIC_CONSTRUCTOR (current_function_decl) = 1;
#endif

  GNU_INIT_decl = current_function_decl;
  finish_function ();

#ifdef OBJCPLUS
    pop_lang_context ();
#endif
}

#ifdef OBJCPLUS
/* Return 1 if the __objc_gnu_init function has been synthesized and needs
   to be called by the module initializer routine.  */

int
objc_static_init_needed_p (void)
{
  return (GNU_INIT_decl != NULL_TREE);
}

/* Generate a call to the __objc_gnu_init initializer function.  */

tree
objc_generate_static_init_call (tree ctors ATTRIBUTE_UNUSED)
{
  add_stmt (build_stmt (EXPR_STMT,
			build_function_call (GNU_INIT_decl, NULL_TREE)));

  return ctors;
}
#endif /* OBJCPLUS */

/* Return the DECL of the string IDENT in the SECTION.  */
/* APPLE LOCAL begin radar 4359757 */
/* This routine has been rewritten for compile-time performance improvement. */

static tree
get_objc_string_decl (tree ident, enum string_section section)
{
  hash *hash_table;
  hash hsh;

  if (section == class_names)
    hash_table = class_names_hash_list;
  else if (section == meth_var_names)
    hash_table = meth_var_names_hash_list;
  else if (section == meth_var_types)
    hash_table = meth_var_types_hash_list;
  else if (section == prop_names_attr)
    hash_table = prop_names_attr_hash_list;
  else
    abort ();

  hsh = hash_ident_lookup (hash_table, ident);
  if (hsh)
    return hsh->list->value;

  abort ();
  return NULL_TREE;
}
/* APPLE LOCAL end radar 4359757 */

/* Output references to all statically allocated objects.  Return the DECL
   for the array built.  */

static void
generate_static_references (void)
{
  tree decls = NULL_TREE, expr = NULL_TREE;
  tree class_name, class, decl, initlist;
  tree cl_chain, in_chain, type
    = build_array_type (build_pointer_type (void_type_node), NULL_TREE);
  int num_inst, num_class;
  char buf[256];

  if (flag_next_runtime)
    abort ();

  for (cl_chain = objc_static_instances, num_class = 0;
       cl_chain; cl_chain = TREE_CHAIN (cl_chain), num_class++)
    {
      for (num_inst = 0, in_chain = TREE_PURPOSE (cl_chain);
	   in_chain; num_inst++, in_chain = TREE_CHAIN (in_chain));

      sprintf (buf, "_OBJC_STATIC_INSTANCES_%d", num_class);
      decl = start_var_decl (type, buf);

      /* Output {class_name, ...}.  */
      class = TREE_VALUE (cl_chain);
      class_name = get_objc_string_decl (OBJC_TYPE_NAME (class), class_names);
      initlist = build_tree_list (NULL_TREE,
				  build_unary_op (ADDR_EXPR, class_name, 1));

      /* Output {..., instance, ...}.  */
      for (in_chain = TREE_PURPOSE (cl_chain);
	   in_chain; in_chain = TREE_CHAIN (in_chain))
	{
	  expr = build_unary_op (ADDR_EXPR, TREE_VALUE (in_chain), 1);
	  initlist = tree_cons (NULL_TREE, expr, initlist);
	}

      /* Output {..., NULL}.  */
      /* APPLE LOCAL radar 6285794 */
      initlist = tree_cons (NULL_TREE, integer_zero_node, initlist);

      expr = objc_build_constructor (TREE_TYPE (decl), nreverse (initlist));
      finish_var_decl (decl, expr);
      decls
	= tree_cons (NULL_TREE, build_unary_op (ADDR_EXPR, decl, 1), decls);
    }

  /* APPLE LOCAL radar 6285794 */
  decls = tree_cons (NULL_TREE, integer_zero_node, decls);
  expr = objc_build_constructor (type, nreverse (decls));
  static_instances_decl = start_var_decl (type, "_OBJC_STATIC_INSTANCES");
  finish_var_decl (static_instances_decl, expr);
}

/* APPLE LOCAL begin radar 4349690 */
/* MERGE FIXME 5416374 See how

   2005-08-31  Andrew Pinski  <pinskia@physics.uc.edu> PR objc/23306

   Updated the code to not need update_var_decl and transform leftover code to use that strategy.  */

/* This routine is used to get finish_decl to install an initializer for this 
   forward definition and get the declaration resent to the backend.  After 
   this is called, finish_decl or finish_var_decl must be used. */

static tree
update_var_decl (tree decl)
{
  DECL_INITIAL (decl) = error_mark_node;
  DECL_EXTERNAL (decl) = 0;
  TREE_STATIC (decl) = 1;
  TREE_ASM_WRITTEN (decl) = 0;
  return decl;
}
/* APPLE LOCAL end radar 4349690 */

/* APPLE LOCAL begin ObjC new abi */
static GTY(()) int classlist_reference_idx;

/* This routine creates a global variable of type 'Class' to hold address of a
   class. */

static tree
build_classlist_reference_decl (void)
{
  tree decl;
  char buf[256];

  sprintf (buf, "_OBJC_CLASSLIST_REFERENCES_$_%d", classlist_reference_idx++);
  decl = start_var_decl (objc_class_type, buf);

  return decl;
}

/* Routine builds name of Interface's main meta-data of type class_t. */

static char*
objc_build_internal_classname (tree ident, bool metaclass)
{
  static char string[BUFSIZE];
  sprintf (string, "%s_%s", !metaclass ? "OBJC_CLASS_$" 
				       : "OBJC_METACLASS_$", 
			    IDENTIFIER_POINTER (ident));
  return string;
}

/* Build decl = initializer; for each externally visible class reference. */

static void
build_classlist_translation_table (bool metaclass_chain)
{
  tree chain;

  for (chain = !metaclass_chain ? classlist_ref_chain 
			        : metaclasslist_ref_chain; 
       chain; chain = TREE_CHAIN (chain))
    {
      tree expr = TREE_VALUE (chain);
      tree decl = TREE_PURPOSE (chain);
      /* Interface with no implementation and yet one of its messages has been
	 used. Need to generate a full address-of tree for it here. */
      if (TREE_CODE (expr) == IDENTIFIER_NODE)
        {
          expr = create_extern_decl (objc_v2_class_template,
				     objc_build_internal_classname (expr, metaclass_chain));
	  expr = convert (objc_class_type, build_fold_addr_expr (expr));
	}
      finish_var_decl (decl, expr);
      /* LLVM LOCAL begin - radar 5720120 */
#ifdef ENABLE_LLVM
      /* Reset the initializer for this reference as it most likely changed.  */
      if (!optimize)
        reset_initializer_llvm(decl);
#endif
      /* LLVM LOCAL end - radar 5720120 */
    }
}

/* APPLE LOCAL begin radar 4561192 */
/* Set the alignment to __attribute__ ((size_of_el_type)) */

static void
objc_set_alignment_attribute (tree decl, tree el_type)
{
  int int_size = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (el_type));
  tree size = build_int_cst (NULL_TREE, int_size);
  tree attribute = build_tree_list (get_identifier ("aligned"),
				    build_tree_list (NULL_TREE, size));
  decl_attributes (&decl, attribute, 0);
  return;
}
/* APPLE LOCAL end radar 4561192 */

/* Build the __class_list section table containing address of all @implemented class 
   meta-data. */

static void
build_class_list_address_table (bool nonlazy)
{
  tree chain;
  int count=0;
  tree type;
  tree initlist = NULL_TREE;
  tree decl;
  tree expr;
  tree list_chain = nonlazy ? nonlazy_class_list_chain : class_list_chain;
  const char *label_name = nonlazy ? "_OBJC_LABEL_NONLAZY_CLASS_$" 
			     	   : "_OBJC_LABEL_CLASS_$";

  for (chain = list_chain; chain; chain = TREE_CHAIN (chain))
    {
      tree purpose = NULL_TREE;
      expr = TREE_VALUE (chain);
      expr = convert (objc_class_type, build_fold_addr_expr (expr));
#ifndef OBJCPLUS
      purpose = build_int_cst (NULL_TREE, count);
#endif
      ++count;
      initlist = tree_cons (purpose, expr, initlist);
    }
  gcc_assert (count > 0);
  type = build_array_type (objc_class_type, 
			   build_index_type (build_int_cst (NULL_TREE, count - 1)));
  decl = start_var_decl (type, label_name);
  expr = objc_build_constructor (type, nreverse (initlist));
  /* APPLE LOCAL radar 4561192 */
  objc_set_alignment_attribute (decl, objc_class_type);
  finish_var_decl (decl, expr);
}

/* Build the __category_list (NONLAZY is false) or __nonlazy_category (NONLAZY is true)
   section table containing address of all @implemented category meta-data. */

static void
build_category_list_address_table (bool nonlazy)
{
  tree chain;
  int count=0;
  tree type;
  tree initlist = NULL_TREE;
  tree decl;
  tree expr;
  tree list_chain = nonlazy ? nonlazy_category_list_chain : category_list_chain;
  const char * label_name = nonlazy ? "_OBJC_LABEL_NONLAZY_CATEGORY_$" 
			      	    : "_OBJC_LABEL_CATEGORY_$";

  for (chain = list_chain; chain; chain = TREE_CHAIN (chain))
    {
      tree purpose = NULL_TREE;
      expr = TREE_VALUE (chain);
      expr = convert (objc_class_type, build_fold_addr_expr (expr));
#ifndef OBJCPLUS
      purpose = build_int_cst (NULL_TREE, count);
#endif
      ++count;
      initlist = tree_cons (purpose, expr, initlist);
    }
  gcc_assert (count > 0);
  type = build_array_type (objc_class_type,
			   build_index_type (build_int_cst (NULL_TREE, count - 1)));
  decl = start_var_decl (type, label_name);
  expr = objc_build_constructor (type, nreverse (initlist));
  /* APPLE LOCAL radar 4561192 */
  objc_set_alignment_attribute (decl, objc_class_type);
  finish_var_decl (decl, expr);
}
/* APPLE LOCAL end ObjC new abi */

static GTY(()) int selector_reference_idx;

static tree
build_selector_reference_decl (void)
{
  tree decl;
  char buf[256];

  sprintf (buf, "_OBJC_SELECTOR_REFERENCES_%d", selector_reference_idx++);
  decl = start_var_decl (objc_selector_type, buf);

  return decl;
}

static void
build_selector_table_decl (void)
{
  tree temp;

  if (flag_typed_selectors)
    {
      build_selector_template ();
      temp = build_array_type (objc_selector_template, NULL_TREE);
    }
  else
    temp = build_array_type (objc_selector_type, NULL_TREE);

  UOBJC_SELECTOR_TABLE_decl = start_var_decl (temp, "_OBJC_SELECTOR_TABLE");
}

/* Just a handy wrapper for add_objc_string.  */

static tree
build_selector (tree ident)
{
  return convert (objc_selector_type,
		  add_objc_string (ident, meth_var_names));
}

static void
build_selector_translation_table (void)
{
  tree chain, initlist = NULL_TREE;
  int offset = 0;
  tree decl = NULL_TREE;

  for (chain = sel_ref_chain; chain; chain = TREE_CHAIN (chain))
    {
      tree expr;

      if (warn_selector && objc_implementation_context)
      {
        tree method_chain;
        bool found = false;
        for (method_chain = meth_var_names_chain;
             method_chain;
             method_chain = TREE_CHAIN (method_chain))
          {
            if (TREE_VALUE (method_chain) == TREE_VALUE (chain))
              {
                found = true;
                break;
              }
          }
        if (!found)
	  {
	    location_t *loc;
	    if (flag_next_runtime && TREE_PURPOSE (chain))
	      loc = &DECL_SOURCE_LOCATION (TREE_PURPOSE (chain));
	    else
	      loc = &input_location;
	    warning (0, "%Hcreating selector for nonexistent method %qE",
		     loc, TREE_VALUE (chain));
	  }
      }

      expr = build_selector (TREE_VALUE (chain));
      /* add one for the '\0' character */
      offset += IDENTIFIER_LENGTH (TREE_VALUE (chain)) + 1;

      if (flag_next_runtime)
	{
	  decl = TREE_PURPOSE (chain);
	  finish_var_decl (decl, expr);
          /* LLVM LOCAL begin - radar 5676233 */
#ifdef ENABLE_LLVM
          /* Reset the initializer for this reference as it most likely
             changed.  */
          reset_initializer_llvm(decl);
#endif
          /* LLVM LOCAL end - radar 5676233 */
	}
      else
	{
	  if (flag_typed_selectors)
	    {
	      tree eltlist = NULL_TREE;
	      tree encoding = get_proto_encoding (TREE_PURPOSE (chain));
	      eltlist = tree_cons (NULL_TREE, expr, NULL_TREE);
	      eltlist = tree_cons (NULL_TREE, encoding, eltlist);
	      expr = objc_build_constructor (objc_selector_template,
					     nreverse (eltlist));
	    }

	  initlist = tree_cons (NULL_TREE, expr, initlist);
	}
    }

  if (! flag_next_runtime)
    {
      /* Cause the selector table (previously forward-declared)
	 to be actually output.  */
      initlist = tree_cons (NULL_TREE,
			    flag_typed_selectors
			    ? objc_build_constructor
			      (objc_selector_template,
			       tree_cons (NULL_TREE,
					  /* APPLE LOCAL radar 6285794 */
					  integer_zero_node,
					  tree_cons (NULL_TREE,
						     /* APPLE LOCAL radar 6285794 */
						     integer_zero_node,
						     NULL_TREE)))
			    /* APPLE LOCAL radar 6285794 */
			    : integer_zero_node, initlist);
      initlist = objc_build_constructor (TREE_TYPE (UOBJC_SELECTOR_TABLE_decl),
					 nreverse (initlist));
      finish_var_decl (UOBJC_SELECTOR_TABLE_decl, initlist);
    }
}

static tree
get_proto_encoding (tree proto)
{
  tree encoding;
  if (proto)
    {
      if (! METHOD_ENCODING (proto))
	{
	  encoding = encode_method_prototype (proto);
	  METHOD_ENCODING (proto) = encoding;
	}
      else
	encoding = METHOD_ENCODING (proto);

      return add_objc_string (encoding, meth_var_types);
    }
  else
    /* APPLE LOCAL radar 6285794 */
    return integer_zero_node;
}

/* sel_ref_chain is a list whose "value" fields will be instances of
   identifier_node that represent the selector.  */

static tree
build_typed_selector_reference (tree ident, tree prototype)
{
  tree *chain = &sel_ref_chain;
  tree expr;
  int index = 0;

  while (*chain)
    {
      if (TREE_PURPOSE (*chain) == prototype && TREE_VALUE (*chain) == ident)
	goto return_at_index;

      index++;
      chain = &TREE_CHAIN (*chain);
    }

  *chain = tree_cons (prototype, ident, NULL_TREE);

 return_at_index:
  expr = build_unary_op (ADDR_EXPR,
			 build_array_ref (UOBJC_SELECTOR_TABLE_decl,
					  build_int_cst (NULL_TREE, index)),
			 1);
  return convert (objc_selector_type, expr);
}

/* APPLE LOCAL begin radar 4359757 */
/* New routine replacing old build_selector_reference to improve 
   compile-time performance. */
static tree
build_next_runtime_selector_reference (tree ident)
{
  hash hsh;
  tree *chain = &sel_ref_chain;
  tree expr;

  hsh = hash_ident_lookup (sel_ref_hash_list, ident);
  if (hsh)
    return hsh->list->value;

  expr = build_selector_reference_decl ();
  hsh = hash_ident_enter (sel_ref_hash_list, ident);
  hash_add_attr (hsh, expr);

  *chain = tree_cons (expr, ident, *chain);

  return expr;
}

static tree
build_selector_reference (tree ident)
{
  tree *chain = &sel_ref_chain;
  tree expr;
  int index = 0;

  while (*chain)
    {
      if (TREE_VALUE (*chain) == ident)
	return build_array_ref (UOBJC_SELECTOR_TABLE_decl,
				build_int_cst (NULL_TREE, index));

      index++;
      chain = &TREE_CHAIN (*chain);
    }

  expr = NULL_TREE;
  *chain = tree_cons (expr, ident, NULL_TREE);

  return build_array_ref (UOBJC_SELECTOR_TABLE_decl,
			  build_int_cst (NULL_TREE, index));
}
/* APPLE LOCAL end radar 4359757 */

/* APPLE LOCAL begin ObjC new abi */
/* Declare a variable of type 'struct message_ref_t'. */

static GTY(()) int message_reference_idx;

/* APPLE LOCAL begin radar 5575115 - radar 6064186 */
static tree
build_message_reference_decl (tree sel_name, tree message_func_ident)
{
  tree decl;
  char *buf;
  char *temp;
  int fixed_prefix_size = 
        2 + IDENTIFIER_LENGTH (message_func_ident) + 1;
  buf = (char *)alloca (fixed_prefix_size + 
                        strlen (IDENTIFIER_POINTER (sel_name)) + 1);
  sprintf (buf, "l_%s_%s", IDENTIFIER_POINTER (message_func_ident),
		IDENTIFIER_POINTER (sel_name));
  for (temp = buf+fixed_prefix_size; *temp; temp++)
    if (*temp == ':') *temp = '_';

  decl = create_hidden_decl (objc_v2_message_ref_template, buf);
  DECL_WEAK (decl) = 1;
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  /* These are 4-byte aligned in 32-bit, 16-byte in 64-bit, which does not
     follow from the alignments of the component types. */
  DECL_ALIGN(decl) = BITS_PER_WORD==32 ? 32 : 128;
  DECL_USER_ALIGN(decl) = 1;

  /* Let optimizer know that this decl is not removable.  */
  DECL_PRESERVE_P (decl) = 1;
#endif
  /* LLVM LOCAL end */
  set_user_assembler_name (decl, buf);
  return decl;
}
/* APPLE LOCAL end radar 5575115 - radar 6064186 */

/* Build the list of (objc_msgSend_fixup_xxx, selector name) Used later on to
   initialize the table of 'struct message_ref_t' elements. */

static tree
build_selector_messenger_reference (tree sel_name, tree message_func_decl)
{
  tree *chain = &message_ref_chain;
  tree mess_expr;

  while (*chain)
    {
      if (TREE_VALUE (*chain) == message_func_decl)
	{
	  if (TREE_CHAIN (*chain) && TREE_VALUE (TREE_CHAIN (*chain)) == sel_name)
	    return TREE_PURPOSE (*chain);
	}
      chain = &TREE_CHAIN (*chain);
    }

  /* APPLE LOCAL radar 5575115 - radar 6064186 */
  mess_expr = build_message_reference_decl (sel_name, DECL_NAME (message_func_decl));
  *chain = tree_cons (mess_expr, message_func_decl, NULL_TREE);

  chain = &TREE_CHAIN (*chain);

  *chain = tree_cons (NULL_TREE, sel_name, NULL_TREE);

  return mess_expr;
}

/* Build the struct message_ref_t msg = 
	       {objc_msgSend_fixup_xxx, @selector(func)} 
   table.  */

static void
build_message_ref_translation_table (void)
{
  tree chain;
  tree decl = NULL_TREE;
  for (chain = message_ref_chain; chain; chain = TREE_CHAIN (chain))
    {
      tree expr;
      tree fields;
      tree initializer = NULL_TREE;
      tree constructor;
      tree struct_type;

      decl = TREE_PURPOSE (chain);
      struct_type = TREE_TYPE (decl);
      fields = TYPE_FIELDS (struct_type);

      /* First 'IMP messenger' field */
      expr = build_unary_op (ADDR_EXPR, TREE_VALUE (chain), 0);
      /* APPLE LOCAL begin radar 4582204 */
      expr = convert (objc_v2_imp_type, expr);
      initializer = build_tree_list (NULL_TREE, expr);
      /* APPLE LOCAL end radar 4582204 */
    
      /* Next the 'SEL name' field */
      fields = TREE_CHAIN (fields);
      gcc_assert (TREE_CHAIN (chain));
      chain = TREE_CHAIN (chain);
      expr = build_selector (TREE_VALUE (chain));
      /* APPLE LOCAL radar 4582204 */
      initializer = tree_cons (NULL_TREE, expr, initializer);
      constructor = objc_build_constructor (struct_type, nreverse (initializer));
      TREE_INVARIANT (constructor) = true;
      finish_var_decl (decl, constructor); 
      /* LLVM LOCAL begin - radar 5720120 */
#ifdef ENABLE_LLVM
      /* Reset the initializer for this reference as it most likely changed.  */
      reset_initializer_llvm(decl);
#endif
      /* LLVM LOCAL end - radar 5720120 */
    }
}

/* Assign all arguments in VALUES which have side-effect to a temporary and
   replaced that argument in VALUES list with the temporary. TYPELIST is the
   list of argument types. */

static tree
objc_copy_to_temp_side_effect_params (tree typelist, tree values)
{
  tree valtail, typetail;
  /* skip over receiver and the &_msf_ref types */
  gcc_assert (TREE_CHAIN (typelist));
  typetail = TREE_CHAIN (TREE_CHAIN (typelist));

  for (valtail = values;
       valtail; 
       valtail = TREE_CHAIN (valtail), typetail = TREE_CHAIN (typetail))
    {
      tree value = TREE_VALUE (valtail);
      tree type = typetail ? TREE_VALUE (typetail) : NULL_TREE;
      if (type == NULL_TREE)
	break; 
      if (!TREE_SIDE_EFFECTS (value))
	continue;
      /* To prevent reevaluation */
      value = save_expr (value);
      add_stmt (value);
      TREE_VALUE (valtail) = value;
    }
  return values;
}
/* APPLE LOCAL end ObjC new abi */

static GTY(()) int class_reference_idx;

static tree
build_class_reference_decl (void)
{
  tree decl;
  char buf[256];

  sprintf (buf, "_OBJC_CLASS_REFERENCES_%d", class_reference_idx++);
  decl = start_var_decl (objc_class_type, buf);

  return decl;
}

/* Create a class reference, but don't create a variable to reference
   it.  */

static void
add_class_reference (tree ident)
{
  tree chain;

  if ((chain = cls_ref_chain))
    {
      tree tail;
      do
        {
	  if (ident == TREE_VALUE (chain))
	    return;

	  tail = chain;
	  chain = TREE_CHAIN (chain);
        }
      while (chain);

      /* Append to the end of the list */
      TREE_CHAIN (tail) = tree_cons (NULL_TREE, ident, NULL_TREE);
    }
  else
    cls_ref_chain = tree_cons (NULL_TREE, ident, NULL_TREE);
}

/* APPLE LOCAL begin ObjC abi v2 */
/* This routine builds the class_reference_chain for each class name used
   in a [super class_name] message. IDENT is current a class's name. Initially,
   a (global_name, IDENT) tuple is added to the list. But in call to
   build_classlist_translation_table (), each IDENT is replaced with address of
   the class metadata (of type 'Class'). */

static tree
objc_v2_get_class_reference (tree ident)
{
  tree *chain;
  tree decl;

  for (chain = &classlist_ref_chain; *chain; chain = &TREE_CHAIN (*chain))
    if (TREE_VALUE (*chain) == ident)
      {
        if (! TREE_PURPOSE (*chain))
          TREE_PURPOSE (*chain) = build_classlist_reference_decl ();

        return TREE_PURPOSE (*chain);
      }

  decl = build_classlist_reference_decl ();
  *chain = tree_cons (decl, ident, NULL_TREE);
  return decl;
}
/* APPLE LOCAL end ObjC abi v2 */

/* APPLE LOCAL begin radar 4535676 */

/* This routine creates a global variable of type 'Class' to hold address of a
   class. */

static tree
build_classlist_super_reference_decl (void)
{
  tree decl;
  char buf[256];

  sprintf (buf, "_OBJC_CLASSLIST_SUP_REFS_$_%d", classlist_reference_idx++);
  decl = start_var_decl (objc_class_type, buf);

  return decl;
}
/* This routine builds the class_super_ref_chain/metaclass_super_ref_chain 
   for each class name used in a [super class_name] message. IDENT is 
   current a class's name. Initially,
   a (global_name, IDENT) tuple is added to the list. But in call to
   build_classlist_translation_table (), each IDENT is replaced with address of
   the class metadata (of type 'Class'). */

static tree
objc_v2_get_class_super_reference (tree ident, bool inst)
{
  tree *chain;
  tree decl;

  for (chain = inst ? &classlist_super_ref_chain : &metaclasslist_super_ref_chain; 
       *chain; chain = &TREE_CHAIN (*chain))
    if (TREE_VALUE (*chain) == ident)
      {
        if (! TREE_PURPOSE (*chain))
          TREE_PURPOSE (*chain) = build_classlist_super_reference_decl ();

        return TREE_PURPOSE (*chain);
      }

  decl = build_classlist_super_reference_decl ();
  *chain = tree_cons (decl, ident, NULL_TREE);
  return decl;
}

/* Build decl = initializer; for each externally visible class reference. */

static void
build_classlist_super_translation_table (bool metaclass_chain)
{
  tree chain;

  for (chain = !metaclass_chain ? classlist_super_ref_chain 
			        : metaclasslist_super_ref_chain; 
       chain; chain = TREE_CHAIN (chain))
    {
      tree expr = TREE_VALUE (chain);
      tree decl = TREE_PURPOSE (chain);
      /* Interface with no implementation and yet one of its messages has been
	 used. Need to generate a full address-of tree for it here. */
      if (TREE_CODE (expr) == IDENTIFIER_NODE)
        {
          expr = create_extern_decl (objc_v2_class_template,
				     objc_build_internal_classname (expr, metaclass_chain));
	  expr = convert (objc_class_type, build_fold_addr_expr (expr));
	}
      finish_var_decl (decl, expr);
      /* LLVM LOCAL begin - radar 5998983 */
#ifdef ENABLE_LLVM
      /* Reset the initializer for this reference as it most likely changed.  */
      if (!optimize)
        reset_initializer_llvm(decl);
#endif
      /* LLVM LOCAL end - radar 5998983 */
    }
}
/* APPLE LOCAL end radar 4535676 */

/* Get a class reference, creating it if necessary.  Also create the
   reference variable.  */

tree
objc_get_class_reference (tree ident)
{
  tree orig_ident = (DECL_P (ident)
		     ? DECL_NAME (ident)
		     : TYPE_P (ident)
		     ? OBJC_TYPE_NAME (ident)
		     : ident);
  bool local_scope = false;

  /* APPLE LOCAL begin prevent extra error diagnostics. */
  if (orig_ident == error_mark_node)
    return orig_ident;
  /* APPLE LOCAL end prevent extra error diagnostics. */

#ifdef OBJCPLUS
  if (processing_template_decl)
    /* Must wait until template instantiation time.  */
    return build_min_nt (CLASS_REFERENCE_EXPR, ident);
#endif

  if (TREE_CODE (ident) == TYPE_DECL)
    ident = (DECL_ORIGINAL_TYPE (ident)
	     ? DECL_ORIGINAL_TYPE (ident)
	     : TREE_TYPE (ident));

#ifdef OBJCPLUS
  if (TYPE_P (ident) && TYPE_CONTEXT (ident)
      && TYPE_CONTEXT (ident) != global_namespace)
    local_scope = true;
#endif

  if (local_scope || !(ident = objc_is_class_name (ident)))
    {
      error ("%qs is not an Objective-C class name or alias",
	     IDENTIFIER_POINTER (orig_ident));
      return error_mark_node;
    }

  /* APPLE LOCAL begin ObjC abi v2 */
  if (flag_objc_abi == 2 && !flag_zero_link)
    return objc_v2_get_class_reference (ident);
  /* APPLE LOCAL end ObjC abi v2 */
  if (flag_next_runtime && !flag_zero_link)
    {
      tree *chain;
      tree decl;

      for (chain = &cls_ref_chain; *chain; chain = &TREE_CHAIN (*chain))
	if (TREE_VALUE (*chain) == ident)
	  {
	    if (! TREE_PURPOSE (*chain))
	      TREE_PURPOSE (*chain) = build_class_reference_decl ();

	    return TREE_PURPOSE (*chain);
	  }

      decl = build_class_reference_decl ();
      *chain = tree_cons (decl, ident, NULL_TREE);
      return decl;
    }
  else
    {
      tree params;

      add_class_reference (ident);

      params = build_tree_list (NULL_TREE,
				my_build_string_pointer
				(IDENTIFIER_LENGTH (ident) + 1,
				 IDENTIFIER_POINTER (ident)));

      assemble_external (objc_get_class_decl);
      return build_function_call (objc_get_class_decl, params);
    }
}

/* APPLE LOCAL begin radar 4359757 */
/* New routine to do hash lookup keyed on IDENTIFIER node. */

static hash
hash_ident_lookup (hash *hashlist, tree sel_name)
{
  hash target;

  target = hashlist[hash_func (sel_name) % SIZEHASHTABLE];

  while (target)
    {
      if (sel_name == target->key)
        return target;

      target = target->next;
    }
  return 0;
}

/* New routine to add IDENTIFIER node to hash table. */
static hash
hash_ident_enter (hash *hashlist, tree ident)
{
  hash obj;
  int slot = hash_func (ident) % SIZEHASHTABLE;

  obj = (hash) ggc_alloc (sizeof (struct hashed_entry));
  obj->list = 0;
  obj->next = hashlist[slot];
  obj->key = ident;

  hashlist[slot] = obj;         /* append to front */
  return obj;
}

/* For each string section we have a chain which maps identifier nodes
   to decls for the strings.  */

static tree
add_objc_string (tree ident, enum string_section section)
{
  /* APPLE LOCAL do not create an unreferenced decl */
  tree *chain, decl, type, string_expr, sectionname;
  hash *hash_table;
  hash hsh;

  if (section == class_names)
    {
      chain = &class_names_chain;
      hash_table = class_names_hash_list;
    }
  else if (section == meth_var_names)
    {
      chain = &meth_var_names_chain;
      hash_table = meth_var_names_hash_list;
    }
  else if (section == meth_var_types)
    {
      chain = &meth_var_types_chain;
      hash_table = meth_var_types_hash_list;
    }
  else if (section == prop_names_attr)
    {
      chain = &prop_names_attr_chain;
      hash_table = prop_names_attr_hash_list;
    }
  else
    abort ();

  hsh = hash_ident_lookup (hash_table, ident);
  if (hsh)
    return convert (string_type_node, 
		    build_unary_op (ADDR_EXPR, hsh->list->value, 1));

  /* APPLE LOCAL do not create an unreferenced decl */
  sectionname = build_objc_string_ident (section);

  type = build_array_type
	 (char_type_node,
	  build_index_type
	  (build_int_cst (NULL_TREE,
			  IDENTIFIER_LENGTH (ident))));
  /* APPLE LOCAL do not create an unreferenced decl */
  decl = start_var_decl (type, IDENTIFIER_POINTER (sectionname));
  string_expr = my_build_string (IDENTIFIER_LENGTH (ident) + 1,
				 IDENTIFIER_POINTER (ident));
  finish_var_decl (decl, string_expr);

  hsh = hash_ident_enter (hash_table, ident);
  hash_add_attr (hsh, decl);
  *chain = tree_cons (decl, ident, *chain);

  return convert (string_type_node, build_unary_op (ADDR_EXPR, decl, 1));
}
/* APPLE LOCAL end radar 4359757 */

static GTY(()) int class_names_idx;
static GTY(()) int meth_var_names_idx;
static GTY(()) int meth_var_types_idx;
/* APPLE LOCAL C* property metadata (Radar 4498373) */
static GTY(()) int property_name_attr_idx;

static tree
/* APPLE LOCAL do not create an unreferenced decl */
build_objc_string_ident (enum string_section section)
{
/* APPLE LOCAL begin do not create an unreferenced decl */
/* APPLE LOCAL end do not create an unreferenced decl */
  char buf[256];

  if (section == class_names)
    sprintf (buf, "_OBJC_CLASS_NAME_%d", class_names_idx++);
  else if (section == meth_var_names)
    sprintf (buf, "_OBJC_METH_VAR_NAME_%d", meth_var_names_idx++);
  else if (section == meth_var_types)
    sprintf (buf, "_OBJC_METH_VAR_TYPE_%d", meth_var_types_idx++);
  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
  else if (section == prop_names_attr)
    sprintf (buf, "_OBJC_PROP_NAME_ATTR_%d", property_name_attr_idx++);
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
  /* APPLE LOCAL begin do not create an unreferenced decl */
  return get_identifier (buf);
  /* APPLE LOCAL end do not create an unreferenced decl */
}


void
objc_declare_alias (tree alias_ident, tree class_ident)
{
  tree underlying_class;

#ifdef OBJCPLUS
  if (current_namespace != global_namespace) {
    error ("Objective-C declarations may only appear in global scope");
  }
#endif /* OBJCPLUS */

  if (!(underlying_class = objc_is_class_name (class_ident)))
    warning (0, "cannot find class %qs", IDENTIFIER_POINTER (class_ident));
  else if (objc_is_class_name (alias_ident))
    warning (0, "class %qs already exists", IDENTIFIER_POINTER (alias_ident));
  else
    {
      /* Implement @compatibility_alias as a typedef.  */
#ifdef OBJCPLUS
      push_lang_context (lang_name_c); /* extern "C" */
#endif
      lang_hooks.decls.pushdecl (build_decl
				 (TYPE_DECL,
				  alias_ident,
				  xref_tag (RECORD_TYPE, underlying_class)));
#ifdef OBJCPLUS
      pop_lang_context ();
#endif
      /* APPLE LOCAL begin radar 4345837 */
      hash_class_name_enter (als_name_hash_list, alias_ident, 
			     underlying_class);
      /* APPLE LOCAL end radar 4345837 */
    }
}

void
objc_declare_class (tree ident_list)
{
  tree list;
#ifdef OBJCPLUS
  if (current_namespace != global_namespace) {
    error ("Objective-C declarations may only appear in global scope");
  }
#endif /* OBJCPLUS */

  for (list = ident_list; list; list = TREE_CHAIN (list))
    {
      tree ident = TREE_VALUE (list);

      if (! objc_is_class_name (ident))
	{
	  tree record = lookup_name (ident), type = record;

	  if (record)
	    {
	      if (TREE_CODE (record) == TYPE_DECL)
	      /* APPLE LOCAL begin radar 4278236 */
		type = DECL_ORIGINAL_TYPE (record) ? 
			DECL_ORIGINAL_TYPE (record) : 
			TREE_TYPE (record);
	      /* APPLE LOCAL end radar 4278236 */

	      /* APPLE LOCAL radar 4829851 */
	      if (TREE_CODE (type) != RECORD_TYPE || !TYPE_HAS_OBJC_INFO (type)
		  || !TYPE_OBJC_INTERFACE (type))
		{
		  error ("%qs redeclared as different kind of symbol",
			 IDENTIFIER_POINTER (ident));
		  error ("previous declaration of %q+D",
			 record);
		}
	    }

	  record = xref_tag (RECORD_TYPE, ident);
	  INIT_TYPE_OBJC_INFO (record);
	  TYPE_OBJC_INTERFACE (record) = ident;
	  /* APPLE LOCAL radar 4345837 */
	  hash_class_name_enter (cls_name_hash_list, ident, NULL_TREE);
	}
    }
}

tree
objc_is_class_name (tree ident)
{
  /* APPLE LOCAL radar 4345837 */
  hash target;

  if (ident && TREE_CODE (ident) == IDENTIFIER_NODE
      && identifier_global_value (ident))
    ident = identifier_global_value (ident);
  while (ident && TREE_CODE (ident) == TYPE_DECL && DECL_ORIGINAL_TYPE (ident))
    ident = OBJC_TYPE_NAME (DECL_ORIGINAL_TYPE (ident));

  if (ident && TREE_CODE (ident) == RECORD_TYPE)
    ident = OBJC_TYPE_NAME (ident);
#ifdef OBJCPLUS
  if (ident && TREE_CODE (ident) == TYPE_DECL)
    /* APPLE LOCAL begin radar 4407151 */
    {
      tree type = TREE_TYPE (ident);
      if (type && TREE_CODE (type) == TEMPLATE_TYPE_PARM)
        return NULL_TREE;
      ident = DECL_NAME (ident);
    }
    /* APPLE LOCAL end radar 4407151 */
#endif
  if (!ident || TREE_CODE (ident) != IDENTIFIER_NODE)
    return NULL_TREE;

  if (lookup_interface (ident))
    return ident;
  /* APPLE LOCAL begin radar 4345837 */
  target = hash_class_name_lookup (cls_name_hash_list, ident);
  if (target)
    return target->key;

  target = hash_class_name_lookup (als_name_hash_list, ident);
  if (target)
    {
      gcc_assert (target->list && target->list->value);
      return target->list->value;
    }
  /* APPLE LOCAL end radar 4345837 */

  return 0;
}

/* Check whether TYPE is either 'id' or 'Class'.  */

tree
objc_is_id (tree type)
{
  if (type && TREE_CODE (type) == IDENTIFIER_NODE
      && identifier_global_value (type))
    type = identifier_global_value (type);

  if (type && TREE_CODE (type) == TYPE_DECL)
    type = TREE_TYPE (type);

  /* NB: This function may be called before the ObjC front-end has
     been initialized, in which case OBJC_OBJECT_TYPE will (still) be NULL.  */
  return (objc_object_type && type
	  && (IS_ID (type) || IS_CLASS (type) || IS_SUPER (type))
	  ? type
	  : NULL_TREE);
}

/* Check whether TYPE is either 'id', 'Class', or a pointer to an ObjC
   class instance.  This is needed by other parts of the compiler to
   handle ObjC types gracefully.  */

tree
objc_is_object_ptr (tree type)
{
  tree ret;
  /* APPLE LOCAL begin radar 5595325 */
  if (objc_nsobject_attribute_type (type))
    return TREE_TYPE (type);
  /* APPLE LOCAL end radar 5595325 */

  type = TYPE_MAIN_VARIANT (type);
  if (!POINTER_TYPE_P (type))
    return 0;

  ret = objc_is_id (type);
  if (!ret)
    /* APPLE LOCAL ObjC GC */
    ret = objc_is_class_name (TYPE_MAIN_VARIANT (TREE_TYPE (type)));

  return ret;
}

/* APPLE LOCAL begin ObjC GC */
/* Return 1 if TYPE should be garbage collected, -1 if has __weak attribut
   and 0 otherwise.  Types marked with the '__strong' attribute are GC-able, 
   whereas those marked with __weak' are not.  Types marked with neither 
   attribute are GC-able if (a) they are Objective-C pointer types or (b) 
   they are pointers to types that are themselves GC-able.  */

/* APPLE LOCAL end ObjC GC */
/* APPLE LOCAL radar 5847976 */
int
/* APPLE LOCAL ObjC GC */
objc_is_gcable_type (tree type)
{
  /* APPLE LOCAL begin objc gc 5547128 */
  int strong;
  /* APPLE LOCAL end objc gc 5547128 */

  /* APPLE LOCAL ObjC GC */
  /* CODE FRAGMENT REMOVED.  */
  /* APPLE LOCAL begin radar 4829803 */
  if (!TYPE_P (type) 
#ifdef OBJCPLUS
      || type_unknown_p (type)
#endif
     )
  /* APPLE LOCAL end radar 4829803 */
    return 0;
  /* APPLE LOCAL begin ObjC GC */

  /* APPLE LOCAL objc gc 5547128 */
  strong = 0;
  do
    {
      /* The '__strong' and '__weak' keywords trump all.  */
      /* APPLE LOCAL objc gc 5547128 */
      strong = objc_is_strong_p (type);

      /* APPLE LOCAL begin radar 4426814 */
      if (strong)
	/* APPLE LOCAL objc gc 5547128 */
	break;
      /* APPLE LOCAL end radar 4426814 */

      /* APPLE LOCAL begin objc gc 5547128 */
      /* Function pointers are not GC-able.  */
      if (TREE_CODE (type) == FUNCTION_TYPE
	  || TREE_CODE (type) == METHOD_TYPE)
	break;
      /* APPLE LOCAL end objc gc 5547128 */

      /* Objective-C objects are GC-able, unless they were tagged with
	 '__weak'.  */
      if (objc_is_object_ptr (type))
 	/* APPLE LOCAL radar 4426814 */
	/* APPLE LOCAL begin objc gc 5547128 */
	{
	  strong = (objc_is_strong_p (TREE_TYPE (type)) >= 0 ? 1 : -1);
	  break;
	}
      /* APPLE LOCAL begin radar 5882266, 5832193 */
      /* Block pointer types are GC-able, unless they were tagged with
         '__weak' */
      if (TREE_CODE (type) == BLOCK_POINTER_TYPE)
        {
	  strong = block_pointer_strong_value (type);
          break;
        }
      /* APPLE LOCAL end radar 5882266, 5832193 */
      /* APPLE LOCAL end objc gc 5547128 */

      type = TREE_TYPE (type);
    }
  while (type);

  /* APPLE LOCAL objc gc 5547128 */
  return strong;
}
/* APPLE LOCAL end ObjC GC */
/* APPLE LOCAL begin radar 4982951 */
/* This routine retrievs the outervar of the objc2 tree EXPR representing address of an ivar.
   Tree EXPR is in the form of ((*typeof ivar)(OUTERVAR + OBJC2_IVAR_OFFSET_VAR)).
   It returns OUTERVAR. Note that input EXPR tree is already validate via call to
   objc_v2_ivar_reference. So no additional checking is done in this routine. */
static
tree objc2_outervar (tree expr)
{
  gcc_assert (TREE_CODE (expr) == NOP_EXPR);
  expr = TREE_OPERAND (expr, 0);
  gcc_assert (TREE_CODE (expr) == PLUS_EXPR);
  return TREE_OPERAND (expr, 0);
}

/* This routine builds * ((typeof (INDIR)*) 0 + OFFSET)
   This tree corresponds to * (&((typeof(outervar) *) 0)->ivar) in 'fragile' objc1
   abi. (see comment in objc_build_ivar_assignment)
*/

static tree
objc2_build_indirect_ref_ivar2 (tree indir, tree offset, tree newoutervar)
{
  tree ftype = TREE_TYPE (indir);
  tree expr = build_c_cast (string_type_node, newoutervar);
  /* (char*)newoutervar + offset */
  expr = fold (build2 (PLUS_EXPR, string_type_node, expr, offset));

  /* (ftype*)((char*)newoutervar + offset) */
  expr = build_c_cast (build_pointer_type (ftype), expr);

  /* Finally: *(ftype*)((char*)newoutervar + offset) */
  expr = build_indirect_ref (expr, "unary *");
  return expr;
}	/* This comment appeases the checklocal daemon.  */
/* APPLE LOCAL end radar 4982951 */

/* LLVM LOCAL - begin pointer arithmetic */
#ifdef ENABLE_LLVM
/* llvm-gcc intentionally preserves array notation &array[i] and avoids
   pointer arithmetic.

   Example 1: struct { int *a; } *b;    b->a[2] = 42;
   Example 2: struct { int a[42]; } *b;  b->a[2] = 42;

   In both of this cases, expr is preserved as ARRAY_REF. Normally, gcc
   would decomponse first example into a pointer arithmetic expression.
   So in llvm mode, check expression's field type to ensure that this is really
   an array reference or not.  */
static int objc_is_really_array_ref(tree expr) {
  tree base = NULL_TREE;

  if (TREE_CODE(expr) != ARRAY_REF)
    return 0;

  base = TREE_OPERAND(expr, 0);
  return base && TREE_CODE(TREE_TYPE(base)) == ARRAY_TYPE;
}
#endif
/* LLVM LOCAL - end pointer arithmetic */

static tree
objc_substitute_decl (tree expr, tree oldexpr, tree newexpr)
{
  if (expr == oldexpr)
    return newexpr;

  switch (TREE_CODE (expr))
    {
    case COMPONENT_REF:
      return objc_build_component_ref
	     (objc_substitute_decl (TREE_OPERAND (expr, 0),
				    oldexpr,
				    newexpr),
	      DECL_NAME (TREE_OPERAND (expr, 1)));
    /* LLVM LOCAL - begin pointer arithmetic */
#ifndef ENABLE_LLVM
    case ARRAY_REF:
      return build_array_ref (objc_substitute_decl (TREE_OPERAND (expr, 0),
						    oldexpr,
						    newexpr),
			      TREE_OPERAND (expr, 1));
#else
    /* Moved ARRAY_REF to "default" case */
#endif
    /* LLVM LOCAL - end pointer arithmetic */
    case INDIRECT_REF:
      /* APPLE LOCAL begin radar 4982951 */
      {
        tree ivar_offset_var;
        ivar_offset_var = objc_v2_ivar_reference (expr);
        if (ivar_offset_var != NULL_TREE)
          return objc2_build_indirect_ref_ivar2 (expr, ivar_offset_var, newexpr);
      }
      /* APPLE LOCAL end radar 4982951 */
      return build_indirect_ref (objc_substitute_decl (TREE_OPERAND (expr, 0),
						       oldexpr,
						       newexpr), "->");
    default:
      /* LLVM LOCAL - begin pointer arithmetic */
#ifdef ENABLE_LLVM
      if (objc_is_really_array_ref(expr))
        return build_array_ref (objc_substitute_decl (TREE_OPERAND (expr, 0),
                                                      oldexpr,
                                                      newexpr),
                                TREE_OPERAND (expr, 1));
#endif
      /* LLVM LOCAL - end pointer arithmetic */
      return expr;
    }
}

static tree
/* APPLE LOCAL radar 4995066 */
objc_build_ivar_assignment (tree outervar, bool objc2_ivar_ref, tree lhs, tree rhs)
{
  tree func_params;
  /* The LHS parameter contains the expression 'outervar->memberspec';
     we need to transform it into '&((typeof(outervar) *) 0)->memberspec',
     where memberspec may be arbitrarily complex (e.g., 'g->f.d[2].g[3]').
  */
  /* APPLE LOCAL begin radar 4783068 */
  tree offs, func;
  int save_warn_invalid_offsetof = warn_invalid_offsetof;
  warn_invalid_offsetof = 0;
  /* APPLE LOCAL begin radar 4982951 */
  /* APPLE LOCAL radar 4995066 */
  if (flag_objc_abi == 2 && objc2_ivar_ref)
    outervar = objc2_outervar (outervar);
  /* APPLE LOCAL end radar 4982951 */
  offs
    = objc_substitute_decl
      (lhs, outervar, convert (TREE_TYPE (outervar), integer_zero_node));
  warn_invalid_offsetof = save_warn_invalid_offsetof;
  /* APPLE LOCAL radar 4590221 */
  func = objc_assign_ivar_decl;
  /* APPLE LOCAL end radar 4783068 */

  /* APPLE LOCAL objc gc 5547128 */
  offs = convert (ptrdiff_type_node, build_unary_op (ADDR_EXPR, offs, 0));
  offs = fold (offs);
  func_params = tree_cons (NULL_TREE,
	convert (objc_object_type, rhs),
	    tree_cons (NULL_TREE, convert (objc_object_type, outervar),
		tree_cons (NULL_TREE, offs,
		    NULL_TREE)));

  assemble_external (func);
  return build_function_call (func, func_params);
}

static tree
/* APPLE LOCAL radar 4426814 */
objc_build_global_assignment (tree lhs, tree rhs, int strong)
{
  tree func_params = tree_cons (NULL_TREE,
	convert (objc_object_type, rhs),
	    tree_cons (NULL_TREE, convert (build_pointer_type (objc_object_type),
		      build_unary_op (ADDR_EXPR, lhs, 0)),
		    NULL_TREE));
  /* APPLE LOCAL begin radar 4426814 */
  tree func = (strong == 1) ? objc_assign_global_decl : objc_assign_weak_decl;

  assemble_external (func);
  return build_function_call (func, func_params);
}
/* APPLE LOCAL end radar 4426814 */

/* APPLE LOCAL begin radar 4426814 */

/* APPLE LOCAL begin radar 4591756 */
/* This routine generates call to objc_read_weak (&EXPR);  */

static tree
objc_build_weak_read (tree expr)
{
  tree func_params;
  expr = build_unary_op (ADDR_EXPR, expr, 1);
  expr = convert (build_pointer_type (objc_object_type), expr);
  func_params = tree_cons (NULL_TREE, expr, NULL_TREE);

  assemble_external (objc_read_weak_decl);
  return build_function_call (objc_read_weak_decl, func_params);
}
/* APPLE LOCAL end radar 4591756 */

/* APPLE LOCAL begin radar 5276085 */
/* Return TRUE if EXPR is a 'weak' expression */
static bool
objc_tree_is_weak_expr (tree expr)
{
#ifdef OBJCPLUS
  if (processing_template_decl)
    return false;
#endif
  if (skip_evaluation || TREE_CODE (expr) == OBJC_WEAK_REFERENCE_EXPR
      || TREE_TYPE (expr) == NULL_TREE)
    return false;

  return (objc_is_gcable_p (expr) == -1);
}

/* This routine builds a tree referencing the 'weak' object pointer expression */
tree
objc_build_weak_reference_tree (tree expr)
{
  /* APPLE LOCAL begin radar 5847976 */
  if (!flag_objc_gc)
    return expr;
  /* APPLE LOCAL end radar 5847976 */
  return objc_tree_is_weak_expr (expr) ?
         build1 (OBJC_WEAK_REFERENCE_EXPR, TREE_TYPE (expr), expr) : expr;
}

/* Return 'weak' reference object pointer expression */
void
objc_weak_reference_expr (tree *ref)
{
  tree expr = *ref;
  if (expr && TREE_CODE (expr) == OBJC_WEAK_REFERENCE_EXPR)
    *ref = TREE_OPERAND (expr, 0);
}
/* APPLE LOCAL end radar 5276085 */
/* APPLE LOCAL end radar 4426814 */


static tree
objc_build_strong_cast_assignment (tree lhs, tree rhs)
{
  /* APPLE LOCAL begin objc gc 5547128 */
  tree func_params
    = tree_cons (NULL_TREE,
		 convert (objc_object_type, rhs),
		 tree_cons (NULL_TREE, convert (build_pointer_type (objc_object_type),
						build_unary_op (ADDR_EXPR, lhs, 0)),
			    NULL_TREE));
  /* APPLE LOCAL end objc gc 5547128 */

  assemble_external (objc_assign_strong_cast_decl);
  return build_function_call (objc_assign_strong_cast_decl, func_params);
}

/* APPLE LOCAL begin radar 4592503 */
/* This routine checks to see if a local variable or a structure field
   has __weak attribute and issues warning in that case.  */
void
objc_checkon_weak_attribute (tree decl)
{
  /* APPLE LOCAL begin radar 5747976 */
  if (!flag_objc_gc)
    return;
  /* APPLE LOCAL end radar 5747976 */
  if (decl && DECL_P (decl))
    {
      if (objc_collecting_ivars
	  || (TREE_CODE (decl) == VAR_DECL
	      && (TREE_STATIC (decl) || DECL_EXTERNAL (decl))))
	return;

      /* APPLE LOCAL radar 5847976 */
      if ((TREE_CODE (decl) == FIELD_DECL || (TREE_CODE (decl) == VAR_DECL && !COPYABLE_BYREF_LOCAL_VAR (decl)))
	  && objc_is_gcable_type (TREE_TYPE (decl)) == -1)
	{
	  location_t saved_location = input_location;
	  input_location = DECL_SOURCE_LOCATION (decl);
	  warning (0, "__weak attribute cannot be specified on a %s declaration",
		   TREE_CODE (decl) == FIELD_DECL ? "field" : "local object");
	  input_location = saved_location;
	  return;
	}
    }
}

/* APPLE LOCAL end radar 4592503 */

/* APPLE LOCAL begin radar 3742561 */
/* This routine build a call to void *objc_memmove_collectable (&LHS, &RHS, sizeof(TYPE))
*/
static tree
objc_build_memmove_collectable (tree lhs, tree rhs, tree type)
{
  tree arg1 = convert (ptr_type_node, build_unary_op (ADDR_EXPR, lhs, 1));
  tree arg2 = convert (ptr_type_node, build_unary_op (ADDR_EXPR, rhs, 1));
  int size = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (type));
  tree arg3 = build_int_cst (NULL_TREE, size);
  tree func_params = tree_cons (NULL_TREE, arg1,
		       tree_cons (NULL_TREE, arg2,
		         tree_cons (NULL_TREE, arg3, NULL_TREE)));
  assemble_external (objc_memmove_collectable_decl);
  return build_function_call (objc_memmove_collectable_decl, func_params);  
}

/* Check aggregate type and return 1 if it contains a field of object pointer type or one
   whose pointer type is '__strong'. */

static int
objc_aggr_type_strong (tree aggr_type)
{
  tree  fieldlist, field_decl;

  if (!flag_objc_gc && !flag_objc_gc_only)
    return 0;

  gcc_assert (TREE_CODE (aggr_type) == RECORD_TYPE 
	      || TREE_CODE (aggr_type) == UNION_TYPE);

  fieldlist = TYPE_FIELDS (aggr_type);

  for (field_decl = fieldlist; field_decl; field_decl = TREE_CHAIN (field_decl))
  {
    if (TREE_TYPE (field_decl) == error_mark_node)
      continue;

    if (field_decl && TREE_CODE (field_decl) == FIELD_DECL
        && DECL_NAME (field_decl) && !DECL_BIT_FIELD_TYPE (field_decl)
#ifdef OBJCPLUS
        && !TREE_STATIC (field_decl)
#endif
       )
    {
      tree field_type = TREE_TYPE (field_decl);
      if (TREE_CODE (field_type) == ARRAY_TYPE)
        {
	  /* Get element type of the array. */
	  field_type = TREE_TYPE (field_type);
	  while (TREE_CODE (field_type) == ARRAY_TYPE)
	    field_type = TREE_TYPE (field_type);
        }
      if (TREE_CODE (field_type) == RECORD_TYPE || TREE_CODE (field_type) == UNION_TYPE)
	{
	  if (objc_aggr_type_strong (field_type))
	    return 1;
	  else
	    continue;
	}
      if (objc_is_gcable_type (field_type) == 1)
	return 1;
      continue;
    }
  }
  return 0;
}
/* APPLE LOCAL end radar 3742561 */

/* APPLE LOCAL begin ObjC GC */
/* Return 1 if EXPR is marked with the __strong attribute, -1 if it is marked
   with the __weak attribute, and 0 if it is marked with neither (regardless
   if it is otherwise GC-able).  */

static int
objc_is_strong_p (tree expr)
{
  if (TYPE_P (expr) || DECL_P (expr))
    {
      tree attr = lookup_attribute ("objc_gc",
				    (TYPE_P (expr)
				     ? TYPE_ATTRIBUTES (expr)
				     : DECL_ATTRIBUTES (expr)));

      if (attr && TREE_VALUE (attr))
	{
	  if (TREE_VALUE (TREE_VALUE (attr)) == get_identifier ("strong"))
	    return 1;

	  if (TREE_VALUE (TREE_VALUE (attr)) == get_identifier ("weak"))
	    return -1;
	}
    }

  return 0;
}

/* Return 1 (__strong) if a call to a write-barrier should be generated when assigning
   to EXPR.  Return -1 if a call to objc_assign_weak should be generated when assigning
   to  EXPR with a __weak attribute; 0 otherwise. */

/* APPLE LOCAL end ObjC GC */
static int
objc_is_gcable_p (tree expr)
/* APPLE LOCAL begin ObjC GC */
{
  tree t = TREE_TYPE (expr);
  /* The '__strong' and '__weak' keywords trump all.  */
  int strong = objc_is_strong_p (t);

  /* APPLE LOCAL begin radar 4426814 */
  if (strong)
    return strong;
  /* APPLE LOCAL end radar 4426814 */

  /* Discard lvalue casts, if any.  */
  while (TREE_CODE (expr) == INDIRECT_REF
	 && TREE_CODE (TREE_OPERAND (expr, 0)) == NOP_EXPR
	 && TREE_CODE (TREE_OPERAND (TREE_OPERAND (expr, 0), 0)) == ADDR_EXPR)
    expr = TREE_OPERAND (TREE_OPERAND (TREE_OPERAND (expr, 0), 0), 0);

  /* Zero in on the variable/parameter being assigned to.  */
  /* LLVM LOCAL - begin pointer arithmetic */
#ifdef ENABLE_LLVM
  while (TREE_CODE (expr) == COMPONENT_REF || objc_is_really_array_ref(expr))
#else
  while (TREE_CODE (expr) == COMPONENT_REF || TREE_CODE (expr) == ARRAY_REF)
#endif
  /* LLVM LOCAL - end pointer arithmetic */
    expr = TREE_OPERAND (expr, 0);

  /* Parameters and local variables (and their fields) are NOT GC-able.  */
  if (TREE_CODE (expr) == PARM_DECL ||
      (TREE_CODE (expr) == VAR_DECL
       && DECL_CONTEXT (expr)
       && !TREE_STATIC (expr)))
    return 0;

  return objc_is_gcable_type (t);
}
/* APPLE LOCAL end ObjC GC */

/* APPLE LOCAL begin radar 4894756 */
/* This routine checks and returns true if tree is reference to an 'ivar' through the ivar offset
   variable reference. See objc_v2_build_ivar_ref for detail of how this tree is generated. */

static
tree objc_v2_ivar_reference (tree expr)
{
  if (flag_objc_abi < 2 || TREE_CODE (expr) != INDIRECT_REF)
    return NULL_TREE;
  expr = TREE_OPERAND (expr, 0);
  if (TREE_CODE (expr) != NOP_EXPR)
    return NULL_TREE;
  expr = TREE_OPERAND (expr, 0);
  if (TREE_CODE (expr) != PLUS_EXPR)
    return NULL_TREE;
  expr = TREE_OPERAND (expr, 1);

  return (TREE_CODE (expr) == VAR_DECL && DECL_NAME (expr)
          && strncmp (IDENTIFIER_POINTER (DECL_NAME (expr)), STRING_V2_IVAR_OFFSET_PREFIX,
                      strlen (STRING_V2_IVAR_OFFSET_PREFIX)) == 0) ? expr : NULL_TREE;
}
/* APPLE LOCAL end radar 4894756 */

/* APPLE LOCAL begin radar 4591756 */
static tree
objc_strip_off_indirection (tree expr)
{
  if ((TREE_CODE (expr) == INDIRECT_REF && (objc_v2_ivar_reference (expr) == NULL_TREE))
      || TREE_CODE (expr) == NOP_EXPR
      || TREE_CODE (expr) == ADDR_EXPR || TREE_CODE (expr) == PLUS_EXPR)
    return objc_strip_off_indirection (TREE_OPERAND (expr, 0));
  return expr;
}
/* APPLE LOCAL end radar 4591756 */

static int
objc_is_ivar_reference_p (tree expr)
{
  /* LLVM LOCAL - begin pointer arithmetic */
#ifdef ENABLE_LLVM
  return (objc_is_really_array_ref(expr) == 1 
#else
  return (TREE_CODE (expr) == ARRAY_REF
#endif
  /* LLVM LOCAL - end pointer arithmetic */
	  ? objc_is_ivar_reference_p (TREE_OPERAND (expr, 0))
	  : TREE_CODE (expr) == COMPONENT_REF
	  ? TREE_CODE (TREE_OPERAND (expr, 1)) == FIELD_DECL
	  /* APPLE LOCAL objc2 4894756 */
	  : (objc_v2_ivar_reference (expr) != NULL_TREE));
}

static int
objc_is_global_reference_p (tree expr)
{
  return (TREE_CODE (expr) == INDIRECT_REF || TREE_CODE (expr) == PLUS_EXPR
	  ? objc_is_global_reference_p (TREE_OPERAND (expr, 0))
	  : DECL_P (expr)
	  ? (!DECL_CONTEXT (expr) || TREE_STATIC (expr))
	  : 0);
}

tree
objc_generate_write_barrier (tree lhs, enum tree_code modifycode, tree rhs)
{
  /* APPLE LOCAL begin ObjC GC */
  tree outer;
  int indirect_p = 0;
  /* APPLE LOCAL radar 4426814 */
  int strong;
  bool objc2_ivar_ref;
  /* APPLE LOCAL begin radar 3742561 */
  tree lhs_type = TREE_TYPE (lhs);
  if (TREE_CODE (lhs_type) == RECORD_TYPE || TREE_CODE (lhs_type) == UNION_TYPE)
    {
      strong = objc_aggr_type_strong (lhs_type);
      return strong ? objc_build_memmove_collectable (lhs, rhs, lhs_type) 
		    : NULL_TREE;
    }
  /* APPLE LOCAL end radar 3742561 */

  /* APPLE LOCAL begin objc gc 5547128 */
  /* Only pointers need barriers.  We can get here when
       __strong float *p; p[0] = 3.14;  */
  /* APPLE LOCAL begin radar 5832193 */
  if (TREE_CODE (lhs_type) != POINTER_TYPE
      && TREE_CODE (TREE_TYPE (rhs)) != POINTER_TYPE
      && TREE_CODE (lhs_type) != BLOCK_POINTER_TYPE
      && TREE_CODE (TREE_TYPE (rhs)) != BLOCK_POINTER_TYPE)
  /* APPLE LOCAL end radar 5832193 */
    return NULL_TREE;
  /* APPLE LOCAL end objc gc 5547128 */

  /* the lhs must be of a suitable type, regardless of its underlying
     structure.  Furthermore, __weak must not have been used.  */
  /* APPLE LOCAL radar 4426814 */
  if ((strong = objc_is_gcable_p (lhs)) == 0)
    return NULL_TREE;

  /* At this point, we are committed to using one of the write-barriers,
     unless the user is attempting to perform pointer arithmetic.  */
  /* APPLE LOCAL radar 4291099 */
  if (modifycode != NOP_EXPR && modifycode != INIT_EXPR)
    {
      warning (0, "pointer arithmetic for garbage-collected objects not allowed");
      return NULL_TREE;
    }
  /* APPLE LOCAL end ObjC GC */
  /* APPLE LOCAL radar 4291099 */
  gcc_assert (modifycode != INIT_EXPR || c_dialect_cxx ());

  /* APPLE LOCAL ObjC GC */
  /* CODE FRAGMENT REMOVED.  */
  outer = lhs;

  /* APPLE LOCAL begin radar 4591756 */
  if (strong == -1)
    outer = objc_strip_off_indirection (outer);
  /* APPLE LOCAL end radar 4591756 */
  while (outer
	 && (TREE_CODE (outer) == COMPONENT_REF
             /* LLVM LOCAL - begin pointer arithmetic */
#ifdef ENABLE_LLVM
             || objc_is_really_array_ref(outer)))
#else
	     || TREE_CODE (outer) == ARRAY_REF))
#endif
             /* LLVM LOCAL - end pointer arithmetic */
    outer = TREE_OPERAND (outer, 0);

  /* APPLE LOCAL objc2 */
  objc2_ivar_ref = objc_v2_ivar_reference (outer) != NULL_TREE;
  /* APPLE LOCAL objc2 */
  if (TREE_CODE (outer) == INDIRECT_REF && !objc2_ivar_ref)
    {
      outer = TREE_OPERAND (outer, 0);
      indirect_p = 1;
    }

  /* APPLE LOCAL ObjC GC */
  /* CODE FRAGMENT REMOVED.  */

  /* Handle ivar assignments. */
  /* APPLE LOCAL begin ObjC GC */
  /* APPLE LOCAL begin radar 4591756 */
  if (indirect_p && objc_is_ivar_reference_p (strong == -1 
						? objc_strip_off_indirection (lhs) 
						: lhs)
  /* APPLE LOCAL end radar 4591756 */
      && objc_is_object_ptr (TREE_TYPE (outer)))
  /* APPLE LOCAL end ObjC GC */
    {
      /* APPLE LOCAL ObjC GC */
      /* CODE FRAGMENT REMOVED.  */
      if (warn_assign_intercept)
	warning (0, "instance variable assignment has been intercepted");

      /* APPLE LOCAL begin radar 4426814 */
      /* APPLE LOCAL ObjC GC */
      return strong == 1 
		/* APPLE LOCAL radar 4995066 */
		? objc_build_ivar_assignment (outer, objc2_ivar_ref, lhs, rhs)
		: objc_build_global_assignment (lhs, rhs, strong);
      /* APPLE LOCAL end radar 4426814 */
    }
  /* APPLE LOCAL begin objc2 */
  if (!indirect_p && objc2_ivar_ref)
    {
      if (warn_assign_intercept)
	warning (0, "instance variable assignment has been intercepted");

      return strong == 1
		/* APPLE LOCAL radar 4995066 */
		? objc_build_ivar_assignment (TREE_OPERAND (outer, 0), objc2_ivar_ref, lhs, rhs)
	        : objc_build_global_assignment (lhs, rhs, strong);
    }
  /* APPLE LOCAL end objc2 */
  /* Likewise, intercept assignment to global/static variables if their type is
     GC-marked.  */
  /* APPLE LOCAL ObjC GC */
  if (!indirect_p && objc_is_global_reference_p (outer))
    {
      /* APPLE LOCAL ObjC GC */
      /* CODE FRAGMENT REMOVED.  */
      /* APPLE LOCAL begin confused diff */
      if (warn_assign_intercept)
	warning (0, "global/static variable assignment has been intercepted");
      /* APPLE LOCAL end confused diff */

      /* APPLE LOCAL begin radar 4426814 */
      /* APPLE LOCAL ObjC GC */
      return objc_build_global_assignment (lhs, rhs, strong);
      /* APPLE LOCAL end radar 4426814 */
    /* APPLE LOCAL confused diff */
    }
  /* APPLE LOCAL begin radar 4426814 */
  if (strong == -1)
    {
      /* APPLE LOCAL begin radar 5847976 */
      /* Must generate write barrier for __block objects which are __weak
         and used in an assignment. */
      if (TREE_CODE (outer) == COMPONENT_REF && TREE_OPERAND (outer, 0)
	  && TREE_CODE (TREE_OPERAND (outer, 0)) == VAR_DECL)
        {
	  tree var_decl = TREE_OPERAND (outer, 0);
	  if (COPYABLE_WEAK_BLOCK (var_decl))
	    return objc_build_global_assignment (lhs, rhs, -1);
        }
      /* APPLE LOCAL end radar 5847976 */
      return NULL_TREE;
    }
  /* APPLE LOCAL end radar 4426814 */

  /* LLVM LOCAL - begin 5541393 + 6522054 */
#ifdef ENABLE_LLVM
  if (TREE_CODE(TREE_TYPE(lhs)) != POINTER_TYPE  &&
      TREE_CODE(TREE_TYPE(lhs)) != BLOCK_POINTER_TYPE) return NULL_TREE; 
#endif
  /* LLVM LOCAL - end 5541393 + 6522054 */
  /* APPLE LOCAL begin ObjC GC */
  /* Use the strong-cast write barrier as a last resort.  */
  if (warn_assign_intercept)
    warning (0, "strong-cast assignment has been intercepted");

  return objc_build_strong_cast_assignment (lhs, rhs);
  /* APPLE LOCAL end ObjC GC */
/* APPLE LOCAL confused diff */
}

/* APPLE LOCAL begin ObjC new abi */

/* This routine returns the ivar declaration, if component is a valid ivar field; 
   NULL_TREE otherwise. On finding an ivar, it also returns the class name in CLASS.  */

static tree
objc_is_ivar (tree expr, tree component, tree *class)
{
  tree field = NULL_TREE;
  tree basetype = TYPE_MAIN_VARIANT (TREE_TYPE (expr));

  if (TREE_CODE (basetype) == RECORD_TYPE 
      && TYPE_HAS_OBJC_INFO (basetype) && TYPE_OBJC_INTERFACE (basetype))
    {
      *class = lookup_interface (OBJC_TYPE_NAME (basetype));
      if (*class)
	{
	  do
	    {
	      tree ivar_chain = CLASS_RAW_IVARS (*class);
	      if (ivar_chain)
		{
		  field = is_ivar (ivar_chain, component);
		  if (field != NULL_TREE)
		    break;
	        }
	      *class = lookup_interface (CLASS_SUPER_NAME (*class));
	    }
	  while (*class);
	}
    }
  return field;
} 

/* APPLE LOCAL begin radar 4441049 */
/* This routine creates an OFFSET_IVAR variable for COMPONENT_REF EXP
   and returns it. This routine is for bitfield ivars only.  */
tree
objc_v2_component_ref_field_offset (tree exp)
{
  char var_offset_name[512];
  tree class_name, offset;
  tree datum = TREE_OPERAND (exp, 0);
  tree field = TREE_OPERAND (exp, 1);
  tree component = DECL_NAME (field);

  /* unnamed bitfields are not user ivars. */
  if (!component)
    return NULL_TREE;

  if (!((flag_objc_abi == 2)
        && (field = objc_is_ivar (datum, component, &class_name))))
    return NULL_TREE;

  /* This routine must only be called for bitfield ivars. */
  /* DECL_INITIAL macro is set to width of bitfield and can be relied on to
     check for bitfield ivars. Note that I cannot rely on DECL_BIT_FIELD macro
     because it is only set when the whole struct is seen (at finish_struct) 
     and not when the ivar chain is built. */
  gcc_assert (DECL_INITIAL (field));

  create_ivar_offset_name (var_offset_name, CLASS_NAME (class_name), 
			   field);
  /* NOTE! type of variable for ivar offset MUST match type of offset assumed
     by the front-end. Otherwise, FE asserts when attempting to do futher
     math on the tree whose one operand is one of these offsets. */
  offset = create_extern_decl (TREE_TYPE (size_zero_node), var_offset_name);
  return offset;
}

/* This routine computes the bit position from the beginning of its byte. 
   This is only done for bitfield ivars. */

tree
objc_v2_bitfield_ivar_bitpos (tree exp)
{
  tree class_name;
  int bitpos;
  tree datum = TREE_OPERAND (exp, 0);
  tree field = TREE_OPERAND (exp, 1);
  tree component = DECL_NAME (field);
  tree ivar_field;

  if (!((flag_objc_abi == 2)
        && (ivar_field = objc_is_ivar (datum, component, &class_name))))
    return NULL_TREE;
  
  /* This routine must only be called for bitfield ivars. */
  /* DECL_INITIAL macro is set to width of bitfield and can be relied on to
     check for bitfield ivars. Note that I cannot rely on DECL_BIT_FIELD macro
     because it is only set when the whole struct is seen (at finish_struct) 
     and not when the ivar chain is built. */
  gcc_assert (DECL_INITIAL (ivar_field));

  bitpos = tree_low_cst (DECL_FIELD_BIT_OFFSET (field), 0);
  bitpos = bitpos % BITS_PER_UNIT;
  return build_int_cst (TREE_TYPE (bitsize_zero_node), bitpos);
}

/* APPLE LOCAL end radar 4441049 */

/* This routine generates new abi's ivar reference tree. It amounts to generating
   *(TYPE*)((char*)pObj + OFFSET_IVAR) when we normally generate pObj->IVAR
   OFFSET_IVAR is an 'extern' variable holding the offset for 'IVAR' field. TYPE
   is type of IVAR field.  */

tree
objc_v2_build_ivar_ref (tree datum, tree component)
{
  tree field, ref, class_name, offset, ftype, expr;
  char var_offset_name[512];

  if (!((flag_objc_abi == 2)
        && (field = objc_is_ivar (datum, component, &class_name))))
    return NULL_TREE;

  /* APPLE LOCAL begin radar 4441049 */
  /* This routine only handles non-bitfield fields */
  /* DECL_INITIAL macro is set to width of bitfield and can be relied on to
     check for bitfield ivars. Note that I cannot rely on DECL_BIT_FIELD macro
     because it is only set when the whole struct is seen (at finish_struct) 
     and not when the ivar chain is built. */
  if (DECL_INITIAL (field))
    return NULL_TREE;
  /* APPLE LOCAL end radar 4441049 */
  /* APPLE LOCAL begin radar 4954480 */
  if (TREE_CODE (datum) == INDIRECT_REF && (TREE_OPERAND (datum, 0) == null_pointer_node
#ifdef OBJCPLUS
    || (TREE_CODE (TREE_OPERAND (datum, 0)) == NOP_EXPR 
	&& TREE_OPERAND (TREE_OPERAND (datum, 0), 0) == null_pointer_node)
#endif
     ))
    {
      /* APPLE LOCAL radar 6230800 */
      error ("Illegal reference to non-fragile ivar");
      return error_mark_node;
    }
  /* APPLE LOCAL end radar 4954480 */

  create_ivar_offset_name (var_offset_name, CLASS_NAME (class_name), 
			   field);
  /* APPLE LOCAL radar 4441049 */
  offset = create_extern_decl (TREE_TYPE (size_zero_node), var_offset_name);

  ftype = TREE_TYPE (field);

  /* (char*)datum */
  expr = build_c_cast (string_type_node, 
		       build_fold_addr_expr (datum));

  /* (char*)datum + offset */
  expr = fold (build2 (PLUS_EXPR, string_type_node, expr, offset));
  
  /* (ftype*)((char*)datum + offset) */
  expr = build_c_cast (build_pointer_type (ftype), expr);

  /* Finally: *(ftype*)((char*)datum + offset) */
  ref = build_indirect_ref (expr, "unary *");

  /* We must set type of the resulting expression to be the same as the
     field type. This is because, build_indirect_ref (...) rebuilds the
     type which may result in lost information; as in the case of 
     protocol-qualified types (id <protocol> ). */
  TREE_TYPE (ref) = ftype;

  if (TREE_READONLY (datum) || TREE_READONLY (field))
    TREE_READONLY (ref) = 1;

  if (TREE_THIS_VOLATILE (datum) || TREE_THIS_VOLATILE (field))
    TREE_THIS_VOLATILE (ref) = 1;

  if (TREE_DEPRECATED (field))
    warn_deprecated_use (field);

  /* APPLE LOCAL begin "unavailable" attribute (radar 2809697) */
  if (TREE_UNAVAILABLE (field))
    error_unavailable_use (field);
  /* APPLE LOCAL end "unavailable" attribute (radar 2809697) */

  /* APPLE LOCAL begin radar 5376125 */
  if (!flag_objc_gc && warn_direct_ivar_access)
    objc_warn_direct_ivar_access (TYPE_MAIN_VARIANT (TREE_TYPE (datum)), 
				  component);
  /* APPLE LOCAL end radar 5376125 */
  return ref;
}
/* APPLE LOCAL end ObjC new abi */

struct interface_tuple GTY(())
{
  tree id;
  tree class_name;
};

static GTY ((param_is (struct interface_tuple))) htab_t interface_htab;

static hashval_t
hash_interface (const void *p)
{
  const struct interface_tuple *d = p;
  return IDENTIFIER_HASH_VALUE (d->id);
}

static int
eq_interface (const void *p1, const void *p2)
{
  const struct interface_tuple *d = p1;
  return d->id == p2;
}

static tree
lookup_interface (tree ident)
{
#ifdef OBJCPLUS
  if (ident && TREE_CODE (ident) == TYPE_DECL)
    ident = DECL_NAME (ident);
#endif

  if (ident == NULL_TREE || TREE_CODE (ident) != IDENTIFIER_NODE)
    return NULL_TREE;

  {
    struct interface_tuple **slot;
    tree i = NULL_TREE;

    if (interface_htab)
      {
	slot = (struct interface_tuple **)
	  htab_find_slot_with_hash (interface_htab, ident,
				    IDENTIFIER_HASH_VALUE (ident),
				    NO_INSERT);
	if (slot && *slot)
	  i = (*slot)->class_name;
      }
    return i;
  }
}

/* Implement @defs (<classname>) within struct bodies.  */

tree
objc_get_class_ivars (tree class_name)
{
  tree interface = lookup_interface (class_name);

  if (interface)
    return get_class_ivars (interface, true);

  error ("cannot find interface declaration for %qs",
	 IDENTIFIER_POINTER (class_name));

  return error_mark_node;
}

/* APPLE LOCAL begin radar 4291785 */
/* Generate an error for any duplicate field names in FIELDLIST.  Munge
   the list such that this does not present a problem later.  */

void
objc_detect_field_duplicates (tree fieldlist)
{
  tree x, y;
  int timeout = 10;
#ifdef OBJCPLUS
  /* for objective-c++, we only care about duplicate checking of ivars. */
  if (!objc_collecting_ivars)
    return;
#endif

  /* First, see if there are more than "a few" fields.
     This is trivially true if there are zero or one fields.  */
  if (!fieldlist)
    return;
  x = TREE_CHAIN (fieldlist);
  if (!x)
    return;
  do {
    timeout--;
    x = TREE_CHAIN (x);
  } while (timeout > 0 && x);

  /* If there were "few" fields, avoid the overhead of allocating
     a hash table.  Instead just do the nested traversal thing.  */
  if (timeout > 0)
    {
      for (x = TREE_CHAIN (fieldlist); x ; x = TREE_CHAIN (x))
        if (DECL_NAME (x))
          {
            for (y = fieldlist; y != x; y = TREE_CHAIN (y))
              if (DECL_NAME (y) == DECL_NAME (x))
                {
		  if (objc_collecting_ivars)
		    {
		      error ("%Jduplicate member %qD", y, y);
		      DECL_NAME (y) = NULL_TREE;
		    }
		  else
		    {
		      error ("%Jduplicate member %qD", x, x);
		      DECL_NAME (x) = NULL_TREE;
		    }
                }
          }
    }
  else
    {
      htab_t htab = htab_create (37, htab_hash_pointer, htab_eq_pointer, NULL);
      void **slot;

      for (x = fieldlist; x ; x = TREE_CHAIN (x))
        if ((y = DECL_NAME (x)) != 0)
          {
            slot = htab_find_slot (htab, y, INSERT);
            if (*slot)
              {
	        if (objc_collecting_ivars)
	          {
		    tree z;
		    /* Hackery to get the correct position of the duplicate field.
		       It is slow, but we are reporting error, remember. */
		    for (z = fieldlist; z ; z = TREE_CHAIN (z))
		      if (DECL_NAME (x) == DECL_NAME (z))
			{
		          error ("%Jduplicate member %qD", z, z);
			  DECL_NAME (z) = NULL_TREE;
		          break;
			}
		  }
	        else
		  {
		    error ("%Jduplicate member %qD", x, x);
                    DECL_NAME (x) = NULL_TREE;
		  }
              }
            *slot = y;
          }

      htab_delete (htab);
    }
}

/* For current interface with inherited interface chain, this function returns the
   flattened list of ivars in current and inherited interfaces. Otherwise, it
   returns the argument passed to it. */
tree
objc_get_interface_ivars (tree fieldlist)
{
  tree ivar_chain;
  tree interface;
  if (!objc_collecting_ivars || !objc_interface_context 
      || TREE_CODE (objc_interface_context) != CLASS_INTERFACE_TYPE
      || CLASS_SUPER_NAME (objc_interface_context) == NULL_TREE)
    return fieldlist;
  interface = objc_interface_context;
  gcc_assert (TOTAL_CLASS_RAW_IVARS (objc_interface_context) == NULL_TREE);
  ivar_chain = copy_list (CLASS_RAW_IVARS (interface));
  if (CLASS_SUPER_NAME (interface))
    {
      /* Prepend super-class ivars.  */
      interface = lookup_interface (CLASS_SUPER_NAME (interface));
      /* Root base interface may not have its TOTAL_CLASS_RAW_IVARS set yet. */
      if (TOTAL_CLASS_RAW_IVARS (interface) == NULL_TREE)
        TOTAL_CLASS_RAW_IVARS (interface) = copy_list (CLASS_RAW_IVARS (interface));
      ivar_chain = chainon (ivar_chain, TOTAL_CLASS_RAW_IVARS (interface));
    }
  TOTAL_CLASS_RAW_IVARS (objc_interface_context) = ivar_chain;
  return ivar_chain;
}
/* APPLE LOCAL end radar 4291785 */

/* Used by: build_private_template, continue_class,
   and for @defs constructs.  */

static tree
get_class_ivars (tree interface, bool inherited)
{
  tree ivar_chain = copy_list (CLASS_RAW_IVARS (interface));

  /* Both CLASS_RAW_IVARS and CLASS_IVARS contain a list of ivars declared
     by the current class (i.e., they do not include super-class ivars).
     However, the CLASS_IVARS list will be side-effected by a call to
     finish_struct(), which will fill in field offsets.  */
  if (!CLASS_IVARS (interface))
    CLASS_IVARS (interface) = ivar_chain;

  if (!inherited)
    return ivar_chain;

  while (CLASS_SUPER_NAME (interface))
    {
      /* Prepend super-class ivars.  */
      interface = lookup_interface (CLASS_SUPER_NAME (interface));
      ivar_chain = chainon (copy_list (CLASS_RAW_IVARS (interface)),
			    ivar_chain);
    }

  return ivar_chain;
}

static tree
objc_create_temporary_var (tree type)
{
  tree decl;

  decl = build_decl (VAR_DECL, NULL_TREE, type);
  TREE_USED (decl) = 1;
  DECL_ARTIFICIAL (decl) = 1;
  DECL_IGNORED_P (decl) = 1;
  DECL_CONTEXT (decl) = current_function_decl;

  return decl;
}


/* APPLE LOCAL begin radar 2848255 */
static tree
create_weak_decl (tree type, const char *name)
{
    tree decl = create_global_decl (type, name);
    DECL_WEAK (decl) = 1;
    return decl;
}

/* This routine builds objc2's typeinfo type for the purpose of zero-cost exception.
   Declaration of this type is:

   struct _objc_ehtype_t {
     void *_objc_ehtype_vtable;
     const char *className;
     struct class_t *const cls;
   }
*/

static void
objc2_build_ehtype (void)
{
  tree field_decl, field_decl_chain;
  objc2_ehtype_template = start_struct (RECORD_TYPE,
				     get_identifier ("_objc_ehtype_t"));

  /* void *_objc_ehtype_vtable; */
  field_decl = create_field_decl (ptr_type_node, "_objc_ehtype_vtable");
  field_decl_chain = field_decl;

  /* const char *className; */
  field_decl = create_field_decl (string_type_node, "className");
  chainon (field_decl_chain, field_decl);

  /* struct class_t *const cls; */
  field_decl = create_field_decl (build_pointer_type (objc_v2_class_template),
				  "cls");
  chainon (field_decl_chain, field_decl);
  finish_struct (objc2_ehtype_template, field_decl_chain, NULL_TREE);
}

/* Declare objc_ehtype_vtable and OBJC_EHTYPE_id. */
static
void init_UOBJC2_EHTYPE_decls (void)
{
      /* declare extern void *objc_ehtype_vtable */
      if (!UOBJC2_EHTYPE_VTABLE_decl)
        {
          UOBJC2_EHTYPE_VTABLE_decl = lookup_name (get_identifier ("objc_ehtype_vtable"));
          if (!UOBJC2_EHTYPE_VTABLE_decl)
	    UOBJC2_EHTYPE_VTABLE_decl = create_extern_decl (ptr_type_node, 
							    "objc_ehtype_vtable");
	}
      /* declare extern struct _objc_ehtype_t OBJC_EHTYPE_id; */
      if (!UOBJC2_EHTYPE_id_decl)
	{
          UOBJC2_EHTYPE_id_decl = lookup_name (get_identifier ("OBJC_EHTYPE_id")); 
          if (!UOBJC2_EHTYPE_id_decl)
            UOBJC2_EHTYPE_id_decl = create_extern_decl (objc2_ehtype_template, 
						        "OBJC_EHTYPE_id");
	}

}

/* This routine builds initializer list for object of type struct _objc_ehtype_t.
*/
 
static tree
objc2_build_ehtype_initializer (tree name, tree cls)
{
  tree initlist = NULL_TREE;
  tree addr;
  init_UOBJC2_EHTYPE_decls ();
  /* APPLE LOCAL begin radar 4995942 */
  /* _objc_ehtype_vtable + 2*sizeof(void*) */
  addr = 
    fold (build2 (MULT_EXPR, ptr_type_node, 
	  TYPE_SIZE_UNIT (ptr_type_node), build_int_cst (NULL_TREE, 2)));
  addr = fold (build2 (PLUS_EXPR, ptr_type_node, 
	      build_fold_addr_expr (UOBJC2_EHTYPE_VTABLE_decl), addr));
  initlist = tree_cons (NULL_TREE, addr, initlist);
  /* APPLE LOCAL end radar 4995942 */

  /* className */
  initlist = tree_cons (NULL_TREE, name, initlist);

  /* cls */
  initlist = tree_cons (NULL_TREE, cls, initlist);

  return objc_build_constructor (objc2_ehtype_template, nreverse (initlist));
}

static char*
objc2_internal_ehtype_name (tree cls_name_ident)
{
  static char string[BUFSIZE];
  sprintf (string, "%s_%s", STRING_V2_OBJC_EHTYPE_PREFIX, IDENTIFIER_POINTER (cls_name_ident));
  return string;
}

/* This routine declares objective-c's 'typeinfo' data structure. */

static tree
/* APPLE LOCAL radar 5008110 */
objc2_build_ehtype_var_decl (tree cls_name_ident, bool extern_decl, bool weak_decl)
{
  tree initlist, ehtype_vdecl;
  tree name_expr, class_name_expr;

  const char* string = objc2_internal_ehtype_name (cls_name_ident);
  /* APPLE LOCAL begin radar 5008110 */
  if (extern_decl)
    return create_extern_decl (objc2_ehtype_template, string);
  name_expr = add_objc_string (cls_name_ident, class_names);
  class_name_expr = create_extern_decl (objc_v2_class_template,
					objc_build_internal_classname (cls_name_ident, false));
  class_name_expr = build_fold_addr_expr (class_name_expr);
  ehtype_vdecl = weak_decl ? create_weak_decl (objc2_ehtype_template, string)
			   : create_global_decl (objc2_ehtype_template, string);
  /* APPLE LOCAL end radar 5008110 */
  initlist = objc2_build_ehtype_initializer (name_expr, class_name_expr);
  finish_var_decl (ehtype_vdecl, initlist);
  return ehtype_vdecl;
}

#ifdef OBJCPLUS
/* This routine returns true if TYPE is a valid objective-c type which can be
   used in a @catch clause */
bool
objc2_valid_objc_catch_type (tree type)
{
  return flag_objc_zerocost_exceptions && managed_objc_object_pointer (type);
}
#endif

/* APPLE LOCAL begin radar 5008110 */
/* This routine returns TRUE if CLS or any of its super classes have their 
   __objc_exception__ atttribute set.
*/
static bool
objc2_objc_exception_attr (tree cls)
{
  tree chain;
  while (cls)
    {
      for (chain = CLASS_ATTRIBUTES (cls); chain; chain = TREE_CHAIN (chain))
        if (is_attribute_p ("objc_exception", TREE_PURPOSE (chain)))
          return true;
      cls = lookup_interface (CLASS_SUPER_NAME (cls));
    }

  return false; 
}

/* APPLE LOCAL end radar 5008110 */

/* This hook, called via lang_eh_runtime_type, generates a runtime object
   which is either the address of the 'OBJC_EHTYPE_$_class' object or
   address of external OBJC_EHTYPE_id object.
*/
static tree
objc2_eh_runtime_type (tree type)
{
  tree t;
  if (type == error_mark_node)
    return type;
  if (POINTER_TYPE_P (type) && objc_is_object_id (TREE_TYPE (type)))
    return build_fold_addr_expr (UOBJC2_EHTYPE_id_decl);
  if (!POINTER_TYPE_P (type) || !TYPED_OBJECT (TREE_TYPE (type)))
    {
#ifdef OBJCPLUS
      /* This routine is also called for c++'s catch clause; in which case,
	 we use c++'s typeinfo decl. */
      return objcp_build_eh_type_type (type);
#else
      error ("non-objective-c type '%T' cannot be caught", type);
      return error_mark_node;
#endif
    }

  t = OBJC_TYPE_NAME (TREE_TYPE (type));
  /* APPLE LOCAL begin radar 5008110 */
  t = objc2_build_ehtype_var_decl (t, 
	objc2_objc_exception_attr (lookup_interface (t)), true);
  /* APPLE LOCAL end radar 5008110 */
  return build_fold_addr_expr (t);
}

/* APPLE LOCAL begin radar 4951324 */
/* This routine declares an external class object to be used by
   objc2_build_ehtype_var_decl routine. */
static void
objc2_build_extern_decl_catch_object (tree type)
{
  if (POINTER_TYPE_P (type) && objc_is_object_id (TREE_TYPE (type)))
    return;

  if (POINTER_TYPE_P (type) && TYPED_OBJECT (TREE_TYPE (type)))
    {
      tree t = OBJC_TYPE_NAME (TREE_TYPE (type));
      (void)add_objc_string (t, class_names);
      (void) create_extern_decl (objc_v2_class_template, 
				 objc_build_internal_classname (t, false));
      if (objc2_objc_exception_attr (lookup_interface (t)))
	(void) create_extern_decl (objc2_ehtype_template, objc2_internal_ehtype_name (t));
    }
}
/* APPLE LOCAL end radar 4951324 */
/* APPLE LOCAL end radar 2848255 */

/* Exception handling constructs.  We begin by having the parser do most
   of the work and passing us blocks.  What we do next depends on whether
   we're doing "native" exception handling or legacy Darwin setjmp exceptions.
   We abstract all of this in a handful of appropriately named routines.  */

/* Stack of open try blocks.  */

struct objc_try_context
{
  struct objc_try_context *outer;

  /* Statements (or statement lists) as processed by the parser.  */
  tree try_body;
  tree finally_body;

  /* Some file position locations.  */
  location_t try_locus;
  location_t end_try_locus;
  location_t end_catch_locus;
  location_t finally_locus;
  location_t end_finally_locus;

  /* A STATEMENT_LIST of CATCH_EXPRs, appropriate for sticking into op1
     of a TRY_CATCH_EXPR.  Even when doing Darwin setjmp.  */
  tree catch_list;

  /* The CATCH_EXPR of an open @catch clause.  */
  tree current_catch;

  /* The VAR_DECL holding the Darwin equivalent of EXC_PTR_EXPR.  */
  tree caught_decl;
  tree stack_decl;
  tree rethrow_decl;
};

static struct objc_try_context *cur_try_context;

/* LLVM LOCAL begin 7069718 */
#ifdef ENABLE_LLVM
/* Do nothing (return NULL_TREE).  */
static tree return_null_tree (void);

tree
return_null_tree (void)
{
  return NULL_TREE;
}
#endif
/* LLVM LOCAL end 7069718 */

/* This hook, called via lang_eh_runtime_type, generates a runtime object
   that represents TYPE.  For Objective-C, this is just the class name.  */
/* ??? Isn't there a class object or some such?  Is it easy to get?  */

#ifndef OBJCPLUS
static tree
objc_eh_runtime_type (tree type)
{
  return add_objc_string (OBJC_TYPE_NAME (TREE_TYPE (type)), class_names);
}
#endif

/* Initialize exception handling.  */

static void
objc_init_exceptions (void)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  /* APPLE LOCAL begin radar 4590191 */
  if (flag_objc_sjlj_exceptions)
    {
      /* LLVM LOCAL begin */    
#ifdef ENABLE_LLVM
      OBJC_FLAG_SJLJ_EXCEPTIONS;
#else
      if (darwin_macosx_version_min 
	  && strverscmp (darwin_macosx_version_min, "10.3") < 0)
	warning (0, "Mac OS X version 10.3 or later is needed instead of %s for objc/obj-c++ exceptions",
	         darwin_macosx_version_min);
#endif
      /* LLVM LOCAL end */
    }
  /* APPLE LOCAL end radar 4590191 */
 /* APPLE LOCAL begin radar 2848255 */
  else if (flag_objc_zerocost_exceptions)
    {
#ifndef OBJCPLUS
      c_eh_initialized_p = true;
#endif
      /* APPPLE LOCAL radar 5023725 */
      OBJC_FLAG_ZEROCOST_EXCEPTIONS; 
      /* LLVM local begin */
#ifdef ENABLE_LLVM
      llvm_eh_personality_libfunc
        = llvm_init_one_libfunc ("__objc_personality_v0");
#else
      eh_personality_libfunc
        = init_one_libfunc ("__objc_personality_v0");
#endif
      /* LLVM local end */
      default_init_unwind_resume_libfunc ();
      using_eh_for_cleanups ();
      lang_eh_runtime_type = objc2_eh_runtime_type;
      /* APPLE LOCAL radar 4951324 */
      init_UOBJC2_EHTYPE_decls ();
    }
  /* APPLE LOCAL end radar 2848255 */
#ifndef OBJCPLUS
  else
    {
      c_eh_initialized_p = true;
      /* LLVM local begin */
#ifdef ENABLE_LLVM
      llvm_eh_personality_libfunc
	= llvm_init_one_libfunc (USING_SJLJ_EXCEPTIONS
                                 ? "__gnu_objc_personality_sj0"
                                 : "__gnu_objc_personality_v0");
#else
      eh_personality_libfunc
	= init_one_libfunc (USING_SJLJ_EXCEPTIONS
			    ? "__gnu_objc_personality_sj0"
			    : "__gnu_objc_personality_v0");
#endif
      /* LLVM local end */
      default_init_unwind_resume_libfunc ();
      using_eh_for_cleanups ();
      lang_eh_runtime_type = objc_eh_runtime_type;
    }
#endif

  /* LLVM LOCAL begin 7069718 */
#ifdef ENABLE_LLVM
  if (flag_objc_zerocost_exceptions)
    lang_eh_catch_all = return_null_tree;
#endif
  /* LLVM LOCAL end 7069718 */
}

/* Build an EXC_PTR_EXPR, or the moral equivalent.  In the case of Darwin,
   we'll arrange for it to be initialized (and associated with a binding)
   later.  */

static tree
objc_build_exc_ptr (void)
{
  if (flag_objc_sjlj_exceptions)
    {
      tree var = cur_try_context->caught_decl;
      if (!var)
	{
	  var = objc_create_temporary_var (objc_object_type);
	  cur_try_context->caught_decl = var;
	}
      return var;
    }
  else
    return build0 (EXC_PTR_EXPR, objc_object_type);
}

/* Build "objc_exception_try_exit(&_stack)".  */

static tree
next_sjlj_build_try_exit (void)
{
  tree t;
  t = build_fold_addr_expr (cur_try_context->stack_decl);
  t = tree_cons (NULL, t, NULL);
  t = build_function_call (objc_exception_try_exit_decl, t);
  return t;
}

/* Build
	objc_exception_try_enter (&_stack);
	if (_setjmp(&_stack.buf))
	  ;
	else
	  ;
   Return the COND_EXPR.  Note that the THEN and ELSE fields are left
   empty, ready for the caller to fill them in.  */

static tree
next_sjlj_build_enter_and_setjmp (void)
{
  tree t, enter, sj, cond;

  t = build_fold_addr_expr (cur_try_context->stack_decl);
  t = tree_cons (NULL, t, NULL);
  enter = build_function_call (objc_exception_try_enter_decl, t);

  t = objc_build_component_ref (cur_try_context->stack_decl,
				get_identifier ("buf"));
  t = build_fold_addr_expr (t);
#ifdef OBJCPLUS
  /* Convert _setjmp argument to type that is expected.  */
  if (TYPE_ARG_TYPES (TREE_TYPE (objc_setjmp_decl)))
    t = convert (TREE_VALUE (TYPE_ARG_TYPES (TREE_TYPE (objc_setjmp_decl))), t);
  else
    t = convert (ptr_type_node, t);
#else
  t = convert (ptr_type_node, t);
#endif
  t = tree_cons (NULL, t, NULL);
  sj = build_function_call (objc_setjmp_decl, t);

  cond = build2 (COMPOUND_EXPR, TREE_TYPE (sj), enter, sj);
  cond = c_common_truthvalue_conversion (cond);

  return build3 (COND_EXPR, void_type_node, cond, NULL, NULL);
}

/* Build:

   DECL = objc_exception_extract(&_stack);  */

static tree
next_sjlj_build_exc_extract (tree decl)
{
  tree t;

  t = build_fold_addr_expr (cur_try_context->stack_decl);
  t = tree_cons (NULL, t, NULL);
  t = build_function_call (objc_exception_extract_decl, t);
  t = convert (TREE_TYPE (decl), t);
  t = build2 (MODIFY_EXPR, void_type_node, decl, t);

  return t;
}

/* Build
	if (objc_exception_match(obj_get_class(TYPE), _caught)
	  BODY
	else if (...)
	  ...
	else
	  {
	    _rethrow = _caught;
	    objc_exception_try_exit(&_stack);
	  }
   from the sequence of CATCH_EXPRs in the current try context.  */

static tree
next_sjlj_build_catch_list (void)
{
  tree_stmt_iterator i = tsi_start (cur_try_context->catch_list);
  tree catch_seq, t;
  tree *last = &catch_seq;
  bool saw_id = false;

  for (; !tsi_end_p (i); tsi_next (&i))
    {
      tree stmt = tsi_stmt (i);
      tree type = CATCH_TYPES (stmt);
      tree body = CATCH_BODY (stmt);

      if (type == NULL)
	{
	  *last = body;
	  saw_id = true;
	  break;
	}
      else
	{
	  tree args, cond;

	  if (type == error_mark_node)
	    cond = error_mark_node;
	  else
	    {
	      args = tree_cons (NULL, cur_try_context->caught_decl, NULL);
	      t = objc_get_class_reference (OBJC_TYPE_NAME (TREE_TYPE (type)));
	      args = tree_cons (NULL, t, args);
	      t = build_function_call (objc_exception_match_decl, args);
	      cond = c_common_truthvalue_conversion (t);
	    }
	  t = build3 (COND_EXPR, void_type_node, cond, body, NULL);
	  SET_EXPR_LOCUS (t, EXPR_LOCUS (stmt));

	  *last = t;
	  last = &COND_EXPR_ELSE (t);
	}
    }

  if (!saw_id)
    {
      t = build2 (MODIFY_EXPR, void_type_node, cur_try_context->rethrow_decl,
		  cur_try_context->caught_decl);
      SET_EXPR_LOCATION (t, cur_try_context->end_catch_locus);
      append_to_statement_list (t, last);

      t = next_sjlj_build_try_exit ();
      SET_EXPR_LOCATION (t, cur_try_context->end_catch_locus);
      append_to_statement_list (t, last);
    }

  return catch_seq;
}

/* Build a complete @try-@catch-@finally block for legacy Darwin setjmp
   exception handling.  We aim to build:

	{
	  struct _objc_exception_data _stack;
	  id _rethrow = 0;
	  try
	    {
	      objc_exception_try_enter (&_stack);
	      if (_setjmp(&_stack.buf))
	        {
		  id _caught = objc_exception_extract(&_stack);
		  objc_exception_try_enter (&_stack);
		  if (_setjmp(&_stack.buf))
		    _rethrow = objc_exception_extract(&_stack);
		  else
		    CATCH-LIST
	        }
	      else
		TRY-BLOCK
	    }
	  finally
	    {
	      if (!_rethrow)
		objc_exception_try_exit(&_stack);
	      FINALLY-BLOCK
	      if (_rethrow)
		objc_exception_throw(_rethrow);
	    }
	}

   If CATCH-LIST is empty, we can omit all of the block containing
   "_caught" except for the setting of _rethrow.  Note the use of
   a real TRY_FINALLY_EXPR here, which is not involved in EH per-se,
   but handles goto and other exits from the block.  */

static tree
next_sjlj_build_try_catch_finally (void)
{
  tree rethrow_decl, stack_decl, t;
  tree catch_seq, try_fin, bind;

  /* Create the declarations involved.  */
  t = xref_tag (RECORD_TYPE, get_identifier (UTAG_EXCDATA));
  stack_decl = objc_create_temporary_var (t);
  cur_try_context->stack_decl = stack_decl;

  rethrow_decl = objc_create_temporary_var (objc_object_type);
  cur_try_context->rethrow_decl = rethrow_decl;
  TREE_CHAIN (rethrow_decl) = stack_decl;

  /* Build the outermost variable binding level.  */
  bind = build3 (BIND_EXPR, void_type_node, rethrow_decl, NULL, NULL);
  SET_EXPR_LOCATION (bind, cur_try_context->try_locus);
  TREE_SIDE_EFFECTS (bind) = 1;

  /* Initialize rethrow_decl.  */
  t = build2 (MODIFY_EXPR, void_type_node, rethrow_decl,
	      convert (objc_object_type, null_pointer_node));
  SET_EXPR_LOCATION (t, cur_try_context->try_locus);
  append_to_statement_list (t, &BIND_EXPR_BODY (bind));

  /* Build the outermost TRY_FINALLY_EXPR.  */
  try_fin = build2 (TRY_FINALLY_EXPR, void_type_node, NULL, NULL);
  SET_EXPR_LOCATION (try_fin, cur_try_context->try_locus);
  TREE_SIDE_EFFECTS (try_fin) = 1;
  append_to_statement_list (try_fin, &BIND_EXPR_BODY (bind));

  /* Create the complete catch sequence.  */
  if (cur_try_context->catch_list)
    {
      tree caught_decl = objc_build_exc_ptr ();
      catch_seq = build_stmt (BIND_EXPR, caught_decl, NULL, NULL);
      TREE_SIDE_EFFECTS (catch_seq) = 1;

      t = next_sjlj_build_exc_extract (caught_decl);
      append_to_statement_list (t, &BIND_EXPR_BODY (catch_seq));

      t = next_sjlj_build_enter_and_setjmp ();
      COND_EXPR_THEN (t) = next_sjlj_build_exc_extract (rethrow_decl);
      COND_EXPR_ELSE (t) = next_sjlj_build_catch_list ();
      append_to_statement_list (t, &BIND_EXPR_BODY (catch_seq));
    }
  else
    catch_seq = next_sjlj_build_exc_extract (rethrow_decl);
  SET_EXPR_LOCATION (catch_seq, cur_try_context->end_try_locus);

  /* Build the main register-and-try if statement.  */
  t = next_sjlj_build_enter_and_setjmp ();
  SET_EXPR_LOCATION (t, cur_try_context->try_locus);
  COND_EXPR_THEN (t) = catch_seq;
  COND_EXPR_ELSE (t) = cur_try_context->try_body;
  TREE_OPERAND (try_fin, 0) = t;

  /* Build the complete FINALLY statement list.  */
  t = next_sjlj_build_try_exit ();
  t = build_stmt (COND_EXPR,
		  c_common_truthvalue_conversion (rethrow_decl),
		  NULL, t);
  SET_EXPR_LOCATION (t, cur_try_context->finally_locus);
  append_to_statement_list (t, &TREE_OPERAND (try_fin, 1));

  append_to_statement_list (cur_try_context->finally_body,
			    &TREE_OPERAND (try_fin, 1));

  t = tree_cons (NULL, rethrow_decl, NULL);
  t = build_function_call (objc_exception_throw_decl, t);
  t = build_stmt (COND_EXPR,
		  c_common_truthvalue_conversion (rethrow_decl),
		  t, NULL);
  SET_EXPR_LOCATION (t, cur_try_context->end_finally_locus);
  append_to_statement_list (t, &TREE_OPERAND (try_fin, 1));

  return bind;
}

/* Called just after parsing the @try and its associated BODY.  We now
   must prepare for the tricky bits -- handling the catches and finally.  */

void
objc_begin_try_stmt (location_t try_locus, tree body)
{
  struct objc_try_context *c = xcalloc (1, sizeof (*c));
  c->outer = cur_try_context;
  c->try_body = body;
  c->try_locus = try_locus;
  c->end_try_locus = input_location;
  cur_try_context = c;

  objc_init_exceptions ();

  if (flag_objc_sjlj_exceptions)
    objc_mark_locals_volatile (NULL);
}

/* Called just after parsing "@catch (parm)".  Open a binding level,
   enter DECL into the binding level, and initialize it.  Leave the
   binding level open while the body of the compound statement is parsed.  */

void
objc_begin_catch_clause (tree decl)
{
  tree compound, type, t;

  /* Begin a new scope that the entire catch clause will live in.  */
  compound = c_begin_compound_stmt (true);

  /* APPLE LOCAL begin radar 2848255 */
  if (decl == NULL_TREE)
    {
      /* @catch (...) */
      if (flag_objc_zerocost_exceptions)
        {
          t = build_stmt (CATCH_EXPR, NULL_TREE, compound);
          cur_try_context->current_catch = t;
          /* APPLE LOCAL 5837617 */
          t = build1(NOP_EXPR, ptr_type_node, objc_build_exc_ptr ());
          t = build_function_call (objc2_begin_catch_decl,
                                   tree_cons (NULL_TREE, t, NULL_TREE));
          add_stmt (t);
          return;
        }
      else
        /* APPLE LOCAL begin radar 4995967 */
        /* Assumes @catch (...) is same as @catch (id). */
          decl = create_tmp_var_raw (objc_object_type, NULL);
        /* APPLE LOCAL end radar 4995967 */
    }
  /* APPLE LOCAL end radar 2848255 */

  /* The parser passed in a PARM_DECL, but what we really want is a VAR_DECL.  */
  decl = build_decl (VAR_DECL, DECL_NAME (decl), TREE_TYPE (decl));
  lang_hooks.decls.pushdecl (decl);

  /* Since a decl is required here by syntax, don't warn if its unused.  */
  /* ??? As opposed to __attribute__((unused))?  Anyway, this appears to
     be what the previous objc implementation did.  */
  TREE_USED (decl) = 1;

  /* Verify that the type of the catch is valid.  It must be a pointer
     to an Objective-C class, or "id" (which is catch-all).  */
  type = TREE_TYPE (decl);

  if (POINTER_TYPE_P (type) && objc_is_object_id (TREE_TYPE (type)))
    /* APPLE LOCAL radar 2848255 */
    type = !flag_objc_zerocost_exceptions ? NULL : type;
  else if (!POINTER_TYPE_P (type) || !TYPED_OBJECT (TREE_TYPE (type)))
    {
      error ("@catch parameter is not a known Objective-C class type");
      type = error_mark_node;
    }
  else if (cur_try_context->catch_list)
    {
      /* Examine previous @catch clauses and see if we've already
	 caught the type in question.  */
      tree_stmt_iterator i = tsi_start (cur_try_context->catch_list);
      for (; !tsi_end_p (i); tsi_next (&i))
	{
	  tree stmt = tsi_stmt (i);
	  t = CATCH_TYPES (stmt);
	  if (t == error_mark_node)
	    continue;
	  if (!t || DERIVED_FROM_P (TREE_TYPE (t), TREE_TYPE (type)))
	    {
	      warning (0, "exception of type %<%T%> will be caught",
		       TREE_TYPE (type));
	      warning (0, "%H   by earlier handler for %<%T%>",
		       EXPR_LOCUS (stmt), TREE_TYPE (t ? t : objc_object_type));
	      break;
	    }
	}
    }

  /* Record the data for the catch in the try context so that we can
     finalize it later.  */
  t = build_stmt (CATCH_EXPR, type, compound);
  cur_try_context->current_catch = t;

  /* Initialize the decl from the EXC_PTR_EXPR we get from the runtime.  */
  t = objc_build_exc_ptr ();
  t = convert (TREE_TYPE (decl), t);
  /* APPLE LOCAL begin radar 2848255 - radar 4951324 */
  if (flag_objc_zerocost_exceptions)
    {     
      /* Decl an external declaration for the objc class for this catch type. */
      objc2_build_extern_decl_catch_object (type);
      /* APPLE LOCAL 5837617 */
      t = build1(NOP_EXPR, ptr_type_node, t);
      t = build_function_call (objc2_begin_catch_decl,
                               tree_cons (NULL_TREE, t, NULL_TREE));
    }
  /* APPLE LOCAL end radar 2848255 - radar 4951324 */
  t = build2 (MODIFY_EXPR, void_type_node, decl, t);
  add_stmt (t);
}

/* APPLE LOCAL begin radar 2848255 */
/* This routine adds a finally { objc_end_catch (); } block to end of try-block. */
static tree
objc2_liteweight_finish_try_stmt (void)
{    
  struct objc_try_context *c = cur_try_context;
  tree stmt, t;
  
  stmt = c->try_body;
  t = build_function_call (objc2_end_catch_decl, NULL_TREE);
  append_to_statement_list (t, &c->finally_body);
  stmt = build_stmt (TRY_FINALLY_EXPR, stmt, c->finally_body);
  SET_EXPR_LOCATION (stmt, c->try_locus);
      
  cur_try_context = c->outer;
  free (c);
  return stmt;
}
/* APPLE LOCAL end radar 2848255 */

/* Called just after parsing the closing brace of a @catch clause.  Close
   the open binding level, and record a CATCH_EXPR for it.  */

void
objc_finish_catch_clause (void)
{
  tree c = cur_try_context->current_catch;
  cur_try_context->current_catch = NULL;
  cur_try_context->end_catch_locus = input_location;

  CATCH_BODY (c) = c_end_compound_stmt (CATCH_BODY (c), 1);
  /* APPLE LOCAL begin radar 2848255 */
  if (flag_objc_zerocost_exceptions)
    {
      /* try { catch-body } finally { objc_end_catch (); } */
      tree try_exp, t;
      t = CATCH_BODY (c);
      if (TREE_CODE (t) == BIND_EXPR)
        {
          /* more common case of @catch (objc-expr). */
          objc_begin_try_stmt (cur_try_context->try_locus, BIND_EXPR_BODY (t));
          try_exp = objc2_liteweight_finish_try_stmt ();
          BIND_EXPR_BODY (t) = NULL_TREE;
          append_to_statement_list (try_exp, &BIND_EXPR_BODY (t));
        }
      else
        {
          /* case of the new @catch (...) */
          objc_begin_try_stmt (cur_try_context->try_locus, t);
          try_exp = objc2_liteweight_finish_try_stmt ();
          CATCH_BODY (c) = NULL_TREE;
          append_to_statement_list (try_exp, &CATCH_BODY (c));
        }
    }
  /* APPLE LOCAL end radar 2848255 */
  append_to_statement_list (c, &cur_try_context->catch_list);
}

/* Called after parsing a @finally clause and its associated BODY.
   Record the body for later placement.  */

void
objc_build_finally_clause (location_t finally_locus, tree body)
{
  cur_try_context->finally_body = body;
  cur_try_context->finally_locus = finally_locus;
  cur_try_context->end_finally_locus = input_location;
}

/* Called to finalize a @try construct.  */

tree
objc_finish_try_stmt (void)
{
  struct objc_try_context *c = cur_try_context;
  tree stmt;

  if (c->catch_list == NULL && c->finally_body == NULL)
    error ("%<@try%> without %<@catch%> or %<@finally%>");

  /* If we're doing Darwin setjmp exceptions, build the big nasty.  */
  if (flag_objc_sjlj_exceptions)
    {
      if (!cur_try_context->finally_body)
	{
	  cur_try_context->finally_locus = input_location;
	  cur_try_context->end_finally_locus = input_location;
	}
      stmt = next_sjlj_build_try_catch_finally ();
    }
  else
    {
      /* Otherwise, nest the CATCH inside a FINALLY.  */
      stmt = c->try_body;
      if (c->catch_list)
	{
          stmt = build_stmt (TRY_CATCH_EXPR, stmt, c->catch_list);
	  SET_EXPR_LOCATION (stmt, cur_try_context->try_locus);
	}
      if (c->finally_body)
	{
	  stmt = build_stmt (TRY_FINALLY_EXPR, stmt, c->finally_body);
	  SET_EXPR_LOCATION (stmt, cur_try_context->try_locus);
	}
    }
  add_stmt (stmt);

  cur_try_context = c->outer;
  free (c);
  return stmt;
}

/* APPLE LOCAL begin radar 2848255 */
tree
objc2_build_throw_call (tree throw_expr)
{
  tree args;

  objc_init_exceptions ();

  if (throw_expr == NULL)
    {
      /* If we're not inside a @catch block, there is no "current
         exception" to be rethrown.  */
      if (cur_try_context == NULL
          || cur_try_context->current_catch == NULL)
        {
          error ("%<@throw%> (rethrow) used outside of a @catch block");
          return error_mark_node;
        }
      /* APPLE LOCAL begin radar 4957534 */
      if (flag_objc_zerocost_exceptions)
        return build_function_call (objc_rethrow_exception_decl, NULL_TREE);
      /* APPLE LOCAL end radar 4957534 */
      /* Otherwise the object is still sitting in the EXC_PTR_EXPR
         value that we get from the runtime.  */
      throw_expr = objc_build_exc_ptr ();
    }

  /* A throw is just a call to the runtime throw function with the
     object as a parameter.  */
  args = tree_cons (NULL, throw_expr, NULL);
  return build_function_call (objc_exception_throw_decl, args);
}

tree objc_build_throw_stmt (tree throw_expr)
{
  return add_stmt (objc2_build_throw_call (throw_expr));
}
/* APPLE LOCAL end radar 2848255 */

tree
objc_build_synchronized (location_t start_locus, tree mutex, tree body)
{
  tree args, call;

  /* First lock the mutex.  */
  mutex = save_expr (mutex);
  args = tree_cons (NULL, mutex, NULL);
  call = build_function_call (objc_sync_enter_decl, args);
  SET_EXPR_LOCATION (call, start_locus);
  add_stmt (call);

  /* Build the mutex unlock.  */
  args = tree_cons (NULL, mutex, NULL);
  call = build_function_call (objc_sync_exit_decl, args);
  SET_EXPR_LOCATION (call, input_location);

  /* Put the that and the body in a TRY_FINALLY.  */
  objc_begin_try_stmt (start_locus, body);
  objc_build_finally_clause (input_location, call);
  return objc_finish_try_stmt ();
}


/* Predefine the following data type:

   struct _objc_exception_data
   {
     int buf[OBJC_JBLEN];
     void *pointers[4];
   }; */

/* The following yuckiness should prevent users from having to #include
   <setjmp.h> in their code... */

/* Define to a harmless positive value so the below code doesn't die.  */
#ifndef OBJC_JBLEN
#define OBJC_JBLEN 18
#endif

static void
build_next_objc_exception_stuff (void)
{
  tree field_decl, field_decl_chain, index, temp_type;

  objc_exception_data_template
    = start_struct (RECORD_TYPE, get_identifier (UTAG_EXCDATA));

  /* int buf[OBJC_JBLEN]; */

  index = build_index_type (build_int_cst (NULL_TREE, OBJC_JBLEN - 1));
  field_decl = create_field_decl (build_array_type (integer_type_node, index),
				  "buf");
  field_decl_chain = field_decl;

  /* void *pointers[4]; */

  index = build_index_type (build_int_cst (NULL_TREE, 4 - 1));
  field_decl = create_field_decl (build_array_type (ptr_type_node, index),
				  "pointers");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_exception_data_template, field_decl_chain, NULL_TREE);

  /* int _setjmp(...); */
  /* If the user includes <setjmp.h>, this shall be superseded by
     'int _setjmp(jmp_buf);' */
  temp_type = build_function_type (integer_type_node, NULL_TREE);
  objc_setjmp_decl
    = builtin_function (TAG_SETJMP, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);

  /* id objc_exception_extract(struct _objc_exception_data *); */
  temp_type
    = build_function_type (objc_object_type,
			   tree_cons (NULL_TREE,
				      build_pointer_type (objc_exception_data_template),
				      OBJC_VOID_AT_END));
  objc_exception_extract_decl
    = builtin_function (TAG_EXCEPTIONEXTRACT, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);
  /* void objc_exception_try_enter(struct _objc_exception_data *); */
  /* void objc_exception_try_exit(struct _objc_exception_data *); */
  temp_type
    = build_function_type (void_type_node,
			   tree_cons (NULL_TREE,
				      build_pointer_type (objc_exception_data_template),
				      OBJC_VOID_AT_END));
  objc_exception_try_enter_decl
    = builtin_function (TAG_EXCEPTIONTRYENTER, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);
  objc_exception_try_exit_decl
    = builtin_function (TAG_EXCEPTIONTRYEXIT, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);

  /* int objc_exception_match(id, id); */
  temp_type
    = build_function_type (integer_type_node,
			   tree_cons (NULL_TREE, objc_object_type,
				      tree_cons (NULL_TREE, objc_object_type,
						 OBJC_VOID_AT_END)));
  objc_exception_match_decl
    = builtin_function (TAG_EXCEPTIONMATCH, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);

  /* APPLE LOCAL objc gc 5656857 */
  /* id objc_assign_ivar (id, id, ptrdiff_t); */
  /* APPLE LOCAL objc gc 5656857 */
  /* id objc_assign_ivar_Fast (id, id, ptrdiff_t)
       __attribute__ ((hard_coded_address (OFFS_ASSIGNIVAR_FAST))); */
  temp_type
    = build_function_type (objc_object_type,
			   tree_cons
			   (NULL_TREE, objc_object_type,
			    tree_cons (NULL_TREE, objc_object_type,
				       tree_cons (NULL_TREE,
						  /* APPLE LOCAL objc gc 5656857 */
						  ptrdiff_type_node,
						  OBJC_VOID_AT_END))));
  /* APPLE LOCAL begin radar 4590221 */
  if (OFFS_ASSIGNIVAR_FAST)
    {
      /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
      tree objc_assign_ivar_fast_decl;
      objc_assign_ivar_decl
	= builtin_function (TAG_ASSIGNIVAR_FAST, temp_type, 0, 
			    NOT_BUILT_IN, NULL, NULL_TREE);
      objc_assign_ivar_fast_decl = build_int_cst (NULL_TREE, OFFS_ASSIGNIVAR_FAST);
      objc_assign_ivar_fast_decl
        = build_c_cast (build_pointer_type (TREE_TYPE (objc_assign_ivar_decl)),
                        objc_assign_ivar_fast_decl);
      objc_assign_ivar_fast_decl
        = build_indirect_ref (objc_assign_ivar_fast_decl, "*");
      objc_assign_ivar_decl = objc_assign_ivar_fast_decl;
#else
      objc_assign_ivar_decl
	= builtin_function (TAG_ASSIGNIVAR_FAST, temp_type, 0, 
			    NOT_BUILT_IN, NULL, NULL_TREE);
      DECL_ATTRIBUTES (objc_assign_ivar_decl)
	= tree_cons (get_identifier ("hard_coded_address"),
		     build_int_cst (NULL_TREE, OFFS_ASSIGNIVAR_FAST),
		     NULL_TREE);
#endif
      /* LLVM LOCAL end */
    }
  else
    {
      objc_assign_ivar_decl
	= builtin_function (TAG_ASSIGNIVAR, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);
    }
  /* APPLE LOCAL end radar 4590221 */
  /* id objc_assign_global (id, id *); */
  /* id objc_assign_strongCast (id, id *); */
  temp_type = build_function_type (objc_object_type,
		tree_cons (NULL_TREE, objc_object_type,
		    tree_cons (NULL_TREE, build_pointer_type (objc_object_type),
			OBJC_VOID_AT_END)));
  objc_assign_global_decl
	= builtin_function (TAG_ASSIGNGLOBAL, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);
  objc_assign_strong_cast_decl
	= builtin_function (TAG_ASSIGNSTRONGCAST, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);
  /* APPLE LOCAL begin radar 4426814 */
  /* id objc_assign_weak (id, id *); */
  objc_assign_weak_decl
	= builtin_function (TAG_ASSIGN_WEAK, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);
  /* APPLE LOCAL radar 4591756 */
  /* id objc_read_weak (id *); */
  temp_type = build_function_type (objc_object_type, 
				   tree_cons (NULL_TREE, 
					      build_pointer_type (objc_object_type), 
					      OBJC_VOID_AT_END));
  /* APPLE LOCAL radar 4591756 */
  objc_read_weak_decl
	= builtin_function (TAG_READ_WEAK, temp_type, 0, NOT_BUILT_IN, NULL, NULL_TREE);
  /* APPLE LOCAL end radar 4426814 */
  /* APPLE LOCAL begin radar 3742561 */
  /* void *objc_memmove_collectable(void *dst, const void *src, size_t size) */
  temp_type = build_function_type (ptr_type_node,
				     tree_cons (NULL_TREE, ptr_type_node,
				       tree_cons (NULL_TREE, ptr_type_node,
				         tree_cons (NULL_TREE, long_unsigned_type_node, 
						    OBJC_VOID_AT_END))));
  objc_memmove_collectable_decl
	= builtin_function (TAG_MEMMOVE_COLLECTABLE, temp_type, 0, 
			    NOT_BUILT_IN, NULL, NULL_TREE);
  /* APPLE LOCAL end radar 3742561 */
}

static void
build_objc_exception_stuff (void)
{
  tree noreturn_list, nothrow_list, temp_type;

  noreturn_list = tree_cons (get_identifier ("noreturn"), NULL, NULL);
  nothrow_list = tree_cons (get_identifier ("nothrow"), NULL, NULL);

  /* void objc_exception_throw(id) __attribute__((noreturn)); */
  /* void objc_sync_enter(id); */
  /* void objc_sync_exit(id); */
  temp_type = build_function_type (void_type_node,
				   tree_cons (NULL_TREE, objc_object_type,
					      OBJC_VOID_AT_END));
  objc_exception_throw_decl
    = builtin_function (TAG_EXCEPTIONTHROW, temp_type, 0, NOT_BUILT_IN, NULL,
			noreturn_list);
  objc_sync_enter_decl
    = builtin_function (TAG_SYNCENTER, temp_type, 0, NOT_BUILT_IN,
			NULL, nothrow_list);
  objc_sync_exit_decl
    = builtin_function (TAG_SYNCEXIT, temp_type, 0, NOT_BUILT_IN,
			NULL, nothrow_list);
  /* APPLE LOCAL begin radar 2848255 */
  if (flag_objc_zerocost_exceptions)
    {
      /* void * objc_begin_catch (void *) */
      tree type
        = build_function_type (ptr_type_node,
                           tree_cons (NULL_TREE, ptr_type_node,
                           OBJC_VOID_AT_END));
      objc2_begin_catch_decl = builtin_function ("objc_begin_catch",
                                                 type, 0, NOT_BUILT_IN,
                                                 NULL, NULL_TREE);
      TREE_NOTHROW (objc2_begin_catch_decl) = 0;

      /* void objc_end_catch () */
      type
        = build_function_type (void_type_node, OBJC_VOID_AT_END);
      objc2_end_catch_decl = builtin_function ("objc_end_catch",
                                               type, 0, NOT_BUILT_IN,
                                               NULL, NULL_TREE);
      TREE_NOTHROW (objc2_end_catch_decl) = 0;

      /* APPLE LOCAL begin radar 4957534 */
      /* void objc_exception_rethrow (void) */
      type
        = build_function_type (void_type_node, OBJC_VOID_AT_END);
      objc_rethrow_exception_decl = builtin_function ("objc_exception_rethrow",
                                               type, 0, NOT_BUILT_IN,
                                               NULL, NULL_TREE);
      TREE_NOTHROW (objc_rethrow_exception_decl) = 0;
      /* APPLE LOCAL end radar 4957534 */

      /* APPLE LOCAL begin radar 5060975 */
      /* Make sure that objc_exception_throw (id) claims that it may throw an exception. */
      TREE_NOTHROW (objc_exception_throw_decl) = 0;
      /* APPLE LOCAL end radar 5060975 */

      /* build struct _objc_ehtype_t type */
      objc2_build_ehtype ();
    }
  /* APPLE LOCAL end radar 2848255 */
}

/* Construct a C struct corresponding to ObjC class CLASS, with the same
   name as the class:

   struct <classname> {
     struct _objc_class *isa;
     ...
   };  */

static void
build_private_template (tree class)
{
  if (!CLASS_STATIC_TEMPLATE (class))
    {
      tree record = objc_build_struct (class,
				       get_class_ivars (class, false),
				       CLASS_SUPER_NAME (class));

      /* Set the TREE_USED bit for this struct, so that stab generator
	 can emit stabs for this struct type.  */
      /* APPLE LOCAL begin radar 4666559 */
      if (flag_debug_only_used_symbols && !objc_collecting_ivars 
	  && TYPE_STUB_DECL (record))
      /* APPLE LOCAL end radar 4666559 */
	TREE_USED (TYPE_STUB_DECL (record)) = 1;

      /* APPLE LOCAL begin radar 4548636 */
      if (TREE_CODE (class) == CLASS_INTERFACE_TYPE)
	objc_attach_attrs_to_type (record, class); 
      /* APPLE LOCAL end radar 4548636 */
      /* APPLE LOCAL begin ObjC new abi */
      if (flag_objc_abi == 2)
        CLASS_TYPE (class) = record;
    }
}
      /* APPLE LOCAL end ObjC new abi */

/* APPLE LOCAL begin ObjC new abi */
/* struct protocol_t {
     // APPLE LOCAL radar 4695109
     id isa;  // NULL
     const char * const protocol_name;
     const struct protocol_list_t * const protocol_list;
     const struct method_list_t * const instance_methods;
     const struct method_list_t * const class_methods;
     // APPLE LOCAL begin radar 4695109 
     const struct method_list_t *optionalInstanceMethods;
     const struct method_list_t *optionalClassMethods;
     // APPLE LOCAL end radar 4695109 
     const struct _prop_list_t * const properties;
     // APPLE LOCAL begin radar 5192466
     const uint32_t size;  // sizeof(struct protocol_t)
     const uint32_t flags;  // = 0
     // APPLE LOCAL end radar 5192466
   }  */

static void
build_v2_protocol_template (void)
{
  tree field_decl, field_decl_chain;

  objc_v2_protocol_template = start_struct (RECORD_TYPE,
                                         get_identifier (UTAG_V2_PROTOCOL));

  /* APPLE LOCAL begin radar 4533974 - ObjC new protocol - radar 4695109 */
  /* id isa; */
  field_decl = create_field_decl (objc_object_type, "isa");
  field_decl_chain = field_decl;
  /* APPLE LOCAL end radar 4533974 - ObjC new protocol - radar 4695109 */

  /* char *protocol_name; */ 
  field_decl = create_field_decl (string_type_node, "protocol_name");
  /* APPLE LOCAL radar 4533974 - ObjC new protocol */
  chainon (field_decl_chain, field_decl);

  /* const struct protocol_list_t * const protocol_list; */
  field_decl = create_field_decl (build_pointer_type
                                   (objc_v2_protocol_template),
                                  "protocol_list");
  chainon (field_decl_chain, field_decl);

  /* const struct method_list_t * const instance_methods; */
  field_decl = create_field_decl (objc_method_proto_list_ptr,
				  "instance_methods");
  chainon (field_decl_chain, field_decl);

  /* const struct method_list_t * const class_methods; */
  field_decl = create_field_decl (objc_method_proto_list_ptr,
                                  "class_methods");
  chainon (field_decl_chain, field_decl);

  /* APPLE LOCAL begin radar 4695109 */
  /* const struct method_list_t * optionalInstanceMethods */
  field_decl = create_field_decl (objc_method_proto_list_ptr,
                                  "optionalInstanceMethods");
  chainon (field_decl_chain, field_decl);

  /* const struct method_list_t * optionalClassMethods */
  field_decl = create_field_decl (objc_method_proto_list_ptr,
                                  "optionalClassMethods");
  chainon (field_decl_chain, field_decl);
  /* APPLE LOCAL end radar 4695109 */

  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
  /* struct _prop_list_t * properties; */
  field_decl = create_field_decl (objc_prop_list_ptr,
				  "properties");
  chainon (field_decl_chain, field_decl);
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
  /* APPLE LOCAL begin radar 5192466 */
  /* const uint32_t size;  */
  field_decl = create_field_decl (integer_type_node, "size");
  chainon (field_decl_chain, field_decl);
  /* const uint32_t flags;  */
  field_decl = create_field_decl (integer_type_node, "flags");
  chainon (field_decl_chain, field_decl);
  /* APPLE LOCAL end radar 5192466 */

  finish_struct (objc_v2_protocol_template, field_decl_chain, NULL_TREE);


}

/* This routine declares all variables used to declare protocol references.  */

static void
/* APPLE LOCAL radar 4695109 */
/* Any reference to protocol_impl is removed */
generate_v2_protocol_references (tree plist)
{
  tree lproto;

  /* Forward declare protocols referenced.  */
  for (lproto = plist; lproto; lproto = TREE_CHAIN (lproto))
    {
      tree proto = TREE_VALUE (lproto);

      if (TREE_CODE (proto) == PROTOCOL_INTERFACE_TYPE
	  && PROTOCOL_NAME (proto))
	{
          if (! PROTOCOL_V2_FORWARD_DECL (proto))
            build_v2_protocol_reference (proto);

          if (PROTOCOL_LIST (proto))
            generate_v2_protocol_references (PROTOCOL_LIST (proto));
        }
    }
}
/* APPLE LOCAL end ObjC new abi */


/* Begin code generation for protocols...  */

/* struct _objc_protocol {
     // APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions
     struct _objc_protocol_extension *isa;
     char *protocol_name;
     struct _objc_protocol **protocol_list;
     struct _objc__method_prototype_list *instance_methods;
     struct _objc__method_prototype_list *class_methods;
   };  */

static void
build_protocol_template (void)
{
  tree field_decl, field_decl_chain;

  objc_protocol_template = start_struct (RECORD_TYPE,
					 get_identifier (UTAG_PROTOCOL));

  /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
  /* struct _objc_protocol_extension *isa; */
  field_decl = create_field_decl (build_pointer_type
				  (xref_tag (RECORD_TYPE,
					     get_identifier (UTAG_PROTOCOL_EXT))),
				  "isa");
  /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */
  field_decl_chain = field_decl;

  /* char *protocol_name; */
  field_decl = create_field_decl (string_type_node, "protocol_name");
  chainon (field_decl_chain, field_decl);

  /* struct _objc_protocol **protocol_list; */
  field_decl = create_field_decl (build_pointer_type
				  (build_pointer_type
				   (objc_protocol_template)),
				  "protocol_list");
  chainon (field_decl_chain, field_decl);

  /* struct _objc__method_prototype_list *instance_methods; */
  field_decl = create_field_decl (objc_method_proto_list_ptr,
				  "instance_methods");
  chainon (field_decl_chain, field_decl);

  /* struct _objc__method_prototype_list *class_methods; */
  field_decl = create_field_decl (objc_method_proto_list_ptr,
				  "class_methods");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_protocol_template, field_decl_chain, NULL_TREE);
}

static tree
build_descriptor_table_initializer (tree type, tree entries)
{
  tree initlist = NULL_TREE;

  do
    {
      tree eltlist = NULL_TREE;

      eltlist
	= tree_cons (NULL_TREE,
		     build_selector (METHOD_SEL_NAME (entries)), NULL_TREE);
      eltlist
	= tree_cons (NULL_TREE,
		     add_objc_string (METHOD_ENCODING (entries),
				      meth_var_types),
		     eltlist);

      initlist
	= tree_cons (NULL_TREE,
		     objc_build_constructor (type, nreverse (eltlist)),
		     initlist);

      entries = TREE_CHAIN (entries);
    }
  while (entries);

  return objc_build_constructor (build_array_type (type, 0),
				 nreverse (initlist));
}

/* struct objc_method_prototype_list {
     int count;
     struct objc_method_prototype {
	SEL name;
	char *types;
     } list[1];
   };  */

static tree
build_method_prototype_list_template (tree list_type, int size)
{
  tree objc_ivar_list_record;
  tree field_decl, field_decl_chain;

  /* Generate an unnamed struct definition.  */

  objc_ivar_list_record = start_struct (RECORD_TYPE, NULL_TREE);

  /* int method_count; */
  field_decl = create_field_decl (integer_type_node, "method_count");
  field_decl_chain = field_decl;

  /* struct objc_method method_list[]; */
  field_decl = create_field_decl (build_array_type
				  (list_type,
				   build_index_type
				   (build_int_cst (NULL_TREE, size - 1))),
				  "method_list");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_ivar_list_record, field_decl_chain, NULL_TREE);

  return objc_ivar_list_record;
}

static tree
build_method_prototype_template (void)
{
  tree proto_record;
  tree field_decl, field_decl_chain;

  proto_record
    = start_struct (RECORD_TYPE, get_identifier (UTAG_METHOD_PROTOTYPE));

  /* SEL _cmd; */
  field_decl = create_field_decl (objc_selector_type, "_cmd");
  field_decl_chain = field_decl;

  /* char *method_types; */
  field_decl = create_field_decl (string_type_node, "method_types");
  chainon (field_decl_chain, field_decl);

  finish_struct (proto_record, field_decl_chain, NULL_TREE);

  return proto_record;
}

static tree
objc_method_parm_type (tree type)
{
  type = TREE_VALUE (TREE_TYPE (type));
  if (TREE_CODE (type) == TYPE_DECL)
    type = TREE_TYPE (type);
  return type;
}

static int
objc_encoded_type_size (tree type)
{
  int sz = int_size_in_bytes (type);

  /* Make all integer and enum types at least as large
     as an int.  */
  if (sz > 0 && INTEGRAL_TYPE_P (type))
    sz = MAX (sz, int_size_in_bytes (integer_type_node));
  /* Treat arrays as pointers, since that's how they're
     passed in.  */
  else if (TREE_CODE (type) == ARRAY_TYPE)
    sz = int_size_in_bytes (ptr_type_node);
  return sz;
}

static tree
encode_method_prototype (tree method_decl)
{
  tree parms;
  int parm_offset, i;
  char buf[40];
  tree result;

  /* ONEWAY and BYCOPY, for remote object are the only method qualifiers.  */
  encode_type_qualifiers (TREE_PURPOSE (TREE_TYPE (method_decl)));

  /* Encode return type.  */
  encode_type (objc_method_parm_type (method_decl),
	       obstack_object_size (&util_obstack),
	       OBJC_ENCODE_INLINE_DEFS);

  /* Stack size.  */
  /* The first two arguments (self and _cmd) are pointers; account for
     their size.  */
  i = int_size_in_bytes (ptr_type_node);
  parm_offset = 2 * i;
  for (parms = METHOD_SEL_ARGS (method_decl); parms;
       parms = TREE_CHAIN (parms))
    {
      tree type = objc_method_parm_type (parms);
      int sz = objc_encoded_type_size (type);

      /* If a type size is not known, bail out.  */
      if (sz < 0)
	{
	  error ("type %q+D does not have a known size",
		 type);
	  /* Pretend that the encoding succeeded; the compilation will
	     fail nevertheless.  */
	  goto finish_encoding;
	}
      parm_offset += sz;
    }

  sprintf (buf, "%d@0:%d", parm_offset, i);
  obstack_grow (&util_obstack, buf, strlen (buf));

  /* Argument types.  */
  parm_offset = 2 * i;
  for (parms = METHOD_SEL_ARGS (method_decl); parms;
       parms = TREE_CHAIN (parms))
    {
      tree type = objc_method_parm_type (parms);

      /* Process argument qualifiers for user supplied arguments.  */
      encode_type_qualifiers (TREE_PURPOSE (TREE_TYPE (parms)));

      /* Type.  */
      encode_type (type, obstack_object_size (&util_obstack),
		   OBJC_ENCODE_INLINE_DEFS);

      /* Compute offset.  */
      sprintf (buf, "%d", parm_offset);
      parm_offset += objc_encoded_type_size (type);

      obstack_grow (&util_obstack, buf, strlen (buf));
    }

  finish_encoding:
  obstack_1grow (&util_obstack, '\0');
  result = get_identifier (obstack_finish (&util_obstack));
  obstack_free (&util_obstack, util_firstobj);
  return result;
}

static tree
generate_descriptor_table (tree type, const char *name, int size, tree list,
			   /* APPLE LOCAL ObjC new abi */
			   tree proto, bool newabi)
{
  tree decl, initlist;
  /* APPLE LOCAL begin radar 6064186 */
  const char* ref_name = synth_id_with_class_suffix (name, proto);
  decl = start_var_decl (type, ref_name);
  /* APPLE LOCAL end radar 6064186 */

  /* APPLE LOCAL begin ObjC new abi */
  if (newabi)
    {
      int entsize;
      /* APPLE LOCAL radar 6064186 */
      set_user_assembler_name (decl, ref_name);
      entsize = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_method_template));
      initlist = build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, entsize));
      initlist = tree_cons (NULL_TREE, build_int_cst (NULL_TREE, size), initlist);
    }
  else
    initlist = build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, size));
  /* APPLE LOCAL end ObjC new abi */
  initlist = tree_cons (NULL_TREE, list, initlist);

  finish_var_decl (decl, objc_build_constructor (type, nreverse (initlist)));

  return decl;
}

/* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
static void
generate_method_descriptors (tree protocol, bool is_optional)
{
  tree initlist, chain, method_list_template;
  int size;

  if (!objc_method_prototype_template)
    objc_method_prototype_template = build_method_prototype_template ();

  chain = is_optional ? PROTOCOL_OPTIONAL_CLS_METHODS (protocol) 
		      : PROTOCOL_CLS_METHODS (protocol);
  if (chain)
    {
      size = list_length (chain);

      method_list_template
	= build_method_prototype_list_template (objc_method_prototype_template,
						size);

      initlist
	= build_descriptor_table_initializer (objc_method_prototype_template,
					      chain);

      if (is_optional)
	UOBJC_PROTOCOL_OPT_CLS_METHODS_decl
	  = generate_descriptor_table (method_list_template,
				       /* APPLE LOCAL radar 4695109 */
				       "_OBJC_PROTOCOL_CLASS_METHODS_OPT",
				       /* APPLE LOCAL ObjC new abi */
				       size, initlist, protocol, false);
      else
        UOBJC_CLASS_METHODS_decl
	  = generate_descriptor_table (method_list_template,
				       "_OBJC_PROTOCOL_CLASS_METHODS",
				       /* APPLE LOCAL ObjC new abi */
				       size, initlist, protocol, false);
    }
  else
    {
      if (is_optional)
	UOBJC_PROTOCOL_OPT_CLS_METHODS_decl = 0;
      else
        UOBJC_CLASS_METHODS_decl = 0;
    }

  chain = is_optional ? PROTOCOL_OPTIONAL_NST_METHODS (protocol) 
		      : PROTOCOL_NST_METHODS (protocol);
  if (chain)
    {
      size = list_length (chain);

      method_list_template
	= build_method_prototype_list_template (objc_method_prototype_template,
						size);
      initlist
	= build_descriptor_table_initializer (objc_method_prototype_template,
					      chain);

      if (is_optional)
        UOBJC_PROTOCOL_OPT_NST_METHODS_decl
	  = generate_descriptor_table (method_list_template,
				       /* APPLE LOCAL radar 4695109 */
				       "_OBJC_PROTOCOL_INSTANCE_METHODS_OPT",
				       /* APPLE LOCAL ObjC new abi */
				       size, initlist, protocol, false);
      else
        UOBJC_INSTANCE_METHODS_decl
	  = generate_descriptor_table (method_list_template,
				       "_OBJC_PROTOCOL_INSTANCE_METHODS",
				       /* APPLE LOCAL ObjC new abi */
				       size, initlist, protocol, false);
    }
  else
    {
      if (is_optional)
	UOBJC_PROTOCOL_OPT_NST_METHODS_decl = 0;
      else
    	UOBJC_INSTANCE_METHODS_decl = 0;
    }
}
/* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */

static void
generate_protocol_references (tree plist)
{
  tree lproto;

  /* Forward declare protocols referenced.  */
  for (lproto = plist; lproto; lproto = TREE_CHAIN (lproto))
    {
      tree proto = TREE_VALUE (lproto);

      if (TREE_CODE (proto) == PROTOCOL_INTERFACE_TYPE
	  && PROTOCOL_NAME (proto))
	{
          if (! PROTOCOL_FORWARD_DECL (proto))
            build_protocol_reference (proto);

          if (PROTOCOL_LIST (proto))
            generate_protocol_references (PROTOCOL_LIST (proto));
        }
    }
}

/* Generate either '- .cxx_construct' or '- .cxx_destruct' for the
   current class.  */
#ifdef OBJCPLUS
static void
objc_generate_cxx_ctor_or_dtor (bool dtor)
{
  tree fn, body, compound_stmt, ivar;

  /* - (id) .cxx_construct { ... return self; } */
  /* - (void) .cxx_construct { ... }            */

  objc_set_method_type (MINUS_EXPR);
  objc_start_method_definition
   (objc_build_method_signature (build_tree_list (NULL_TREE,
						  dtor
						  ? void_type_node
						  : objc_object_type),
				 get_identifier (dtor
						 ? TAG_CXX_DESTRUCT
						 : TAG_CXX_CONSTRUCT),
				 make_node (TREE_LIST),
				 /* APPLE LOCAL radar 3803157 - objc attribute */
				 false), NULL_TREE);
  body = begin_function_body ();
  compound_stmt = begin_compound_stmt (0);

  ivar = CLASS_IVARS (implementation_template);
  /* Destroy ivars in reverse order.  */
  if (dtor)
    ivar = nreverse (copy_list (ivar));

  for (; ivar; ivar = TREE_CHAIN (ivar))
    {
      if (TREE_CODE (ivar) == FIELD_DECL)
	{
	  tree type = TREE_TYPE (ivar);

	  /* Call the ivar's default constructor or destructor.  Do not
	     call the destructor unless a corresponding constructor call
	     has also been made (or is not needed).  */
	  if (IS_AGGR_TYPE (type)
	      && (dtor
		  ? (TYPE_HAS_NONTRIVIAL_DESTRUCTOR (type)
		     && (!TYPE_NEEDS_CONSTRUCTING (type)
			 || TYPE_HAS_DEFAULT_CONSTRUCTOR (type)))
		  : (TYPE_NEEDS_CONSTRUCTING (type)
		     && TYPE_HAS_DEFAULT_CONSTRUCTOR (type))))
	    finish_expr_stmt
	     (build_special_member_call
	      (build_ivar_reference (DECL_NAME (ivar)),
	       dtor ? complete_dtor_identifier : complete_ctor_identifier,
	       NULL_TREE, type, LOOKUP_NORMAL));
	}
    }

  /* The constructor returns 'self'.  */
  if (!dtor)
    finish_return_stmt (self_decl);

  finish_compound_stmt (compound_stmt);
  finish_function_body (body);
  fn = current_function_decl;
  finish_function ();
  objc_finish_method_definition (fn);
}

/* The following routine will examine the current @interface for any
   non-POD C++ ivars requiring non-trivial construction and/or
   destruction, and then synthesize special '- .cxx_construct' and/or
   '- .cxx_destruct' methods which will run the appropriate
   construction or destruction code.  Note that ivars inherited from
   super-classes are _not_ considered.  */
static void
objc_generate_cxx_cdtors (void)
{
  bool need_ctor = false, need_dtor = false;
  tree ivar;

  /* APPLE LOCAL begin radar 4407151 */
  /* Error case, due to possibly an extra @end. */
  if (!objc_implementation_context)
    return;
  /* APPLE LOCAL end radar 4407151 */

  /* We do not want to do this for categories, since they do not have
     their own ivars.  */

  if (TREE_CODE (objc_implementation_context) != CLASS_IMPLEMENTATION_TYPE)
    return;

  /* First, determine if we even need a constructor and/or destructor.  */

  for (ivar = CLASS_IVARS (implementation_template); ivar;
       ivar = TREE_CHAIN (ivar))
    {
      if (TREE_CODE (ivar) == FIELD_DECL)
	{
	  tree type = TREE_TYPE (ivar);

	  if (IS_AGGR_TYPE (type))
	    {
	      if (TYPE_NEEDS_CONSTRUCTING (type)
		  && TYPE_HAS_DEFAULT_CONSTRUCTOR (type))
		/* NB: If a default constructor is not available, we will not
		   be able to initialize this ivar; the add_instance_variable()
		   routine will already have warned about this.  */
		need_ctor = true;

	      if (TYPE_HAS_NONTRIVIAL_DESTRUCTOR (type)
		  && (!TYPE_NEEDS_CONSTRUCTING (type)
		      || TYPE_HAS_DEFAULT_CONSTRUCTOR (type)))
		/* NB: If a default constructor is not available, we will not
		   call the destructor either, for symmetry.  */
		need_dtor = true;
	    }
	}
    }

  /* Generate '- .cxx_construct' if needed.  */

  if (need_ctor)
    objc_generate_cxx_ctor_or_dtor (false);

  /* Generate '- .cxx_destruct' if needed.  */

  if (need_dtor)
    objc_generate_cxx_ctor_or_dtor (true);

  /* The 'imp_list' variable points at an imp_entry record for the current
     @implementation.  Record the existence of '- .cxx_construct' and/or
     '- .cxx_destruct' methods therein; it will be included in the
     metadata for the class.  */
  if (flag_next_runtime)
    imp_list->has_cxx_cdtors = (need_ctor || need_dtor);
}
#endif

/* For each protocol which was referenced either from a @protocol()
   expression, or because a class/category implements it (then a
   pointer to the protocol is stored in the struct describing the
   class/category), we create a statically allocated instance of the
   Protocol class.  The code is written in such a way as to generate
   as few Protocol objects as possible; we generate a unique Protocol
   instance for each protocol, and we don't generate a Protocol
   instance if the protocol is never referenced (either from a
   @protocol() or from a class/category implementation).  These
   statically allocated objects can be referred to via the static
   (that is, private to this module) symbols _OBJC_PROTOCOL_n.

   The statically allocated Protocol objects that we generate here
   need to be fixed up at runtime in order to be used: the 'isa'
   pointer of the objects need to be set up to point to the 'Protocol'
   class, as known at runtime.

   The NeXT runtime fixes up all protocols at program startup time,
   before main() is entered.  It uses a low-level trick to look up all
   those symbols, then loops on them and fixes them up.

   The GNU runtime as well fixes up all protocols before user code
   from the module is executed; it requires pointers to those symbols
   to be put in the objc_symtab (which is then passed as argument to
   the function __objc_exec_class() which the compiler sets up to be
   executed automatically when the module is loaded); setup of those
   Protocol objects happen in two ways in the GNU runtime: all
   Protocol objects referred to by a class or category implementation
   are fixed up when the class/category is loaded; all Protocol
   objects referred to by a @protocol() expression are added by the
   compiler to the list of statically allocated instances to fixup
   (the same list holding the statically allocated constant string
   objects).  Because, as explained above, the compiler generates as
   few Protocol objects as possible, some Protocol object might end up
   being referenced multiple times when compiled with the GNU runtime,
   and end up being fixed up multiple times at runtime initialization.
   But that doesn't hurt, it's just a little inefficient.  */

static void
generate_protocols (void)
{
  tree p, encoding;
  tree decl;
  tree initlist, protocol_name_expr, refs_decl, refs_expr;

  /* If a protocol was directly referenced, pull in indirect references.  */
  for (p = protocol_chain; p; p = TREE_CHAIN (p))
    if (PROTOCOL_FORWARD_DECL (p) && PROTOCOL_LIST (p))
      generate_protocol_references (PROTOCOL_LIST (p));

  for (p = protocol_chain; p; p = TREE_CHAIN (p))
    {
      /* APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions */
      tree save_objc_implementation_context;
      tree nst_methods = PROTOCOL_NST_METHODS (p);
      tree cls_methods = PROTOCOL_CLS_METHODS (p);

      /* If protocol wasn't referenced, don't generate any code.  */
      decl = PROTOCOL_FORWARD_DECL (p);

      if (!decl)
	continue;

      /* Make sure we link in the Protocol class.  */
      add_class_reference (get_identifier (PROTOCOL_OBJECT_CLASS_NAME));

      while (nst_methods)
	{
	  if (! METHOD_ENCODING (nst_methods))
	    {
	      encoding = encode_method_prototype (nst_methods);
	      METHOD_ENCODING (nst_methods) = encoding;
	    }
	  nst_methods = TREE_CHAIN (nst_methods);
	}

      while (cls_methods)
	{
	  if (! METHOD_ENCODING (cls_methods))
	    {
	      encoding = encode_method_prototype (cls_methods);
	      METHOD_ENCODING (cls_methods) = encoding;
	    }

	  cls_methods = TREE_CHAIN (cls_methods);
	}
      /* APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions */
      generate_method_descriptors (p, false);

      if (PROTOCOL_LIST (p))
	refs_decl = generate_protocol_list (p);
      else
	refs_decl = 0;

      /* static struct objc_protocol _OBJC_PROTOCOL_<mumble>; */
      protocol_name_expr = add_objc_string (PROTOCOL_NAME (p), class_names);

      if (refs_decl)
	refs_expr = convert (build_pointer_type (build_pointer_type
						 (objc_protocol_template)),
			     build_unary_op (ADDR_EXPR, refs_decl, 0));
      else
      /* APPLE LOCAL begin radar 6285794 */
        refs_expr =
          convert (build_pointer_type (build_pointer_type (objc_protocol_template)),
                   integer_zero_node);
      /* APPLE LOCAL end radar 6285794 */
      /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
      save_objc_implementation_context = objc_implementation_context;
      objc_implementation_context = p;
      generate_v2_property_tables (p);
      objc_implementation_context = save_objc_implementation_context;

      nst_methods = PROTOCOL_OPTIONAL_NST_METHODS (p);
      cls_methods = PROTOCOL_OPTIONAL_CLS_METHODS (p);
      while (nst_methods)
        {
          if (! METHOD_ENCODING (nst_methods))
            {                     
              encoding = encode_method_prototype (nst_methods);
              METHOD_ENCODING (nst_methods) = encoding;
            }
          nst_methods = TREE_CHAIN (nst_methods);
        }
      while (cls_methods)                    
        {
          if (! METHOD_ENCODING (cls_methods))
            {
              encoding = encode_method_prototype (cls_methods);
              METHOD_ENCODING (cls_methods) = encoding;
            }

          cls_methods = TREE_CHAIN (cls_methods);
        }
      generate_method_descriptors (p, true);
      UOBJC_PROTOCOL_EXT_decl = generate_objc_protocol_extension (p, 
				  UOBJC_PROTOCOL_OPT_NST_METHODS_decl,
				  UOBJC_PROTOCOL_OPT_CLS_METHODS_decl, 
				  UOBJC_V2_PROPERTY_decl);

      /* UOBJC_INSTANCE_METHODS_decl/UOBJC_CLASS_METHODS_decl are set
	 by generate_method_descriptors, which is called above.  */
      initlist = build_protocol_initializer (TREE_TYPE (decl),
					     protocol_name_expr, refs_expr,
					     UOBJC_INSTANCE_METHODS_decl,
					     /* APPLE LOCAL ObjC new abi */
					     UOBJC_CLASS_METHODS_decl, false, NULL_TREE,
					     /* APPLE LOCAL radar 4695109 */
					     UOBJC_PROTOCOL_EXT_decl, NULL_TREE);
      /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */
      /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
      /* Force 4 byte alignment for protocols */
      DECL_ALIGN(decl) = 32;
      DECL_USER_ALIGN(decl) = 1;
#endif
      /* LLVM LOCAL end */

      finish_var_decl (decl, initlist);

      /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
      /* At -O0, we may have emitted references to the decl earlier. */
      if (!optimize)
        reset_initializer_llvm(decl);
#endif
      /* LLVM LOCAL end */
    }
}

static tree
build_protocol_initializer (tree type, tree protocol_name,
			    tree protocol_list, tree instance_methods,
			    /* APPLE LOCAL ObjC new abi */
			    tree class_methods, bool newabi, tree property_list,
/* APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions - radar 4695109 */
			    tree objc_protocol_or_opt_ins_meth, tree opt_cls_meth)
{
  tree initlist = NULL_TREE, expr;
  /* APPLE LOCAL begin ObjC new abi */
  /* APPLE LOCAL begin radar 4533974 - ObjC new protocol */
  /* APPLE LOCAL begin radar 4533974 - ObjC newprotocol - radar 4695109 */
  if (newabi)
    {
      /* 'isa' is NULL in the new ObjC abi */
      /* APPLE LOCAL begin radar 6285794 */
      expr = convert (objc_object_type, integer_zero_node);
      /* APPLE LOCAL end radar 6285794 */
    }
  /* APPLE LOCAL end radar 4533974 - ObjC newprotocol - radar 4695109 */
  /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
  /* "isa" field now points to struct _objc_protocol_extension * */
  else
    {
      if (!objc_protocol_or_opt_ins_meth)
        /* APPLE LOCAL begin radar 6285794 */
        {
          if (!objc_protocol_extension_template)
            build_objc_protocol_extension_template ();    
          
          expr =
            convert (build_pointer_type (objc_protocol_extension_template),
                     integer_zero_node);
        }
        /* APPLE LOCAL end radar 6285794 */
      else
	expr = convert (build_pointer_type (objc_protocol_extension_template),
			build_unary_op (ADDR_EXPR, objc_protocol_or_opt_ins_meth, 0));	
    }
  /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */

  initlist = tree_cons (NULL_TREE, expr, initlist);
  /* APPLE LOCAL end radar 4533974 - ObjC new protocol */
  /* APPLE LOCAL end ObjC new abi */
  initlist = tree_cons (NULL_TREE, protocol_name, initlist);
  initlist = tree_cons (NULL_TREE, protocol_list, initlist);

  if (!instance_methods)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_method_proto_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_method_proto_list_ptr,
		      build_unary_op (ADDR_EXPR, instance_methods, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  if (!class_methods)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_method_proto_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_method_proto_list_ptr,
		      build_unary_op (ADDR_EXPR, class_methods, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
  if (newabi)
    {
      /* APPLE LOCAL begin radar 4695109 */
      if (!objc_protocol_or_opt_ins_meth)
        /* APPLE LOCAL begin radar 6285794 */
        initlist = tree_cons (NULL_TREE,
                              convert (objc_method_proto_list_ptr,
                                       integer_zero_node),
                              initlist);
        /* APPLE LOCAL end radar 6285794 */
      else
	{
	  expr = convert (objc_method_proto_list_ptr,
			  build_unary_op (ADDR_EXPR, objc_protocol_or_opt_ins_meth, 0));
	  initlist = tree_cons (NULL_TREE, expr, initlist);
	}
      if (!opt_cls_meth)
        /* APPLE LOCAL begin radar 6285794 */
        initlist = tree_cons (NULL_TREE,
                              convert (objc_method_proto_list_ptr,
                                       integer_zero_node),
                              initlist);
        /* APPLE LOCAL end radar 6285794 */
      else
	{
	  expr = convert (objc_method_proto_list_ptr,
			  build_unary_op (ADDR_EXPR, opt_cls_meth, 0));
	  initlist = tree_cons (NULL_TREE, expr, initlist);
	}
      /* APPLE LOCAL end radar 4695109 */
      if (!property_list)
        /* APPLE LOCAL begin radar 6285794 */
        initlist = tree_cons (NULL_TREE,
                              convert (objc_prop_list_ptr,
                                       integer_zero_node),
                              initlist);
        /* APPLE LOCAL end radar 6285794 */
      else
	{
	  expr = convert (objc_prop_list_ptr,
			  build_unary_op (ADDR_EXPR, property_list, 0));
	  initlist = tree_cons (NULL_TREE, expr, initlist);
	}
      /* APPLE LOCAL begin radar 5192466 */
      /* const uint32_t size;  = sizeof(struct protocol_t) */
      expr = build_int_cst (
	      NULL_TREE, 
	      TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_v2_protocol_template)));
      initlist = tree_cons (NULL_TREE, expr, initlist);
      /* const uint32_t flags; = 0 */
      initlist = tree_cons (NULL_TREE, integer_zero_node, initlist);
      /* APPLE LOCAL end radar 5192466 */
    }
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
  return objc_build_constructor (type, nreverse (initlist));
}

/* struct _objc_category {
     char *category_name;
     char *class_name;
     struct _objc_method_list *instance_methods;
     struct _objc_method_list *class_methods;
     struct _objc_protocol_list *protocols;
     // APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions
     uint32_t size;	// sizeof (struct _objc_category)
     struct _objc_property_list *instance_properties;  // category's own @property decl.
     // APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions
   };   */

static void
build_category_template (void)
{
  tree field_decl, field_decl_chain;

  objc_category_template = start_struct (RECORD_TYPE,
					 get_identifier (UTAG_CATEGORY));

  /* char *category_name; */
  field_decl = create_field_decl (string_type_node, "category_name");
  field_decl_chain = field_decl;

  /* char *class_name; */
  field_decl = create_field_decl (string_type_node, "class_name");
  chainon (field_decl_chain, field_decl);

  /* struct _objc_method_list *instance_methods; */
  field_decl = create_field_decl (objc_method_list_ptr,
				  "instance_methods");
  chainon (field_decl_chain, field_decl);

  /* struct _objc_method_list *class_methods; */
  field_decl = create_field_decl (objc_method_list_ptr,
				  "class_methods");
  chainon (field_decl_chain, field_decl);

  /* struct _objc_protocol **protocol_list; */
  field_decl = create_field_decl (build_pointer_type
				  (build_pointer_type
				   (objc_protocol_template)),
				  "protocol_list");
  chainon (field_decl_chain, field_decl);

  /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
  /*  uint32_t size; - sizeof (struct _objc_category) */
  field_decl = create_field_decl (integer_type_node, "size");
  chainon (field_decl_chain, field_decl);
  /*  struct _objc_property_list *instance_properties;  This field describes
      the category's @property declarations. Properties from inherited protocols
      are not included. */
  field_decl = create_field_decl (build_pointer_type (
				    xref_tag (RECORD_TYPE, 
					      get_identifier (UTAG_PROPERTY_LIST))), 
				  "instance_properties");
  chainon (field_decl_chain, field_decl);
  /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */

  finish_struct (objc_category_template, field_decl_chain, NULL_TREE);
}

/* struct _objc_selector {
     SEL sel_id;
     char *sel_type;
   }; */

static void
build_selector_template (void)
{

  tree field_decl, field_decl_chain;

  objc_selector_template
    = start_struct (RECORD_TYPE, get_identifier (UTAG_SELECTOR));

  /* SEL sel_id; */
  field_decl = create_field_decl (objc_selector_type, "sel_id");
  field_decl_chain = field_decl;

  /* char *sel_type; */
  field_decl = create_field_decl (string_type_node, "sel_type");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_selector_template, field_decl_chain, NULL_TREE);
}

/* APPLE LOCAL begin ObjC new abi */

/* Build type for a category:
   struct category_t {
     const char * const name;
     struct class_t *const cls;
     const struct method_list_t * const instance_methods;
     const struct method_list_t * const class_methods;
     const struct protocol_list_t * const protocols;
     const struct _prop_list_t * const properties;
   }  */

static void
build_v2_category_template (void)
{
  tree field_decl, field_decl_chain;

  objc_v2_category_template = start_struct (RECORD_TYPE,
                                         get_identifier ("_category_t"));

  /* char *name; */
  field_decl = create_field_decl (string_type_node, "name");
  field_decl_chain = field_decl;

  /* struct class_t *const cls; */
  field_decl = create_field_decl (build_pointer_type (objc_v2_class_template), 
				  "cls");
  chainon (field_decl_chain, field_decl);

  /* struct method_list_t *instance_methods; */
  field_decl = create_field_decl (objc_method_list_ptr,
                                  "instance_methods");
  chainon (field_decl_chain, field_decl);

  /* struct method_list_t *class_methods; */
  field_decl = create_field_decl (objc_method_list_ptr,
                                  "class_methods");
  chainon (field_decl_chain, field_decl);

  /* struct protocol_list_t *protocol_list; */
  field_decl = create_field_decl (build_pointer_type
                                   (objc_v2_protocol_template),
                                  "protocol_list");
  chainon (field_decl_chain, field_decl);

  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
  /* struct _prop_list_t * properties; */
  field_decl = create_field_decl (objc_prop_list_ptr,
				  "properties");
  chainon (field_decl_chain, field_decl);
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
  finish_struct (objc_v2_category_template, field_decl_chain, NULL_TREE);
}


/* Build following types which represent each class implementation.

  struct class_t {
    struct class_t *isa;
    // APPLE LOCAL radar 4705214
    struct class_t * const superclass;
    __strong void *cache;
    __strong IMP *vtable;
    // APPLE LOCAL radar 4705214
    __strong struct class_ro_t *ro;
  };

  struct class_ro_t {
    uint32_t const flags;
    uint32_t const instanceStart;
    uint32_t const instanceSize;
  #ifdef __LP64__
    uint32_t const reserved;
  #endif
    const uint8_t * const ivarLayout;
    // APPLE LOCAL radar 4705214
    // - Moved to: 'struct class_t' struct class_t * const superclass;

    const char *const name;
    const struct method_list_t * const baseMethods;
    const struct objc_protocol_list *const baseProtocols;
    const struct ivar_list_t *const ivars;
    // APPLE LOCAL radar 4695101
    const uint8_t * const weakIvarLayout;
    const struct _prop_list_t * const properties;
  };  */

static void
build_v2_class_template (void)
{
  tree field_decl, field_decl_chain;

  objc_v2_class_ro_template
    = start_struct (RECORD_TYPE, get_identifier (UTAG_V2_CLASS_RO));

  objc_v2_class_template
    = start_struct (RECORD_TYPE, get_identifier (UTAG_V2_CLASS));

  /* struct class_t *isa; */
  field_decl = create_field_decl (build_pointer_type (objc_v2_class_template),
				  "isa");
  field_decl_chain = field_decl;

  /* struct class_t * const superclass */
  field_decl = create_field_decl (build_pointer_type (objc_v2_class_template),
				  "superclass");
  chainon (field_decl_chain, field_decl);

  /* __strong void *cache; */
  /* TODO: __strong is missing. */
  field_decl = create_field_decl (build_pointer_type (void_type_node),
				  "cache");
  chainon (field_decl_chain, field_decl);
  
  /* __strong IMP *vtable; */
  /* TODO: __strong is missing. */
  field_decl = create_field_decl (build_pointer_type (objc_imp_type),
				  "vtable");
  chainon (field_decl_chain, field_decl);

  /* __strong struct class_ro_t *ro; */
  /* TODO: __strong is missing. */
  field_decl = create_field_decl (build_pointer_type (objc_v2_class_ro_template),
				  "ro");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_v2_class_template, field_decl_chain, NULL_TREE);

  /* struct class_ro_t {...} */
  
  /* uint32_t const flags; */
  field_decl = create_field_decl (integer_type_node, "flags");
  field_decl_chain = field_decl;

  /* uint32_t const instanceStart */
  field_decl = create_field_decl (integer_type_node, "instanceStart");
  chainon (field_decl_chain, field_decl);

  /* uint32_t const instanceSize */
  field_decl = create_field_decl (integer_type_node, "instanceSize");
  chainon (field_decl_chain, field_decl);
  
  if (TARGET_64BIT)
    {
      /* uint32_t const reserved */
      field_decl = create_field_decl (integer_type_node, "reserved");
      chainon (field_decl_chain, field_decl);
    }

  /* const uint8_t * const ivarLayout */
  /* APPLE LOCAL 4695101 */
  field_decl = create_field_decl (string_type_node, 
				  "ivarLayout");
  chainon (field_decl_chain, field_decl);

  /* const char *const name; */
  field_decl = create_field_decl (string_type_node, "name");
  chainon (field_decl_chain, field_decl);
  
  /* const struct method_list_t * const baseMethods */
  field_decl = create_field_decl (objc_method_list_ptr,
				  "baseMethods");
  chainon (field_decl_chain, field_decl);

  /* const struct objc_protocol_list *const baseProtocols */
  field_decl = create_field_decl (build_pointer_type 
				  (xref_tag (RECORD_TYPE, 
					     get_identifier
					     (UTAG_V2_PROTOCOL_LIST))),
				  "baseProtocols");
  chainon (field_decl_chain, field_decl);

  /* const struct ivar_list_t *const ivars */
  field_decl = create_field_decl (objc_v2_ivar_list_ptr,
				  "ivars");  
  chainon (field_decl_chain, field_decl);

  /* APPLE LOCAL begin radar 4695101 */
  /* const uint8_t * const weakIvarLayout; */
  field_decl = create_field_decl (string_type_node, "weakIvarLayout");
  chainon (field_decl_chain, field_decl);
  /* APPLE LOCAL end radar 4695101 */

  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
  /* struct _prop_list_t * properties */
  field_decl = create_field_decl (objc_prop_list_ptr,
				  "properties");
  chainon (field_decl_chain, field_decl);
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
   finish_struct (objc_v2_class_ro_template, field_decl_chain, NULL_TREE);
}
/* APPLE LOCAL end ObjC new abi */

/* struct _objc_class {
     struct _objc_class *isa;
     struct _objc_class *super_class;
     char *name;
     long version;
     long info;
     long instance_size;
     struct _objc_ivar_list *ivars;
     struct _objc_method_list *methods;
     #ifdef __NEXT_RUNTIME__
       struct objc_cache *cache;
     #else
       struct sarray *dtable;
       struct _objc_class *subclass_list;
       struct _objc_class *sibling_class;
     #endif
     struct _objc_protocol_list *protocols;
     // APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions
     #ifdef __NEXT_RUNTIME__
       const char *ivar_layout;
       struct _objc_class_ext  *ext;
     #else
       void *gc_object_type;
     #endif
     // APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions
   };  */

/* NB: The 'sel_id' and 'gc_object_type' fields are not being used by
   the NeXT/Apple runtime; still, the compiler must generate them to
   maintain backward binary compatibility (and to allow for future
   expansion).  */

static void
build_class_template (void)
{
  tree field_decl, field_decl_chain;

  objc_class_template
    = start_struct (RECORD_TYPE, get_identifier (UTAG_CLASS));

  /* struct _objc_class *isa; */
  field_decl = create_field_decl (build_pointer_type (objc_class_template),
				  "isa");
  field_decl_chain = field_decl;

  /* struct _objc_class *super_class; */
  field_decl = create_field_decl (build_pointer_type (objc_class_template),
				  "super_class");
  chainon (field_decl_chain, field_decl);

  /* char *name; */
  field_decl = create_field_decl (string_type_node, "name");
  chainon (field_decl_chain, field_decl);

  /* long version; */
  field_decl = create_field_decl (long_integer_type_node, "version");
  chainon (field_decl_chain, field_decl);

  /* long info; */
  field_decl = create_field_decl (long_integer_type_node, "info");
  chainon (field_decl_chain, field_decl);

  /* long instance_size; */
  field_decl = create_field_decl (long_integer_type_node, "instance_size");
  chainon (field_decl_chain, field_decl);

  /* struct _objc_ivar_list *ivars; */
  field_decl = create_field_decl (objc_ivar_list_ptr,
				  "ivars");
  chainon (field_decl_chain, field_decl);

  /* struct _objc_method_list *methods; */
  field_decl = create_field_decl (objc_method_list_ptr,
				  "methods");
  chainon (field_decl_chain, field_decl);

  if (flag_next_runtime)
    {
      /* struct objc_cache *cache; */
      field_decl = create_field_decl (build_pointer_type
				      (xref_tag (RECORD_TYPE,
						 get_identifier
						 ("objc_cache"))),
				      "cache");
      chainon (field_decl_chain, field_decl);
    }
  else
    {
      /* struct sarray *dtable; */
      field_decl = create_field_decl (build_pointer_type
				      (xref_tag (RECORD_TYPE,
						 get_identifier
						 ("sarray"))),
				      "dtable");
      chainon (field_decl_chain, field_decl);

      /* struct objc_class *subclass_list; */
      field_decl = create_field_decl (build_pointer_type
				      (objc_class_template),
				      "subclass_list");
      chainon (field_decl_chain, field_decl);

      /* struct objc_class *sibling_class; */
      field_decl = create_field_decl (build_pointer_type
				      (objc_class_template),
				      "sibling_class");
      chainon (field_decl_chain, field_decl);
    }

  /* struct _objc_protocol **protocol_list; */
  field_decl = create_field_decl (build_pointer_type
				  (build_pointer_type
				   (xref_tag (RECORD_TYPE,
					     get_identifier
					     (UTAG_PROTOCOL)))),
				  "protocol_list");
  chainon (field_decl_chain, field_decl);

  /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
  if (flag_next_runtime)
    {
      /* const char *ivar_layout; */
      field_decl = create_field_decl (string_type_node, "ivar_layout");
      chainon (field_decl_chain, field_decl);

      /* struct _objc_class_ext *ext; */
      field_decl = create_field_decl (build_pointer_type (
					xref_tag (RECORD_TYPE, 
						  get_identifier 
						  (UTAG_CLASS_EXT))),
				      "ext");
      chainon (field_decl_chain, field_decl);
    }
  else
    {
      /* void *gc_object_type; */
      field_decl = create_field_decl (build_pointer_type (void_type_node),
				      "gc_object_type");
      chainon (field_decl_chain, field_decl);
    }
  /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */

  finish_struct (objc_class_template, field_decl_chain, NULL_TREE);
}

/* Generate appropriate forward declarations for an implementation.  */

static void
synth_forward_declarations (void)
{
  tree an_id;

  /* static struct objc_class _OBJC_CLASS_<my_name>; */
  UOBJC_CLASS_decl = build_metadata_decl ("_OBJC_CLASS",
					  /* APPLE LOCAL radar 5202926 */
					  objc_class_template, false);

  /* static struct objc_class _OBJC_METACLASS_<my_name>; */
  UOBJC_METACLASS_decl = build_metadata_decl ("_OBJC_METACLASS",
						  /* APPLE LOCAL radar 5202926 */
						  objc_class_template, false);

  /* Pre-build the following entities - for speed/convenience.  */

  an_id = get_identifier ("super_class");
  ucls_super_ref = objc_build_component_ref (UOBJC_CLASS_decl, an_id);
  uucls_super_ref = objc_build_component_ref (UOBJC_METACLASS_decl, an_id);
}

/* APPLE LOCAL begin ObjC abi v2 */
/* Generate appropriate forward declarations for an implementation.  */

static void
synth_v2_forward_declarations (void)
{
  /* struct class_t OBJC_CLASS_$_<my_name>; */
  UOBJC_V2_CLASS_decl = build_metadata_decl ("OBJC_CLASS_$",
					     /* APPLE LOCAL radar 5202926 */
                                             objc_v2_class_template, true);

 /* struct class_t OBJC_METACLASS_$_<my_name>; */
 UOBJC_V2_METACLASS_decl = build_metadata_decl ("OBJC_METACLASS_$",
						/* APPLE LOCAL radar 5202926 */
                                                objc_v2_class_template, true);
}

static void
/* APPLE LOCAL end ObjC abi v2 */
error_with_ivar (const char *message, tree decl)
{
  error ("%J%s %qs", decl,
         message, gen_declaration (decl));

}

static void
check_ivars (tree inter, tree imp)
{
  tree intdecls = CLASS_RAW_IVARS (inter);
  tree impdecls = CLASS_RAW_IVARS (imp);

  while (1)
    {
      tree t1, t2;

#ifdef OBJCPLUS
      if (intdecls && TREE_CODE (intdecls) == TYPE_DECL)
	intdecls = TREE_CHAIN (intdecls);
#endif
      if (intdecls == 0 && impdecls == 0)
	break;
      if (intdecls == 0 || impdecls == 0)
	{
	  error ("inconsistent instance variable specification");
	  break;
	}

      t1 = TREE_TYPE (intdecls); t2 = TREE_TYPE (impdecls);

      if (!comptypes (t1, t2)
	  || !tree_int_cst_equal (DECL_INITIAL (intdecls),
				  DECL_INITIAL (impdecls)))
	{
	  if (DECL_NAME (intdecls) == DECL_NAME (impdecls))
	    {
	      error_with_ivar ("conflicting instance variable type",
			       impdecls);
	      error_with_ivar ("previous declaration of",
			       intdecls);
	    }
	  else			/* both the type and the name don't match */
	    {
	      error ("inconsistent instance variable specification");
	      break;
	    }
	}

      else if (DECL_NAME (intdecls) != DECL_NAME (impdecls))
	{
	  error_with_ivar ("conflicting instance variable name",
			   impdecls);
	  error_with_ivar ("previous declaration of",
			   intdecls);
	}

      intdecls = TREE_CHAIN (intdecls);
      impdecls = TREE_CHAIN (impdecls);
    }
}

/* APPLE LOCAL begin ObjC new abi */
/* Set 'objc_v2_message_ref_template' to the data type node for 'struct _message_ref_t'.
   This needs to be done just once per compilation.  Also Set 
   'objc_v2_super_message_ref_template' to data type node 
   for 'struct _super_message_ref_t'.

   struct _message_ref_t {
     IMP messenger;
     SEL name;
   };
   where IMP is: id (*) (id, _message_ref_t*, ...)

   struct _super_message_ref_t {
     SUPER_IMP messenger;
     SEL name;
   };
   where SUPER_IMP is: id (*) ( super_t*, _super_message_ref_t*, ...)  */

static void
build_message_ref_template (void)
{
  tree ptr_message_ref_t;
  tree field_decl, field_decl_chain;
  /* struct _message_ref_t {...} */
  objc_v2_message_ref_template = start_struct (RECORD_TYPE, 
						   get_identifier ("_message_ref_t"));

  /* IMP messenger; */
  ptr_message_ref_t = build_pointer_type (xref_tag (
					  RECORD_TYPE, get_identifier ("_message_ref_t")));
  objc_v2_imp_type
    = build_pointer_type
          (build_function_type (objc_object_type,
                                tree_cons (NULL_TREE, objc_object_type,
                                           tree_cons (NULL_TREE, ptr_message_ref_t,
                                                      NULL_TREE))));
  field_decl = create_field_decl (objc_v2_imp_type, "messenger");
  field_decl_chain = field_decl;

  /* SEL name; */
  field_decl = create_field_decl (objc_selector_type, "name");
  chainon (field_decl_chain, field_decl); 

  finish_struct (objc_v2_message_ref_template, field_decl_chain, NULL_TREE);
  objc_v2_selector_type = build_pointer_type (objc_v2_message_ref_template);

  /* struct _super_message_ref_t {...} */
  objc_v2_super_message_ref_template = start_struct (RECORD_TYPE, 
						         get_identifier ("_super_message_ref_t"));

  /* SUPER_IMP messenger; */
  ptr_message_ref_t = build_pointer_type (xref_tag (
					  RECORD_TYPE, get_identifier ("_super_message_ref_t")));

  objc_v2_super_imp_type
    = build_pointer_type
          (build_function_type (objc_object_type,
                                tree_cons (NULL_TREE, objc_super_type,
                                           tree_cons (NULL_TREE, ptr_message_ref_t,
                                                      NULL_TREE))));
  field_decl = create_field_decl (objc_v2_super_imp_type, "messenger");
  field_decl_chain = field_decl;

  /* SEL name; */
  field_decl = create_field_decl (objc_selector_type, "name");
  chainon (field_decl_chain, field_decl); 

  finish_struct (objc_v2_super_message_ref_template, field_decl_chain, NULL_TREE);
  objc_v2_super_selector_type = build_pointer_type (objc_v2_super_message_ref_template);
}
/* APPLE LOCAL end ObjC new abi */

/* Set 'objc_super_template' to the data type node for 'struct _objc_super'.
   This needs to be done just once per compilation.  */

/* APPLE LOCAL begin ObjC abi v2 */
/* struct _objc_super {
     struct _objc_object *self;
     struct _objc_class *cls;   // or Class cls; for the abi v2
   };  */
/* APPLE LOCAL end ObjC abi v2 */

static void
build_super_template (void)
{
  tree field_decl, field_decl_chain;

  objc_super_template = start_struct (RECORD_TYPE, get_identifier (UTAG_SUPER));

  /* struct _objc_object *self; */
  field_decl = create_field_decl (objc_object_type, "self");
  field_decl_chain = field_decl;

  /* APPLE LOCAL begin ObjC abi v2 */
  if (flag_objc_abi == 2)
    {
      /* Class cls; */
      field_decl = create_field_decl (objc_class_type, "cls");
    }
  else
    {
      /* struct _objc_class *cls; */
      field_decl = create_field_decl (build_pointer_type (objc_class_template),
                                      "cls");
    }
  /* APPLE LOCAL end ObjC abi v2 */
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_super_template, field_decl_chain, NULL_TREE);
}

/* APPLE LOCAL begin ObjC new abi */

/* struct ivar_t {
     unsigned long int *offset;
     char *name;
     char *type;
     uint32_t alignment;
     uint32_t size;
   };  */

static tree
build_v2_ivar_t_template (void)
{
  tree objc_ivar_id, objc_ivar_record;
  tree field_decl, field_decl_chain;

  objc_ivar_id = get_identifier ("_ivar_t");
  objc_ivar_record = start_struct (RECORD_TYPE, objc_ivar_id);

  /* uint32_t *offset */
  field_decl = create_field_decl (
		 /* APPLE LOCAL radar 4441049 */
		 build_pointer_type (TREE_TYPE (size_zero_node)), "offset");
  field_decl_chain = field_decl;

  /* char *name; */
  field_decl = create_field_decl (string_type_node, "name");
  chainon (field_decl_chain, field_decl);

  /* char *type; */
  field_decl = create_field_decl (string_type_node, "type");
  chainon (field_decl_chain, field_decl);

  /* uint32_t alignment; */
  field_decl = create_field_decl (integer_type_node, "alignment");
  chainon (field_decl_chain, field_decl);

  /* uint32_t size; */
  field_decl = create_field_decl (integer_type_node, "size");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_ivar_record, field_decl_chain, NULL_TREE);

  return objc_ivar_record;
}

/* Build up:

  struct ivar_list_t {
    uint32 entsize;
    uint32 count;
    struct iver_t list[count];
  };  */

static tree
build_v2_ivar_list_t_template (tree list_type, int size)
{
  tree objc_ivar_list_record;
  tree field_decl, field_decl_chain;

  objc_ivar_list_record = start_struct (RECORD_TYPE, NULL_TREE);

  /* uint32 entsize; */
  field_decl = create_field_decl (integer_type_node, "entsize");
  field_decl_chain = field_decl;

  /* uint32 count; */
  field_decl = create_field_decl (integer_type_node, "count");
  chainon (field_decl_chain, field_decl);

  /* struct objc_ivar ivar_list[]; */
  field_decl = create_field_decl (build_array_type
                                  (list_type,
                                   build_index_type
                                   (build_int_cst (NULL_TREE, size - 1))),
                                  "list");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_ivar_list_record, field_decl_chain, NULL_TREE);

  return objc_ivar_list_record;

}
     
/* APPLE LOCAL end ObjC new abi */

/* struct _objc_ivar {
     char *ivar_name;
     char *ivar_type;
     int ivar_offset;
   };  */

static tree
build_ivar_template (void)
{
  tree objc_ivar_id, objc_ivar_record;
  tree field_decl, field_decl_chain;

  objc_ivar_id = get_identifier (UTAG_IVAR);
  objc_ivar_record = start_struct (RECORD_TYPE, objc_ivar_id);

  /* char *ivar_name; */
  field_decl = create_field_decl (string_type_node, "ivar_name");
  field_decl_chain = field_decl;

  /* char *ivar_type; */
  field_decl = create_field_decl (string_type_node, "ivar_type");
  chainon (field_decl_chain, field_decl);

  /* int ivar_offset; */
  field_decl = create_field_decl (integer_type_node, "ivar_offset");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_ivar_record, field_decl_chain, NULL_TREE);

  return objc_ivar_record;
}

/* struct {
     int ivar_count;
     struct objc_ivar ivar_list[ivar_count];
   };  */

static tree
build_ivar_list_template (tree list_type, int size)
{
  tree objc_ivar_list_record;
  tree field_decl, field_decl_chain;

  objc_ivar_list_record = start_struct (RECORD_TYPE, NULL_TREE);

  /* int ivar_count; */
  field_decl = create_field_decl (integer_type_node, "ivar_count");
  field_decl_chain = field_decl;

  /* struct objc_ivar ivar_list[]; */
  field_decl = create_field_decl (build_array_type
				  (list_type,
				   build_index_type
				   (build_int_cst (NULL_TREE, size - 1))),
				  "ivar_list");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_ivar_list_record, field_decl_chain, NULL_TREE);

  return objc_ivar_list_record;
}

/* struct {
     struct _objc__method_prototype_list *method_next;
     int method_count;
     struct objc_method method_list[method_count];
   };  */

/* APPLE LOCAL begin ObjC new abi */
/* struct method_list_t {
     uint32_t entsize;
     uint32_t method_count;
     struct objc_method method_list[method_count];
   };  */

static tree
build_v2_method_list_template (tree list_type, int size)
{
  tree method_list_t_record;
  tree field_decl, field_decl_chain;

  method_list_t_record = start_struct (RECORD_TYPE, NULL_TREE);

  /* uint32_t const entsize */
  field_decl = create_field_decl (integer_type_node, "entsize");

  field_decl_chain = field_decl;

  /* int method_count; */
  field_decl = create_field_decl (integer_type_node, "method_count");
  chainon (field_decl_chain, field_decl);

  /* struct objc_method method_list[]; */
  field_decl = create_field_decl (build_array_type
				  (list_type,
				   build_index_type
				   (build_int_cst (NULL_TREE, size - 1))),
				  "method_list");
  chainon (field_decl_chain, field_decl);

  finish_struct (method_list_t_record, field_decl_chain, NULL_TREE);

  return method_list_t_record;
}
static tree
/* APPLE LOCAL end ObjC new abi */
build_method_list_template (tree list_type, int size)
{
  tree objc_ivar_list_record;
  tree field_decl, field_decl_chain;

  objc_ivar_list_record = start_struct (RECORD_TYPE, NULL_TREE);

  /* struct _objc__method_prototype_list *method_next; */
  field_decl = create_field_decl (objc_method_proto_list_ptr,
				  "method_next");
  field_decl_chain = field_decl;

  /* int method_count; */
  field_decl = create_field_decl (integer_type_node, "method_count");
  chainon (field_decl_chain, field_decl);

  /* struct objc_method method_list[]; */
  field_decl = create_field_decl (build_array_type
				  (list_type,
				   build_index_type
				   (build_int_cst (NULL_TREE, size - 1))),
				  "method_list");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_ivar_list_record, field_decl_chain, NULL_TREE);

  return objc_ivar_list_record;
}

/* APPLE LOCAL begin ObjC new abi */

/* APPLE LOCAL begin radar 4705298, 4843145 */

/* Return a class's visibility attribute string, or 
   NULL if it has no visibility attribute set. */

static const char *
objc_class_visibility (tree class)
{
  tree chain;
  /* APPLE LOCAL begin radar 4894756 */
  /* Handle very odd case of a class with no @interface but with @implementation only! */
  if (class && TREE_CODE (class) == CLASS_IMPLEMENTATION_TYPE)
    return NULL;
  /* APPLE LOCAL end radar 4894756 */
  gcc_assert (class && TREE_CODE (class) == CLASS_INTERFACE_TYPE);
  for (chain = CLASS_ATTRIBUTES (class); chain; chain = TREE_CHAIN (chain))
    if (is_attribute_p ("visibility", TREE_PURPOSE (chain)))
      return TREE_STRING_POINTER (TREE_VALUE (TREE_VALUE ((chain))));
  return NULL;
}

/* Create a symbol whose visibility attribute matches that of 
   the given class. */

static tree
objc_create_global_decl_for_class (tree type, const char *name, tree class)
{
  tree decl = create_global_decl (type, name);
  const char *visibility = objc_class_visibility (class);

  if (visibility  &&  strcmp (visibility, "hidden") == 0) 
    {
      DECL_VISIBILITY (decl) = VISIBILITY_HIDDEN;
      DECL_VISIBILITY_SPECIFIED (decl) = 1;
    } 
  else if (visibility  &&  strcmp (visibility, "default") == 0) 
    {
      DECL_VISIBILITY (decl) = VISIBILITY_DEFAULT;
      DECL_VISIBILITY_SPECIFIED (decl) = 1;
    }

  return decl;
}
/* APPLE LOCAL end radar 4705298 , 4843145*/

/* This routine builds a name to hold ivar offset. It is of the form:
   .objc_ivar.CLASS_NAME.FIELD_DECL  */

static void
create_ivar_offset_name (char *buf, tree class_name, tree field_decl)
{
  tree fname = DECL_NAME (field_decl);

  /* APPLE LOCAL radar 4540451 */
  sprintf (buf, "%s%s.%s", STRING_V2_IVAR_OFFSET_PREFIX,
	   IDENTIFIER_POINTER (class_name),
	   IDENTIFIER_POINTER (fname));
  return;
}

/* This routine declares a variable to hold the offset for ivar FIELD_DECL.
   Variable name is .objc_ivar.ClassName.IvarName. */

static tree
ivar_offset_ref (tree class_name, tree field_decl)
{
  tree decl;
  char buf[512];
  tree *chain;
  tree iter;
  bool global_var;
  tree field_decl_id;

  /* APPLE LOCAL radar 5610134 */
  gcc_assert (field_decl);
  create_ivar_offset_name (buf, class_name, field_decl);
  field_decl_id = get_identifier (buf);

  for (iter = ivar_offset_ref_chain; iter; iter = TREE_CHAIN (iter))
    {
      tree var = TREE_PURPOSE (iter);
      if (DECL_NAME (var) == field_decl_id)
	return var;
    }

  /* An existing offset symbol not found. Create a new one and add to the chain. */
  chain = &ivar_offset_ref_chain;
  /* APPLE LOCAL radar 4951615 */
  global_var =  IVAR_PUBLIC_OR_PROTECTED (field_decl);
  /* APPLE LOCAL begin radar 4441049 */
  /* APPLE LOCAL begin radar 4705298, 4843145 */
  decl = global_var ? 
         objc_create_global_decl_for_class (
	   TREE_TYPE (size_zero_node), buf, implementation_template) : 
	 create_hidden_decl (TREE_TYPE (size_zero_node), buf);
  /* APPLE LOCAL end radar 4705298 , 4843145*/
  /* APPLE LOCAL end radar 4441049 */

  while (*chain)
    chain = &TREE_CHAIN (*chain);

  /* APPLE LOCAL begin radar 4441049 */
  *chain = tree_cons (decl, byte_position (field_decl), NULL_TREE);
  /* APPLE LOCAL end radar 4441049 */
  
  return decl;
}

/* This routine builds initializer-list needed to initialize 'struct ivar_t list[count]
   of 'struct ivar_list_t' meta data. TYPE is 'struct ivar_t' and FIELD_DECL is
   list of ivars for the target class.  */

static tree
build_v2_ivar_list_initializer (tree class_name, tree type, tree field_decl)
{
  tree initlist = NULL_TREE;
  int val;

  do {
    tree ivar = NULL_TREE;

    /* Unnamed bitfields are ignored. */
    if (!DECL_NAME (field_decl))
      {
        do
          field_decl = TREE_CHAIN (field_decl);
        while (field_decl && TREE_CODE (field_decl) != FIELD_DECL);
        continue;
      }

    /* Set offset */
    ivar = tree_cons (NULL_TREE, 
		      build_unary_op (ADDR_EXPR, ivar_offset_ref (class_name, field_decl), 0), 
		      ivar);

    /* Set name */
    /* APPLE LOCAL begin radar 5724385 */
    /* At this point fields must be named (no unnamed bitfield, that is) */
    gcc_assert (DECL_NAME (field_decl));
    ivar = tree_cons (NULL_TREE,
                      add_objc_string (DECL_NAME (field_decl),
                                       meth_var_names), ivar);
    /* APPLE LOCAL end radar 5724385 */

    /* Set type */
    encode_field_decl (field_decl,
		       obstack_object_size (&util_obstack),
		       OBJC_ENCODE_DONT_INLINE_DEFS);
    /* Null terminate string.  */
    obstack_1grow (&util_obstack, 0);
    ivar
      = tree_cons
          (NULL_TREE,
           add_objc_string (get_identifier (obstack_finish (&util_obstack)),
                            meth_var_types),
           ivar);
    obstack_free (&util_obstack, util_firstobj);

    /* Set alignment */
    /* APPLE LOCAL begin radar 5724385 */
    val = TYPE_ALIGN_UNIT (
            DECL_BIT_FIELD_TYPE (field_decl) ? DECL_BIT_FIELD_TYPE (field_decl) : 
            TREE_TYPE (field_decl));
    /* APPLE LOCAL end radar 5724385 */
    val = exact_log2 (val);
    ivar = tree_cons (NULL_TREE, build_int_cst (NULL_TREE, val), ivar);

    /* Set size */
    val = TREE_INT_CST_LOW (DECL_SIZE_UNIT (field_decl));
    ivar = tree_cons (NULL_TREE, build_int_cst (NULL_TREE, val), ivar);

    initlist = tree_cons (NULL_TREE,
			  objc_build_constructor (type, nreverse (ivar)),
			  initlist);

    do
       field_decl = TREE_CHAIN (field_decl);
    while (field_decl && TREE_CODE (field_decl) != FIELD_DECL);
  }
  while (field_decl);

  return objc_build_constructor (build_array_type (type, 0),
				 nreverse (initlist));
}

static tree
/* APPLE LOCAL end ObjC new abi */
build_ivar_list_initializer (tree type, tree field_decl)
{
  tree initlist = NULL_TREE;

  do
    {
      tree ivar = NULL_TREE;

      /* Set name.  */
      if (DECL_NAME (field_decl))
	ivar = tree_cons (NULL_TREE,
			  add_objc_string (DECL_NAME (field_decl),
					   meth_var_names),
			  ivar);
      else
	/* Unnamed bit-field ivar (yuck).  */
        /* APPLE LOCAL begin radar 6285794 */
        ivar = tree_cons (NULL_TREE,
                          convert (string_type_node,
                                   integer_zero_node),
                          ivar);
        /* APPLE LOCAL end radar 6285794 */

      /* Set type.  */
      encode_field_decl (field_decl,
			 obstack_object_size (&util_obstack),
			 OBJC_ENCODE_DONT_INLINE_DEFS);

      /* Null terminate string.  */
      obstack_1grow (&util_obstack, 0);
      ivar
	= tree_cons
	  (NULL_TREE,
	   add_objc_string (get_identifier (obstack_finish (&util_obstack)),
			    meth_var_types),
	   ivar);
      obstack_free (&util_obstack, util_firstobj);

      /* Set offset.  */
      /* LLVM LOCAL - begin make initializer size match type size */
#ifdef ENABLE_LLVM
      ivar = tree_cons (NULL_TREE, convert (integer_type_node,
                                            byte_position (field_decl)), ivar);
#endif
      /* LLVM LOCAL - end make initializer size match type size */
      initlist = tree_cons (NULL_TREE,
			    objc_build_constructor (type, nreverse (ivar)),
			    initlist);
      do
	field_decl = TREE_CHAIN (field_decl);
      while (field_decl && TREE_CODE (field_decl) != FIELD_DECL);
    }
  while (field_decl);

  return objc_build_constructor (build_array_type (type, 0),
				 nreverse (initlist));
}

static tree
generate_ivars_list (tree type, const char *name, int size, tree list)
{
  tree decl, initlist;

  decl = start_var_decl (type, synth_id_with_class_suffix
			       (name, objc_implementation_context));

  initlist = build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, size));
  initlist = tree_cons (NULL_TREE, list, initlist);

  finish_var_decl (decl,
		   objc_build_constructor (TREE_TYPE (decl),
					   nreverse (initlist)));

  return decl;
}

/* Count only the fields occurring in T.  */

static int
ivar_list_length (tree t)
{
  int count = 0;

  for (; t; t = TREE_CHAIN (t))
    if (TREE_CODE (t) == FIELD_DECL)
      ++count;

  return count;
}

/* APPLE LOCAL begin ObjC new abi */

/* This routine outputs the (ivar_reference_offset, offset) tuples. */

static void
generate_v2_ivar_offset_ref_lists (void)
{
  tree chain;

  for (chain = ivar_offset_ref_chain; chain; chain = TREE_CHAIN (chain))
    {
      tree decl = TREE_PURPOSE (chain);
      tree offset = TREE_VALUE (chain);
      finish_var_decl (decl, offset);      
      /* LLVM LOCAL begin - radar 5698757 */
#ifdef ENABLE_LLVM
      /* Reset the initializer for this reference as it may have changed with
         -O0  */
      if (!optimize)
        reset_initializer_llvm (decl);
#endif
      /* LLVM LOCAL end - radar 5698757 */
    }
}

/* This routine declares a static variable of type 'struct ivar_list_t' and initializes
   it. TYPE is 'struct ivar_list_t'. NAME is the suffix for the variable. SIZE is
   number of ivars. LIST is the initializer list for list data member of 
   'struct ivar_list_t'. IVAR_T_SIZE is size of (struct ivar_t). */

static tree
generate_v2_ivars_list (tree type, const char *name, int ivar_t_size,
			    int size, tree list)
{
  tree decl, initlist;
  /* APPLE LOCAL begin radar 6064186 */
  const char *ref_name = synth_id_with_class_suffix (name, objc_implementation_context);
  decl = start_var_decl (type, ref_name);
  set_user_assembler_name (decl, ref_name);
  /* APPLE LOCAL end radar 6064186 */

  initlist = build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, ivar_t_size));

  initlist = tree_cons (NULL_TREE, build_int_cst (NULL_TREE, size), initlist);

  initlist = tree_cons (NULL_TREE, list, initlist);

  finish_var_decl (decl,
		   objc_build_constructor (TREE_TYPE (decl),
					   nreverse (initlist)));

  return decl;
}

/* This is the top-level routine to build and initialize meta-data for both class and
   instance variables.  */

static void
generate_v2_ivar_lists (void)
{
  tree initlist, ivar_list_template, chain;
  int size;

  generating_instance_variables = 1;

  /* build:  struct ivar_t {...}; type if not already done so. */
  if (!objc_v2_ivar_template)
    objc_v2_ivar_template = build_v2_ivar_t_template ();

  /* Currently there is no class ivars and generation of class variables for the root of 
     the inheritance has been removed. It causes multiple defines if there are two root classes
     in the link, because each will define its own identically-named offset variable. */

  UOBJC_V2_CLASS_VARIABLES_decl = 0;

  chain = CLASS_IVARS (implementation_template);
  if (chain)
    {
      /* APPLE LOCAL begin radar 5724385 */
      /* Only named data fields are generated. 'size' is this count. */
      tree field_decl = chain;
      size = 0;
      while (field_decl) {
	if (TREE_CODE (field_decl) == FIELD_DECL && DECL_NAME (field_decl))
	  size++;
	field_decl = TREE_CHAIN (field_decl);
      }
      /* APPLE LOCAL end radar 5724385 */
      ivar_list_template = build_v2_ivar_list_t_template (objc_v2_ivar_template, 
							      size);
      initlist = build_v2_ivar_list_initializer (CLASS_NAME (implementation_template),
						     objc_v2_ivar_template, chain);

      UOBJC_V2_INSTANCE_VARIABLES_decl
	/* APPLE LOCAL radar 6064186 */
        = generate_v2_ivars_list (ivar_list_template, "l_OBJC_$_INSTANCE_VARIABLES",
				      TREE_INT_CST_LOW (
				   	TYPE_SIZE_UNIT (objc_v2_ivar_template)),
                               	      size, initlist);
    }
  else
    UOBJC_V2_INSTANCE_VARIABLES_decl = 0;

  generating_instance_variables = 0;
}

static void
/* APPLE LOCAL end ObjC new abi */
generate_ivar_lists (void)
{
  tree initlist, ivar_list_template, chain;
  int size;

  generating_instance_variables = 1;

  if (!objc_ivar_template)
    objc_ivar_template = build_ivar_template ();

  /* Only generate class variables for the root of the inheritance
     hierarchy since these will be the same for every class.  */

  if (CLASS_SUPER_NAME (implementation_template) == NULL_TREE
      && (chain = TYPE_FIELDS (objc_class_template)))
    {
      size = ivar_list_length (chain);

      ivar_list_template = build_ivar_list_template (objc_ivar_template, size);
      initlist = build_ivar_list_initializer (objc_ivar_template, chain);

      UOBJC_CLASS_VARIABLES_decl
	= generate_ivars_list (ivar_list_template, "_OBJC_CLASS_VARIABLES",
			       size, initlist);
    }
  else
    UOBJC_CLASS_VARIABLES_decl = 0;

  chain = CLASS_IVARS (implementation_template);
  if (chain)
    {
      size = ivar_list_length (chain);
      ivar_list_template = build_ivar_list_template (objc_ivar_template, size);
      initlist = build_ivar_list_initializer (objc_ivar_template, chain);

      UOBJC_INSTANCE_VARIABLES_decl
	= generate_ivars_list (ivar_list_template, "_OBJC_INSTANCE_VARIABLES",
			       size, initlist);
    }
  else
    UOBJC_INSTANCE_VARIABLES_decl = 0;

  generating_instance_variables = 0;
}

static tree
build_dispatch_table_initializer (tree type, tree entries)
{
  tree initlist = NULL_TREE;

  do
    {
      tree elemlist = NULL_TREE;

      elemlist = tree_cons (NULL_TREE,
			    build_selector (METHOD_SEL_NAME (entries)),
			    NULL_TREE);

      /* Generate the method encoding if we don't have one already.  */
      if (! METHOD_ENCODING (entries))
	METHOD_ENCODING (entries) =
	  encode_method_prototype (entries);

      elemlist = tree_cons (NULL_TREE,
			    add_objc_string (METHOD_ENCODING (entries),
					     meth_var_types),
			    elemlist);

      elemlist
	= tree_cons (NULL_TREE,
		     convert (ptr_type_node,
			      build_unary_op (ADDR_EXPR,
					      METHOD_DEFINITION (entries), 1)),
		     elemlist);

      initlist = tree_cons (NULL_TREE,
			    objc_build_constructor (type, nreverse (elemlist)),
			    initlist);

      entries = TREE_CHAIN (entries);
    }
  while (entries);

  return objc_build_constructor (build_array_type (type, 0),
				 nreverse (initlist));
}

/* To accomplish method prototyping without generating all kinds of
   inane warnings, the definition of the dispatch table entries were
   changed from:

	struct objc_method { SEL _cmd; ...; id (*_imp)(); };
   to:
	struct objc_method { SEL _cmd; ...; void *_imp; };  */

static tree
build_method_template (void)
{
  tree _SLT_record;
  tree field_decl, field_decl_chain;

  _SLT_record = start_struct (RECORD_TYPE, get_identifier (UTAG_METHOD));

  /* SEL _cmd; */
  field_decl = create_field_decl (objc_selector_type, "_cmd");
  field_decl_chain = field_decl;

  /* char *method_types; */
  field_decl = create_field_decl (string_type_node, "method_types");
  chainon (field_decl_chain, field_decl);

  /* void *_imp; */
  field_decl = create_field_decl (build_pointer_type (void_type_node),
				  "_imp");
  chainon (field_decl_chain, field_decl);

  finish_struct (_SLT_record, field_decl_chain, NULL_TREE);

  return _SLT_record;
}


static tree
/* APPLE LOCAL ObjC new abi */
generate_dispatch_table (tree type, const char *name, int size, tree list, bool newabi)
{
  tree decl, initlist;
  /* APPLE LOCAL begin ObjC new abi */
  int init_val = newabi 
	         ? TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_method_template)) 
	         : 0;
  /* APPLE LOCAL end ObjC new abi */
  /* APPLE LOCAL begin radar 6064186 */
  const char* ref_name = synth_id_with_class_suffix (name, objc_implementation_context);
  decl = start_var_decl (type, ref_name);
  if (newabi)
    set_user_assembler_name (decl, ref_name);
  /* APPLE LOCAL end radar 6064186 */

  /* LLVM LOCAL - begin make initializer size match type size */
  /* APPLE LOCAL ObjC new abi */
#ifdef OBJCPLUS
  initlist = build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, init_val));
#else
  initlist = build_tree_list (NULL_TREE,
                              build_int_cst (newabi ? NULL_TREE : ptr_type_node,
					     init_val));
#endif
  /* LLVM LOCAL - end make initializer size match type size */
  initlist = tree_cons (NULL_TREE, build_int_cst (NULL_TREE, size), initlist);
  initlist = tree_cons (NULL_TREE, list, initlist);

  finish_var_decl (decl,
		   objc_build_constructor (TREE_TYPE (decl),
					   nreverse (initlist)));

  return decl;
}

static void
mark_referenced_methods (void)
{
  struct imp_entry *impent;
  tree chain;

  for (impent = imp_list; impent; impent = impent->next)
    {
      chain = CLASS_CLS_METHODS (impent->imp_context);
      while (chain)
	{
	  cgraph_mark_needed_node (cgraph_node (METHOD_DEFINITION (chain)));
	  chain = TREE_CHAIN (chain);
	}

      chain = CLASS_NST_METHODS (impent->imp_context);
      while (chain)
	{
	  cgraph_mark_needed_node (cgraph_node (METHOD_DEFINITION (chain)));
	  chain = TREE_CHAIN (chain);
	}
    }
}

static void
generate_dispatch_tables (void)
{
  tree initlist, chain, method_list_template;
  int size;

  if (!objc_method_template)
    objc_method_template = build_method_template ();

  chain = CLASS_CLS_METHODS (objc_implementation_context);
  if (chain)
    {
      size = list_length (chain);

      method_list_template
	= build_method_list_template (objc_method_template, size);
      initlist
	= build_dispatch_table_initializer (objc_method_template, chain);

      /* APPLE LOCAL begin ObjC new abi */
      UOBJC_CLASS_METHODS_decl
	= generate_dispatch_table (method_list_template,
				   ((TREE_CODE (objc_implementation_context)
				     == CLASS_IMPLEMENTATION_TYPE)
				    ? "_OBJC_CLASS_METHODS"
				    : "_OBJC_CATEGORY_CLASS_METHODS"),
				   size, initlist, false);
      /* APPLE LOCAL end ObjC new abi */
    }
  else
    UOBJC_CLASS_METHODS_decl = 0;

  chain = CLASS_NST_METHODS (objc_implementation_context);
  if (chain)
    {
      size = list_length (chain);

      method_list_template
	= build_method_list_template (objc_method_template, size);
      initlist
	= build_dispatch_table_initializer (objc_method_template, chain);

      /* APPLE LOCAL begin ObjC new abi */
      if (TREE_CODE (objc_implementation_context) == CLASS_IMPLEMENTATION_TYPE)
	UOBJC_INSTANCE_METHODS_decl
	  = generate_dispatch_table (method_list_template,
				     "_OBJC_INSTANCE_METHODS",
				     size, initlist, false);
      else
	/* We have a category.  */
	UOBJC_INSTANCE_METHODS_decl
	  = generate_dispatch_table (method_list_template,
				     "_OBJC_CATEGORY_INSTANCE_METHODS",
				     size, initlist, false);
      /* APPLE LOCAL end ObjC new abi */
    }
  else
    UOBJC_INSTANCE_METHODS_decl = 0;
}

/* APPLE LOCAL begin ObjC new abi */
static void
generate_v2_dispatch_tables (void)
{
  tree initlist, chain, method_list_template;
  int size;

  if (!objc_method_template)
    objc_method_template = build_method_template ();

  chain = CLASS_CLS_METHODS (objc_implementation_context);
  if (chain)
    {
      size = list_length (chain);

      method_list_template
	= build_v2_method_list_template (objc_method_template, size);
      initlist
	= build_dispatch_table_initializer (objc_method_template, chain);

      UOBJC_V2_CLASS_METHODS_decl
	= generate_dispatch_table (method_list_template,
				   ((TREE_CODE (objc_implementation_context)
				     == CLASS_IMPLEMENTATION_TYPE)
				    /* APPLE LOCAL begin radar 6064186 */
				    ? "l_OBJC_$_CLASS_METHODS"
				    : "l_OBJC_$_CATEGORY_CLASS_METHODS"),
				    /* APPLE LOCAL end radar 6064186 */
				   size, initlist, true);
    }
  else
    UOBJC_V2_CLASS_METHODS_decl = 0;

  chain = CLASS_NST_METHODS (objc_implementation_context);
  if (chain)
    {
      size = list_length (chain);

      method_list_template
	= build_v2_method_list_template (objc_method_template, size);
      initlist
	= build_dispatch_table_initializer (objc_method_template, chain);

      if (TREE_CODE (objc_implementation_context) == CLASS_IMPLEMENTATION_TYPE)
	UOBJC_V2_INSTANCE_METHODS_decl
	  = generate_dispatch_table (method_list_template,
				     /* APPLE LOCAL radar 6064186 */
				     "l_OBJC_$_INSTANCE_METHODS",
				     size, initlist, true);
      else
	/* We have a category.  */
	UOBJC_V2_INSTANCE_METHODS_decl
	  = generate_dispatch_table (method_list_template,
				     /* APPLE LOCAL radar 6064186 */
				     "l_OBJC_$_CATEGORY_INSTANCE_METHODS",
				     size, initlist, true);
    }
  else
    UOBJC_V2_INSTANCE_METHODS_decl = 0;
}

/* This routine declares a variable to hold meta data for 'struct protocol_list_t'. */

static tree
generate_v2_protocol_list (tree i_or_p)
{
  tree initlist;
  tree refs_decl, lproto, e, plist;
  int size = 0;
  const char *ref_name;

  if (TREE_CODE (i_or_p) == CLASS_INTERFACE_TYPE
      || TREE_CODE (i_or_p) == CATEGORY_INTERFACE_TYPE)
    plist = CLASS_PROTOCOL_LIST (i_or_p);
  else if (TREE_CODE (i_or_p) == PROTOCOL_INTERFACE_TYPE)
    plist = PROTOCOL_LIST (i_or_p);
  else
    abort ();

  /* Compute size.  */
  for (lproto = plist; lproto; lproto = TREE_CHAIN (lproto))
    if (TREE_CODE (TREE_VALUE (lproto)) == PROTOCOL_INTERFACE_TYPE
	&& PROTOCOL_V2_FORWARD_DECL (TREE_VALUE (lproto)))
      size++;

  /* Build initializer.  */
  initlist = NULL_TREE;
  e = build_int_cst (build_pointer_type (objc_v2_protocol_template), size);
  initlist = tree_cons (NULL_TREE, e, initlist);

  for (lproto = plist; lproto; lproto = TREE_CHAIN (lproto))
    {
      tree pval = TREE_VALUE (lproto);

      if (TREE_CODE (pval) == PROTOCOL_INTERFACE_TYPE
	  && PROTOCOL_V2_FORWARD_DECL (pval))
	{
	  e = build_unary_op (ADDR_EXPR, PROTOCOL_V2_FORWARD_DECL (pval), 0);
	  initlist = tree_cons (NULL_TREE, e, initlist);
	}
    }

  /* static struct protocol_list_t *list[size]; */

  /* APPLE LOCAL begin radar 6064186 */
  if (TREE_CODE (i_or_p) == PROTOCOL_INTERFACE_TYPE)
    ref_name = synth_id_with_class_suffix ("l_OBJC_$_PROTOCOL_REFS", i_or_p);
  else if (TREE_CODE (i_or_p) == CLASS_INTERFACE_TYPE)
    ref_name = synth_id_with_class_suffix ("l_OBJC_CLASS_PROTOCOLS_$", i_or_p);
  else if (TREE_CODE (i_or_p) == CATEGORY_INTERFACE_TYPE)
    ref_name = synth_id_with_class_suffix ("l_OBJC_CATEGORY_PROTOCOLS_$", i_or_p);
  else
    abort ();

  refs_decl = start_var_decl
	      (build_array_type
	       (build_pointer_type (objc_v2_protocol_template),
		build_index_type (build_int_cst (NULL_TREE, size))),
	       ref_name);
  set_user_assembler_name (refs_decl, ref_name);
  /* APPLE LOCAL end radar 6064186 */

  finish_var_decl (refs_decl, objc_build_constructor (TREE_TYPE (refs_decl),
  						      nreverse (initlist)));

  return refs_decl;
}

/* This routine builds one 'struct method_t' initializer list. Note that the old ABI 
   is supposed to build 'struct objc_method' which has 3 fields. But it does not 
   build the initialization expression for 'method_imp' which for protocols is NULL
   any way. But to be consistant with declaration of 'struct method_t', in the new 
   ABI we set the method_t.imp to NULL.  */

static tree
build_v2_descriptor_table_initializer (tree type, tree entries)
{
  tree initlist = NULL_TREE;

  do
    {
      tree eltlist
        = tree_cons (NULL_TREE,
                     build_selector (METHOD_SEL_NAME (entries)), NULL_TREE);
      eltlist
        = tree_cons (NULL_TREE,
                     add_objc_string (METHOD_ENCODING (entries),
                                      meth_var_types),
                     eltlist);

      /* APPLE LOCAL begin radar 6285794 */
      eltlist = tree_cons (NULL_TREE, 
                           convert (objc_method_list_ptr,
                                    integer_zero_node),
                           eltlist);
      /* APPLE LOCAL end radar 6285794 */
      initlist
        = tree_cons (NULL_TREE,
                     objc_build_constructor (type, nreverse (eltlist)),
                     initlist);

      entries = TREE_CHAIN (entries);
    }
  while (entries);

  return objc_build_constructor (build_array_type (type, 0),
                                 nreverse (initlist));
}

/* This routine builds instance_methods and class_methods use to declare a 
   protocole. */

static void
/* APPLE LOCAL begin radar 4695109 */
generate_v2_method_descriptors (tree protocol, bool is_optional)
{
  tree initlist, chain, method_list_template;
  int size;

  if (!objc_method_template)
    objc_method_template = build_method_template ();

  chain = is_optional ? PROTOCOL_OPTIONAL_CLS_METHODS (protocol)
		      : PROTOCOL_CLS_METHODS (protocol);
  if (chain)
    {
      size = list_length (chain);

      method_list_template
	= build_v2_method_list_template (objc_method_template, size);

      initlist
	= build_v2_descriptor_table_initializer (objc_method_template, chain);

      if (is_optional)
	UOBJC_PROTOCOL_OPT_CLS_METHODS_decl
	  = generate_descriptor_table (method_list_template,
				       /* APPLE LOCAL radar 6064186 */
				       "l_OBJC_$_PROTOCOL_CLASS_METHODS_OPT",
				       size, initlist, protocol, true);
      else
        UOBJC_V2_CLASS_METHODS_decl
	  = generate_descriptor_table (method_list_template,
				       /* APPLE LOCAL radar 6064186 */
			       	       "l_OBJC_$_PROTOCOL_CLASS_METHODS",
				       size, initlist, protocol, true);
    }
  else
    {
      if (is_optional)
	UOBJC_PROTOCOL_OPT_CLS_METHODS_decl = 0;
      else
        UOBJC_V2_CLASS_METHODS_decl = 0;
    }

  chain = is_optional ? PROTOCOL_OPTIONAL_NST_METHODS (protocol)
		      : PROTOCOL_NST_METHODS (protocol);
  if (chain)
    {
      size = list_length (chain);

      method_list_template
	= build_v2_method_list_template (objc_method_template, size);
      initlist
	= build_v2_descriptor_table_initializer (objc_method_template, chain);

      if (is_optional)
	UOBJC_PROTOCOL_OPT_NST_METHODS_decl
	  = generate_descriptor_table (method_list_template,
				       /* APPLE LOCAL radar 6064186 */
				       "l_OBJC_$_PROTOCOL_INSTANCE_METHODS_OPT",
				       size, initlist, protocol, true);
      else
        UOBJC_V2_INSTANCE_METHODS_decl
	  = generate_descriptor_table (method_list_template,
				       /* APPLE LOCAL radar 6064186 */
				       "l_OBJC_$_PROTOCOL_INSTANCE_METHODS",
				       size, initlist, protocol, true);
    }
  else
    {
      if (is_optional)
	UOBJC_PROTOCOL_OPT_NST_METHODS_decl = 0;
      else
        UOBJC_V2_INSTANCE_METHODS_decl = 0;
    }
}
/* APPLE LOCAL end radar 4695109 */

static tree
/* APPLE LOCAL end ObjC new abi */
generate_protocol_list (tree i_or_p)
{
  tree initlist;
  tree refs_decl, lproto, e, plist;
  int size = 0;
  const char *ref_name;

  if (TREE_CODE (i_or_p) == CLASS_INTERFACE_TYPE
      || TREE_CODE (i_or_p) == CATEGORY_INTERFACE_TYPE)
    plist = CLASS_PROTOCOL_LIST (i_or_p);
  else if (TREE_CODE (i_or_p) == PROTOCOL_INTERFACE_TYPE)
    plist = PROTOCOL_LIST (i_or_p);
  else
    abort ();

  /* Compute size.  */
  for (lproto = plist; lproto; lproto = TREE_CHAIN (lproto))
    if (TREE_CODE (TREE_VALUE (lproto)) == PROTOCOL_INTERFACE_TYPE
	&& PROTOCOL_FORWARD_DECL (TREE_VALUE (lproto)))
      size++;

  /* Build initializer.  */
  /* APPLE LOCAL radar 6285794 */
  initlist = tree_cons (NULL_TREE, integer_zero_node, NULL_TREE);
  e = build_int_cst (build_pointer_type (objc_protocol_template), size);
  initlist = tree_cons (NULL_TREE, e, initlist);

  for (lproto = plist; lproto; lproto = TREE_CHAIN (lproto))
    {
      tree pval = TREE_VALUE (lproto);

      if (TREE_CODE (pval) == PROTOCOL_INTERFACE_TYPE
	  && PROTOCOL_FORWARD_DECL (pval))
	{
	  e = build_unary_op (ADDR_EXPR, PROTOCOL_FORWARD_DECL (pval), 0);
	  initlist = tree_cons (NULL_TREE, e, initlist);
	}
    }

  /* static struct objc_protocol *refs[n]; */

  if (TREE_CODE (i_or_p) == PROTOCOL_INTERFACE_TYPE)
    ref_name = synth_id_with_class_suffix ("_OBJC_PROTOCOL_REFS", i_or_p);
  else if (TREE_CODE (i_or_p) == CLASS_INTERFACE_TYPE)
    ref_name = synth_id_with_class_suffix ("_OBJC_CLASS_PROTOCOLS", i_or_p);
  else if (TREE_CODE (i_or_p) == CATEGORY_INTERFACE_TYPE)
    ref_name = synth_id_with_class_suffix ("_OBJC_CATEGORY_PROTOCOLS", i_or_p);
  else
    abort ();

  refs_decl = start_var_decl
	      (build_array_type
	       (build_pointer_type (objc_protocol_template),
		build_index_type (build_int_cst (NULL_TREE, size + 2))),
	       ref_name);

  finish_var_decl (refs_decl, objc_build_constructor (TREE_TYPE (refs_decl),
  						      nreverse (initlist)));

  return refs_decl;
}

static tree
build_category_initializer (tree type, tree cat_name, tree class_name,
			    tree instance_methods, tree class_methods,
			    /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
			    tree protocol_list,
			    bool abi_v2, tree property_list)
			    /* APPLE LOCAL end C* property metadata (Radar 4498373) */
{
  tree initlist = NULL_TREE, expr;

  initlist = tree_cons (NULL_TREE, cat_name, initlist);
  initlist = tree_cons (NULL_TREE, class_name, initlist);

  if (!instance_methods)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_method_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_method_list_ptr,
		      build_unary_op (ADDR_EXPR, instance_methods, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }
  if (!class_methods)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_method_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_method_list_ptr,
		      build_unary_op (ADDR_EXPR, class_methods, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  /* protocol_list = */
  /* APPLE LOCAL begin radar 6285794 */
  {
    tree protocol_list_ptr =
      (abi_v2) ? build_pointer_type (objc_v2_protocol_template)
               : build_pointer_type (build_pointer_type (objc_protocol_template));
    
    if (!protocol_list)
      initlist = tree_cons (NULL_TREE, 
                            convert (protocol_list_ptr,
                                     integer_zero_node),
                            initlist);
    else
      {
        /* APPLE LOCAL begin radar 4533974 - ObjC new protocol */
        expr = convert (protocol_list_ptr, build_unary_op (ADDR_EXPR, protocol_list, 0));
        /* APPLE LOCAL end radar 4533974 - ObjC new protocol */
        initlist = tree_cons (NULL_TREE, expr, initlist);
      }
  }
  /* APPLE LOCAL end radar 6285794 */
  /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
  if (!abi_v2)
    {
      /* uint32_t size; */
      int val = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_category_template));
      expr = build_int_cst (NULL_TREE, val);
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }
  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
  /* struct _objc_property_list *instance_properties; */
  if (!property_list)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE, 
                          convert (objc_prop_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_prop_list_ptr,
		      build_unary_op (ADDR_EXPR, property_list, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
  /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */
  return objc_build_constructor (type, nreverse (initlist));
}

/* struct _objc_class {
     struct objc_class *isa;
     struct objc_class *super_class;
     char *name;
     long version;
     long info;
     long instance_size;
     struct objc_ivar_list *ivars;
     struct objc_method_list *methods;
     if (flag_next_runtime)
       struct objc_cache *cache;
     else {
       struct sarray *dtable;
       struct objc_class *subclass_list;
       struct objc_class *sibling_class;
     }
     struct objc_protocol_list *protocols;
     if (flag_next_runtime)
       void *sel_id;
     void *gc_object_type;
   };  */

static tree
build_shared_structure_initializer (tree type, tree isa, tree super,
				    tree name, tree size, int status,
				    tree dispatch_table, tree ivar_list,
	        /* APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions */
				    tree protocol_list, tree objc_class_ext)
{
  tree initlist = NULL_TREE, expr;

  /* isa = */
  initlist = tree_cons (NULL_TREE, isa, initlist);

  /* super_class = */
  initlist = tree_cons (NULL_TREE, super, initlist);

  /* name = */
  initlist = tree_cons (NULL_TREE, default_conversion (name), initlist);

  /* version = */
  initlist = tree_cons (NULL_TREE, build_int_cst (long_integer_type_node, 0),
			initlist);

  /* info = */
  initlist = tree_cons (NULL_TREE,
			build_int_cst (long_integer_type_node, status),
			initlist);

  /* instance_size = */
  initlist = tree_cons (NULL_TREE, convert (long_integer_type_node, size),
			initlist);

  /* objc_ivar_list = */
  if (!ivar_list)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE, 
                          convert (objc_ivar_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_ivar_list_ptr,
		      build_unary_op (ADDR_EXPR, ivar_list, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  /* objc_method_list = */
  if (!dispatch_table)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_method_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_method_list_ptr,
		      build_unary_op (ADDR_EXPR, dispatch_table, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  /* APPLE LOCAL begin radar 6285794 */
  if (flag_next_runtime)
    /* method_cache = */
    initlist = tree_cons (NULL_TREE,
                          convert (build_pointer_type
                                   (xref_tag (RECORD_TYPE,
                                              get_identifier
                                              ("objc_cache"))),
                                   integer_zero_node),
                          initlist);
  else
    {
      tree null_objc_class_ptr = 
        convert (build_pointer_type (objc_class_template),
                 integer_zero_node);
 
      /* dtable = */
      initlist = tree_cons (NULL_TREE,
                            convert (build_pointer_type (xref_tag (RECORD_TYPE,
                                                                   get_identifier
                                                                   ("sarray"))),
                                     integer_zero_node),
                            initlist);
 
      /* subclass_list = */
      initlist = tree_cons (NULL_TREE, null_objc_class_ptr, initlist);
 
      /* sibling_class = */
      initlist = tree_cons (NULL_TREE, null_objc_class_ptr, initlist);
    }
    /* APPLE LOCAL end radar 6285794 */

  /* protocol_list = */
  /* APPLE LOCAL begin radar 6285794 */
  {
    tree objc_protocol_ptr_ptr =
      build_pointer_type (build_pointer_type (objc_protocol_template));
 
    if (!protocol_list)
      initlist = tree_cons (NULL_TREE,
                            convert (objc_protocol_ptr_ptr,
                                     integer_zero_node),
                            initlist);
    else
      {
        expr = convert (objc_protocol_ptr_ptr,
                        build_unary_op (ADDR_EXPR, protocol_list, 0));
        initlist = tree_cons (NULL_TREE, expr, initlist);
      }
  }
  /* APPLE LOCAL end radar 6285794 */

  /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
  if (flag_next_runtime)
    {
      /* const char *ivar_layout; */
      if (IS_CLS_META (status)) /* Meta Class ? */
 	/* APPLE LOCAL begin radar 6285794 */
        initlist = tree_cons (NULL_TREE,
                              convert (string_type_node,
                                       integer_zero_node),
                              initlist);
 	/* APPLE LOCAL end radar 6285794 */
      else
	{
	  tree ivar_layout = objc_build_ivar_layout (true);
	  if (!ivar_layout)
            /* APPLE LOCAL begin radar 6285794 */
            initlist = tree_cons (NULL_TREE,
                                  convert (string_type_node,
                                           integer_zero_node),
                                  initlist);
 	    /* APPLE LOCAL end radar 6285794 */
	  else
	    initlist = tree_cons (NULL_TREE, ivar_layout, initlist);
	}
      /* struct _objc_class_ext *ext; */
      /* APPLE LOCAL begin radar 6285794 */
      if (!objc_class_ext_template)
        build_objc_class_ext_template ();

      {
        tree objc_class_ext_ptr =
          build_pointer_type (objc_class_ext_template);
 
        if (!objc_class_ext)
          initlist = tree_cons (NULL_TREE,
                                convert (objc_class_ext_ptr,
                                         integer_zero_node),
                                initlist);
        else
          {
            expr = convert (objc_class_ext_ptr,
                            build_unary_op (ADDR_EXPR, objc_class_ext, 0));
            initlist = tree_cons (NULL_TREE, expr, initlist);
          }
      }
      /* APPLE LOCAL end radar 6285794 */
    }
  else
    /* gc_object_type = NULL */
    /* APPLE LOCAL radar 6285794 - NUL pointer */
    initlist = tree_cons (NULL_TREE, null_pointer_node, initlist);
  /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */

  return objc_build_constructor (type, nreverse (initlist));
}

/* Retrieve category interface CAT_NAME (if any) associated with CLASS.  */

static inline tree
lookup_category (tree class, tree cat_name)
{
  tree category = CLASS_CATEGORY_LIST (class);

  while (category && CLASS_SUPER_NAME (category) != cat_name)
    category = CLASS_CATEGORY_LIST (category);
  return category;
}

/* static struct objc_category _OBJC_CATEGORY_<name> = { ... };  */

static void
/* APPLE LOCAL radar 4349690 */
generate_category (tree cat, struct imp_entry *impent)
{
  tree decl;
  tree initlist, cat_name_expr, class_name_expr;
  tree protocol_decl, category;

  add_class_reference (CLASS_NAME (cat));
  cat_name_expr = add_objc_string (CLASS_SUPER_NAME (cat), class_names);

  class_name_expr = add_objc_string (CLASS_NAME (cat), class_names);

  category = lookup_category (implementation_template,
				CLASS_SUPER_NAME (cat));

  if (category && CLASS_PROTOCOL_LIST (category))
    {
      generate_protocol_references (CLASS_PROTOCOL_LIST (category));
      protocol_decl = generate_protocol_list (category);
    }
  else
    protocol_decl = 0;

  /* APPLE LOCAL radar 4349690 */
  decl = update_var_decl(impent->class_decl);

  initlist = build_category_initializer (TREE_TYPE (decl),
					 cat_name_expr, class_name_expr,
					 UOBJC_INSTANCE_METHODS_decl,
					 UOBJC_CLASS_METHODS_decl,
/* APPLE LOCAL begin C* property metadata (Radar 4498373) */
					 protocol_decl,
/* APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions */
					 false, UOBJC_V2_PROPERTY_decl);

  finish_var_decl (decl, initlist);
}
/* APPLE LOCAL end C* property metadata (Radar 4498373) */
/* APPLE LOCAL begin ObjC new abi */

/* APPLE LOCAL begin radar 4698856 */
/* Routine to build UOBJC_V2_CACHE_decl and UOBJC_V2_VTABLE_decl,
   if need be.
*/

static void
build_empty_cache_vtable_vars (void)
{
  UOBJC_V2_CACHE_decl = lookup_name (get_identifier ("_objc_empty_cache"));
  if (!UOBJC_V2_CACHE_decl)
    UOBJC_V2_CACHE_decl = create_extern_decl (ptr_type_node, "_objc_empty_cache");
  UOBJC_V2_VTABLE_decl = lookup_name (get_identifier ("_objc_empty_vtable"));
  if (!UOBJC_V2_VTABLE_decl)
    UOBJC_V2_VTABLE_decl = create_extern_decl (objc_imp_type, "_objc_empty_vtable");
}
/* APPLE LOCAL end radar 4698856 */

/* static struct category_t _OBJC_CATEGORY_$_<name> = { ... };  */

static void
generate_v2_category (tree cat, struct imp_entry *impent)
{
  tree decl;
  tree initlist, cat_name_expr, class_name_expr;
  tree protocol_decl, category;

  cat_name_expr = add_objc_string (CLASS_SUPER_NAME (cat), class_names);

  (void)objc_v2_get_class_reference (CLASS_NAME (cat));

  class_name_expr = create_extern_decl (objc_v2_class_template,
				        objc_build_internal_classname (
					  CLASS_NAME (cat), false));
  class_name_expr = build_fold_addr_expr (class_name_expr);

  category = lookup_category (implementation_template,
				CLASS_SUPER_NAME (cat));

  if (category && CLASS_PROTOCOL_LIST (category))
    {
      /* APPLE LOCAL radar 4695109 */
      generate_v2_protocol_references (CLASS_PROTOCOL_LIST (category));
      protocol_decl = generate_v2_protocol_list (category);
    }
  else
    protocol_decl = 0;

  decl = update_var_decl(impent->class_v2_decl);

  initlist = build_category_initializer (TREE_TYPE (decl),
					 cat_name_expr, class_name_expr,
					 UOBJC_V2_INSTANCE_METHODS_decl,
					 UOBJC_V2_CLASS_METHODS_decl,
  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
					 protocol_decl,
					 true, UOBJC_V2_PROPERTY_decl);
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */

  finish_var_decl (decl, initlist);
  
  /* Add to list of pointers in __category_list section */
  objc_add_to_category_list_chain (decl);
  if (CLASS_OR_CATEGORY_HAS_LOAD_IMPL (objc_implementation_context) != NULL_TREE)
    objc_add_to_nonlazy_category_list_chain (decl);
}

/* Build the name for object of type struct class_ro_t */

static const char *
newabi_append_ro (const char *name)
{
  char *dollar;
  char *p;
  static char string[BUFSIZE];
  dollar = strchr (name, '$');
  gcc_assert (dollar);
  p = string;
  /* APPLE LOCAL radar 6064186 */
  *p = 'l'; p++;
  *p = '_'; p++;
  strncpy (p, name, (int)(dollar - name));
  p += (int)(dollar - name);
  sprintf (p, "RO_%s", dollar);
  return string;
}
 
/* Routine to build initializer list to initialize objects of type struct class_t; */

static tree
build_class_t_initializer (tree type, tree isa, tree superclass, tree ro, tree cache, tree vtable)
{
  tree initlist = NULL_TREE;

  /* isa */
  initlist = tree_cons (NULL_TREE, isa, initlist);

  /* superclass */
  initlist = tree_cons (NULL_TREE, superclass, initlist);

  /* cache */
  if (cache)
    initlist = tree_cons (NULL_TREE, cache, initlist);
  else
    /* APPLE LOCAL radar 6285794 - NUL pointer */
    initlist = tree_cons (NULL_TREE, null_pointer_node, initlist);

  /* vtable */
  if (vtable)
    initlist = tree_cons (NULL_TREE, vtable, initlist);
  else
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (build_pointer_type (objc_imp_type),
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */

  /* ro */
  initlist = tree_cons (NULL_TREE, ro, initlist);

  return objc_build_constructor (type, nreverse (initlist));
}

static tree
build_class_ro_t_initializer (tree type, tree name, 
			      unsigned int flags, unsigned int instanceStart, unsigned int instanceSize,
			      /* APPLE LOCAL radar 4695101 */
			      tree ivarLayout, 
			      tree baseMethods, tree baseProtocols, tree ivars,
			      /* APPLE LOCAL radar 4695101 */
			      tree weakIvarLayout,
			      tree property_list)
{
  tree initlist = NULL_TREE, expr;

  /* flags */
  initlist = tree_cons (NULL_TREE, 
		 	build_int_cst (integer_type_node, flags), 
			initlist);  

  /* instanceStart */
  initlist = tree_cons (NULL_TREE, 
		 	build_int_cst (integer_type_node, instanceStart), 
			initlist);  

  /* instanceSize */
  initlist = tree_cons (NULL_TREE, 
		 	build_int_cst (integer_type_node, instanceSize), 
			initlist);  

  /* reserved */
  if (TARGET_64BIT)
    initlist = tree_cons (NULL_TREE, 
		 	  build_int_cst (integer_type_node, 0), 
			  initlist);  

  /* APPLE LOCAL begin radar 4695101 */
  /* ivarLayout */
  if (!ivarLayout)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (string_type_node,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    initlist = tree_cons (NULL_TREE, ivarLayout, initlist);
  /* APPLE LOCAL end radar 4695101 */

  /* name */
  initlist = tree_cons (NULL_TREE, default_conversion (name), initlist);

  /* baseMethods */
  if (!baseMethods)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_method_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_method_list_ptr,
                      build_unary_op (ADDR_EXPR, baseMethods, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  /* baseProtocols */
  /* APPLE LOCAL begin radar 6285794 */
  {
    tree protocol_list_t_p =
      build_pointer_type (xref_tag (RECORD_TYPE, 
                                    get_identifier (UTAG_V2_PROTOCOL_LIST)));

    if (!baseProtocols)
      initlist = tree_cons (NULL_TREE,
                            convert (protocol_list_t_p,
                                     integer_zero_node),
                            initlist);
    else
      {
        expr = convert (protocol_list_t_p,
                        build_unary_op (ADDR_EXPR, baseProtocols, 0));
        initlist = tree_cons (NULL_TREE, expr, initlist);
      }
  }
  /* APPLE LOCAL end radar 6285794 */

  /* ivars */
  if (!ivars)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_v2_ivar_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_v2_ivar_list_ptr,
                      build_unary_op (ADDR_EXPR, ivars, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }

  /* APPLE LOCAL begin radar 4695101 */
  /* weakIvarLayout */
  if (!weakIvarLayout)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (string_type_node,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    initlist = tree_cons (NULL_TREE, weakIvarLayout, initlist);
  /* APPLE LOCAL end radar 4695101 */

  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
  /* property list */
  if (!property_list)
    /* APPLE LOCAL begin radar 6285794 */
    initlist = tree_cons (NULL_TREE,
                          convert (objc_prop_list_ptr,
                                   integer_zero_node),
                          initlist);
    /* APPLE LOCAL end radar 6285794 */
  else
    {
      expr = convert (objc_prop_list_ptr,
                      build_unary_op (ADDR_EXPR, property_list, 0));
      initlist = tree_cons (NULL_TREE, expr, initlist);
    }
  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
  return objc_build_constructor (type, nreverse (initlist));
}

/* APPLE LOCAL begin radar 4533974 - ObjC new protocol  - radar 4695109 */
/* Add the local protocol meta-data declaration to the list which later on ends up 
   in the __protocol_list section. */

static void
/* APPLE LOCAL begin radar 6351990 */
objc_add_to_protocol_list_chain (tree protocol_interface_decl, tree protocol_decl)
{
  tree *chain;
  for (chain = &protocol_list_chain; *chain; chain = &TREE_CHAIN (*chain))
    ;
  *chain = tree_cons (protocol_interface_decl, protocol_decl, NULL_TREE);
}

/* Build the __protocol_list section table containing address of all generate protocol_t 
   meta-data. */

static void
build_protocol_list_address_table (void)
{
  tree chain;
  tree list_chain = protocol_list_chain;
  char *string = NULL;
  unsigned int  buf_size = 0;

  for (chain = list_chain; chain; chain = TREE_CHAIN (chain))
    {
      tree decl = TREE_PURPOSE (chain);
      tree expr = TREE_VALUE (chain);
      gcc_assert (decl && TREE_CODE (decl) == PROTOCOL_INTERFACE_TYPE);
      if ((strlen ("l_OBJC_LABEL_PROTOCOL_$_") + strlen (IDENTIFIER_POINTER (PROTOCOL_NAME (decl))) + 1) > buf_size)
        {
	  if (!buf_size)
	    buf_size = BUFSIZE;
	  else
	    buf_size = strlen ("l_OBJC_LABEL_PROTOCOL_$_") + strlen (IDENTIFIER_POINTER (PROTOCOL_NAME (decl))) + 1;
	  string = (char *)alloca (buf_size);
	} 
      sprintf (string,  "l_OBJC_LABEL_PROTOCOL_$_%s", IDENTIFIER_POINTER (PROTOCOL_NAME (decl)));
      decl = create_hidden_decl (objc_protocol_type, string);
      DECL_WEAK (decl) = 1;
      set_user_assembler_name (decl, string);
      /* LLVM LOCAL begin 7069676 */
#ifdef ENABLE_LLVM
      /* Let optimizer know that this decl is not removable.  */
      DECL_PRESERVE_P (decl) = 1;
#endif
      /* LLVM LOCAL end 7069676 */
      expr = convert (objc_protocol_type, build_fold_addr_expr (expr));
      /* APPLE LOCAL radar 4561192 */
      objc_set_alignment_attribute (decl, objc_protocol_type);
      finish_var_decl (decl, expr);
    }
}
/* APPLE LOCAL end radar 6351990 */

/* Build decl = initializer; for each protocol referenced in @protocol(MyProtole) expression. */

static void
build_protocollist_translation_table (void)
{
  tree chain;
  static char string[BUFSIZE];

  for (chain = protocollist_ref_chain; chain; chain = TREE_CHAIN (chain))
    {
      tree expr = TREE_VALUE (chain);
      tree decl = TREE_PURPOSE (chain);
      gcc_assert (TREE_CODE (expr) == PROTOCOL_INTERFACE_TYPE);
      /* APPLE LOCAL begin radar 4695109 */
      /* LLVM LOCAL begin - radar 5476262 */
#ifdef ENABLE_LLVM
      if (flag_objc_abi == 2)
        /* LLVM LOCAL - add 'l' prefix */
        sprintf (string, "l_OBJC_PROTOCOL_$_%s", 
                 IDENTIFIER_POINTER (PROTOCOL_NAME (expr)));
      else
          /* LLVM LOCAL - add 'L' prefix */
          sprintf (string, "L_OBJC_PROTOCOL_$_%s", 
                   IDENTIFIER_POINTER (PROTOCOL_NAME (expr)));

      expr = lookup_name (get_identifier(string));

      if (expr == NULL_TREE)
        /* LLVM LOCAL - &string[1] because of '[Ll]' prefix */
	expr = start_var_decl (objc_v2_protocol_template, &string[1]);
#else
      /* APPLE LOCAL begin radar 6255913 */
      if (flag_objc_abi == 2)
      {
        sprintf (string, "l_OBJC_PROTOCOL_$_%s", 
	         IDENTIFIER_POINTER (PROTOCOL_NAME (expr)));
        expr = start_var_decl (objc_v2_protocol_template, string);
	set_user_assembler_name (expr, string);
      }
      /* APPLE LOCAL end radar 6255913 */
      else
      {
        sprintf (string, "_OBJC_PROTOCOL_$_%s", 
	         IDENTIFIER_POINTER (PROTOCOL_NAME (expr)));
        expr = start_var_decl (objc_v2_protocol_template, string);
      }
#endif
      /* LLVM LOCAL end - radar 5476262 */
      /* APPLE LOCAL end radar 4695109 */
      expr = convert (objc_protocol_type, build_fold_addr_expr (expr));
      finish_var_decl (decl, expr);
      /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
      /* At -O0, we may have emitted references to the decl earlier. */
      if (!optimize)
        reset_initializer_llvm(decl);
#endif
      /* LLVM LOCAL end */
    }
}

/* This routine builds the protocol_reference_chain for each protocol name used
   @protocol(MyProtocol) expression. IDENT is current protocol name.  */

static tree
objc_v2_get_protocol_reference (tree ident)
{
  tree *chain;
  tree decl;

  for (chain = &protocollist_ref_chain; *chain; chain = &TREE_CHAIN (*chain))
    if (TREE_VALUE (*chain) == ident)
      {
        if (! TREE_PURPOSE (*chain))
	  /* APPLE LOCAL radar 6351990 */
          TREE_PURPOSE (*chain) = build_protocollist_reference_decl (ident);

        return TREE_PURPOSE (*chain);
      }

  /* APPLE LOCAL radar 6351990 */
  decl = build_protocollist_reference_decl (ident);
  *chain = tree_cons (decl, ident, NULL_TREE);
  return decl;
}

/* This routine creates a static variable used to implement @protocol(MyProtocol) 
   expression. This variable will be initialized to global protocol_t meta-data
   pointer. */

/* APPLE LOCAL begin radar 6351990 */
static tree
build_protocollist_reference_decl (tree protocol)
{
  tree decl;
  tree protocol_ident = PROTOCOL_NAME (protocol);
  char *buf = (char *)alloca (strlen ("l_OBJC_PROTOCOL_REFERENCE_$_") + 
	       		      IDENTIFIER_LENGTH (protocol_ident) + 1);

  sprintf (buf, "l_OBJC_PROTOCOL_REFERENCE_$_%s", IDENTIFIER_POINTER (protocol_ident));
  decl = create_hidden_decl (objc_protocol_type, buf);
  DECL_WEAK (decl) = 1;
  set_user_assembler_name (decl, buf);
  /* LLVM LOCAL begin 7069676 */
#ifdef ENABLE_LLVM
  /* Let optimizer know that this decl is not removable.  */
  DECL_PRESERVE_P (decl) = 1;
#endif
  /* LLVM LOCAL end 7069676 */
  return decl;
}
/* APPLE LOCAL end radar 6351990 */
/* APPLE LOCAL end radar 4533974 - ObjC new protocol */

/* Add the global class meta-data declaration to the list which later on ends up 
   in the __nonlazy_class section. */

static void
objc_add_to_nonlazy_class_list_chain (tree global_class_decl)
{
  tree *chain;
  for (chain = &nonlazy_class_list_chain; *chain; chain = &TREE_CHAIN (*chain))
    ;
  *chain = tree_cons (NULL_TREE, global_class_decl, NULL_TREE);
}

/* Add the global class meta-data declaration to the list which later on ends up 
   in the __class_list section. */

static void
objc_add_to_class_list_chain (tree global_class_decl)
{
  tree *chain;
  for (chain = &class_list_chain; *chain; chain = &TREE_CHAIN (*chain))
    ;
  *chain = tree_cons (NULL_TREE, global_class_decl, NULL_TREE);
}

/* Add the category meta-data declaration to the list which later on ends up 
   in the __nonlazy_category section. */

static void
objc_add_to_nonlazy_category_list_chain (tree global_category_decl)
{
  tree *chain;
  for (chain = &nonlazy_category_list_chain; *chain; chain = &TREE_CHAIN (*chain))
    ;
  *chain = tree_cons (NULL_TREE, global_category_decl, NULL_TREE);
}

/* Add the category meta-data declaration to the list which later on ends up 
   in the __category_list section. */

static void
objc_add_to_category_list_chain (tree global_category_decl)
{
  tree *chain;
  for (chain = &category_list_chain; *chain; chain = &TREE_CHAIN (*chain))
    ;
  *chain = tree_cons (NULL_TREE, global_category_decl, NULL_TREE);
}

/* Routine to build object of struct class_ro_t { ... }; */

static void
generate_v2_shared_structures (int cls_flags)
{
  tree sc_spec, decl_specs, decl;
  tree name_expr, root_expr = NULL_TREE;
  tree my_super_id = NULL_TREE;
  tree initlist, protocol_decl;
  unsigned int instanceStart, instanceSize;
  /* APPLE LOCAL begin radar 4695101 */
  tree ivarLayout;
  tree weakIvarLayout;
  /* APPLE LOCAL end radar 4695101 */
  tree metaclass_decl, class_decl;
  tree field, firstIvar;
  tree class_superclass_expr, metaclass_superclass_expr;
  unsigned int flags = 0x1; /* Start with CLS_META */
  /* APPLE LOCAL radar 6064186 */
  const char* ref_name;
  /* APPLE LOCAL begin radar 4705298, 4843145 */
  class_decl = objc_create_global_decl_for_class (
		 objc_v2_class_template, 
		 IDENTIFIER_POINTER (DECL_NAME (UOBJC_V2_CLASS_decl)), 
		 implementation_template);
  metaclass_decl = objc_create_global_decl_for_class (
		     objc_v2_class_template, 
		     IDENTIFIER_POINTER (DECL_NAME (UOBJC_V2_METACLASS_decl)), 
		     implementation_template);

  if (DECL_VISIBILITY (class_decl) == VISIBILITY_HIDDEN)
    flags |= OBJC2_CLS_HIDDEN; 
  /* APPLE LOCAL end radar 4705298, 4843145*/

  /* Generation of data for meta class */
  my_super_id = CLASS_SUPER_NAME (implementation_template);
  if (my_super_id)
    {
      /* compute reference to root's name. For meta class, "isa" is reference 
	 to root class name. */
      tree sav;
      tree my_root_id = my_super_id;
      tree my_root_int;
      tree interface;
      do
        {
          my_root_int = lookup_interface (my_root_id);

          if (my_root_int && CLASS_SUPER_NAME (my_root_int))
            my_root_id = CLASS_SUPER_NAME (my_root_int);
          else
            break;
        }
      while (1);
      sav = objc_implementation_context;
      objc_implementation_context = my_root_int;
      /* APPLE LOCAL radar 5202926 */
      root_expr = build_metadata_decl ("OBJC_METACLASS_$", objc_v2_class_template, true);
      /* LLVM LOCAL begin - radar 5658734 */
#ifdef ENABLE_LLVM
      DECL_EXTERNAL(root_expr) = 1;
#endif
      /* LLVM LOCAL end - radar 5658734 */
      root_expr = build_fold_addr_expr (root_expr);

      /* Install class `isa' and `super' pointers at runtime.  */
      interface = lookup_interface (my_super_id);
      gcc_assert (interface);
      objc_implementation_context = interface;
      /* Note! I had to remove '_' prefix to 'OBJC' to make this an extern symbol. Darwin's
         back-end, recognizes '_OBJC_' prefix and prepends an 'L' in front of this. Darwin
         assembler treats names starting with 'L_' as local symbols. */
      /* APPLE LOCAL radar 5202926 */
      class_superclass_expr = build_metadata_decl ("OBJC_CLASS_$", objc_v2_class_template, true);
      /* LLVM LOCAL begin - radar 5658734 */
#ifdef ENABLE_LLVM
      DECL_EXTERNAL(class_superclass_expr) = 1;
#endif
      /* LLVM LOCAL end - radar 5658734 */
      class_superclass_expr = build_fold_addr_expr (class_superclass_expr);
      /* APPLE LOCAL radar 5202926 */
      metaclass_superclass_expr = build_metadata_decl ("OBJC_METACLASS_$", objc_v2_class_template, true);
      /* LLVM LOCAL begin - radar 5658734 */
#ifdef ENABLE_LLVM
      DECL_EXTERNAL(metaclass_superclass_expr) = 1;
#endif
      /* LLVM LOCAL end - radar 5658734 */
      metaclass_superclass_expr = build_fold_addr_expr (metaclass_superclass_expr);
      objc_implementation_context = sav;
    }
  else
    {
      /* root class.  */
      root_expr = build_unary_op (ADDR_EXPR, metaclass_decl, 0);
      metaclass_superclass_expr = build_unary_op (ADDR_EXPR, class_decl, 0);
      /* APPLE LOCAL begin radar 6285794 */
      class_superclass_expr = convert (build_pointer_type (objc_v2_class_template),
                                       integer_zero_node);
      /* APPLE LOCAL end radar 6285794 */
      flags |= 0x2; /* CLS_ROOT: it is also a root meta class */
    }


  if (CLASS_PROTOCOL_LIST (implementation_template))
    {
      /* APPLE LOCAL begin radar 4695109 */
      generate_v2_protocol_references
        (CLASS_PROTOCOL_LIST (implementation_template));
      /* APPLE LOCAL end radar 4695109 */
      protocol_decl = generate_v2_protocol_list (implementation_template);
    }
  else
    protocol_decl = 0;

  /* static struct class_ro_t  _OBJC_METACLASS_Foo = { ... }; */

  /* APPLE LOCAL begin radar 4702359 */
  instanceStart = TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_v2_class_template));
  instanceSize =  TREE_INT_CST_LOW (TYPE_SIZE_UNIT (objc_v2_class_template)); 
  /* APPLE LOCAL end radar 4702359 */

  name_expr = add_objc_string (CLASS_NAME (implementation_template),
                               class_names);

  sc_spec = build_tree_list (NULL_TREE, ridpointers[(int) RID_STATIC]);
  decl_specs = tree_cons (NULL_TREE, objc_v2_class_ro_template, sc_spec);

  /* APPLE LOCAL begin radar 6064186 */
  ref_name = newabi_append_ro (
	       IDENTIFIER_POINTER (DECL_NAME (UOBJC_V2_METACLASS_decl))); 
  decl = start_var_decl (objc_v2_class_ro_template, ref_name);
  set_user_assembler_name (decl, ref_name);
  /* APPLE LOCAL end radar 6064186 */

  initlist = build_class_ro_t_initializer
	       (TREE_TYPE (decl),
		name_expr,
		(flags | cls_flags), instanceStart, instanceSize, 
		/* APPLE LOCAL radar 4695101 */
		NULL_TREE, UOBJC_V2_CLASS_METHODS_decl, protocol_decl,
		UOBJC_V2_CLASS_VARIABLES_decl,
		/* APPLE LOCAL radar 4695101 */
		NULL_TREE,
		NULL_TREE);

  finish_var_decl (decl, initlist);

  /* APPLE LOCAL radar 4698856*/
  build_empty_cache_vtable_vars ();

  /* static struct class_t _OBJC_METACLASS_Foo = { ... }; */
  initlist = build_class_t_initializer (TREE_TYPE (metaclass_decl),
					root_expr,
					metaclass_superclass_expr,
					build_fold_addr_expr (decl),
					build_fold_addr_expr (UOBJC_V2_CACHE_decl), 
					build_fold_addr_expr (UOBJC_V2_VTABLE_decl));
  finish_var_decl (metaclass_decl, initlist);

  /* Generation of data for the class */

  flags = 0x0;		/* CLS */
  /* APPLE LOCAL begin radar 4705298, 4843145 */
  if (DECL_VISIBILITY (class_decl) == VISIBILITY_HIDDEN)
    flags |= OBJC2_CLS_HIDDEN;
  /* APPLE LOCAL end radar 4705298, 48431458 */
  if (!my_super_id)
    flags |= 0x2;	/* CLS_ROOT: this is a root class */
  /* APPLE LOCAL begin radar 5008110 */
  if (objc2_objc_exception_attr (implementation_template))
    flags |= 0x20;
  /* APPLE LOCAL end radar 5008110 */

  /* Compute instanceStart */
  gcc_assert (CLASS_TYPE (implementation_template));
  field = TYPE_FIELDS (CLASS_TYPE (implementation_template));
  /* APPLE LOCAL begin radar 5333233 */
  /* If class has no ivar, 'field' remains the super class hidden field. */
  if (UOBJC_V2_INSTANCE_VARIABLES_decl 
      && my_super_id && field && TREE_CHAIN (field))
  /* APPLE LOCAL end radar 5333233 */
    field = TREE_CHAIN (field);

  firstIvar = field;

  while (firstIvar && TREE_CODE (firstIvar) != FIELD_DECL)
    firstIvar = TREE_CHAIN (firstIvar);
  gcc_assert (UOBJC_V2_INSTANCE_VARIABLES_decl? (firstIvar != NULL_TREE): true);

  /* Compute instanceSize */
  while (field && TREE_CHAIN (field)
         && TREE_CODE (TREE_CHAIN (field)) == FIELD_DECL)
    field = TREE_CHAIN (field);
  
  if (field && TREE_CODE (field) == FIELD_DECL)
    instanceSize = int_byte_position (field) * BITS_PER_UNIT + tree_low_cst (DECL_SIZE (field), 0);
  else
    instanceSize = 0;
  instanceSize /= BITS_PER_UNIT;

  /* If the class has no ivars, instanceStart should be set to the superclass's 
     instanceSize */
  /* APPLE LOCAL begin ARM signedness mismatch */
  instanceStart = UOBJC_V2_INSTANCE_VARIABLES_decl
		  ? (unsigned int) int_byte_position (firstIvar)
		  : instanceSize;
  /* APPLE LOCAL end ARM signedness mismatch */

  /* APPLE LOCAL begin radar 6064186 */
  ref_name = newabi_append_ro (IDENTIFIER_POINTER (DECL_NAME (UOBJC_V2_CLASS_decl)));
  decl = start_var_decl (objc_v2_class_ro_template, ref_name);
  set_user_assembler_name (decl, ref_name);
  /* APPLE LOCAL end radar 6064186 */

  /* APPLE LOCAL begin radar 4695101 */
  ivarLayout = objc_build_ivar_layout (true);
  weakIvarLayout = objc_build_ivar_layout (false); 
  /* APPLE LOCAL end radar 4695101 */

  initlist = build_class_ro_t_initializer (TREE_TYPE (decl),
					   name_expr,
					   (flags | cls_flags), instanceStart, instanceSize,
					   ivarLayout, UOBJC_V2_INSTANCE_METHODS_decl, protocol_decl,
					   UOBJC_V2_INSTANCE_VARIABLES_decl,
					   /* APPLE LOCAL radar 4695101 */
					   weakIvarLayout,
					   UOBJC_V2_PROPERTY_decl);
  finish_var_decl (decl, initlist);

  /* static struct class_t _OBJC_ACLASS_Foo = { ... }; */
  initlist = build_class_t_initializer (TREE_TYPE (class_decl),
					build_fold_addr_expr (metaclass_decl),
					class_superclass_expr,
                                        build_fold_addr_expr (decl),
                                        build_fold_addr_expr (UOBJC_V2_CACHE_decl), 
                                        build_fold_addr_expr (UOBJC_V2_VTABLE_decl)); 

  finish_var_decl (class_decl, initlist);
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  /* At -O0, we may have emitted references to the decl earlier. */
  if (!optimize)
    reset_type_and_initializer_llvm(class_decl);
#endif
  /* LLVM LOCAL end */
  objc_add_to_class_list_chain (class_decl);
  if (CLASS_OR_CATEGORY_HAS_LOAD_IMPL (objc_implementation_context) != NULL_TREE)
    objc_add_to_nonlazy_class_list_chain (class_decl);
  /* APPLE LOCAL begin radar 5008110 */
  if (flag_objc_zerocost_exceptions && (flags & 0x20))
    objc2_build_ehtype_var_decl (CLASS_NAME (implementation_template),
                                 false, false);
  /* APPLE LOCAL end radar 5008110 */
}
/* APPLE LOCAL end ObjC new abi */

/* static struct objc_class _OBJC_METACLASS_Foo={ ... };
   static struct objc_class _OBJC_CLASS_Foo={ ... };  */

static void
generate_shared_structures (int cls_flags)
{
  tree sc_spec, decl_specs, decl;
  tree name_expr, super_expr, root_expr;
  tree my_root_id = NULL_TREE, my_super_id = NULL_TREE;
  tree cast_type, initlist, protocol_decl;
  /* APPLE LOCAL begin radar 5142207 */
  const char *visibility = objc_class_visibility (implementation_template);
  int hidden_flag = (visibility  &&  strcmp (visibility, "hidden") == 0) ? CLS_HIDDEN : 0;
  /* APPLE LOCAL end radar 5142207 */

  my_super_id = CLASS_SUPER_NAME (implementation_template);
  if (my_super_id)
    {
      add_class_reference (my_super_id);

      /* Compute "my_root_id" - this is required for code generation.
         the "isa" for all meta class structures points to the root of
         the inheritance hierarchy (e.g. "__Object")...  */
      my_root_id = my_super_id;
      do
	{
	  tree my_root_int = lookup_interface (my_root_id);

	  if (my_root_int && CLASS_SUPER_NAME (my_root_int))
	    my_root_id = CLASS_SUPER_NAME (my_root_int);
	  else
	    break;
	}
      while (1);
    }
  else
    /* No super class.  */
    my_root_id = CLASS_NAME (implementation_template);

  cast_type = build_pointer_type (objc_class_template);
  name_expr = add_objc_string (CLASS_NAME (implementation_template),
			       class_names);

  /* Install class `isa' and `super' pointers at runtime.  */
  if (my_super_id)
    {
      super_expr = add_objc_string (my_super_id, class_names);
      super_expr = build_c_cast (cast_type, super_expr); /* cast! */
    }
  else
    /* APPLE LOCAL begin radar 6285794 */
    {
      super_expr = convert (string_type_node,
                            integer_zero_node);
      super_expr = build_c_cast (cast_type, super_expr); /* cast! */
    }
    /* APPLE LOCAL end radar 6285794 */

  root_expr = add_objc_string (my_root_id, class_names);
  root_expr = build_c_cast (cast_type, root_expr); /* cast! */

  if (CLASS_PROTOCOL_LIST (implementation_template))
    {
      generate_protocol_references
	(CLASS_PROTOCOL_LIST (implementation_template));
      protocol_decl = generate_protocol_list (implementation_template);
    }
  else
    protocol_decl = 0;

  /* static struct objc_class _OBJC_METACLASS_Foo = { ... }; */

  sc_spec = build_tree_list (NULL_TREE, ridpointers[(int) RID_STATIC]);
  decl_specs = tree_cons (NULL_TREE, objc_class_template, sc_spec);

  /* APPLE LOCAL begin radar 4349690 */
  decl = update_var_decl (UOBJC_METACLASS_decl);
  /* APPLE LOCAL end radar 4349690 */

  initlist
    = build_shared_structure_initializer
      (TREE_TYPE (decl),
       root_expr, super_expr, name_expr,
       convert (integer_type_node, TYPE_SIZE_UNIT (objc_class_template)),
       /* APPLE LOCAL radar 5142207 */
       CLS_META | hidden_flag,
       UOBJC_CLASS_METHODS_decl,
       UOBJC_CLASS_VARIABLES_decl,
       /* APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions */
       protocol_decl, NULL_TREE);

  finish_var_decl (decl, initlist);

  /* static struct objc_class _OBJC_CLASS_Foo={ ... }; */

  /* APPLE LOCAL begin radar 4349690 */
  decl = update_var_decl (UOBJC_CLASS_decl);
  /* APPLE LOCAL end radar 4349690 */

  initlist
    = build_shared_structure_initializer
      (TREE_TYPE (decl),
       build_unary_op (ADDR_EXPR, UOBJC_METACLASS_decl, 0),
       super_expr, name_expr,
       convert (integer_type_node,
		TYPE_SIZE_UNIT (CLASS_STATIC_TEMPLATE
				(implementation_template))),
       /* APPLE LOCAL radar 5142207 */
       CLS_FACTORY | hidden_flag | cls_flags,
       UOBJC_INSTANCE_METHODS_decl,
       UOBJC_INSTANCE_VARIABLES_decl,
/* APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions */
       protocol_decl, UOBJC_CLASS_EXT_decl);

  finish_var_decl (decl, initlist);
}


static const char *
synth_id_with_class_suffix (const char *preamble, tree ctxt)
{
  static char string[BUFSIZE];

  if (TREE_CODE (ctxt) == CLASS_IMPLEMENTATION_TYPE
      || TREE_CODE (ctxt) == CLASS_INTERFACE_TYPE)
    {
      sprintf (string, "%s_%s", preamble,
	       IDENTIFIER_POINTER (CLASS_NAME (ctxt)));
    }
  else if (TREE_CODE (ctxt) == CATEGORY_IMPLEMENTATION_TYPE
	   || TREE_CODE (ctxt) == CATEGORY_INTERFACE_TYPE)
    {
      /* We have a category.  */
      const char *const class_name
	= IDENTIFIER_POINTER (CLASS_NAME (objc_implementation_context));
      const char *const class_super_name
	= IDENTIFIER_POINTER (CLASS_SUPER_NAME (objc_implementation_context));
      /* APPLE LOCAL begin ObjC abi v2 */
      if (flag_objc_abi == 2)
        sprintf (string, "%s_%s_$_%s", preamble, class_name, class_super_name);
      else
        sprintf (string, "%s_%s_%s", preamble, class_name, class_super_name);
      /* APPLE LOCAL end ObjC abi v2 */
    }
  else if (TREE_CODE (ctxt) == PROTOCOL_INTERFACE_TYPE)
    {
      const char *protocol_name = IDENTIFIER_POINTER (PROTOCOL_NAME (ctxt));
      sprintf (string, "%s_%s", preamble, protocol_name);
    }
  else
    abort ();

  return string;
}

/* If type is empty or only type qualifiers are present, add default
   type of id (otherwise grokdeclarator will default to int).  */

static tree
adjust_type_for_id_default (tree type)
{
  if (!type)
    type = make_node (TREE_LIST);

  if (!TREE_VALUE (type))
    TREE_VALUE (type) = objc_object_type;
  else if (TREE_CODE (TREE_VALUE (type)) == RECORD_TYPE
	   && TYPED_OBJECT (TREE_VALUE (type)))
    error ("can not use an object as parameter to a method");

  return type;
}

/*   Usage:
		keyworddecl:
			// APPLE LOCAL radar 4157812
			selector ':' '(' typename ')' [attr-list] identifier

     Purpose:
		Transform an Objective-C keyword argument into
		the C equivalent parameter declarator.

     In:	key_name, an "identifier_node" (optional).
		arg_type, a  "tree_list" (optional).
		arg_name, an "identifier_node".
		// APPLE LOCAL radar 4157812
		arg_attr, a  "tree_list" (optional)

     Note:	It would be really nice to strongly type the preceding
		arguments in the function prototype; however, then I
		could not use the "accessor" macros defined in "tree.h".

     Out:	an instance of "keyword_decl".  */

tree
/* APPLE LOCAL radar 4157812 */
objc_build_keyword_decl (tree key_name, tree arg_type, tree arg_name, tree arg_attr)
{
  tree keyword_decl;

  /* If no type is specified, default to "id".  */
  arg_type = adjust_type_for_id_default (arg_type);

  keyword_decl = make_node (KEYWORD_DECL);

  TREE_TYPE (keyword_decl) = arg_type;
  KEYWORD_ARG_NAME (keyword_decl) = arg_name;
  /* APPLE LOCAL radar 4157812 */
  KEYWORD_ARG_ATTRIBUTE (keyword_decl) = arg_attr;
  KEYWORD_KEY_NAME (keyword_decl) = key_name;

  return keyword_decl;
}

/* Given a chain of keyword_decl's, synthesize the full keyword selector.  */

static tree
build_keyword_selector (tree selector)
{
  int len = 0;
  tree key_chain, key_name;
  char *buf;

  /* Scan the selector to see how much space we'll need.  */
  for (key_chain = selector; key_chain; key_chain = TREE_CHAIN (key_chain))
    {
      if (TREE_CODE (selector) == KEYWORD_DECL)
	key_name = KEYWORD_KEY_NAME (key_chain);
      else if (TREE_CODE (selector) == TREE_LIST)
	key_name = TREE_PURPOSE (key_chain);
      else
	abort ();

      if (key_name)
	len += IDENTIFIER_LENGTH (key_name) + 1;
      else
	/* Just a ':' arg.  */
	len++;
    }

  buf = (char *) alloca (len + 1);
  /* Start the buffer out as an empty string.  */
  buf[0] = '\0';

  for (key_chain = selector; key_chain; key_chain = TREE_CHAIN (key_chain))
    {
      if (TREE_CODE (selector) == KEYWORD_DECL)
	key_name = KEYWORD_KEY_NAME (key_chain);
      else if (TREE_CODE (selector) == TREE_LIST)
	{
	  key_name = TREE_PURPOSE (key_chain);
	  /* The keyword decl chain will later be used as a function argument
	     chain.  Unhook the selector itself so as to not confuse other
	     parts of the compiler.  */
	  TREE_PURPOSE (key_chain) = NULL_TREE;
	}
      else
	abort ();

      if (key_name)
	strcat (buf, IDENTIFIER_POINTER (key_name));
      strcat (buf, ":");
    }

  return get_identifier (buf);
}

/* Used for declarations and definitions.  */

static tree
build_method_decl (enum tree_code code, tree ret_type, tree selector,
		   tree add_args, bool ellipsis)
{
  tree method_decl;

  /* If no type is specified, default to "id".  */
  ret_type = adjust_type_for_id_default (ret_type);

  method_decl = make_node (code);
  TREE_TYPE (method_decl) = ret_type;

  /* If we have a keyword selector, create an identifier_node that
     represents the full selector name (`:' included)...  */
  if (TREE_CODE (selector) == KEYWORD_DECL)
    {
      METHOD_SEL_NAME (method_decl) = build_keyword_selector (selector);
      METHOD_SEL_ARGS (method_decl) = selector;
      METHOD_ADD_ARGS (method_decl) = add_args;
      METHOD_ADD_ARGS_ELLIPSIS_P (method_decl) = ellipsis;
    }
  else
    {
      METHOD_SEL_NAME (method_decl) = selector;
      METHOD_SEL_ARGS (method_decl) = NULL_TREE;
      METHOD_ADD_ARGS (method_decl) = NULL_TREE;
    }

  return method_decl;
}

#define METHOD_DEF 0
#define METHOD_REF 1

/* APPLE LOCAL begin radar 3803157 - objc attribute */
/* APPLE LOCAL begin radar 4529765 */
/* This routine is called for a "format" attribute. It adds the number of
   hidden argument ('2') to the format's 2nd and 3rd argument to compensate
   for these two arguments. This is to make rest of the "format" attribute
   processing done in the middle-end to work seemlessly. */

static void
objc_delta_format_args (tree format)
{
  tree format_num_expr, first_arg_num_expr;
  int val; 
  tree args = TREE_VALUE (format);
  gcc_assert (TREE_CHAIN (args) && TREE_CHAIN (TREE_CHAIN (args)));
  format_num_expr = TREE_VALUE (TREE_CHAIN (args));
  first_arg_num_expr = TREE_VALUE (TREE_CHAIN (TREE_CHAIN (args)));
  if (format_num_expr && TREE_CODE (format_num_expr) == INTEGER_CST)
    {
      val = TREE_INT_CST_LOW (format_num_expr);
      TREE_VALUE (TREE_CHAIN (args)) = build_int_cst (NULL_TREE, val+2);
    }
  if (first_arg_num_expr && TREE_CODE (first_arg_num_expr) == INTEGER_CST)
    {
      val = TREE_INT_CST_LOW (first_arg_num_expr);
      /* APPLE LOCAL begin radar 6157135 */
      if (val != 0)
        TREE_VALUE (TREE_CHAIN (TREE_CHAIN (args))) = build_int_cst (NULL_TREE, val+2);
      /* APPLE LOCAL end radar 6157135 */
    }
}

/* This routine recognizes objc legal attributes. In case of "format" 
   attribute, it calls objc_delta_format_args to compensate for hidden 
   argument objc passes to its messaging call. */
static bool
objc_any_recognized_attribute (tree attributes)
{
  tree chain;
  bool res = false;
  for (chain = attributes; chain; chain = TREE_CHAIN (chain))
    {
      if (is_attribute_p ("format", TREE_PURPOSE (chain)))
	{
	  objc_delta_format_args (chain);
	  res = true;
	}
      else if (is_attribute_p ("sentinel", TREE_PURPOSE (chain)))
	res = true;	
    }
  return res;
}
/* APPLE LOCAL end radar 4529765 */
/* This routine processes objective-c method attributes. */

static void
objc_decl_method_attributes (tree *node, tree attributes, int flags)
{
  /* APPLE LOCAL radar 4529765 */
  if (objc_any_recognized_attribute (attributes))
    {
      /* hackery to make an obj method look like a function type. */
      tree rettype = TREE_TYPE (*node);
      TREE_TYPE (*node) = build_function_type (TREE_VALUE (rettype), 
			    /* APPLE LOCAL radar 6307941 */
                            get_arg_type_list (METHOD_SEL_NAME (*node), 
                                               *node, METHOD_REF, 0));
      decl_attributes (node, attributes, flags);
      METHOD_TYPE_ATTRIBUTES (*node) = TYPE_ATTRIBUTES (TREE_TYPE (*node));
      TREE_TYPE (*node) = rettype;
    }
  else
    decl_attributes (node, attributes, flags);
}
/* APPLE LOCAL end radar 3803157 - objc attribute */

/* Used by `build_objc_method_call' and `comp_proto_with_proto'.  Return
   an argument list for method METH.  CONTEXT is either METHOD_DEF or
   METHOD_REF, saying whether we are trying to define a method or call
   one.  SUPERFLAG says this is for a send to super; this makes a
   difference for the NeXT calling sequence in which the lookup and
   the method call are done together.  If METH is null, user-defined
   arguments (i.e., beyond self and _cmd) shall be represented by `...'.  */

static tree
/* APPLE LOCAL radar 6307941 */
get_arg_type_list (tree sel_name, tree meth, int context, int superflag)
{
  tree arglist, akey;

  /* Receiver type.  */
  if (flag_next_runtime && superflag)
    arglist = build_tree_list (NULL_TREE, objc_super_type);
  else if (context == METHOD_DEF && TREE_CODE (meth) == INSTANCE_METHOD_DECL)
    arglist = build_tree_list (NULL_TREE, objc_instance_type);
  else
    arglist = build_tree_list (NULL_TREE, objc_object_type);

  /* Selector type - will eventually change to `int'.  */
  /* APPLE LOCAL begin ObjC new abi */
  chainon (arglist, build_tree_list (NULL_TREE, flag_objc_abi == 2 
				     /* APPLE LOCAL ARM hybrid objc-2.0 - radar 6307941 */
                                     && !objc_legacy_dispatched_selector_p (sel_name)
				     ? (superflag 
					? objc_v2_super_selector_type 
					: objc_v2_selector_type)
				     : objc_selector_type));
  /* APPLE LOCAL end ObjC new abi */

  /* No actual method prototype given -- assume that remaining arguments
     are `...'.  */
  if (!meth)
    return arglist;

  /* Build a list of argument types.  */
  for (akey = METHOD_SEL_ARGS (meth); akey; akey = TREE_CHAIN (akey))
    {
      tree arg_type = TREE_VALUE (TREE_TYPE (akey));

      /* APPLE LOCAL begin 4209854 */
      /* Decay argument types for the underlying C function as appropriate.  */
      arg_type = objc_decay_parm_type (arg_type);
      /* APPLE LOCAL end 4209854 */

      chainon (arglist, build_tree_list (NULL_TREE, arg_type));
    }

  if (METHOD_ADD_ARGS (meth))
    {
      for (akey = TREE_CHAIN (METHOD_ADD_ARGS (meth));
	   akey; akey = TREE_CHAIN (akey))
	{
	  tree arg_type = TREE_TYPE (TREE_VALUE (akey));

	  /* APPLE LOCAL begin 4209854 */
	  arg_type = objc_decay_parm_type (arg_type);

	  /* APPLE LOCAL end 4209854 */
	  chainon (arglist, build_tree_list (NULL_TREE, arg_type));
	}

      if (!METHOD_ADD_ARGS_ELLIPSIS_P (meth))
	goto lack_of_ellipsis;
    }
  else
    {
     lack_of_ellipsis:
      chainon (arglist, OBJC_VOID_AT_END);
    }

  return arglist;
}

static tree
check_duplicates (hash hsh, int methods, int is_class)
{
  tree meth = NULL_TREE;

  if (hsh)
    {
      meth = hsh->key;

      if (hsh->list)
        {
	  /* We have two or more methods with the same name but
	     different types.  */
	  attr loop;

	  /* But just how different are those types?  If
	     -Wno-strict-selector-match is specified, we shall not
	     complain if the differences are solely among types with
	     identical size and alignment.  */
	  if (!warn_strict_selector_match)
	    {
	      for (loop = hsh->list; loop; loop = loop->next)
		if (!comp_proto_with_proto (meth, loop->value, 0))
		  goto issue_warning;

	      return meth;
	    }

	issue_warning:
	  warning (0, "multiple %s named %<%c%s%> found",
		   methods ? "methods" : "selectors",
		   (is_class ? '+' : '-'),
		   IDENTIFIER_POINTER (METHOD_SEL_NAME (meth)));

	  warn_with_method (methods ? "using" : "found",
			    ((TREE_CODE (meth) == INSTANCE_METHOD_DECL)
			     ? '-'
			     : '+'),
			    meth);
	  for (loop = hsh->list; loop; loop = loop->next)
	    warn_with_method ("also found",
			      ((TREE_CODE (loop->value) == INSTANCE_METHOD_DECL)
			       ? '-'
			       : '+'),
			      loop->value);
        }
    }
  return meth;
}

/* If RECEIVER is a class reference, return the identifier node for
   the referenced class.  RECEIVER is created by objc_get_class_reference,
   so we check the exact form created depending on which runtimes are
   used.  */

static tree
receiver_is_class_object (tree receiver, int self, int super)
{
  tree chain, exp, arg;

  /* The receiver is 'self' or 'super' in the context of a class method.  */
  if (objc_method_context
      && TREE_CODE (objc_method_context) == CLASS_METHOD_DECL
      && (self || super))
    return (super
	    ? CLASS_SUPER_NAME (implementation_template)
	    : CLASS_NAME (implementation_template));

  if (flag_next_runtime)
    {
      /* The receiver is a variable created by
         build_class_reference_decl.  */
      if (TREE_CODE (receiver) == VAR_DECL && IS_CLASS (TREE_TYPE (receiver)))
        /* Look up the identifier.  */
  	/* APPLE LOCAL begin ObjC abi v2 */
	for (chain = (flag_objc_abi == 2) 
		      ? classlist_ref_chain 
		      : cls_ref_chain; chain; chain = TREE_CHAIN (chain))
  	/* APPLE LOCAL end ObjC abi v2 */
	  if (TREE_PURPOSE (chain) == receiver)
            return TREE_VALUE (chain);
    }

  /* The receiver is a function call that returns an id.  Check if
     it is a call to objc_getClass, if so, pick up the class name.  */
  if (TREE_CODE (receiver) == CALL_EXPR
      && (exp = TREE_OPERAND (receiver, 0))
      && TREE_CODE (exp) == ADDR_EXPR
      && (exp = TREE_OPERAND (exp, 0))
      && TREE_CODE (exp) == FUNCTION_DECL
      /* For some reason, we sometimes wind up with multiple FUNCTION_DECL
	 prototypes for objc_get_class().  Thankfully, they seem to share the
	 same function type.  */
      && TREE_TYPE (exp) == TREE_TYPE (objc_get_class_decl)
      && !strcmp (IDENTIFIER_POINTER (DECL_NAME (exp)), TAG_GETCLASS)
      /* We have a call to objc_get_class/objc_getClass!  */
      && (arg = TREE_OPERAND (receiver, 1))
      && TREE_CODE (arg) == TREE_LIST
      && (arg = TREE_VALUE (arg)))
    {
      STRIP_NOPS (arg);
      if (TREE_CODE (arg) == ADDR_EXPR
	  && (arg = TREE_OPERAND (arg, 0))
	  && TREE_CODE (arg) == STRING_CST)
	/* Finally, we have the class name.  */
	return get_identifier (TREE_STRING_POINTER (arg));
    }
  return 0;
}

/* If we are currently building a message expr, this holds
   the identifier of the selector of the message.  This is
   used when printing warnings about argument mismatches.  */

static tree current_objc_message_selector = 0;

tree
objc_message_selector (void)
{
  return current_objc_message_selector;
}

/* Construct an expression for sending a message.
   MESS has the object to send to in TREE_PURPOSE
   and the argument list (including selector) in TREE_VALUE.

   (*(<abstract_decl>(*)())_msg)(receiver, selTransTbl[n], ...);
   (*(<abstract_decl>(*)())_msgSuper)(receiver, selTransTbl[n], ...);  */

tree
objc_build_message_expr (tree mess)
{
  tree receiver = TREE_PURPOSE (mess);
  tree sel_name;
#ifdef OBJCPLUS
  tree args = TREE_PURPOSE (TREE_VALUE (mess));
#else
  tree args = TREE_VALUE (mess);
#endif
  tree method_params = NULL_TREE;

  /* APPLE LOCAL radar 4294425 */
  if (TREE_CODE (receiver) == ERROR_MARK || TREE_CODE (args) == ERROR_MARK)
    return error_mark_node;

  /* Obtain the full selector name.  */
  if (TREE_CODE (args) == IDENTIFIER_NODE)
    /* A unary selector.  */
    sel_name = args;
  else if (TREE_CODE (args) == TREE_LIST)
    sel_name = build_keyword_selector (args);
  else
    abort ();

  /* Build the parameter list to give to the method.  */
  if (TREE_CODE (args) == TREE_LIST)
#ifdef OBJCPLUS
    method_params = chainon (args, TREE_VALUE (TREE_VALUE (mess)));
#else
    {
      tree chain = args, prev = NULL_TREE;

      /* We have a keyword selector--check for comma expressions.  */
      while (chain)
	{
	  tree element = TREE_VALUE (chain);

	  /* We have a comma expression, must collapse...  */
	  if (TREE_CODE (element) == TREE_LIST)
	    {
	      if (prev)
		TREE_CHAIN (prev) = element;
	      else
		args = element;
	    }
	  prev = chain;
	  chain = TREE_CHAIN (chain);
        }
      method_params = args;
    }
#endif

#ifdef OBJCPLUS
  if (processing_template_decl)
    /* Must wait until template instantiation time.  */
    return build_min_nt (MESSAGE_SEND_EXPR, receiver, sel_name,
			 method_params);
#endif

  return objc_finish_message_expr (receiver, sel_name, method_params);
}

/* Look up method SEL_NAME that would be suitable for receiver
   of type 'id' (if IS_CLASS is zero) or 'Class' (if IS_CLASS is
   nonzero), and report on any duplicates.  */

static tree
lookup_method_in_hash_lists (tree sel_name, int is_class)
{
  hash method_prototype = NULL;

  if (!is_class)
    method_prototype = hash_lookup (nst_method_hash_list,
				    sel_name);

  if (!method_prototype)
    {
      method_prototype = hash_lookup (cls_method_hash_list,
				      sel_name);
      is_class = 1;
    }

  return check_duplicates (method_prototype, 1, is_class);
}

/* The 'objc_finish_message_expr' routine is called from within
   'objc_build_message_expr' for non-template functions.  In the case of
   C++ template functions, it is called from 'build_expr_from_tree'
   (in decl2.c) after RECEIVER and METHOD_PARAMS have been expanded.  */

tree
objc_finish_message_expr (tree receiver, tree sel_name, tree method_params)
{
  tree method_prototype = NULL_TREE, rprotos = NULL_TREE, rtype;
  tree selector, retval, class_tree;
  int self, super, have_cast;
  /* APPLE LOCAL radar 4547918 */
  bool forward_class = false;

  /* APPLE LOCAL begin radar 4180592 */
  if (receiver == error_mark_node || TREE_TYPE (receiver) == error_mark_node)
    return receiver;
  /* APPLE LOCAL end radar 4180592 */
  /* APPLE LOCAL begin radar 5809099 */
  if (TREE_CODE (TREE_TYPE (receiver)) == BLOCK_POINTER_TYPE)
    receiver = convert (objc_object_type, receiver);
  /* APPLE LOCAL end radar 5809099 */
  /* Extract the receiver of the message, as well as its type
     (where the latter may take the form of a cast or be inferred
     from the implementation context).  */
  /* APPLE LOCAL begin radar 4653319 */
  rtype = objc_get_inner_exp (receiver);
  /* APPLE LOCAL end radar 4653319 */
  self = (rtype == self_decl);
  super = (rtype == UOBJC_SUPER_decl);
  rtype = TREE_TYPE (receiver);
  have_cast = (TREE_CODE (receiver) == NOP_EXPR
	       || (TREE_CODE (receiver) == COMPOUND_EXPR
		   && !IS_SUPER (rtype)));

  /* If we are calling [super dealloc], reset our warning flag.  */
  if (super && !strcmp ("dealloc", IDENTIFIER_POINTER (sel_name)))
    should_call_super_dealloc = 0;
  /* APPLE LOCAL begin radar 4757423 */
  /* If we are calling [super finalize], reset our warning flag.  */
  if (super && !strcmp ("finalize", IDENTIFIER_POINTER (sel_name)))
    should_call_super_finalize = 0;
  /* APPLE LOCAL end radar 4757423 */

  /* If the receiver is a class object, retrieve the corresponding
     @interface, if one exists. */
  class_tree = receiver_is_class_object (receiver, self, super);

  /* Now determine the receiver type (if an explicit cast has not been
     provided).  */
  if (!have_cast)
    {
      if (class_tree)
      /* APPLE LOCAL begin radar 4547918 */
	{
	  /* APPLE LOCAL begin radar 5741070  */
	  tree interface_record_type = NULL;
	  rtype = lookup_interface (class_tree);

	  /* Find the record type definition for the interface class.  */

	  if (rtype)
	    interface_record_type = 
	      c_return_interface_record_type (OBJC_TYPE_NAME (rtype));

	  /* If the record type is found, mark is as used (since it
	     is used to dispatch a method call).  */

	  if (interface_record_type != NULL)
	    used_types_insert (interface_record_type);
	  /* APPLE LOCAL end radar 5741070  */
	  forward_class = !rtype;
	}
      /* APPLE LOCAL end radar 4547918 */
      /* Handle `self' and `super'.  */
      else if (super)
	{
	  if (!CLASS_SUPER_NAME (implementation_template))
	    {
	      error ("no super class declared in @interface for %qs",
		     IDENTIFIER_POINTER (CLASS_NAME (implementation_template)));
	      return error_mark_node;
	    }
	  rtype = lookup_interface (CLASS_SUPER_NAME (implementation_template));
	}
      else if (self)
	rtype = lookup_interface (CLASS_NAME (implementation_template));
    }

  /* If receiver is of type `id' or `Class' (or if the @interface for a
     class is not visible), we shall be satisfied with the existence of
     any instance or class method. */
  if (objc_is_id (rtype))
    {
      class_tree = (IS_CLASS (rtype) ? objc_class_name : NULL_TREE);
      rprotos = (TYPE_HAS_OBJC_INFO (TREE_TYPE (rtype))
		 ? TYPE_OBJC_PROTOCOL_LIST (TREE_TYPE (rtype))
		 : NULL_TREE);
      rtype = NULL_TREE;

      if (rprotos)
	{
	  /* If messaging 'id <Protos>' or 'Class <Proto>', first search
	     in protocols themselves for the method prototype.  */
	  method_prototype
	    = lookup_method_in_protocol_list (rprotos, sel_name,
					      class_tree != NULL_TREE);

	  /* If messaging 'Class <Proto>' but did not find a class method
	     prototype, search for an instance method instead, and warn
	     about having done so.  */
	  if (!method_prototype && !rtype && class_tree != NULL_TREE)
	    {
	      method_prototype
		= lookup_method_in_protocol_list (rprotos, sel_name, 0);

	      if (method_prototype)
		warning (0, "found %<-%s%> instead of %<+%s%> in protocol(s)",
			 IDENTIFIER_POINTER (sel_name),
			 IDENTIFIER_POINTER (sel_name));
	    }
	}
    }
  else if (rtype)
    {
      tree orig_rtype = rtype, saved_rtype;

      if (TREE_CODE (rtype) == POINTER_TYPE)
	rtype = TREE_TYPE (rtype);
      /* Traverse typedef aliases */
      while (TREE_CODE (rtype) == RECORD_TYPE && OBJC_TYPE_NAME (rtype)
	     && TREE_CODE (OBJC_TYPE_NAME (rtype)) == TYPE_DECL
	     && DECL_ORIGINAL_TYPE (OBJC_TYPE_NAME (rtype)))
	rtype = DECL_ORIGINAL_TYPE (OBJC_TYPE_NAME (rtype));
      saved_rtype = rtype;
      if (TYPED_OBJECT (rtype))
	{
	  rprotos = TYPE_OBJC_PROTOCOL_LIST (rtype);
	  rtype = TYPE_OBJC_INTERFACE (rtype);
	}
      /* If we could not find an @interface declaration, we must have
	 only seen a @class declaration; so, we cannot say anything
	 more intelligent about which methods the receiver will
	 understand. */
      if (!rtype || TREE_CODE (rtype) == IDENTIFIER_NODE)
	/* APPLE LOCAL begin radar 4457381 */
	{
	  rtype = NULL_TREE;
	  /* We could not find an @interface declaration, yet Message maybe in a 
	     @class's protocol. */
	  if (!method_prototype && rprotos)
	    method_prototype
	      = lookup_method_in_protocol_list (rprotos, sel_name, 0);
	}
	/* APPLE LOCAL end radar 4457381 */
      else if (TREE_CODE (rtype) == CLASS_INTERFACE_TYPE
	  || TREE_CODE (rtype) == CLASS_IMPLEMENTATION_TYPE)
	{
	  /* We have a valid ObjC class name.  Look up the method name
	     in the published @interface for the class (and its
	     superclasses). */
	  method_prototype
	    /* APPLE LOCAL objc new property */
	    = lookup_method_static (rtype, sel_name, class_tree != NULL_TREE, false, false);

	  /* If the method was not found in the @interface, it may still
	     exist locally as part of the @implementation.  */
	  if (!method_prototype && objc_implementation_context
	     && CLASS_NAME (objc_implementation_context)
		== OBJC_TYPE_NAME (rtype))
	    method_prototype
	      = lookup_method
		((class_tree
		  ? CLASS_CLS_METHODS (objc_implementation_context)
		  : CLASS_NST_METHODS (objc_implementation_context)),
		  sel_name);

	  /* If we haven't found a candidate method by now, try looking for
	     it in the protocol list.  */
	  if (!method_prototype && rprotos)
	    method_prototype
	      = lookup_method_in_protocol_list (rprotos, sel_name,
						class_tree != NULL_TREE);
	}
      else
	{
	  /* APPLE LOCAL begin decay function/array receivers */
#ifndef OBJCPLUS
	  /* In C need to decay array/function receivers so can be converted 
	     to id. */
	  struct c_expr exp;
	  exp.value = receiver;
	  exp = default_function_array_conversion (exp);
	  receiver = exp.value;
	/* APPLE LOCAL begin radar 3533972 */
#else
	  if (can_convert_arg (objc_object_type, TREE_TYPE (receiver), 
				receiver, LOOKUP_NORMAL))
	    {
	      /* In rare cases, 'receiver' must be converted to type 'id' using
	         user-defined type conversion. 'id' is type of the 1st argument to
	         objc_msgSend (id self, SEL op, ...); */
	      tree cnv_rec = perform_implicit_conversion (objc_object_type, receiver);
              if (cnv_rec && cnv_rec != error_mark_node)
	        return objc_finish_message_expr (cnv_rec, sel_name, method_params);
	    }
	/* APPLE LOCAL end radar 3533972 */
#endif
	  /* APPLE LOCAL end decay function/array receivers */
          /* APPLE LOCAL begin radar 5595325 */
          /* Do not issue this warning on random NSObject objects. */
          if (!objc_nsobject_attribute_type (orig_rtype))
            warning (0, "invalid receiver type %qs",
                     gen_type_name (orig_rtype));
            /* After issuing the "invalid receiver" warning, perform method
              lookup as if we were messaging 'id'.  */
           /* APPLE LOCAL end radar 5595325 */
	  rtype = rprotos = NULL_TREE;
	}
    }


  /* For 'id' or 'Class' receivers, search in the global hash table
     as a last resort.  For all receivers, warn if protocol searches
     have failed.  */
  if (!method_prototype)
    {
      if (rprotos)
	warning (0, "%<%c%s%> not found in protocol(s)",
		 (class_tree ? '+' : '-'),
		 IDENTIFIER_POINTER (sel_name));

      if (!rtype)
	/* APPLE LOCAL begin radar 4547918 */
	{
	  method_prototype
	    = lookup_method_in_hash_lists (sel_name, class_tree != NULL_TREE);
	  if (forward_class && method_prototype)
	    warning (0, "receiver %qs is a forward class and corresponding @interface may not exist",
		     IDENTIFIER_POINTER (class_tree));
	}
	/* APPLE LOCAL end radar 4547918 */
    }

  /* APPLE LOCAL begin radar 4506893 */
  /* APPLE LOCAL begin objc new property */
  if (!method_prototype && in_objc_property_setter_name_context)
    {
      error ("object cannot be set - either readonly property or no setter found");
      /* APPLE LOCAL radar 5390587 */
      return error_mark_node;
    }
  /* APPLE LOCAL end objc new property */
  else if (!method_prototype) 
  /* APPLE LOCAL end radar 4506893 */
    {
      static bool warn_missing_methods = false;

      if (rtype)
	warning (0, "%qs may not respond to %<%c%s%>",
		 IDENTIFIER_POINTER (OBJC_TYPE_NAME (rtype)),
		 (class_tree ? '+' : '-'),
		 IDENTIFIER_POINTER (sel_name));
      /* If we are messaging an 'id' or 'Class' object and made it here,
	 then we have failed to find _any_ instance or class method,
	 respectively.  */
       /* APPLE LOCAL radar 5932809 - copyable byref blocks */
      else if (!donot_warn_missing_methods)
	warning (0, "no %<%c%s%> method found",
		 (class_tree ? '+' : '-'),
		 IDENTIFIER_POINTER (sel_name));
      /* APPLE LOCAL radar 5932809 - copyable byref blocks */
      if (!warn_missing_methods && !donot_warn_missing_methods)
	{
	  warning (0, "(Messages without a matching method signature");
	  warning (0, "will be assumed to return %<id%> and accept");
	  warning (0, "%<...%> as arguments.)");
	  warn_missing_methods = true;
	}
    }

  /* APPLE LOCAL begin radar 6049617 - radar 5005756 */
  /* If receiver has an 'id' type, then method selection happens at runtime.
   It is misleading to issue warning on a method call not knowing which
   method actually gets called; thus check for objc_is_id is added. */
  if (method_prototype && rtype)
  {
    if (TREE_DEPRECATED (method_prototype))
      warn_deprecated_use (method_prototype);
    if (TREE_UNAVAILABLE (method_prototype))
      error_unavailable_use (method_prototype);    
  }
  /* APPLE LOCAL end radar 6049617 - radar 5005756 */
  /* Save the selector name for printing error messages.  */
  current_objc_message_selector = sel_name;

  /* Build the parameters list for looking up the method.
     These are the object itself and the selector.  */

  /* APPLE LOCAL ObjC new abi */
  /* Code moved down */
  /* APPLE LOCAL begin ObjC new abi */
  /* APPLE LOCAL ARM hybrid objc-2.0 - radar 6307941 */
  if (flag_objc_abi == 2 && !objc_legacy_dispatched_selector_p (sel_name))
    {
      tree ret_type;
      tree message_func_decl;
      bool check_for_nil = flag_objc_nilcheck;

      ret_type = (method_prototype ? 
	      	    TREE_VALUE (TREE_TYPE (method_prototype)) : 
		    objc_object_type);

      /* Do we need to check for nil receivers ? */
      /* For now, message sent to classes need no nil check. In future, class
	 declaration marked as weak_import must be nil checked. */
      if (super 
	  || (TREE_CODE (receiver) == VAR_DECL 
	      && TREE_TYPE (receiver) == objc_class_type))
	check_for_nil = false;

      /* LLVM LOCAL - begin PR1654 */
#ifdef ENABLE_LLVM
      if (aggregate_value_p (ret_type, 0))
#else
      if (!targetm.calls.struct_value_rtx (0, 0)
          && (TREE_CODE (ret_type) == RECORD_TYPE 
       	      || TREE_CODE (ret_type) == UNION_TYPE)
          /* APPLE LOCAL begin radar 5080710 */
          && (TREE_ADDRESSABLE (ret_type)
              || targetm.calls.return_in_memory (ret_type, 0)))
          /* APPLE LOCAL end radar 5080710 */
#endif
      /* LLVM LOCAL - end PR1654 */
	{
	  if (super)
	    message_func_decl = umsg_id_super2_stret_fixup_decl;
	  else
	    message_func_decl = objc_is_id (rtype) 
			    	  ? umsg_id_stret_fixup_decl 
				  : umsg_stret_fixup_decl;
        }
      else
	{
	  if (super)
	    message_func_decl = umsg_id_super2_fixup_decl;
	  else
	  /* APPLE LOCAL begin radar 4557598 */
	    {
	      if (objc_is_id (rtype))
		message_func_decl = umsg_id_fixup_decl;
	      else if (targetm.calls.objc_fpreturn_msgcall (ret_type, true))
		message_func_decl = umsg_fixup_decl;
	      else if (targetm.calls.objc_fpreturn_msgcall (ret_type, false))
		message_func_decl = umsg_fpret_fixup_decl;
	      else
		message_func_decl = umsg_fixup_decl;
	    }
	  /* APPLE LOCAL end radar 4557598 */
	}

      selector =  build_selector_messenger_reference (sel_name,  
						      message_func_decl);

      /* selector = &_msg; */
      selector = build_unary_op (ADDR_EXPR, selector, 0);
 
      selector = build_c_cast (super 
			       ? objc_v2_super_selector_type 
			       : objc_v2_selector_type, selector);

      /* (*_msg.messenger) (receiver, &_msg, ...); */
      retval = build_v2_build_objc_method_call (super, method_prototype,
						/* APPLE LOCAL radar 6307941 */
                                                receiver, sel_name, selector, 
                                                method_params,
                                                check_for_nil);
    }
  else
    {
      if (flag_typed_selectors)
        selector = build_typed_selector_reference (sel_name, method_prototype);
      else
/* APPLE LOCAL begin radar 4359757 */
        selector = flag_next_runtime 
		     ? build_next_runtime_selector_reference (sel_name) 
		     : build_selector_reference (sel_name);
/* APPLE LOCAL end radar 4359757 */

        retval = build_objc_method_call (super, method_prototype,
					 /* APPLE LOCAL radar 6307941 */
				         receiver, sel_name, 
				         selector, method_params);
    }
  /* APPLE LOCAL end ObjC new abi */

  current_objc_message_selector = 0;

  return retval;
}

/* Build a tree expression to send OBJECT the operation SELECTOR,
   looking up the method on object LOOKUP_OBJECT (often same as OBJECT),
   assuming the method has prototype METHOD_PROTOTYPE.
   (That is an INSTANCE_METHOD_DECL or CLASS_METHOD_DECL.)
   Use METHOD_PARAMS as list of args to pass to the method.
   If SUPER_FLAG is nonzero, we look up the superclass's method.  */

static tree
build_objc_method_call (int super_flag, tree method_prototype,
			/* APPLE LOCAL radar 6307941 */
			tree lookup_object, tree sel_name, tree selector,
			tree method_params)
{
  tree sender = (super_flag ? umsg_super_decl :
		 (!flag_next_runtime || flag_nil_receivers
		  /* APPLE LOCAL begin radar 4590221 */
		  ? umsg_decl
		  /* APPLE LOCAL end radar 4590221 */
		  : umsg_nonnil_decl));
  tree rcv_p = (super_flag ? objc_super_type : objc_object_type);

  /* If a prototype for the method to be called exists, then cast
     the sender's return type and arguments to match that of the method.
     Otherwise, leave sender as is.  */
  tree ret_type
    = (method_prototype
       ? TREE_VALUE (TREE_TYPE (method_prototype))
       : objc_object_type);
  /* APPLE LOCAL begin radar 3803157 - objc attribute */
  /* APPLE LOCAL radar 6307941 */
  tree method_param_types = get_arg_type_list (sel_name, method_prototype, 
                                               METHOD_REF, super_flag);
  tree ftype = build_function_type (ret_type, method_param_types);
  tree sender_cast;
  tree method, t;
  if (method_prototype && METHOD_TYPE_ATTRIBUTES (method_prototype))
    ftype = build_type_attribute_variant (
	      ftype, METHOD_TYPE_ATTRIBUTES (method_prototype));

  sender_cast = build_pointer_type (ftype);
  lookup_object = build_c_cast (rcv_p, lookup_object);
  /* APPLE LOCAL end radar 3803157 - objc attribute */

  /* Use SAVE_EXPR to avoid evaluating the receiver twice.  */
  lookup_object = save_expr (lookup_object);

  if (flag_next_runtime)
    {
/* APPLE LOCAL begin radar 4491211 */
#ifdef OBJCPLUS
      complete_type (ret_type);
#endif
/* APPLE LOCAL end radar 4491211 */
      /* If we are returning a struct in memory, and the address
	 of that memory location is passed as a hidden first
	 argument, then change which messenger entry point this
	 expr will call.  NB: Note that sender_cast remains
	 unchanged (it already has a struct return type).  */
      /* APPLE LOCAL begin ARM 4961719 */
      if (aggregate_value_p (ret_type, 0))
      /* APPLE LOCAL end ARM 4961719 */
	sender = (super_flag ? umsg_super_stret_decl :
		flag_nil_receivers ? umsg_stret_decl : umsg_nonnil_stret_decl);
      /* APPLE LOCAL begin radar 4280641 */
      else if (!super_flag && targetm.calls.objc_fpreturn_msgcall (ret_type, true))
	sender = umsg_decl;
      else if (!super_flag && targetm.calls.objc_fpreturn_msgcall (ret_type, false))
	sender = umsg_fpret_decl;
      /* APPLE LOCAL end radar 4280641 */

      method_params = tree_cons (NULL_TREE, lookup_object,
				 tree_cons (NULL_TREE, selector,
					    method_params));
      method = build_fold_addr_expr (sender);
    }
  else
    {
      /* This is the portable (GNU) way.  */
      tree object;

      /* First, call the lookup function to get a pointer to the method,
	 then cast the pointer, then call it with the method arguments.  */

      object = (super_flag ? self_decl : lookup_object);

      t = tree_cons (NULL_TREE, selector, NULL_TREE);
      t = tree_cons (NULL_TREE, lookup_object, t);
      method = build_function_call (sender, t);

      /* Pass the object to the method.  */
      method_params = tree_cons (NULL_TREE, object,
				 tree_cons (NULL_TREE, selector,
					    method_params));
    }

  /* ??? Selector is not at this point something we can use inside
     the compiler itself.  Set it to garbage for the nonce.  */
  t = build3 (OBJ_TYPE_REF, sender_cast, method, lookup_object, size_zero_node);
  return build_function_call (t, method_params);
}


/* APPLE LOCAL begin ObjC new abi */
/* Build the new abi's messaging librrary call. It looks like:
   (*_msg.messanger) (receiver, &_msg, ...)  */

static tree
build_v2_build_objc_method_call (int super_flag, tree method_prototype,
                                 tree lookup_object, tree sel_name, 
                                 tree selector, tree method_params,
                                 bool check_for_nil)
{
  tree ret_val;
  tree sender, rcv_p, t;
  tree ret_type
    = (method_prototype
       ? TREE_VALUE (TREE_TYPE (method_prototype))
       : objc_object_type);
  /* APPLE LOCAL radar 6307941 */
  tree method_param_types = get_arg_type_list (sel_name, method_prototype, 
					       METHOD_REF, super_flag);
  /* APPLE LOCAL begin radar 3803157 - objc attribute */
  tree ftype = build_function_type (ret_type, method_param_types);
  tree sender_cast;

  if (method_prototype && METHOD_TYPE_ATTRIBUTES (method_prototype))
    ftype = build_type_attribute_variant (
	      ftype, METHOD_TYPE_ATTRIBUTES (method_prototype));

  sender_cast = build_pointer_type (ftype);
  /* APPLE LOCAL end radar 3803157 - objc attribute */
  if (check_for_nil)
    method_params = objc_copy_to_temp_side_effect_params (method_param_types, 
							  method_params);

  /* Get &message_ref_t.messenger */
  sender = build_c_cast (build_pointer_type (
			   super_flag
		   	   ? objc_v2_super_imp_type
		   	   : objc_v2_imp_type), selector);

  sender = build_indirect_ref (sender, "unary *");

  rcv_p = (super_flag ? objc_super_type : objc_object_type);

  lookup_object = build_c_cast (rcv_p, lookup_object);

  /* Use SAVE_EXPR to avoid evaluating the receiver twice.  */
  lookup_object = save_expr (lookup_object);

  method_params = tree_cons (NULL_TREE, lookup_object,
                             tree_cons (NULL_TREE, selector,
                                        method_params));
  t = build3 (OBJ_TYPE_REF, sender_cast, sender, lookup_object, size_zero_node);
  ret_val =  build_function_call (t, method_params);
  if (check_for_nil)
    {
      /* receiver != nil ? ret_val : 0 */
      tree ftree;
      tree ifexp;

      if (TREE_CODE (ret_type) == RECORD_TYPE
	  || TREE_CODE (ret_type) == UNION_TYPE)
	ftree = build_constructor_from_list (ret_type, NULL_TREE);
      else
	ftree = fold_convert (ret_type, integer_zero_node);

      ifexp = build_binary_op (NE_EXPR, 
			       lookup_object, 
			       fold_convert (rcv_p, integer_zero_node), 1);

      ret_val = build_conditional_expr (ifexp, ret_val, ftree);
 
    }
  return ret_val;
}

/* Declare variable which holds 'struct protocol_t' meta data.  */
/* APPLE LOCAL begin radar 4695109 */
/* poniter to protocol metadata in new ABI protocol is now generated 
   like the old abi. */
static void
build_v2_protocol_reference (tree p)
{
  tree decl;
  const char *proto_name;

  /* static struct protocol_t  _OBJC_PROTOCOL_$<mumble>; */

  /* APPLE LOCAL begin radar 6255913 */
  proto_name = synth_id_with_class_suffix ("l_OBJC_PROTOCOL_$", p);
  decl = create_hidden_decl (objc_v2_protocol_template, proto_name);
  DECL_WEAK (decl) = 1;
  set_user_assembler_name (decl, proto_name);
  /* APPLE LOCAL end radar 6255913 */
  PROTOCOL_V2_FORWARD_DECL (p) = decl;
  /* LLVM LOCAL begin - radar 5476262 */
#ifdef ENABLE_LLVM
  /* begin radar 7069676 */
  /* Let optimizer know that this decl is not removable.  */
  DECL_PRESERVE_P (decl) = 1;
  /* end radar 7069676 */
  pushdecl_top_level(decl);
#endif
  /* LLVM LOCAL end - radar 5476262 */
}
/* APPLE LOCAL end radar 4695109 */

/* Main routine to build all meta data for all protocols used in a translation unit.  */

static void
generate_v2_protocols (void)
{
  tree p, encoding;
  tree decl;
  tree initlist, protocol_name_expr, refs_decl, refs_expr;

  /* If a protocol was directly referenced, pull in indirect references.  */
  for (p = protocol_chain; p; p = TREE_CHAIN (p))
    /* APPLE LOCAL begin radar 4695109 */
    if (PROTOCOL_V2_FORWARD_DECL (p) && PROTOCOL_LIST (p))
      generate_v2_protocol_references (PROTOCOL_LIST (p));
    /* APPLE LOCAL end radar 4695109 */

  for (p = protocol_chain; p; p = TREE_CHAIN (p))
    {
      tree nst_methods = PROTOCOL_NST_METHODS (p);
      tree cls_methods = PROTOCOL_CLS_METHODS (p);

      /* If protocol wasn't referenced, don't generate any code.  */
      decl = PROTOCOL_V2_FORWARD_DECL (p);

      /* APPLE LOCAL radar 4695109 */
      if (!decl)
	continue;

      /* Make sure we link in the Protocol class. */
      objc_v2_get_class_reference (get_identifier (PROTOCOL_OBJECT_CLASS_NAME));

      while (nst_methods)
	{
	  if (! METHOD_ENCODING (nst_methods))
	    {
	      encoding = encode_method_prototype (nst_methods);
	      METHOD_ENCODING (nst_methods) = encoding;
	    }
	  nst_methods = TREE_CHAIN (nst_methods);
	}

      while (cls_methods)
	{
	  if (! METHOD_ENCODING (cls_methods))
	    {
	      encoding = encode_method_prototype (cls_methods);
	      METHOD_ENCODING (cls_methods) = encoding;
	    }

	  cls_methods = TREE_CHAIN (cls_methods);
	}
      /* APPLE LOCAL begin radar 4695109 */
      generate_v2_method_descriptors (p, false);

      nst_methods = PROTOCOL_OPTIONAL_NST_METHODS (p);
      while (nst_methods)
	{
	  if (! METHOD_ENCODING (nst_methods))
	    {
	      encoding = encode_method_prototype (nst_methods);
	      METHOD_ENCODING (nst_methods) = encoding;
	    }
	  nst_methods = TREE_CHAIN (nst_methods);
	}

      cls_methods = PROTOCOL_OPTIONAL_CLS_METHODS (p);
      while (cls_methods)
	{
	  if (! METHOD_ENCODING (cls_methods))
	    {
	      encoding = encode_method_prototype (cls_methods);
	      METHOD_ENCODING (cls_methods) = encoding;
	    }

	  cls_methods = TREE_CHAIN (cls_methods);
	}

      generate_v2_method_descriptors (p, true);
      /* APPLE LOCAL end radar 4695109 */

      if (PROTOCOL_LIST (p))
	refs_decl = generate_v2_protocol_list (p);
      else
	refs_decl = 0;

      /* static struct objc_protocol _OBJC_PROTOCOL_<mumble>; */
      protocol_name_expr = add_objc_string (PROTOCOL_NAME (p), class_names);

      if (refs_decl)
	refs_expr = convert (build_pointer_type (objc_v2_protocol_template),
			     build_unary_op (ADDR_EXPR, refs_decl, 0));
      else
	/* APPLE LOCAL begin radar 6285794 */
        refs_expr =
          convert (build_pointer_type (objc_v2_protocol_template),
                   integer_zero_node);
	/* APPLE LOCAL end radar 6285794 */

      /* APPLE LOCAL begin radar 4695101 */
      /* Build table of list of properties for this protocol. */
      objc_implementation_context = p;
      generate_v2_property_tables (p);
      /* UOBJC_V2_PROPERTY_decl points to table of properties for 
	 this protocol. */
      /* APPLE LOCAL end radar 4695101 */

      /* UOBJC_V2_INSTANCE_METHODS_decl/UOBJC_V2_CLASS_METHODS_decl are set
	 by generate_v2_method_descriptors, which is called above.  */
      initlist = build_protocol_initializer (TREE_TYPE (decl),
					     protocol_name_expr, refs_expr,
					     UOBJC_V2_INSTANCE_METHODS_decl,
					     UOBJC_V2_CLASS_METHODS_decl, true,
					     /* APPLE LOCAL radar 4695101 */
					     UOBJC_V2_PROPERTY_decl,
					     /* APPLE LOCAL begin radar 4695109 */
					     UOBJC_PROTOCOL_OPT_NST_METHODS_decl, 
					     UOBJC_PROTOCOL_OPT_CLS_METHODS_decl);
					     /* APPLE LOCAL end radar 4695109 */
      finish_var_decl (decl, initlist);
      /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
      /* At -O0, we may have emitted references to the decl earlier. */
      if (!optimize)
        reset_initializer_llvm(decl);
#endif
      /* LLVM LOCAL end */
      /* APPLE LOCAL radar 4533974 - ObjC new protocol - radar 6351990 */
      objc_add_to_protocol_list_chain (p, decl);
    }
}

static void
/* APPLE LOCAL end ObjC new abi */
build_protocol_reference (tree p)
{
  tree decl;
  const char *proto_name;

  /* static struct _objc_protocol _OBJC_PROTOCOL_<mumble>; */

  proto_name = synth_id_with_class_suffix ("_OBJC_PROTOCOL", p);
  decl = start_var_decl (objc_protocol_template, proto_name);
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  /* Force 4 byte alignment for protocols */
  DECL_ALIGN(decl) = 32;
  DECL_USER_ALIGN(decl) = 1;
#endif
  /* LLVM LOCAL end */
  PROTOCOL_FORWARD_DECL (p) = decl;
}

/* This function is called by the parser when (and only when) a
   @protocol() expression is found, in order to compile it.  */
tree
objc_build_protocol_expr (tree protoname)
{
  tree expr;
  tree p = lookup_protocol (protoname);

  if (!p)
    {
      error ("cannot find protocol declaration for %qs",
	     IDENTIFIER_POINTER (protoname));
      return error_mark_node;
    }

  /* APPLE LOCAL begin ObjC abi v2 */
  if (flag_objc_abi == 2)
    {
      /* APPLE LOCAL begin radar 4533974 - ObjC new protocol */
      if (!PROTOCOL_V2_FORWARD_DECL (p))
    	build_v2_protocol_reference (p);
      return objc_v2_get_protocol_reference (p);
      /* APPLE LOCAL end radar 4533974 - ObjC new protocol */
    }
  else
    {
      if (!PROTOCOL_FORWARD_DECL (p))
    	build_protocol_reference (p);

      expr = build_unary_op (ADDR_EXPR, PROTOCOL_FORWARD_DECL (p), 0);
    }
  /* APPLE LOCAL end ObjC abi v2 */

  /* ??? Ideally we'd build the reference with objc_protocol_type directly,
     if we have it, rather than converting it here.  */
  expr = convert (objc_protocol_type, expr);

  /* The @protocol() expression is being compiled into a pointer to a
     statically allocated instance of the Protocol class.  To become
     usable at runtime, the 'isa' pointer of the instance need to be
     fixed up at runtime by the runtime library, to point to the
     actual 'Protocol' class.  */

  /* For the GNU runtime, put the static Protocol instance in the list
     of statically allocated instances, so that we make sure that its
     'isa' pointer is fixed up at runtime by the GNU runtime library
     to point to the Protocol class (at runtime, when loading the
     module, the GNU runtime library loops on the statically allocated
     instances (as found in the defs field in objc_symtab) and fixups
     all the 'isa' pointers of those objects).  */
  if (! flag_next_runtime)
    {
      /* This type is a struct containing the fields of a Protocol
        object.  (Cfr. objc_protocol_type instead is the type of a pointer
        to such a struct).  */
      tree protocol_struct_type = xref_tag
       (RECORD_TYPE, get_identifier (PROTOCOL_OBJECT_CLASS_NAME));
      tree *chain;

      /* Look for the list of Protocol statically allocated instances
        to fixup at runtime.  Create a new list to hold Protocol
        statically allocated instances, if the list is not found.  At
        present there is only another list, holding NSConstantString
        static instances to be fixed up at runtime.  */
      for (chain = &objc_static_instances;
	   *chain && TREE_VALUE (*chain) != protocol_struct_type;
	   chain = &TREE_CHAIN (*chain));
      if (!*chain)
	{
         *chain = tree_cons (NULL_TREE, protocol_struct_type, NULL_TREE);
         add_objc_string (OBJC_TYPE_NAME (protocol_struct_type),
                          class_names);
       }

      /* Add this statically allocated instance to the Protocol list.  */
      TREE_PURPOSE (*chain) = tree_cons (NULL_TREE,
					 PROTOCOL_FORWARD_DECL (p),
					 TREE_PURPOSE (*chain));
    }


  return expr;
}

/* This function is called by the parser when a @selector() expression
   is found, in order to compile it.  It is only called by the parser
   and only to compile a @selector().  */
tree
objc_build_selector_expr (tree selnamelist)
{
  tree selname;

  /* APPLE LOCAL begin radar 5409313 */
  if (selnamelist == NULL_TREE)
    {
      error ("method name missing in @selector");
      return error_mark_node;
    }
  /* APPLE LOCAL end radar 5409313 */
  /* Obtain the full selector name.  */
  if (TREE_CODE (selnamelist) == IDENTIFIER_NODE)
    /* A unary selector.  */
    selname = selnamelist;
  else if (TREE_CODE (selnamelist) == TREE_LIST)
    selname = build_keyword_selector (selnamelist);
  else
    abort ();

  /* If we are required to check @selector() expressions as they
     are found, check that the selector has been declared.  */
  if (warn_undeclared_selector)
    {
      /* Look the selector up in the list of all known class and
         instance methods (up to this line) to check that the selector
         exists.  */
      hash hsh;

      /* First try with instance methods.  */
      hsh = hash_lookup (nst_method_hash_list, selname);

      /* If not found, try with class methods.  */
      if (!hsh)
	{
	  hsh = hash_lookup (cls_method_hash_list, selname);
	}

      /* If still not found, print out a warning.  */
      if (!hsh)
	{
	  warning (0, "undeclared selector %qs", IDENTIFIER_POINTER (selname));
	}
    }


  if (flag_typed_selectors)
    return build_typed_selector_reference (selname, 0);
  else
/* APPLE LOCAL begin radar 4359757 */
    return flag_next_runtime 
	     ? build_next_runtime_selector_reference (selname) 
	     : build_selector_reference (selname);
/* APPLE LOCAL end radar 4359757 */
}

tree
objc_build_encode_expr (tree type)
{
  tree result;
  const char *string;

  encode_type (type, obstack_object_size (&util_obstack),
	       OBJC_ENCODE_INLINE_DEFS);
  obstack_1grow (&util_obstack, 0);    /* null terminate string */
  string = obstack_finish (&util_obstack);

  /* Synthesize a string that represents the encoded struct/union.  */
  result = my_build_string (strlen (string) + 1, string);
  obstack_free (&util_obstack, util_firstobj);
  return result;
}

/* APPLE LOCAL begin radar 6230701 */
/* Find a 'self' declaration in this block.  If not found,
   add a 'const' copy in current block. */
static void
access_block_ref_self_decl (tree *decl)
{
  if (lookup_name_in_block (DECL_NAME (*decl), decl))
    *decl = lookup_name (DECL_NAME (*decl));
  else
    *decl = build_block_ref_decl (DECL_NAME (*decl), *decl);
  gcc_assert (*decl);
}
/* APPLE LOCAL end radar 6230701 */

static tree
build_ivar_reference (tree id)
{
  /* APPLE LOCAL radar 5811191 - blocks */
  tree decl;
  /* APPLE LOCAL ObjC new abi */
  tree ivar, base;
  if (TREE_CODE (objc_method_context) == CLASS_METHOD_DECL)
    {
      /* Historically, a class method that produced objects (factory
	 method) would assign `self' to the instance that it
	 allocated.  This would effectively turn the class method into
	 an instance method.  Following this assignment, the instance
	 variables could be accessed.  That practice, while safe,
	 violates the simple rule that a class method should not refer
	 to an instance variable.  It's better to catch the cases
	 where this is done unknowingly than to support the above
	 paradigm.  */
      /* APPLE LOCAL begin radar 7901648 */
      if (cur_block) {
        error ("instance variable %qs accessed in class method",
	        IDENTIFIER_POINTER (id));
        return error_mark_node;
      }
      warning (0, "instance variable %qs accessed in class method",
                  IDENTIFIER_POINTER (id));
      self_decl = convert (objc_instance_type, self_decl); /* cast */
      /* APPLE LOCAL end radar 7901648 */
    }

  /* APPLE LOCAL begin ObjC new abi */
  /* APPLE LOCAL begin radar 5811191 - blocks */
  decl = self_decl;
  if (cur_block)
  {
    /* Find a 'self' declaration in this block.  If not found,
       add a 'const' copy in current block. */
    access_block_ref_self_decl (&decl);
    gcc_assert (decl);
  }
  base = build_indirect_ref (decl, "->");
  /* APPLE LOCAL end radar 5811191 - blocks */
  if ((ivar = objc_v2_build_ivar_ref (base, id)))
    return ivar;
  else
    return objc_build_component_ref (base, id);
  /* APPLE LOCAL end ObjC new abi */
}

/* Compute a hash value for a given method SEL_NAME.  */

static size_t
hash_func (tree sel_name)
{
  const unsigned char *s
    = (const unsigned char *)IDENTIFIER_POINTER (sel_name);
  size_t h = 0;

  while (*s)
    h = h * 67 + *s++ - 113;
  return h;
}

static void
hash_init (void)
{
  nst_method_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  cls_method_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  /* APPLE LOCAL begin radar 4359757 */
  class_nst_method_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  class_cls_method_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  class_names_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  meth_var_names_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  meth_var_types_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  prop_names_attr_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  sel_ref_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  /* APPLE LOCAL end radar 4359757 */

  /* APPLE LOCAL begin radar 4345837 */
  cls_name_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  als_name_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  /* APPLE LOCAL end radar 4345837 */

  /* APPLE LOCAL begin radar 4441049 */
  ivar_offset_hash_list
    = (hash *) ggc_alloc_cleared (SIZEHASHTABLE * sizeof (hash));
  /* APPLE LOCAL end radar 4441049 */

  /* Initialize the hash table used to hold the constant string objects.  */
  string_htab = htab_create_ggc (31, string_hash,
				   string_eq, NULL);
/* APPLE LOCAL begin radar 4204796 */
  /* code removed */
}
/* APPLE LOCAL end radar 4204796 */
/* APPLE LOCAL begin radar 4345837 */
/* This routine adds sel_name to the hash list. sel_name  is a class or alias
   name for the class. If alias name, then value is its underlying class.
   If class, the value is NULL_TREE. */

static void
hash_class_name_enter (hash *hashlist, tree sel_name, tree value)
{
  hash obj;
  int slot = hash_func (sel_name) % SIZEHASHTABLE;

  obj = (hash) ggc_alloc (sizeof (struct hashed_entry));
  if (value != NULL_TREE)
    {
      /* Save the underlying class for the 'alias' in the hash table */
      attr obj_attr = (attr) ggc_alloc (sizeof (struct hashed_attribute));
      obj_attr->value = value;
      obj->list = obj_attr;
    }
  else
    obj->list = 0;
  obj->next = hashlist[slot];
  obj->key = sel_name;

  hashlist[slot] = obj;         /* append to front */

}

/* Searches in the hash table looking for a match for class or alias name.  */

static hash
hash_class_name_lookup (hash *hashlist, tree sel_name)
{
  hash target;

  target = hashlist[hash_func (sel_name) % SIZEHASHTABLE];

  while (target)
    {
      if (sel_name == target->key)
	return target;

      target = target->next;
    }
  return 0;
}
/* APPLE LOCAL end radar 4345837 */

/* APPLE LOCAL begin radar 4441049 */
/* This routine is given an extern variable and enters it in its hash table.
   Note that hashing is done on its inner IDENTIFIER_NODE node.  */

static void 
hash_name_enter (hash *hashlist, tree ivar)
{
  hash obj;
  int slot = hash_func (DECL_NAME (ivar)) % SIZEHASHTABLE;

  obj = (hash) ggc_alloc (sizeof (struct hashed_entry));
  obj->list = 0;
  obj->next = hashlist[slot];
  obj->key = ivar;

  hashlist[slot] = obj;		/* append to front */
}

/* This routine is given a name and returns a matching extern variable if 
   one is found.  */

static tree
hash_name_lookup (hash *hashlist, tree ivar_name)
{
  hash target;

  target = hashlist[hash_func (ivar_name) % SIZEHASHTABLE];

  while (target)
    {
      if (ivar_name == DECL_NAME (target->key))
	return target->key;

      target = target->next;
    }
  return 0;
}
/* APPLE LOCAL end radar 4441049 */

/* WARNING!!!!  hash_enter is called with a method, and will peek
   inside to find its selector!  But hash_lookup is given a selector
   directly, and looks for the selector that's inside the found
   entry's key (method) for comparison.  */

static void
hash_enter (hash *hashlist, tree method)
{
  hash obj;
  int slot = hash_func (METHOD_SEL_NAME (method)) % SIZEHASHTABLE;

  obj = (hash) ggc_alloc (sizeof (struct hashed_entry));
  obj->list = 0;
  obj->next = hashlist[slot];
  obj->key = method;

  hashlist[slot] = obj;		/* append to front */
}

static hash
hash_lookup (hash *hashlist, tree sel_name)
{
  hash target;

  target = hashlist[hash_func (sel_name) % SIZEHASHTABLE];

  while (target)
    {
      if (sel_name == METHOD_SEL_NAME (target->key))
	return target;

      target = target->next;
    }
  return 0;
}

static void
hash_add_attr (hash entry, tree value)
{
  attr obj;

  obj = (attr) ggc_alloc (sizeof (struct hashed_attribute));
  obj->next = entry->list;
  obj->value = value;

  entry->list = obj;		/* append to front */
}

static tree
lookup_method (tree mchain, tree method)
{
  tree key;

  if (TREE_CODE (method) == IDENTIFIER_NODE)
    key = method;
  else
    key = METHOD_SEL_NAME (method);

  while (mchain)
    {
      if (METHOD_SEL_NAME (mchain) == key)
	return mchain;

      mchain = TREE_CHAIN (mchain);
    }
  return NULL_TREE;
}

/* APPLE LOCAL begin radar 4359757 */
/* This routine does a hash lookup keyed on method name and further
   qualified by its 'class'. */
static hash
class_method_hash_lookup (hash *hashlist, tree sel_name, tree class)
{
  hash target;

  target = hashlist[hash_func (sel_name) % SIZEHASHTABLE];
  while (target)
    {
      if (sel_name == METHOD_SEL_NAME (target->key)
	  && class == METHOD_CONTEXT (target->key))
        return target;
      target = target->next;
    }
  return 0;
}

static void
add_class_method_hash (hash *hash_list, tree method, tree class)
{
  hash_enter (hash_list, method);
  METHOD_CONTEXT (method) = class;
}

/* This routine does a quick hash look up of METHOD_IDENT in global 
   class-method hash table. Hash function is keyed on method name but 
   'context' of method found must also match CLASS. 
*/ 
static tree 
fast_lookup_method (int is_class, tree class, tree method_ident)
{
  hash hsh = (!is_class) ? class_method_hash_lookup (class_nst_method_hash_list, 
				              	     method_ident, class) 
			 : class_method_hash_lookup (class_cls_method_hash_list, 
					             method_ident, class);
  return !hsh ? NULL_TREE : hsh->key;
}
/* APPLE LOCAL end radar 4359757 */

/* Look up a class (if OBJC_LOOKUP_CLASS is set in FLAGS) or instance method
   in INTERFACE, along with any categories and protocols attached thereto.
   If method is not found, and the OBJC_LOOKUP_NO_SUPER is _not_ set in FLAGS,
   recursively examine the INTERFACE's superclass.  If OBJC_LOOKUP_CLASS is
   set, OBJC_LOOKUP_NO_SUPER is cleared, and no suitable class method could
   be found in INTERFACE or any of its superclasses, look for an _instance_
   method of the same name in the root class as a last resort.

   If a suitable method cannot be found, return NULL_TREE.  */

static tree
/* APPLE LOCAL objc new property */
lookup_method_static (tree interface, tree ident, int flags, bool property_lookup,
/* APPLE LOCAL radar 5277239 */
		      bool class_method_only)
{
  tree meth = NULL_TREE, root_inter = NULL_TREE;
  tree inter = interface;
  int is_class = (flags & OBJC_LOOKUP_CLASS);
  int no_superclasses = (flags & OBJC_LOOKUP_NO_SUPER);

  while (inter)
    {
      tree chain = is_class ? CLASS_CLS_METHODS (inter) : CLASS_NST_METHODS (inter);
      tree category = inter;

      /* First, look up the method in the class itself.  */
      /* APPLE LOCAL begin objc new property - radar 4359757 */
      if ((meth = fast_lookup_method (is_class, inter, ident)) 
	  && (!property_lookup || METHOD_PROPERTY_CONTEXT (meth)))
      /* APPLE LOCAL end objc new property */
	return meth;

      /* Failing that, look for the method in each category of the class.  */
      while ((category = CLASS_CATEGORY_LIST (category)))
	{
	  chain = is_class ? CLASS_CLS_METHODS (category) : CLASS_NST_METHODS (category);

	  /* Check directly in each category.  */
	  /* APPLE LOCAL begin objc new property */
	  if ((meth = lookup_method (chain, ident))
	      && (!property_lookup || METHOD_PROPERTY_CONTEXT (meth)))
	  /* APPLE LOCAL end objc new property */
	    return meth;

	  /* Failing that, check in each category's protocols.  */
	  if (CLASS_PROTOCOL_LIST (category))
	    {
	      /* APPLE LOCAL begin objc new property */
	      if ((meth = (lookup_method_in_protocol_list
			   (CLASS_PROTOCOL_LIST (category), ident, is_class)))
		  && (!property_lookup || METHOD_PROPERTY_CONTEXT (meth)))
	      /* APPLE LOCAL end objc new property */
		return meth;
	    }
	}

      /* If not found in categories, check in protocols of the main class.  */
      if (CLASS_PROTOCOL_LIST (inter))
	{
	  /* APPLE LOCAL begin objc new property */
	  if ((meth = (lookup_method_in_protocol_list
		       (CLASS_PROTOCOL_LIST (inter), ident, is_class)))
	      && (!property_lookup || METHOD_PROPERTY_CONTEXT (meth)))
	  /* APPLE LOCAL end objc new property */
	    return meth;
	}

      /* If we were instructed not to look in superclasses, don't.  */
      if (no_superclasses)
	return NULL_TREE;

      /* Failing that, climb up the inheritance hierarchy.  */
      root_inter = inter;
      inter = lookup_interface (CLASS_SUPER_NAME (inter));
    }
  while (inter);

  /* If no class (factory) method was found, check if an _instance_
     method of the same name exists in the root class.  This is what
     the Objective-C runtime will do.  If an instance method was not
     found, return 0.  */
  /* APPLE LOCAL begin radar 5277239 */
  /* APPLE LOCAL objc new property */
  return (is_class && !class_method_only) 
	   ? lookup_method_static (root_inter, ident, 0, property_lookup, false): NULL_TREE;
  /* APPLE LOCAL end radar 5277239 */
}

/* Add the method to the hash list if it doesn't contain an identical
   method already. */

static void
add_method_to_hash_list (hash *hash_list, tree method)
{
  hash hsh;

  if (!(hsh = hash_lookup (hash_list, METHOD_SEL_NAME (method))))
    {
      /* Install on a global chain.  */
      hash_enter (hash_list, method);
    }
  else
    {
      /* Check types against those; if different, add to a list.  */
      attr loop;
      /* APPLE LOCAL radar 5370783 */
      int already_there = comp_proto_with_proto (method, hsh->key, 2);
      for (loop = hsh->list; !already_there && loop; loop = loop->next)
        /* APPLE LOCAL radar 5370783 */
	already_there |= comp_proto_with_proto (method, loop->value, 2);
      if (!already_there)
	hash_add_attr (hsh, method);
    }
}

static tree
/* APPLE LOCAL C* language */
objc_add_method (tree class, tree method, int is_class, int is_optional)
{
  tree mth;

  /* APPLE LOCAL begin C* language */
  /* @optional methods are added to protocol's OPTIONAL list */
  if (is_optional)
    {
      gcc_assert (TREE_CODE (class) == PROTOCOL_INTERFACE_TYPE);
      if (!(mth = lookup_method (is_class
                                ? PROTOCOL_OPTIONAL_CLS_METHODS (class)
                                : PROTOCOL_OPTIONAL_NST_METHODS (class), method)))
	{
	  if (is_class)
	    {
	      TREE_CHAIN (method) = PROTOCOL_OPTIONAL_CLS_METHODS (class);
	      PROTOCOL_OPTIONAL_CLS_METHODS (class) = method;
	    }
	  else
	    {
	      TREE_CHAIN (method) = PROTOCOL_OPTIONAL_NST_METHODS (class);
	      PROTOCOL_OPTIONAL_NST_METHODS (class) = method;
	    }
	}
    }
  else
  /* APPLE LOCAL end C* language */
  /* APPLE LOCAL begin radar 4359757 */
  if (!(mth = fast_lookup_method (is_class, class, METHOD_SEL_NAME (method))))
  /* APPLE LOCAL end radar 4359757 */
    {
      /* put method on list in reverse order */
      if (is_class)
	{
	  TREE_CHAIN (method) = CLASS_CLS_METHODS (class);
	  CLASS_CLS_METHODS (class) = method;
	}
      else
	{
	  TREE_CHAIN (method) = CLASS_NST_METHODS (class);
	  CLASS_NST_METHODS (class) = method;
	}
      /* APPLE LOCAL begin radar 4359757 */
      add_class_method_hash ((!is_class) ? class_nst_method_hash_list
                                         : class_cls_method_hash_list,
                              method, class);
      /* APPLE LOCAL end radar 4359757 */
    }
  else
    {
      /* When processing an @interface for a class or category, give hard
	 errors on methods with identical selectors but differing argument
	 and/or return types. We do not do this for @implementations, because
	 C/C++ will do it for us (i.e., there will be duplicate function
	 definition errors).  */
      if ((TREE_CODE (class) == CLASS_INTERFACE_TYPE
	   || TREE_CODE (class) == CATEGORY_INTERFACE_TYPE)
          /* APPLE LOCAL radar 5370783 */
	  && !comp_proto_with_proto (method, mth, 2))
	error ("duplicate declaration of method %<%c%s%>",
		is_class ? '+' : '-',
		IDENTIFIER_POINTER (METHOD_SEL_NAME (mth)));
    }

  if (is_class)
    add_method_to_hash_list (cls_method_hash_list, method);
  else
    {
      add_method_to_hash_list (nst_method_hash_list, method);

      /* Instance methods in root classes (and categories thereof)
	 may act as class methods as a last resort.  We also add
	 instance methods listed in @protocol declarations to
	 the class hash table, on the assumption that @protocols
	 may be adopted by root classes or categories.  */
      if (TREE_CODE (class) == CATEGORY_INTERFACE_TYPE
	  || TREE_CODE (class) == CATEGORY_IMPLEMENTATION_TYPE)
	class = lookup_interface (CLASS_NAME (class));

      if (TREE_CODE (class) == PROTOCOL_INTERFACE_TYPE
	  || !CLASS_SUPER_NAME (class))
	add_method_to_hash_list (cls_method_hash_list, method);
    }

  return method;
}

static tree
add_class (tree class_name, tree name)
{
  struct interface_tuple **slot;

  /* Put interfaces on list in reverse order.  */
  TREE_CHAIN (class_name) = interface_chain;
  interface_chain = class_name;

  if (interface_htab == NULL)
    interface_htab = htab_create_ggc (31, hash_interface, eq_interface, NULL);
  slot = (struct interface_tuple **)
    htab_find_slot_with_hash (interface_htab, name,
			      IDENTIFIER_HASH_VALUE (name),
			      INSERT);
  if (!*slot)
    {
      *slot = (struct interface_tuple *) ggc_alloc_cleared (sizeof (struct interface_tuple));
      (*slot)->id = name;
    }
  (*slot)->class_name = class_name;

  return interface_chain;
}

/* APPLE LOCAL begin radar 4548636 */
/* This routine updates TREE_DEPRECATED/TREE_UNAVAILABLE fields of 'record' 
   type of the class. */
static void
objc_attach_attrs_to_type (tree type, tree class)
{
  tree chain;

  if (!CLASS_ATTRIBUTES (class))
    return;  /* The most common case. */

  for (chain = CLASS_ATTRIBUTES (class); chain; chain = TREE_CHAIN (chain))
    {
      if (is_attribute_p ("deprecated", TREE_PURPOSE (chain)))
	TREE_DEPRECATED (type) = 1;
      else if (is_attribute_p ("unavailable", TREE_PURPOSE (chain)))
	TREE_UNAVAILABLE (type) = 1;
    }
  return;
}

/* Warn when a 'class' with deprecated/available attribute 
   set is used. */

static void
objc_warn_on_class_attributes (tree class, bool use)
{
  tree chain;

  if (!class || !CLASS_ATTRIBUTES (class))
    return;  /* The most common case. */
  for (chain = CLASS_ATTRIBUTES (class); chain; chain = TREE_CHAIN (chain))
    {
      if (is_attribute_p ("deprecated", TREE_PURPOSE (chain)))
	{
	  if (use)
	    warning (0, "class %qs is deprecated",
		     IDENTIFIER_POINTER (CLASS_NAME (class))); 
	}
      else if (is_attribute_p ("unavailable", TREE_PURPOSE (chain)))
	{
	  if (use)
	    warning (0, "class %qs is unavailable",
		     IDENTIFIER_POINTER (CLASS_NAME (class)));
	}
      /* APPLE LOCAL begin radar 4705298, 48431458 */
      else if (!use && is_attribute_p ("visibility", TREE_PURPOSE (chain)))
        {
          tree id = TREE_VALUE (TREE_VALUE ((chain)));
          const char *attr_arg = TREE_STRING_POINTER (id);
          if (strcmp (attr_arg, "hidden") && strcmp (attr_arg, "default"))
            warning (0, "only \"hidden\" or \"default\" visibility attribute allowed on a class");
          return;
        }
      /* APPLE LOCAL end radar 4705298, 48431458 */
      /* APPLE LOCAL begin radar 5008110 */
      else if (is_attribute_p ("objc_exception", TREE_PURPOSE (chain)))
        ;
      /* APPLE LOCAL end radar 5008110 */
      /* APPLE LOCAL begin radar 7865106 */
      else if (is_attribute_p ("weak_import", TREE_PURPOSE (chain)))
        ;
      /* APPLE LOCAL end radar 7865106 */
      else if (!use)
	warning (0, "attribute %s is unknown - ignored",
		  IDENTIFIER_POINTER (TREE_PURPOSE (chain)));
    }
  return;
}
/* APPLE LOCAL end radar 4548636 */

static void
add_category (tree class, tree category)
{
  /* Put categories on list in reverse order.  */
  tree cat = lookup_category (class, CLASS_SUPER_NAME (category));

  if (cat)
    {
      warning (0, "duplicate interface declaration for category %<%s(%s)%>",
	       IDENTIFIER_POINTER (CLASS_NAME (class)),
	       IDENTIFIER_POINTER (CLASS_SUPER_NAME (category)));
    }
  else
    {
      CLASS_CATEGORY_LIST (category) = CLASS_CATEGORY_LIST (class);
      CLASS_CATEGORY_LIST (class) = category;
    }
  /* APPLE LOCAL radar 4548636 */
  objc_warn_on_class_attributes (class, true);
}

/* Called after parsing each instance variable declaration. Necessary to
   preserve typedefs and implement public/private...

   PUBLIC is 1 for public, 0 for protected, and 2 for private.  */

static tree
add_instance_variable (tree class, int public, tree field_decl)
{
  tree field_type = TREE_TYPE (field_decl);
  const char *ivar_name = DECL_NAME (field_decl)
			  ? IDENTIFIER_POINTER (DECL_NAME (field_decl))
			  : "<unnamed>";

#ifdef OBJCPLUS
  if (TREE_CODE (field_type) == REFERENCE_TYPE)
    {
      error ("illegal reference type specified for instance variable %qs",
	     ivar_name);
      /* Return class as is without adding this ivar.  */
      return class;
    }
#endif

  if (field_type == error_mark_node || !TYPE_SIZE (field_type)
      || TYPE_SIZE (field_type) == error_mark_node)
      /* 'type[0]' is allowed, but 'type[]' is not! */
    {
      error ("instance variable %qs has unknown size", ivar_name);
      /* Return class as is without adding this ivar.  */
      return class;
    }

#ifdef OBJCPLUS
  /* Check if the ivar being added has a non-POD C++ type.   If so, we will
     need to either (1) warn the user about it or (2) generate suitable
     constructor/destructor call from '- .cxx_construct' or '- .cxx_destruct'
     methods (if '-fobjc-call-cxx-cdtors' was specified).  */
  if (IS_AGGR_TYPE (field_type)
      && (TYPE_NEEDS_CONSTRUCTING (field_type)
	  || TYPE_HAS_NONTRIVIAL_DESTRUCTOR (field_type)
	  || TYPE_POLYMORPHIC_P (field_type)))
    {
      const char *type_name = IDENTIFIER_POINTER (OBJC_TYPE_NAME (field_type));

      if (flag_objc_call_cxx_cdtors)
        {
	  /* Since the ObjC runtime will be calling the constructors and
	     destructors for us, the only thing we can't handle is the lack
	     of a default constructor.  */
	  if (TYPE_NEEDS_CONSTRUCTING (field_type)
	      && !TYPE_HAS_DEFAULT_CONSTRUCTOR (field_type))
	    {
	      warning (0, "type %qs has no default constructor to call",
		       type_name);

	      /* If we cannot call a constructor, we should also avoid
		 calling the destructor, for symmetry.  */
	      if (TYPE_HAS_NONTRIVIAL_DESTRUCTOR (field_type))
		warning (0, "destructor for %qs shall not be run either",
			 type_name);
	    }
        }
      else
	{
	  static bool warn_cxx_ivars = false;

	  if (TYPE_POLYMORPHIC_P (field_type))
	    {
	      /* Vtable pointers are Real Bad(tm), since Obj-C cannot
		 initialize them.  */
	      error ("type %qs has virtual member functions", type_name);
	      error ("illegal aggregate type %qs specified "
		     "for instance variable %qs",
		     type_name, ivar_name);
	      /* Return class as is without adding this ivar.  */
	      return class;
	    }

	  /* User-defined constructors and destructors are not known to Obj-C
	     and hence will not be called.  This may or may not be a problem. */
	  if (TYPE_NEEDS_CONSTRUCTING (field_type))
	    warning (0, "type %qs has a user-defined constructor", type_name);
	  if (TYPE_HAS_NONTRIVIAL_DESTRUCTOR (field_type))
	    warning (0, "type %qs has a user-defined destructor", type_name);

	  if (!warn_cxx_ivars)
	    {
	      warning (0, "C++ constructors and destructors will not "
		       "be invoked for Objective-C fields");
	      warn_cxx_ivars = true;
	    }
	}
    }
#endif

  /* Overload the public attribute, it is not used for FIELD_DECLs.  */
  switch (public)
    {
    case 0:
      TREE_PUBLIC (field_decl) = 0;
      TREE_PRIVATE (field_decl) = 0;
      TREE_PROTECTED (field_decl) = 1;
      /* APPLE LOCAL radar 4951615 */
      IVAR_PUBLIC_OR_PROTECTED (field_decl) = 1;
      break;

    /* APPLE LOCAL radar 4564694 */
    case 3: /* @package */
    case 1:
      TREE_PUBLIC (field_decl) = 1;
      /* APPLE LOCAL radar 4951615, radar 4564694 */
      IVAR_PUBLIC_OR_PROTECTED (field_decl) = (flag_objc_abi == 2 && public == 3) ? 0 : 1;
      TREE_PRIVATE (field_decl) = 0;
      TREE_PROTECTED (field_decl) = 0;
      break;

    case 2:
      TREE_PUBLIC (field_decl) = 0;
      TREE_PRIVATE (field_decl) = 1;
      TREE_PROTECTED (field_decl) = 0;
      /* APPLE LOCAL radar 4951615 */
      IVAR_PUBLIC_OR_PROTECTED (field_decl) = 0;
      break;

    }

  CLASS_RAW_IVARS (class) = chainon (CLASS_RAW_IVARS (class), field_decl);

  return class;
}

static tree
is_ivar (tree decl_chain, tree ident)
{
  for ( ; decl_chain; decl_chain = TREE_CHAIN (decl_chain))
    if (DECL_NAME (decl_chain) == ident)
      return decl_chain;
  return NULL_TREE;
}

/* APPLE LOCAL begin radar 4653319 */
/* Just like is_ivar, except it also looks into inheritance chain. */

static tree
nested_ivar_lookup (tree interface, tree ident)
{
  tree ivar_chain = CLASS_IVARS (interface);
  tree res = is_ivar (ivar_chain, ident);
  if (res)
    return res;

  while (CLASS_SUPER_NAME (interface))
    {
      interface = lookup_interface (CLASS_SUPER_NAME (interface));
      if (interface)
        return nested_ivar_lookup (interface, ident);
    }
  return res;
}

/* APPLE LOCAL end radar 4653319 */

/* True if the ivar is private and we are not in its implementation.  */

static int
is_private (tree decl)
{
  return (TREE_PRIVATE (decl)
	  && ! is_ivar (CLASS_IVARS (implementation_template),
			DECL_NAME (decl)));
}

/* We have an instance variable reference;, check to see if it is public.  */

int
objc_is_public (tree expr, tree identifier)
{
  tree basetype, decl;

#ifdef OBJCPLUS
  if (processing_template_decl)
    return 1;
#endif
  /* APPLE LOCAL begin radar 4862848 */
  if (expr == error_mark_node)
    return 0;
  /* APPLE LOCAL end radar 4862848 */

  if (TREE_TYPE (expr) == error_mark_node)
    return 1;

  basetype = TYPE_MAIN_VARIANT (TREE_TYPE (expr));

  if (basetype && TREE_CODE (basetype) == RECORD_TYPE)
    {
      if (TYPE_HAS_OBJC_INFO (basetype) && TYPE_OBJC_INTERFACE (basetype))
	{
	  tree class = lookup_interface (OBJC_TYPE_NAME (basetype));

	  if (!class)
	    {
	      error ("cannot find interface declaration for %qs",
		     IDENTIFIER_POINTER (OBJC_TYPE_NAME (basetype)));
	      return 0;
	    }

	  if ((decl = is_ivar (get_class_ivars (class, true), identifier)))
	    {
	      if (TREE_PUBLIC (decl))
		return 1;

	      /* Important difference between the Stepstone translator:
		 all instance variables should be public within the context
		 of the implementation.  */
	      if (objc_implementation_context
		 && ((TREE_CODE (objc_implementation_context)
		      == CLASS_IMPLEMENTATION_TYPE)
		     || (TREE_CODE (objc_implementation_context)
			 == CATEGORY_IMPLEMENTATION_TYPE)))
		{
		  tree curtype = TYPE_MAIN_VARIANT
				 (CLASS_STATIC_TEMPLATE
				  (implementation_template));

		  if (basetype == curtype
		      || DERIVED_FROM_P (basetype, curtype))
		    {
		      int private = is_private (decl);

		      if (private)
			error ("instance variable %qs is declared private",
			       IDENTIFIER_POINTER (DECL_NAME (decl)));

		      return !private;
		    }
		}

	      /* The 2.95.2 compiler sometimes allowed C functions to access
		 non-@public ivars.  We will let this slide for now...  */
	      if (!objc_method_context)
	      {
		/* APPLE LOCAL begin radar 4805612 */
		/* Now must allow access to private/protected ivar through an object 
		   pointer in a nested C function. */
		if (objc_interface_context &&
		    (TREE_CODE (objc_interface_context) == CLASS_INTERFACE_TYPE
		     || TREE_CODE (objc_interface_context) == CATEGORY_INTERFACE_TYPE))
		  {
                    /* APPLE LOCAL begin radar 5471096 */
                    tree class = 
                      TREE_CODE (objc_interface_context) == CLASS_INTERFACE_TYPE 
                        ? objc_interface_context 
                        : lookup_interface (CLASS_NAME (objc_interface_context));
		    tree curtype = 
                      TYPE_MAIN_VARIANT (CLASS_STATIC_TEMPLATE (class));
                    /* APPLE LOCAL end radar 5471096 */
		    if (basetype == curtype
			|| DERIVED_FROM_P (basetype, curtype))
		     {
		       int private;
		       gcc_assert (!implementation_template);
		       implementation_template = objc_interface_context; /* ref'ed in is_private */
		       private = is_private (decl);
		       implementation_template = NULL_TREE;
		       if (private)
			 error ("instance variable %qs is declared private",
				IDENTIFIER_POINTER (DECL_NAME (decl)));
		       return !private;
		     }
		  }
		/* APPLE LOCAL end radar 4805612 */
		warning (0, "instance variable %qs is %s; "
			 "this will be a hard error in the future",
			 IDENTIFIER_POINTER (identifier),
			 TREE_PRIVATE (decl) ? "@private" : "@protected");
		return 1;
	      }

	      error ("instance variable %qs is declared %s",
		     IDENTIFIER_POINTER (identifier),
		     TREE_PRIVATE (decl) ? "private" : "protected");
	      return 0;
	    }
	}
    }

  return 1;
}
/* APPLE LOCAL begin radar 4568791 */
/* This routine gets called when there is no implementation of method declared
   in protocol in current class. This used to cause a warning, but now rules
   are relaxed so if the method has been declared in an inherited class no warning
   will be issued. 
*/
static bool
objc_method_inherited (tree meth, int mtype, tree interface)
{
  tree list;
  tree inter = interface;

  while ((inter = lookup_interface (CLASS_SUPER_NAME (inter))))
    {
      list = (mtype == '+') ? CLASS_CLS_METHODS (inter) 
			    : CLASS_NST_METHODS (inter);
    if (lookup_method (list, meth))
      return true;
    }

  return false;
}
/* APPLE LOCAL end radar 4568791 */

/* APPLE LOCAL begin radar 4449535 */
/* This routine checks to see if class implements forwardInvocation method and objects 
   of this class are derived from 'NSProxy' so that to forward requests from one object 
   to another. It then returns 'true' or 'false' accordingly.
   Under such conditions, which means that every method possible is implemented in the class,
   gcc should not issue "Method definition not found" warnings.
*/
static bool
objc_impl_forwardInvocation (tree context)
{
  tree inherit;
  if (fast_lookup_method (false, context, get_identifier ("forwardInvocation:"))
      == NULL_TREE)
    return false;
  inherit = lookup_interface (CLASS_SUPER_NAME (context));
  while (inherit)
    {
      if (!strcmp (IDENTIFIER_POINTER (CLASS_NAME (inherit)), "NSProxy"))
	return true;
      inherit = lookup_interface (CLASS_SUPER_NAME (inherit));
    }
  return false;
}
/* APPLE LOCAL end radar 4449535 */

/* APPLE LOCAL begin radar 5453108 */
/* This routine checks to see if there is a setter/getter method declared in current
   class. If so, it returns 'true' so caller does not issue a warning. This is 
   the case of a method declared in class's protocol which matches class's getter/settter.
*/
static bool
property_method_in_class (tree meth, bool inst_method, tree interface)
{
  tree list, property_method;
  if (!interface || !inst_method)
    return false;
  list = CLASS_NST_METHODS (interface);

  property_method = lookup_method (list, meth);
  return (property_method && 
	  METHOD_PROPERTY_CONTEXT (property_method) != NULL); 
}
/* APPLE LOCAL end radar 5453108 */
/* Make sure all entries in CHAIN are also in LIST.  */

static int
/* APPLE LOCAL radar 4568791, 4359757 */
check_methods (tree chain, int mtype, tree interface)
{
  int first = 1;

  while (chain)
    {
      /* APPLE LOCAL begin radar 4359757 */
      if (!fast_lookup_method (mtype == (int)'+', 
			       objc_implementation_context, METHOD_SEL_NAME (chain)))
      /* APPLE LOCAL end radar 4359757 */
	{
	  /* APPLE LOCAL begin C* property metadata (Radar 4498373) */
	  /* APPLE LOCAL begin radar 5453108 */
	  if (chain != NULL && (METHOD_PROPERTY_CONTEXT (chain) != NULL_TREE ||
				property_method_in_class (chain, mtype == (int)'-', interface)))
	  /* APPLE LOCAL end radar 5453108 */
	    {
	      /* Case of instance method in interface not found in its implementation.
		 This is OK in case of instance method setter/getter declaration synthesized 
		 via property declaration in the interface. Happens for dynamic properties. */
	      gcc_assert (mtype == (int)'-');
	      chain = TREE_CHAIN (chain);
	      continue;
	    }
	  /* APPLE LOCAL end C* property metadata (Radar 4498373) */
	  /* APPLE LOCAL begin radar 4568791 */
	  else if (interface && objc_method_inherited (chain, mtype, interface))
	    {
	      /* Don't issue the warning if method declared in protocol has been
		 declared in the inheritance heirarchy. */
	      chain = TREE_CHAIN (chain);
	      continue;
	    }
	  /* APPLE LOCAL end radar 4568791 */
	  if (first)
	    {
	      if (TREE_CODE (objc_implementation_context)
		  == CLASS_IMPLEMENTATION_TYPE)
		warning (0, "incomplete implementation of class %qs",
			 IDENTIFIER_POINTER (CLASS_NAME (objc_implementation_context)));
	      else if (TREE_CODE (objc_implementation_context)
		       == CATEGORY_IMPLEMENTATION_TYPE)
		warning (0, "incomplete implementation of category %qs",
			 IDENTIFIER_POINTER (CLASS_SUPER_NAME (objc_implementation_context)));
	      first = 0;
	    }

	  warning (0, "method definition for %<%c%s%> not found",
		   mtype, IDENTIFIER_POINTER (METHOD_SEL_NAME (chain)));
	}

      chain = TREE_CHAIN (chain);
    }

    return first;
}

/* Check if CLASS, or its superclasses, explicitly conforms to PROTOCOL.  */

static int
conforms_to_protocol (tree class, tree protocol)
{
   if (TREE_CODE (protocol) == PROTOCOL_INTERFACE_TYPE)
     {
       tree p = CLASS_PROTOCOL_LIST (class);
       while (p && TREE_VALUE (p) != protocol)
	 p = TREE_CHAIN (p);

       if (!p)
	 {
	   tree super = (CLASS_SUPER_NAME (class)
			 ? lookup_interface (CLASS_SUPER_NAME (class))
			 : NULL_TREE);
	   int tmp = super ? conforms_to_protocol (super, protocol) : 0;
	   if (!tmp)
	     return 0;
	 }
     }

   return 1;
}

/* Make sure all methods in CHAIN are accessible as MTYPE methods in
   CONTEXT.  This is one of two mechanisms to check protocol integrity.  */

static int
check_methods_accessible (tree chain, tree context, int mtype)
{
  int first = 1;
  tree list;
  tree base_context = context;

  while (chain)
    {
      /* APPLE LOCAL begin radar 4963113 */
      if (chain != NULL && METHOD_PROPERTY_CONTEXT (chain) != NULL_TREE)
	{
	  /* Case of an accessor in protocol not implemented in @implementation.
	     This is OK when property is declared dynamic. So, with -Wno-protocol
	     no warning must be issued. Cousin of radar 4498373 */
	  gcc_assert (mtype == (int)'-');
	  gcc_assert (TREE_CODE (METHOD_PROPERTY_CONTEXT (chain)) == PROPERTY_DECL);
	  chain = TREE_CHAIN (chain);
	  continue;
        }
      /* APPLE LOCAL end radar 4963113 */
      context = base_context;
      while (context)
	{
	  if (mtype == '+')
	    list = CLASS_CLS_METHODS (context);
	  else
	    list = CLASS_NST_METHODS (context);

	  if (lookup_method (list, chain))
	      break;

	  else if (TREE_CODE (context) == CLASS_IMPLEMENTATION_TYPE
		   || TREE_CODE (context) == CLASS_INTERFACE_TYPE)
	    context = (CLASS_SUPER_NAME (context)
		       ? lookup_interface (CLASS_SUPER_NAME (context))
		       : NULL_TREE);

	  else if (TREE_CODE (context) == CATEGORY_IMPLEMENTATION_TYPE
		   || TREE_CODE (context) == CATEGORY_INTERFACE_TYPE)
	    context = (CLASS_NAME (context)
		       ? lookup_interface (CLASS_NAME (context))
		       : NULL_TREE);
	  else
	    abort ();
	}

      if (context == NULL_TREE)
	{
	  if (first)
	    {
	      if (TREE_CODE (objc_implementation_context)
		  == CLASS_IMPLEMENTATION_TYPE)
		warning (0, "incomplete implementation of class %qs",
			 IDENTIFIER_POINTER
			   (CLASS_NAME (objc_implementation_context)));
	      else if (TREE_CODE (objc_implementation_context)
		       == CATEGORY_IMPLEMENTATION_TYPE)
		warning (0, "incomplete implementation of category %qs",
			 IDENTIFIER_POINTER
			   (CLASS_SUPER_NAME (objc_implementation_context)));
	      first = 0;
	    }
	  warning (0, "method definition for %<%c%s%> not found",
		   mtype, IDENTIFIER_POINTER (METHOD_SEL_NAME (chain)));
	}

      chain = TREE_CHAIN (chain); /* next method...  */
    }
  return first;
}

/* Check whether the current interface (accessible via
   'objc_implementation_context') actually implements protocol P, along
   with any protocols that P inherits.  */

static void
/* APPLE LOCAL radar 4568791 */
check_protocol (tree p, const char *type, const char *name, tree interface)
{
  if (TREE_CODE (p) == PROTOCOL_INTERFACE_TYPE)
    {
      /* APPLE LOCAL radar 4449535 */
      int f1, f2=1;

      /* Ensure that all protocols have bodies!  */
      if (warn_protocol)
	{
	  f1 = check_methods (PROTOCOL_CLS_METHODS (p),
			      /* APPLE LOCAL radar 4359757 */
			      /* argument removed */
			      /* APPLE LOCAL radar 4568791 */
			      '+', interface);
	  /* APPLE LOCAL begin radar 4449535 */
	  if (!objc_impl_forwardInvocation (objc_implementation_context))
	    f2 = check_methods (PROTOCOL_NST_METHODS (p),
			        /* APPLE LOCAL radar 4359757 */
			        /* argument removed */
			        /* APPLE LOCAL radar 4568791 */
			        '-', interface);
	  /* APPLE LOCAL end radar 4449535 */
	}
      else
	{
	  f1 = check_methods_accessible (PROTOCOL_CLS_METHODS (p),
					 objc_implementation_context,
					 '+');
 	  /* APPLE LOCAL begin radar 4449535 */
	  if (!objc_impl_forwardInvocation (objc_implementation_context))
	    f2 = check_methods_accessible (PROTOCOL_NST_METHODS (p),
					   objc_implementation_context,
					   '-');
 	  /* APPLE LOCAL end radar 4449535 */
	}

      if (!f1 || !f2)
	warning (0, "%s %qs does not fully implement the %qs protocol",
		 type, name, IDENTIFIER_POINTER (PROTOCOL_NAME (p)));
    }

  /* Check protocols recursively.  */
  if (PROTOCOL_LIST (p))
    {
      tree subs = PROTOCOL_LIST (p);
      tree super_class =
	lookup_interface (CLASS_SUPER_NAME (implementation_template));

      while (subs)
	{
	  tree sub = TREE_VALUE (subs);

	  /* If the superclass does not conform to the protocols
	     inherited by P, then we must!  */
	  if (!super_class || !conforms_to_protocol (super_class, sub))
	    /* APPLE LOCAL radar 4568791 */
	    check_protocol (sub, type, name, interface);
	  subs = TREE_CHAIN (subs);
	}
    }
}

/* Check whether the current interface (accessible via
   'objc_implementation_context') actually implements the protocols listed
   in PROTO_LIST.  */

static void
/* APPLE LOCAL radar 4568791 */
check_protocols (tree proto_list, const char *type, const char *name, tree interface)
{
  for ( ; proto_list; proto_list = TREE_CHAIN (proto_list))
    {
      tree p = TREE_VALUE (proto_list);

      /* APPLE LOCAL radar 4568791 */
      check_protocol (p, type, name, interface);
    }
}

/* Make sure that the class CLASS_NAME is defined
   CODE says which kind of thing CLASS_NAME ought to be.
   It can be CLASS_INTERFACE_TYPE, CLASS_IMPLEMENTATION_TYPE,
   CATEGORY_INTERFACE_TYPE, or CATEGORY_IMPLEMENTATION_TYPE.  */

static tree
start_class (enum tree_code code, tree class_name, tree super_name,
	     tree protocol_list)
{
  /* APPLE LOCAL radar 5835805 */
  tree class, decl, aliased_class_name;

#ifdef OBJCPLUS
  if (current_namespace != global_namespace) {
    error ("Objective-C declarations may only appear in global scope");
  }
#endif /* OBJCPLUS */

  if (objc_implementation_context)
    {
      warning (0, "%<@end%> missing in implementation context");
      finish_class (objc_implementation_context);
      objc_ivar_chain = NULL_TREE;
      objc_implementation_context = NULL_TREE;
    }

  class = make_node (code);
  TYPE_LANG_SLOT_1 (class) = make_tree_vec (CLASS_LANG_SLOT_ELTS);

  /* Check for existence of the super class, if one was specified.  Note
     that we must have seen an @interface, not just a @class.  If we
     are looking at a @compatibility_alias, traverse it first.  */
  if ((code == CLASS_INTERFACE_TYPE || code == CLASS_IMPLEMENTATION_TYPE)
      && super_name)
    {
      tree super = objc_is_class_name (super_name);
      /* APPLE LOCAL begin radar 4548636 */
      tree super_interface = NULL_TREE;
      if (!super || !(super_interface = lookup_interface (super)))
	{
	  error ("cannot find interface declaration for %qs, superclass of %qs",
		 IDENTIFIER_POINTER (super ? super : super_name),
		 IDENTIFIER_POINTER (class_name));
	  super_name = NULL_TREE;
	}
      else
        {
	  objc_warn_on_class_attributes (super_interface, true);
	  super_name = super;
        }
      /* APPLE LOCAL end radar 4548636 */
    }
  /* APPLE LOCAL begin radar 5835805 */
  aliased_class_name = objc_is_class_name (class_name);
  if (aliased_class_name)
    class_name = aliased_class_name;
  /* APPLE LOCAL end radar 5835805 */
  CLASS_NAME (class) = class_name;
  CLASS_SUPER_NAME (class) = super_name;
  CLASS_CLS_METHODS (class) = NULL_TREE;

  /* APPLE LOCAL radar 4695109 */
  /* PROTOCOL_IMPLEMENTATION_TYPE removed */
  /* APPLE LOCAL radar 5835805 */
  if (!aliased_class_name 
      && (decl = lookup_name (class_name)))
    {
      error ("%qs redeclared as different kind of symbol",
	     IDENTIFIER_POINTER (class_name));
      error ("previous declaration of %q+D",
	     decl);
    }

  if (code == CLASS_IMPLEMENTATION_TYPE)
    {
      {
        tree chain;

        for (chain = implemented_classes; chain; chain = TREE_CHAIN (chain))
           if (TREE_VALUE (chain) == class_name)
	     {
	       error ("reimplementation of class %qs",
		      IDENTIFIER_POINTER (class_name));
	       /* APPLE LOCAL radar 4689268 */
	       return class;
	     }
        implemented_classes = tree_cons (NULL_TREE, class_name,
					 implemented_classes);
      }

      /* Reset for multiple classes per file.  */
      method_slot = 0;

      objc_implementation_context = class;

      /* Lookup the interface for this implementation.  */

      if (!(implementation_template = lookup_interface (class_name)))
        {
	  warning (0, "cannot find interface declaration for %qs",
		   IDENTIFIER_POINTER (class_name));
	  add_class (implementation_template = objc_implementation_context,
		     class_name);
        }

      /* If a super class has been specified in the implementation,
	 insure it conforms to the one specified in the interface.  */

      if (super_name
	  && (super_name != CLASS_SUPER_NAME (implementation_template)))
        {
	  tree previous_name = CLASS_SUPER_NAME (implementation_template);
          const char *const name =
	    previous_name ? IDENTIFIER_POINTER (previous_name) : "";
          /* APPLE LOCAL begin radar 5835805 */
	  error ("conflicting super class name %qs in interface %qs and its implementation",
		 IDENTIFIER_POINTER (super_name), IDENTIFIER_POINTER (class_name));
          if (previous_name)
            error ("previous declaration of %qs", name);
          /* APPLE LOCAL end radar 5835805 */
        }

      else if (! super_name)
	{
	  CLASS_SUPER_NAME (objc_implementation_context)
	    = CLASS_SUPER_NAME (implementation_template);
	}
    }

  else if (code == CLASS_INTERFACE_TYPE)
    {
      if (lookup_interface (class_name))
        /* APPLE LOCAL begin radar 5835805 */
	error ("duplicate interface declaration for class %qs",
               IDENTIFIER_POINTER (class_name));
        /* APPLE LOCAL end radar 5835805 */
      else
        add_class (class, class_name);

      if (protocol_list)
	CLASS_PROTOCOL_LIST (class)
	    /* APPLE LOCAL radar 4398221 */
	  = lookup_and_install_protocols (protocol_list, true);
    }

  else if (code == CATEGORY_INTERFACE_TYPE)
    {
      tree class_category_is_assoc_with;

      /* For a category, class_name is really the name of the class that
	 the following set of methods will be associated with. We must
	 find the interface so that can derive the objects template.  */

      if (!(class_category_is_assoc_with = lookup_interface (class_name)))
	{
	  error ("cannot find interface declaration for %qs",
		 IDENTIFIER_POINTER (class_name));
	  exit (FATAL_EXIT_CODE);
	}
      else
        add_category (class_category_is_assoc_with, class);

      if (protocol_list)
	CLASS_PROTOCOL_LIST (class)
	    /* APPLE LOCAL radar 4398221 */	
	  = lookup_and_install_protocols (protocol_list, true);
    }

  else if (code == CATEGORY_IMPLEMENTATION_TYPE)
    {
      /* Reset for multiple classes per file.  */
      method_slot = 0;

      objc_implementation_context = class;

      /* For a category, class_name is really the name of the class that
	 the following set of methods will be associated with.  We must
	 find the interface so that can derive the objects template.  */

      if (!(implementation_template = lookup_interface (class_name)))
        {
	  error ("cannot find interface declaration for %qs",
		 IDENTIFIER_POINTER (class_name));
	  exit (FATAL_EXIT_CODE);
        }
    }
  return class;
}

static tree
continue_class (tree class)
{
  if (TREE_CODE (class) == CLASS_IMPLEMENTATION_TYPE
      || TREE_CODE (class) == CATEGORY_IMPLEMENTATION_TYPE)
    {
      struct imp_entry *imp_entry;

      /* Check consistency of the instance variables.  */

      if (CLASS_RAW_IVARS (class))
	check_ivars (implementation_template, class);

      /* code generation */

#ifdef OBJCPLUS
      push_lang_context (lang_name_c);
#endif

      build_private_template (implementation_template);
      uprivate_record = CLASS_STATIC_TEMPLATE (implementation_template);
      objc_instance_type = build_pointer_type (uprivate_record);

      /* APPLE LOCAL radar 4551602 */
      imp_entry = (struct imp_entry *) ggc_alloc_cleared (sizeof (struct imp_entry));

      imp_entry->next = imp_list;
      imp_entry->imp_context = class;
      imp_entry->imp_template = implementation_template;

      /* APPLE LOCAL begin ObjC new abi */
      if (flag_objc_abi == 2)
	{
	  synth_v2_forward_declarations ();
      	  imp_entry->class_v2_decl = UOBJC_V2_CLASS_decl;
      	  imp_entry->meta_v2_decl = UOBJC_V2_METACLASS_decl;
	}
      else
	{
          synth_forward_declarations ();
          imp_entry->class_decl = UOBJC_CLASS_decl;
          imp_entry->meta_decl = UOBJC_METACLASS_decl;
	}
      /* APPLE LOCAL end ObjC new abi */
      imp_entry->has_cxx_cdtors = 0;

      /* Append to front and increment count.  */
      imp_list = imp_entry;
      if (TREE_CODE (class) == CLASS_IMPLEMENTATION_TYPE)
	imp_count++;
      else
	cat_count++;

#ifdef OBJCPLUS
      pop_lang_context ();
#endif /* OBJCPLUS */

      return get_class_ivars (implementation_template, true);
    }

  else if (TREE_CODE (class) == CLASS_INTERFACE_TYPE)
    {
#ifdef OBJCPLUS
      push_lang_context (lang_name_c);
#endif /* OBJCPLUS */

      /* APPLE LOCAL radar 4291785 */
      objc_collecting_ivars = 1;
      build_private_template (class);
      /* APPLE LOCAL radar 4291785 */
      objc_collecting_ivars = 0;

#ifdef OBJCPLUS
      pop_lang_context ();
#endif /* OBJCPLUS */

      return NULL_TREE;
    }
  /* APPLE LOCAL radar 4695109 */
  /* PROTOCOL_IMPLEMENTATION_TYPE removed */

  else
    return error_mark_node;
}

/* APPLE LOCAL begin objc new property */
/* This routine issues diagnostics on two mismatched properties passed as argument. 
   - CL_PROP class property
   - PROPERTY is declared in 'super' class (SUPER is true) or protocol (SUPER is false).
   - MESS is the string attached to the diagnostic.
   - NAME property name.
*/
static void
/* APPLE LOCAL radar 5218071 */
diagnose_property_mismatch (tree cl_prop, bool super, tree property, const char *mess, tree name)
{
  /* Check that attributes and types match for protocol and its conforming class. */
  /* APPLE LOCAL begin radar 4815061 */
  if (PROPERTY_READONLY (cl_prop) == boolean_true_node 
      && PROPERTY_READWRITE (property) == boolean_true_node)
    warning (0, "attribute 'readonly' of property %qs restricts attribute 'readwrite' of %qs property in %s",
	   IDENTIFIER_POINTER (PROPERTY_NAME (cl_prop)), 
	   IDENTIFIER_POINTER (name), mess);
  /* APPLE LOCAL end radar 4815061 */
  if (PROPERTY_COPY (cl_prop) != PROPERTY_COPY (property))
    warning (0, "property %qs 'copy' attribute does not match %s %qs property",
	   IDENTIFIER_POINTER (PROPERTY_NAME (cl_prop)), mess,
	   IDENTIFIER_POINTER (name));
  else if (PROPERTY_RETAIN (cl_prop) != PROPERTY_RETAIN (property))
    warning (0, "property %qs 'retain' attribute does not match %s %qs property",
	   IDENTIFIER_POINTER (PROPERTY_NAME (cl_prop)), mess,
	   IDENTIFIER_POINTER (name));
  if (PROPERTY_SETTER_NAME (cl_prop) != PROPERTY_SETTER_NAME (property))
    warning (0, "property %qs 'setter' attribute does not match %s %qs property",
	   IDENTIFIER_POINTER (PROPERTY_NAME (cl_prop)), mess,
	   IDENTIFIER_POINTER (name));
  if (PROPERTY_GETTER_NAME (cl_prop) != PROPERTY_GETTER_NAME (property))
    warning (0, "property %qs 'getter' attribute does not match %s %qs property",
	   IDENTIFIER_POINTER (PROPERTY_NAME (cl_prop)), mess,
	   IDENTIFIER_POINTER (name));
  /* APPLE LOCAL begin radar 5218071 */
  if (comptypes (TREE_TYPE (cl_prop), TREE_TYPE (property)) != 1) {
    if (super && 
        PROPERTY_READONLY (cl_prop) == boolean_true_node &&
        PROPERTY_READONLY (property) == boolean_true_node &&
        objc_compare_types (TREE_TYPE (cl_prop), TREE_TYPE (property), 
			    /* APPLE LOCAL radar 6231433 */
                            -6, NULL_TREE, NULL))
      ;
    else
      warning (0, "property %qs type does not match %s %qs property type",
               IDENTIFIER_POINTER (PROPERTY_NAME (cl_prop)), mess,
               IDENTIFIER_POINTER (name));
  }
  /* APPLE LOCAL end radar 5218071 */
  if (flag_objc_gc &&
      objc_is_gcable_type (TREE_TYPE (cl_prop)) != objc_is_gcable_type (TREE_TYPE (property)))
    warning (0, "property %qs storage type does not match %s %qs property storage type",
	   IDENTIFIER_POINTER (PROPERTY_NAME (cl_prop)), mess,
	   IDENTIFIER_POINTER (name));
  /* APPLE LOCAL begin radar 4947014 - objc atomic property */
  if (IS_ATOMIC (cl_prop) != IS_ATOMIC (property))
    warning (0, "property %qs and its %s %qs don't have matching 'atomic' attribute",
	     IDENTIFIER_POINTER (PROPERTY_NAME (cl_prop)), mess,
	     IDENTIFIER_POINTER (name));
  /* APPLE LOCAL end radar 4947014 - objc atomic property */
}

/* This routine goes through list of 'super' class's property list and diagnoses attribute
   mismatch between those and the same properties declared in the class. */

static void
objc_compare_properties_with_super (tree class)
{
  tree super;
  tree property, cl_prop;

  if (CLASS_SUPER_NAME (class) == NULL_TREE)
    return;
  super = lookup_interface (CLASS_SUPER_NAME (class));
  if (super == NULL_TREE)
    return;
  for (property = CLASS_PROPERTY_DECL (super); property;
       property = TREE_CHAIN (property))
    if ((cl_prop = lookup_property_in_list (class, DECL_NAME (property))))
      /* APPLE LOCAL radar 5218071 */
      diagnose_property_mismatch (cl_prop, true, property, "super class", CLASS_NAME (super));
}
/* APPLE LOCAL end objc new property */

/* APPLE LOCAL begin radar 4564386 */
/* This routine merges all properties declared in protocol but not 
   in the class to the class. */

static void
objc_merge_proto_properties_in_class (tree class, tree rproto_list)
{
  tree rproto;
  for (rproto = rproto_list; rproto; rproto = TREE_CHAIN (rproto))
    {
      tree p = TREE_VALUE (rproto);
      if (TREE_CODE (p) == PROTOCOL_INTERFACE_TYPE)
	{
	  /* APPLE LOCAL objc new property */
	  tree property, cl_prop;
	  for (property = CLASS_PROPERTY_DECL (p); property; 
	       property = TREE_CHAIN (property))
	    /* APPLE LOCAL objc new property */
	    if (!(cl_prop = lookup_property_in_list (class, DECL_NAME (property))))
	      {
		/* Add the property found in protocol but not in the class to
		   the class's list of properties. */
		tree x = copy_node (property);
	  	TREE_CHAIN (x) = IMPL_PROPERTY_DECL (class);
		IMPL_PROPERTY_DECL (class) = x;
	      }
	    /* APPLE LOCAL begin objc new property */
	    else
               /* APPLE LOCAL radar 5218071 */
	      diagnose_property_mismatch (cl_prop, false, property, "protocol", PROTOCOL_NAME (p));
	    /* APPLE LOCAL end objc new property */
	  /* Search in nested protocols also. */
	  objc_merge_proto_properties_in_class (class, PROTOCOL_LIST (p));
	}      
    }
}
/* APPLE LOCAL end radar 4564386 */

/* APPLE LOCAL begin C* language */
/* This routine builds a property ivar name. */

static char *
objc_build_property_ivar_name (tree property_decl)
{
  static char string[BUFSIZE];
  sprintf (string, "_%s", IDENTIFIER_POINTER (PROPERTY_NAME (property_decl)));
  return string;
}

/* This routine builds name of the setter synthesized function. */

static char *
objc_build_property_setter_name (tree ident, bool delimit_colon)
{
  extern int toupper(int);
  static char string[BUFSIZE];
  if (delimit_colon)
    sprintf (string, "set%s:", IDENTIFIER_POINTER (ident));
  else
    sprintf (string, "set%s", IDENTIFIER_POINTER (ident));
  string[3] = toupper (string[3]);
  return string;
}

/* This routine does all the work for generting data and code per each 
   property declared in current implementation. */

static void
objc_gen_one_property_data (tree class, tree property, tree class_methods)
{
  /* If getter, check that it is already declared in user code. */
  if (PROPERTY_GETTER_NAME (property))
    {
      if (!lookup_method (CLASS_NST_METHODS (class_methods), 
			  PROPERTY_GETTER_NAME (property))
	  && !lookup_method_in_protocol_list (CLASS_PROTOCOL_LIST (class_methods),
					      PROPERTY_GETTER_NAME (property), 0))
	error ("property getter %qs not declared in class %qs",  
		IDENTIFIER_POINTER (PROPERTY_GETTER_NAME (property)), 
		IDENTIFIER_POINTER (CLASS_NAME (class_methods)));
    }
  /* If setter, check that it is already declared in user code. */
  if (PROPERTY_SETTER_NAME (property))
    {
      if (!lookup_method (CLASS_NST_METHODS (class_methods), 
			  PROPERTY_SETTER_NAME (property))
	  && !lookup_method_in_protocol_list (CLASS_PROTOCOL_LIST (class_methods),
					      PROPERTY_SETTER_NAME (property), 0))
	error ("property setter %qs not declared in class %qs",  
		IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (property)), 
		IDENTIFIER_POINTER (CLASS_NAME (class_methods)));
    }
  /* If ivar attribute specified, check that it is already declared. */
  if (PROPERTY_IVAR_NAME (property) 
      && PROPERTY_IVAR_NAME (property) != boolean_true_node)
    {
      if (!nested_ivar_lookup (class, PROPERTY_IVAR_NAME (property)))
	error ("ivar %qs in property declaration must be an existing ivar", 
               IDENTIFIER_POINTER (PROPERTY_IVAR_NAME (property)));
    }
}

/* This routine processes an existing getter or setter attribute.
   It aliases internal property getter or setter to the user implemented 
   getter or setter.  */

static void 
objc_process_getter_setter (tree class, tree property, bool getter)
{
  tree prop_mth_decl;
  tree prop_getter_mth_decl;
  tree name_ident;

  if (getter)
    /* getter name is same as property name. */
    name_ident = PROPERTY_NAME (property);
  else
    /* Must synthesize setter name from property name. */
    name_ident = get_identifier (objc_build_property_setter_name (
				   PROPERTY_NAME (property), true));

  /* Find declaration of instance method for the property in its class. */
  prop_mth_decl = lookup_method (CLASS_NST_METHODS (class), name_ident);

  if (!prop_mth_decl)
    return;

  prop_getter_mth_decl = lookup_method (CLASS_NST_METHODS (objc_implementation_context),
					getter ? PROPERTY_GETTER_NAME (property) 
					       : PROPERTY_SETTER_NAME (property));

  if (!prop_getter_mth_decl)
    return;

  /* APPLE LOCAL radar 5370783 */
  if (!match_proto_with_proto (prop_getter_mth_decl, prop_mth_decl, 2))
    {
      error ("User %s %qs does not match property %qs type", 
		getter ? "getter" : "setter",
		IDENTIFIER_POINTER (DECL_NAME (prop_getter_mth_decl)), 
		IDENTIFIER_POINTER (PROPERTY_NAME (property)));
      return;
    }
  /* We alias internal property getter to the user implemented getter by copying relevant
     entries from user's implementation to the internal one. */
  prop_mth_decl = copy_node (prop_mth_decl);
  METHOD_ENCODING (prop_mth_decl) = METHOD_ENCODING (prop_getter_mth_decl);
  METHOD_DEFINITION (prop_mth_decl) = METHOD_DEFINITION (prop_getter_mth_decl);
  objc_add_method (objc_implementation_context, prop_mth_decl, 0, 0);
}

/* This routine builds the return value of a synthesized getter in a variety
   of situations.
*/
static tree
objc_build_getter_retval (tree ivar_ident)
{
  tree ret_val;

  ret_val = objc_lookup_ivar (NULL_TREE, ivar_ident);

  return ret_val;
}

/* This routine synthesizes a 'getter' routine. */

static void
objc_synthesize_getter (tree class, tree class_method, tree property)
{
  tree fn, decl;
  tree body;
  tree ret_val;
  tree ivar_ident;

  /* No getter for a class with no 'ivar' attribute. */
  if (!PROPERTY_IVAR_NAME (property))
    {
      /* Issue a warning if there is no user-implemented getter. */
      if (!lookup_method (CLASS_NST_METHODS (
			  objc_implementation_context),PROPERTY_NAME (property)))
	{
          bool warn = true;
	  if (TREE_CODE (objc_implementation_context) == CATEGORY_IMPLEMENTATION_TYPE)
	    {
	      tree category = lookup_category (class, 
				CLASS_SUPER_NAME (objc_implementation_context));
	      if (category 
		  && lookup_method (CLASS_NST_METHODS (
				    category),PROPERTY_NAME (property)))
	        warn = false;
	    }
	  if (warn)
	    warning (0, "no synthesized or user getter is available for property %qs",
		     IDENTIFIER_POINTER (DECL_NAME (property))); 
	}
      return;
    }

  /* If user has implemented a getter with same name then do nothing. */
  if (lookup_method (CLASS_NST_METHODS (
		     objc_implementation_context),PROPERTY_NAME (property)))
    return;
  /* Find declaration of the property in the interface. There must be one. */
  decl = lookup_method (CLASS_NST_METHODS (class_method),
                        PROPERTY_NAME (property));
  /* If one not declared in the inerface, this condition has already been reported
     as user error (because property was not declared in the interface. */
  if (!decl)
    return;

  objc_inherit_code = INSTANCE_METHOD_DECL;
  objc_start_method_definition (copy_node (decl), NULL_TREE);
  body = c_begin_compound_stmt (true);
  /* return self->_property_name; */
  /* If user specified an ivar, us it in generation of the getter. */
  ivar_ident = (PROPERTY_IVAR_NAME (property) != boolean_true_node)
	  	? PROPERTY_IVAR_NAME (property) 
	  	: get_identifier (objc_build_property_ivar_name (property));
  ret_val = objc_build_getter_retval (ivar_ident);

  if (ret_val)
    {
      /* APPLE LOCAL radar 5276085 */
      ret_val = objc_build_weak_reference_tree (ret_val);
#ifdef OBJCPLUS
      finish_return_stmt (ret_val);
#else
      (void)c_finish_return (ret_val);
#endif
    }
  add_stmt (c_end_compound_stmt (body, true));
  fn = current_function_decl;
#ifdef OBJCPLUS
  finish_function ();
#endif
  objc_finish_method_definition (fn);

}

/* This routine builds a single statement in a synthesized 'setter' function
   in a variety of situations. */

static tree
objc_build_setter_stmt (tree ivar_ident)
{
  tree lhs;

  lhs = objc_lookup_ivar (NULL_TREE, ivar_ident);
  /* If ivar attribute is not a user declared attribute, this condition has
     already been repored as error. */
  gcc_assert (lhs);

  if (lhs)
    {
      int save_flag_objc_gc = flag_objc_gc;
      tree store;
      tree rhs = lookup_name (get_identifier ("_value"));
      /* Recover when method does not have '_value' argument. This is because user
	 provided its own accessor and for which an error is already issued. */
      if (!rhs)
	rhs = lhs;
      /* For 'weak' property, must generate objc_assign_weak regardless of -fobjc-gc */
      flag_objc_gc = 1;
      store =  build_modify_expr (lhs, NOP_EXPR, rhs);
      flag_objc_gc = save_flag_objc_gc;
      return store;
    }
  return NULL_TREE;
}

/* This routine synthesizes a 'setter' routine. */

static void
objc_synthesize_setter (tree class, tree class_method, tree property)
{
  tree fn, decl, ivar_ident, stmt;
  tree body;
  char *setter_name;
  tree setter_ident;

  setter_name = objc_build_property_setter_name (
		  PROPERTY_NAME (property), true);
  setter_ident = get_identifier (setter_name);

  if (!PROPERTY_IVAR_NAME (property))
    {
      /* Issue a warning if there is no user-implemented getter. */
      if (!lookup_method (CLASS_NST_METHODS (
			  objc_implementation_context),setter_ident))
	{
          bool warn = true;
	  if (TREE_CODE (objc_implementation_context) == CATEGORY_IMPLEMENTATION_TYPE)
	    {
	      tree category = lookup_category (class, 
				CLASS_SUPER_NAME (objc_implementation_context));
	      if (category
		  && lookup_method (CLASS_NST_METHODS (
				    category),setter_ident))
	        warn = false;
	    }
	  if (warn)
	    warning (0, "no synthesized or user setter is available for property %qs",
		     IDENTIFIER_POINTER (DECL_NAME (property))); 
	}
      return;
    }

  /* If user has implemented a setter with same name then do nothing. */
  if (lookup_method (CLASS_NST_METHODS (
		    objc_implementation_context),setter_ident))
    return;

  /* Find declaration of the property in the interface. There must be one. */
  decl = lookup_method (CLASS_NST_METHODS (class_method), setter_ident);
  /* If one not declared in the inerface, this condition has already been reported
     as user error (because property was not declared in the interface. */
  if (!decl)
    return;

  objc_inherit_code = INSTANCE_METHOD_DECL;
  objc_start_method_definition (copy_node (decl), NULL_TREE);
  body = c_begin_compound_stmt (true);
  /* _property_name = _value; */
  /* If user specified an ivar, us it in generation of the setter. */
  ivar_ident = (PROPERTY_IVAR_NAME (property) != boolean_true_node)
	  	? PROPERTY_IVAR_NAME (property) 
	  	: get_identifier (objc_build_property_ivar_name (property));
  stmt = objc_build_setter_stmt (ivar_ident);
  if (stmt)
    add_stmt (stmt);
  add_stmt (c_end_compound_stmt (body, true));
  fn = current_function_decl;
#ifdef OBJCPLUS
  finish_function ();
#endif
  objc_finish_method_definition (fn);
}

/* This routine synthesizes a 'getter' routine for new property. */
static void
objc_synthesize_new_getter (tree class, tree class_method, tree property)
{
  /* APPLE LOCAL radar 5839812 - location for synthesized methods  */
  location_t save_input_location;
  tree decl,body,ret_val,fn;
  /* APPLE LOCAL begin radar 5207415 */
  tree property_getter = PROPERTY_GETTER_NAME (property) 
			   ? PROPERTY_GETTER_NAME (property) 
			   : PROPERTY_NAME (property);
  /* APPLE LOCAL end radar 5207415 */
  /* If user has implemented a getter with same name then do nothing. */
  if (lookup_method (CLASS_NST_METHODS (
		     /* APPLE LOCAL radar 5207415 */
                     objc_implementation_context),property_getter))
    return;

  /* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
  save_input_location = input_location;
  input_location = DECL_SOURCE_LOCATION (property);
  /* APPLE LOCAL end radar 5839812 - location for synthesized methods  */

  /* APPLE LOCAL begin radar 5435299 */
  if (!flag_new_property_ivar_synthesis)
    objc_lookup_property_ivar (class, property);
  /* APPLE LOCAL end radar 5435299 */

  /* Find declaration of the property in the interface. There must be one. */
  /* APPLE LOCAL radar 5040740 - radar 5207415 */
  decl = lookup_nested_method (class_method, property_getter);
  /* If one not declared in the inerface, this condition has already been reported
     as user error (because property was not declared in the interface. */
  if (!decl)
    return;  

  /* APPLE LOCAL begin radar 4947014 - objc atomic property */
  if (!umsg_GetAtomicProperty || !umsg_CopyAtomicStruct)
    declare_atomic_property_api ();
  objc_inherit_code = INSTANCE_METHOD_DECL;
  /* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
  decl = copy_node (decl);
  DECL_SOURCE_LOCATION (decl) = DECL_SOURCE_LOCATION (property);
  objc_start_method_definition (decl, NULL_TREE);
  /* APPLE LOCAL end radar 5839812 - location for synthesized methods  */
  body = c_begin_compound_stmt (true);
  if (!flag_objc_gc_only && IS_ATOMIC (property)
      && (PROPERTY_COPY (property) == boolean_true_node
          || PROPERTY_RETAIN (property) == boolean_true_node))
    {
      /* build call to:
	 id objc_getProperty (self, _cmd, offsetof (class, ivar), isAtomic) */
      tree cmd;
      /* APPLE LOCAL begin radar 5610134 */
      tree func_params, func;
      tree field_decl = nested_ivar_lookup (class, PROPERTY_IVAR_NAME (property));
      tree offset = flag_objc_abi <= 1 ? byte_position (field_decl) 
				       : ivar_offset_ref (CLASS_NAME (class), field_decl);
      /* APPLE LOCAL end radar 5610134 */
      gcc_assert (self_decl);
      cmd = TREE_CHAIN (self_decl);
      gcc_assert (cmd);
      func_params = tree_cons (NULL_TREE, self_decl,
                      tree_cons (NULL_TREE, cmd,
                        tree_cons (NULL_TREE, offset, 
			  tree_cons (NULL_TREE, boolean_true_node, NULL_TREE))));
      func = umsg_GetAtomicProperty;
      ret_val = build_function_call (func, func_params);
    }
    else
      {
 	bool isStrong = false;
        tree ret_type = TREE_TYPE (TREE_TYPE (current_function_decl));
        /* return self->ivar_name; */
        /* APPLE LOCAL begin radar 5376125 */
        int save_warn_direct_ivar_access = warn_direct_ivar_access;
        warn_direct_ivar_access = 0;
        ret_val = build_ivar_reference (PROPERTY_IVAR_NAME (property));
        warn_direct_ivar_access = save_warn_direct_ivar_access;
        /* APPLE LOCAL end radar 5376125 */

	/* Handle struct-valued functions */
        /* APPLE LOCAL begin 6671703 ARM 64-bit atomic properties */
        if (
#ifdef TARGET_ARM
          (IS_ATOMIC (property)
           && TREE_INT_CST_LOW (TYPE_SIZE_UNIT (ret_type)) > 4) ||
#endif
          ((TREE_CODE (ret_type) == RECORD_TYPE
            || TREE_CODE (ret_type) == UNION_TYPE)
            /* APPLE LOCAL radar 5080710 */
            && (TREE_ADDRESSABLE (ret_type) || targetm.calls.return_in_memory  (ret_type, 0))
	    && (IS_ATOMIC (property) ||
		(isStrong = ((flag_objc_gc || flag_objc_gc_only)
			     && aggregate_contains_objc_pointer (ret_type))))))
        /* APPLE LOCAL end 6671703 ARM 64-bit atomic properties */
        {
	    /* struct something tmp; 
	       objc_copyStruct (&tmp, &structIvar, sizeof (struct something), isAtomic, false);
	       return tmp;
	    */
	    tree decl = create_tmp_var_raw (ret_type, NULL);
	    tree isAtomic = (IS_ATOMIC (property) ? boolean_true_node : boolean_false_node);
	    tree hasStrong = (isStrong ? boolean_true_node : boolean_false_node);
	    tree size_ret_val = build_int_cst_wide (NULL_TREE,
                                                    TREE_INT_CST_LOW (TYPE_SIZE_UNIT (ret_type)),
					            TREE_INT_CST_HIGH (TYPE_SIZE_UNIT (ret_type)));
	    tree func_params, stmt, tmp;
	    lang_hooks.decls.pushdecl (decl);
	    tmp = build_fold_addr_expr (decl);
	    ret_val = build_fold_addr_expr (ret_val);
	    func_params = tree_cons (NULL_TREE, tmp,
			    tree_cons (NULL_TREE, ret_val,
			      tree_cons (NULL_TREE, size_ret_val,
				tree_cons (NULL_TREE, isAtomic,
				  tree_cons (NULL_TREE, hasStrong, NULL_TREE)))));
	    stmt = build_function_call (umsg_CopyAtomicStruct, func_params);
	    add_stmt (stmt); 
	    ret_val = decl;
	  }
      }
  /* APPLE LOCAL end radar 4947014 - objc atomic property */

  if (ret_val)
    {
      /* APPLE LOCAL radar 5276085 */
      ret_val = objc_build_weak_reference_tree (ret_val);
#ifdef OBJCPLUS
      finish_return_stmt (ret_val);
#else
      (void)c_finish_return (ret_val);
#endif
    }
  add_stmt (c_end_compound_stmt (body, true));
  fn = current_function_decl;
#ifdef OBJCPLUS
  finish_function ();
#endif
  objc_finish_method_definition (fn);
  /* APPLE LOCAL radar 5839812 - location for synthesized methods  */
  input_location = save_input_location;
}

/* This routine synthesizes a 'setter' routine for new property. */
static void
objc_synthesize_new_setter (tree class, tree class_method, tree property)
{
  tree fn, decl, stmt;
  tree body, lhs;
  /* APPLE LOCAL begin radar 5207415 */
  tree setter_ident;
  /* APPLE LOCAL radar 5839812 - location for synthesized methods  */
  location_t save_input_location;

  if (PROPERTY_SETTER_NAME (property))
    setter_ident = PROPERTY_SETTER_NAME (property);
  else
    setter_ident = get_identifier (objc_build_property_setter_name (PROPERTY_NAME (property), true));
  /* APPLE LOCAL end radar 5207415 */ 

  /* If user has implemented a setter with same name then do nothing. */
  if (lookup_method (CLASS_NST_METHODS (
                     objc_implementation_context),setter_ident))
    return;

  /* APPLE LOCAL begin radar 5435299 */
  if (!flag_new_property_ivar_synthesis)
    objc_lookup_property_ivar (class, property);
  /* APPLE LOCAL end radar 5435299 */

  /* Find declaration of the property in the interface. There must be one. */
  /* APPLE LOCAL radar 5040740 */
  decl = lookup_nested_method (class_method, setter_ident);
  /* If not declared in the inerface, this condition has already been reported
     as user error (because property was not declared in the interface. */
  if (!decl)
    return;

  /* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
  save_input_location = input_location;
  input_location = DECL_SOURCE_LOCATION (property);
  /* APPLE LOCAL end radar 5839812 - location for synthesized methods  */

  /* APPLE LOCAL begin radar 4947014 - objc atomic property */
  if (!umsg_SetAtomicProperty || !umsg_CopyAtomicStruct)
    declare_atomic_property_api ();
  objc_inherit_code = INSTANCE_METHOD_DECL;
  /* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
  decl = copy_node (decl);
  DECL_SOURCE_LOCATION (decl) = DECL_SOURCE_LOCATION (property);
  objc_start_method_definition (decl, NULL_TREE);
  /* APPLE LOCAL end radar 5839812 - location for synthesized methods  */
  body = c_begin_compound_stmt (true);
  /* property_name = _value; */
  stmt = NULL_TREE;
  if (PROPERTY_COPY (property) == boolean_true_node
      || (!flag_objc_gc_only && PROPERTY_RETAIN (property) == boolean_true_node))
    {
      /* build call to:
	 objc_setProperty (self, _cmd, offsetof (class, ivar), arg, [true|false], [true|false]) */
      tree cmd, arg;
      /* APPLE LOCAL begin radar 5610134 */
      tree func_params, func;
      tree field_decl = nested_ivar_lookup (class, PROPERTY_IVAR_NAME (property));
      tree offset = flag_objc_abi <= 1 ? byte_position (field_decl)
				       : ivar_offset_ref (CLASS_NAME (class), field_decl);
      /* APPLE LOCAL end radar 5610134 */
      tree shouldCopy = (PROPERTY_COPY (property) == boolean_true_node) 
		          ? boolean_true_node : boolean_false_node; 
      tree isAtomic = IS_ATOMIC (property) ? boolean_true_node : boolean_false_node;
      gcc_assert (self_decl);
      cmd = TREE_CHAIN (self_decl);
      gcc_assert (cmd);
      arg = TREE_CHAIN (cmd);
      gcc_assert (arg);
      /* APPLE LOCAL radar 5398274 */
      TREE_USED (arg) = 1;
      func_params = tree_cons (NULL_TREE, self_decl,
                      tree_cons (NULL_TREE, cmd,
                        tree_cons (NULL_TREE, offset,
                          tree_cons (NULL_TREE, arg,
                            tree_cons (NULL_TREE, isAtomic,
                              tree_cons (NULL_TREE, shouldCopy, NULL_TREE))))));
      func = umsg_SetAtomicProperty;
      stmt =  build_function_call (func, func_params);
    }
  else
    {
      tree ivar_type;
      /* APPLE LOCAL begin radar 5852190 */
      tree rhs = NULL_TREE;
      int save_warn_direct_ivar_access = warn_direct_ivar_access;
      
      if (current_function_decl && DECL_ARGUMENTS (current_function_decl))
        rhs = TREE_CHAIN (TREE_CHAIN (DECL_ARGUMENTS (current_function_decl)));
      /* APPLE LOCAL begin radar 5376125 */
      warn_direct_ivar_access = 0;
      lhs = build_ivar_reference (PROPERTY_IVAR_NAME (property));
      warn_direct_ivar_access = save_warn_direct_ivar_access;
      /* APPLE LOCAL end radar 5376125 */
      /* Recover when method does not have '_value' argument. This is because user
         provided its own accessor and for which an error is already issued. */
      if (!rhs) {
	fatal_error ("Failed to synthesize the setter - possibly due to earlier error");
        rhs = lhs;
      }
      /* APPLE LOCAL end radar 5852190 */
      /* APPLE LOCAL begin radar 5232840 */
      else
        TREE_USED (rhs) = 1;
      /* APPLE LOCAL end radar 5232840 */
      ivar_type = TREE_TYPE (lhs);
      /* APPLE LOCAL begin 6671703 ARM 64-bit atomic properties */
      if (
#ifdef TARGET_ARM
        (IS_ATOMIC (property)
         && TREE_INT_CST_LOW (TYPE_SIZE_UNIT (ivar_type)) > 4) ||
#endif
       ((TREE_CODE (ivar_type) == RECORD_TYPE || TREE_CODE (ivar_type) == UNION_TYPE)
          /* APPLE LOCAL begin radar 5080710 */
          && IS_ATOMIC (property)
          && (TREE_ADDRESSABLE (ivar_type) || targetm.calls.return_in_memory  (ivar_type, 0))))
          /* APPLE LOCAL end radar 5080710 */
      /* APPLE LOCAL end 6671703 ARM 64-bit atomic properties */
        {
	  /* objc_copyStruct (&structIvar, &value, sizeof (struct something), true, false); */
	  tree func_params;
	  tree isAtomic = boolean_true_node;
	  tree hasStrong = boolean_false_node;
	  tree addr_structIvar = build_fold_addr_expr (lhs);
	  tree size_struct = build_int_cst_wide (NULL_TREE,
                                                 TREE_INT_CST_LOW (TYPE_SIZE_UNIT (ivar_type)),
					         TREE_INT_CST_HIGH (TYPE_SIZE_UNIT (ivar_type)));
	  rhs = build_fold_addr_expr (rhs);
	  func_params = tree_cons (NULL_TREE, addr_structIvar,
			  tree_cons (NULL_TREE, rhs,
			    tree_cons (NULL_TREE, size_struct,
			      tree_cons (NULL_TREE, isAtomic,
				tree_cons (NULL_TREE, hasStrong, NULL_TREE)))));
	  stmt = build_function_call (umsg_CopyAtomicStruct, func_params);  
	}
      else
    	{
          /* Common case */
	  /* APPLE LOCAL 5675908 */
          stmt =  build_modify_expr (lhs, NOP_EXPR, rhs);
       }
    }
  /* APPLE LOCAL end radar 4947014 - objc atomic property */

  if (stmt)
    add_stmt (stmt);
  add_stmt (c_end_compound_stmt (body, true));
  fn = current_function_decl;
#ifdef OBJCPLUS
  finish_function ();
#endif
  objc_finish_method_definition (fn);
  /* APPLE LOCAL radar 5839812 - location for synthesized methods  */
  input_location = save_input_location;
}

/* APPLE LOCAL begin radar 4966565 */
/* This routine retrievs the base class implementation of the category implementation
   or NULL_TREE if one not found. CATEGORY_IMPL is to be CATEGORY_IMPLEMENTATION_TYPE
   and BASE_CLASS is CLASS_INTERFACE_TYPE of base class of this category. */

static tree
get_category_base_class_impl (tree category_impl, tree base_class)
{
  if (TREE_CODE  (category_impl) == CATEGORY_IMPLEMENTATION_TYPE
      && TREE_CODE (base_class) == CLASS_INTERFACE_TYPE)
    {
      struct imp_entry *impent;
      for (impent = imp_list; impent; impent = impent->next)
        {
          if (TREE_CODE (impent->imp_context) == CLASS_IMPLEMENTATION_TYPE
              && impent->imp_template == base_class)
            return impent->imp_context;
        }
    }
  return NULL_TREE;
}

/* This routine returns true, if accessor is implemenated in base @implementation of the given
   category @implementation; false otherwise. CATEGORY_IMPL is to be CATEGORY_IMPLEMENTATION_TYPE
   and BASE_CLASS is CLASS_INTERFACE_TYPE of base class of this category. */

static bool
lookup_accessor_in_base_class_impl (tree category_impl, tree base_class, tree method_ident)
{
  tree base_impl = get_category_base_class_impl (category_impl, base_class);
  return (base_impl != NULL_TREE) 
	 && lookup_method (CLASS_NST_METHODS (base_impl), method_ident);
}
/* APPLE LOCAL end radar 4966565 */

/* Main routine to generate code/data for all the property information for 
   current implementation (class or category). CLASS is the interface where
   ivars are declared in. CLASS_METHODS is where methods are found which
   could be a class or a category depending on wheter we are implementing
   property of a class or a category.  */

static void
objc_gen_property_data (tree class, tree class_methods)
{
  tree x;
  /* Merge any property declared in interface but not in
     implementation to implementation. */
  objc_v2_merge_property ();
  
  /* Synthesize accessors for those properties specified an @synthesize */
  for (x = IMPL_PROPERTY_DECL (objc_implementation_context); x; x = TREE_CHAIN (x))
    {
      /* Skip dynamic property. */
      if (PROPERTY_DYNAMIC (x) == boolean_true_node)
	continue;
      else if (PROPERTY_IVAR_NAME (x))
	{
	  /* property 'ivar' is set only when @synthesize is specified. */
	  objc_synthesize_new_getter (class, class_methods, x);
	  if (PROPERTY_READONLY (x) == boolean_false_node)
	    objc_synthesize_new_setter (class, class_methods, x);
	}
      else 
	{
	  if (PROPERTY_GETTER_NAME (x))
	    objc_process_getter_setter (class_methods, x, true);
	  /* APPLE LOCAL begin radar 4966565 - radar 4653422 */
	  else if (!lookup_method (CLASS_NST_METHODS (objc_implementation_context), PROPERTY_NAME (x))
	           && !lookup_accessor_in_base_class_impl (objc_implementation_context, class,
						           PROPERTY_NAME (x))
		   && !OPTIONAL_PROPERTY (x))
	  /* APPLE LOCAL end radar 4966565 - radar 4653422 */
	    warning (0, "property %qs requires method '-%s' to be defined - use @synthesize, "
			"@dynamic or provide a method implementation", 
		     IDENTIFIER_POINTER (PROPERTY_NAME (x)), IDENTIFIER_POINTER (PROPERTY_NAME (x)));
	  if (PROPERTY_READONLY (x) == boolean_false_node
	      && PROPERTY_SETTER_NAME (x))
	    objc_process_getter_setter (class_methods, x, false);
	  else if (PROPERTY_READONLY (x) == boolean_false_node)
	    {
	      char *setter_string_name = objc_build_property_setter_name (PROPERTY_NAME (x), true);
	      /* APPLE LOCAL begin radar 4966565 - radar 4653422 */
	      tree ident = get_identifier (setter_string_name);
	      if (!lookup_method (CLASS_NST_METHODS (objc_implementation_context), ident)
	          && !lookup_accessor_in_base_class_impl (objc_implementation_context, class, ident)
		  && !OPTIONAL_PROPERTY (x))
		warning (0, "property %qs requires the method '%s' to be defined - use @synthesize, "
			    "@dynamic or provide a method implementation",
			 IDENTIFIER_POINTER (PROPERTY_NAME (x)), setter_string_name);
	      /* APPLE LOCAL end radar 4966565 - radar 4653422 */
	    }
	}
      }
      return;

  for (x = IMPL_PROPERTY_DECL (objc_implementation_context); x; x = TREE_CHAIN (x))
     objc_gen_one_property_data (class, x, class_methods);

  /* Synthesize all getters for properties. */
  for (x = IMPL_PROPERTY_DECL (objc_implementation_context); x; x = TREE_CHAIN (x))
    {
     /* Skip dynamic property. */
     if (PROPERTY_DYNAMIC (x) == boolean_true_node)
       continue;
	
     /* Property has a getter attribute, no need to synthesize one. */
     if (PROPERTY_GETTER_NAME (x) == NULL_TREE)
       objc_synthesize_getter (class, class_methods, x);
     else
       objc_process_getter_setter (class_methods, x, true);

     if (PROPERTY_READONLY (x) == boolean_false_node)
       {
	 /* not a readonly property. */
	 if (PROPERTY_SETTER_NAME (x) == NULL_TREE)
	   objc_synthesize_setter (class, class_methods, x);
	 else
	   objc_process_getter_setter (class_methods, x, false);
       }
    }
}

/* This routine creates an ivar and adds it to current class's ivar list. */

static void
objc_add_internal_ivar (tree class, tree property, const char *ivar_name)
{
  tree record = CLASS_STATIC_TEMPLATE (class);
  tree type = TREE_TYPE (property);
  tree field_decl, field, nf;
  /* APPLE LOCAL begin radar 6029624 */
#ifdef OBJCPLUS
  if (TREE_CODE (type) == REFERENCE_TYPE)
    type = TYPE_MAIN_VARIANT (TREE_TYPE (type));
#endif
  /* APPLE LOCAL end radar 6029624 */
  field_decl = create_field_decl (type,
                                  ivar_name ? ivar_name 
					    : objc_build_property_ivar_name (property));
  DECL_CONTEXT (field_decl) = record;
  /* APPLE LOCAL begin radar 5435299 */
  /* With -fnew-property-ivar_synthesis, synthesized 'ivar' is always private. */
  (void) add_instance_variable (class,
                                !flag_new_property_ivar_synthesis ? 1 : 2, field_decl);
  /* APPLE LOCAL end radar 5435299 */
  if (objc_is_strong_p (type) == -1)
    {
      tree attribute = tree_cons (NULL_TREE, get_identifier ("weak"), NULL_TREE);
      attribute = tree_cons (get_identifier ("objc_gc"), attribute, NULL_TREE);
      decl_attributes (&field_decl, attribute, 0);
    }
  /* APPLE LOCAL begin radar 5435299 */
  nf = copy_node (field_decl);
  if (flag_new_property_ivar_synthesis)
    {
      /* New field added to struct must not have ivar specific access info.
         This confuses c++'s side of its own access field checking. */
      TREE_PUBLIC (nf) = IVAR_PUBLIC_OR_PROTECTED (nf) = 1;
      TREE_PRIVATE (nf) = TREE_PROTECTED (nf) = 0;
    }
  /* APPLE LOCAL end radar 5435299 */
  /* Unfortunately, CLASS_IVARS is completed when interface is completed.
     Must add the new ivar by hand to its list here. */

#ifdef OBJCPLUS
  /* Must insert this field before the TYPE_DECL node in the chain of fields. */
  field = CLASS_IVARS (class);
  while (field && TREE_CHAIN (field)
	 && TREE_CODE (TREE_CHAIN (field)) == FIELD_DECL)
    field = TREE_CHAIN (field);
  if (field && TREE_CHAIN (field)
      && TREE_CODE (TREE_CHAIN (field)) != FIELD_DECL)
    {
      TREE_CHAIN (nf) = TREE_CHAIN (field);
      TREE_CHAIN (field) = nf;
    }
  else
#endif /* OBJCPLUS */
  CLASS_IVARS (class) = chainon (CLASS_IVARS (class), nf);
  gcc_assert (record);
  /* Must also add this ivar to the end of list of fields for this class. */
  field = TYPE_FIELDS (record);
  if (field && field != CLASS_IVARS (class))
    /* class has a hidden field, attach ivar list after the hiddent field. */
    TREE_CHAIN (field) = CLASS_IVARS (class);
  else
    TYPE_FIELDS (record) = CLASS_IVARS (class);
  /* Ugh, must recalculate struct layout since an ivar was added. */
  TYPE_SIZE (record) = 0;
  layout_type (record);
  /* APPLE LOCAL begin radar 5435299 */
  if (flag_new_property_ivar_synthesis)
    objc_ivar_chain = CLASS_RAW_IVARS (class);
  /* APPLE LOCAL end radar 5435299 */
}

/* This routine looks up PROPERTY's 'ivar' in the CLASS. If one not found, it inserts one
   and may issue error for 32-bit abi (fragile 'ivar') in this case. */
static void
objc_lookup_property_ivar (tree class, tree property)
{
  /* Check for an existing or synthesized 'ivar' and diagnose as needed. */
  tree ivar_name = PROPERTY_IVAR_NAME (property);
  if (!nested_ivar_lookup (class, PROPERTY_IVAR_NAME (property)))
    {
      if (flag_objc_abi <= 1)
	error ("synthesized property %qs must either be named the same as a compatible ivar or must explicitly name an ivar",
	       IDENTIFIER_POINTER (PROPERTY_NAME (property)));
        objc_add_internal_ivar (class, property, IDENTIFIER_POINTER (ivar_name));
    }
}
/* APPLE LOCAL end C* language */

/* APPLE LOCAL begin radar 4965989 */
/* This routine merges all methods declared or synthesized in anonymous class
   into PRIMARY primary class. CHAIN is the list of methods in anonymous category.
   MTYPE is '+' for class methods and '-' for instance methods.
*/          
static void   
objc_merge_methods (tree primary, tree chain ,int mtype)
{     
  while (chain)
    {   
      tree method = chain; 
      chain = TREE_CHAIN (chain);
      objc_add_method (primary, method, mtype == '+', false);
      if (mtype == '-' && METHOD_PROPERTY_CONTEXT (method))
        {
          tree x;
          for (x = CLASS_PROPERTY_DECL (primary); x; x = TREE_CHAIN (x)) 
            {    
              if (PROPERTY_NAME (x) == PROPERTY_NAME (METHOD_PROPERTY_CONTEXT (method)))
                {
                  METHOD_PROPERTY_CONTEXT (method) = x;
                  break;
                }
            }
        }
    } 
}         
/* APPLE LOCAL end radar 4965989 */

/* APPLE LOCAL begin radar 4968128 */
static tree
strip_end_colon (tree ident)
{
    size_t pos = IDENTIFIER_LENGTH (ident) - 1;
    gcc_assert (IDENTIFIER_LENGTH (ident) > 0 && IDENTIFIER_POINTER (ident)[pos] == ':');
    return get_identifier_with_length (IDENTIFIER_POINTER (ident), pos);
}
/* APPLE LOCAL end radar 4968128 */

/* APPLE LOCAL begin radar 6017984 */
/* This routine merges all methods declared in the protocol-list
   into the current class. */
static void
objc_merge_protocol_methods (tree class, tree rproto_list)
{
  tree rproto;
  for (rproto = rproto_list; rproto; rproto = TREE_CHAIN (rproto))
    {
      tree p = TREE_VALUE (rproto);
      if (TREE_CODE (p) == PROTOCOL_INTERFACE_TYPE)
        {
	  /* APPLE LOCAL begin radar 6370136 */
	  /* Call objc_merge_methods on copied list of methods, as insertion
	     of methods in class's method list has side-effect of modifying
	     protocol's method list which must not change. */
	  objc_merge_methods (class, copy_list (PROTOCOL_CLS_METHODS (p)), '+');
	  objc_merge_methods (class, copy_list (PROTOCOL_NST_METHODS (p)), '-');
	  /* APPLE LOCAL end radar 6370136 */
          /* Search in nested protocols also. */
          objc_merge_proto_properties_in_class (class, PROTOCOL_LIST (p));
        }
    }
}
/* APPLE LOCAL end radar 6017984 */

/* This is called once we see the "@end" in an interface/implementation.  */

static void
finish_class (tree class)
{
  if (TREE_CODE (class) == CLASS_IMPLEMENTATION_TYPE)
    {
      /* All code generation is done in finish_objc.  */

      /* APPLE LOCAL begin C* property (Radar 4436866) */
      /* Generate what needed for property; setters, getters, etc. */
      objc_gen_property_data (implementation_template, implementation_template);
      /* APPLE LOCAL end C* property (Radar 4436866) */
      if (implementation_template != objc_implementation_context)
	{
	  /* Ensure that all method listed in the interface contain bodies.  */
	  check_methods (CLASS_CLS_METHODS (implementation_template),
			 /* APPLE LOCAL radar 4568791, 4359757 */
			 '+', NULL_TREE);
	  check_methods (CLASS_NST_METHODS (implementation_template),
			 /* APPLE LOCAL radar 4568791, 4359757 */
			 '-', NULL_TREE);

	  if (CLASS_PROTOCOL_LIST (implementation_template))
	    check_protocols (CLASS_PROTOCOL_LIST (implementation_template),
			     "class",
			     /* APPLE LOCAL begin radar 4568791 */
			     IDENTIFIER_POINTER (CLASS_NAME (objc_implementation_context)),
			     implementation_template);
			     /* APPLE LOCAL end radar 4568791 */
	}
    }

  else if (TREE_CODE (class) == CATEGORY_IMPLEMENTATION_TYPE)
    {
      tree category = lookup_category (implementation_template, CLASS_SUPER_NAME (class));

      if (category)
	{
      	  /* APPLE LOCAL begin C* property (Radar 4436866) */
          /* Generate what needed for property; setters, getters, etc. */
          objc_gen_property_data (implementation_template, category);
	  /* APPLE LOCAL end C* property (Radar 4436866) */
	  /* Ensure all method listed in the interface contain bodies.  */
    /* APPLE LOCAL begin radar 4568791 */
	  check_methods (CLASS_CLS_METHODS (category),
		         /* APPLE LOCAL radar 4359757 */
			 '+', NULL_TREE);
	  check_methods (CLASS_NST_METHODS (category),
			 /* APPLE LOCAL radar 4359757 */
			 '-', NULL_TREE);

	  if (CLASS_PROTOCOL_LIST (category))
	    check_protocols (CLASS_PROTOCOL_LIST (category),
			     "category",
			     IDENTIFIER_POINTER (CLASS_SUPER_NAME (objc_implementation_context)),
			     implementation_template);
        }
    }
  /* APPLE LOCAL end radar 4568791 */
  /* APPLE LOCAL radar 4695109 */
  /* PROTOCOL_IMPLEMENTATION_TYPE removed */
  /* APPLE LOCAL begin objc new property */
  else
    {
      tree x;
      /* APPLE LOCAL radar 5962694 */
      if (TREE_CODE (class) == CLASS_INTERFACE_TYPE || TREE_CODE (class) == CATEGORY_INTERFACE_TYPE)
	{
	  objc_compare_properties_with_super (class);
	  objc_merge_proto_properties_in_class (class, CLASS_PROTOCOL_LIST (class));
	}
      /* Process properties of the class. */
      for (x = CLASS_PROPERTY_DECL (class); x; x = TREE_CHAIN (x))
	{
	  tree getter_decl, setter_decl;
	  tree type = TREE_TYPE (x);
	  tree prop_name = PROPERTY_NAME (x);
	  if (PROPERTY_GETTER_NAME (x) == NULL_TREE)
	    {
	      /* No 'getter=name' attribute specified. Find the default getter and
		 if one not found, add one. */
	      getter_decl = lookup_method (CLASS_NST_METHODS (class), prop_name);
	      if (getter_decl)
	    	{
                  /* APPLE LOCAL begin radar 6083666 */
                  tree getter_type = TREE_VALUE (TREE_TYPE (getter_decl));
	          if ((comptypes (type, getter_type) != 1)
                      && !objc_compare_types (type, getter_type, -6, NULL_TREE, NULL))
		    /* APPLE LOCAL radar 4815054 */
	            warning (0, "type of accessor does not match the type of property %qs",
		             IDENTIFIER_POINTER (prop_name));
                  /* APPLE LOCAL end radar 6083666 */
		  if (METHOD_SEL_ARGS (getter_decl) != NULL_TREE)
		    error ("accessor %<%c%s%> cannot have any argument", 
		           '-', IDENTIFIER_POINTER (prop_name));	
	          METHOD_PROPERTY_CONTEXT (getter_decl) = x;
	          TREE_DEPRECATED (getter_decl) = TREE_DEPRECATED (x);
	    	}
	      /* APPLE LOCAL radar begin 4653422 */
	      else if (!OPTIONAL_PROPERTY (x) || TREE_CODE (class) == PROTOCOL_INTERFACE_TYPE)
	        {
	          tree rettype = build_tree_list (NULL_TREE, type);
	          getter_decl = build_method_decl (INSTANCE_METHOD_DECL,
	     				           rettype, prop_name, NULL_TREE,
						   false);
		  /* APPLE LOCAL begin radar 5839812 - synthesized methods  */
		  DECL_SOURCE_LOCATION (getter_decl) = 
		                                      DECL_SOURCE_LOCATION (x);
		  /* APPLE LOCAL end radar 5839812 - synthesized methods  */
	  	  objc_add_method (class, getter_decl, false, OPTIONAL_PROPERTY (x));
	          METHOD_PROPERTY_CONTEXT (getter_decl) = x;
	          TREE_DEPRECATED (getter_decl) = TREE_DEPRECATED (x);
	        }
	      /* APPLE LOCAL radar end 4653422 */
	    }
	  else
	    {
	      /* getter = getter_name */
	      getter_decl = lookup_method (CLASS_NST_METHODS (class), PROPERTY_GETTER_NAME (x));
	      if (!getter_decl)
		{
		  /* getter in 'getter' declaration has no declaration in this class. Provide
		     one here. */
	          tree rettype = build_tree_list (NULL_TREE, type);
	          getter_decl = build_method_decl (INSTANCE_METHOD_DECL,
	     				           rettype, PROPERTY_GETTER_NAME (x), 
						   NULL_TREE, false);
		  /* APPLE LOCAL begin radar 5839812 - synthesized methods  */
		  DECL_SOURCE_LOCATION (getter_decl) = 
		                                      DECL_SOURCE_LOCATION (x);
		  /* APPLE LOCAL end radar 5839812 - synthesized methods  */
	  	  objc_add_method (class, getter_decl, false, false);
		  /* APPLE LOCAL radar 5390587 */
	          METHOD_PROPERTY_CONTEXT (getter_decl) = x;
		}
	      else
		{
		  if (comptypes (type, TREE_VALUE (TREE_TYPE (getter_decl))) != 1)
		    /* APPLE LOCAL radar 4815054 */
		    error ("type of existing accessor %<%c%s%> does not match the type of property %qs",
			   '-', IDENTIFIER_POINTER (PROPERTY_GETTER_NAME (x)), IDENTIFIER_POINTER (prop_name));
		  if (METHOD_SEL_ARGS (getter_decl) != NULL_TREE)
		    error ("existing accessor %<%c%s%> cannot have any argument",
			   '-', IDENTIFIER_POINTER (PROPERTY_GETTER_NAME (x)));	
		}
	     /* APPLE LOCAL radar 4712415 */
	     TREE_DEPRECATED (getter_decl) = TREE_DEPRECATED (x);
	    }
	  if (PROPERTY_SETTER_NAME (x) == NULL_TREE
	      && PROPERTY_READONLY (x) == boolean_false_node)
	    {
	      /* No setter is specified and it is not a 'readonly' property. Generate an instance
		 or find an existing one. */
	      tree setter_name = get_identifier (objc_build_property_setter_name (
				   prop_name, true)); 
	      setter_decl = lookup_method (CLASS_NST_METHODS (class), setter_name);
	      if (setter_decl)
		{
		  /* An eixsting setter must take a single argument of type matching the property's
		     type and must return 'void'. */
		  if (TREE_VALUE (TREE_TYPE (setter_decl)) != void_type_node)
		    error ("setter %<%c%s%>  type must be 'void'",
			   '-', IDENTIFIER_POINTER (prop_name));
		  if (!METHOD_SEL_ARGS (setter_decl))
		    error ("setter %<%c%s%> has no argument", '-', IDENTIFIER_POINTER (prop_name));
		  else
		    {
		      tree akey = METHOD_SEL_ARGS (setter_decl);
		      tree arg_type = TREE_VALUE (TREE_TYPE (akey));
		      if (comptypes (type, arg_type) != 1)
			error ("setter %<%c%s%> argument type does not match property type",
				'-', IDENTIFIER_POINTER (prop_name));
		      akey = TREE_CHAIN (akey);
		      if (akey)
			error ("setter %<%c%s%> has too many arguments",
				'-', IDENTIFIER_POINTER (prop_name));
		    }
	    	  /* APPLE LOCAL radar 4712415 */
	    	  TREE_DEPRECATED (setter_decl) = TREE_DEPRECATED (x);
		}
	      /* APPLE LOCAL radar begin 4653422 */
	      else if (!OPTIONAL_PROPERTY (x) 
		       || TREE_CODE (class) == PROTOCOL_INTERFACE_TYPE)
		objc_add_property_setter_method (class, x);
	      /* APPLE LOCAL radar end 4653422 */
	    }
	  else if (PROPERTY_SETTER_NAME (x))
	    {
	      if (PROPERTY_READONLY (x) == boolean_true_node)
		error ("setter cannot be specified for a 'readonly' property %qs"
		       , IDENTIFIER_POINTER (PROPERTY_NAME (x)));
	      else
		{
		  /* User setter is specified in the attribute. See if it is of correct form. */
		  setter_decl = lookup_method (CLASS_NST_METHODS (class), PROPERTY_SETTER_NAME (x));
		  if (!setter_decl)
		    {
		      tree prop;
		      /* setter declared in 'setter' attribute has no declaration in current
			 class. Provide this declaration internally. */
		      tree arg_type = build_tree_list (NULL_TREE, type);
		      tree arg_name = get_identifier ("_value");
		      /* APPLE LOCAL radar 4968128 */
		      tree selector = objc_build_keyword_decl (strip_end_colon (PROPERTY_SETTER_NAME (x)),
							       /* APPLE LOCAL radar 4157812 */
							       arg_type, arg_name, NULL_TREE);
		      setter_decl = build_method_decl (INSTANCE_METHOD_DECL,
                                      build_tree_list (NULL_TREE, void_type_node), selector,
                                      build_tree_list (NULL_TREE, NULL_TREE), false);
		      /* APPLE LOCAL begin radar 5839812 - synthesized methods  */
		      DECL_SOURCE_LOCATION (setter_decl) = 
		                                      DECL_SOURCE_LOCATION (x);
		      /* APPLE LOCAL end radar 5839812 - synthesized methods  */
		      objc_add_method (class, setter_decl, false, false);
		      METHOD_PROPERTY_CONTEXT (setter_decl) = x;
		      /* Issue error if setter name matches a property name. */
		      for (prop = CLASS_PROPERTY_DECL (class); prop; prop = TREE_CHAIN (prop))
			{
			  /* APPLE LOCAL begin radar 5338634 */
			  const char *property_name = IDENTIFIER_POINTER (PROPERTY_NAME (prop));
			  const char *setter_name = IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (x));
			  size_t len = strlen (setter_name) - 1;
			  if (strlen (property_name) == len  && !strncmp (setter_name, property_name, len))
			    {
			      error ("setter name %qs matches property %qs name", 
				     setter_name, property_name);
			      break;
			    }
			  /* APPLE LOCAL end radar 5338634 */
			}
		    }
		  else
		    {
		      if (TREE_VALUE (TREE_TYPE (setter_decl)) != void_type_node)
			error ("user setter's type must be 'void' for property %qs",
			       IDENTIFIER_POINTER (PROPERTY_NAME (x)));
		      if (!METHOD_SEL_ARGS (setter_decl))
		    	error ("setter %<%c%s%> has no argument",
				'-', IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (x)));
		      else
		    	{
		          tree akey = METHOD_SEL_ARGS (setter_decl);
			  if (akey)
			  {
		            tree arg_type = TREE_VALUE (TREE_TYPE (akey));
		            if (comptypes (type, arg_type) != 1)
			      /* APPLE LOCAL radar 4815054 */
			      error ("argument type of setter %<%c%s%> does not match the type of property %qs",
				     '-', IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (x)), 
				     IDENTIFIER_POINTER (PROPERTY_NAME (x)));
		            akey = TREE_CHAIN (akey);
			    if (akey)
			      error ("specified setter %<%c%s%> has too many arguments",
				     '-', IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (x)));
			  }
			  else
			    error ("specified setter %<%c%s%> has no argument",
				     '-', IDENTIFIER_POINTER (PROPERTY_SETTER_NAME (x)));
		    	}
		    }
	    	    /* APPLE LOCAL radar 4712415 */
	    	    TREE_DEPRECATED (setter_decl) = TREE_DEPRECATED (x);
		}	
	    }
	  if (PROPERTY_RETAIN (x) == boolean_true_node)
	    {
	      if (!objc_type_valid_for_messaging (type))
	        error ("property %qs with 'retain' attribute must be of object type",
		       IDENTIFIER_POINTER (PROPERTY_NAME (x)));
	      if (objc_is_gcable_type (type) == -1)
		error ("property %qs with 'retain' attribute cannot be used with __weak storage specifier",
		       IDENTIFIER_POINTER (PROPERTY_NAME (x)));
	    }
	  if (PROPERTY_COPY (x) == boolean_true_node)
	    {
	      if (!objc_type_valid_for_messaging (type))
	        error ("property %qs with 'copy' attribute must be of object type",
		       IDENTIFIER_POINTER (PROPERTY_NAME (x)));
	      if (objc_is_gcable_type (type) == -1)
		error ("property %qs with 'copy' attribute cannot be used with __weak storage specifier",
		       IDENTIFIER_POINTER (PROPERTY_NAME (x)));
	    }
	}
        /* APPLE LOCAL begin radar 4965989 */ 
        /* New anonymous category */
        if (ANONYMOUS_CATEGORY (class))
          {
            /* Merge all anonymous category's methods (including setter/getter) into its primary class. */
            tree primary = lookup_interface (CLASS_NAME (class));
            if (primary)
              {
                objc_merge_methods (primary, CLASS_NST_METHODS (class), '-');
                objc_merge_methods (primary, CLASS_CLS_METHODS (class), '+');
	        /* APPLE LOCAL begin radar 6017984 */
		/* Just as we are adding anonymous category's methods to the
		   primary class, must also add methods in anonymous category's protocols
		   to the class as well, */
		objc_merge_protocol_methods (primary, CLASS_PROTOCOL_LIST (class));
	        /* APPLE LOCAL end radar 6017984 */
              } 
          }
        /* APPLE LOCAL end radar 4965989 */
    }
  /* APPLE LOCAL end objc new property */
}

static tree
add_protocol (tree protocol)
{
  /* Put protocol on list in reverse order.  */
  TREE_CHAIN (protocol) = protocol_chain;
  protocol_chain = protocol;
  return protocol_chain;
}

static tree
lookup_protocol (tree ident)
{
  tree chain;

  for (chain = protocol_chain; chain; chain = TREE_CHAIN (chain))
    if (ident == PROTOCOL_NAME (chain))
      return chain;

  return NULL_TREE;
}

/* This function forward declares the protocols named by NAMES.  If
   they are already declared or defined, the function has no effect.  */

void
/* APPLE LOCAL radar 4947311 - protocol attributes */
objc_declare_protocols (tree names, tree prefix_attrs)
{
  tree list;

#ifdef OBJCPLUS
  if (current_namespace != global_namespace) {
    error ("Objective-C declarations may only appear in global scope");
  }
#endif /* OBJCPLUS */

  for (list = names; list; list = TREE_CHAIN (list))
    {
      tree name = TREE_VALUE (list);

      if (lookup_protocol (name) == NULL_TREE)
	{
	  tree protocol = make_node (PROTOCOL_INTERFACE_TYPE);

	  TYPE_LANG_SLOT_1 (protocol)
	    = make_tree_vec (PROTOCOL_LANG_SLOT_ELTS);
	  PROTOCOL_NAME (protocol) = name;
	  PROTOCOL_LIST (protocol) = NULL_TREE;
	  add_protocol (protocol);
	  PROTOCOL_DEFINED (protocol) = 0;
	  PROTOCOL_FORWARD_DECL (protocol) = NULL_TREE;
	  /* APPLE LOCAL ObjC abi v2 */
	  PROTOCOL_V2_FORWARD_DECL (protocol) = NULL_TREE;
	  /* APPLE LOCAL radar 4947311 - protocol attributes */
	  PROTOCOL_ATTRIBUTES (protocol) = prefix_attrs;
	}
    }
}

static tree
/* APPLE LOCAL radar 4947311 - protocol attributes */
start_protocol (tree name, tree list, tree prefix_attrs)
{
  tree protocol;

#ifdef OBJCPLUS
  if (current_namespace != global_namespace) {
    error ("Objective-C declarations may only appear in global scope");
  }
#endif /* OBJCPLUS */

  protocol = lookup_protocol (name);

  if (!protocol)
    {
      /* APPLE LOCAL radar 4947311 - protocol attributes */
      protocol = make_node (PROTOCOL_INTERFACE_TYPE);
      TYPE_LANG_SLOT_1 (protocol) = make_tree_vec (PROTOCOL_LANG_SLOT_ELTS);

      PROTOCOL_NAME (protocol) = name;
      /* APPLE LOCAL radar 4398221 */
      PROTOCOL_LIST (protocol) = lookup_and_install_protocols (list, false);
      add_protocol (protocol);
      PROTOCOL_DEFINED (protocol) = 1;
      PROTOCOL_FORWARD_DECL (protocol) = NULL_TREE;
      /* APPLE LOCAL ObjC abi v2 */
      PROTOCOL_V2_FORWARD_DECL (protocol) = NULL_TREE;
      /* APPLE LOCAL radar 4947311 - protocol attributes */
      PROTOCOL_ATTRIBUTES (protocol) = prefix_attrs;
      check_protocol_recursively (protocol, list);
    }
  else if (! PROTOCOL_DEFINED (protocol))
    {
      PROTOCOL_DEFINED (protocol) = 1;
      /* APPLE LOCAL begin radar 4947311 - protocol attributes */
      PROTOCOL_ATTRIBUTES (protocol) = merge_attributes 
					 (PROTOCOL_ATTRIBUTES (protocol), 
					  prefix_attrs);
      /* APPLE LOCAL end radar 4947311 - protocol attributes */
      /* APPLE LOCAL radar 4398221 */
      PROTOCOL_LIST (protocol) = lookup_and_install_protocols (list, false);

      check_protocol_recursively (protocol, list);
    }
  else
    {
      warning (0, "duplicate declaration for protocol %qs",
	       IDENTIFIER_POINTER (name));
    }
  return protocol;
}


/* "Encode" a data type into a string, which grows in util_obstack.
   ??? What is the FORMAT?  Someone please document this!  */

static void
encode_type_qualifiers (tree declspecs)
{
  tree spec;

  for (spec = declspecs; spec; spec = TREE_CHAIN (spec))
    {
      if (ridpointers[(int) RID_IN] == TREE_VALUE (spec))
	obstack_1grow (&util_obstack, 'n');
      else if (ridpointers[(int) RID_INOUT] == TREE_VALUE (spec))
	obstack_1grow (&util_obstack, 'N');
      else if (ridpointers[(int) RID_OUT] == TREE_VALUE (spec))
	obstack_1grow (&util_obstack, 'o');
      else if (ridpointers[(int) RID_BYCOPY] == TREE_VALUE (spec))
	obstack_1grow (&util_obstack, 'O');
      else if (ridpointers[(int) RID_BYREF] == TREE_VALUE (spec))
        obstack_1grow (&util_obstack, 'R');
      else if (ridpointers[(int) RID_ONEWAY] == TREE_VALUE (spec))
	obstack_1grow (&util_obstack, 'V');
    }
}

/* APPLE LOCAL begin 4136935 */
/* Determine if a pointee is marked read-only.  */

static bool
pointee_is_readonly (tree pointee)
{
  while (POINTER_TYPE_P (pointee))
    pointee = TREE_TYPE (pointee);

  return TYPE_READONLY (pointee);
}

/* APPLE LOCAL end 4136935 */
/* APPLE LOCAL begin radar 4577849 */
static void encode_protocol_list (tree record, bool quote)
{
  tree p;
  bool firsttime = true;
  bool close_quote = false;
  for (p = TYPE_OBJC_PROTOCOL_LIST (record); p; p = TREE_CHAIN (p))
    {
      tree proto = TREE_VALUE (p);
      if (TREE_CODE (proto) == PROTOCOL_INTERFACE_TYPE
	  && PROTOCOL_NAME (proto))
	{
	  const char *name = IDENTIFIER_POINTER (PROTOCOL_NAME (proto));
	  if (quote && firsttime)
	   {
	     obstack_1grow (&util_obstack, '"');
	     firsttime = false;
	     close_quote = true;
	   }
	  obstack_1grow (&util_obstack, '<');
	  obstack_grow (&util_obstack, name, strlen (name));
	  obstack_1grow (&util_obstack, '>');
	}
    }
  if (quote && close_quote)
    obstack_1grow (&util_obstack, '"');
}
/* APPLE LOCAL end radar 4577849 */

/* Encode a pointer type.  */

static void
encode_pointer (tree type, int curtype, int format)
{
  tree pointer_to = TREE_TYPE (type);

  /* APPLE LOCAL begin 4136935 */
  /* For historical/compatibility reasons, the read-only qualifier of the
     pointee gets emitted _before_ the '^'.  The read-only qualifier of
     the pointer itself gets ignored, _unless_ we are looking at a typedef!  
     Also, do not emit the 'r' for anything but the outermost type!  */
  if (!generating_instance_variables
      && (obstack_object_size (&util_obstack) - curtype <= 1)
      && (TYPE_NAME (type) && TREE_CODE (TYPE_NAME (type)) == TYPE_DECL
	  ? TYPE_READONLY (type)
	  : pointee_is_readonly (pointer_to)))
    obstack_1grow (&util_obstack, 'r');

  /* APPLE LOCAL end 4136935 */
  if (TREE_CODE (pointer_to) == RECORD_TYPE)
    {
      /* APPLE LOCAL begin radar 5002848 */
      if (generating_property_type_encoding &&
          TYPE_NAME (pointer_to)
          && TREE_CODE (TYPE_NAME (pointer_to)) == TYPE_DECL
          && DECL_ORIGINAL_TYPE (TYPE_NAME (pointer_to)))
      pointer_to = DECL_ORIGINAL_TYPE (TYPE_NAME (pointer_to));
      /* APPLE LOCAL end radar 5002848 */

      if (OBJC_TYPE_NAME (pointer_to)
	  && TREE_CODE (OBJC_TYPE_NAME (pointer_to)) == IDENTIFIER_NODE)
	{
	  const char *name = IDENTIFIER_POINTER (OBJC_TYPE_NAME (pointer_to));

	  if (strcmp (name, TAG_OBJECT) == 0) /* '@' */
	    {
	      obstack_1grow (&util_obstack, '@');
	      /* APPLE LOCAL begin radar 4577849 */
	      if (generating_instance_variables 
		  && TYPE_HAS_OBJC_INFO (pointer_to))
	        encode_protocol_list (pointer_to, true);
	      /* APPLE LOCAL end radar 4577849 */
	      return;
	    }
	  else if (TYPE_HAS_OBJC_INFO (pointer_to)
		   && TYPE_OBJC_INTERFACE (pointer_to))
	    {
              if (generating_instance_variables)
	        {
	          obstack_1grow (&util_obstack, '@');
	          obstack_1grow (&util_obstack, '"');
	          obstack_grow (&util_obstack, name, strlen (name));
		  /* APPLE LOCAL radar 4577849 */
		  encode_protocol_list (pointer_to, false);
	          obstack_1grow (&util_obstack, '"');
	          return;
		}
              else
	        {
	          obstack_1grow (&util_obstack, '@');
	          return;
		}
	    }
	  else if (strcmp (name, TAG_CLASS) == 0) /* '#' */
	    {
	      obstack_1grow (&util_obstack, '#');
	      return;
	    }
	  else if (strcmp (name, TAG_SELECTOR) == 0) /* ':' */
	    {
	      obstack_1grow (&util_obstack, ':');
	      return;
	    }
	}
    }
  else if (TREE_CODE (pointer_to) == INTEGER_TYPE
	   && TYPE_MODE (pointer_to) == QImode)
    {
      tree pname = TREE_CODE (OBJC_TYPE_NAME (pointer_to)) == IDENTIFIER_NODE
	          ? OBJC_TYPE_NAME (pointer_to)
	          : DECL_NAME (OBJC_TYPE_NAME (pointer_to));

      if (!flag_next_runtime || strcmp (IDENTIFIER_POINTER (pname), "BOOL"))
	{
	  /* APPLE LOCAL 4136935 */
	  /* The "r" is now generated above.  */

	  obstack_1grow (&util_obstack, '*');
	  return;
	}
    }

  /* We have a type that does not get special treatment.  */

  /* NeXT extension */
  /* APPLE LOCAL radar 5849129 */
  obstack_1grow (&util_obstack, TREE_CODE (type) == BLOCK_POINTER_TYPE ? '@' : '^');
  encode_type (pointer_to, curtype, format);
}

static void
encode_array (tree type, int curtype, int format)
{
  tree an_int_cst = TYPE_SIZE (type);
  tree array_of = TREE_TYPE (type);
  char buffer[40];

  /* An incomplete array is treated like a pointer.  */
  if (an_int_cst == NULL)
    {
      encode_pointer (type, curtype, format);
      return;
    }

  if (TREE_INT_CST_LOW (TYPE_SIZE (array_of)) == 0)
   sprintf (buffer, "[" HOST_WIDE_INT_PRINT_DEC, (HOST_WIDE_INT)0);
  else
    sprintf (buffer, "[" HOST_WIDE_INT_PRINT_DEC,
	     TREE_INT_CST_LOW (an_int_cst)
	      / TREE_INT_CST_LOW (TYPE_SIZE (array_of)));

  obstack_grow (&util_obstack, buffer, strlen (buffer));
  encode_type (array_of, curtype, format);
  obstack_1grow (&util_obstack, ']');
  return;
}

static void
encode_aggregate_fields (tree type, int pointed_to, int curtype, int format)
{
  tree field = TYPE_FIELDS (type);

  for (; field; field = TREE_CHAIN (field))
    {
#ifdef OBJCPLUS
      /* C++ static members, and things that are not field at all,
	 should not appear in the encoding.  */
      if (TREE_CODE (field) != FIELD_DECL || TREE_STATIC (field))
	continue;
#endif

      /* Recursively encode fields of embedded base classes.  */
      if (DECL_ARTIFICIAL (field) && !DECL_NAME (field)
	  && TREE_CODE (TREE_TYPE (field)) == RECORD_TYPE)
	{
	  encode_aggregate_fields (TREE_TYPE (field),
				   pointed_to, curtype, format);
	  continue;
	}

      if (generating_instance_variables && !pointed_to)
	{
	  tree fname = DECL_NAME (field);

	  obstack_1grow (&util_obstack, '"');

	  if (fname && TREE_CODE (fname) == IDENTIFIER_NODE)
	    obstack_grow (&util_obstack,
			  IDENTIFIER_POINTER (fname),
			  strlen (IDENTIFIER_POINTER (fname)));

	  obstack_1grow (&util_obstack, '"');
        }

      encode_field_decl (field, curtype, format);
    }
}

static void
encode_aggregate_within (tree type, int curtype, int format, int left,
			 int right)
{
  tree name;
  /* NB: aggregates that are pointed to have slightly different encoding
     rules in that you never encode the names of instance variables.  */
  /* APPLE LOCAL begin 4136935 */
  int ob_size = obstack_object_size (&util_obstack);
  int pointed_to = (ob_size > 0
		    ? *(obstack_next_free (&util_obstack) - 1) == '^'
		    : 0);
  int inline_contents
    = ((format == OBJC_ENCODE_INLINE_DEFS || generating_instance_variables)
       && (!pointed_to
	   || ob_size - curtype == 1
	   || (ob_size - curtype == 2
	       && *(obstack_next_free (&util_obstack) - 2) == 'r')));
  /* APPLE LOCAL end 4136935 */

  /* Traverse struct aliases; it is important to get the
     original struct and its tag name (if any).  */
  type = TYPE_MAIN_VARIANT (type);
  name = OBJC_TYPE_NAME (type);
  /* Open parenth/bracket.  */
  obstack_1grow (&util_obstack, left);

  /* Encode the struct/union tag name, or '?' if a tag was
     not provided.  Typedef aliases do not qualify.  */
  if (name && TREE_CODE (name) == IDENTIFIER_NODE
#ifdef OBJCPLUS
      /* Did this struct have a tag?  */
      && !TYPE_WAS_ANONYMOUS (type)
#endif
      )
    obstack_grow (&util_obstack,
		  IDENTIFIER_POINTER (name),
		  strlen (IDENTIFIER_POINTER (name)));
  else
    obstack_1grow (&util_obstack, '?');

  /* Encode the types (and possibly names) of the inner fields,
     if required.  */
  if (inline_contents)
    {
      obstack_1grow (&util_obstack, '=');
      encode_aggregate_fields (type, pointed_to, curtype, format);
    }
  /* Close parenth/bracket.  */
  obstack_1grow (&util_obstack, right);
}

static void
encode_aggregate (tree type, int curtype, int format)
{
  enum tree_code code = TREE_CODE (type);

  switch (code)
    {
    case RECORD_TYPE:
      {
	encode_aggregate_within (type, curtype, format, '{', '}');
	break;
      }
    case UNION_TYPE:
      {
	encode_aggregate_within (type, curtype, format, '(', ')');
	break;
      }

    case ENUMERAL_TYPE:
      obstack_1grow (&util_obstack, 'i');
      break;

    default:
      break;
    }
}

/* Encode a bitfield NeXT-style (i.e., without a bit offset or the underlying
   field type.  */

static void
encode_next_bitfield (int width)
{
  char buffer[40];
  sprintf (buffer, "b%d", width);
  obstack_grow (&util_obstack, buffer, strlen (buffer));
}

/* FORMAT will be OBJC_ENCODE_INLINE_DEFS or OBJC_ENCODE_DONT_INLINE_DEFS.  */
static void
encode_type (tree type, int curtype, int format)
{
  enum tree_code code = TREE_CODE (type);
  char c;
  /* APPLE LOCAL begin 4136935 */
  tree int_type;

  /* Ignore type qualifiers when encoding a type; see encode_pointer()
     for an exception to this rule.  */
  /* APPLE LOCAL end 4136935 */
  if (code == INTEGER_TYPE)
    {
      switch (GET_MODE_BITSIZE (TYPE_MODE (type)))
	{
	case 8:  c = TYPE_UNSIGNED (type) ? 'C' : 'c'; break;
	case 16: c = TYPE_UNSIGNED (type) ? 'S' : 's'; break;
	/* APPLE LOCAL begin 4136935 */
	case 32:
	  /* Another legacy kludge: 32-bit longs are encoded as 'l' or 'L',
	     but not always.  For typedefs, we need to use 'i' or 'I' instead
	     if encoding a struct field, or a pointer!  */
	  int_type =  ((!generating_instance_variables
			&& (obstack_object_size (&util_obstack)
			    == (unsigned) curtype))
		       ? TYPE_MAIN_VARIANT (type)
		       : type);

	  if (int_type == long_unsigned_type_node
	      || int_type == long_integer_type_node)
	  /* APPLE LOCAL end 4136935 */
	         c = TYPE_UNSIGNED (type) ? 'L' : 'l';
	  else
	         c = TYPE_UNSIGNED (type) ? 'I' : 'i';
	  break;
	case 64: c = TYPE_UNSIGNED (type) ? 'Q' : 'q'; break;
        /* APPLE LOCAL radar 5996271 */
	case 128: c = TYPE_UNSIGNED (type) ? 'T' : 't'; break;
	default: abort ();
	}
      obstack_1grow (&util_obstack, c);
    }

  else if (code == REAL_TYPE)
    {
      /* Floating point types.  */
      switch (GET_MODE_BITSIZE (TYPE_MODE (type)))
	{
	case 32:  c = 'f'; break;
        /* APPLE LOCAL begin radar 4900615 */
	case 64:  c = 'd'; break;
	case 96:
	case 128: c = 'D'; break;
        /* APPLE LOCAL end radar 4900615 */
	default: abort ();
	}
      obstack_1grow (&util_obstack, c);
    }

  else if (code == VOID_TYPE)
    obstack_1grow (&util_obstack, 'v');

  else if (code == BOOLEAN_TYPE)
    obstack_1grow (&util_obstack, 'B');

  else if (code == ARRAY_TYPE)
    encode_array (type, curtype, format);

/* APPLE LOCAL begin radar 4476365 */
#ifdef OBJCPLUS
  else if (code == REFERENCE_TYPE)
    encode_pointer (type, curtype, format);
#endif
/* APPLE LOCAL end radar 4476365 */

  /* APPLE LOCAL radar 5849129 */
  else if (code == POINTER_TYPE || code == BLOCK_POINTER_TYPE)
    encode_pointer (type, curtype, format);

  else if (code == RECORD_TYPE || code == UNION_TYPE || code == ENUMERAL_TYPE)
    encode_aggregate (type, curtype, format);

  else if (code == FUNCTION_TYPE) /* '?' */
    obstack_1grow (&util_obstack, '?');

  /* APPLE LOCAL begin 4136935 */
  /* Super-kludge.  Some ObjC qualifier and type combinations need to be
     rearranged for compatibility with gcc-3.3.  */
  if (code == POINTER_TYPE && obstack_object_size (&util_obstack) >= 3)
    {
      char *enc = obstack_base (&util_obstack) + curtype;

      /* Rewrite "in const" from "nr" to "rn".  */
      if (curtype >= 1 && !strncmp (enc - 1, "nr", 2))
        strncpy (enc - 1, "rn", 2);
      /* APPLE LOCAL begin radar 4301047 */
      /* code removed */
      /* APPLE LOCAL end radar 4301047 */
    }
  /* APPLE LOCAL end 4136935 */

  else if (code == COMPLEX_TYPE)
    {
      obstack_1grow (&util_obstack, 'j');
      encode_type (TREE_TYPE (type), curtype, format);
    }
}

static void
encode_gnu_bitfield (int position, tree type, int size)
{
  enum tree_code code = TREE_CODE (type);
  char buffer[40];
  char charType = '?';

  if (code == INTEGER_TYPE)
    {
      if (integer_zerop (TYPE_MIN_VALUE (type)))
	{
	  /* Unsigned integer types.  */

	  if (TYPE_MODE (type) == QImode)
	    charType = 'C';
	  else if (TYPE_MODE (type) == HImode)
	    charType = 'S';
	  else if (TYPE_MODE (type) == SImode)
	    {
	      if (type == long_unsigned_type_node)
		charType = 'L';
	      else
		charType = 'I';
	    }
	  else if (TYPE_MODE (type) == DImode)
	    charType = 'Q';
	}

      else
	/* Signed integer types.  */
	{
	  if (TYPE_MODE (type) == QImode)
	    charType = 'c';
	  else if (TYPE_MODE (type) == HImode)
	    charType = 's';
	  else if (TYPE_MODE (type) == SImode)
	    {
	      if (type == long_integer_type_node)
		charType = 'l';
	      else
		charType = 'i';
	    }

	  else if (TYPE_MODE (type) == DImode)
	    charType = 'q';
	}
    }
  else if (code == ENUMERAL_TYPE)
    charType = 'i';
  else
    abort ();

  sprintf (buffer, "b%d%c%d", position, charType, size);
  obstack_grow (&util_obstack, buffer, strlen (buffer));
}

static void
encode_field_decl (tree field_decl, int curtype, int format)
{
  tree type;

#ifdef OBJCPLUS
  /* C++ static members, and things that are not fields at all,
     should not appear in the encoding.  */
  if (TREE_CODE (field_decl) != FIELD_DECL || TREE_STATIC (field_decl))
    return;
#endif

  type = TREE_TYPE (field_decl);

  /* Generate the bitfield typing information, if needed.  Note the difference
     between GNU and NeXT runtimes.  */
  if (DECL_BIT_FIELD_TYPE (field_decl))
    {
      int size = tree_low_cst (DECL_SIZE (field_decl), 1);

      if (flag_next_runtime)
	encode_next_bitfield (size);
      else
	encode_gnu_bitfield (int_bit_position (field_decl),
				  DECL_BIT_FIELD_TYPE (field_decl), size);
    }
  else
    encode_type (TREE_TYPE (field_decl), curtype, format);
}

/* APPLE LOCAL begin 4209854 */
/* Decay array and function parameters into pointers.  */

static tree
objc_decay_parm_type (tree type)
{
  if (TREE_CODE (type) == ARRAY_TYPE || TREE_CODE (type) == FUNCTION_TYPE)
    type = build_pointer_type (TREE_CODE (type) == ARRAY_TYPE
			       ? TREE_TYPE (type)
			       : type);

  return type;
}

/* APPLE LOCAL end 4209854 */
static GTY(()) tree objc_parmlist = NULL_TREE;

/* Append PARM to a list of formal parameters of a method, making a necessary
   array-to-pointer adjustment along the way.  */

static void
objc_push_parm (tree parm)
{
  /* APPLE LOCAL begin 4209854 */
  /* Decay arrays and functions into pointers.  */
  tree type = objc_decay_parm_type (TREE_TYPE (parm));

  /* If the parameter type has been decayed, a new PARM_DECL needs to be
     built as well.  */
  if (type != TREE_TYPE (parm))
    {
      TREE_TYPE (parm) = type;
      relayout_decl (parm);
    }
  /* APPLE LOCAL end 4209854 */

  DECL_ARG_TYPE (parm)
    = lang_hooks.types.type_promotes_to (TREE_TYPE (parm));

  /* Record constancy and volatility.  */
  c_apply_type_quals_to_decl
  ((TYPE_READONLY (TREE_TYPE (parm)) ? TYPE_QUAL_CONST : 0)
   | (TYPE_RESTRICT (TREE_TYPE (parm)) ? TYPE_QUAL_RESTRICT : 0)
   | (TYPE_VOLATILE (TREE_TYPE (parm)) ? TYPE_QUAL_VOLATILE : 0), parm);

  objc_parmlist = chainon (objc_parmlist, parm);
}

/* Retrieve the formal parameter list constructed via preceding calls to
   objc_push_parm().  */

#ifdef OBJCPLUS
static tree
objc_get_parm_info (int have_ellipsis ATTRIBUTE_UNUSED)
#else
static struct c_arg_info *
objc_get_parm_info (int have_ellipsis)
#endif
{
#ifdef OBJCPLUS
  tree parm_info = objc_parmlist;
  objc_parmlist = NULL_TREE;

  return parm_info;
#else
  tree parm_info = objc_parmlist;
  struct c_arg_info *arg_info;
  /* The C front-end requires an elaborate song and dance at
     this point.  */
  push_scope ();
  declare_parm_level ();
  while (parm_info)
    {
      tree next = TREE_CHAIN (parm_info);

      TREE_CHAIN (parm_info) = NULL_TREE;
      parm_info = pushdecl (parm_info);
      finish_decl (parm_info, NULL_TREE, NULL_TREE);
      parm_info = next;
    }
  arg_info = get_parm_info (have_ellipsis);
  pop_scope ();
  objc_parmlist = NULL_TREE;
  return arg_info;
#endif
}

/* Synthesize the formal parameters 'id self' and 'SEL _cmd' needed for ObjC
   method definitions.  In the case of instance methods, we can be more
   specific as to the type of 'self'.  */

static void
synth_self_and_ucmd_args (void)
{
  tree self_type;

  if (objc_method_context
      && TREE_CODE (objc_method_context) == INSTANCE_METHOD_DECL)
    self_type = objc_instance_type;
  else
    /* Really a `struct objc_class *'. However, we allow people to
       assign to self, which changes its type midstream.  */
    self_type = objc_object_type;

  /* id self; */
  objc_push_parm (build_decl (PARM_DECL, self_id, self_type));

  /* SEL _cmd; */
  objc_push_parm (build_decl (PARM_DECL, ucmd_id, objc_selector_type));
}

/* Transform an Objective-C method definition into a static C function
   definition, synthesizing the first two arguments, "self" and "_cmd",
   in the process.  */

static void
start_method_def (tree method)
{
  tree parmlist;
#ifdef OBJCPLUS
  tree parm_info;
#else
  struct c_arg_info *parm_info;
#endif
  int have_ellipsis = 0;

  /* If we are defining a "dealloc" method in a non-root class, we
     will need to check if a [super dealloc] is missing, and warn if
     it is.  */
  if(CLASS_SUPER_NAME (objc_implementation_context)
     && !strcmp ("dealloc", IDENTIFIER_POINTER (METHOD_SEL_NAME (method))))
    should_call_super_dealloc = 1;
  else
    should_call_super_dealloc = 0;
  /* APPLE LOCAL begin radar 4757423 */
  /* If we are defining a "finalize" method in a non-root class, we will need
     to check if a [super finalize] is missing, and warn if it is.  */
  should_call_super_finalize =
    CLASS_SUPER_NAME (objc_implementation_context)
    /* APPLE LOCAL radar 5424473 */
    && TREE_CODE (method) == INSTANCE_METHOD_DECL
    && !strcmp ("finalize", IDENTIFIER_POINTER (METHOD_SEL_NAME (method)));
  /* APPLE LOCAL end radar 4757423 */

  /* Required to implement _msgSuper.  */
  objc_method_context = method;
  UOBJC_SUPER_decl = NULL_TREE;

  /* Generate prototype declarations for arguments..."new-style".  */
  synth_self_and_ucmd_args ();

  /* Generate argument declarations if a keyword_decl.  */
  parmlist = METHOD_SEL_ARGS (method);
  while (parmlist)
    {
      tree type = TREE_VALUE (TREE_TYPE (parmlist)), parm;

      parm = build_decl (PARM_DECL, KEYWORD_ARG_NAME (parmlist), type);
      /* APPLE LOCAL radar 4157812 */
      decl_attributes (&parm, KEYWORD_ARG_ATTRIBUTE (parmlist), 0);
      objc_push_parm (parm);
      parmlist = TREE_CHAIN (parmlist);
    }

  if (METHOD_ADD_ARGS (method))
    {
      tree akey;

      for (akey = TREE_CHAIN (METHOD_ADD_ARGS (method));
	   akey; akey = TREE_CHAIN (akey))
	{
	  objc_push_parm (TREE_VALUE (akey));
	}

      if (METHOD_ADD_ARGS_ELLIPSIS_P (method))
	have_ellipsis = 1;
    }

  parm_info = objc_get_parm_info (have_ellipsis);

  really_start_method (objc_method_context, parm_info);
}

static void
warn_with_method (const char *message, int mtype, tree method)
{
  /* Add a readable method name to the warning.  */
  warning (0, "%J%s %<%c%s%>", method,
           message, mtype, gen_method_decl (method));
}

/* Return 1 if TYPE1 is equivalent to TYPE2
   for purposes of method overloading.  */

static int
objc_types_are_equivalent (tree type1, tree type2)
{
  if (type1 == type2)
    return 1;

  /* Strip away indirections.  */
  /* APPLE LOCAL begin radar 5795493 - blocks */
  while ((TREE_CODE (type1) == ARRAY_TYPE || TREE_CODE (type1) == POINTER_TYPE ||
          TREE_CODE (type1) == BLOCK_POINTER_TYPE)
  /* APPLE LOCAL end radar 5795493 - blocks */
	 && (TREE_CODE (type1) == TREE_CODE (type2)))
    type1 = TREE_TYPE (type1), type2 = TREE_TYPE (type2);
  if (TYPE_MAIN_VARIANT (type1) != TYPE_MAIN_VARIANT (type2))
    return 0;

  type1 = (TYPE_HAS_OBJC_INFO (type1)
	   ? TYPE_OBJC_PROTOCOL_LIST (type1)
	   : NULL_TREE);
  type2 = (TYPE_HAS_OBJC_INFO (type2)
	   ? TYPE_OBJC_PROTOCOL_LIST (type2)
	   : NULL_TREE);

  if (list_length (type1) == list_length (type2))
    {
      for (; type2; type2 = TREE_CHAIN (type2))
	if (!lookup_protocol_in_reflist (type1, TREE_VALUE (type2)))
	  return 0;
      return 1;
    }
  return 0;
}

/* Return 1 if TYPE1 has the same size and alignment as TYPE2.  */

static int
objc_types_share_size_and_alignment (tree type1, tree type2)
{
  return (simple_cst_equal (TYPE_SIZE (type1), TYPE_SIZE (type2))
	  && TYPE_ALIGN (type1) == TYPE_ALIGN (type2));
}

/* Return 1 if PROTO1 is equivalent to PROTO2
   for purposes of method overloading.  Ordinarily, the type signatures
   should match up exactly, unless STRICT is zero, in which case we
   shall allow differences in which the size and alignment of a type
   // APPLE LOCAL radar 5370783
   is the same.  When STRICT is 1, we allow for valid object type comparisons. */

static int
comp_proto_with_proto (tree proto1, tree proto2, int strict)
{
  /* APPLE LOCAL C* property (Radar 4436866) */
  /* code removed */
  /* The following test is needed in case there are hashing
     collisions.  */
  if (METHOD_SEL_NAME (proto1) != METHOD_SEL_NAME (proto2))
    return 0;
  /* APPLE LOCAL begin C* property (Radar 4436866) */
  return match_proto_with_proto (proto1, proto2, strict);
}

static int
match_proto_with_proto (tree proto1, tree proto2, int strict)
{
  tree type1, type2;
  /* APPLE LOCAL end C* property (Radar 4436866) */
  /* Compare return types.  */
  type1 = TREE_VALUE (TREE_TYPE (proto1));
  type2 = TREE_VALUE (TREE_TYPE (proto2));
  /* APPLE LOCAL begin radar 5370783 */
  if (!objc_types_are_equivalent (type1, type2)) {
    if (strict == 2)
      return 0;
    if (!objc_types_share_size_and_alignment (type1, type2))
      return 0;
    /* APPLE LOCAL radar 6231433 */
    if (strict == 1 && !objc_compare_types(type1, type2, -2, 0, NULL))
      return 0;
  }

  /* Compare argument types.  */
  /* APPLE LOCAL begin radar 6307941 */
  for (type1 = get_arg_type_list (METHOD_SEL_NAME (proto1), proto1, METHOD_REF, 0),
       type2 = get_arg_type_list (METHOD_SEL_NAME (proto2), proto2, METHOD_REF, 0);
  /* APPLE LOCAL end radar 6307941 */
       type1 && type2;
       type1 = TREE_CHAIN (type1), type2 = TREE_CHAIN (type2))
    {
      if (!objc_types_are_equivalent (TREE_VALUE (type1), TREE_VALUE (type2))) {
        if (strict == 2)
          return 0;
        if (!objc_types_share_size_and_alignment (TREE_VALUE (type1), TREE_VALUE (type2)))
          return 0;
        /* Note, order of type2 and type1 in argument call is intentional. */
	/* APPLE LOCAL radar 6231433 */
        if (strict == 1 && !objc_compare_types(TREE_VALUE (type2), TREE_VALUE (type1), -2, 0, NULL))
          return 0;
      }
    }
  /* APPLE LOCAL end radar 5370783 */

  return (!type1 && !type2);
}

/* Fold an OBJ_TYPE_REF expression for ObjC method dispatches, where
   this occurs.  ObjC method dispatches are _not_ like C++ virtual
   member function dispatches, and we account for the difference here.  */
tree
#ifdef OBJCPLUS
objc_fold_obj_type_ref (tree ref, tree known_type)
#else
objc_fold_obj_type_ref (tree ref ATTRIBUTE_UNUSED,
			tree known_type ATTRIBUTE_UNUSED)
#endif
{
#ifdef OBJCPLUS
  tree v = BINFO_VIRTUALS (TYPE_BINFO (known_type));

  /* If the receiver does not have virtual member functions, there
     is nothing we can (or need to) do here.  */
  if (!v)
    return NULL_TREE;

  /* Let C++ handle C++ virtual functions.  */
  return cp_fold_obj_type_ref (ref, known_type);
#else
  /* For plain ObjC, we currently do not need to do anything.  */
  return NULL_TREE;
#endif
}

static void
objc_start_function (tree name, tree type, tree attrs,
/* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
#ifdef OBJCPLUS
		     tree params,
#else
		     struct c_arg_info *params,
#endif
		     tree method)
/* APPLE LOCAL end radar 5839812 - location for synthesized methods  */
{
  tree fndecl = build_decl (FUNCTION_DECL, name, type);

#ifdef OBJCPLUS
  /* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
  /* fndecl's source location is, by default, the current input location
     (build_decl automatically does that).  If the fndecl is for a synthesized
     property's getter or setter method, then the current input location will 
     be the '@end', rather than the '@property' or '@synthesize' statement.
     The following statement detects that situation and re-sets fndecl to
     the correct source location for the '@property' or '@synthesize' 
     statement.  */
  if (method
      && (strcmp (DECL_SOURCE_FILE (fndecl), DECL_SOURCE_FILE (method)) != 0
	  || DECL_SOURCE_LINE (fndecl) != DECL_SOURCE_LINE (method)))
    DECL_SOURCE_LOCATION (fndecl) = DECL_SOURCE_LOCATION (method);
  /* APPLE LOCAL end radar 5839812 - location for synthesized methods  */
  DECL_ARGUMENTS (fndecl) = params;
  DECL_INITIAL (fndecl) = error_mark_node;
  DECL_EXTERNAL (fndecl) = 0;
  TREE_STATIC (fndecl) = 1;
  retrofit_lang_decl (fndecl);
  cplus_decl_attributes (&fndecl, attrs, 0);
  /* APPLE LOCAL begin optimization pragmas 3124235/3420242 */
  /* Build a mapping between this decl and the per-function options in
     effect at this point.  */
  record_func_cl_pf_opts_mapping (fndecl);
  /* APPLE LOCAL end optimization pragmas 3124235/3420242 */
  start_preparsed_function (fndecl, attrs, /*flags=*/SF_DEFAULT);
#else
  struct c_label_context_se *nstack_se;
  struct c_label_context_vm *nstack_vm;
  /* APPLE LOCAL begin radar 5839812 - location for synthesized methods  */
  /* fndecl's source location is, by default, the current input location
     (build_decl automatically does that).  If the fndecl is for a synthesized
     property's getter or setter method, then the current input location will 
     be the '@end', rather than the '@property' or '@synthesize' statement.
     The following statement detects that situation and re-sets fndecl to
     the correct source location for the '@property' or '@synthesize' 
     statement.  */
  if (method
      && (strcmp (DECL_SOURCE_FILE (fndecl), DECL_SOURCE_FILE (method)) != 0
	  || DECL_SOURCE_LINE (fndecl) != DECL_SOURCE_LINE (method)))
    DECL_SOURCE_LOCATION (fndecl) = DECL_SOURCE_LOCATION (method);
  /* APPLE LOCAL end radar 5839812 - location for synthesized methods  */
  nstack_se = XOBNEW (&parser_obstack, struct c_label_context_se);
  nstack_se->labels_def = NULL;
  nstack_se->labels_used = NULL;
  nstack_se->next = label_context_stack_se;
  label_context_stack_se = nstack_se;
  nstack_vm = XOBNEW (&parser_obstack, struct c_label_context_vm);
  nstack_vm->labels_def = NULL;
  nstack_vm->labels_used = NULL;
  nstack_vm->scope = 0;
  nstack_vm->next = label_context_stack_vm;
  label_context_stack_vm = nstack_vm;
  current_function_returns_value = 0;  /* Assume, until we see it does.  */
  current_function_returns_null = 0;

  decl_attributes (&fndecl, attrs, 0);
  announce_function (fndecl);
  DECL_INITIAL (fndecl) = error_mark_node;
  DECL_EXTERNAL (fndecl) = 0;
  TREE_STATIC (fndecl) = 1;
  current_function_decl = pushdecl (fndecl);
  /* APPLE LOCAL begin optimization pragmas 3124235/3420242 */
  /* Build a mapping between this decl and the per-function options in
     effect at this point.  */
  record_func_cl_pf_opts_mapping (fndecl);
  /* APPLE LOCAL end optimization pragmas 3124235/3420242 */
  push_scope ();
  declare_parm_level ();
  DECL_RESULT (current_function_decl)
    = build_decl (RESULT_DECL, NULL_TREE,
		  TREE_TYPE (TREE_TYPE (current_function_decl)));
  DECL_ARTIFICIAL (DECL_RESULT (current_function_decl)) = 1;
  DECL_IGNORED_P (DECL_RESULT (current_function_decl)) = 1;
  start_fname_decls ();
  store_parm_decls_from (params);
#endif

  TREE_USED (current_function_decl) = 1;
}

/* - Generate an identifier for the function. the format is "_n_cls",
     where 1 <= n <= nMethods, and cls is the name the implementation we
     are processing.
   - Install the return type from the method declaration.
   - If we have a prototype, check for type consistency.  */

static void
really_start_method (tree method,
#ifdef OBJCPLUS
		     tree parmlist
#else
		     struct c_arg_info *parmlist
#endif
		     )
{
  tree ret_type, meth_type;
  tree method_id;
  const char *sel_name, *class_name, *cat_name;
  char *buf;

  /* Synth the storage class & assemble the return type.  */
  ret_type = TREE_VALUE (TREE_TYPE (method));

  sel_name = IDENTIFIER_POINTER (METHOD_SEL_NAME (method));
  class_name = IDENTIFIER_POINTER (CLASS_NAME (objc_implementation_context));
  cat_name = ((TREE_CODE (objc_implementation_context)
	       == CLASS_IMPLEMENTATION_TYPE)
	      ? NULL
	      : IDENTIFIER_POINTER (CLASS_SUPER_NAME (objc_implementation_context)));
  method_slot++;

  /* Make sure this is big enough for any plausible method label.  */
  buf = (char *) alloca (50 + strlen (sel_name) + strlen (class_name)
			 + (cat_name ? strlen (cat_name) : 0));

  OBJC_GEN_METHOD_LABEL (buf, TREE_CODE (method) == INSTANCE_METHOD_DECL,
			 class_name, cat_name, sel_name, method_slot);

  method_id = get_identifier (buf);

#ifdef OBJCPLUS
  /* Objective-C methods cannot be overloaded, so we don't need
     the type encoding appended.  It looks bad anyway... */
  push_lang_context (lang_name_c);
#endif

  meth_type
    = build_function_type (ret_type,
		    	   /* APPLE LOCAL radar 6307941 */
			   get_arg_type_list (METHOD_SEL_NAME (method), method, METHOD_DEF, 0));
  /* APPLE LOCAL radar 5839812 - location for synthesized methods  */
  objc_start_function (method_id, meth_type, NULL_TREE, parmlist, method);
  /* LLVM LOCAL begin prevent llvm from adding leading _ */
#ifdef ENABLE_LLVM
  set_user_assembler_name(current_function_decl, buf);
#endif
  /* LLVM LOCAL end prevent llvm from adding leading _ */

  /* Set self_decl from the first argument.  */
  self_decl = DECL_ARGUMENTS (current_function_decl);

  /* Suppress unused warnings.  */
  TREE_USED (self_decl) = 1;
  TREE_USED (TREE_CHAIN (self_decl)) = 1;
#ifdef OBJCPLUS
  pop_lang_context ();
#endif

  METHOD_DEFINITION (method) = current_function_decl;
  /* APPLE LOCAL deprecated use in deprecated 6425499 */
  TREE_DEPRECATED (current_function_decl) = TREE_DEPRECATED (method);

  /* Check consistency...start_function, pushdecl, duplicate_decls.  */

  if (implementation_template != objc_implementation_context)
    {
      /* APPLE LOCAL begin radar 5370783 */
      tree proto
	= lookup_method_static (implementation_template,
				METHOD_SEL_NAME (method),
				(TREE_CODE (method) == CLASS_METHOD_DECL), 
				false, false);
      /* APPLE LOCAL end radar 5370783 */

      if (proto)
	{
	  /* APPLE LOCAL radar 4606233 */
	  /* code removed */
	  if (!comp_proto_with_proto (method, proto, 1))
	    {
	      char type = (TREE_CODE (method) == INSTANCE_METHOD_DECL ? '-' : '+');

	      warn_with_method ("conflicting types for", type, method);
	      warn_with_method ("previous declaration of", type, proto);
	    }
	  /* APPLE LOCAL begin radar 4727659 */
	  /* Move __attribute__ ((noreturn)) to DECL_FUNCTION definition node.
	     so warning may come out */
	  if (METHOD_DEFINITION (method))
            TREE_THIS_VOLATILE (METHOD_DEFINITION (method)) 
	      = TREE_THIS_VOLATILE (proto);
	  /* APPLE LOCAL end radar 4727659 */
	}
      else
	{
	  /* We have a method @implementation even though we did not
	     see a corresponding @interface declaration (which is allowed
	     by Objective-C rules).  Go ahead and place the method in
	     the @interface anyway, so that message dispatch lookups
	     will see it.  */
	  tree interface = implementation_template;

	  if (TREE_CODE (objc_implementation_context)
	      == CATEGORY_IMPLEMENTATION_TYPE)
	    interface = lookup_category
			(interface,
			 CLASS_SUPER_NAME (objc_implementation_context));

	  if (interface)
	    objc_add_method (interface, copy_node (method),
			     /* APPLE LOCAL C* language */
			     TREE_CODE (method) == CLASS_METHOD_DECL, 0);
	}
    }
}

static void *UOBJC_SUPER_scope = 0;

/* _n_Method (id self, SEL sel, ...)
     {
       struct objc_super _S;
       _msgSuper ((_S.self = self, _S.class = _cls, &_S), ...);
     }  */

static tree
get_super_receiver (void)
{
  if (objc_method_context)
    {
      tree super_expr, super_expr_list;

      if (!UOBJC_SUPER_decl)
      {
	UOBJC_SUPER_decl = build_decl (VAR_DECL, get_identifier (TAG_SUPER),
				       objc_super_template);
	/* This prevents `unused variable' warnings when compiling with -Wall.  */
	TREE_USED (UOBJC_SUPER_decl) = 1;
	lang_hooks.decls.pushdecl (UOBJC_SUPER_decl);
        finish_decl (UOBJC_SUPER_decl, NULL_TREE, NULL_TREE);
	UOBJC_SUPER_scope = objc_get_current_scope ();
      }

      /* Set receiver to self.  */
      super_expr = objc_build_component_ref (UOBJC_SUPER_decl, self_id);
      /* APPLE LOCAL begin radar 6230701 */
      if (cur_block)
      {
        tree local_self_decl = self_decl;
        access_block_ref_self_decl (&local_self_decl);
        super_expr = build_modify_expr (super_expr, NOP_EXPR, local_self_decl);
      }
      else
        super_expr = build_modify_expr (super_expr, NOP_EXPR, self_decl);
       /* APPLE LOCAL end radar 6230701 */
      super_expr_list = super_expr;

      /* Set class to begin searching.  */
      super_expr = objc_build_component_ref (UOBJC_SUPER_decl,
      /* APPLE LOCAL ObjC abi v2 */
					     get_identifier ("cls"));

      if (TREE_CODE (objc_implementation_context) == CLASS_IMPLEMENTATION_TYPE)
	{
          /* APPLE LOCAL begin ObjC abi v2 */
          if (flag_objc_abi == 2)
            {
              /* For new ObjC abi, "cls" is loaded from a __class_refs table entry. */
	      /* For super call in class method, must have super_t.cls point to its
		 metaclass. */
              tree class_expr;
	      /* APPLE LOCAL begin radar 4535676 */
	      class_expr = objc_v2_get_class_super_reference (
			     CLASS_NAME (objc_implementation_context),
			     (TREE_CODE (objc_method_context) == INSTANCE_METHOD_DECL));
	      /* APPLE LOCAL end radar 4535676 */
              super_expr = build_modify_expr (super_expr, NOP_EXPR, class_expr);
            }
          else
            {
              /* [_cls, __cls]Super are "pre-built" in
                 synth_forward_declarations.  */

              super_expr = build_modify_expr (super_expr, NOP_EXPR,
                                              ((TREE_CODE (objc_method_context)
                                                == INSTANCE_METHOD_DECL)
                                               ? ucls_super_ref
                                               : uucls_super_ref));
            }
          /* APPLE LOCAL end ObjC abi v2 */
	}

      else
	/* We have a category.  */
	{
	  tree super_name = CLASS_SUPER_NAME (implementation_template);
	  tree super_class;

	  /* Barf if super used in a category of Object.  */
	  if (!super_name)
	    {
	      error ("no super class declared in interface for %qs",
		    IDENTIFIER_POINTER (CLASS_NAME (implementation_template)));
	      return error_mark_node;
	    }

	  if (flag_next_runtime && !flag_zero_link)
	    {
              /* APPLE LOCAL begin ObjC abi v2 */
              super_class = objc_get_class_reference ((flag_objc_abi == 2)
                                                      ? CLASS_NAME (implementation_template)
                                                      : super_name);
              /* APPLE LOCAL end ObjC abi v2 */
	      if (TREE_CODE (objc_method_context) == CLASS_METHOD_DECL)
		/* If we are in a class method, we must retrieve the
		   _metaclass_ for the current class, pointed at by
		   the class's "isa" pointer.  The following assumes that
		   "isa" is the first ivar in a class (which it must be).  */
		super_class
		  = build_indirect_ref
		    (build_c_cast (build_pointer_type (objc_class_type),
				   super_class), "unary *");
	    }
	  else
	    {
	      add_class_reference (super_name);
	      super_class = (TREE_CODE (objc_method_context) == INSTANCE_METHOD_DECL
			     ? objc_get_class_decl : objc_get_meta_class_decl);
	      assemble_external (super_class);
	      super_class
		= build_function_call
		  (super_class,
		   build_tree_list
		   (NULL_TREE,
		    my_build_string_pointer
		    (IDENTIFIER_LENGTH (super_name) + 1,
		     IDENTIFIER_POINTER (super_name))));
	    }

	  super_expr
	    = build_modify_expr (super_expr, NOP_EXPR,
				 build_c_cast (TREE_TYPE (super_expr),
					       super_class));
	}

      super_expr_list = build_compound_expr (super_expr_list, super_expr);

      super_expr = build_unary_op (ADDR_EXPR, UOBJC_SUPER_decl, 0);
      super_expr_list = build_compound_expr (super_expr_list, super_expr);

      return super_expr_list;
    }
  else
    {
      error ("[super ...] must appear in a method context");
      return error_mark_node;
    }
}

/* When exiting a scope, sever links to a 'super' declaration (if any)
   therein contained.  */

void
objc_clear_super_receiver (void)
{
  if (objc_method_context
      && UOBJC_SUPER_scope == objc_get_current_scope ()) {
    UOBJC_SUPER_decl = 0;
    UOBJC_SUPER_scope = 0;
  }
}

void
objc_finish_method_definition (tree fndecl)
{
  /* We cannot validly inline ObjC methods, at least not without a language
     extension to declare that a method need not be dynamically
     dispatched, so suppress all thoughts of doing so.  */
  DECL_INLINE (fndecl) = 0;
  DECL_UNINLINABLE (fndecl) = 1;

#ifndef OBJCPLUS
  /* The C++ front-end will have called finish_function() for us.  */
  finish_function ();
#endif

  METHOD_ENCODING (objc_method_context)
    = encode_method_prototype (objc_method_context);

  /* Required to implement _msgSuper. This must be done AFTER finish_function,
     since the optimizer may find "may be used before set" errors.  */
  objc_method_context = NULL_TREE;

  if (should_call_super_dealloc)
    warning (0, "method possibly missing a [super dealloc] call");
  /* APPLE LOCAL begin radar 4757423 */
  if (should_call_super_finalize)
    warning (0, "method possibly missing a [super finalize] call");
  /* APPLE LOCAL end radar 4757423 */
}

#if 0
int
lang_report_error_function (tree decl)
{
  if (objc_method_context)
    {
      fprintf (stderr, "In method %qs\n",
	       IDENTIFIER_POINTER (METHOD_SEL_NAME (objc_method_context)));
      return 1;
    }

  else
    return 0;
}
#endif

/* Given a tree DECL node, produce a printable description of it in the given
   buffer, overwriting the buffer.  */

static char *
gen_declaration (tree decl)
{
  errbuf[0] = '\0';

  if (DECL_P (decl))
    {
      gen_type_name_0 (TREE_TYPE (decl));

      if (DECL_NAME (decl))
	{
	  if (!POINTER_TYPE_P (TREE_TYPE (decl)))
	    strcat (errbuf, " ");

	  strcat (errbuf, IDENTIFIER_POINTER (DECL_NAME (decl)));
	}

      if (DECL_INITIAL (decl)
	  && TREE_CODE (DECL_INITIAL (decl)) == INTEGER_CST)
	sprintf (errbuf + strlen (errbuf), ": " HOST_WIDE_INT_PRINT_DEC,
		 TREE_INT_CST_LOW (DECL_INITIAL (decl)));
    }

  return errbuf;
}

/* Given a tree TYPE node, produce a printable description of it in the given
   buffer, overwriting the buffer.  */

static char *
gen_type_name_0 (tree type)
{
  tree orig = type, proto;

  if (TYPE_P (type) && TYPE_NAME (type))
    type = TYPE_NAME (type);
  else if (POINTER_TYPE_P (type) || TREE_CODE (type) == ARRAY_TYPE)
    {
      tree inner = TREE_TYPE (type);

      while (TREE_CODE (inner) == ARRAY_TYPE)
	inner = TREE_TYPE (inner);

      gen_type_name_0 (inner);

      if (!POINTER_TYPE_P (inner))
	strcat (errbuf, " ");

      if (POINTER_TYPE_P (type))
	strcat (errbuf, "*");
      else
	while (type != inner)
	  {
	    strcat (errbuf, "[");

	    if (TYPE_DOMAIN (type))
	      {
		char sz[20];

		sprintf (sz, HOST_WIDE_INT_PRINT_DEC,
			 (TREE_INT_CST_LOW
			  (TYPE_MAX_VALUE (TYPE_DOMAIN (type))) + 1));
		strcat (errbuf, sz);
	      }

	    strcat (errbuf, "]");
	    type = TREE_TYPE (type);
	  }

      goto exit_function;
    }

  if (TREE_CODE (type) == TYPE_DECL && DECL_NAME (type))
    type = DECL_NAME (type);

  strcat (errbuf, TREE_CODE (type) == IDENTIFIER_NODE
	  	  ? IDENTIFIER_POINTER (type)
		  : "");

  /* For 'id' and 'Class', adopted protocols are stored in the pointee.  */
  if (objc_is_id (orig))
    orig = TREE_TYPE (orig);

  proto = TYPE_HAS_OBJC_INFO (orig) ? TYPE_OBJC_PROTOCOL_LIST (orig) : NULL_TREE;

  if (proto)
    {
      strcat (errbuf, " <");

      while (proto) {
	strcat (errbuf,
		IDENTIFIER_POINTER (PROTOCOL_NAME (TREE_VALUE (proto))));
	proto = TREE_CHAIN (proto);
	strcat (errbuf, proto ? ", " : ">");
      }
    }

 exit_function:
  return errbuf;
}

static char *
gen_type_name (tree type)
{
  errbuf[0] = '\0';

  return gen_type_name_0 (type);
}

/* Given a method tree, put a printable description into the given
   buffer (overwriting) and return a pointer to the buffer.  */

static char *
gen_method_decl (tree method)
{
  tree chain;

  strcpy (errbuf, "(");  /* NB: Do _not_ call strcat() here.  */
  gen_type_name_0 (TREE_VALUE (TREE_TYPE (method)));
  strcat (errbuf, ")");
  chain = METHOD_SEL_ARGS (method);

  if (chain)
    {
      /* We have a chain of keyword_decls.  */
      do
        {
	  if (KEYWORD_KEY_NAME (chain))
	    strcat (errbuf, IDENTIFIER_POINTER (KEYWORD_KEY_NAME (chain)));

	  strcat (errbuf, ":(");
	  gen_type_name_0 (TREE_VALUE (TREE_TYPE (chain)));
	  strcat (errbuf, ")");

	  strcat (errbuf, IDENTIFIER_POINTER (KEYWORD_ARG_NAME (chain)));
	  if ((chain = TREE_CHAIN (chain)))
	    strcat (errbuf, " ");
        }
      while (chain);

      if (METHOD_ADD_ARGS (method))
	{
	  chain = TREE_CHAIN (METHOD_ADD_ARGS (method));

	  /* Know we have a chain of parm_decls.  */
	  while (chain)
	    {
	      strcat (errbuf, ", ");
	      gen_type_name_0 (TREE_TYPE (TREE_VALUE (chain)));
	      chain = TREE_CHAIN (chain);
	    }

	  if (METHOD_ADD_ARGS_ELLIPSIS_P (method))
	    strcat (errbuf, ", ...");
	}
    }

  else
    /* We have a unary selector.  */
    strcat (errbuf, IDENTIFIER_POINTER (METHOD_SEL_NAME (method)));

  return errbuf;
}

/* Debug info.  */


/* Dump an @interface declaration of the supplied class CHAIN to the
   supplied file FP.  Used to implement the -gen-decls option (which
   prints out an @interface declaration of all classes compiled in
   this run); potentially useful for debugging the compiler too.  */
static void
dump_interface (FILE *fp, tree chain)
{
  /* FIXME: A heap overflow here whenever a method (or ivar)
     declaration is so long that it doesn't fit in the buffer.  The
     code and all the related functions should be rewritten to avoid
     using fixed size buffers.  */
  const char *my_name = IDENTIFIER_POINTER (CLASS_NAME (chain));
  tree ivar_decls = CLASS_RAW_IVARS (chain);
  tree nst_methods = CLASS_NST_METHODS (chain);
  tree cls_methods = CLASS_CLS_METHODS (chain);

  fprintf (fp, "\n@interface %s", my_name);

  /* CLASS_SUPER_NAME is used to store the superclass name for
     classes, and the category name for categories.  */
  if (CLASS_SUPER_NAME (chain))
    {
      const char *name = IDENTIFIER_POINTER (CLASS_SUPER_NAME (chain));

      if (TREE_CODE (chain) == CATEGORY_IMPLEMENTATION_TYPE
	  || TREE_CODE (chain) == CATEGORY_INTERFACE_TYPE)
	{
	  fprintf (fp, " (%s)\n", name);
	}
      else
	{
	  fprintf (fp, " : %s\n", name);
	}
    }
  else
    fprintf (fp, "\n");

  /* FIXME - the following doesn't seem to work at the moment.  */
  if (ivar_decls)
    {
      fprintf (fp, "{\n");
      do
	{
	  fprintf (fp, "\t%s;\n", gen_declaration (ivar_decls));
	  ivar_decls = TREE_CHAIN (ivar_decls);
	}
      while (ivar_decls);
      fprintf (fp, "}\n");
    }

  while (nst_methods)
    {
      fprintf (fp, "- %s;\n", gen_method_decl (nst_methods));
      nst_methods = TREE_CHAIN (nst_methods);
    }

  while (cls_methods)
    {
      fprintf (fp, "+ %s;\n", gen_method_decl (cls_methods));
      cls_methods = TREE_CHAIN (cls_methods);
    }

  fprintf (fp, "@end\n");
}

/* Demangle function for Objective-C */
static const char *
/* APPLE LOCAL radar 4638467,4734562 */
objc_demangle (tree decl, int vrb)
{
  char *demangled, *cp;
  /* APPLE LOCAL radar 4638467 */
  const char *mangled = IDENTIFIER_POINTER (DECL_NAME (decl));

  if (mangled[0] == '_' &&
      (mangled[1] == 'i' || mangled[1] == 'c') &&
      mangled[2] == '_')
    {
      cp = demangled = XNEWVEC (char, strlen(mangled) + 2);
      if (mangled[1] == 'i')
	*cp++ = '-';            /* for instance method */
      else
	*cp++ = '+';            /* for class method */
      *cp++ = '[';              /* opening left brace */
      strcpy(cp, mangled+3);    /* tack on the rest of the mangled name */
      while (*cp && *cp == '_')
	cp++;                   /* skip any initial underbars in class name */
      cp = strchr(cp, '_');     /* find first non-initial underbar */
      if (cp == NULL)
	{
	  free(demangled);      /* not mangled name */
	  return mangled;
	}
      if (cp[1] == '_')  /* easy case: no category name */
	{
	  *cp++ = ' ';            /* replace two '_' with one ' ' */
	  strcpy(cp, mangled + (cp - demangled) + 2);
	}
      else
	{
	  *cp++ = '(';            /* less easy case: category name */
	  cp = strchr(cp, '_');
	  if (cp == 0)
	    {
	      free(demangled);    /* not mangled name */
	      return mangled;
	    }
	  *cp++ = ')';
	  *cp++ = ' ';            /* overwriting 1st char of method name... */
	  strcpy(cp, mangled + (cp - demangled)); /* get it back */
	}
      while (*cp && *cp == '_')
	cp++;                   /* skip any initial underbars in method name */
      for (; *cp; cp++)
	if (*cp == '_')
	  *cp = ':';            /* replace remaining '_' with ':' */
      *cp++ = ']';              /* closing right brace */
      *cp++ = 0;                /* string terminator */
      return demangled;
    }
  else
/* APPLE LOCAL begin radar 4638467,4734562, 3904139 */
#ifdef OBJCPLUS
    return cxx_printable_name (decl, vrb);
#else
    return vrb == 2 ? mangled : mangled; /* not an objc mangled name */
#endif
/* APPLE LOCAL end radar 4638467,4734562, 3904139 */
}

const char *
/* APPLE LOCAL radar 4734562 */
objc_printable_name (tree decl, int vrb)
{
  /* APPLE LOCAL radar 4638467,4734562 */
  return objc_demangle (decl, vrb);
}

static void
init_objc (void)
{
  gcc_obstack_init (&util_obstack);
  util_firstobj = (char *) obstack_finish (&util_obstack);

  errbuf = XNEWVEC (char, 1024 * 10);
  hash_init ();
  synth_module_prologue ();
}

static void
finish_objc (void)
{
  struct imp_entry *impent;
  tree chain;
  /* The internally generated initializers appear to have missing braces.
     Don't warn about this.  */
  int save_warn_missing_braces = warn_missing_braces;
  warn_missing_braces = 0;

  /* A missing @end may not be detected by the parser.  */
  if (objc_implementation_context)
    {
      warning (0, "%<@end%> missing in implementation context");
      finish_class (objc_implementation_context);
      objc_ivar_chain = NULL_TREE;
      objc_implementation_context = NULL_TREE;
    }

  /* Process the static instances here because initialization of objc_symtab
     depends on them.  */
  if (objc_static_instances)
    generate_static_references ();

  if (imp_list || class_names_chain
      /* APPLE LOCAL C* property metadata (Radar 4498373) */
      || prop_names_attr_chain
      || meth_var_names_chain || meth_var_types_chain || sel_ref_chain)
    generate_objc_symtab_decl ();

  for (impent = imp_list; impent; impent = impent->next)
    {
      objc_implementation_context = impent->imp_context;
      implementation_template = impent->imp_template;

      /* APPLE LOCAL begin ObjC new abi */
      if (flag_objc_abi == 2)
	{
      	  UOBJC_V2_CLASS_decl = impent->class_v2_decl;
      	  UOBJC_V2_METACLASS_decl = impent->meta_v2_decl;
	}
      else
	{
          UOBJC_CLASS_decl = impent->class_decl;
          UOBJC_METACLASS_decl = impent->meta_decl;
	}
      /* APPLE LOCAL end ObjC new abi */

      /* Dump the @interface of each class as we compile it, if the
	 -gen-decls option is in use.  TODO: Dump the classes in the
         order they were found, rather than in reverse order as we
         are doing now.  */
      if (flag_gen_declaration)
	{
	  dump_interface (gen_declaration_file, objc_implementation_context);
	}

      if (TREE_CODE (objc_implementation_context) == CLASS_IMPLEMENTATION_TYPE)
	{
	  /* all of the following reference the string pool...  */

	  /* APPLE LOCAL begin ObjC abi v2 */
	  if (flag_objc_abi == 2)
	    {
	      generate_v2_ivar_lists ();
	      generate_v2_dispatch_tables ();
	      /* APPLE LOCAL C* property metadata (Radar 4498373, 4559114) */
	      generate_v2_property_tables (NULL_TREE);
	      generate_v2_shared_structures (impent->has_cxx_cdtors
					     /* APPLE LOCAL radar 4923634 */
					     ? OBJC2_CLS_HAS_CXX_STRUCTORS
					     : 0);
	    }
          else
            {
              generate_ivar_lists ();
              generate_dispatch_tables ();
	      /* APPLE LOCAL begin radar 4585769 - Objective-C 1.0 extensions */
	      generate_v2_property_tables (NULL_TREE);
	      UOBJC_CLASS_EXT_decl = generate_objc_class_ext (UOBJC_V2_PROPERTY_decl);
	      /* APPLE LOCAL end radar 4585769 - Objective-C 1.0 extensions */	      

              generate_shared_structures (impent->has_cxx_cdtors
                                          ? CLS_HAS_CXX_STRUCTORS
                                          : 0);
            }
	}
          /* APPLE LOCAL end ObjC abi v2 */
      /* APPLE LOCAL radar 4695109 */
      /* PROTOCOL_IMPLEMENTATION_TYPE removed */
      else
	{
	  /* APPLE LOCAL begin ObjC abi v2 */
	  if (flag_objc_abi == 2)
	    {
	      generate_v2_dispatch_tables ();
	      /* APPLE LOCAL C* property metadata (Radar 4498373, 4559114) */
	      generate_v2_property_tables (NULL_TREE);
	      generate_v2_category (objc_implementation_context, impent);
	    }
	  else
	    {
	      generate_dispatch_tables ();
	      /* APPLE LOCAL radar 4585769 - Objective-C 1.0 extensions */
	      generate_v2_property_tables (NULL_TREE);
	      /* APPLE LOCAL radar 4349690 */
	      generate_category (objc_implementation_context, impent);
	    }
	  /* APPLE LOCAL end ObjC abi v2 */
	}
    }

  /* If we are using an array of selectors, we must always
     finish up the array decl even if no selectors were used.  */
  if (! flag_next_runtime || sel_ref_chain)
    build_selector_translation_table ();

  /* APPLE LOCAL begin ObjC abi v2 */
  if (message_ref_chain)
    build_message_ref_translation_table ();
  if (classlist_ref_chain)
    build_classlist_translation_table (false);
  if (metaclasslist_ref_chain)
    build_classlist_translation_table (true);
  /* APPLE LOCAL begin radar 4535676 */
  if (classlist_super_ref_chain)
    build_classlist_super_translation_table (false);
  if (metaclasslist_super_ref_chain)
    build_classlist_super_translation_table (true);
  /* APPLE LOCAL end radar 4535676 */
  if (ivar_offset_ref_chain)
    generate_v2_ivar_offset_ref_lists ();
  if (protocol_chain)
    {
      if (flag_objc_abi == 2)
	{
          generate_v2_protocols ();
	}
      else
        generate_protocols ();
    }
  /* APPLE LOCAL begin radar 4533974 - ObjC new protocol */
  if (protocol_list_chain)
    build_protocol_list_address_table ();
  if (protocollist_ref_chain)
    build_protocollist_translation_table ();
  /* APPLE LOCAL end radar 4533974 - ObjC new protocol */
  if (class_list_chain)
    build_class_list_address_table (false);
  if (category_list_chain)
    build_category_list_address_table (false);
  if (nonlazy_class_list_chain)
    build_class_list_address_table (true);
  if (nonlazy_category_list_chain)
    build_category_list_address_table (true);
  /* APPLE LOCAL end ObjC abi v2 */

  /* APPLE LOCAL radar 4810587 */
  generate_objc_image_info ();

  /* Arrange for ObjC data structures to be initialized at run time.  */
  if (objc_implementation_context || class_names_chain || objc_static_instances
      || meth_var_names_chain || meth_var_types_chain || sel_ref_chain)
    {
      /* APPLE LOCAL begin ObjC abi v2 */
      /* APPLE LOCAL radar 4531482 */
      if (flag_objc_abi != 2)
        build_module_descriptor ();
      /* APPLE LOCAL end ObjC abi v2 */

      if (!flag_next_runtime)
	build_module_initializer_routine ();
    }

  /* Dump the class references.  This forces the appropriate classes
     to be linked into the executable image, preserving unix archive
     semantics.  This can be removed when we move to a more dynamically
     linked environment.  */

  for (chain = cls_ref_chain; chain; chain = TREE_CHAIN (chain))
    {
      handle_class_ref (chain);
      /* LLVM LOCAL begin - radar 5676233 */
      if (TREE_PURPOSE (chain)) {
	generate_classref_translation_entry (chain);
#ifdef ENABLE_LLVM
        /* Reset the initializer for this reference as it most likely
           changed.  */
        reset_initializer_llvm(TREE_PURPOSE (chain));
#endif
      }
      /* LLVM LOCAL end - radar 5676233 */
    }

  for (impent = imp_list; impent; impent = impent->next)
    handle_impent (impent);

  if (warn_selector)
    {
      int slot;
      hash hsh;

      /* Run through the selector hash tables and print a warning for any
         selector which has multiple methods.  */

      for (slot = 0; slot < SIZEHASHTABLE; slot++)
	{
	  for (hsh = cls_method_hash_list[slot]; hsh; hsh = hsh->next)
	    check_duplicates (hsh, 0, 1);
	  for (hsh = nst_method_hash_list[slot]; hsh; hsh = hsh->next)
	    check_duplicates (hsh, 0, 1);
	}
    }

  warn_missing_braces = save_warn_missing_braces;
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  {
    int i;
    for (i = 0; i < OCTI_MAX; i++)
      if (objc_global_trees[i] && DECL_P (objc_global_trees[i]))
        /* Let optimizer know that this decl is not removable.  */
        DECL_PRESERVE_P (objc_global_trees[i]) = 1;
  }
#endif
  /* LLVM LOCAL end */
}

/* Subroutines of finish_objc.  */

static void
generate_classref_translation_entry (tree chain)
{
  tree expr, decl, type;

  decl = TREE_PURPOSE (chain);
  type = TREE_TYPE (decl);

  expr = add_objc_string (TREE_VALUE (chain), class_names);
  expr = convert (type, expr); /* cast! */

  /* The decl that is the one that we
     forward declared in build_class_reference.  */
  finish_var_decl (decl, expr);
  return;
}

static void
handle_class_ref (tree chain)
{
  const char *name = IDENTIFIER_POINTER (TREE_VALUE (chain));
  char *string = (char *) alloca (strlen (name) + 30);
  tree decl;
  tree exp;

  sprintf (string, "%sobjc_class_name_%s",
	   (flag_next_runtime ? "." : "__"), name);

#ifdef ASM_DECLARE_UNRESOLVED_REFERENCE
  if (flag_next_runtime)
    {
      ASM_DECLARE_UNRESOLVED_REFERENCE (asm_out_file, string);
      return;
    }
#endif

  /* Make a decl for this name, so we can use its address in a tree.  */
  decl = build_decl (VAR_DECL, get_identifier (string), char_type_node);
  DECL_EXTERNAL (decl) = 1;
  TREE_PUBLIC (decl) = 1;
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  /* This decl's name is special. Ask llvm to not add leading underscore by 
     setting it as a user supplied asm name.  */
  set_user_assembler_name(decl, string);
  /* Let optimizer know that this decl is not removable.  */
  DECL_PRESERVE_P (decl) = 1;
#endif
  /* LLVM LOCAL end */
  pushdecl (decl);
  rest_of_decl_compilation (decl, 0, 0);

  /* Make a decl for the address.  */
  sprintf (string, "%sobjc_class_ref_%s",
	   (flag_next_runtime ? "." : "__"), name);
  exp = build1 (ADDR_EXPR, string_type_node, decl);
  decl = build_decl (VAR_DECL, get_identifier (string), string_type_node);
  DECL_INITIAL (decl) = exp;
  TREE_STATIC (decl) = 1;
  TREE_USED (decl) = 1;
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  /* This decl's name is special. Ask llvm to not add leading underscore by 
     setting it as a user supplied asm name.  */
  set_user_assembler_name(decl, string);
#endif
  /* LLVM LOCAL end */
  /* Force the output of the decl as this forces the reference of the class.  */
  mark_decl_referenced (decl);

  pushdecl (decl);
  rest_of_decl_compilation (decl, 0, 0);
}

static void
handle_impent (struct imp_entry *impent)
{
  char *string;

  objc_implementation_context = impent->imp_context;
  implementation_template = impent->imp_template;

  if (TREE_CODE (impent->imp_context) == CLASS_IMPLEMENTATION_TYPE)
    {
      const char *const class_name =
	IDENTIFIER_POINTER (CLASS_NAME (impent->imp_context));

      string = (char *) alloca (strlen (class_name) + 30);

      sprintf (string, "%sobjc_class_name_%s",
               (flag_next_runtime ? "." : "__"), class_name);
    }
  /* APPLE LOCAL radar 5774213 */
  else if (flag_objc_abi <= 1 && TREE_CODE (impent->imp_context) == CATEGORY_IMPLEMENTATION_TYPE)
    {
      const char *const class_name =
	IDENTIFIER_POINTER (CLASS_NAME (impent->imp_context));
      const char *const class_super_name =
        IDENTIFIER_POINTER (CLASS_SUPER_NAME (impent->imp_context));

      string = (char *) alloca (strlen (class_name)
				+ strlen (class_super_name) + 30);

      /* Do the same for categories.  Even though no references to
         these symbols are generated automatically by the compiler, it
         gives you a handle to pull them into an archive by hand.  */
      /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
      /* The * is a sentinel for gcc's back end, but is not wanted by llvm. */
      sprintf (string, "%sobjc_category_name_%s_%s",
               (flag_next_runtime ? "." : "__"), class_name, class_super_name);
#else
      sprintf (string, "*%sobjc_category_name_%s_%s",
               (flag_next_runtime ? "." : "__"), class_name, class_super_name);
#endif
      /* LLVM LOCAL end */
    }
  else
    return;

#ifdef ASM_DECLARE_CLASS_REFERENCE
  if (flag_next_runtime)
    {
      /* LLVM LOCAL begin - radar 5702446 */
#ifdef ENABLE_LLVM
      if (flag_objc_abi != 2)
#endif
      /* LLVM LOCAL end - radar 5702446 */
      ASM_DECLARE_CLASS_REFERENCE (asm_out_file, string);
      return;
    }
  else
#endif
    {
      tree decl, init;

      init = build_int_cst (c_common_type_for_size (BITS_PER_WORD, 1), 0);
      decl = build_decl (VAR_DECL, get_identifier (string), TREE_TYPE (init));
      TREE_PUBLIC (decl) = 1;
      TREE_READONLY (decl) = 1;
      TREE_USED (decl) = 1;
      TREE_CONSTANT (decl) = 1;
      DECL_CONTEXT (decl) = 0;
      DECL_ARTIFICIAL (decl) = 1;
      /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
      DECL_VISIBILITY (decl) = VISIBILITY_DEFAULT;
      DECL_VISIBILITY_SPECIFIED (decl) = 1;
      set_user_assembler_name(decl, string);
      /* Let optimizer know that this decl is not removable.  */
      DECL_PRESERVE_P (decl) = 1;
#endif
      /* LLVM LOCAL end */
      DECL_INITIAL (decl) = init;
      assemble_variable (decl, 1, 0, 0);
    }
}

/* The Fix-and-Continue functionality available in Mac OS X 10.3 and
   later requires that ObjC translation units participating in F&C be
   specially marked.  The following routine accomplishes this.  */

/* static int _OBJC_IMAGE_INFO[2] = { 0, 1 }; */

static void
generate_objc_image_info (void)
{
  tree decl, initlist;
  int flags
    = ((flag_replace_objc_classes && imp_list ? 1 : 0)
       | (flag_objc_gc ? 2 : 0));
  /* APPLE LOCAL begin radar 4810609 */
  if (flag_objc_gc_only)
    flags |= 6;
  /* APPLE LOCAL end radar 4810609 */
  /* APPLE LOCAL begin radar 6803242 */
  if (flag_objc_abi == 2)
    flags |= 16;
  /* APPLE LOCAL end radar 6803242 */

  /* APPLE LOCAL begin radar 4810587 */
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  /* Darwin linker prefers to use 'L' as a prefix. GCC codegen handles this
     later while emitting symbols, but fix it here for llvm.  */
  decl = build_decl (VAR_DECL, get_identifier ("L_OBJC_IMAGE_INFO"),
                     build_array_type
                     (integer_type_node,
                      build_index_type (build_int_cst (NULL_TREE, 2 - 1))));
#else
  /* LLVM LOCAL end */
  decl = build_decl (VAR_DECL, get_identifier ("_OBJC_IMAGE_INFO"), 
		     build_array_type
		       (integer_type_node,
		  	build_index_type (build_int_cst (NULL_TREE, 2 - 1))));
   /* LLVM LOCAL */
#endif
  initlist = build_tree_list (NULL_TREE, integer_zero_node);
  initlist = tree_cons (NULL_TREE, build_int_cst (NULL_TREE, flags), initlist);
  initlist = objc_build_constructor (TREE_TYPE (decl), nreverse (initlist));
  TREE_TYPE (initlist) = TREE_TYPE (decl);
  /* APPLE LOCAL begin radar 4888238 */
  TREE_PUBLIC (decl) = 0;
  TREE_STATIC (decl) = 1;
  /* APPLE LOCAL end radar 4888238 */
  TREE_READONLY (decl) = 1;
  TREE_USED (decl) = 1;
  TREE_CONSTANT (decl) = 1;
  DECL_CONTEXT (decl) = 0;
  DECL_ARTIFICIAL (decl) = 1;
  DECL_INITIAL (decl) = initlist;
  /* LLVM LOCAL begin */
#ifdef ENABLE_LLVM
  /* Let optimizer know that this decl is not removable.  */
  set_user_assembler_name(decl, IDENTIFIER_POINTER (DECL_NAME(decl)));
  DECL_PRESERVE_P (decl) = 1;
#endif
  /* LLVM LOCAL end */
  assemble_variable (decl, 1, 0, 0);
}
  /* APPLE LOCAL end radar 4810587 */

/* APPLE LOCAL begin radar 4133425 */
/* Routine is called to issue diagnostic when reference to a private 
   ivar is made and no other variable with same name is found in 
   current scope. */
bool
objc_diagnose_private_ivar (tree id)
{
  tree ivar;
  if (!objc_method_context)
    return false;
  ivar = is_ivar (objc_ivar_chain, id);
  if (ivar && is_private (ivar))
    {
      error ("instance variable %qs is declared private", 
	     IDENTIFIER_POINTER (id));
      return true;
    }
  return false;
}
/* APPLE LOCAL end radar 4133425 */

/* Look up ID as an instance variable.  OTHER contains the result of
   the C or C++ lookup, which we may want to use instead.  */
/* APPLE LOCAL begin C* property (Radar 4436866) */
/* APPLE LOCAL radar 4668023 */
/* Support for referencing property name without use of 'self' qualifier is removed */

tree
objc_lookup_ivar (tree other, tree id)
{
  tree ivar;

  /* If we are not inside of an ObjC method, ivar lookup makes no sense.  */
  if (!objc_method_context)
    return other;
  
  /* APPLE LOCAL begin radar 5796058 - blocks */
  /* Make a quick exit if variable is for a previously declared copied-in
     or byref variable used for an 'ivar' access. */
  if (other && TREE_CODE (other) == VAR_DECL &&
      (BLOCK_DECL_BYREF (other) || BLOCK_DECL_COPIED (other)))
    return other;
  /* APPLE LOCAL end radar 5796058 - blocks */
               
  if (id && TREE_CODE (id) == IDENTIFIER_NODE
      && !strcmp (IDENTIFIER_POINTER (id), "super"))
    /* We have a message to super.  */
    return get_super_receiver ();

  /* In a class method, look up an instance variable only as a last
     resort.  */
  if (TREE_CODE (objc_method_context) == CLASS_METHOD_DECL
      && other && other != error_mark_node)
    return other;

  /* Look up the ivar, but do not use it if it is not accessible.  */
  ivar = is_ivar (objc_ivar_chain, id);

  if (!ivar || is_private (ivar))
    return other;

  /* In an instance method, a local variable (or parameter) may hide the
     instance variable.  */
  if (TREE_CODE (objc_method_context) == INSTANCE_METHOD_DECL
      && other && other != error_mark_node
#ifdef OBJCPLUS
      && CP_DECL_CONTEXT (other) != global_namespace)
#else
      && !DECL_FILE_SCOPE_P (other))
#endif
    {
      warning (0, "local declaration of %qs hides instance variable",
               IDENTIFIER_POINTER (id));

      return other;
    }

  /* At this point, we are either in an instance method with no obscuring
     local definitions, or in a class method with no alternate definitions
     at all.  */
  return build_ivar_reference (id);
}
/* APPLE LOCAL end C* property (Radar 4436866) */

/* Possibly rewrite a function CALL into an OBJ_TYPE_REF expression.  This
   needs to be done if we are calling a function through a cast.  */

tree
objc_rewrite_function_call (tree function, tree params)
{
  if (TREE_CODE (function) == NOP_EXPR
      && TREE_CODE (TREE_OPERAND (function, 0)) == ADDR_EXPR
      && TREE_CODE (TREE_OPERAND (TREE_OPERAND (function, 0), 0))
	 == FUNCTION_DECL)
    {
      function = build3 (OBJ_TYPE_REF, TREE_TYPE (function),
			 TREE_OPERAND (function, 0),
			 TREE_VALUE (params), size_zero_node);
    }

  return function;
}

/* APPLE LOCAL begin radar 5802025 */
tree objc_build_property_getter_func_call (tree pref_expr)
{
  tree getter_call;
  tree save_UOBJC_SUPER_decl = UOBJC_SUPER_decl;
  UOBJC_SUPER_decl = TREE_OPERAND (pref_expr, 2);
  getter_call = objc_build_getter_call (TREE_OPERAND (pref_expr, 0),
                                    TREE_OPERAND (pref_expr, 1));
  UOBJC_SUPER_decl = save_UOBJC_SUPER_decl;
  return getter_call;
}
/* APPLE LOCAL end radar 5802025 */
  
/* Look for the special case of OBJC_TYPE_REF with the address of
   a function in OBJ_TYPE_REF_EXPR (presumably objc_msgSend or one
   of its cousins).  */

enum gimplify_status
objc_gimplify_expr (tree *expr_p, tree *pre_p, tree *post_p)
{
  enum gimplify_status r0, r1;
  if (TREE_CODE (*expr_p) == OBJ_TYPE_REF
      && TREE_CODE (OBJ_TYPE_REF_EXPR (*expr_p)) == ADDR_EXPR
      && TREE_CODE (TREE_OPERAND (OBJ_TYPE_REF_EXPR (*expr_p), 0))
	 == FUNCTION_DECL)
    {
      /* Postincrements in OBJ_TYPE_REF_OBJECT don't affect the
	 value of the OBJ_TYPE_REF, so force them to be emitted
	 during subexpression evaluation rather than after the
	 OBJ_TYPE_REF. This permits objc_msgSend calls in Objective
	 C to use direct rather than indirect calls when the
	 object expression has a postincrement.  */
      r0 = gimplify_expr (&OBJ_TYPE_REF_OBJECT (*expr_p), pre_p, NULL,
			  is_gimple_val, fb_rvalue);
      r1 = gimplify_expr (&OBJ_TYPE_REF_EXPR (*expr_p), pre_p, post_p,
			  is_gimple_val, fb_rvalue);

      return MIN (r0, r1);
    }
  /* APPLE LOCAL begin radar 5276085 */
  else if (TREE_CODE (*expr_p) == OBJC_WEAK_REFERENCE_EXPR)
    {
      tree expr = TREE_OPERAND (*expr_p, 0);
      tree t = TREE_TYPE (expr);
      expr = objc_build_weak_read (expr);
      expr = convert (t, expr);
      *expr_p = expr;
    }
  /* APPLE LOCAL end radar 5276085 */
  /* APPLE LOCAL begin radar 5285911 5494488 */
  else if (objc_property_reference_expr (*expr_p))
    /* APPLE LOCAL radar 5802025 */
    *expr_p = objc_build_property_getter_func_call (*expr_p);
  /* APPLE LOCAL end radar 5285911 5494488 */

#ifdef OBJCPLUS
  return cp_gimplify_expr (expr_p, pre_p, post_p);
#else
  return c_gimplify_expr (expr_p, pre_p, post_p);
#endif
}

/* Given a CALL expression, find the function being called.  The ObjC
   version looks for the OBJ_TYPE_REF_EXPR which is used for objc_msgSend.  */

tree
objc_get_callee_fndecl (tree call_expr)
{
  tree addr = TREE_OPERAND (call_expr, 0);
  if (TREE_CODE (addr) != OBJ_TYPE_REF)
    return 0;

  addr = OBJ_TYPE_REF_EXPR (addr);

  /* If the address is just `&f' for some function `f', then we know
     that `f' is being called.  */
  if (TREE_CODE (addr) == ADDR_EXPR
      && TREE_CODE (TREE_OPERAND (addr, 0)) == FUNCTION_DECL)
    return TREE_OPERAND (addr, 0);

  return 0;
}

/* APPLE LOCAL begin C* language */

/* This routine builds the following type.

   struct __objcFastEnumerationState {
     unsigned long state;
     id *itemsPtr;
     unsigned long *mutationsPtr;
     unsigned long extra[5];
   };  */

static void
build_objc_fast_enum_state_type (void)
{
  tree field_decl, field_decl_chain;
  objc_fast_enum_state_type = start_struct (RECORD_TYPE, 
					    get_identifier ("__objcFastEnumerationState"));

  /* unsigned long state; */
  field_decl = create_field_decl (long_unsigned_type_node, "state");
  field_decl_chain = field_decl;

  /* id *itemsPtr; */
  field_decl = create_field_decl (build_pointer_type (objc_object_type), "itemsPtr");
  chainon (field_decl_chain, field_decl);
 
  /* unsigned long *mutationsPtr; */
  field_decl = create_field_decl (build_pointer_type (
				  long_unsigned_type_node), "mutationsPtr");
  chainon (field_decl_chain, field_decl);

  /* unsigned long extra[5]; */
  field_decl = create_field_decl (build_array_type
                                  (long_unsigned_type_node,
                                   build_index_type
                                   (build_int_cst (NULL_TREE, 5 - 1))),
                                   "extra");
  chainon (field_decl_chain, field_decl);

  finish_struct (objc_fast_enum_state_type, field_decl_chain, NULL_TREE);
}

/* This routine creates a named temporary local variable. */

static tree
objc_create_named_tmp_var (tree type, const char *prefix)
{
  tree decl = create_tmp_var_raw (type, prefix);
  DECL_CONTEXT (decl) = current_function_decl;
  return decl;
}

/* This routine builds and returns an assortment of objc components needed in
   synthesis of the foreach statement; including:

   __objcFastEnumerationState enumState = { 0 };
   id items[16];
   unsigned long limit; 
   unsigned long startMutations;
   unsigned long counter;
   [collection countByEnumeratingWithState:&enumState objects:items count:16];

   it returns expression for:
     objc_enumerationMutation (self)  */

tree
objc_build_foreach_components (tree receiver, tree *enumState_decl, 
		               tree *items_decl, 
			       tree *limit_decl,
			       tree *startMutations_decl,
			       tree *counter_decl,
			       tree *countByEnumeratingWithState)
{
  tree constructor_fields, initlist, init;
  tree type, exp;
  tree sel_name, method_params;

  tree fast_enum_state_type = objc_fast_enum_state_type;
  tree enum_st_type_decl = lookup_name (get_identifier ("NSFastEnumerationState"));
  /* APPLE LOCAL radar 4531086 */
  OBJC_WARN_OBJC2_FEATURES ("foreach-collection-statement");
  if (enum_st_type_decl && TREE_CODE (enum_st_type_decl) == TYPE_DECL)
    fast_enum_state_type = DECL_ORIGINAL_TYPE (enum_st_type_decl) 
			     ? DECL_ORIGINAL_TYPE (enum_st_type_decl)  
			     : TREE_TYPE (enum_st_type_decl);
  gcc_assert (fast_enum_state_type != NULL_TREE 
	      && TREE_CODE (fast_enum_state_type) == RECORD_TYPE);
  constructor_fields = TYPE_FIELDS (fast_enum_state_type);
  /* __objcFastEnumerationState enumState = { 0 }; */
  *enumState_decl = objc_create_named_tmp_var (fast_enum_state_type, "cstar");
  /* APPLE LOCAL begin radar 6285794 */
  initlist = build_tree_list (constructor_fields, 
                              convert (TREE_TYPE(constructor_fields),
                                       integer_zero_node));
  /* APPLE LOCAL end radar 6285794 */
  init = objc_build_constructor (fast_enum_state_type, initlist);
  DECL_INITIAL (*enumState_decl) = init;

  /* id items[16]; */
  type = build_array_type (objc_object_type,
			   build_index_type (build_int_cst (NULL_TREE, 16 - 1)));
  *items_decl = objc_create_named_tmp_var (type, "cstar");

  /* unsigned long limit */
  *limit_decl = objc_create_named_tmp_var (long_unsigned_type_node, "cstar");

  /* unsigned long startMutations */
  *startMutations_decl = objc_create_named_tmp_var (long_unsigned_type_node, "cstar");

  /* unsigned long counter */
  *counter_decl = objc_create_named_tmp_var (long_unsigned_type_node, "cstar");

  /* [collection countByEnumeratingWithState:&enumState objects:items count:16] */
  sel_name = get_identifier ("countByEnumeratingWithState:objects:count:");
  exp = build_fold_addr_expr (*enumState_decl);
  method_params = build_tree_list (NULL_TREE, exp);
/* APPLE LOCAL begin radar 4708210 (for_objc_collection in 4.2) */
#ifndef OBJCPLUS
  {
    struct c_expr cexpr;
    /* In C need to decay array objects when passing to functions. */
    cexpr.value = *items_decl;
    chainon (method_params,
             build_tree_list (NULL_TREE, 
			      default_function_array_conversion (cexpr).value));
  }
#else
  chainon (method_params,
           build_tree_list (NULL_TREE, *items_decl));
#endif
/* APPLE LOCAL end radar 4708210 (for_objc_collection in 4.2) */
  chainon (method_params,
           build_tree_list (NULL_TREE, build_int_cst (NULL_TREE, 16)));

  exp = objc_finish_message_expr (receiver, sel_name, method_params);
  *countByEnumeratingWithState = exp;

  /* objc_enumerationMutation (self) */
  exp = build_function_call (objc_enum_mutation_decl,
                             build_tree_list (NULL_TREE, save_expr (receiver)));

  return exp;
}

/* This routine returns true if TYP is a valid objc object type, 
   suitable for messaging; false otherwise.  */

bool
objc_type_valid_for_messaging (tree typ)
{
  /* APPLE LOCAL begin radar 5831920 */
  if (TREE_CODE (typ) == BLOCK_POINTER_TYPE)
    return true;
  /* APPLE LOCAL end radar 5831920 */
  
  if (!POINTER_TYPE_P (typ))
    return false;

  /* APPLE LOCAL begin radar 5595325 */
  if (objc_nsobject_attribute_type (typ))
    return true;
  /* APPLE LOCAL end radar 5595325 */
  do
    typ = TREE_TYPE (typ);  /* Remove indirections.  */
  while (POINTER_TYPE_P (typ));

  if (TREE_CODE (typ) != RECORD_TYPE)
    return false;

  /* type 'Class' is OK */
  return objc_is_object_id (typ) 
	 || objc_is_class_id (typ) || TYPE_HAS_OBJC_INFO (typ);
}

#ifndef OBJCPLUS
/* Synthesizer routine for C*'s feareach statement. 

   It synthesizes:
   for ( type elem in collection) { stmts; }

   Into:
    {
    type elem;
    __objcFastEnumerationState enumState = { 0 };
    id items[16];

    unsigned long limit = [collection countByEnumeratingWithState:&enumState objects:items count:16];
    if (limit) {
      unsigned long startMutations = *enumState.mutationsPtr;
      do {
         unsigned long counter = 0;
         do {
           if (startMutations != *enumState.mutationsPtr) objc_enumerationMutation(collection);
           elem = enumState.itemsPtr[counter++];
           stmts;
         } while (counter < limit);
      } while (limit = [collection countByEnumeratingWithState:&enumState objects:items count:16]);
    }
    else
      elem = nil; radar 4854605, 5128402 */

void
objc_finish_foreach_loop (location_t location, tree cond, tree for_body, tree blab, tree clab)
{
  tree enumState_decl, items_decl, limit_decl, limit_decl_assign_expr; 
  tree startMutations_decl, counter_decl;
  tree enumerationMutation_call_exp, countByEnumeratingWithState;
  tree exp;
  tree receiver, elem_decl;
  tree bind;
  tree if_condition, do_condition;
  tree outer_if_body, inner_if_body;
  tree outer_if_block_start, inner_if_block_start;
  tree outer_do_body, inner_do_body;
  tree inner_do_block_start, outer_do_block_start;
  tree body;
  location_t save_input_location = input_location;
  tree exit_label;
  /* APPLE LOCAL radar 4854605 - radar 5128402 */
  tree outer_else_body;
 
  receiver = TREE_VALUE (cond);
  elem_decl = TREE_PURPOSE (cond);

  /* APPLE LOCAL begin radar 5130983 */
  if (!lvalue_or_else (&elem_decl, lv_foreach))
    return; 
  /* APPLE LOCAL end radar 5130983 */

  if (!objc_type_valid_for_messaging (TREE_TYPE (elem_decl)))
    {
      error ("selector element does not have a valid object type");
      return;
    }

  if (!objc_type_valid_for_messaging (TREE_TYPE (receiver)))
    {
      error ("expression does not have a valid object type");
      return;
    }

  input_location = location; 
  enumerationMutation_call_exp = objc_build_foreach_components  (receiver, &enumState_decl,
                                                                 &items_decl, &limit_decl,
                                                                 &startMutations_decl, &counter_decl,
                                                                 &countByEnumeratingWithState);

  /* __objcFastEnumerationState enumState = { 0 }; */
  exp = build_stmt (DECL_EXPR, enumState_decl);
  bind = build3 (BIND_EXPR, void_type_node, enumState_decl, exp, NULL);
  TREE_SIDE_EFFECTS (bind) = 1;
  add_stmt (bind);

  /* id items[16]; */
  bind = build3 (BIND_EXPR, void_type_node, items_decl, NULL, NULL);
  TREE_SIDE_EFFECTS (bind) = 1;
  add_stmt (bind);
  
  /* Generate this statement and add it to the list. */
  /* limit = [collection countByEnumeratingWithState:&enumState objects:items count:16] */
  limit_decl_assign_expr = build2 (MODIFY_EXPR, TREE_TYPE (limit_decl), limit_decl, 
				   countByEnumeratingWithState);
  bind = build3 (BIND_EXPR, void_type_node, limit_decl, NULL, NULL);
  TREE_SIDE_EFFECTS (bind) = 1;
  add_stmt (bind);

  outer_if_block_start = c_begin_compound_stmt (true);
  /* if (limit) { */
  outer_if_body = c_begin_compound_stmt (true);

  /* unsigned long startMutations = *enumState.mutationsPtr; */
  exp = objc_build_component_ref (enumState_decl, get_identifier("mutationsPtr"));
  exp = build_indirect_ref (exp, "unary *");
  exp = build2 (MODIFY_EXPR, void_type_node, startMutations_decl, exp);
  bind = build3 (BIND_EXPR, void_type_node, startMutations_decl, exp, NULL);
  TREE_SIDE_EFFECTS (bind) = 1;
  add_stmt (bind);

  outer_do_block_start = c_begin_compound_stmt (true);
  /* do { */
  outer_do_body = c_begin_compound_stmt (true);

  /* unsigned long counter = 0; */
  exp = build2 (MODIFY_EXPR, void_type_node, counter_decl, 
		fold_convert (TREE_TYPE (counter_decl), integer_zero_node)); 
  bind = build3 (BIND_EXPR, void_type_node, counter_decl, exp, NULL);
  TREE_SIDE_EFFECTS (bind) = 1;
  add_stmt (bind);
 
  inner_do_block_start = c_begin_compound_stmt (true);
  /*   do { */
  inner_do_body = c_begin_compound_stmt (true);
  
  inner_if_block_start = c_begin_compound_stmt (true);
  /* if (startMutations != *enumState.mutationsPtr) objc_enumerationMutation (collection); */
  inner_if_body = c_begin_compound_stmt (true);
  exp = objc_build_component_ref (enumState_decl,
				  get_identifier ("mutationsPtr"));
  exp = build_indirect_ref (exp, "unary *");
  if_condition = build_binary_op (NE_EXPR, startMutations_decl, exp, 1);
  body = build_function_call (objc_enum_mutation_decl, 
  			      build_tree_list (NULL_TREE, save_expr (receiver)));
  add_stmt (body);
  inner_if_body = c_end_compound_stmt (inner_if_body, true);
  c_finish_if_stmt (location, if_condition, inner_if_body, NULL, false);
  add_stmt (c_end_compound_stmt (inner_if_block_start, true));

  /* elem = enumState.itemsPtr [counter]; */
  exp = objc_build_component_ref (enumState_decl, get_identifier("itemsPtr")); 
  exp = build_array_ref (exp, counter_decl);
  add_stmt (build2 (MODIFY_EXPR, void_type_node, elem_decl, exp));
  TREE_USED (elem_decl) = 1;

  /* counter++; */
  exp = build2 (PLUS_EXPR, TREE_TYPE (counter_decl), counter_decl, 
		build_int_cst (NULL_TREE, 1));
  add_stmt (build2 (MODIFY_EXPR, void_type_node, counter_decl, exp)); 

  /* stmts; */
  add_stmt (for_body);

  /*   } while (counter < limit ); */
  do_condition  = build_binary_op (LT_EXPR, counter_decl, limit_decl, 1); 
  inner_do_body = c_end_compound_stmt (inner_do_body, true);
/* APPLE LOCAL begin for-fsf-4_4 3274130 5295549 */ \
  c_finish_loop (location, do_condition, NULL, inner_do_body, NULL_TREE, clab,
		 NULL_TREE, false); 
/* APPLE LOCAL end for-fsf-4_4 3274130 5295549 */ \
  add_stmt (c_end_compound_stmt (inner_do_block_start, true));

  /* } while (limit = [collection countByEnumeratingWithState:&enumState objects:items count:16]);  */
  exp = unshare_expr (limit_decl_assign_expr);
  do_condition  = build_binary_op (NE_EXPR, exp, 
				   fold_convert (TREE_TYPE (limit_decl), integer_zero_node), 
				   1); 
  outer_do_body = c_end_compound_stmt (outer_do_body, true);
  /* New spec. requires that if no match was found; i.e. foreach exited with
     no match, 'elem' be set to nil. So, we use a new label for getting out of
     of the outer while loop and set 'elem=nill' after this label. */ 
  exit_label = create_artificial_label ();
/* APPLE LOCAL begin for-fsf-4_4 3274130 5295549 */ \
  c_finish_loop (location, do_condition, NULL, outer_do_body, exit_label,
		 NULL_TREE, NULL_TREE, false);
/* APPLE LOCAL end for-fsf-4_4 3274130 5295549 */ \
  /* elem = nil */
  add_stmt (build2 (MODIFY_EXPR, void_type_node, elem_decl, 
		    fold_convert (TREE_TYPE (elem_decl), integer_zero_node)));
  if (blab)
    add_stmt (build1 (LABEL_EXPR, void_type_node, blab));
  add_stmt (c_end_compound_stmt (outer_do_block_start, true));

  /* } */
  if_condition = unshare_expr (do_condition);
  outer_if_body = c_end_compound_stmt (outer_if_body, true);
  /* APPLE LOCAL begin radar 4854605 - radar 5128402 */
  /* else {elem = nil; } */
  outer_else_body = c_begin_compound_stmt (true);
  add_stmt (build2 (MODIFY_EXPR, void_type_node, elem_decl,
            fold_convert (TREE_TYPE (elem_decl), integer_zero_node)));
  outer_else_body = c_end_compound_stmt (outer_else_body, true);
  c_finish_if_stmt (location, if_condition, outer_if_body, outer_else_body, false);
  /* APPLE LOCAL end radar 4854605 - radar 5128402 */
  add_stmt (c_end_compound_stmt (outer_if_block_start, true));
  input_location = save_input_location;
  return;
}
#endif

/* This routine issues a diagnostic if a @selector expression is
   type cast to anothing other than SEL. */
void
diagnose_selector_cast (tree cast_type, tree sel_exp)
{
  tree type = TREE_TYPE (sel_exp);
  if (type)
    {
      if (type == objc_selector_type && cast_type != objc_selector_type)
	/* @selector expression type cast to something other than a 
     	   matching type. */
	if (flag_objc_abi == 2 || flag_objc2_check)
	  warning (0, "type-cast of @selector expression won't be supported in future");
    }
}

bool 
objc_method_decl (enum tree_code opcode)
{
  return opcode == INSTANCE_METHOD_DECL || opcode == CLASS_METHOD_DECL;
}

/* 
   This routine builds a comma-expr tree represents pre/post incr/decr expression
   tree of the a property (see below).

   CODE is one of POSTINCREMENT_EXPR (prop++), POSTDECREMENT_EXPR (prop--),
   PREINCREMENT_EXPR (++prop), PREDECREMENT_EXPR (--prop).
   GETTER_EXPR is the property in the form of a property getter call.
   INC is the amount property is incrmented or decremented by.

   for POSTINCREMENT_EXPR we generate: (tmp = prop, prop = tmp+1, tmp).
   for POSTDECREMENT_EXPR we generate: (tmp = prop, prop = tmp-1, tmp).
   for PREINCREMENT_EXPR we generate: (tmp = prop+1, prop = tmp, tmp).
   for PREDECREMENT_EXPR we generate: (tmp = prop-1, prop = tmp, tmp). 
   Function returns NULL_TREE (if not a valid property tree) or above
   comma-expression tree.
*/

static tree
objc_build_compound_incr_decr_setter_call (tree receiver, tree prop_ident,
	      				   enum tree_code code, tree getter_expr, 
					   tree inc)
{
  tree temp, bind, rhs, comma_exp;
  temp = objc_create_named_tmp_var (TREE_TYPE (inc), "prop");
  bind = build3 (BIND_EXPR, void_type_node, temp, NULL, NULL);
  TREE_SIDE_EFFECTS (bind) = 1;
  add_stmt (bind);
  /* build rhs expression of 'tmp = exp' statement. */
  if (code == POSTINCREMENT_EXPR || code == POSTDECREMENT_EXPR)
    rhs = getter_expr;
  else if (code == PREINCREMENT_EXPR)
    rhs = build2 (PLUS_EXPR, TREE_TYPE (inc), getter_expr, inc);
  else
    rhs = build2 (MINUS_EXPR, TREE_TYPE (inc), getter_expr, inc);
  /* tmp = rhs; */
  comma_exp = build_modify_expr (temp, NOP_EXPR, rhs);

  /* Now build rhs of 'prop = tmp' statement. */
  if (code == PREINCREMENT_EXPR || code == PREDECREMENT_EXPR)
    rhs = temp;
  else if (code == POSTINCREMENT_EXPR)
    rhs = build2 (PLUS_EXPR, TREE_TYPE (inc), temp, inc);
  else
    rhs = build2 (MINUS_EXPR, TREE_TYPE (inc), temp, inc);
  comma_exp = build_compound_expr (comma_exp,
                    objc_setter_func_call (receiver, prop_ident, rhs));
  /* Shut-off C++ warning on comma-expr with no side-effect. */
  TREE_NO_WARNING (temp) = 1;
  return build_compound_expr (comma_exp, temp);
}

/* APPLE LOCAL begin radar 5285911 */
/*
   global main routine to build property's version of pre/post incr/decr
   expression tree.

   CODE is one of POSTINCREMENT_EXPR (prop++), POSTDECREMENT_EXPR (prop--),
   PREINCREMENT_EXPR (++prop), PREDECREMENT_EXPR (--prop).
   LHS is the property in the form of OBJC_PROPERTY_REFERENCE_EXPR tree.
   INC is the amount property is incrmented or decremented by.
*/

tree
objc_build_incr_decr_setter_call (enum tree_code code, tree lhs, tree inc)
{
  if (objc_property_reference_expr (lhs))
    {
      tree expr = objc_build_getter_call (TREE_OPERAND (lhs, 0), TREE_OPERAND (lhs, 1));
      return objc_build_compound_incr_decr_setter_call 
	       (TREE_OPERAND (lhs, 0), TREE_OPERAND (lhs, 1), code, expr, inc);
    }
  return NULL_TREE;

}
/* APPLE LOCAL end radar 5285911 */

/* APPLE LOCAL begin radar 5040740 */
/* This routine looks up the PROPERTY in the INTERFACE_TYPE class or one of its
   super classes. */
static tree
lookup_nested_property (tree interface_type, tree property)
{
  tree inter = interface_type;
  while (inter)
    {
      tree x;
      if ((x = lookup_property_in_list (inter, property)))
        return x;
      /* Failing that, climb up the inheritance hierarchy.  */
      inter = lookup_interface (CLASS_SUPER_NAME (inter));
    }
  return NULL_TREE;
}

/* This routine looks up METHOD_NAME in the INTERFACE_TYPE class or one of its
   super class. */
static tree
lookup_nested_method (tree interface_type, tree method_name)
{
  tree inter = interface_type;
  while (inter)
    {
      tree x;
      /* APPLE LOCAL begin radar 5777307 */
      /* When property is declared as @optional in a protocol declaration,
         must also seach for the setter/getter method declarations in
         the protocols because they have no declaration in the 'inter'
         class. */
      if ((x = lookup_method (CLASS_NST_METHODS (inter), method_name)) ||
          (x = lookup_method_in_protocol_list 
                 (CLASS_PROTOCOL_LIST (inter), method_name, 0)))
      /* APPLE LOCAL end radar 5777307 */
        return x;
      /* Failing that, climb up the inheritance hierarchy.  */
      inter = lookup_interface (CLASS_SUPER_NAME (inter));
    }
  return NULL_TREE;
}

/* APPLE LOCAL end radar 5040740 */

/* APPLE LOCAL begin radar 5435299 */
/* lookup_property_impl_in_list - Look for an existing property
  @synthesize/@dynamic in current implementation.
*/
static tree
lookup_property_impl_in_list (tree chain, tree property)
{
  tree x;
  for (x = IMPL_PROPERTY_DECL (chain); x; x = TREE_CHAIN (x))
    if (PROPERTY_NAME (x) == property)
      return x;
  return NULL_TREE;
}

/* lookup_ivar_in_property_list - Searches current property list looking for
   one which uses the given ivar_name. If found, it returns this property.
*/

static tree
lookup_ivar_in_property_list (tree chain, tree ivar_name)
{
  tree x;
  for (x = IMPL_PROPERTY_DECL (chain); x; x = TREE_CHAIN (x))
    if (PROPERTY_IVAR_NAME (x) == ivar_name)
      return x;
  return NULL_TREE;
}

/* APPLE LOCAL end radar 5435299 */

/* APPLE LOCAL begin radar 6209554 */
static void error_previous_property_decl (tree x)
{ tree decl = build_decl (VAR_DECL, DECL_NAME (x), TREE_TYPE (x));
  DECL_SOURCE_LOCATION (decl) = DECL_SOURCE_LOCATION (x);
  error ("previous property declaration of %q+D was here", decl);
}
/* APPLE LOCAL end radar 6209554 */
/* This routine declares a new property implementation. Triggered by a @synthesize or
   @dynamic declaration. */
void objc_declare_property_impl (int impl_code, tree tree_list)
{
  tree chain, interface, class;
  if (tree_list == error_mark_node 
      || TREE_PURPOSE (tree_list) == error_mark_node
      || TREE_VALUE (tree_list) == error_mark_node)
    return;
  /* Find the @interface for this @implementation. */
  /* APPLE LOCAL begin radar 5265608 */
  if (objc_implementation_context == NULL_TREE)
    {
      error ("@%s property must be in implementation context",
             impl_code == 1 ? "synthesize" : "dynamic");
      return;
    }
  /* APPLE LOCAL end radar 5265608 */
  interface = lookup_interface (CLASS_NAME (objc_implementation_context));
  if (!interface)
    {
      error ("%s property may not be specified in implementation without an interface", 
	     impl_code == 1 ? "synthesize" : "dynamic");
      return;
    }
  class = interface;
  if (TREE_CODE (objc_implementation_context) == CATEGORY_IMPLEMENTATION_TYPE)
    {
      /* APPLE LOCAL begin radar 5180172 */
      if (impl_code == 1) /* @synthesize */
	error ("@synthesize not allowed in a category's implementation");
      interface = lookup_category (interface,
                                   CLASS_SUPER_NAME (objc_implementation_context));
      if (!interface && (impl_code == 2)) /* @dynamic */
        {
	  error ("@dynamic may not be specified in category without an interface");
          return;
        }
      /* APPLE LOCAL end radar 5180172 */
    }

  if (impl_code == 1 || impl_code == 2)
    {
      for (chain = tree_list; chain; chain = TREE_CHAIN (chain))
	{
	  tree property_name = TREE_VALUE (chain);
	  tree x;
	  if (!property_name)
	    continue;
	  /* APPLE LOCAL begin radar 5040740 */
          /* Look up this property in the @interface declaration or in its superclass(s). */
          x = lookup_nested_property (interface, property_name);
          /* APPLE LOCAL end radar 5040740 */
      	  if (!x)
              error ("no declaration of property %qs found in the interface",
                     IDENTIFIER_POINTER (property_name));
	  else
	    {
	      tree property_decl = copy_node (x);

	      /* APPLE LOCAL radar 5839812 location for synthesized methods  */
	      DECL_SOURCE_LOCATION (property_decl) = input_location;
	      
	      if (impl_code == 2)  /* @dynamic ... */
	        PROPERTY_DYNAMIC (property_decl) = boolean_true_node;
	      else
		{
		  /* @synthesize ... */
		  tree ivar_decl;
		  tree ivar_name = TREE_PURPOSE (chain);
		  /* If 'ivar' unspecified, then an instance variable with same name as 
		     property name must be used. */
		  if (!ivar_name)
		    ivar_name = property_name;
	   	  /* APPLE LOCAL radar 5435299 */
		  PROPERTY_IVAR_NAME (property_decl) = ivar_name;
		  ivar_decl = nested_ivar_lookup (class, ivar_name);
		  /* APPLE LOCAL begin radar 5847641 */
	          if (flag_objc_abi <= 1 && TREE_PURPOSE (chain) && !ivar_decl)
		    warning (0, "ivar name %qs specified on the synthesized property %qs not found",
                    	     IDENTIFIER_POINTER (ivar_name), IDENTIFIER_POINTER (property_name)) ;
		  /* APPLE LOCAL end radar 5847641 */
		  if (ivar_decl)
		    {
		    /* Check for semnatic correctness of the existing ivar. */
		      tree ivar_type = DECL_BIT_FIELD_TYPE (ivar_decl) 
					 ? DECL_BIT_FIELD_TYPE (ivar_decl) 
					 : TREE_TYPE (ivar_decl);
                      /* APPLE LOCAL begin radar 6029624 */
                      tree property_type = TREE_TYPE (property_decl);
		      bool comparison_result;
                      /* APPLE LOCAL begin radar 5435299  - radar 6825962 */
                      if (flag_new_property_ivar_synthesis && flag_objc_abi == 2) {
                        /* In ObjC2 abi, it is illegal when a @synthesize with no named ivar
                           does not have a matching ivar in its class but some superclass ivar
                           already has the desired name */
                        tree record = CLASS_STATIC_TEMPLATE (class);
                        if (record && record != DECL_CONTEXT (ivar_decl))
                          error ("property %qs attempting to use ivar %qs declared in super class of %qs",
                                 IDENTIFIER_POINTER (property_name),
                                 IDENTIFIER_POINTER (ivar_name),
                                 IDENTIFIER_POINTER (OBJC_TYPE_NAME (record)));
                      }
                      /* APPLE LOCAL end radar 5435299  - radar 6825962 */
		      /* APPLE LOCAL begin radar 5389292 */
#ifdef OBJCPLUS
                      if (TREE_CODE (property_type) == REFERENCE_TYPE)
			{
		          property_type = TREE_TYPE (property_type);
		          comparison_result = 
                            !objcp_reference_related_p (property_type, ivar_type);
		        }
		      else
#endif
		        comparison_result = comptypes (ivar_type, property_type) != 1;
		      if (comparison_result
			  /* APPLE LOCAL radar 6231433 */
			  && !objc_compare_types (property_type, ivar_type, -5, NULL_TREE, NULL))
                      /* APPLE LOCAL end radar 6029624 */
		      /* APPLE LOCAL end radar 5389292 */
			{
		          error ("type of property %qs does not match type of ivar %qs", 
			         IDENTIFIER_POINTER (property_name), IDENTIFIER_POINTER (ivar_name));
		          PROPERTY_DYNAMIC (property_decl) = boolean_true_node; /* recover */
		        }
		      if (flag_objc_gc)
			{
			  int strong = objc_is_gcable_type (property_type);
			  if (strong)
			    {
		              /* APPLE LOCAL radar 5389292 */
			      int gc_able = objc_is_gcable_type (ivar_type);
			      if (strong != gc_able)
				{
				  if (strong == -1)
	                	    error ("existing ivar %qs for the '__weak' property %qs must be __weak",
		          		   IDENTIFIER_POINTER (ivar_name),
					   IDENTIFIER_POINTER (property_name));
				  else /* strong == 1 */
	                	    error ("existing ivar %qs for a '__strong' property %qs must be garbage collectable",
					   IDENTIFIER_POINTER (ivar_name),
		          		   IDENTIFIER_POINTER (property_name));
				}
			    }
			}
		    }
                  /* APPLE LOCAL begin radar 5435299 - radar 6209554 */
                  else if (flag_new_property_ivar_synthesis)
                         objc_lookup_property_ivar (class, property_decl);
                  if (flag_new_property_ivar_synthesis &&
		      (x = lookup_ivar_in_property_list (objc_implementation_context,
                                                         ivar_name)))
		    {
                      error ("synthesized properties %qs and %qs both claim ivar %qs",
                             IDENTIFIER_POINTER (property_name),
                             IDENTIFIER_POINTER (DECL_NAME (x)),
                             IDENTIFIER_POINTER (ivar_name));
                      error_previous_property_decl (x);
		    }
                  /* APPLE LOCAL end radar 5435299 - radar 6209554 */
		}
              /* APPLE LOCAL begin radar 5435299 - radar 6209554 */
              if (flag_new_property_ivar_synthesis &&
		  (x = lookup_property_impl_in_list (objc_implementation_context, property_name)))
	        {
                  error ("property %qs is already implemented",
                         IDENTIFIER_POINTER (property_name));
		  error_previous_property_decl (x);
		}
              /* APPLE LOCAL end radar 5435299 - radar 6209554 */
	     /* Add the property to the list of properties for current implementation. */
             TREE_CHAIN (property_decl) = IMPL_PROPERTY_DECL (objc_implementation_context);
             IMPL_PROPERTY_DECL (objc_implementation_context) = property_decl;
	    }
	}
    }
  else
    gcc_assert (false);
}
/* APPLE LOCAL end C* language */

/* APPLE LOCAL begin radar 4985544 - 5195402 */
/* Check that TYPE is an "NSString *" type. */
bool
objc_check_nsstring_pointer_type (tree type)
{
  tree NSString_type;

  if (TREE_CODE (type) != POINTER_TYPE)
    return false;
  NSString_type = buildNSStringType();
  if (!NSString_type)
    return false;
  return (TYPE_MAIN_VARIANT (TREE_TYPE (type)) == NSString_type);
}

/* This routine checks that FORMAT_NUM'th argument ARGUMENT has the 'NSString *' type. */
  
bool  
objc_check_format_nsstring (tree argument, 
                            unsigned HOST_WIDE_INT format_num,
                            bool *no_add_attrs)
{ 
  unsigned HOST_WIDE_INT i;
    
  for (i = 1; i != format_num; i++)
    {        
      if (argument == 0)
        break;
       argument = TREE_CHAIN (argument);
    }
  if (!argument || !objc_check_nsstring_pointer_type (TREE_VALUE (argument)))
    {
      error ("format NSString argument not an 'NSString *' type");
      *no_add_attrs = true;
      return false;
    }
  return true;
}
/* APPLE LOCAL end radar 4985544 - 5195402 */

/* APPLE LOCAL begin radar 5202926 */
/* This routine returns 'true' if given NAME is the special objective-c 
   anonymous file-scope static name. It accomodates c++'s mangling of such 
   symbols as a result of radar 5173149. */
bool
objc_anonymous_local_objc_name (const char * name)
{
  const char *p = name;
#ifdef OBJCPLUS
  if (!strncmp (p, "_ZL", 3))
    {
      p += 3;
      while (p && ISDIGIT (*p))
        p++;
    }
#endif
  return p && !strncmp (p, "_OBJC_", 6);
}
/* APPLE LOCAL end radar 5202926 */

/* APPLE LOCAL begin radar 5355344 */
/* Checks that list has nothing but protocols; returns TRUE or FALSE accordingly.
*/

bool
cp_objc_protocol_id_list (tree protocols)
{
  tree proto;
  if (!protocols)
    return false;
  for (proto = protocols; proto; proto = TREE_CHAIN (proto))
    {
      tree ident = TREE_VALUE (proto);
      tree p = lookup_protocol (ident);
      if (!p)
        return false;
    }
  return true;
}
/* APPLE LOCAL end radar 5355344 */
/* APPLE LOCAL begin radar 5376125 */
/* This routine checks for direct access to a pointer to object ivar and issues a warning
   if done so. */
static void
objc_warn_direct_ivar_access (tree basetype, tree component)
{
  while (basetype != NULL_TREE
         && TREE_CODE (basetype) == RECORD_TYPE && OBJC_TYPE_NAME (basetype)
         && TREE_CODE (OBJC_TYPE_NAME (basetype)) == TYPE_DECL
         && DECL_ORIGINAL_TYPE (OBJC_TYPE_NAME (basetype)))
    basetype = DECL_ORIGINAL_TYPE (OBJC_TYPE_NAME (basetype));
  if (basetype != NULL_TREE && TYPED_OBJECT (basetype))
    {
      tree interface_type = TYPE_OBJC_INTERFACE (basetype);
      if (interface_type &&
          TREE_CODE (interface_type) == CLASS_INTERFACE_TYPE)
        {
          tree ivar = nested_ivar_lookup (interface_type, component);
          if (ivar && managed_objc_object_pointer (TREE_TYPE (ivar)))
            warning (0, "ivar %qs is being directly accessed", IDENTIFIER_POINTER (component));
        }
    }
  return;
}
/* APPLE LOCAL end radar 5376125 */
/* APPLE LOCAL begin radar 5782740 - blocks */
bool block_requires_copying (tree exp)
{
  return TREE_CODE (TREE_TYPE (exp)) == BLOCK_POINTER_TYPE ||
	 objc_is_object_ptr (TREE_TYPE (exp));
}
/* APPLE LOCAL end radar 5782740 - blocks */
#include "gt-objc-objc-act.h"
