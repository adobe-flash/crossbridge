/* Prototypes.
   Copyright (C) 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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

extern void darwin_init_sections (void);
extern int name_needs_quotes (const char *);

/* APPLE LOCAL ARM pic support */
extern int machopic_lookup_stub_or_non_lazy_ptr (const char *);
extern void machopic_validate_stub_or_non_lazy_ptr (const char *);

extern const char *machopic_function_base_name (void);
extern void machopic_output_function_base_name (FILE *);
extern const char *machopic_indirection_name (rtx, bool);
extern const char *machopic_mcount_stub_name (void);

#ifdef RTX_CODE

extern rtx machopic_function_base_sym (void);
extern int machopic_operand_p (rtx);
extern enum machopic_addr_class machopic_classify_symbol (rtx);

extern rtx machopic_indirect_data_reference (rtx, rtx);
/* APPLE LOCAL 4380289 */
extern rtx machopic_force_indirect_call_target (rtx);
extern rtx machopic_indirect_call_target (rtx);
extern rtx machopic_legitimize_pic_address (rtx, enum machine_mode, rtx);

extern void machopic_asm_out_constructor (rtx, int);
extern void machopic_asm_out_destructor (rtx, int);
/* APPLE LOCAL ARM pic support */
extern int machopic_data_defined_p (rtx sym_ref);
extern int indirect_data (rtx sym_ref);
#endif /* RTX_CODE */

#ifdef TREE_CODE

extern void machopic_define_symbol (rtx);
extern void darwin_encode_section_info (tree, rtx, int);
extern void darwin_set_default_type_attributes (tree);
/* APPLE LOCAL CW asm blocks */
extern tree darwin_iasm_special_label (tree);

#endif /* TREE_CODE */

extern void machopic_finish (FILE *);

extern int machopic_reloc_rw_mask (void);
extern section *machopic_select_section (tree, int, unsigned HOST_WIDE_INT);
extern section *machopic_select_rtx_section (enum machine_mode, rtx,
					     unsigned HOST_WIDE_INT);

extern void darwin_unique_section (tree decl, int reloc);
extern void darwin_asm_named_section (const char *, unsigned int, tree);
extern void darwin_non_lazy_pcrel (FILE *, rtx);

extern void darwin_emit_unwind_label (FILE *, tree, int, int);
extern void darwin_emit_except_table_label (FILE *);

extern void darwin_pragma_ignore (struct cpp_reader *);
extern void darwin_pragma_options (struct cpp_reader *);
extern void darwin_pragma_unused (struct cpp_reader *);
extern void darwin_pragma_ms_struct (struct cpp_reader *);
/* APPLE LOCAL pragma fenv */
extern void darwin_pragma_fenv (struct cpp_reader *);
/* APPLE LOCAL pragma reverse_bitfields */
extern void darwin_pragma_reverse_bitfields (struct cpp_reader *);
/* APPLE LOCAL begin optimization pragmas 3124235/3420242 */
extern void darwin_pragma_opt_level (struct cpp_reader *);
extern void darwin_pragma_opt_size (struct cpp_reader *);
/* APPLE LOCAL end optimization pragmas 3124235/3420242 */

/* APPLE LOCAL begin Macintosh alignment 2002-1-22 --ff */
extern void darwin_pragma_pack (struct cpp_reader *);
/* APPLE LOCAL end Macintosh alignment 2002-1-22 --ff */

/* APPLE LOCAL begin darwin_set_section_for_var_p  */
extern section* darwin_set_section_for_var_p (tree, int, int, section*);
/* APPLE LOCAL end darwin_set_section_for_var_p  */

/* APPLE LOCAL ObjC GC */
extern tree darwin_handle_objc_gc_attribute (tree *, tree, tree, int, bool *);
/* APPLE LOCAL file radar 5595352 */
extern tree darwin_handle_nsobject_attribute (tree *, tree, tree, int, bool *);

extern void darwin_file_start (void);
extern void darwin_file_end (void);

extern void darwin_mark_decl_preserved (const char *);

extern tree darwin_handle_kext_attribute (tree *, tree, tree, int, bool *);
extern tree darwin_handle_weak_import_attribute (tree *node, tree name,
						 tree args, int flags,
						 bool * no_add_attrs);
extern void machopic_output_stub (FILE *, const char *, const char *);
extern void darwin_globalize_label (FILE *, const char *);
extern void darwin_assemble_visibility (tree, int);
extern void darwin_asm_output_dwarf_delta (FILE *, int, const char *,
					   const char *);
extern void darwin_asm_output_dwarf_offset (FILE *, int, const char *,
					    section *);
extern bool darwin_binds_local_p (tree);
extern void darwin_cpp_builtins (struct cpp_reader *);
/* APPLE LOCAL iframework for 4.3 4094959 */
extern bool darwin_handle_c_option (size_t code, const char *arg, int value);
extern void darwin_asm_output_anchor (rtx symbol);
extern bool darwin_kextabi_p (void);
extern void darwin_override_options (void);
/* APPLE LOCAL optimization pragmas 3124235/3420242 */
extern void reset_optimization_options (int, int);

/* APPLE LOCAL begin constant cfstrings */
extern void darwin_init_cfstring_builtins (void);
extern tree darwin_expand_tree_builtin (tree, tree, tree);
extern tree darwin_construct_objc_string (tree);
extern bool darwin_constant_cfstring_p (tree);
/* APPLE LOCAL end constant cfstrings */

/* APPLE LOCAL begin radar 4985544 */
extern bool darwin_cfstring_type_node (tree);
extern bool objc_check_format_cfstring (tree, unsigned HOST_WIDE_INT, bool *);
/* APPLE LOCAL end radar 4985544 */
/* APPLE LOCAL radar 5195402 */
extern bool objc_check_cfstringref_type (tree);

/* APPLE LOCAL begin radar 2996215 - 6068877 */
extern bool cvt_utf8_utf16 (const unsigned char *, size_t, unsigned char **, size_t *);
extern tree create_init_utf16_var (const unsigned char *inbuf, size_t length, size_t *numUniChars);
/* APPLE LOCAL end radar 2996215 - 6068877 */
/* APPLE LOCAL radar 5202926 */
extern bool objc_anonymous_local_objc_name (const char *);
