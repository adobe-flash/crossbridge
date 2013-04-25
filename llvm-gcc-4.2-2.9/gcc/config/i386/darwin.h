/* Target definitions for x86 running Darwin.
   Copyright (C) 2001, 2002, 2004, 2005 Free Software Foundation, Inc.
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

/* Enable Mach-O bits in generic x86 code.  */
#undef TARGET_MACHO
#define TARGET_MACHO 1

/* APPLE LOCAL begin mainline */
#undef  TARGET_64BIT
/* APPLE LOCAL begin 5612787 mainline sse4 */
#define TARGET_64BIT (target_flags & MASK_64BIT)
/* APPLE LOCAL end 5612787 mainline sse4 */

#ifdef IN_LIBGCC2
#undef TARGET_64BIT
#ifdef __x86_64__
#define TARGET_64BIT 1
#else
#define TARGET_64BIT 0
#endif
#endif

#define TARGET_VERSION fprintf (stderr, " (i686 Darwin)");
/* APPLE LOCAL end mainline */

#undef  TARGET_64BIT
#define TARGET_64BIT (target_flags & MASK_64BIT)

#ifdef IN_LIBGCC2
#undef TARGET_64BIT
#ifdef __x86_64__
#define TARGET_64BIT 1
#else
#define TARGET_64BIT 0
#endif
#endif

/* Size of the Obj-C jump buffer.  */
#define OBJC_JBLEN ((TARGET_64BIT) ? ((9 * 2) + 3 + 16) : (18))

#undef TARGET_FPMATH_DEFAULT
#define TARGET_FPMATH_DEFAULT (TARGET_SSE ? FPMATH_SSE : FPMATH_387)

/* APPLE LOCAL begin mainline */
#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE (TARGET_64BIT ? "long int" : "int")

#undef MAX_BITS_PER_WORD
#define MAX_BITS_PER_WORD 64

#define TARGET_OS_CPP_BUILTINS()                \
  do                                            \
    {                                           \
      builtin_define ("__LITTLE_ENDIAN__");     \
      darwin_cpp_builtins (pfile);		\
    }                                           \
  while (0)
/* APPLE LOCAL end mainline */

#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE (TARGET_64BIT ? "long int" : "int")

#undef WCHAR_TYPE
#define WCHAR_TYPE "int"

#undef WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE 32

#undef MAX_BITS_PER_WORD
#define MAX_BITS_PER_WORD 64

#undef FORCE_PREFERRED_STACK_BOUNDARY_IN_MAIN
#define FORCE_PREFERRED_STACK_BOUNDARY_IN_MAIN (0)

/* We want -fPIC by default, unless we're using -static to compile for
   the kernel or some such.  */

#undef CC1_SPEC
#define CC1_SPEC "%{!mkernel:%{!static:%{!mdynamic-no-pic:-fPIC}}} \
  "/* APPLE LOCAL ARM ignore -mthumb and -mno-thumb */"\
  %<mthumb %<mno-thumb \
  "/* APPLE LOCAL ARM 5683689 */"\
  %{!mmacosx-version-min=*: %{!miphoneos-version-min=*: %(darwin_cc1_minversion)}} \
  "/* APPLE LOCAL ignore -mcpu=G4 -mcpu=G5 */"\
  %<faltivec %<mno-fused-madd %<mlong-branch %<mlongcall %<mcpu=G4 %<mcpu=G5 \
  %{g: %{!fno-eliminate-unused-debug-symbols: -feliminate-unused-debug-symbols }}"

/* APPLE LOCAL AltiVec */
#define CPP_ALTIVEC_SPEC "%<faltivec"

/* APPLE LOCAL begin mainline */
#undef ASM_SPEC
/* APPLE LOCAL begin kext weak_import 5935650 */
#define ASM_SPEC "-arch %(darwin_arch) -force_cpusubtype_ALL \
  %{mkernel|static|fapple-kext:%{!m64:-static}}"
/* APPLE LOCAL end kext weak_import 5935650 */

#define DARWIN_ARCH_SPEC "%{m64:x86_64;:i386}"
#define DARWIN_SUBARCH_SPEC DARWIN_ARCH_SPEC

/* APPLE LOCAL begin mainline 2007-03-13 5005743 5040758 */ \
/* Determine a minimum version based on compiler options.  */
#define DARWIN_MINVERSION_SPEC				\
 "%{!m64|fgnu-runtime:10.4;				\
    ,objective-c|,objc-cpp-output:10.5;			\
    ,objective-c-header:10.5;				\
    ,objective-c++|,objective-c++-cpp-output:10.5;	\
    ,objective-c++-header|,objc++-cpp-output:10.5;	\
    :10.4}"

/* APPLE LOCAL end mainline 2007-03-13 5005743 5040758 */ \
/* APPLE LOCAL begin ARM 5683689 */
/* Default cc1 option for specifying minimum version number.  */
#define DARWIN_CC1_MINVERSION_SPEC "-mmacosx-version-min=%(darwin_minversion)"

/* Default ld option for specifying minimum version number.  */
#define DARWIN_LD_MINVERSION_SPEC "-macosx_version_min %(darwin_minversion)"

/* Use macosx version numbers by default.  */
#define DARWIN_DEFAULT_VERSION_TYPE DARWIN_VERSION_MACOSX
/* APPLE LOCAL end ARM 5683689 */

/* APPLE LOCAL ARM 5681645 8307333 */
#define DARWIN_IPHONEOS_LIBGCC_SPEC "-lgcc"

/* APPLE LOCAL begin link optimizations 6499452 */
#undef DARWIN_CRT1_SPEC
#define DARWIN_CRT1_SPEC						\
  "%:version-compare(!> 10.5 mmacosx-version-min= -lcrt1.o)		\
   %:version-compare(>< 10.5 10.6 mmacosx-version-min= -lcrt1.10.5.o)	\
   %:version-compare(>= 10.6 mmacosx-version-min= -lcrt1.10.6.o)"

#undef DARWIN_DYLIB1_SPEC
#define DARWIN_DYLIB1_SPEC						\
  "%:version-compare(!> 10.5 mmacosx-version-min= -ldylib1.o)		\
   %:version-compare(>< 10.5 10.6 mmacosx-version-min= -ldylib1.10.5.o)"

#undef DARWIN_BUNDLE1_SPEC
#define DARWIN_BUNDLE1_SPEC						\
  "%:version-compare(!> 10.6 mmacosx-version-min= -lbundle1.o)"
/* APPLE LOCAL end link optimizations 6499452 */

#undef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS                                   \
  DARWIN_EXTRA_SPECS                                            \
  { "darwin_arch", DARWIN_ARCH_SPEC },                          \
  { "darwin_crt2", "" },                                        \
  { "darwin_subarch", DARWIN_SUBARCH_SPEC },
/* APPLE LOCAL end mainline */

/* APPLE LOCAL begin prefer -lSystem 6645902 */
#undef LINK_GCC_C_SEQUENCE_SPEC
#define LINK_GCC_C_SEQUENCE_SPEC					\
  "%{miphoneos-version-min=*: %G %L}					\
   %{!miphoneos-version-min=*:						\
     %{!static:%:version-compare(>= 10.6 mmacosx-version-min= -lSystem)} %G %L}"
/* APPLE LOCAL end prefer -lSystem 6645902 */

/* Use the following macro for any Darwin/x86-specific command-line option
   translation.  */
#define SUBTARGET_OPTION_TRANSLATE_TABLE \
  { "", "" }

/* The Darwin assembler mostly follows AT&T syntax.  */
#undef ASSEMBLER_DIALECT
#define ASSEMBLER_DIALECT ASM_ATT

/* Define macro used to output shift-double opcodes when the shift
   count is in %cl.  Some assemblers require %cl as an argument;
   some don't.  This macro controls what to do: by default, don't
   print %cl.  */

#define SHIFT_DOUBLE_OMITS_COUNT 0

extern void darwin_x86_file_end (void);
#undef TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END darwin_x86_file_end

/* Define the syntax of pseudo-ops, labels and comments.  */

/* String containing the assembler's comment-starter.  */

#define ASM_COMMENT_START "#"

/* By default, target has a 80387, uses IEEE compatible arithmetic,
   and returns float values in the 387.  */

#define TARGET_SUBTARGET_DEFAULT (MASK_80387 | MASK_IEEE_FP | MASK_FLOAT_RETURNS | MASK_128BIT_LONG_DOUBLE)
/* APPLE LOCAL begin mainline */
/* For darwin we want to target specific processor features as a minimum,
   but these unfortunately don't correspond to a specific processor.  */
#undef TARGET_SUBTARGET32_DEFAULT
#define TARGET_SUBTARGET32_DEFAULT (MASK_MMX                   \
				    | MASK_SSE		       \
				    | MASK_SSE2)

#undef TARGET_SUBTARGET64_DEFAULT
#define TARGET_SUBTARGET64_DEFAULT (MASK_MMX                   \
				    | MASK_SSE		       \
				    | MASK_SSE2		       \
				    | MASK_SSE3)
/* APPLE LOCAL end mainline */
/* APPLE LOCAL mdynamic-no-pic */
/* Remove disabling of mdynamic-no-pic */

#undef GOT_SYMBOL_NAME
#define GOT_SYMBOL_NAME (machopic_function_base_name ())

/* Define the syntax of pseudo-ops, labels and comments.  */

#define LPREFIX "L"

/* These are used by -fbranch-probabilities */
#define HOT_TEXT_SECTION_NAME "__TEXT,__text,regular,pure_instructions"
#define UNLIKELY_EXECUTED_TEXT_SECTION_NAME \
                              "__TEXT,__unlikely,regular,pure_instructions"

/* Assembler pseudos to introduce constants of various size.  */

#define ASM_BYTE_OP "\t.byte\t"
#define ASM_SHORT "\t.word\t"
#define ASM_LONG "\t.long\t"
#define ASM_QUAD "\t.quad\t"

#define SUBTARGET_ENCODE_SECTION_INFO  darwin_encode_section_info

#undef ASM_OUTPUT_ALIGN
#define ASM_OUTPUT_ALIGN(FILE,LOG)	\
 do { if ((LOG) != 0)			\
        {				\
          if (in_section == text_section) \
            fprintf (FILE, "\t%s %d,0x90\n", ALIGN_ASM_OP, (LOG)); \
          else				\
            fprintf (FILE, "\t%s %d\n", ALIGN_ASM_OP, (LOG)); \
        }				\
    } while (0)

/* This says how to output an assembler line
   to define a global common symbol.  */

#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)  \
( fputs (".comm ", (FILE)),			\
  assemble_name ((FILE), (NAME)),		\
  /* APPLE LOCAL begin mainline */         \
  fprintf ((FILE), ","HOST_WIDE_INT_PRINT_UNSIGNED"\n", (ROUNDED)))
  /* APPLE LOCAL end mainline */

/* This says how to output an assembler line
   to define a local common symbol.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)  \
( fputs (".lcomm ", (FILE)),			\
  assemble_name ((FILE), (NAME)),		\
  fprintf ((FILE), ","HOST_WIDE_INT_PRINT_UNSIGNED"\n", (ROUNDED)))


/* APPLE LOCAL begin Macintosh alignment 2002-2-19 --ff */
#if 0
#define MASK_ALIGN_NATURAL	0x40000000
#define TARGET_ALIGN_NATURAL	(target_flags & MASK_ALIGN_NATURAL)
#define MASK_ALIGN_MAC68K	0x20000000
#define TARGET_ALIGN_MAC68K	(target_flags & MASK_ALIGN_MAC68K)
#endif
#define rs6000_alignment_flags target_flags

#define ROUND_TYPE_ALIGN(TYPE, COMPUTED, SPECIFIED) \
  (((TREE_CODE (TYPE) == RECORD_TYPE \
     || TREE_CODE (TYPE) == UNION_TYPE \
     || TREE_CODE (TYPE) == QUAL_UNION_TYPE) \
    && OPTION_ALIGN_MAC68K \
    && MAX (COMPUTED, SPECIFIED) == 8) ? 16 \
    : MAX (COMPUTED, SPECIFIED))
/* APPLE LOCAL end Macintosh alignment 2002-2-19 --ff */

/* Darwin profiling -- call mcount.  */
#undef FUNCTION_PROFILER
#define FUNCTION_PROFILER(FILE, LABELNO)				\
    do {								\
      /* APPLE LOCAL axe stubs 5571540 */				\
      if (darwin_stubs && MACHOPIC_INDIRECT && !TARGET_64BIT)		\
	{								\
	  const char *name = machopic_mcount_stub_name ();		\
	  fprintf (FILE, "\tcall %s\n", name+1);  /*  skip '&'  */	\
	  machopic_validate_stub_or_non_lazy_ptr (name);		\
	}								\
      else fprintf (FILE, "\tcall mcount\n");				\
    } while (0)

/* APPLE LOCAL CW asm blocks */
extern int flag_iasm_blocks;
/* APPLE LOCAL begin fix-and-continue x86 */
#undef SUBTARGET_OVERRIDE_OPTIONS
#define SUBTARGET_OVERRIDE_OPTIONS				\
  do {								\
    /* APPLE LOCAL begin ARM 5683689 */				\
    if (!darwin_macosx_version_min				\
	&& !darwin_iphoneos_version_min)			\
      darwin_macosx_version_min = "10.1";			\
    /* APPLE LOCAL end ARM 5683689 */				\
    /* APPLE LOCAL begin CW asm blocks */			\
    if (flag_iasm_blocks)					\
      flag_ms_asms = 1;						\
    /* APPLE LOCAL end CW asm blocks */				\
    /* APPLE LOCAL begin constant cfstrings */			\
    if (darwin_constant_cfstrings < 0)				\
      darwin_constant_cfstrings = 1;				\
    /* APPLE LOCAL end constant cfstrings */			\
    if (TARGET_64BIT)						\
      {								\
	if (MACHO_DYNAMIC_NO_PIC_P)				\
	  target_flags &= ~MASK_MACHO_DYNAMIC_NO_PIC;		\
      }								\
    /* APPLE LOCAL begin fix this for mainline */		\
    /* For mainline this needs to be fixed to have every	\
       cpu architecture feature as an isa mask.	 Every		\
       cpu we've shipped supports all of these features.	\
       This includes all ix86_arch cpu features currently	\
       defined except x86_cmove which is turned on for		\
       TARGET_SSE anyhow.  */					\
    if (!ix86_arch_string)					\
      {								\
	x86_cmpxchg = ~(0);					\
	x86_cmpxchg8b = ~(0);					\
	x86_cmpxchg16b = ~(0);					\
	x86_xadd = ~(0);					\
	x86_bswap = ~(0);					\
      }								\
    /* APPLE LOCAL end fix this for mainline */			\
  } while (0)

/* True, iff we're generating fast turn around debugging code.  When
   true, we arrange for function prologues to start with 6 nops so
   that gdb may insert code to redirect them, and for data to be
   accessed indirectly.  The runtime uses this indirection to forward
   references for data to the original instance of that data.  */

#define TARGET_FIX_AND_CONTINUE (darwin_fix_and_continue)
/* APPLE LOCAL end fix-and-continue x86 */

#define C_COMMON_OVERRIDE_OPTIONS					\
  do {									\
    SUBTARGET_C_COMMON_OVERRIDE_OPTIONS;				\
  } while (0)

/* APPLE LOCAL begin mainline 4.3 2006-10-31 4370143 */
/* Removed PREFERRED_DEBUGGING_TYPE */
/* APPLE LOCAL end mainline 4.3 2006-10-31 4370143 */

/* Darwin uses the standard DWARF register numbers but the default
   register numbers for STABS.  Fortunately for 64-bit code the
   default and the standard are the same.  */
#undef DBX_REGISTER_NUMBER
#define DBX_REGISTER_NUMBER(n) 					\
  (TARGET_64BIT ? dbx64_register_map[n]				\
   : write_symbols == DWARF2_DEBUG ? svr4_dbx_register_map[n]	\
   : dbx_register_map[n])

/* Unfortunately, the 32-bit EH information also doesn't use the standard
   DWARF register numbers.  */
#define DWARF2_FRAME_REG_OUT(n, for_eh)					\
  (! (for_eh) || write_symbols != DWARF2_DEBUG || TARGET_64BIT ? (n)	\
   : (n) == 5 ? 4							\
   : (n) == 4 ? 5							\
   : (n) >= 11 && (n) <= 18 ? (n) + 1					\
   : (n))

/* APPLE LOCAL begin 4457939 stack alignment mishandled */
/* <rdar://problem/4471596> stack alignment is not handled properly

   Please remove this entire a**le local when addressing this
   Radar.  */
extern void ix86_darwin_init_expanders (void);
#define INIT_EXPANDERS (ix86_darwin_init_expanders ())
/* APPLE LOCAL end 4457939 stack alignment mishandled */


/* APPLE LOCAL begin CW asm blocks */
#define IASM_VALID_PIC(DECL, E)						\
  do {									\
    if (! TARGET_64BIT							\
	&& E->as_immediate && ! MACHO_DYNAMIC_NO_PIC_P && flag_pic)	\
      warning (0, "non-pic addressing form not suitible for pic code");	\
  } while (0)
#define IASM_RIP(X) do { if (TARGET_64BIT) strcat (X, "(%%rip)"); } while (0)
/* APPLE LOCAL end cw asm blocks */

/* APPLE LOCAL KEXT */
#define TARGET_SUPPORTS_KEXTABI1 (! TARGET_64BIT)


#undef REGISTER_TARGET_PRAGMAS
#define REGISTER_TARGET_PRAGMAS() DARWIN_REGISTER_TARGET_PRAGMAS()

#undef TARGET_SET_DEFAULT_TYPE_ATTRIBUTES
#define TARGET_SET_DEFAULT_TYPE_ATTRIBUTES darwin_set_default_type_attributes

/* APPLE LOCAL begin mainline */
/* For 64-bit, we need to add 4 because @GOTPCREL is relative to the
   end of the instruction, but without the 4 we'd only have the right
   address for the start of the instruction.  */
#undef ASM_MAYBE_OUTPUT_ENCODED_ADDR_RTX
#define ASM_MAYBE_OUTPUT_ENCODED_ADDR_RTX(FILE, ENCODING, SIZE, ADDR, DONE) \
  if (TARGET_64BIT)							\
    {									\
      if ((SIZE) == 4 && ((ENCODING) & 0x70) == DW_EH_PE_pcrel)		\
        {								\
	  fputs (ASM_LONG, FILE);					\
	  assemble_name (FILE, XSTR (ADDR, 0));				\
	  fputs ("+4@GOTPCREL", FILE);					\
	  goto DONE;							\
        }								\
    }									\
  else									\
    {									\
      if (ENCODING == ASM_PREFERRED_EH_DATA_FORMAT (2, 1))		\
        {								\
          darwin_non_lazy_pcrel (FILE, ADDR);				\
          goto DONE;							\
        }								\
    }
/* APPLE LOCAL end mainline */
/* APPLE LOCAL begin track initialization status 4964532  */
/* APPLE LOCAL begin ARM 5683689 */
#undef  TARGET_DWARF_UNINIT_VARS
#define TARGET_DWARF_UNINIT_VARS   \
  (darwin_iphoneos_version_min ||	   \
   strverscmp (darwin_macosx_version_min, "10.4") >= 0)
/* APPLE LOCAL end ARM 5683689 */
/* APPLE LOCAL end track initialization status 4964532  */

/* This needs to move since i386 uses the first flag and other flags are
   used in Mach-O.  */
#undef MACHO_SYMBOL_FLAG_VARIABLE
#define MACHO_SYMBOL_FLAG_VARIABLE ((SYMBOL_FLAG_MACH_DEP) << 3)
