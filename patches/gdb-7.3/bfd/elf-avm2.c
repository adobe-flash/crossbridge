/*
** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2, or (at your option)
** any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
**/

#define TARGET_LITTLE_SYM		bfd_avm2_vec
#define TARGET_LITTLE_NAME		"avm2"
#define ELF_ARCH			bfd_arch_avm2
#define ELF_TARGET_ID			GENERIC_ELF_DATA
#define ELF_MACHINE_CODE		EM_386 // TODO: change this?
#define ELF_MAXPAGESIZE			0x1000

#define elf_backend_can_gc_sections	1
#define elf_backend_can_refcount	1
#define elf_backend_want_got_plt	1
#define elf_backend_plt_readonly	1
#define elf_backend_want_plt_sym	0
#define elf_backend_got_header_size	12

#if 0

/* Support RELA for objdump of prelink objects.  */
#define elf_info_to_howto		      elf_i386_info_to_howto_rel
#define elf_info_to_howto_rel		      elf_i386_info_to_howto_rel

#define bfd_elf32_mkobject		      elf_i386_mkobject

#define bfd_elf32_bfd_is_local_label_name     elf_i386_is_local_label_name
#define bfd_elf32_bfd_link_hash_table_create  elf_i386_link_hash_table_create
#define bfd_elf32_bfd_link_hash_table_free    elf_i386_link_hash_table_free
#define bfd_elf32_bfd_reloc_type_lookup	      elf_i386_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup	      elf_i386_reloc_name_lookup

#define elf_backend_adjust_dynamic_symbol     elf_i386_adjust_dynamic_symbol
#define elf_backend_relocs_compatible	      _bfd_elf_relocs_compatible
#define elf_backend_check_relocs	      elf_i386_check_relocs
#define elf_backend_copy_indirect_symbol      elf_i386_copy_indirect_symbol
#define elf_backend_create_dynamic_sections   elf_i386_create_dynamic_sections
#define elf_backend_fake_sections	      elf_i386_fake_sections
#define elf_backend_finish_dynamic_sections   elf_i386_finish_dynamic_sections
#define elf_backend_finish_dynamic_symbol     elf_i386_finish_dynamic_symbol
#define elf_backend_gc_mark_hook	      elf_i386_gc_mark_hook
#define elf_backend_gc_sweep_hook	      elf_i386_gc_sweep_hook
#define elf_backend_grok_prstatus	      elf_i386_grok_prstatus
#define elf_backend_grok_psinfo		      elf_i386_grok_psinfo
#define elf_backend_reloc_type_class	      elf_i386_reloc_type_class
#define elf_backend_relocate_section	      elf_i386_relocate_section
#define elf_backend_size_dynamic_sections     elf_i386_size_dynamic_sections
#define elf_backend_always_size_sections      elf_i386_always_size_sections
#define elf_backend_omit_section_dynsym \
  ((bfd_boolean (*) (bfd *, struct bfd_link_info *, asection *)) bfd_true)
#define elf_backend_plt_sym_val		      elf_i386_plt_sym_val
#define elf_backend_hash_symbol		      elf_i386_hash_symbol
#define elf_backend_add_symbol_hook           elf_i386_add_symbol_hook
#undef	elf_backend_post_process_headers
#define	elf_backend_post_process_headers	_bfd_elf_set_osabi

#endif

#include "elf32-target.h"

