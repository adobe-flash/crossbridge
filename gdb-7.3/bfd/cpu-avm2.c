/* BFD support for the AVM2 architecture.
   Copyright 1992, 1994, 1995, 1996, 1998, 2000, 2001, 2002, 2004, 2005,
   2007, 2009, 2010, 2011
   Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"

const bfd_arch_info_type bfd_avm2_arch =
{
  32,	/* 32 bits in a word */
  32,	/* 32 bits in an address */
  8,	/* 8 bits in a byte */
  bfd_arch_avm2,
  0,
  "avm2",
  "avm2",
  3,
  TRUE,
  bfd_default_compatible,
  bfd_default_scan,
  0
};


//extern const bfd_target bfd_avm2_vec;

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


#include "elf/i386.h"
#define bfd_elf32_bfd_reloc_type_lookup	      elf_i386_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup	      elf_i386_reloc_name_lookup

static reloc_howto_type elf_howto_table[]=
{
  HOWTO(R_386_NONE, 0, 0, 0, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_NONE",
	TRUE, 0x00000000, 0x00000000, FALSE),
  HOWTO(R_386_32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_32",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_PC32, 0, 2, 32, TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_PC32",
	TRUE, 0xffffffff, 0xffffffff, TRUE),
  HOWTO(R_386_GOT32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_GOT32",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_PLT32, 0, 2, 32, TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_PLT32",
	TRUE, 0xffffffff, 0xffffffff, TRUE),
  HOWTO(R_386_COPY, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_COPY",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_GLOB_DAT, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_GLOB_DAT",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_JUMP_SLOT, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_JUMP_SLOT",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_RELATIVE, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_RELATIVE",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_GOTOFF, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_GOTOFF",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_GOTPC, 0, 2, 32, TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_GOTPC",
	TRUE, 0xffffffff, 0xffffffff, TRUE),

  /* We have a gap in the reloc numbers here.
     R_386_standard counts the number up to this point, and
     R_386_ext_offset is the value to subtract from a reloc type of
     R_386_16 thru R_386_PC8 to form an index into this table.  */
#define R_386_standard (R_386_GOTPC + 1)
#define R_386_ext_offset (R_386_TLS_TPOFF - R_386_standard)

  /* These relocs are a GNU extension.  */
  HOWTO(R_386_TLS_TPOFF, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_TPOFF",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_IE, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_IE",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_GOTIE, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_GOTIE",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_LE, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_LE",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_GD, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_GD",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_LDM, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_LDM",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_16, 0, 1, 16, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_16",
	TRUE, 0xffff, 0xffff, FALSE),
  HOWTO(R_386_PC16, 0, 1, 16, TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_PC16",
	TRUE, 0xffff, 0xffff, TRUE),
  HOWTO(R_386_8, 0, 0, 8, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_8",
	TRUE, 0xff, 0xff, FALSE),
  HOWTO(R_386_PC8, 0, 0, 8, TRUE, 0, complain_overflow_signed,
	bfd_elf_generic_reloc, "R_386_PC8",
	TRUE, 0xff, 0xff, TRUE),

#define R_386_ext (R_386_PC8 + 1 - R_386_ext_offset)
#define R_386_tls_offset (R_386_TLS_LDO_32 - R_386_ext)
  /* These are common with Solaris TLS implementation.  */
  HOWTO(R_386_TLS_LDO_32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_LDO_32",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_IE_32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_IE_32",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_LE_32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_LE_32",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_DTPMOD32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_DTPMOD32",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_DTPOFF32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_DTPOFF32",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_TPOFF32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_TPOFF32",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  EMPTY_HOWTO (38),
  HOWTO(R_386_TLS_GOTDESC, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_GOTDESC",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_TLS_DESC_CALL, 0, 0, 0, FALSE, 0, complain_overflow_dont,
	bfd_elf_generic_reloc, "R_386_TLS_DESC_CALL",
	FALSE, 0, 0, FALSE),
  HOWTO(R_386_TLS_DESC, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_TLS_DESC",
	TRUE, 0xffffffff, 0xffffffff, FALSE),
  HOWTO(R_386_IRELATIVE, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_386_IRELATIVE",
	TRUE, 0xffffffff, 0xffffffff, FALSE),

  /* Another gap.  */
#define R_386_irelative (R_386_IRELATIVE + 1 - R_386_tls_offset)
#define R_386_vt_offset (R_386_GNU_VTINHERIT - R_386_irelative)

/* GNU extension to record C++ vtable hierarchy.  */
  HOWTO (R_386_GNU_VTINHERIT,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_386_GNU_VTINHERIT",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

/* GNU extension to record C++ vtable member usage.  */
  HOWTO (R_386_GNU_VTENTRY,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn, /* special_function */
	 "R_386_GNU_VTENTRY",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE)			/* pcrel_offset */

#define R_386_vt (R_386_GNU_VTENTRY + 1 - R_386_vt_offset)

};

static reloc_howto_type *
elf_i386_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			    const char *r_name)
{
  unsigned int i;

  for (i = 0; i < sizeof (elf_howto_table) / sizeof (elf_howto_table[0]); i++)
    if (elf_howto_table[i].name != NULL
	&& strcasecmp (elf_howto_table[i].name, r_name) == 0)
      return &elf_howto_table[i];

  return NULL;
}

#ifdef DEBUG_GEN_RELOC
#define TRACE(str) \
  fprintf (stderr, "i386 bfd reloc lookup %d (%s)\n", code, str)
#else
#define TRACE(str)
#endif

static reloc_howto_type *
elf_i386_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			    bfd_reloc_code_real_type code)
{
  switch (code)
    {
    case BFD_RELOC_NONE:
      TRACE ("BFD_RELOC_NONE");
      return &elf_howto_table[R_386_NONE];

    case BFD_RELOC_32:
      TRACE ("BFD_RELOC_32");
      return &elf_howto_table[R_386_32];

    case BFD_RELOC_CTOR:
      TRACE ("BFD_RELOC_CTOR");
      return &elf_howto_table[R_386_32];

    case BFD_RELOC_32_PCREL:
      TRACE ("BFD_RELOC_PC32");
      return &elf_howto_table[R_386_PC32];

    case BFD_RELOC_386_GOT32:
      TRACE ("BFD_RELOC_386_GOT32");
      return &elf_howto_table[R_386_GOT32];

    case BFD_RELOC_386_PLT32:
      TRACE ("BFD_RELOC_386_PLT32");
      return &elf_howto_table[R_386_PLT32];

    case BFD_RELOC_386_COPY:
      TRACE ("BFD_RELOC_386_COPY");
      return &elf_howto_table[R_386_COPY];

    case BFD_RELOC_386_GLOB_DAT:
      TRACE ("BFD_RELOC_386_GLOB_DAT");
      return &elf_howto_table[R_386_GLOB_DAT];

    case BFD_RELOC_386_JUMP_SLOT:
      TRACE ("BFD_RELOC_386_JUMP_SLOT");
      return &elf_howto_table[R_386_JUMP_SLOT];

    case BFD_RELOC_386_RELATIVE:
      TRACE ("BFD_RELOC_386_RELATIVE");
      return &elf_howto_table[R_386_RELATIVE];

    case BFD_RELOC_386_GOTOFF:
      TRACE ("BFD_RELOC_386_GOTOFF");
      return &elf_howto_table[R_386_GOTOFF];

    case BFD_RELOC_386_GOTPC:
      TRACE ("BFD_RELOC_386_GOTPC");
      return &elf_howto_table[R_386_GOTPC];

      /* These relocs are a GNU extension.  */
    case BFD_RELOC_386_TLS_TPOFF:
      TRACE ("BFD_RELOC_386_TLS_TPOFF");
      return &elf_howto_table[R_386_TLS_TPOFF - R_386_ext_offset];

    case BFD_RELOC_386_TLS_IE:
      TRACE ("BFD_RELOC_386_TLS_IE");
      return &elf_howto_table[R_386_TLS_IE - R_386_ext_offset];

    case BFD_RELOC_386_TLS_GOTIE:
      TRACE ("BFD_RELOC_386_TLS_GOTIE");
      return &elf_howto_table[R_386_TLS_GOTIE - R_386_ext_offset];

    case BFD_RELOC_386_TLS_LE:
      TRACE ("BFD_RELOC_386_TLS_LE");
      return &elf_howto_table[R_386_TLS_LE - R_386_ext_offset];

    case BFD_RELOC_386_TLS_GD:
      TRACE ("BFD_RELOC_386_TLS_GD");
      return &elf_howto_table[R_386_TLS_GD - R_386_ext_offset];

    case BFD_RELOC_386_TLS_LDM:
      TRACE ("BFD_RELOC_386_TLS_LDM");
      return &elf_howto_table[R_386_TLS_LDM - R_386_ext_offset];

    case BFD_RELOC_16:
      TRACE ("BFD_RELOC_16");
      return &elf_howto_table[R_386_16 - R_386_ext_offset];

    case BFD_RELOC_16_PCREL:
      TRACE ("BFD_RELOC_16_PCREL");
      return &elf_howto_table[R_386_PC16 - R_386_ext_offset];

    case BFD_RELOC_8:
      TRACE ("BFD_RELOC_8");
      return &elf_howto_table[R_386_8 - R_386_ext_offset];

    case BFD_RELOC_8_PCREL:
      TRACE ("BFD_RELOC_8_PCREL");
      return &elf_howto_table[R_386_PC8 - R_386_ext_offset];

    /* Common with Sun TLS implementation.  */
    case BFD_RELOC_386_TLS_LDO_32:
      TRACE ("BFD_RELOC_386_TLS_LDO_32");
      return &elf_howto_table[R_386_TLS_LDO_32 - R_386_tls_offset];

    case BFD_RELOC_386_TLS_IE_32:
      TRACE ("BFD_RELOC_386_TLS_IE_32");
      return &elf_howto_table[R_386_TLS_IE_32 - R_386_tls_offset];

    case BFD_RELOC_386_TLS_LE_32:
      TRACE ("BFD_RELOC_386_TLS_LE_32");
      return &elf_howto_table[R_386_TLS_LE_32 - R_386_tls_offset];

    case BFD_RELOC_386_TLS_DTPMOD32:
      TRACE ("BFD_RELOC_386_TLS_DTPMOD32");
      return &elf_howto_table[R_386_TLS_DTPMOD32 - R_386_tls_offset];

    case BFD_RELOC_386_TLS_DTPOFF32:
      TRACE ("BFD_RELOC_386_TLS_DTPOFF32");
      return &elf_howto_table[R_386_TLS_DTPOFF32 - R_386_tls_offset];

    case BFD_RELOC_386_TLS_TPOFF32:
      TRACE ("BFD_RELOC_386_TLS_TPOFF32");
      return &elf_howto_table[R_386_TLS_TPOFF32 - R_386_tls_offset];

    case BFD_RELOC_386_TLS_GOTDESC:
      TRACE ("BFD_RELOC_386_TLS_GOTDESC");
      return &elf_howto_table[R_386_TLS_GOTDESC - R_386_tls_offset];

    case BFD_RELOC_386_TLS_DESC_CALL:
      TRACE ("BFD_RELOC_386_TLS_DESC_CALL");
      return &elf_howto_table[R_386_TLS_DESC_CALL - R_386_tls_offset];

    case BFD_RELOC_386_TLS_DESC:
      TRACE ("BFD_RELOC_386_TLS_DESC");
      return &elf_howto_table[R_386_TLS_DESC - R_386_tls_offset];

    case BFD_RELOC_386_IRELATIVE:
      TRACE ("BFD_RELOC_386_IRELATIVE");
      return &elf_howto_table[R_386_IRELATIVE];

    case BFD_RELOC_VTABLE_INHERIT:
      TRACE ("BFD_RELOC_VTABLE_INHERIT");
      return &elf_howto_table[R_386_GNU_VTINHERIT - R_386_vt_offset];

    case BFD_RELOC_VTABLE_ENTRY:
      TRACE ("BFD_RELOC_VTABLE_ENTRY");
      return &elf_howto_table[R_386_GNU_VTENTRY - R_386_vt_offset];

    default:
      break;
    }

  TRACE ("Unknown");
  return 0;
}

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

//#undef elf32_bed
//#define elf32_bed elf32_avm_bed

#include "elf32-target.h"
