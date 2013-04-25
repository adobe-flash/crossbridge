/* Definitions of target machine for GCC for IA-32.
   Copyright (C) 1988, 1992, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
   2001, 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

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

/* The purpose of this file is to define the characteristics of the i386,
   independent of assembler syntax or operating system.

   Three other files build on this one to describe a specific assembler syntax:
   bsd386.h, att386.h, and sun386.h.

   The actual tm.h file for a particular system should include
   this file, and then the file for the appropriate assembler syntax.

   Many macros that specify assembler syntax are omitted entirely from
   this file because they really belong in the files for particular
   assemblers.  These include RP, IP, LPREFIX, PUT_OP_SIZE, USE_STAR,
   ADDR_BEG, ADDR_END, PRINT_IREG, PRINT_SCALE, PRINT_B_I_S, and many
   that start with ASM_ or end in ASM_OP.  */

/* LLVM LOCAL begin */
#ifdef ENABLE_LLVM

/* Add general target specific stuff */
#include "llvm-i386-target.h"

/* Register class used for passing given 64bit part of the argument.
   These represent classes as documented by the PS ABI, with the exception
   of SSESF, SSEDF classes, that are basically SSE class, just gcc will
   use SF or DFmode move instead of DImode to avoid reformatting penalties.

   Similarly we play games with INTEGERSI_CLASS to use cheaper SImode moves
   whenever possible (upper half does contain padding).
 */
enum x86_64_reg_class
  {
    X86_64_NO_CLASS,
    X86_64_INTEGER_CLASS,
    X86_64_INTEGERSI_CLASS,
    X86_64_SSE_CLASS,
    X86_64_SSESF_CLASS,
    X86_64_SSEDF_CLASS,
    X86_64_SSEUP_CLASS,
    X86_64_X87_CLASS,
    X86_64_X87UP_CLASS,
    X86_64_COMPLEX_X87_CLASS,
    X86_64_MEMORY_CLASS,
    X86_64_POINTER_CLASS
  };

#endif /* ENABLE_LLVM */
/* LLVM LOCAL end */

/* Define the specific costs for a given cpu */

struct processor_costs {
  const int add;		/* cost of an add instruction */
  const int lea;		/* cost of a lea instruction */
  const int shift_var;		/* variable shift costs */
  const int shift_const;	/* constant shift costs */
  const int mult_init[5];	/* cost of starting a multiply
				   in QImode, HImode, SImode, DImode, TImode*/
  const int mult_bit;		/* cost of multiply per each bit set */
  const int divide[5];		/* cost of a divide/mod
				   in QImode, HImode, SImode, DImode, TImode*/
  int movsx;			/* The cost of movsx operation.  */
  int movzx;			/* The cost of movzx operation.  */
  const int large_insn;		/* insns larger than this cost more */
  const int move_ratio;		/* The threshold of number of scalar
				   memory-to-memory move insns.  */
  const int movzbl_load;	/* cost of loading using movzbl */
  const int int_load[3];	/* cost of loading integer registers
				   in QImode, HImode and SImode relative
				   to reg-reg move (2).  */
  const int int_store[3];	/* cost of storing integer register
				   in QImode, HImode and SImode */
  const int fp_move;		/* cost of reg,reg fld/fst */
  const int fp_load[3];		/* cost of loading FP register
				   in SFmode, DFmode and XFmode */
  const int fp_store[3];	/* cost of storing FP register
				   in SFmode, DFmode and XFmode */
  const int mmx_move;		/* cost of moving MMX register.  */
  const int mmx_load[2];	/* cost of loading MMX register
				   in SImode and DImode */
  const int mmx_store[2];	/* cost of storing MMX register
				   in SImode and DImode */
  const int sse_move;		/* cost of moving SSE register.  */
  const int sse_load[3];	/* cost of loading SSE register
				   in SImode, DImode and TImode*/
  const int sse_store[3];	/* cost of storing SSE register
				   in SImode, DImode and TImode*/
  const int mmxsse_to_integer;	/* cost of moving mmxsse register to
				   integer and vice versa.  */
  const int prefetch_block;	/* bytes moved to cache for prefetch.  */
  const int simultaneous_prefetches; /* number of parallel prefetch
				   operations.  */
  const int branch_cost;	/* Default value for BRANCH_COST.  */
  const int fadd;		/* cost of FADD and FSUB instructions.  */
  const int fmul;		/* cost of FMUL instruction.  */
  const int fdiv;		/* cost of FDIV instruction.  */
  const int fabs;		/* cost of FABS instruction.  */
  const int fchs;		/* cost of FCHS instruction.  */
  const int fsqrt;		/* cost of FSQRT instruction.  */
};

extern const struct processor_costs *ix86_cost;

/* Macros used in the machine description to test the flags.  */

/* configure can arrange to make this 2, to force a 486.  */

#ifndef TARGET_CPU_DEFAULT
#define TARGET_CPU_DEFAULT TARGET_CPU_DEFAULT_generic
#endif

#ifndef TARGET_FPMATH_DEFAULT
#define TARGET_FPMATH_DEFAULT \
  (TARGET_64BIT && TARGET_SSE ? FPMATH_SSE : FPMATH_387)
#endif

#define TARGET_FLOAT_RETURNS_IN_80387 TARGET_FLOAT_RETURNS
/* APPLE LOCAL begin AT&T-style stub 4164563 */
#define MACHOPIC_NL_SYMBOL_PTR_SECTION ".section __IMPORT,__pointers,non_lazy_symbol_pointers"
/* APPLE LOCAL end AT&T-style stub 4164563 */

/* 64bit Sledgehammer mode.  For libgcc2 we make sure this is a
   compile-time constant.  */
#ifdef IN_LIBGCC2
#undef TARGET_64BIT
#ifdef __x86_64__
#define TARGET_64BIT 1
#else
#define TARGET_64BIT 0
#endif
#else
#ifndef TARGET_BI_ARCH
#undef TARGET_64BIT
#if TARGET_64BIT_DEFAULT
#define TARGET_64BIT 1
#else
#define TARGET_64BIT 0
#endif
#endif
#endif

#define HAS_LONG_COND_BRANCH 1
#define HAS_LONG_UNCOND_BRANCH 1

#define TARGET_386 (ix86_tune == PROCESSOR_I386)
#define TARGET_486 (ix86_tune == PROCESSOR_I486)
#define TARGET_PENTIUM (ix86_tune == PROCESSOR_PENTIUM)
#define TARGET_PENTIUMPRO (ix86_tune == PROCESSOR_PENTIUMPRO)
#define TARGET_K6 (ix86_tune == PROCESSOR_K6)
#define TARGET_ATHLON (ix86_tune == PROCESSOR_ATHLON)
#define TARGET_PENTIUM4 (ix86_tune == PROCESSOR_PENTIUM4)
#define TARGET_K8 (ix86_tune == PROCESSOR_K8)
#define TARGET_ATHLON_K8 (TARGET_K8 || TARGET_ATHLON)
#define TARGET_NOCONA (ix86_tune == PROCESSOR_NOCONA)
/* APPLE LOCAL mainline */
#define TARGET_CORE2 (ix86_tune == PROCESSOR_CORE2)
#define TARGET_GENERIC32 (ix86_tune == PROCESSOR_GENERIC32)
#define TARGET_GENERIC64 (ix86_tune == PROCESSOR_GENERIC64)
#define TARGET_GENERIC (TARGET_GENERIC32 || TARGET_GENERIC64)

#define TUNEMASK (1 << ix86_tune)
extern const int x86_use_leave, x86_push_memory, x86_zero_extend_with_and;
extern const int x86_use_bit_test, x86_cmove, x86_deep_branch;
extern const int x86_branch_hints, x86_unroll_strlen;
extern const int x86_double_with_add, x86_partial_reg_stall, x86_movx;
extern const int x86_use_himode_fiop, x86_use_simode_fiop;
extern const int x86_use_mov0, x86_use_cltd, x86_read_modify_write;
extern const int x86_read_modify, x86_split_long_moves;
extern const int x86_promote_QImode, x86_single_stringop, x86_fast_prefix;
extern const int x86_himode_math, x86_qimode_math, x86_promote_qi_regs;
extern const int x86_promote_hi_regs, x86_integer_DFmode_moves;
extern const int x86_add_esp_4, x86_add_esp_8, x86_sub_esp_4, x86_sub_esp_8;
extern const int x86_partial_reg_dependency, x86_memory_mismatch_stall;
extern const int x86_accumulate_outgoing_args, x86_prologue_using_move;
extern const int x86_epilogue_using_move, x86_decompose_lea;
extern const int x86_arch_always_fancy_math_387, x86_shift1;
extern const int x86_sse_partial_reg_dependency, x86_sse_split_regs;
extern const int x86_sse_typeless_stores, x86_sse_load0_by_pxor;
extern const int x86_use_ffreep;
extern const int x86_inter_unit_moves, x86_schedule;
extern const int x86_use_bt;
/* APPLE LOCAL override options */
extern int x86_cmpxchg, x86_cmpxchg8b, x86_cmpxchg16b, x86_xadd;
extern const int x86_use_incdec;
extern const int x86_pad_returns;
/* APPLE LOCAL mainline bswap/local override options */
extern int x86_bswap;
extern const int x86_partial_flag_reg_stall;
extern int x86_prefetch_sse;

#define TARGET_USE_LEAVE (x86_use_leave & TUNEMASK)
#define TARGET_PUSH_MEMORY (x86_push_memory & TUNEMASK)
#define TARGET_ZERO_EXTEND_WITH_AND (x86_zero_extend_with_and & TUNEMASK)
#define TARGET_USE_BIT_TEST (x86_use_bit_test & TUNEMASK)
#define TARGET_UNROLL_STRLEN (x86_unroll_strlen & TUNEMASK)
/* For sane SSE instruction set generation we need fcomi instruction.  It is
   safe to enable all CMOVE instructions.  */
#define TARGET_CMOVE ((x86_cmove & (1 << ix86_arch)) || TARGET_SSE)
#define TARGET_FISTTP (TARGET_SSE3 && TARGET_80387)
#define TARGET_DEEP_BRANCH_PREDICTION (x86_deep_branch & TUNEMASK)
#define TARGET_BRANCH_PREDICTION_HINTS (x86_branch_hints & TUNEMASK)
#define TARGET_DOUBLE_WITH_ADD (x86_double_with_add & TUNEMASK)
#define TARGET_USE_SAHF ((x86_use_sahf & TUNEMASK) && !TARGET_64BIT)
#define TARGET_MOVX (x86_movx & TUNEMASK)
#define TARGET_PARTIAL_REG_STALL (x86_partial_reg_stall & TUNEMASK)
#define TARGET_PARTIAL_FLAG_REG_STALL (x86_partial_flag_reg_stall & TUNEMASK)
#define TARGET_USE_HIMODE_FIOP (x86_use_himode_fiop & TUNEMASK)
#define TARGET_USE_SIMODE_FIOP (x86_use_simode_fiop & TUNEMASK)
#define TARGET_USE_MOV0 (x86_use_mov0 & TUNEMASK)
#define TARGET_USE_CLTD (x86_use_cltd & TUNEMASK)
#define TARGET_SPLIT_LONG_MOVES (x86_split_long_moves & TUNEMASK)
#define TARGET_READ_MODIFY_WRITE (x86_read_modify_write & TUNEMASK)
#define TARGET_READ_MODIFY (x86_read_modify & TUNEMASK)
#define TARGET_PROMOTE_QImode (x86_promote_QImode & TUNEMASK)
#define TARGET_FAST_PREFIX (x86_fast_prefix & TUNEMASK)
#define TARGET_SINGLE_STRINGOP (x86_single_stringop & TUNEMASK)
#define TARGET_QIMODE_MATH (x86_qimode_math & TUNEMASK)
#define TARGET_HIMODE_MATH (x86_himode_math & TUNEMASK)
#define TARGET_PROMOTE_QI_REGS (x86_promote_qi_regs & TUNEMASK)
#define TARGET_PROMOTE_HI_REGS (x86_promote_hi_regs & TUNEMASK)
#define TARGET_ADD_ESP_4 (x86_add_esp_4 & TUNEMASK)
#define TARGET_ADD_ESP_8 (x86_add_esp_8 & TUNEMASK)
#define TARGET_SUB_ESP_4 (x86_sub_esp_4 & TUNEMASK)
#define TARGET_SUB_ESP_8 (x86_sub_esp_8 & TUNEMASK)
#define TARGET_INTEGER_DFMODE_MOVES (x86_integer_DFmode_moves & TUNEMASK)
#define TARGET_PARTIAL_REG_DEPENDENCY (x86_partial_reg_dependency & TUNEMASK)
#define TARGET_SSE_PARTIAL_REG_DEPENDENCY \
				      (x86_sse_partial_reg_dependency & TUNEMASK)
#define TARGET_SSE_SPLIT_REGS (x86_sse_split_regs & TUNEMASK)
#define TARGET_SSE_TYPELESS_STORES (x86_sse_typeless_stores & TUNEMASK)
#define TARGET_SSE_LOAD0_BY_PXOR (x86_sse_load0_by_pxor & TUNEMASK)
#define TARGET_MEMORY_MISMATCH_STALL (x86_memory_mismatch_stall & TUNEMASK)
#define TARGET_PROLOGUE_USING_MOVE (x86_prologue_using_move & TUNEMASK)
#define TARGET_EPILOGUE_USING_MOVE (x86_epilogue_using_move & TUNEMASK)
#define TARGET_PREFETCH_SSE (x86_prefetch_sse)
#define TARGET_SHIFT1 (x86_shift1 & TUNEMASK)
#define TARGET_USE_FFREEP (x86_use_ffreep & TUNEMASK)
#define TARGET_REP_MOVL_OPTIMAL (x86_rep_movl_optimal & TUNEMASK)
#define TARGET_INTER_UNIT_MOVES (x86_inter_unit_moves & TUNEMASK)
#define TARGET_FOUR_JUMP_LIMIT (x86_four_jump_limit & TUNEMASK)
#define TARGET_SCHEDULE (x86_schedule & TUNEMASK)
#define TARGET_USE_BT (x86_use_bt & TUNEMASK)
#define TARGET_USE_INCDEC (x86_use_incdec & TUNEMASK)
#define TARGET_PAD_RETURNS (x86_pad_returns & TUNEMASK)

#define ASSEMBLER_DIALECT (ix86_asm_dialect)

#define TARGET_SSE_MATH ((ix86_fpmath & FPMATH_SSE) != 0)
#define TARGET_MIX_SSE_I387 ((ix86_fpmath & FPMATH_SSE) \
			     && (ix86_fpmath & FPMATH_387))
/* APPLE LOCAL mainline */
#define TARGET_SSSE3 ((target_flags & MASK_SSSE3) != 0)

#define TARGET_GNU_TLS (ix86_tls_dialect == TLS_DIALECT_GNU)
#define TARGET_GNU2_TLS (ix86_tls_dialect == TLS_DIALECT_GNU2)
#define TARGET_ANY_GNU_TLS (TARGET_GNU_TLS || TARGET_GNU2_TLS)
#define TARGET_SUN_TLS (ix86_tls_dialect == TLS_DIALECT_SUN)

#define TARGET_CMPXCHG (x86_cmpxchg & (1 << ix86_arch))
#define TARGET_CMPXCHG8B (x86_cmpxchg8b & (1 << ix86_arch))
#define TARGET_CMPXCHG16B (x86_cmpxchg16b & (1 << ix86_arch))
#define TARGET_XADD (x86_xadd & (1 << ix86_arch))
/* APPLE LOCAL mainline bswap */
#define TARGET_BSWAP (x86_bswap & (1 << ix86_arch))

#ifndef TARGET_64BIT_DEFAULT
#define TARGET_64BIT_DEFAULT 0
#endif
#ifndef TARGET_TLS_DIRECT_SEG_REFS_DEFAULT
#define TARGET_TLS_DIRECT_SEG_REFS_DEFAULT 0
#endif

/* Once GDB has been enhanced to deal with functions without frame
   pointers, we can change this to allow for elimination of
   the frame pointer in leaf functions.  */
#define TARGET_DEFAULT 0
/* APPLE LOCAL begin mainline */
/* Extra bits to force.  */
#define TARGET_SUBTARGET32_DEFAULT 0

#define TARGET_SUBTARGET64_DEFAULT 0
/* APPLE LOCAL end mainline */

/* This is not really a target flag, but is done this way so that
   it's analogous to similar code for Mach-O on PowerPC.  darwin.h
   redefines this to 1.  */
#define TARGET_MACHO 0
/* LLVM LOCAL begin mainline */
/* Likewise, for the Windows 64-bit ABI.  */
#ifndef TARGET_64BIT_MS_ABI
#define TARGET_64BIT_MS_ABI 0
#endif
/* LLVM LOCAL end mainline */
/* APPLE LOCAL begin mach-o cleanup */
#define MACHOPIC_INDIRECT 0
#define MACHOPIC_PURE 0
/* APPLE LOCAL end mach-o cleanup */

/* Subtargets may reset this to 1 in order to enable 96-bit long double
   with the rounding mode forced to 53 bits.  */
#define TARGET_96_ROUND_53_LONG_DOUBLE 0

/* Sometimes certain combinations of command options do not make
   sense on a particular target machine.  You can define a macro
   `OVERRIDE_OPTIONS' to take account of this.  This macro, if
   defined, is executed once just after all the command options have
   been parsed.

   Don't use this macro to turn on various extra optimizations for
   `-O'.  That is what `OPTIMIZATION_OPTIONS' is for.  */

#define OVERRIDE_OPTIONS override_options ()

/* Define this to change the optimizations performed by default.  */
#define OPTIMIZATION_OPTIONS(LEVEL, SIZE) \
  optimization_options ((LEVEL), (SIZE))

/* -march=native handling only makes sense with compiler running on
   an x86 or x86_64 chip.  If changing this condition, also change
   the condition in driver-i386.c.  */
#if defined(__i386__) || defined(__x86_64__)
/* In driver-i386.c.  */
extern const char *host_detect_local_cpu (int argc, const char **argv);
#define EXTRA_SPEC_FUNCTIONS \
  { "local_cpu_detect", host_detect_local_cpu },
#define HAVE_LOCAL_CPU_DETECT
#endif

/* Support for configure-time defaults of some command line options.
   The order here is important so that -march doesn't squash the
   tune or cpu values.  */
#define OPTION_DEFAULT_SPECS \
  {"tune", "%{!mtune=*:%{!mcpu=*:%{!march=*:-mtune=%(VALUE)}}}" }, \
  {"cpu", "%{!mtune=*:%{!mcpu=*:%{!march=*:-mtune=%(VALUE)}}}" }, \
  {"arch", "%{!march=*:-march=%(VALUE)}"}

/* Specs for the compiler proper */

#ifndef CC1_CPU_SPEC
#define CC1_CPU_SPEC_1 "\
%{!mtune*: \
%{m386:mtune=i386 \
%n`-m386' is deprecated. Use `-march=i386' or `-mtune=i386' instead.\n} \
%{m486:-mtune=i486 \
%n`-m486' is deprecated. Use `-march=i486' or `-mtune=i486' instead.\n} \
%{mpentium:-mtune=pentium \
%n`-mpentium' is deprecated. Use `-march=pentium' or `-mtune=pentium' instead.\n} \
%{mpentiumpro:-mtune=pentiumpro \
%n`-mpentiumpro' is deprecated. Use `-march=pentiumpro' or `-mtune=pentiumpro' instead.\n} \
%{mcpu=*:-mtune=%* \
%n`-mcpu=' is deprecated. Use `-mtune=' or '-march=' instead.\n}} \
%<mcpu=* \
%{mintel-syntax:-masm=intel \
%n`-mintel-syntax' is deprecated. Use `-masm=intel' instead.\n} \
%{mno-intel-syntax:-masm=att \
%n`-mno-intel-syntax' is deprecated. Use `-masm=att' instead.\n}"

#ifndef HAVE_LOCAL_CPU_DETECT
#define CC1_CPU_SPEC CC1_CPU_SPEC_1
#else
#define CC1_CPU_SPEC CC1_CPU_SPEC_1 \
"%{march=native:%<march=native %:local_cpu_detect(arch) \
  %{!mtune=*:%<mtune=native %:local_cpu_detect(tune)}} \
%{mtune=native:%<mtune=native %:local_cpu_detect(tune)}"
#endif
#endif

/* LLVM LOCAL begin PR879 workaround */
#ifdef ENABLE_LLVM
#define LLVM_CPP_BUILTINS builtin_define("__NO_MATH_INLINES");
#else
#define LLVM_CPP_BUILTINS
#endif
/* LLVM LOCAL end PR879 workaround */

/* Target CPU builtins.  */
#define TARGET_CPU_CPP_BUILTINS()				\
  do								\
    {								\
      size_t arch_len = strlen (ix86_arch_string);		\
      size_t tune_len = strlen (ix86_tune_string);		\
      int last_arch_char = ix86_arch_string[arch_len - 1];	\
      int last_tune_char = ix86_tune_string[tune_len - 1];		\
								\
      if (TARGET_64BIT)						\
	{							\
	  builtin_assert ("cpu=x86_64");			\
	  builtin_assert ("machine=x86_64");			\
	  builtin_define ("__amd64");				\
	  builtin_define ("__amd64__");				\
	  builtin_define ("__x86_64");				\
	  builtin_define ("__x86_64__");			\
	}							\
      else							\
	{							\
	  builtin_assert ("cpu=i386");				\
	  builtin_assert ("machine=i386");			\
	  builtin_define_std ("i386");				\
	}							\
								\
      /* Built-ins based on -mtune= (or -march= if no		\
	 -mtune= given).  */					\
      if (TARGET_386)						\
	builtin_define ("__tune_i386__");			\
      else if (TARGET_486)					\
	builtin_define ("__tune_i486__");			\
      else if (TARGET_PENTIUM)					\
	{							\
	  builtin_define ("__tune_i586__");			\
	  builtin_define ("__tune_pentium__");			\
	  if (last_tune_char == 'x')				\
	    builtin_define ("__tune_pentium_mmx__");		\
	}							\
      else if (TARGET_PENTIUMPRO)				\
	{							\
	  builtin_define ("__tune_i686__");			\
	  builtin_define ("__tune_pentiumpro__");		\
	  switch (last_tune_char)				\
	    {							\
	    case '3':						\
	      builtin_define ("__tune_pentium3__");		\
	      /* FALLTHRU */					\
	    case '2':						\
	      builtin_define ("__tune_pentium2__");		\
	      break;						\
	    }							\
	}							\
      else if (TARGET_K6)					\
	{							\
	  builtin_define ("__tune_k6__");			\
	  if (last_tune_char == '2')				\
	    builtin_define ("__tune_k6_2__");			\
	  else if (last_tune_char == '3')			\
	    builtin_define ("__tune_k6_3__");			\
	}							\
      else if (TARGET_ATHLON)					\
	{							\
	  builtin_define ("__tune_athlon__");			\
	  /* Only plain "athlon" lacks SSE.  */			\
	  if (last_tune_char != 'n')				\
	    builtin_define ("__tune_athlon_sse__");		\
	}							\
      else if (TARGET_K8)					\
	builtin_define ("__tune_k8__");				\
      else if (TARGET_PENTIUM4)					\
	builtin_define ("__tune_pentium4__");			\
      else if (TARGET_NOCONA)					\
	builtin_define ("__tune_nocona__");			\
      /* APPLE LOCAL begin mainline */				\
      else if (TARGET_CORE2)					\
	builtin_define ("__tune_core2__");			\
      /* APPLE LOCAL end mainline */				\
								\
      if (TARGET_MMX)						\
	builtin_define ("__MMX__");				\
      if (TARGET_3DNOW)						\
	builtin_define ("__3dNOW__");				\
      if (TARGET_3DNOW_A)					\
	builtin_define ("__3dNOW_A__");				\
      if (TARGET_SSE)						\
	builtin_define ("__SSE__");				\
      if (TARGET_SSE2)						\
	builtin_define ("__SSE2__");				\
      if (TARGET_SSE3)						\
	builtin_define ("__SSE3__");				\
      /* APPLE LOCAL begin mainline */				\
      if (TARGET_SSSE3)						\
	builtin_define ("__SSSE3__");				\
      /* APPLE LOCAL end mainline */				\
      /* APPLE LOCAL begin 5612787 mainline sse4 */			\
      if (TARGET_SSE4_1)					\
	builtin_define ("__SSE4_1__");				\
      if (TARGET_SSE4_2)					\
	builtin_define ("__SSE4_2__");				\
      if (TARGET_SSE4A)						\
 	builtin_define ("__SSE4A__");		                \
      /* APPLE LOCAL end 5612787 mainline sse4 */			\
      if (TARGET_SSE_MATH && TARGET_SSE)			\
	builtin_define ("__SSE_MATH__");			\
      if (TARGET_SSE_MATH && TARGET_SSE2)			\
	builtin_define ("__SSE2_MATH__");			\
								\
      /* Built-ins based on -march=.  */			\
      if (ix86_arch == PROCESSOR_I486)				\
	{							\
	  builtin_define ("__i486");				\
	  builtin_define ("__i486__");				\
	}							\
      else if (ix86_arch == PROCESSOR_PENTIUM)			\
	{							\
	  builtin_define ("__i586");				\
	  builtin_define ("__i586__");				\
	  builtin_define ("__pentium");				\
	  builtin_define ("__pentium__");			\
	  if (last_arch_char == 'x')				\
	    builtin_define ("__pentium_mmx__");			\
	}							\
      else if (ix86_arch == PROCESSOR_PENTIUMPRO)		\
	{							\
	  builtin_define ("__i686");				\
	  builtin_define ("__i686__");				\
	  builtin_define ("__pentiumpro");			\
	  builtin_define ("__pentiumpro__");			\
	}							\
      else if (ix86_arch == PROCESSOR_K6)			\
	{							\
								\
	  builtin_define ("__k6");				\
	  builtin_define ("__k6__");				\
	  if (last_arch_char == '2')				\
	    builtin_define ("__k6_2__");			\
	  else if (last_arch_char == '3')			\
	    builtin_define ("__k6_3__");			\
	}							\
      else if (ix86_arch == PROCESSOR_ATHLON)			\
	{							\
	  builtin_define ("__athlon");				\
	  builtin_define ("__athlon__");			\
	  /* Only plain "athlon" lacks SSE.  */			\
	  if (last_arch_char != 'n')				\
	    builtin_define ("__athlon_sse__");			\
	}							\
      else if (ix86_arch == PROCESSOR_K8)			\
	{							\
	  builtin_define ("__k8");				\
	  builtin_define ("__k8__");				\
	}							\
      else if (ix86_arch == PROCESSOR_PENTIUM4)			\
	{							\
	  builtin_define ("__pentium4");			\
	  builtin_define ("__pentium4__");			\
	}							\
      else if (ix86_arch == PROCESSOR_NOCONA)			\
	{							\
	  builtin_define ("__nocona");				\
	  builtin_define ("__nocona__");			\
	}							\
      /* APPLE LOCAL begin mainline */				\
      else if (ix86_arch == PROCESSOR_CORE2)			\
	{							\
	  builtin_define ("__core2");				\
	  builtin_define ("__core2__");				\
	}							\
      /* APPLE LOCAL end mainline */				\
      /* LLVM LOCAL begin PR879 workaround */                   \
      LLVM_CPP_BUILTINS                                         \
      /* LLVM LOCAL end PR879 workaround */                     \
    }								\
  while (0)

#define TARGET_CPU_DEFAULT_i386 0
#define TARGET_CPU_DEFAULT_i486 1
#define TARGET_CPU_DEFAULT_pentium 2
#define TARGET_CPU_DEFAULT_pentium_mmx 3
#define TARGET_CPU_DEFAULT_pentiumpro 4
#define TARGET_CPU_DEFAULT_pentium2 5
#define TARGET_CPU_DEFAULT_pentium3 6
#define TARGET_CPU_DEFAULT_pentium4 7
#define TARGET_CPU_DEFAULT_k6 8
#define TARGET_CPU_DEFAULT_k6_2 9
#define TARGET_CPU_DEFAULT_k6_3 10
#define TARGET_CPU_DEFAULT_athlon 11
#define TARGET_CPU_DEFAULT_athlon_sse 12
#define TARGET_CPU_DEFAULT_k8 13
#define TARGET_CPU_DEFAULT_pentium_m 14
#define TARGET_CPU_DEFAULT_prescott 15
#define TARGET_CPU_DEFAULT_nocona 16
#define TARGET_CPU_DEFAULT_generic 17
/* APPLE LOCAL mainline */
#define TARGET_CPU_DEFAULT_core2 18
/* APPLE LOCAL begin mainline */
#define TARGET_CPU_DEFAULT_NAMES {"i386", "i486", "pentium", "pentium-mmx",\
				  "pentiumpro", "pentium2", "pentium3", \
				  "pentium4", "k6", "k6-2", "k6-3",\
				  "athlon", "athlon-4", "k8", \
				  "pentium-m", "prescott", "nocona", \
				  "generic", "core2" }
/* APPLE LOCAL end mainline */

#ifndef CC1_SPEC
#define CC1_SPEC "%(cc1_cpu) "
#endif

/* This macro defines names of additional specifications to put in the
   specs that can be used in various specifications like CC1_SPEC.  Its
   definition is an initializer with a subgrouping for each command option.

   Each subgrouping contains a string constant, that defines the
   specification name, and a string constant that used by the GCC driver
   program.

   Do not define this macro if it does not need to do anything.  */

#ifndef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS
#endif

#define EXTRA_SPECS							\
  { "cc1_cpu",  CC1_CPU_SPEC },						\
  SUBTARGET_EXTRA_SPECS

/* target machine storage layout */

#define LONG_DOUBLE_TYPE_SIZE 80

/* Set the value of FLT_EVAL_METHOD in float.h.  When using only the
   FPU, assume that the fpcw is set to extended precision; when using
   only SSE, rounding is correct; when using both SSE and the FPU,
   the rounding precision is indeterminate, since either may be chosen
   apparently at random.  */
#define TARGET_FLT_EVAL_METHOD \
  (TARGET_MIX_SSE_I387 ? -1 : TARGET_SSE_MATH ? 0 : 2)

#define SHORT_TYPE_SIZE 16
#define INT_TYPE_SIZE 32
#define FLOAT_TYPE_SIZE 32
#define LONG_TYPE_SIZE BITS_PER_WORD
#define DOUBLE_TYPE_SIZE 64
#define LONG_LONG_TYPE_SIZE 64

#if defined (TARGET_BI_ARCH) || TARGET_64BIT_DEFAULT
#define MAX_BITS_PER_WORD 64
#else
#define MAX_BITS_PER_WORD 32
#endif

/* Define this if most significant byte of a word is the lowest numbered.  */
/* That is true on the 80386.  */

#define BITS_BIG_ENDIAN 0

/* Define this if most significant byte of a word is the lowest numbered.  */
/* That is not true on the 80386.  */
#define BYTES_BIG_ENDIAN 0

/* Define this if most significant word of a multiword number is the lowest
   numbered.  */
/* Not true for 80386 */
#define WORDS_BIG_ENDIAN 0

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD (TARGET_64BIT ? 8 : 4)
#ifdef IN_LIBGCC2
#define MIN_UNITS_PER_WORD	(TARGET_64BIT ? 8 : 4)
#else
#define MIN_UNITS_PER_WORD	4
#endif

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY BITS_PER_WORD

/* Boundary (in *bits*) on which stack pointer should be aligned.  */
/* APPLE LOCAL begin compiler should obey -mpreferred-stack-boundary (radar 3232990) */
/* prefer * #define STACK_BOUNDARY ((ix86_preferred_stack_boundary > 128) ? 128 : ix86_preferred_stack_boundary) */
/*  We're going to extremes to yield a result of indeterminite
    signedness here; this macro will be expanded in signed and
    unsigned contexts, and mixed signedness induces fatal
    warnings.  Radar 3941684.  */
#define STACK_BOUNDARY ((ix86_preferred_stack_boundary >=  128) ? 128 : \
			(ix86_preferred_stack_boundary == 64) ? 64 : 32)
/* APPLE LOCAL end compiler should obey -mpreferred-stack-boundary (radar 3232990) */

/* Boundary (in *bits*) on which the stack pointer prefers to be
   aligned; the compiler cannot rely on having this alignment.  */
#define PREFERRED_STACK_BOUNDARY ix86_preferred_stack_boundary

/* APPLE LOCAL begin radar 4216496, 4229407, 4120689, 4095567 */
#define SAVE_PREFERRED_STACK_BOUNDARY ix86_save_preferred_stack_boundary
/* APPLE LOCAL end radar 4216496, 4229407, 4120689, 4095567 */

/* As of July 2001, many runtimes do not align the stack properly when
   entering main.  This causes expand_main_function to forcibly align
   the stack, which results in aligned frames for functions called from
   main, though it does nothing for the alignment of main itself.  */
#define FORCE_PREFERRED_STACK_BOUNDARY_IN_MAIN \
  (ix86_preferred_stack_boundary > STACK_BOUNDARY && !TARGET_64BIT)

/* Minimum allocation boundary for the code of a function.  */
#define FUNCTION_BOUNDARY 8

/* C++ stores the virtual bit in the lowest bit of function pointers.  */
#define TARGET_PTRMEMFUNC_VBIT_LOCATION ptrmemfunc_vbit_in_pfn

/* Alignment of field after `int : 0' in a structure.  */

#define EMPTY_FIELD_BOUNDARY BITS_PER_WORD

/* Minimum size in bits of the largest boundary to which any
   and all fundamental data types supported by the hardware
   might need to be aligned. No data type wants to be aligned
   rounder than this.

   Pentium+ prefers DFmode values to be aligned to 64 bit boundary
   and Pentium Pro XFmode values at 128 bit boundaries.  */

#define BIGGEST_ALIGNMENT 128

/* Decide whether a variable of mode MODE should be 128 bit aligned.  */
#define ALIGN_MODE_128(MODE) \
 ((MODE) == XFmode || SSE_REG_MODE_P (MODE))

/* The published ABIs say that doubles should be aligned on word
   boundaries, so lower the alignment for structure fields unless
   -malign-double is set.  */

/* ??? Blah -- this macro is used directly by libobjc.  Since it
   supports no vector modes, cut out the complexity and fall back
   on BIGGEST_FIELD_ALIGNMENT.  */
#ifdef IN_TARGET_LIBS
#ifdef __x86_64__
#define BIGGEST_FIELD_ALIGNMENT 128
#else
#define BIGGEST_FIELD_ALIGNMENT 32
#endif
#else
#define ADJUST_FIELD_ALIGN(FIELD, COMPUTED) \
   x86_field_alignment (FIELD, COMPUTED)
#endif

/* If defined, a C expression to compute the alignment given to a
   constant that is being placed in memory.  EXP is the constant
   and ALIGN is the alignment that the object would ordinarily have.
   The value of this macro is used instead of that alignment to align
   the object.

   If this macro is not defined, then ALIGN is used.

   The typical use of this macro is to increase alignment for string
   constants to be word aligned so that `strcpy' calls that copy
   constants can be done inline.  */

#define CONSTANT_ALIGNMENT(EXP, ALIGN) ix86_constant_alignment ((EXP), (ALIGN))

/* If defined, a C expression to compute the alignment for a static
   variable.  TYPE is the data type, and ALIGN is the alignment that
   the object would ordinarily have.  The value of this macro is used
   instead of that alignment to align the object.

   If this macro is not defined, then ALIGN is used.

   One use of this macro is to increase alignment of medium-size
   data to make it all fit in fewer cache lines.  Another is to
   cause character arrays to be word-aligned so that `strcpy' calls
   that copy constants to character arrays can be done inline.  */

#define DATA_ALIGNMENT(TYPE, ALIGN) ix86_data_alignment ((TYPE), (ALIGN))

/* If defined, a C expression to compute the alignment for a local
   variable.  TYPE is the data type, and ALIGN is the alignment that
   the object would ordinarily have.  The value of this macro is used
   instead of that alignment to align the object.

   If this macro is not defined, then ALIGN is used.

   One use of this macro is to increase alignment of medium-size
   data to make it all fit in fewer cache lines.  */

#define LOCAL_ALIGNMENT(TYPE, ALIGN) ix86_local_alignment ((TYPE), (ALIGN))

/* If defined, a C expression that gives the alignment boundary, in
   bits, of an argument with the specified mode and type.  If it is
   not defined, `PARM_BOUNDARY' is used for all arguments.  */

#define FUNCTION_ARG_BOUNDARY(MODE, TYPE) \
  ix86_function_arg_boundary ((MODE), (TYPE))

/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 0

/* If bit field type is int, don't let it cross an int,
   and give entire struct the alignment of an int.  */
/* Required on the 386 since it doesn't have bit-field insns.  */
#define PCC_BITFIELD_TYPE_MATTERS 1

/* Standard register usage.  */

/* This processor has special stack-like registers.  See reg-stack.c
   for details.  */

/* LLVM LOCAL begin We don't need the RTL-based STACK_REGS (x87) mechanism. */
#ifndef ENABLE_LLVM
#define STACK_REGS
#endif
/* LLVM LOCAL end */

#define IS_STACK_MODE(MODE)					\
  (((MODE) == SFmode && (!TARGET_SSE || !TARGET_SSE_MATH))	\
   || ((MODE) == DFmode && (!TARGET_SSE2 || !TARGET_SSE_MATH))  \
   || (MODE) == XFmode)

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.

   In the 80386 we give the 8 general purpose registers the numbers 0-7.
   We number the floating point registers 8-15.
   Note that registers 0-7 can be accessed as a  short or int,
   while only 0-3 may be used with byte `mov' instructions.

   Reg 16 does not correspond to any hardware register, but instead
   appears in the RTL as an argument pointer prior to reload, and is
   eliminated during reloading in favor of either the stack or frame
   pointer.  */

#define FIRST_PSEUDO_REGISTER 53

/* Number of hardware registers that go into the DWARF-2 unwind info.
   If not defined, equals FIRST_PSEUDO_REGISTER.  */

#define DWARF_FRAME_REGISTERS 17

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.
   On the 80386, the stack pointer is such, as is the arg pointer.

   The value is zero if the register is not fixed on either 32 or
   64 bit targets, one if the register if fixed on both 32 and 64
   bit targets, two if it is only fixed on 32bit targets and three
   if its only fixed on 64bit targets.
   Proper values are computed in the CONDITIONAL_REGISTER_USAGE.
 */
#define FIXED_REGISTERS						\
/*ax,dx,cx,bx,si,di,bp,sp,st,st1,st2,st3,st4,st5,st6,st7*/	\
{  0, 0, 0, 0, 0, 0, 0, 1, 0,  0,  0,  0,  0,  0,  0,  0,	\
/*arg,flags,fpsr,dir,frame*/					\
    1,    1,   1,  1,    1,					\
/*xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7*/			\
     0,   0,   0,   0,   0,   0,   0,   0,			\
/*mmx0,mmx1,mmx2,mmx3,mmx4,mmx5,mmx6,mmx7*/			\
     0,   0,   0,   0,   0,   0,   0,   0,			\
/*  r8,  r9, r10, r11, r12, r13, r14, r15*/			\
     2,   2,   2,   2,   2,   2,   2,   2,			\
/*xmm8,xmm9,xmm10,xmm11,xmm12,xmm13,xmm14,xmm15*/		\
     2,   2,    2,    2,    2,    2,    2,    2}


/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.

   The value is zero if the register is not call used on either 32 or
   64 bit targets, one if the register if call used on both 32 and 64
   bit targets, two if it is only call used on 32bit targets and three
   if its only call used on 64bit targets.
   Proper values are computed in the CONDITIONAL_REGISTER_USAGE.
*/
#define CALL_USED_REGISTERS					\
/*ax,dx,cx,bx,si,di,bp,sp,st,st1,st2,st3,st4,st5,st6,st7*/	\
{  1, 1, 1, 0, 3, 3, 0, 1, 1,  1,  1,  1,  1,  1,  1,  1,	\
/*arg,flags,fpsr,dir,frame*/					\
     1,   1,   1,  1,    1,					\
/*xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7*/			\
     1,   1,   1,   1,   1,  1,    1,   1,			\
/*mmx0,mmx1,mmx2,mmx3,mmx4,mmx5,mmx6,mmx7*/			\
     1,   1,   1,   1,   1,   1,   1,   1,			\
/*  r8,  r9, r10, r11, r12, r13, r14, r15*/			\
     1,   1,   1,   1,   2,   2,   2,   2,			\
/*xmm8,xmm9,xmm10,xmm11,xmm12,xmm13,xmm14,xmm15*/		\
     1,   1,    1,    1,    1,    1,    1,    1}		\

/* Order in which to allocate registers.  Each register must be
   listed once, even those in FIXED_REGISTERS.  List frame pointer
   late and fixed registers last.  Note that, in general, we prefer
   registers listed in CALL_USED_REGISTERS, keeping the others
   available for storage of persistent values.

   The ORDER_REGS_FOR_LOCAL_ALLOC actually overwrite the order,
   so this is just empty initializer for array.  */

#define REG_ALLOC_ORDER 					\
{  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,\
   18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,	\
   33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,  \
   48, 49, 50, 51, 52 }

/* ORDER_REGS_FOR_LOCAL_ALLOC is a macro which permits reg_alloc_order
   to be rearranged based on a particular function.  When using sse math,
   we want to allocate SSE before x87 registers and vice vera.  */

#define ORDER_REGS_FOR_LOCAL_ALLOC x86_order_regs_for_local_alloc ()


/* Macro to conditionally modify fixed_regs/call_used_regs.  */
#define CONDITIONAL_REGISTER_USAGE					\
do {									\
    int i;								\
    for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)				\
      {									\
	if (fixed_regs[i] > 1)						\
	  fixed_regs[i] = (fixed_regs[i] == (TARGET_64BIT ? 3 : 2));	\
	if (call_used_regs[i] > 1)					\
	  call_used_regs[i] = (call_used_regs[i]			\
			       == (TARGET_64BIT ? 3 : 2));		\
      }									\
    if (PIC_OFFSET_TABLE_REGNUM != INVALID_REGNUM)			\
      {									\
	fixed_regs[PIC_OFFSET_TABLE_REGNUM] = 1;			\
	call_used_regs[PIC_OFFSET_TABLE_REGNUM] = 1;			\
      }									\
    if (! TARGET_MMX)							\
      {									\
	int i;								\
        for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)			\
          if (TEST_HARD_REG_BIT (reg_class_contents[(int)MMX_REGS], i))	\
	    fixed_regs[i] = call_used_regs[i] = 1, reg_names[i] = "";	\
      }									\
    if (! TARGET_SSE)							\
      {									\
	int i;								\
        for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)			\
          if (TEST_HARD_REG_BIT (reg_class_contents[(int)SSE_REGS], i))	\
	    fixed_regs[i] = call_used_regs[i] = 1, reg_names[i] = "";	\
      }									\
    if (! TARGET_80387 && ! TARGET_FLOAT_RETURNS_IN_80387)		\
      {									\
	int i;								\
	HARD_REG_SET x;							\
        COPY_HARD_REG_SET (x, reg_class_contents[(int)FLOAT_REGS]);	\
        for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)			\
          if (TEST_HARD_REG_BIT (x, i)) 				\
	    fixed_regs[i] = call_used_regs[i] = 1, reg_names[i] = "";	\
      }									\
    if (! TARGET_64BIT)							\
      {									\
	int i;								\
	for (i = FIRST_REX_INT_REG; i <= LAST_REX_INT_REG; i++)		\
	  reg_names[i] = "";						\
	for (i = FIRST_REX_SSE_REG; i <= LAST_REX_SSE_REG; i++)		\
	  reg_names[i] = "";						\
      }									\
  } while (0)

/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.
   This is ordinarily the length in words of a value of mode MODE
   but can be less for certain modes in special long registers.

   Actually there are no two word move instructions for consecutive
   registers.  And only registers 0-3 may have mov byte instructions
   applied to them.
   */

#define HARD_REGNO_NREGS(REGNO, MODE)   \
  (FP_REGNO_P (REGNO) || SSE_REGNO_P (REGNO) || MMX_REGNO_P (REGNO)	\
   ? (COMPLEX_MODE_P (MODE) ? 2 : 1)					\
   : ((MODE) == XFmode							\
      ? (TARGET_64BIT ? 2 : 3)						\
      : (MODE) == XCmode						\
      ? (TARGET_64BIT ? 4 : 6)						\
      : ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)))

#define HARD_REGNO_NREGS_HAS_PADDING(REGNO, MODE)			\
  ((TARGET_128BIT_LONG_DOUBLE && !TARGET_64BIT)				\
   ? (FP_REGNO_P (REGNO) || SSE_REGNO_P (REGNO) || MMX_REGNO_P (REGNO)	\
      ? 0								\
      : ((MODE) == XFmode || (MODE) == XCmode))				\
   : 0)

#define HARD_REGNO_NREGS_WITH_PADDING(REGNO, MODE) ((MODE) == XFmode ? 4 : 8)

#define VALID_SSE2_REG_MODE(MODE) \
    ((MODE) == V16QImode || (MODE) == V8HImode || (MODE) == V2DFmode    \
     || (MODE) == V2DImode || (MODE) == DFmode)

#define VALID_SSE_REG_MODE(MODE)					\
    ((MODE) == TImode || (MODE) == V4SFmode || (MODE) == V4SImode	\
     || (MODE) == SFmode || (MODE) == TFmode)

#define VALID_MMX_REG_MODE_3DNOW(MODE) \
    ((MODE) == V2SFmode || (MODE) == SFmode)

#define VALID_MMX_REG_MODE(MODE)					\
    ((MODE) == DImode || (MODE) == V8QImode || (MODE) == V4HImode	\
/* APPLE LOCAL 4656532 use V1DImode for _m64 */				\
     || (MODE) == V2SImode || (MODE) == SImode || (MODE) == V1DImode)

/* ??? No autovectorization into MMX or 3DNOW until we can reliably
   place emms and femms instructions.  */
#define UNITS_PER_SIMD_WORD (TARGET_SSE ? 16 : UNITS_PER_WORD)

#define VALID_FP_MODE_P(MODE)						\
    ((MODE) == SFmode || (MODE) == DFmode || (MODE) == XFmode		\
     || (MODE) == SCmode || (MODE) == DCmode || (MODE) == XCmode)	\

#define VALID_INT_MODE_P(MODE)						\
    ((MODE) == QImode || (MODE) == HImode || (MODE) == SImode		\
     || (MODE) == DImode						\
     || (MODE) == CQImode || (MODE) == CHImode || (MODE) == CSImode	\
     || (MODE) == CDImode						\
     || (TARGET_64BIT && ((MODE) == TImode || (MODE) == CTImode		\
         || (MODE) == TFmode || (MODE) == TCmode)))

/* Return true for modes passed in SSE registers.  */
#define SSE_REG_MODE_P(MODE) \
 ((MODE) == TImode || (MODE) == V16QImode || (MODE) == TFmode		\
   || (MODE) == V8HImode || (MODE) == V2DFmode || (MODE) == V2DImode	\
   || (MODE) == V4SFmode || (MODE) == V4SImode)

/* Value is 1 if hard register REGNO can hold a value of machine-mode MODE.  */

#define HARD_REGNO_MODE_OK(REGNO, MODE)	\
   ix86_hard_regno_mode_ok ((REGNO), (MODE))

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */

#define MODES_TIEABLE_P(MODE1, MODE2)  ix86_modes_tieable_p (MODE1, MODE2)

/* It is possible to write patterns to move flags; but until someone
   does it,  */
#define AVOID_CCMODE_COPIES

/* Specify the modes required to caller save a given hard regno.
   We do this on i386 to prevent flags from being saved at all.

   Kill any attempts to combine saving of modes.  */

#define HARD_REGNO_CALLER_SAVE_MODE(REGNO, NREGS, MODE)			\
  (CC_REGNO_P (REGNO) ? VOIDmode					\
   : (MODE) == VOIDmode && (NREGS) != 1 ? VOIDmode			\
   : (MODE) == VOIDmode ? choose_hard_reg_mode ((REGNO), (NREGS), false)\
   : (MODE) == HImode && !TARGET_PARTIAL_REG_STALL ? SImode		\
   : (MODE) == QImode && (REGNO) >= 4 && !TARGET_64BIT ? SImode 	\
   : (MODE))
/* Specify the registers used for certain standard purposes.
   The values of these macros are register numbers.  */

/* on the 386 the pc register is %eip, and is not usable as a general
   register.  The ordinary mov instructions won't work */
/* #define PC_REGNUM  */

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM 7

/* Base register for access to local variables of the function.  */
#define HARD_FRAME_POINTER_REGNUM 6

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM 20

/* First floating point reg */
#define FIRST_FLOAT_REG 8

/* First & last stack-like regs */
#define FIRST_STACK_REG FIRST_FLOAT_REG
#define LAST_STACK_REG (FIRST_FLOAT_REG + 7)

#define FIRST_SSE_REG (FRAME_POINTER_REGNUM + 1)
#define LAST_SSE_REG  (FIRST_SSE_REG + 7)

#define FIRST_MMX_REG  (LAST_SSE_REG + 1)
#define LAST_MMX_REG   (FIRST_MMX_REG + 7)

#define FIRST_REX_INT_REG  (LAST_MMX_REG + 1)
#define LAST_REX_INT_REG   (FIRST_REX_INT_REG + 7)

#define FIRST_REX_SSE_REG  (LAST_REX_INT_REG + 1)
#define LAST_REX_SSE_REG   (FIRST_REX_SSE_REG + 7)

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms
   may be accessed via the stack pointer) in functions that seem suitable.
   This is computed in `reload', in reload1.c.  */
#define FRAME_POINTER_REQUIRED  ix86_frame_pointer_required ()

/* Override this in other tm.h files to cope with various OS lossage
   requiring a frame pointer.  */
#ifndef SUBTARGET_FRAME_POINTER_REQUIRED
#define SUBTARGET_FRAME_POINTER_REQUIRED 0
#endif

/* Make sure we can access arbitrary call frames.  */
#define SETUP_FRAME_ADDRESSES()  ix86_setup_frame_addresses ()

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM 16

/* Register in which static-chain is passed to a function.
   We do use ECX as static chain register for 32 bit ABI.  On the
   64bit ABI, ECX is an argument register, so we use R10 instead.  */
#define STATIC_CHAIN_REGNUM (TARGET_64BIT ? FIRST_REX_INT_REG + 10 - 8 : 2)

/* Register to hold the addressing base for position independent
   code access to data items.  We don't use PIC pointer for 64bit
   mode.  Define the regnum to dummy value to prevent gcc from
   pessimizing code dealing with EBX.

   To avoid clobbering a call-saved register unnecessarily, we renumber
   the pic register when possible.  The change is visible after the
   prologue has been emitted.  */

#define REAL_PIC_OFFSET_TABLE_REGNUM  3

#define PIC_OFFSET_TABLE_REGNUM				\
  /* APPLE LOCAL begin 5695218 */			\
  ((TARGET_64BIT && ix86_cmodel == CM_SMALL_PIC)	\
  || !flag_pic ? INVALID_REGNUM				\
   : reload_completed && pic_offset_table_rtx ? REGNO (pic_offset_table_rtx) \
   : REAL_PIC_OFFSET_TABLE_REGNUM)					\
  /* APPLE LOCAL end 5695218 */

#define GOT_SYMBOL_NAME "_GLOBAL_OFFSET_TABLE_"

/* A C expression which can inhibit the returning of certain function
   values in registers, based on the type of value.  A nonzero value
   says to return the function value in memory, just as large
   structures are always returned.  Here TYPE will be a C expression
   of type `tree', representing the data type of the value.

   Note that values of mode `BLKmode' must be explicitly handled by
   this macro.  Also, the option `-fpcc-struct-return' takes effect
   regardless of this macro.  On most systems, it is possible to
   leave the macro undefined; this causes a default definition to be
   used, whose value is the constant 1 for `BLKmode' values, and 0
   otherwise.

   Do not use this macro to indicate that structures and unions
   should always be returned in memory.  You should instead use
   `DEFAULT_PCC_STRUCT_RETURN' to indicate this.  */

#define RETURN_IN_MEMORY(TYPE) \
  ix86_return_in_memory (TYPE)

/* APPLE LOCAL begin radar 4781080 */
#define OBJC_FPRETURN_MSGCALL(TYPE,WHICH) \
  ix86_objc_fpreturn_msgcall (TYPE, WHICH)
/* APPLE LOCAL end radar 4781080 */

/* This is overridden by <cygwin.h>.  */
#define MS_AGGREGATE_RETURN 0

/* This is overridden by <netware.h>.  */
#define KEEP_AGGREGATE_RETURN_POINTER 0

/* Define the classes of registers for register constraints in the
   machine description.  Also define ranges of constants.

   One of the classes must always be named ALL_REGS and include all hard regs.
   If there is more than one class, another class must be named NO_REGS
   and contain no registers.

   The name GENERAL_REGS must be the name of a class (or an alias for
   another name such as ALL_REGS).  This is the class of registers
   that is allowed by "g" or "r" in a register constraint.
   Also, registers outside this class are allocated only when
   instructions express preferences for them.

   The classes must be numbered in nondecreasing order; that is,
   a larger-numbered class must never be contained completely
   in a smaller-numbered class.

   For any two classes, it is very desirable that there be another
   class that represents their union.

   It might seem that class BREG is unnecessary, since no useful 386
   opcode needs reg %ebx.  But some systems pass args to the OS in ebx,
   and the "b" register constraint is useful in asms for syscalls.

   The flags and fpsr registers are in no class.  */

enum reg_class
{
  NO_REGS,
  AREG, DREG, CREG, BREG, SIREG, DIREG,
  AD_REGS,			/* %eax/%edx for DImode */
  Q_REGS,			/* %eax %ebx %ecx %edx */
  NON_Q_REGS,			/* %esi %edi %ebp %esp */
  INDEX_REGS,			/* %eax %ebx %ecx %edx %esi %edi %ebp */
  LEGACY_REGS,			/* %eax %ebx %ecx %edx %esi %edi %ebp %esp */
  GENERAL_REGS,			/* %eax %ebx %ecx %edx %esi %edi %ebp %esp %r8 - %r15*/
  FP_TOP_REG, FP_SECOND_REG,	/* %st(0) %st(1) */
  FLOAT_REGS,
  /* APPLE LOCAL 5612787 mainline sse4 */
  SSE_FIRST_REG,
  SSE_REGS,
  MMX_REGS,
  FP_TOP_SSE_REGS,
  FP_SECOND_SSE_REGS,
  FLOAT_SSE_REGS,
  FLOAT_INT_REGS,
  INT_SSE_REGS,
  FLOAT_INT_SSE_REGS,
  ALL_REGS, LIM_REG_CLASSES
};

#define N_REG_CLASSES ((int) LIM_REG_CLASSES)

#define INTEGER_CLASS_P(CLASS) \
  reg_class_subset_p ((CLASS), GENERAL_REGS)
#define FLOAT_CLASS_P(CLASS) \
  reg_class_subset_p ((CLASS), FLOAT_REGS)
#define SSE_CLASS_P(CLASS) \
  ((CLASS) == SSE_REGS)
#define MMX_CLASS_P(CLASS) \
  ((CLASS) == MMX_REGS)
#define MAYBE_INTEGER_CLASS_P(CLASS) \
  reg_classes_intersect_p ((CLASS), GENERAL_REGS)
#define MAYBE_FLOAT_CLASS_P(CLASS) \
  reg_classes_intersect_p ((CLASS), FLOAT_REGS)
#define MAYBE_SSE_CLASS_P(CLASS) \
  reg_classes_intersect_p (SSE_REGS, (CLASS))
#define MAYBE_MMX_CLASS_P(CLASS) \
  reg_classes_intersect_p (MMX_REGS, (CLASS))

#define Q_CLASS_P(CLASS) \
  reg_class_subset_p ((CLASS), Q_REGS)

/* Give names of register classes as strings for dump file.  */

#define REG_CLASS_NAMES \
{  "NO_REGS",				\
   "AREG", "DREG", "CREG", "BREG",	\
   "SIREG", "DIREG",			\
   "AD_REGS",				\
   "Q_REGS", "NON_Q_REGS",		\
   "INDEX_REGS",			\
   "LEGACY_REGS",			\
   "GENERAL_REGS",			\
   "FP_TOP_REG", "FP_SECOND_REG",	\
   "FLOAT_REGS",			\
    /* APPLE LOCAL 5612787 mainline sse4 */	\
   "SSE_FIRST_REG",			\
   "SSE_REGS",				\
   "MMX_REGS",				\
   "FP_TOP_SSE_REGS",			\
   "FP_SECOND_SSE_REGS",		\
   "FLOAT_SSE_REGS",			\
   "FLOAT_INT_REGS",			\
   "INT_SSE_REGS",			\
   "FLOAT_INT_SSE_REGS",		\
   "ALL_REGS" }

/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */

#define REG_CLASS_CONTENTS						\
{     { 0x00,     0x0 },						\
      { 0x01,     0x0 }, { 0x02, 0x0 },	/* AREG, DREG */		\
      { 0x04,     0x0 }, { 0x08, 0x0 },	/* CREG, BREG */		\
      { 0x10,     0x0 }, { 0x20, 0x0 },	/* SIREG, DIREG */		\
      { 0x03,     0x0 },		/* AD_REGS */			\
      { 0x0f,     0x0 },		/* Q_REGS */			\
  { 0x1100f0,  0x1fe0 },		/* NON_Q_REGS */		\
      { 0x7f,  0x1fe0 },		/* INDEX_REGS */		\
  { 0x1100ff,  0x0 },			/* LEGACY_REGS */		\
  { 0x1100ff,  0x1fe0 },		/* GENERAL_REGS */		\
     { 0x100,     0x0 }, { 0x0200, 0x0 },/* FP_TOP_REG, FP_SECOND_REG */\
    { 0xff00,     0x0 },		/* FLOAT_REGS */		\
/* APPLE LOCAL 5612787 mainline sse4 */						\
  { 0x200000,     0x0 },		/* SSE_FIRST_REG */		\
{ 0x1fe00000,0x1fe000 },		/* SSE_REGS */			\
{ 0xe0000000,    0x1f },		/* MMX_REGS */			\
{ 0x1fe00100,0x1fe000 },		/* FP_TOP_SSE_REG */		\
{ 0x1fe00200,0x1fe000 },		/* FP_SECOND_SSE_REG */		\
{ 0x1fe0ff00,0x1fe000 },		/* FLOAT_SSE_REGS */		\
   { 0x1ffff,  0x1fe0 },		/* FLOAT_INT_REGS */		\
{ 0x1fe100ff,0x1fffe0 },		/* INT_SSE_REGS */		\
{ 0x1fe1ffff,0x1fffe0 },		/* FLOAT_INT_SSE_REGS */	\
{ 0xffffffff,0x1fffff }							\
}

/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */

#define REGNO_REG_CLASS(REGNO) (regclass_map[REGNO])

/* When defined, the compiler allows registers explicitly used in the
   rtl to be used as spill registers but prevents the compiler from
   extending the lifetime of these registers.  */

#define SMALL_REGISTER_CLASSES 1

#define QI_REG_P(X) \
  (REG_P (X) && REGNO (X) < 4)

#define GENERAL_REGNO_P(N) \
  ((N) < 8 || REX_INT_REGNO_P (N))

#define GENERAL_REG_P(X) \
  (REG_P (X) && GENERAL_REGNO_P (REGNO (X)))

#define ANY_QI_REG_P(X) (TARGET_64BIT ? GENERAL_REG_P(X) : QI_REG_P (X))

#define NON_QI_REG_P(X) \
  (REG_P (X) && REGNO (X) >= 4 && REGNO (X) < FIRST_PSEUDO_REGISTER)

#define REX_INT_REGNO_P(N) ((N) >= FIRST_REX_INT_REG && (N) <= LAST_REX_INT_REG)
#define REX_INT_REG_P(X) (REG_P (X) && REX_INT_REGNO_P (REGNO (X)))

#define FP_REG_P(X) (REG_P (X) && FP_REGNO_P (REGNO (X)))
#define FP_REGNO_P(N) ((N) >= FIRST_STACK_REG && (N) <= LAST_STACK_REG)
#define ANY_FP_REG_P(X) (REG_P (X) && ANY_FP_REGNO_P (REGNO (X)))
#define ANY_FP_REGNO_P(N) (FP_REGNO_P (N) || SSE_REGNO_P (N))

#define SSE_REGNO_P(N) \
  (((N) >= FIRST_SSE_REG && (N) <= LAST_SSE_REG) \
   || ((N) >= FIRST_REX_SSE_REG && (N) <= LAST_REX_SSE_REG))

#define REX_SSE_REGNO_P(N) \
   ((N) >= FIRST_REX_SSE_REG && (N) <= LAST_REX_SSE_REG)

#define SSE_REGNO(N) \
  ((N) < 8 ? FIRST_SSE_REG + (N) : FIRST_REX_SSE_REG + (N) - 8)
#define SSE_REG_P(N) (REG_P (N) && SSE_REGNO_P (REGNO (N)))

#define SSE_FLOAT_MODE_P(MODE) \
  ((TARGET_SSE && (MODE) == SFmode) || (TARGET_SSE2 && (MODE) == DFmode))

#define MMX_REGNO_P(N) ((N) >= FIRST_MMX_REG && (N) <= LAST_MMX_REG)
#define MMX_REG_P(XOP) (REG_P (XOP) && MMX_REGNO_P (REGNO (XOP)))

#define STACK_REG_P(XOP)		\
  (REG_P (XOP) &&		       	\
   REGNO (XOP) >= FIRST_STACK_REG &&	\
   REGNO (XOP) <= LAST_STACK_REG)

#define NON_STACK_REG_P(XOP) (REG_P (XOP) && ! STACK_REG_P (XOP))

#define STACK_TOP_P(XOP) (REG_P (XOP) && REGNO (XOP) == FIRST_STACK_REG)

#define CC_REG_P(X) (REG_P (X) && CC_REGNO_P (REGNO (X)))
#define CC_REGNO_P(X) ((X) == FLAGS_REG || (X) == FPSR_REG)

/* The class value for index registers, and the one for base regs.  */

#define INDEX_REG_CLASS INDEX_REGS
#define BASE_REG_CLASS GENERAL_REGS

/* Place additional restrictions on the register class to use when it
   is necessary to be able to hold a value of mode MODE in a reload
   register for which class CLASS would ordinarily be used.  */

#define LIMIT_RELOAD_CLASS(MODE, CLASS) 			\
  ((MODE) == QImode && !TARGET_64BIT				\
   && ((CLASS) == ALL_REGS || (CLASS) == GENERAL_REGS		\
       || (CLASS) == LEGACY_REGS || (CLASS) == INDEX_REGS)	\
   ? Q_REGS : (CLASS))

/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS; but on some machines
   in some cases it is preferable to use a more restrictive class.
   On the 80386 series, we prevent floating constants from being
   reloaded into floating registers (since no move-insn can do that)
   and we ensure that QImodes aren't reloaded into the esi or edi reg.  */

/* Put float CONST_DOUBLE in the constant pool instead of fp regs.
   QImode must go into class Q_REGS.
   Narrow ALL_REGS to GENERAL_REGS.  This supports allowing movsf and
   movdf to do mem-to-mem moves through integer regs.  */

#define PREFERRED_RELOAD_CLASS(X, CLASS) \
   ix86_preferred_reload_class ((X), (CLASS))

/* Discourage putting floating-point values in SSE registers unless
   SSE math is being used, and likewise for the 387 registers.  */

#define PREFERRED_OUTPUT_RELOAD_CLASS(X, CLASS) \
   ix86_preferred_output_reload_class ((X), (CLASS))

/* If we are copying between general and FP registers, we need a memory
   location. The same is true for SSE and MMX registers.  */
#define SECONDARY_MEMORY_NEEDED(CLASS1, CLASS2, MODE) \
  ix86_secondary_memory_needed ((CLASS1), (CLASS2), (MODE), 1)

/* QImode spills from non-QI registers need a scratch.  This does not
   happen often -- the only example so far requires an uninitialized
   pseudo.  */

#define SECONDARY_OUTPUT_RELOAD_CLASS(CLASS, MODE, OUT)			\
  (((CLASS) == GENERAL_REGS || (CLASS) == LEGACY_REGS			\
    || (CLASS) == INDEX_REGS) && !TARGET_64BIT && (MODE) == QImode	\
   ? Q_REGS : NO_REGS)

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */
/* On the 80386, this is the size of MODE in words,
   except in the FP regs, where a single reg is always enough.  */
#define CLASS_MAX_NREGS(CLASS, MODE)					\
 (!MAYBE_INTEGER_CLASS_P (CLASS)					\
  ? (COMPLEX_MODE_P (MODE) ? 2 : 1)					\
  : (((((MODE) == XFmode ? 12 : GET_MODE_SIZE (MODE)))			\
      + UNITS_PER_WORD - 1) / UNITS_PER_WORD))

/* A C expression whose value is nonzero if pseudos that have been
   assigned to registers of class CLASS would likely be spilled
   because registers of CLASS are needed for spill registers.

   The default value of this macro returns 1 if CLASS has exactly one
   register and zero otherwise.  On most machines, this default
   should be used.  Only define this macro to some other expression
   if pseudo allocated by `local-alloc.c' end up in memory because
   their hard registers were needed for spill registers.  If this
   macro returns nonzero for those classes, those pseudos will only
   be allocated by `global.c', which knows how to reallocate the
   pseudo to another register.  If there would not be another
   register available for reallocation, you should not change the
   definition of this macro since the only effect of such a
   definition would be to slow down register allocation.  */

#define CLASS_LIKELY_SPILLED_P(CLASS)					\
  (((CLASS) == AREG)							\
   || ((CLASS) == DREG)							\
   || ((CLASS) == CREG)							\
   || ((CLASS) == BREG)							\
   || ((CLASS) == AD_REGS)						\
   || ((CLASS) == SIREG)						\
   || ((CLASS) == DIREG)						\
   || ((CLASS) == FP_TOP_REG)						\
   || ((CLASS) == FP_SECOND_REG))

/* Return a class of registers that cannot change FROM mode to TO mode.  */

#define CANNOT_CHANGE_MODE_CLASS(FROM, TO, CLASS) \
  ix86_cannot_change_mode_class (FROM, TO, CLASS)

/* Stack layout; function entry, exit and calling.  */

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD

/* Define this to nonzero if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
#define FRAME_GROWS_DOWNWARD 1

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */
#define STARTING_FRAME_OFFSET 0

/* If we generate an insn to push BYTES bytes,
   this says how many the stack pointer really advances by.
   On 386, we have pushw instruction that decrements by exactly 2 no
   matter what the position was, there is no pushb.
   But as CIE data alignment factor on this arch is -4, we need to make
   sure all stack pointer adjustments are in multiple of 4.

   For 64bit ABI we round up to 8 bytes.
 */

#define PUSH_ROUNDING(BYTES) \
  (TARGET_64BIT		     \
   ? (((BYTES) + 7) & (-8))  \
   : (((BYTES) + 3) & (-4)))

/* If defined, the maximum amount of space required for outgoing arguments will
   be computed and placed into the variable
   `current_function_outgoing_args_size'.  No space will be pushed onto the
   stack for each call; instead, the function prologue should increase the stack
   frame size by this amount.  */

#define ACCUMULATE_OUTGOING_ARGS TARGET_ACCUMULATE_OUTGOING_ARGS

/* If defined, a C expression whose value is nonzero when we want to use PUSH
   instructions to pass outgoing arguments.  */

#define PUSH_ARGS (TARGET_PUSH_ARGS && !ACCUMULATE_OUTGOING_ARGS)

/* We want the stack and args grow in opposite directions, even if
   PUSH_ARGS is 0.  */
#define PUSH_ARGS_REVERSED 1

/* Offset of first parameter from the argument pointer register value.  */
#define FIRST_PARM_OFFSET(FNDECL) 0

/* Define this macro if functions should assume that stack space has been
   allocated for arguments even when their values are passed in registers.

   The value of this macro is the size, in bytes, of the area reserved for
   arguments passed in registers for the function represented by FNDECL.

   This space can be allocated by the caller, or be a part of the
   machine-dependent stack frame: `OUTGOING_REG_PARM_STACK_SPACE' says
   which.  */
#define REG_PARM_STACK_SPACE(FNDECL) 0

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

#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, SIZE) \
  ix86_return_pops_args ((FUNDECL), (FUNTYPE), (SIZE))

#define FUNCTION_VALUE_REGNO_P(N) \
  ix86_function_value_regno_p (N)

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */

#define LIBCALL_VALUE(MODE) \
  ix86_libcall_value (MODE)

/* Define the size of the result block used for communication between
   untyped_call and untyped_return.  The block contains a DImode value
   followed by the block used by fnsave and frstor.  */

#define APPLY_RESULT_SIZE (8+108)

/* 1 if N is a possible register number for function argument passing.  */
#define FUNCTION_ARG_REGNO_P(N) ix86_function_arg_regno_p (N)

/* Define a data type for recording info about an argument list
   during the scan of that argument list.  This data type should
   hold all necessary information about the function itself
   and about the args processed so far, enough to enable macros
   such as FUNCTION_ARG to determine where the next arg should go.  */

typedef struct ix86_args {
  int words;			/* # words passed so far */
  int nregs;			/* # registers available for passing */
  int regno;			/* next available register number */
  int fastcall;			/* fastcall calling convention is used */
  int sse_words;		/* # sse words passed so far */
  int sse_nregs;		/* # sse registers available for passing */
  int warn_sse;			/* True when we want to warn about SSE ABI.  */
  int warn_mmx;			/* True when we want to warn about MMX ABI.  */
  int sse_regno;		/* next available sse register number */
  int mmx_words;		/* # mmx words passed so far */
  int mmx_nregs;		/* # mmx registers available for passing */
  int mmx_regno;		/* next available mmx register number */
  int maybe_vaarg;		/* true for calls to possibly vardic fncts.  */
  int float_in_sse;		/* 1 if in 32-bit mode SFmode (2 for DFmode) should
				   be passed in SSE registers.  Otherwise 0.  */
} CUMULATIVE_ARGS;

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.  */

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS) \
  init_cumulative_args (&(CUM), (FNTYPE), (LIBNAME), (FNDECL))

/* Update the data in CUM to advance over an argument
   of mode MODE and data type TYPE.
   (TYPE is null for libcalls where that information may not be available.)  */

#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED) \
  function_arg_advance (&(CUM), (MODE), (TYPE), (NAMED))

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

#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
  function_arg (&(CUM), (MODE), (TYPE), (NAMED))

/* Implement `va_start' for varargs and stdarg.  */
#define EXPAND_BUILTIN_VA_START(VALIST, NEXTARG) \
  ix86_va_start (VALIST, NEXTARG)

#define TARGET_ASM_FILE_END ix86_file_end
#define NEED_INDICATE_EXEC_STACK 0

/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */

#define FUNCTION_PROFILER(FILE, LABELNO) x86_function_profiler (FILE, LABELNO)

#define MCOUNT_NAME "_mcount"

#define PROFILE_COUNT_REGISTER "edx"

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */
/* Note on the 386 it might be more efficient not to define this since
   we have to restore it ourselves from the frame pointer, in order to
   use pop */

#define EXIT_IGNORE_STACK 1

/* Output assembler code for a block containing the constant parts
   of a trampoline, leaving space for the variable parts.  */

/* On the 386, the trampoline contains two instructions:
     mov #STATIC,ecx
     jmp FUNCTION
   The trampoline is generated entirely at runtime.  The operand of JMP
   is the address of FUNCTION relative to the instruction following the
   JMP (which is 5 bytes long).  */

/* Length in units of the trampoline for entering a nested function.  */

#define TRAMPOLINE_SIZE (TARGET_64BIT ? 23 : 10)

/* Emit RTL insns to initialize the variable parts of a trampoline.
   FNADDR is an RTX for the address of the function's pure code.
   CXT is an RTX for the static chain value for the function.  */

#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT) \
  x86_initialize_trampoline ((TRAMP), (FNADDR), (CXT))

/* Definitions for register eliminations.

   This is an array of structures.  Each structure initializes one pair
   of eliminable registers.  The "from" register number is given first,
   followed by "to".  Eliminations of the same "from" register are listed
   in order of preference.

   There are two registers that can always be eliminated on the i386.
   The frame pointer and the arg pointer can be replaced by either the
   hard frame pointer or to the stack pointer, depending upon the
   circumstances.  The hard frame pointer is not used before reload and
   so it is not eligible for elimination.  */

#define ELIMINABLE_REGS					\
{{ ARG_POINTER_REGNUM, STACK_POINTER_REGNUM},		\
 { ARG_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM},	\
 { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM},		\
 { FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM}}	\

/* Given FROM and TO register numbers, say whether this elimination is
   allowed.  Frame pointer elimination is automatically handled.

   All other eliminations are valid.  */

#define CAN_ELIMINATE(FROM, TO) \
  ((TO) == STACK_POINTER_REGNUM ? ! frame_pointer_needed : 1)

/* Define the offset between two registers, one to be eliminated, and the other
   its replacement, at the start of a routine.  */

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET) \
  ((OFFSET) = ix86_initial_elimination_offset ((FROM), (TO)))

/* Addressing modes, and classification of registers for them.  */

/* Macros to check register numbers against specific register classes.  */

/* These assume that REGNO is a hard or pseudo reg number.
   They give nonzero only if REGNO is a hard reg of the suitable class
   or a pseudo reg currently allocated to a suitable hard reg.
   Since they use reg_renumber, they are safe only once reg_renumber
   has been allocated, which happens in local-alloc.c.  */

#define REGNO_OK_FOR_INDEX_P(REGNO) 					\
  ((REGNO) < STACK_POINTER_REGNUM 					\
   || (REGNO >= FIRST_REX_INT_REG					\
       && (REGNO) <= LAST_REX_INT_REG)					\
   || ((unsigned) reg_renumber[(REGNO)] >= FIRST_REX_INT_REG		\
       && (unsigned) reg_renumber[(REGNO)] <= LAST_REX_INT_REG)		\
   || (unsigned) reg_renumber[(REGNO)] < STACK_POINTER_REGNUM)

#define REGNO_OK_FOR_BASE_P(REGNO) 					\
  ((REGNO) <= STACK_POINTER_REGNUM 					\
   || (REGNO) == ARG_POINTER_REGNUM 					\
   || (REGNO) == FRAME_POINTER_REGNUM 					\
   || (REGNO >= FIRST_REX_INT_REG					\
       && (REGNO) <= LAST_REX_INT_REG)					\
   || ((unsigned) reg_renumber[(REGNO)] >= FIRST_REX_INT_REG		\
       && (unsigned) reg_renumber[(REGNO)] <= LAST_REX_INT_REG)		\
   || (unsigned) reg_renumber[(REGNO)] <= STACK_POINTER_REGNUM)

#define REGNO_OK_FOR_SIREG_P(REGNO) \
  ((REGNO) == 4 || reg_renumber[(REGNO)] == 4)
#define REGNO_OK_FOR_DIREG_P(REGNO) \
  ((REGNO) == 5 || reg_renumber[(REGNO)] == 5)

/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx
   and check its validity for a certain class.
   We have two alternate definitions for each of them.
   The usual definition accepts all pseudo regs; the other rejects
   them unless they have been allocated suitable hard regs.
   The symbol REG_OK_STRICT causes the latter definition to be used.

   Most source files want to accept pseudo regs in the hope that
   they will get allocated to the class that the insn wants them to be in.
   Source files for reload pass need to be strict.
   After reload, it makes no difference, since pseudo regs have
   been eliminated by then.  */


/* Non strict versions, pseudos are ok.  */
#define REG_OK_FOR_INDEX_NONSTRICT_P(X)					\
  (REGNO (X) < STACK_POINTER_REGNUM					\
   || (REGNO (X) >= FIRST_REX_INT_REG					\
       && REGNO (X) <= LAST_REX_INT_REG)				\
   || REGNO (X) >= FIRST_PSEUDO_REGISTER)

#define REG_OK_FOR_BASE_NONSTRICT_P(X)					\
  (REGNO (X) <= STACK_POINTER_REGNUM					\
   || REGNO (X) == ARG_POINTER_REGNUM					\
   || REGNO (X) == FRAME_POINTER_REGNUM 				\
   || (REGNO (X) >= FIRST_REX_INT_REG					\
       && REGNO (X) <= LAST_REX_INT_REG)				\
   || REGNO (X) >= FIRST_PSEUDO_REGISTER)

/* Strict versions, hard registers only */
#define REG_OK_FOR_INDEX_STRICT_P(X) REGNO_OK_FOR_INDEX_P (REGNO (X))
#define REG_OK_FOR_BASE_STRICT_P(X)  REGNO_OK_FOR_BASE_P (REGNO (X))

#ifndef REG_OK_STRICT
#define REG_OK_FOR_INDEX_P(X)  REG_OK_FOR_INDEX_NONSTRICT_P (X)
#define REG_OK_FOR_BASE_P(X)   REG_OK_FOR_BASE_NONSTRICT_P (X)

#else
#define REG_OK_FOR_INDEX_P(X)  REG_OK_FOR_INDEX_STRICT_P (X)
#define REG_OK_FOR_BASE_P(X)   REG_OK_FOR_BASE_STRICT_P (X)
#endif

/* GO_IF_LEGITIMATE_ADDRESS recognizes an RTL expression
   that is a valid memory address for an instruction.
   The MODE argument is the machine mode for the MEM expression
   that wants to use this address.

   The other macros defined here are used only in GO_IF_LEGITIMATE_ADDRESS,
   except for CONSTANT_ADDRESS_P which is usually machine-independent.

   See legitimize_pic_address in i386.c for details as to what
   constitutes a legitimate address when -fpic is used.  */

#define MAX_REGS_PER_ADDRESS 2

#define CONSTANT_ADDRESS_P(X)  constant_address_p (X)

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */

#define LEGITIMATE_CONSTANT_P(X)  legitimate_constant_p (X)

#ifdef REG_OK_STRICT
#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, ADDR)				\
do {									\
  if (legitimate_address_p ((MODE), (X), 1))				\
    goto ADDR;								\
} while (0)

#else
#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, ADDR)				\
do {									\
  if (legitimate_address_p ((MODE), (X), 0))				\
    goto ADDR;								\
} while (0)

#endif

/* If defined, a C expression to determine the base term of address X.
   This macro is used in only one place: `find_base_term' in alias.c.

   It is always safe for this macro to not be defined.  It exists so
   that alias analysis can understand machine-dependent addresses.

   The typical use of this macro is to handle addresses containing
   a label_ref or symbol_ref within an UNSPEC.  */

#define FIND_BASE_TERM(X) ix86_find_base_term (X)

/* Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This macro is used in only one place: `memory_address' in explow.c.

   OLDX is the address as it was before break_out_memory_refs was called.
   In some cases it is useful to look at this to decide what needs to be done.

   MODE and WIN are passed so that this macro can use
   GO_IF_LEGITIMATE_ADDRESS.

   It is always safe for this macro to do nothing.  It exists to recognize
   opportunities to optimize the output.

   For the 80386, we handle X+REG by loading X into a register R and
   using R+REG.  R will go in a general reg and indexing will be used.
   However, if REG is a broken-out memory address or multiplication,
   nothing needs to be done because REG can certainly go in a general reg.

   When -fpic is used, special handling is needed for symbolic references.
   See comments by legitimize_pic_address in i386.c for details.  */

#define LEGITIMIZE_ADDRESS(X, OLDX, MODE, WIN)				\
do {									\
  (X) = legitimize_address ((X), (OLDX), (MODE));			\
  if (memory_address_p ((MODE), (X)))					\
    goto WIN;								\
} while (0)

#define REWRITE_ADDRESS(X) rewrite_address (X)

/* Nonzero if the constant value X is a legitimate general operand
   when generating PIC code.  It is given that flag_pic is on and
   that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */

#define LEGITIMATE_PIC_OPERAND_P(X) legitimate_pic_operand_p (X)

#define SYMBOLIC_CONST(X)	\
  (GET_CODE (X) == SYMBOL_REF						\
   || GET_CODE (X) == LABEL_REF						\
   || (GET_CODE (X) == CONST && symbolic_reference_mentioned_p (X)))

/* Go to LABEL if ADDR (a legitimate address expression)
   has an effect that depends on the machine mode it is used for.
   On the 80386, only postdecrement and postincrement address depend thus
   (the amount of decrement or increment being the length of the operand).  */
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR, LABEL)	\
do {							\
 if (GET_CODE (ADDR) == POST_INC			\
     || GET_CODE (ADDR) == POST_DEC)			\
   goto LABEL;						\
} while (0)

/* Max number of args passed in registers.  If this is more than 3, we will
   have problems with ebx (register #4), since it is a caller save register and
   is also used as the pic register in ELF.  So for now, don't allow more than
   3 registers to be passed in registers.  */

#define REGPARM_MAX (TARGET_64BIT ? 6 : 3)

/* APPLE LOCAL regparmandstackparm */
#define SSE_REGPARM_MAX (TARGET_64BIT ? 8 : (TARGET_MACHO ? 4 : (TARGET_SSE ? 3 : 0)))

#define MMX_REGPARM_MAX (TARGET_64BIT ? 0 : (TARGET_MMX ? 3 : 0))


/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE (!TARGET_64BIT || flag_pic ? SImode : DImode)

/* Define this as 1 if `char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 1

/* Number of bytes moved into a data cache for a single prefetch operation.  */
#define PREFETCH_BLOCK ix86_cost->prefetch_block

/* Number of prefetch operations that can be done in parallel.  */
#define SIMULTANEOUS_PREFETCHES ix86_cost->simultaneous_prefetches

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 16

/* MOVE_MAX_PIECES is the number of bytes at a time which we can
   move efficiently, as opposed to  MOVE_MAX which is the maximum
   number of bytes we can move with a single instruction.  */
#define MOVE_MAX_PIECES (TARGET_64BIT ? 8 : 4)

/* If a memory-to-memory move would take MOVE_RATIO or more simple
   move-instruction pairs, we will do a movmem or libcall instead.
   Increasing the value will always make code faster, but eventually
   incurs high cost in increased code size.

   If you don't define this, a reasonable default is used.  */

#define MOVE_RATIO (optimize_size ? 3 : ix86_cost->move_ratio)

/* If a clear memory operation would take CLEAR_RATIO or more simple
   move-instruction sequences, we will do a clrmem or libcall instead.  */

#define CLEAR_RATIO (optimize_size ? 2 \
		     : ix86_cost->move_ratio > 6 ? 6 : ix86_cost->move_ratio)

/* Define if shifts truncate the shift count
   which implies one can omit a sign-extension or zero-extension
   of a shift count.  */
/* On i386, shifts do truncate the count.  But bit opcodes don't.  */

/* #define SHIFT_COUNT_TRUNCATED */

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* A macro to update M and UNSIGNEDP when an object whose type is
   TYPE and which has the specified mode and signedness is to be
   stored in a register.  This macro is only called when TYPE is a
   scalar type.

   On i386 it is sometimes useful to promote HImode and QImode
   quantities to SImode.  The choice depends on target type.  */

#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE) 		\
do {							\
  if (((MODE) == HImode && TARGET_PROMOTE_HI_REGS)	\
      || ((MODE) == QImode && TARGET_PROMOTE_QI_REGS))	\
    (MODE) = SImode;					\
} while (0)

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode (TARGET_64BIT ? DImode : SImode)

/* A function address in a call instruction
   is a byte address (for indexing purposes)
   so give the MEM rtx a byte's mode.  */
#define FUNCTION_MODE QImode

/* A C expression for the cost of moving data from a register in class FROM to
   one in class TO.  The classes are expressed using the enumeration values
   such as `GENERAL_REGS'.  A value of 2 is the default; other values are
   interpreted relative to that.

   It is not required that the cost always equal 2 when FROM is the same as TO;
   on some machines it is expensive to move between registers if they are not
   general registers.  */

#define REGISTER_MOVE_COST(MODE, CLASS1, CLASS2) \
   ix86_register_move_cost ((MODE), (CLASS1), (CLASS2))

/* A C expression for the cost of moving data of mode M between a
   register and memory.  A value of 2 is the default; this cost is
   relative to those in `REGISTER_MOVE_COST'.

   If moving between registers and memory is more expensive than
   between two registers, you should define this macro to express the
   relative cost.  */

#define MEMORY_MOVE_COST(MODE, CLASS, IN)	\
  ix86_memory_move_cost ((MODE), (CLASS), (IN))

/* A C expression for the cost of a branch instruction.  A value of 1
   is the default; other values are interpreted relative to that.  */

#define BRANCH_COST ix86_branch_cost

/* Define this macro as a C expression which is nonzero if accessing
   less than a word of memory (i.e. a `char' or a `short') is no
   faster than accessing a word of memory, i.e., if such access
   require more than one instruction or if there is no difference in
   cost between byte and (aligned) word loads.

   When this macro is not defined, the compiler will access a field by
   finding the smallest containing object; when it is defined, a
   fullword load will be used if alignment permits.  Unless bytes
   accesses are faster than word accesses, using word accesses is
   preferable since it may eliminate subsequent memory access if
   subsequent accesses occur to other fields in the same word of the
   structure, but to different bytes.  */

/* APPLE LOCAL 6131435 */
#define SLOW_BYTE_ACCESS (!flag_apple_kext && !flag_mkernel && !TARGET_64BIT)

/* Nonzero if access to memory by shorts is slow and undesirable.  */
#define SLOW_SHORT_ACCESS 0

/* Define this macro to be the value 1 if unaligned accesses have a
   cost many times greater than aligned accesses, for example if they
   are emulated in a trap handler.

   When this macro is nonzero, the compiler will act as if
   `STRICT_ALIGNMENT' were nonzero when generating code for block
   moves.  This can cause significantly more instructions to be
   produced.  Therefore, do not set this macro nonzero if unaligned
   accesses only add a cycle or two to the time for a memory access.

   If the value of this macro is always zero, it need not be defined.  */

/* #define SLOW_UNALIGNED_ACCESS(MODE, ALIGN) 0 */

/* Define this macro if it is as good or better to call a constant
   function address than to call an address kept in a register.

   Desirable on the 386 because a CALL with a constant address is
   faster than one with a register address.  */

#define NO_FUNCTION_CSE

/* Given a comparison code (EQ, NE, etc.) and the first operand of a COMPARE,
   return the mode to be used for the comparison.

   For floating-point equality comparisons, CCFPEQmode should be used.
   VOIDmode should be used in all other cases.

   For integer comparisons against zero, reduce to CCNOmode or CCZmode if
   possible, to allow for more combinations.  */

#define SELECT_CC_MODE(OP, X, Y) ix86_cc_mode ((OP), (X), (Y))

/* Return nonzero if MODE implies a floating point inequality can be
   reversed.  */

#define REVERSIBLE_CC_MODE(MODE) 1

/* A C expression whose value is reversed condition code of the CODE for
   comparison done in CC_MODE mode.  */
#define REVERSE_CONDITION(CODE, MODE) ix86_reverse_condition ((CODE), (MODE))


/* Control the assembler format that we output, to the extent
   this does not vary between assemblers.  */

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).  */

/* In order to refer to the first 8 regs as 32 bit regs, prefix an "e".
   For non floating point regs, the following are the HImode names.

   For float regs, the stack top is sometimes referred to as "%st(0)"
   instead of just "%st".  PRINT_OPERAND handles this with the "y" code.  */

#define HI_REGISTER_NAMES						\
{"ax","dx","cx","bx","si","di","bp","sp",				\
 "st","st(1)","st(2)","st(3)","st(4)","st(5)","st(6)","st(7)",		\
 "argp", "flags", "fpsr", "dirflag", "frame",				\
 "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7",		\
 "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"	,		\
 "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",			\
 "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"}

#define REGISTER_NAMES HI_REGISTER_NAMES

/* Table of additional register names to use in user input.  */

#define ADDITIONAL_REGISTER_NAMES \
{ { "eax", 0 }, { "edx", 1 }, { "ecx", 2 }, { "ebx", 3 },	\
  { "esi", 4 }, { "edi", 5 }, { "ebp", 6 }, { "esp", 7 },	\
  { "rax", 0 }, { "rdx", 1 }, { "rcx", 2 }, { "rbx", 3 },	\
  { "rsi", 4 }, { "rdi", 5 }, { "rbp", 6 }, { "rsp", 7 },	\
  { "al", 0 }, { "dl", 1 }, { "cl", 2 }, { "bl", 3 },		\
  { "ah", 0 }, { "dh", 1 }, { "ch", 2 }, { "bh", 3 } }

/* Note we are omitting these since currently I don't know how
to get gcc to use these, since they want the same but different
number as al, and ax.
*/

#define QI_REGISTER_NAMES \
{"al", "dl", "cl", "bl", "sil", "dil", "bpl", "spl",}

/* These parallel the array above, and can be used to access bits 8:15
   of regs 0 through 3.  */

#define QI_HIGH_REGISTER_NAMES \
{"ah", "dh", "ch", "bh", }

/* How to renumber registers for dbx and gdb.  */

#define DBX_REGISTER_NUMBER(N) \
  (TARGET_64BIT ? dbx64_register_map[(N)] : dbx_register_map[(N)])

extern int const dbx_register_map[FIRST_PSEUDO_REGISTER];
extern int const dbx64_register_map[FIRST_PSEUDO_REGISTER];
extern int const svr4_dbx_register_map[FIRST_PSEUDO_REGISTER];

/* Before the prologue, RA is at 0(%esp).  */
#define INCOMING_RETURN_ADDR_RTX \
  gen_rtx_MEM (VOIDmode, gen_rtx_REG (VOIDmode, STACK_POINTER_REGNUM))

/* After the prologue, RA is at -4(AP) in the current frame.  */
#define RETURN_ADDR_RTX(COUNT, FRAME)					   \
  ((COUNT) == 0								   \
   ? gen_rtx_MEM (Pmode, plus_constant (arg_pointer_rtx, -UNITS_PER_WORD)) \
   : gen_rtx_MEM (Pmode, plus_constant (FRAME, UNITS_PER_WORD)))

/* PC is dbx register 8; let's use that column for RA.  */
#define DWARF_FRAME_RETURN_COLUMN 	(TARGET_64BIT ? 16 : 8)

/* Before the prologue, the top of the frame is at 4(%esp).  */
#define INCOMING_FRAME_SP_OFFSET UNITS_PER_WORD

/* Describe how we implement __builtin_eh_return.  */
#define EH_RETURN_DATA_REGNO(N)	((N) < 2 ? (N) : INVALID_REGNUM)
#define EH_RETURN_STACKADJ_RTX	gen_rtx_REG (Pmode, 2)


/* Select a format to encode pointers in exception handling data.  CODE
   is 0 for data, 1 for code labels, 2 for function pointers.  GLOBAL is
   true if the symbol may be affected by dynamic relocations.

   ??? All x86 object file formats are capable of representing this.
   After all, the relocation needed is the same as for the call insn.
   Whether or not a particular assembler allows us to enter such, I
   guess we'll have to see.  */
#define ASM_PREFERRED_EH_DATA_FORMAT(CODE, GLOBAL)       		\
  asm_preferred_eh_data_format ((CODE), (GLOBAL))

/* This is how to output an insn to push a register on the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_PUSH(FILE, REGNO)  \
do {									\
  if (TARGET_64BIT)							\
    asm_fprintf ((FILE), "\tpush{q}\t%%r%s\n",				\
		 reg_names[(REGNO)] + (REX_INT_REGNO_P (REGNO) != 0));	\
  else									\
    asm_fprintf ((FILE), "\tpush{l}\t%%e%s\n", reg_names[(REGNO)]);	\
} while (0)

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_POP(FILE, REGNO)  \
do {									\
  if (TARGET_64BIT)							\
    asm_fprintf ((FILE), "\tpop{q}\t%%r%s\n",				\
		 reg_names[(REGNO)] + (REX_INT_REGNO_P (REGNO) != 0));	\
  else									\
    asm_fprintf ((FILE), "\tpop{l}\t%%e%s\n", reg_names[(REGNO)]);	\
} while (0)

/* This is how to output an element of a case-vector that is absolute.  */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)  \
  ix86_output_addr_vec_elt ((FILE), (VALUE))

/* This is how to output an element of a case-vector that is relative.  */

#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL) \
  ix86_output_addr_diff_elt ((FILE), (VALUE), (REL))

/* Under some conditions we need jump tables in the text section,
   because the assembler cannot handle label differences between
   sections.  This is the case for x86_64 on Mach-O for example.  */

#define JUMP_TABLES_IN_TEXT_SECTION \
  (flag_pic && ((TARGET_MACHO && TARGET_64BIT) \
   || (!TARGET_64BIT && !HAVE_AS_GOTOFF_IN_DATA)))

/* Switch to init or fini section via SECTION_OP, emit a call to FUNC,
   and switch back.  For x86 we do this only to save a few bytes that
   would otherwise be unused in the text section.  */
#define CRT_CALL_STATIC_FUNCTION(SECTION_OP, FUNC)	\
   asm (SECTION_OP "\n\t"				\
	"call " USER_LABEL_PREFIX #FUNC "\n"		\
	TEXT_SECTION_ASM_OP);

/* Print operand X (an rtx) in assembler syntax to file FILE.
   CODE is a letter or dot (`z' in `%z0') or 0 if no letter was specified.
   Effect of various CODE letters is described in i386.c near
   print_operand function.  */

#define PRINT_OPERAND_PUNCT_VALID_P(CODE) \
  ((CODE) == '*' || (CODE) == '+' || (CODE) == '&')

#define PRINT_OPERAND(FILE, X, CODE)  \
  print_operand ((FILE), (X), (CODE))

#define PRINT_OPERAND_ADDRESS(FILE, ADDR)  \
  print_operand_address ((FILE), (ADDR))

#define OUTPUT_ADDR_CONST_EXTRA(FILE, X, FAIL)	\
do {						\
  if (! output_addr_const_extra (FILE, (X)))	\
    goto FAIL;					\
} while (0);

/* a letter which is not needed by the normal asm syntax, which
   we can use for operand syntax in the extended asm */

#define ASM_OPERAND_LETTER '#'
#define RET return ""
#define AT_SP(MODE) (gen_rtx_MEM ((MODE), stack_pointer_rtx))

/* Which processor to schedule for. The cpu attribute defines a list that
   mirrors this list, so changes to i386.md must be made at the same time.  */

enum processor_type
{
  PROCESSOR_I386,			/* 80386 */
  PROCESSOR_I486,			/* 80486DX, 80486SX, 80486DX[24] */
  PROCESSOR_PENTIUM,
  PROCESSOR_PENTIUMPRO,
  PROCESSOR_K6,
  PROCESSOR_ATHLON,
  PROCESSOR_PENTIUM4,
  PROCESSOR_K8,
  PROCESSOR_NOCONA,
  /* APPLE LOCAL mainline */
  PROCESSOR_CORE2,
  PROCESSOR_GENERIC32,
  PROCESSOR_GENERIC64,
  PROCESSOR_max
};

extern enum processor_type ix86_tune;
extern enum processor_type ix86_arch;

enum fpmath_unit
{
  FPMATH_387 = 1,
  FPMATH_SSE = 2
};

extern enum fpmath_unit ix86_fpmath;

enum tls_dialect
{
  TLS_DIALECT_GNU,
  TLS_DIALECT_GNU2,
  TLS_DIALECT_SUN
};

extern enum tls_dialect ix86_tls_dialect;

enum cmodel {
  CM_32,	/* The traditional 32-bit ABI.  */
  CM_SMALL,	/* Assumes all code and data fits in the low 31 bits.  */
  CM_KERNEL,	/* Assumes all code and data fits in the high 31 bits.  */
  CM_MEDIUM,	/* Assumes code fits in the low 31 bits; data unlimited.  */
  CM_LARGE,	/* No assumptions.  */
  CM_SMALL_PIC,	/* Assumes code+data+got/plt fits in a 31 bit region.  */
  CM_MEDIUM_PIC	/* Assumes code+got/plt fits in a 31 bit region.  */
};

extern enum cmodel ix86_cmodel;

/* Size of the RED_ZONE area.  */
#define RED_ZONE_SIZE 128
/* Reserved area of the red zone for temporaries.  */
#define RED_ZONE_RESERVE 8

enum asm_dialect {
  ASM_ATT,
  ASM_INTEL
};

extern enum asm_dialect ix86_asm_dialect;
/* APPLE LOCAL begin regparmandstackparm */
extern void ix86_darwin_handle_regparmandstackparm (tree fndecl);
extern void ix86_darwin_redirect_calls(void);
/* APPLE LOCAL end regparmandstackparm */

extern unsigned int ix86_preferred_stack_boundary;
/* APPLE LOCAL begin radar 4216496, 4229407, 4120689, 4095567 */
extern unsigned int ix86_save_preferred_stack_boundary;
/* APPLE LOCAL end radar 4216496, 4229407, 4120689, 4095567 */
extern int ix86_branch_cost, ix86_section_threshold;

/* Smallest class containing REGNO.  */
extern enum reg_class const regclass_map[FIRST_PSEUDO_REGISTER];

extern rtx ix86_compare_op0;	/* operand 0 for comparisons */
extern rtx ix86_compare_op1;	/* operand 1 for comparisons */
extern rtx ix86_compare_emitted;

/* To properly truncate FP values into integers, we need to set i387 control
   word.  We can't emit proper mode switching code before reload, as spills
   generated by reload may truncate values incorrectly, but we still can avoid
   redundant computation of new control word by the mode switching pass.
   The fldcw instructions are still emitted redundantly, but this is probably
   not going to be noticeable problem, as most CPUs do have fast path for
   the sequence.

   The machinery is to emit simple truncation instructions and split them
   before reload to instructions having USEs of two memory locations that
   are filled by this code to old and new control word.

   Post-reload pass may be later used to eliminate the redundant fildcw if
   needed.  */

enum ix86_entity
{
  I387_TRUNC = 0,
  I387_FLOOR,
  I387_CEIL,
  I387_MASK_PM,
  MAX_386_ENTITIES
};

enum ix86_stack_slot
{
  SLOT_VIRTUAL = 0,
  SLOT_TEMP,
  SLOT_CW_STORED,
  SLOT_CW_TRUNC,
  SLOT_CW_FLOOR,
  SLOT_CW_CEIL,
  SLOT_CW_MASK_PM,
  MAX_386_STACK_LOCALS
};

/* Define this macro if the port needs extra instructions inserted
   for mode switching in an optimizing compilation.  */

#define OPTIMIZE_MODE_SWITCHING(ENTITY) \
   ix86_optimize_mode_switching[(ENTITY)]

/* If you define `OPTIMIZE_MODE_SWITCHING', you have to define this as
   initializer for an array of integers.  Each initializer element N
   refers to an entity that needs mode switching, and specifies the
   number of different modes that might need to be set for this
   entity.  The position of the initializer in the initializer -
   starting counting at zero - determines the integer that is used to
   refer to the mode-switched entity in question.  */

#define NUM_MODES_FOR_MODE_SWITCHING \
   { I387_CW_ANY, I387_CW_ANY, I387_CW_ANY, I387_CW_ANY }

/* ENTITY is an integer specifying a mode-switched entity.  If
   `OPTIMIZE_MODE_SWITCHING' is defined, you must define this macro to
   return an integer value not larger than the corresponding element
   in `NUM_MODES_FOR_MODE_SWITCHING', to denote the mode that ENTITY
   must be switched into prior to the execution of INSN. */

#define MODE_NEEDED(ENTITY, I) ix86_mode_needed ((ENTITY), (I))

/* This macro specifies the order in which modes for ENTITY are
   processed.  0 is the highest priority.  */

#define MODE_PRIORITY_TO_MODE(ENTITY, N) (N)

/* Generate one or more insns to set ENTITY to MODE.  HARD_REG_LIVE
   is the set of hard registers live at the point where the insn(s)
   are to be inserted.  */

#define EMIT_MODE_SET(ENTITY, MODE, HARD_REGS_LIVE) 			\
  ((MODE) != I387_CW_ANY && (MODE) != I387_CW_UNINITIALIZED		\
   ? emit_i387_cw_initialization (MODE), 0				\
   : 0)


/* Avoid renaming of stack registers, as doing so in combination with
   scheduling just increases amount of live registers at time and in
   the turn amount of fxch instructions needed.

   ??? Maybe Pentium chips benefits from renaming, someone can try....  */

#define HARD_REGNO_RENAME_OK(SRC, TARGET)  \
   ((SRC) < FIRST_STACK_REG || (SRC) > LAST_STACK_REG)


#define DLL_IMPORT_EXPORT_PREFIX '#'

#define FASTCALL_PREFIX '@'

struct machine_function GTY(())
{
  struct stack_local_entry *stack_locals;
  const char *some_ld_name;
  rtx force_align_arg_pointer;
  int save_varrargs_registers;
  int accesses_prev_frame;
  int optimize_mode_switching[MAX_386_ENTITIES];
  /* Set by ix86_compute_frame_layout and used by prologue/epilogue expander to
     determine the style used.  */
  int use_fast_prologue_epilogue;
  /* Number of saved registers USE_FAST_PROLOGUE_EPILOGUE has been computed
     for.  */
  int use_fast_prologue_epilogue_nregs;
  /* If true, the current function needs the default PIC register, not
     an alternate register (on x86) and must not use the red zone (on
     x86_64), even if it's a leaf function.  We don't want the
     function to be regarded as non-leaf because TLS calls need not
     affect register allocation.  This flag is set when a TLS call
     instruction is expanded within a function, and never reset, even
     if all such instructions are optimized away.  Use the
     ix86_current_function_calls_tls_descriptor macro for a better
     approximation.  */
  int tls_descriptor_call_expanded_p;
};

#define ix86_stack_locals (cfun->machine->stack_locals)
#define ix86_save_varrargs_registers (cfun->machine->save_varrargs_registers)
#define ix86_optimize_mode_switching (cfun->machine->optimize_mode_switching)
#define ix86_tls_descriptor_calls_expanded_in_cfun \
  (cfun->machine->tls_descriptor_call_expanded_p)
/* Since tls_descriptor_call_expanded is not cleared, even if all TLS
   calls are optimized away, we try to detect cases in which it was
   optimized away.  Since such instructions (use (reg REG_SP)), we can
   verify whether there's any such instruction live by testing that
   REG_SP is live.  */
#define ix86_current_function_calls_tls_descriptor \
  (ix86_tls_descriptor_calls_expanded_in_cfun && regs_ever_live[SP_REG])

/* Control behavior of x86_file_start.  */
#define X86_FILE_START_VERSION_DIRECTIVE false
#define X86_FILE_START_FLTUSED false

/* APPLE LOCAL begin CW asm blocks */
#undef TARGET_IASM_EXTRA_INFO
#define TARGET_IASM_EXTRA_INFO			\
  char mod[3];					\
  bool as_immediate;				\
  bool as_offset;				\
  bool pseudo;

#define TARGET_IASM_REORDER_ARG(OPCODE, NEWARGNUM, NUM_ARGS, ARGNUM)	\
  do {									\
    /* If we are outputting AT&T style assembly language, the argument	\
       numbering is reversed.  */					\
    if (iasm_x86_needs_swapping (opcode))					\
      NEWARGNUM = NUM_ARGS - ARGNUM + 1;				\
  } while (0)

#define IASM_SYNTH_CONSTRAINTS(R, ARGNUM, NUM_ARGS, DB)					\
  do {											\
    /* On x86, operand 2 or 3 can be left out and the assembler will deal with it.	\
											\
       Take for example an opcode:							\
											\
	   opcode r m i									\
											\
       We allow:									\
											\
	   opcode r mi									\
											\
       when we have only 2 operands.  */						\
    if (R										\
	&& ARGNUM == 2									\
	&& NUM_ARGS == 2								\
	&& R < &DB[sizeof(DB) / sizeof (DB[0]) - 1]					\
	&& strcmp (R[1].opcode, R->opcode) == 0						\
	&& R[1].argnum == 3)								\
      {											\
	tree t;										\
	size_t len = strlen (r->constraint) + strlen (r[1].constraint) + 1;		\
	char *p = alloca (len);								\
											\
	sprintf(p, "%s%s", r->constraint, r[1].constraint);				\
	t = build_string (len, p);							\
	return TREE_STRING_POINTER (t);							\
      }											\
  } while (0)

#define TARGET_IASM_PRINT_OP(BUF, ARG, ARGNUM, USES, MUST_BE_REG, MUST_NOT_BE_REG, E) \
 iasm_print_op (BUF, ARG, ARGNUM, USES, MUST_BE_REG, MUST_NOT_BE_REG, E)

extern tree iasm_x86_canonicalize_operands (const char **, tree, void *);
/* On x86, we can rewrite opcodes, change argument ordering and so no... */
#define IASM_CANONICALIZE_OPERANDS(OPCODE, NEW_OPCODE, IARGS, E)		\
  do {								\
    NEW_OPCODE = OPCODE;					\
    IARGS = iasm_x86_canonicalize_operands (&NEW_OPCODE, IARGS, E);	\
  } while (0)

#define IASM_SEE_OPCODE(YYCHAR, T)					\
    /* If we see an int, arrange to see it as an identifier (opcode),	\
       not as a type.  */						\
    ((YYCHAR == TYPESPEC						\
      && C_RID_CODE (T) == RID_INT)					\
     ? IDENTIFIER : YYCHAR)

/* Return true iff the ID is a prefix for an instruction.  */

#define IASM_IS_PREFIX(ID)				\
  do {							\
    const char *myname = IDENTIFIER_POINTER (ID);	\
    if (strcasecmp (myname, "lock") == 0		\
	|| strcasecmp (myname, "rep") == 0		\
	|| strcasecmp (myname, "repe") == 0		\
	|| strcasecmp (myname, "repz") == 0		\
        || strcasecmp (myname, "repne") == 0		\
	|| strcasecmp (myname, "repnz") == 0)		\
      return true;					\
  } while (0)

#define IASM_PRINT_PREFIX(BUF, PREFIX_LIST) iasm_x86_print_prefix(BUF, PREFIX_LIST)

#define IASM_IMMED_PREFIX(E, BUF)		\
  do {						\
    if (!E->pseudo && ! E->as_immediate)	\
      sprintf (BUF + strlen (BUF), "$");	\
  } while (0)

#define IASM_OFFSET_PREFIX(E, BUF)		\
  do {						\
    if (E->as_offset)				\
      sprintf (BUF + strlen (BUF), "$");	\
  } while (0)

/* We can't yet expose ST(x) to reg-stack.c, don't try.  */
#define IASM_HIDE_REG(R) FP_REGNO_P (R)

#define IASM_SEE_IMMEDIATE(E)			\
  E->as_immediate = true

#define IASM_SEE_NO_IMMEDIATE(E)			\
  E->as_immediate = false

/* LLVM LOCAL begin change some = to + */
/* Table of instructions that need extra constraints.  Keep this table sorted.  */
#undef TARGET_IASM_OP_CONSTRAINT
#define TARGET_IASM_OP_CONSTRAINT \
  { "adc", 1, "+" rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64 },\
  { "adc", 2, ri8 "," ri16 "," ri32 C RI64 "," m8 "," m16 "," m32 C M64 },\
  { "add", 1, "+" rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64 },\
  { "add", 2, ri8 "," ri16 "," ri32 C RI64 "," m8 "," m16 "," m32 C M64},\
  { "addpd", 1, "+x"},		\
  { "addpd", 2, "xm"},		\
  { "addps", 1, "+x"},		\
  { "addps", 2, "xm"},		\
  { "addsd", 1, "+x"},		\
  { "addsd", 2, "xm"},		\
  { "addss", 1, "+x"},		\
  { "addss", 2, "xm"},		\
  { "addsubpd", 1, "+x"},      	\
  { "addsubpd", 2, "xm"},	\
  { "addsubps", 1, "+x"},	\
  { "addsubps", 2, "xm"},      	\
  { "and", 1, "+" rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64},\
  { "and", 2, ri8 "," ri16 "," ri32 C RI64 "," m8 "," m16 "," m32 C M64},\
  { "andnpd", 1, "+x"},		\
  { "andnpd", 2, "xm"},		\
  { "andnps", 1, "+x"},		\
  { "andnps", 2, "xm"},		\
  { "andpd", 1, "+x"},		\
  { "andpd", 2, "xm"},		\
  { "andps", 1, "+x"},		\
  { "andps", 2, "xm"},		\
  { NX "arpl", 1, "+" rm16},	\
  { NX "arpl", 2, r16},		\
  { "bound", 1, U("r")},	\
  { "bound", 2, U("m")},	\
  { "bsf", 1, "=r"},		\
  { "bsf", 2, "rm"},		\
  { "bsr", 1, "=r"},		\
  { "bsr", 2, "rm"},		\
  { "bt", 1, "rm"},		\
  { "bt", 2, "ri"},		\
  { "btc", 1, "rm"},		\
  { "btc", 2, "ri"},		\
  { "btr", 1, "rm"},		\
  { "btr", 2, "ri"},		\
  { "bts", 1, "rm"},		\
  { "bts", 2, "ri"},		\
  { NX "call", 1, "rsm"},	\
  { "clflush", 1, "=m"},       	\
  { "cmova", 1, r16 "," r32 C R64},\
  { "cmova", 2, rm16 "," rm32 C RM64},\
  { "cmovae", 2, "rm"},		\
  { "cmovb", 2, "rm"},		\
  { "cmovbe", 2, "rm"},		\
  { "cmovc", 2, "rm"},		\
  { "cmove", 2, "rm"},		\
  { "cmovg", 2, "rm"},		\
  { "cmovge", 2, "rm"},		\
  { "cmovl", 2, "rm"},		\
  { "cmovle", 2, "rm"},		\
  { "cmovna", 2, "rm"},		\
  { "cmovnae", 2, "rm"},	\
  { "cmovnb", 2, "rm"},		\
  { "cmovnbe", 2, "rm"},	\
  { "cmovnc", 2, "rm"},		\
  { "cmovne", 2, "rm"},		\
  { "cmovng", 2, "rm"},		\
  { "cmovnge", 2, "rm"},	\
  { "cmovnl", 2, "rm"},		\
  { "cmovnle", 2, "rm"},	\
  { "cmovno", 2, "rm"},		\
  { "cmovnp", 2, "rm"},		\
  { "cmovns", 2, "rm"},		\
  { "cmovnz", 2, "rm"},		\
  { "cmovo", 2, "rm"},		\
  { "cmovp", 2, "rm"},		\
  { "cmovpe", 2, "rm"},		\
  { "cmovpo", 2, "rm"},		\
  { "cmovs", 2, "rm"},		\
  { "cmovz", 2, "rm"},		\
  { "cmp", 1, rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64},\
  { "cmp", 2, ri8 "," ri16 "," ri32 C RI64 "," m8 "," m16 "," m32 C M64},\
  { "cmpeqpd", 1, "+x"},	\
  { "cmpeqpd", 2, "xm"},	\
  { "cmpeqps", 1, "+x"},	\
  { "cmpeqps", 2, "xm"},	\
  { "cmpeqsd", 1, "+x"},	\
  { "cmpeqsd", 2, "xm"},	\
  { "cmpeqss", 1, "+x"},	\
  { "cmpeqss", 2, "xm"},	\
  { "cmplepd", 1, "+x"},	\
  { "cmplepd", 2, "xm"},	\
  { "cmpleps", 1, "+x"},	\
  { "cmpleps", 2, "xm"},	\
  { "cmplesd", 1, "+x"},	\
  { "cmplesd", 2, "xm"},	\
  { "cmpless", 1, "+x"},	\
  { "cmpless", 2, "xm"},	\
  { "cmpltpd", 1, "+x"},	\
  { "cmpltpd", 2, "xm"},	\
  { "cmpltps", 1, "+x"},	\
  { "cmpltps", 2, "xm"},	\
  { "cmpltsd", 1, "+x"},	\
  { "cmpltsd", 2, "xm"},	\
  { "cmpltss", 1, "+x"},	\
  { "cmpltss", 2, "xm"},	\
  { "cmpneqpd", 1, "+x"},	\
  { "cmpneqpd", 2, "xm"},	\
  { "cmpneqps", 1, "+x"},	\
  { "cmpneqps", 2, "xm"},	\
  { "cmpneqsd", 1, "+x"},	\
  { "cmpneqsd", 2, "xm"},	\
  { "cmpneqss", 1, "+x"},	\
  { "cmpneqss", 2, "xm"},	\
  { "cmpnlepd", 1, "+x"},	\
  { "cmpnlepd", 2, "xm"},	\
  { "cmpnleps", 1, "+x"},	\
  { "cmpnleps", 2, "xm"},	\
  { "cmpnlesd", 1, "+x"},	\
  { "cmpnlesd", 2, "xm"},	\
  { "cmpnless", 1, "+x"},	\
  { "cmpnless", 2, "xm"},	\
  { "cmpnltpd", 1, "+x"},	\
  { "cmpnltpd", 2, "xm"},	\
  { "cmpnltps", 1, "+x"},	\
  { "cmpnltps", 2, "xm"},	\
  { "cmpnltsd", 1, "+x"},	\
  { "cmpnltsd", 2, "xm"},	\
  { "cmpnltss", 1, "+x"},	\
  { "cmpnltss", 2, "xm"},	\
  { "cmpordpd", 1, "+x"},	\
  { "cmpordpd", 2, "xm"},	\
  { "cmpordps", 1, "+x"},	\
  { "cmpordps", 2, "xm"},	\
  { "cmpordsd", 1, "+x"},	\
  { "cmpordsd", 2, "xm"},	\
  { "cmpordss", 1, "+x"},	\
  { "cmpordss", 2, "xm"},	\
  { "cmppd", 1, "+x"},		\
  { "cmppd", 2, "xm"},		\
  { "cmppd", 3, "i"},		\
  { "cmpps", 1, "+x"},		\
  { "cmpps", 2, "xm"},		\
  { "cmpps", 3, "i"},		\
  { "cmpsd", 1, "+x"},		\
  { "cmpsd", 2, "xm"},		\
  { "cmpsd", 3, "i"},		\
  { "cmpss", 1, "+x"},		\
  { "cmpss", 2, "xm"},		\
  { "cmpss", 3, "i"},		\
  { "cmpunordpd", 1, "+x"},	\
  { "cmpunordpd", 2, "xm"},	\
  { "cmpunordps", 1, "+x"},	\
  { "cmpunordps", 2, "xm"},	\
  { "cmpunordsd", 1, "+x"},	\
  { "cmpunordsd", 2, "xm"},	\
  { "cmpunordss", 1, "+x"},	\
  { "cmpunordss", 2, "xm"},	\
  { "cmpxchg", 1, "+mr"},      	\
  { "cmpxchg", 2, "r"},      	\
  { "comisd", 1, "x"},		\
  { "comisd", 2, "xm"},		\
  { "comiss", 1, "x"},		\
  { "comiss", 2, "xm"},		\
  { "cvtdq2pd", 1, "=x"},	\
  { "cvtdq2pd", 2, "xm"},	\
  { "cvtdq2ps", 1, "=x"},	\
  { "cvtdq2ps", 2, "xm"},	\
  { "cvtpd2dq", 1, "=x"},	\
  { "cvtpd2dq", 2, "xm"},	\
  { "cvtpd2pi", 1, "=y"},	\
  { "cvtpd2pi", 2, "xm"},	\
  { "cvtpd2ps", 1, "=x"},	\
  { "cvtpd2ps", 2, "xm"},	\
  { "cvtpi2pd", 1, "=x"},	\
  { "cvtpi2pd", 2, "ym"},	\
  { "cvtpi2ps", 1, "=x"},	\
  { "cvtpi2ps", 2, "ym"},	\
  { "cvtps2dq", 1, "=x"},	\
  { "cvtps2dq", 2, "xm"},	\
  { "cvtps2pd", 1, "=x"},	\
  { "cvtps2pd", 2, "xm"},	\
  { "cvtps2pi", 1, "=y"},	\
  { "cvtps2pi", 2, "xm"},	\
  { "cvtsd2si", 1, "=" r32R64},	\
  { "cvtsd2si", 2, "xm"},	\
  { "cvtsd2ss", 1, "=x"},	\
  { "cvtsd2ss", 2, "xm"},	\
  { "cvtsi2sd", 1, "=x"},	\
  { "cvtsi2sd", 2, rm32RM64},	\
  { "cvtsi2ss", 1, "=x"},	\
  { "cvtsi2ss", 2, rm32RM64},	\
  { "cvtss2sd", 1, "=x"},	\
  { "cvtss2sd", 2, "xm"},	\
  { "cvtss2si", 1, "=r"},	\
  { "cvtss2si", 2, "xm"},	\
  { "cvttpd2dq", 1, "=x"},	\
  { "cvttpd2dq", 2, "xm"},	\
  { "cvttpd2pi", 1, "=y"},	\
  { "cvttpd2pi", 2, "xm"},	\
  { "cvttps2dq", 1, "=x"},	\
  { "cvttps2dq", 2, "xm"},	\
  { "cvttps2pi", 1, "=y"},	\
  { "cvttps2pi", 2, "xm"},	\
  { "cvttsd2si", 1, "=r"},	\
  { "cvttsd2si", 2, "xm"},	\
  { "cvttss2si", 1, "=r"},	\
  { "cvttss2si", 2, "xm"},	\
  { "dec", 1, "+" rm8rm16rm32RM64},\
  { "div", 1, rm8rm16rm32},	\
  { "divpd", 1, "+x"},		\
  { "divpd", 2, "xm"},		\
  { "divps", 1, "+x"},		\
  { "divps", 2, "xm"},		\
  { "divsd", 1, "+x"},		\
  { "divsd", 2, "xm"},		\
  { "divss", 1, "+x"},		\
  { "divss", 2, "xm"},		\
  { "enter", 1, "i"},		\
  { "enter", 2, "i"},		\
  { "fadd", 1, "+t,f,@"},	\
  { "fadd", 2, "f,t," m32fpm64fp},\
  { "faddp", 1, "+f"},		\
  { "faddp", 2, "t"},		\
  { "fbld", 1, "m"},		\
  { "fbstp", 1, "m"},		\
  { "fcmovb", 1, "=t"},		\
  { "fcmovb", 2, "f"},		\
  { "fcmovbe", 1, "=t"},       	\
  { "fcmovbe", 2, "f"},		\
  { "fcmove", 1, "=t"},		\
  { "fcmove", 2, "f"},		\
  { "fcmovnb", 1, "=t"},       	\
  { "fcmovnb", 2, "f"},		\
  { "fcmovnbe", 1, "=t"},	\
  { "fcmovnbe", 2, "f"},	\
  { "fcmovne", 1, "=t"},       	\
  { "fcmovne", 2, "f"},		\
  { "fcmovnu", 1, "=t"},       	\
  { "fcmovnu", 2, "f"},		\
  { "fcmovu", 1, "=t"},		\
  { "fcmovu", 2, "f"},		\
  { "fcom", 1, "f" m32fpm64fp},	\
  { "fcomi", 1, "t"},		\
  { "fcomi", 2, "f"},		\
  { "fcomip", 1, "t"},		\
  { "fcomip", 2, "f"},		\
  { "fcomp", 1, "f" m32fpm64fp},\
  { "fdiv", 1, "+t,f,@"},	\
  { "fdiv", 2, "f,t," m32fpm64fp},\
  { "fdivp", 1, "+f"},		\
  { "fdivp", 2, "t"},		\
  { "fdivr", 1, "+t,@"},	\
  { "fdivr", 2, "f," m32fpm64fp},\
  { "fdivrp", 1, "+f"},		\
  { "fdivrp", 2, "t"},		\
  { "ffree", 1, "f"},		\
  { "fiadd", 1, m16m32},	\
  { "ficom", 1, m16m32},	\
  { "ficomp", 1, m16m32},	\
  { "fidiv", 1, m16m32},	\
  { "fidivr", 1, m16m32},	\
  { "fild", 1, m16m32m64},	\
  { "fimul", 1, m16m32},	\
  { "fist", 1, "=" m16m32},	\
  { "fistp", 1, "=" m16m32m64},	\
  { "fisttp", 1, "=" m16m32m64},\
  { "fisub", 1, m16m32},	\
  { "fisubr", 1, m16m32},	\
  { "fld", 1, "f" m32fpm64fpm80fp},\
  { "fldcw", 1, m16},		\
  { "fldenv", 1, "m"},		\
  { "fldt", 1, "m"},		\
  { "fmul", 1, "+f,t,@"},	\
  { "fmul", 2, "t,f," m32fpm64fp},\
  { "fmulp", 1, "+f"},		\
  { "fmulp", 2, "t"},		\
  { "fnsave", 1, "=m"},		\
  { "fnstcw", 1, "m"},		\
  { "fnstenv", 1, "m"},		\
  { "fnstsw", 1, "ma"},		\
  { "frstor", 1, "m"},		\
  { "fsave", 1, "=m"},		\
  { "fst", 1, "=f" m32fpm64fp},	\
  { "fstcw", 1, "=m"},		\
  { "fstenv", 1, "=m"},		\
  { "fstp", 1, "=f" m32fpm64fpm80fp},\
  { "fstsw", 1, "=ma"},       	\
  { "fsub", 1, "+f,t,@"},	\
  { "fsub", 2, "t,f," m32fpm64fp},\
  { "fsubr", 1, "+f,t," m32fpm64fp},\
  { "fsubr", 2, "t,f,@"},	\
  { "fucom", 1, "f"},		\
  { "fucomi", 1, "t"},		\
  { "fucomi", 2, "f"},		\
  { "fucomip", 1, "t"},		\
  { "fucomip", 2, "f"},		\
  { "fucomp", 1, "f"},		\
  { "fxch", 1, "+f" },		\
  { "fxrstor", 1, "m"},		\
  { "fxsave", 1, "=m"},		\
  { "haddpd", 1, "+x"},		\
  { "haddpd", 2, "xm"},		\
  { "haddps", 1, "+x"},		\
  { "haddps", 2, "xm"},		\
  { "hsubpd", 1, "+x"},		\
  { "hsubpd", 2, "xm"},		\
  { "hsubps", 1, "+x"},		\
  { "hsubps", 2, "xm"},		\
  { "idiv", 1, rm8rm16rm32RM64},\
  { "imul", 1, "+r"},		\
  { "imul", 2, "rm"},		\
  { "imul", 3, "i"},		\
  { "in", 1, "=a"},		\
  { "in", 2, "i"},		\
  { "inc", 1, "+" rm8rm16rm32RM64},\
  { NX "ins", 1, "=" m8m16m32},	\
  { NX "ins", 2, "d"},		\
  { "int", 1, "i"},		\
  { "invlpg", 1, "m"},		\
  { "ja", 1, "s"},		\
  { "jae", 1, "s"},		\
  { "jb", 1, "s"},		\
  { "jbe", 1, "s"},		\
  { "jc", 1, "s"},		\
  { NX "jcxz", 1, rel8},		\
  { "je", 1, "s"},		\
  { "jecxz", 1, rel8},		\
  { "jg", 1, "s"},		\
  { "jge", 1, "s"},		\
  { "jl", 1, "s"},		\
  { "jle", 1, "s"},		\
  { NX "jmp", 1, "s" rm32},	\
  { "jna", 1, "s"},		\
  { "jnae", 1, "s"},		\
  { "jnb", 1, "s"},		\
  { "jnc", 1, "s"},		\
  { "jne", 1, "s"},		\
  { "jng", 1, "s"},		\
  { "jnge", 1, "s"},		\
  { "jnl", 1, "s"},		\
  { "jnle", 1, "s"},		\
  { "jno", 1, "s"},		\
  { "jnp", 1, "s"},		\
  { "jns", 1, "s"},		\
  { "jnz", 1, "s"},		\
  { "jo", 1, "s"},		\
  { "jp", 1, "s"},		\
  { "jpe", 1, "s"},		\
  { "jpo", 1, "s"},		\
  { "js", 1, "s"},		\
  { "jz", 1, "s"},		\
  { "lar", 1, "=r"},		\
  { "lar", 2, "rm"},		\
  { "lddqu", 1, "=x"},		\
  { "lddqu", 2, "m"},		\
  { "ldmxcsr", 1, "m"},		\
  { NX "lds", 1, "=" r16 "," r32 C R64},\
  { NX "lds", 2, m16 "," m32 C M64},\
  { "lea", 1, "=r"},		\
  { "lea", 2, "m"},		\
  { NX "les", 1, "=" r16 "," r32 C R64},\
  { NX "les", 2, m16 "," m32 C M64},\
  { "lfs", 1, "=" r16 "," r32 C R64},\
  { "lfs", 2, m16 "," m32 C M64},\
  { "lgdt", 1, "m"},		\
  { "lgs", 1, "=" r16 "," r32 C R64},\
  { "lgs", 2, m16 "," m32 C M64},\
  { "lidt", 1, "m"},		\
  { "lldt", 1, rm16},		\
  { "lmsw", 1, "m"},		\
  { NX "lods", 1, m8m16m32M64},	\
  { "loop", 1, rel8},		\
  { "loope", 1, rel8},		\
  { "loopne", 1, rel8},		\
  { "loopnz", 1, rel8},		\
  { "loopz", 1, rel8},		\
  { "lsl", 1, "=" r16 "," r32},	\
  { "lsl", 2, rm16 "," rm32},	\
  { "lss", 1, "=" r16 "," r32 C R64},\
  { "lss", 2, m16 "," m32 C M64},\
  { "ltr", 1, rm16},		\
  { "maskmovdqu", 1, "x"},	\
  { "maskmovdqu", 2, "x"},	\
  { "maskmovq", 1, "y"},	\
  { "maskmovq", 2, "y"},	\
  { "maxpd", 1, "+x"},		\
  { "maxpd", 2, "xm"},		\
  { "maxps", 1, "+x"},		\
  { "maxps", 2, "xm"},		\
  { "maxsd", 1, "+x"},		\
  { "maxsd", 2, "xm"},		\
  { "maxss", 1, "+x"},		\
  { "maxss", 2, "xm"},		\
  { "minpd", 1, "+x"},		\
  { "minpd", 2, "xm"},		\
  { "minps", 1, "+x"},		\
  { "minps", 2, "xm"},		\
  { "minsd", 1, "+x"},		\
  { "minsd", 2, "xm"},		\
  { "minss", 1, "+x"},		\
  { "minss", 2, "xm"},		\
  { "mov", 1, "=" rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64}, \
  { "mov", 2, ri8 "," ri16 "," ri32 C RI64 "," rmi8 "," rmi16 "," rmi32 C RMI64}, \
  { "movapd", 1, "=x,xm"},	\
  { "movapd", 2, "xm,x"},      	\
  { "movaps", 1, "=x,xm"},	\
  { "movaps", 2, "xm,x"},	\
  { "movd", 1, "=rm,x,y,rm"},	\
  { "movd", 2, "x,rm,rm,y"},	\
  { "movddup", 1, "=x"},	\
  { "movddup", 2, "xm"},	\
  { "movdq2q", 1, "=y"},	\
  { "movdq2q", 2, "x"},		\
  { "movdqa", 1, "=x"},		\
  { "movdqa", 2, "xm"},		\
  { "movdqu", 1, "=x"},		\
  { "movdqu", 2, "xm"},		\
  { "movhlps", 1, "=x"},	\
  { "movhlps", 2, "x"},		\
  { "movhpd", 1, "=x,m"},	\
  { "movhpd", 2, "m,x"},	\
  { "movhps", 1, "=x,m"},	\
  { "movhps", 2, "m,x"},	\
  { "movlhps", 1, "=x"},	\
  { "movlhps", 2, "x"},		\
  { "movlpd", 1, "=x,m"},	\
  { "movlpd", 2, "m,x"},	\
  { "movlps", 1, "=x,m"},	\
  { "movlps", 2, "m,x"},	\
  { "movmskpd", 1, "=r"},	\
  { "movmskpd", 2, "x"},	\
  { "movmskps", 1, "=r"},	\
  { "movmskps", 2, "x"},	\
  { "movntdq", 1, "=m"},	\
  { "movntdq", 2, "x"},		\
  { "movnti", 1, "=m"},		\
  { "movnti", 2, "r"},		\
  { "movntpd", 1, "=m"},	\
  { "movntpd", 2, "x"},		\
  { "movntps", 1, "=m"},	\
  { "movntps", 2, "x"},		\
  { "movntq", 1, "=m"},		\
  { "movntq", 2, "y"},		\
  { "movq", 1, "=x,m,y,m"},	\
  { "movq", 2, "xm,x,ym,y"},	\
  { "movq2dq", 1, "=x"},	\
  { "movq2dq", 2, "y"},		\
  { "movs", 1, "=" m8 "," m16 "," m32 C M64},\
  { "movs", 2, m8 "," m16 "," m32 C M64},\
  { "movsd", 1, "=xm,x"},	\
  { "movsd", 2, "x,xm"},	\
  { "movshdup", 1, "=x"},	\
  { "movshdup", 2, "xm"},	\
  { "movsldup", 1, "=x"},	\
  { "movsldup", 2, "xm"},	\
  { "movss", 1, "=xm,x"},	\
  { "movss", 2, "x,xm"},	\
  { "movsx", 1, "=" r16 "," r32},\
  { "movsx", 2, rm8 "," rm8rm16},\
  { "movupd", 1, "=x,xm"},	\
  { "movupd", 2, "xm,x"},	\
  { "movups", 1, "=x,xm"},	\
  { "movups", 2, "xm,x"},	\
  { "movzx", 1, "=" r16 "," r32},\
  { "movzx", 2, rm8 "," rm8rm16},\
  { "mul", 1, rm8rm16rm32},	\
  { "mulpd", 1, "+x"},		\
  { "mulpd", 2, "xm"},		\
  { "mulps", 1, "+x"},		\
  { "mulps", 2, "xm"},		\
  { "mulsd", 1, "+x"},		\
  { "mulsd", 2, "xm"},		\
  { "mulss", 1, "+x"},		\
  { "mulss", 2, "xm"},		\
  { "neg", 1, "+" rm8rm16rm32},	\
  { "not", 1, "+" rm8rm16rm32},	\
  { "or", 1, "+" rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64},\
  { "or", 2, ri8 "," ri16 "," ri32 C RI64 "," m8 "," m16 "," m32 C M64},\
  { "orpd", 1, "+x"},		\
  { "orpd", 2, "xm"},		\
  { "orps", 1, "+x"},		\
  { "orps", 2, "xm"},		\
  { "out", 1, "id"},		\
  { "out", 2, a8 a16 a32},	\
  { NX "outs", 1, "d"},	\
  { NX "outs", 2, m8m16m32},	\
  { "packssdw", 1, "+x,y"},	\
  { "packssdw", 2, "xm,ym"},	\
  { "packsswb", 1, "+x,y"},	\
  { "packsswb", 2, "xm,ym"},	\
  { "packuswb", 1, "+x,y"},	\
  { "packuswb", 2, "xm,ym"},	\
  { "paddb", 1, "+x,y"},	\
  { "paddb", 2, "xm,ym"},	\
  { "paddd", 1, "+x,y"},	\
  { "paddd", 2, "xm,ym"},	\
  { "paddq", 1, "+x,y"},	\
  { "paddq", 2, "xm,ym"},	\
  { "paddsb", 1, "+x,y"},       \
  { "paddsb", 2, "xm,ym"},      \
  { "paddsw", 1, "+x,y"},       \
  { "paddsw", 2, "xm,ym"},      \
  { "paddusb", 1, "+x,y"},	\
  { "paddusb", 2, "xm,ym"},	\
  { "paddusw", 1, "+x,y"},	\
  { "paddusw", 2, "xm,ym"},	\
  { "paddw", 1, "+x,y"},	\
  { "paddw", 2, "xm,ym"},	\
  { "pand", 1, "+x,y"},		\
  { "pand", 2, "xm,ym"},	\
  { "pandn", 1, "+x,y"},	\
  { "pandn", 2, "xm,ym"},	\
  { "pavgb", 1, "+x,y"},	\
  { "pavgb", 2, "xm,ym"},	\
  { "pavgw", 1, "+x,y"},	\
  { "pavgw", 2, "xm,ym"},	\
  { "pcmpeqb", 1, "+x,y"},	\
  { "pcmpeqb", 2, "xm,ym"},	\
  { "pcmpeqd", 1, "+x,y"},	\
  { "pcmpeqd", 2, "xm,ym"},	\
  { "pcmpeqw", 1, "+x,y"},	\
  { "pcmpeqw", 2, "xm,ym"},	\
  { "pcmpgtb", 1, "+x,y"},	\
  { "pcmpgtb", 2, "xm,ym"},	\
  { "pcmpgtd", 1, "+x,y"},	\
  { "pcmpgtd", 2, "xm,ym"},	\
  { "pcmpgtw", 1, "+x,y"},	\
  { "pcmpgtw", 2, "xm,ym"},	\
  { "pextrw", 1, "=" r32R64},	\
  { "pextrw", 2, "xy"},		\
  { "pextrw", 3, "i"},		\
  { "pinsrw", 1, "+xy"},	\
  { "pinsrw", 2, r32R64 "m"},	\
  { "pinsrw", 3, "i"},		\
  { "pmaddwd", 1, "+x,y"},	\
  { "pmaddwd", 2, "xm,ym"},	\
  { "pmaxsw", 1, "+x,y"},	\
  { "pmaxsw", 2, "xm,ym"},	\
  { "pmaxub", 1, "+x,y"},	\
  { "pmaxub", 2, "xm,ym"},	\
  { "pminsw", 1, "+x,y"},	\
  { "pminsw", 2, "xm,ym"},	\
  { "pminub", 1, "+x,y"},	\
  { "pminub", 2, "xm,ym"},	\
  { "pmovmskb", 1, "+" r32R64},	\
  { "pmovmskb", 2, "xy"},	\
  { "pmulhuw", 1, "+x,y"},	\
  { "pmulhuw", 2, "xm,ym"},	\
  { "pmulhw", 1, "+x,y"},	\
  { "pmulhw", 2, "xm,ym"},	\
  { "pmullw", 1, "+x,y"},	\
  { "pmullw", 2, "xm,ym"},	\
  { "pmuludq", 1, "+x,y"},	\
  { "pmuludq", 2, "xm,ym"},	\
  { "pop", 1, rm16 T(rm32) RM64},\
  { "por", 1, "+x,y"},		\
  { "por", 2, "xm,ym"},		\
  { "prefetchnta", 1, "m"},	\
  { "prefetcht0", 1, "m"},	\
  { "prefetcht1", 1, "m"},	\
  { "prefetcht2", 1, "m"},	\
  { "psadbw", 1, "+x,y"},	\
  { "psadbw", 2, "xm,ym"},	\
  { "pshufd", 1, "=x"},		\
  { "pshufd", 2, "xm"},		\
  { "pshufd", 3, "i"},		\
  { "pshufhw", 1, "=x"},	\
  { "pshufhw", 2, "xm"},	\
  { "pshufhw", 3, "i"},		\
  { "pshuflw", 1, "=x"},	\
  { "pshuflw", 2, "xm"},	\
  { "pshuflw", 3, "i"},		\
  { "pshufw", 1, "=y"},		\
  { "pshufw", 2, "ym"},		\
  { "pshufw", 3, "i"},		\
  { "pslld", 1, "+x,y"},	\
  { "pslld", 2, "xmi,ymi"},     \
  { "pslldq", 1, "+x"},		\
  { "pslldq", 2, "i"},		\
  { "psllq", 1, "+x,y"},	\
  { "psllq", 2, "xmi,ymi"},     \
  { "psllw", 1, "+x,y"},	\
  { "psllw", 2, "xmi,ymi"},     \
  { "psrad", 1, "+x,y"},	\
  { "psrad", 2, "xmi,ymi"},     \
  { "psraw", 1, "+x,y"},	\
  { "psraw", 2, "xmi,ymi"},	\
  { "psrld", 1, "+x,y"},	\
  { "psrld", 2, "xmi,ymi"},	\
  { "psrldq", 1, "+x"},		\
  { "psrldq", 2, "i"},		\
  { "psrlq", 1, "+x,y"},	\
  { "psrlq", 2, "xmi,ymi"},	\
  { "psrlw", 1, "+x,y"},	\
  { "psrlw", 2, "xmi,ymi"},	\
  { "psubb", 1, "+x,y"},	\
  { "psubb", 2, "xm,ym"},	\
  { "psubd", 1, "+x,y"},	\
  { "psubd", 2, "xm,ym"},	\
  { "psubq", 1, "+x,y"},	\
  { "psubq", 2, "xm,ym"},	\
  { "psubsb", 1, "+x,y"},	\
  { "psubsb", 2, "xm,ym"},	\
  { "psubsw", 1, "+x,y"},	\
  { "psubsw", 2, "xm,ym"},	\
  { "psubusb", 1, "+x,y"},	\
  { "psubusb", 2, "xm,ym"},	\
  { "psubusw", 1, "+x,y"},	\
  { "psubusw", 2, "xm,ym"},	\
  { "psubw", 1, "+x,y"},	\
  { "psubw", 2, "xm,ym"},	\
  { "punpckhbw", 1, "+x,y"},	\
  { "punpckhbw", 2, "xm,ym"},	\
  { "punpckhdq", 1, "+x,y"},	\
  { "punpckhdq", 2, "xm,ym"},	\
  { "punpckhqdq", 1, "+x"},	\
  { "punpckhqdq", 2, "xm"},	\
  { "punpckhwd", 1, "+x,y"},	\
  { "punpckhwd", 2, "xm,ym"},	\
  { "punpcklbw", 1, "+x,y"},	\
  { "punpcklbw", 2, "xm,ym"},	\
  { "punpckldq", 1, "+x,y"},	\
  { "punpckldq", 2, "xm,ym"},	\
  { "punpcklqdq", 1, "+x"},	\
  { "punpcklqdq", 2, "xm"},	\
  { "punpcklwd", 1, "+x,y"},	\
  { "punpcklwd", 2, "xm,ym"},	\
  { "push", 1, rm16 T(rm32) RM64 "i"},\
  { "pxor", 1, "+x,y"},		\
  { "pxor", 2, "xm,ym"},	\
  { "rcl", 1, "+" rm8rm16rm32},	\
  { "rcl", 2, "ic"},		\
  { "rcpps", 1, "+x"},		\
  { "rcpps", 2, "xm"},		\
  { "rcpss", 1, "+x"},		\
  { "rcpss", 2, "xm"},		\
  { "rcr", 1, "+" rm8rm16rm32},	\
  { "rcr", 2, "ic"},		\
  { "ret", 1, "i"},		\
  { "rol", 1, "+" rm8rm16rm32},	\
  { "rol", 2, "ic"},		\
  { "ror", 1, "+" rm8rm16rm32},	\
  { "ror", 2, "ic"},		\
  { "rsqrtps", 1, "=x"},	\
  { "rsqrtps", 2, "xm"},	\
  { "rsqrtss", 1, "=x"},	\
  { "rsqrtss", 2, "xm"},	\
  { "sal", 1, "+" rm8rm16rm32},	\
  { "sal", 2, "ic"},		\
  { "sar", 1, "+" rm8rm16rm32},	\
  { "sar", 2, "ic"},		\
  { "sbb", 1, "+" rm8 "," rm16 "," rm32 "," r8 "," r16 "," r32},\
  { "sbb", 2, ri8 "," ri16 "," ri32 "," m8 "," m16 "," m32},\
  { "scas", 1, m8m16m32M64},	\
  { "seta", 1, "=qm"},		\
  { "setae", 1, "=qm"},		\
  { "setb", 1, "=qm"},		\
  { "setbe", 1, "=qm"},		\
  { "setc", 1, "=qm"},		\
  { "sete", 1, "=qm"},		\
  { "setg", 1, "=qm"},		\
  { "setge", 1, "=qm"},		\
  { "setl", 1, "=qm"},		\
  { "setle", 1, "=qm"},		\
  { "setna", 1, "=qm"},		\
  { "setnae", 1, "=qm"},       	\
  { "setnb", 1, "=qm"},		\
  { "setnbe", 1, "=qm"},       	\
  { "setnc", 1, "=qm"},		\
  { "setne", 1, "=qm"},		\
  { "setng", 1, "=qm"},		\
  { "setnge", 1, "=qm"},       	\
  { "setnl", 1, "=qm"},		\
  { "setnle", 1, "=qm"},	\
  { "setno", 1, "=qm"},		\
  { "setnp", 1, "=qm"},		\
  { "setns", 1, "=qm"},		\
  { "setnz", 1, "=qm"},		\
  { "seto", 1, "=qm"},		\
  { "setp", 1, "=qm"},		\
  { "setpe", 1, "=qm"},		\
  { "setpo", 1, "=qm"},		\
  { "sets", 1, "=qm"},		\
  { "setz", 1, "=qm"},		\
  { NY "sgdt", 1, "=m"},	\
  { "shl", 1, "+" rm8rm16rm32},	\
  { "shl", 2, "ic"},		\
  { "shld", 1, "+" rm16 "," rm32 C RM64},\
  { "shld", 2, r16 "," r32 C R64},\
  { "shld", 3, "ic,ic" X(",ic")},\
  { "shr", 1, "+" rm8rm16rm32},	\
  { "shr", 2, "ic"},		\
  { "shrd", 1, "+" rm16 "," rm32 C RM64},\
  { "shrd", 2, r16 "," r32 C R64},\
  { "shrd", 3, "ic,ic" X(",ic")}, \
  { "shufpd", 1, "+x"},		\
  { "shufpd", 2, "xm"},		\
  { "shufpd", 3, "i"},		\
  { "shufps", 1, "+x"},		\
  { "shufps", 2, "xm"},		\
  { "shufps", 3, "i"},		\
  { NY "sidt", 1, "=m"},	\
  { "sldt", 1, "=q" S("2") "m"},\
  { "smsw", 1, "=q" S("2") "m"},\
  { "sqrtpd", 1, "=x"},		\
  { "sqrtpd", 2, "xm"},		\
  { "sqrtps", 1, "=x"},		\
  { "sqrtps", 2, "xm"},		\
  { "sqrtsd", 1, "=x"},		\
  { "sqrtsd", 2, "xm"},		\
  { "sqrtss", 1, "=x"},		\
  { "sqrtss", 2, "xm"},		\
  { "stmxcsr", 1, "m"},		\
  { "stos", 1, "=m"},		\
  { "str", 1, "=q" S("2") "m"},\
  { "sub", 1, "+" rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64},\
  { "sub", 2, ri8 "," ri16 "," ri32 C RI64 "," m8 "," m16 "," m32 C M64},\
  { "subpd", 1, "+x"},		\
  { "subpd", 2, "xm"},		\
  { "subps", 1, "+x"},		\
  { "subps", 2, "xm"},		\
  { "subsd", 1, "+x"},		\
  { "subsd", 2, "xm"},		\
  { "subss", 1, "+x"},		\
  { "subss", 2, "xm"},		\
  { "test", 1, "+r," rm8rm16rm32},\
  { "test", 2, "r,i"},		\
  { "ucomisd", 1, "+x"},	\
  { "ucomisd", 2, "xm"},	\
  { "ucomiss", 1, "+x"},	\
  { "ucomiss", 2, "xm"},	\
  { "unpckhpd", 1, "+x"},	\
  { "unpckhpd", 2, "xm"},	\
  { "unpckhps", 1, "+x"},	\
  { "unpckhps", 2, "xm"},	\
  { "unpcklpd", 1, "+x"},	\
  { "unpcklpd", 2, "xm"},	\
  { "unpcklps", 1, "+x"},	\
  { "unpcklps", 2, "xm"},	\
  { "verr", 1, rm16},		\
  { "verw", 1, rm16},		\
  { "xadd", 1, "+" rm8 "," rm16 "," rm32},\
  { "xadd", 2, r8 "," r16 "," r32},\
  { "xchg", 1, "+" rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64},\
  { "xchg", 2, "+" r8 "," r16 "," r32 C R64 "," m8 "," m16 "," m32 C M64},\
  { "xlat", 1, "m"},		\
  { "xor", 1, "+" rm8 "," rm16 "," rm32 C RM64 "," r8 "," r16 "," r32 C R64},\
  { "xor", 2, ri8 "," ri16 "," ri32 C RI64 "," m8 "," m16 "," m32 C M64},\
  { "xorpd", 1, "+x"},		\
  { "xorpd", 2, "xm"},		\
  { "xorps", 1, "+x"},		\
  { "xorps", 2, "xm"},
/* LLVM LOCAL end change some = to + */

#define TARGET_IASM_EXTRA_CLOBBERS \
  { "rdtsc", { "edx", "eax"} }

#define IASM_FUNCTION_MODIFIER "P"

#define IASM_REGISTER_NAME(STR, BUF) i386_iasm_register_name (STR, BUF)

/* APPLE LOCAL end CW asm blocks */

/* Flag to mark data that is in the large address area.  */
#define SYMBOL_FLAG_FAR_ADDR		(SYMBOL_FLAG_MACH_DEP << 0)
#define SYMBOL_REF_FAR_ADDR_P(X)	\
	((SYMBOL_REF_FLAGS (X) & SYMBOL_FLAG_FAR_ADDR) != 0)

/* LLVM LOCAL begin */
/* Codes for all the SSE/MMX builtins.  */
enum ix86_builtins
{
  IX86_BUILTIN_ADDPS,
  IX86_BUILTIN_ADDSS,
  IX86_BUILTIN_DIVPS,
  IX86_BUILTIN_DIVSS,
  IX86_BUILTIN_MULPS,
  IX86_BUILTIN_MULSS,
  IX86_BUILTIN_SUBPS,
  IX86_BUILTIN_SUBSS,

  IX86_BUILTIN_CMPEQPS,
  IX86_BUILTIN_CMPLTPS,
  IX86_BUILTIN_CMPLEPS,
  IX86_BUILTIN_CMPGTPS,
  IX86_BUILTIN_CMPGEPS,
  IX86_BUILTIN_CMPNEQPS,
  IX86_BUILTIN_CMPNLTPS,
  IX86_BUILTIN_CMPNLEPS,
  IX86_BUILTIN_CMPNGTPS,
  IX86_BUILTIN_CMPNGEPS,
  IX86_BUILTIN_CMPORDPS,
  IX86_BUILTIN_CMPUNORDPS,
  IX86_BUILTIN_CMPNEPS,
  IX86_BUILTIN_CMPEQSS,
  IX86_BUILTIN_CMPLTSS,
  IX86_BUILTIN_CMPLESS,
  IX86_BUILTIN_CMPNEQSS,
  IX86_BUILTIN_CMPNLTSS,
  IX86_BUILTIN_CMPNLESS,
  IX86_BUILTIN_CMPNGTSS,
  IX86_BUILTIN_CMPNGESS,
  IX86_BUILTIN_CMPORDSS,
  IX86_BUILTIN_CMPUNORDSS,
  IX86_BUILTIN_CMPNESS,

  IX86_BUILTIN_COMIEQSS,
  IX86_BUILTIN_COMILTSS,
  IX86_BUILTIN_COMILESS,
  IX86_BUILTIN_COMIGTSS,
  IX86_BUILTIN_COMIGESS,
  IX86_BUILTIN_COMINEQSS,
  IX86_BUILTIN_UCOMIEQSS,
  IX86_BUILTIN_UCOMILTSS,
  IX86_BUILTIN_UCOMILESS,
  IX86_BUILTIN_UCOMIGTSS,
  IX86_BUILTIN_UCOMIGESS,
  IX86_BUILTIN_UCOMINEQSS,

  IX86_BUILTIN_CVTPI2PS,
  IX86_BUILTIN_CVTPS2PI,
  IX86_BUILTIN_CVTSI2SS,
  IX86_BUILTIN_CVTSI642SS,
  IX86_BUILTIN_CVTSS2SI,
  IX86_BUILTIN_CVTSS2SI64,
  IX86_BUILTIN_CVTTPS2PI,
  IX86_BUILTIN_CVTTSS2SI,
  IX86_BUILTIN_CVTTSS2SI64,

  IX86_BUILTIN_MAXPS,
  IX86_BUILTIN_MAXSS,
  IX86_BUILTIN_MINPS,
  IX86_BUILTIN_MINSS,

  IX86_BUILTIN_LOADUPS,
  IX86_BUILTIN_STOREUPS,
  IX86_BUILTIN_MOVSS,

  IX86_BUILTIN_MOVHLPS,
  IX86_BUILTIN_MOVLHPS,
  IX86_BUILTIN_LOADHPS,
  IX86_BUILTIN_LOADLPS,
  IX86_BUILTIN_STOREHPS,
  IX86_BUILTIN_STORELPS,

  IX86_BUILTIN_MASKMOVQ,
  IX86_BUILTIN_MOVMSKPS,
  IX86_BUILTIN_PMOVMSKB,

  IX86_BUILTIN_MOVNTPS,
  IX86_BUILTIN_MOVNTQ,

  IX86_BUILTIN_LOADDQU,
  IX86_BUILTIN_STOREDQU,

  IX86_BUILTIN_PACKSSWB,
  IX86_BUILTIN_PACKSSDW,
  IX86_BUILTIN_PACKUSWB,

  IX86_BUILTIN_PADDB,
  IX86_BUILTIN_PADDW,
  IX86_BUILTIN_PADDD,
  IX86_BUILTIN_PADDQ,
  IX86_BUILTIN_PADDSB,
  IX86_BUILTIN_PADDSW,
  IX86_BUILTIN_PADDUSB,
  IX86_BUILTIN_PADDUSW,
  IX86_BUILTIN_PSUBB,
  IX86_BUILTIN_PSUBW,
  IX86_BUILTIN_PSUBD,
  IX86_BUILTIN_PSUBQ,
  IX86_BUILTIN_PSUBSB,
  IX86_BUILTIN_PSUBSW,
  IX86_BUILTIN_PSUBUSB,
  IX86_BUILTIN_PSUBUSW,

  IX86_BUILTIN_PAND,
  IX86_BUILTIN_PANDN,
  IX86_BUILTIN_POR,
  IX86_BUILTIN_PXOR,

  IX86_BUILTIN_PAVGB,
  IX86_BUILTIN_PAVGW,

  IX86_BUILTIN_PCMPEQB,
  IX86_BUILTIN_PCMPEQW,
  IX86_BUILTIN_PCMPEQD,
  IX86_BUILTIN_PCMPGTB,
  IX86_BUILTIN_PCMPGTW,
  IX86_BUILTIN_PCMPGTD,

  IX86_BUILTIN_PMADDWD,

  IX86_BUILTIN_PMAXSW,
  IX86_BUILTIN_PMAXUB,
  IX86_BUILTIN_PMINSW,
  IX86_BUILTIN_PMINUB,

  IX86_BUILTIN_PMULHUW,
  IX86_BUILTIN_PMULHW,
  IX86_BUILTIN_PMULLW,

  IX86_BUILTIN_PSADBW,
  IX86_BUILTIN_PSHUFW,

  IX86_BUILTIN_PSLLW,
  IX86_BUILTIN_PSLLD,
  IX86_BUILTIN_PSLLQ,
  IX86_BUILTIN_PSRAW,
  IX86_BUILTIN_PSRAD,
  IX86_BUILTIN_PSRLW,
  IX86_BUILTIN_PSRLD,
  IX86_BUILTIN_PSRLQ,
  IX86_BUILTIN_PSLLWI,
  IX86_BUILTIN_PSLLDI,
  IX86_BUILTIN_PSLLQI,
  IX86_BUILTIN_PSRAWI,
  IX86_BUILTIN_PSRADI,
  IX86_BUILTIN_PSRLWI,
  IX86_BUILTIN_PSRLDI,
  IX86_BUILTIN_PSRLQI,

  IX86_BUILTIN_PUNPCKHBW,
  IX86_BUILTIN_PUNPCKHWD,
  IX86_BUILTIN_PUNPCKHDQ,
  IX86_BUILTIN_PUNPCKLBW,
  IX86_BUILTIN_PUNPCKLWD,
  IX86_BUILTIN_PUNPCKLDQ,

  IX86_BUILTIN_SHUFPS,

  IX86_BUILTIN_RCPPS,
  IX86_BUILTIN_RCPSS,
  IX86_BUILTIN_RSQRTPS,
  IX86_BUILTIN_RSQRTSS,
  IX86_BUILTIN_SQRTPS,
  IX86_BUILTIN_SQRTSS,

  IX86_BUILTIN_UNPCKHPS,
  IX86_BUILTIN_UNPCKLPS,

  IX86_BUILTIN_ANDPS,
  IX86_BUILTIN_ANDNPS,
  IX86_BUILTIN_ORPS,
  IX86_BUILTIN_XORPS,

  IX86_BUILTIN_EMMS,
  IX86_BUILTIN_LDMXCSR,
  IX86_BUILTIN_STMXCSR,
  IX86_BUILTIN_SFENCE,

  /* 3DNow! Original */
  IX86_BUILTIN_FEMMS,
  IX86_BUILTIN_PAVGUSB,
  IX86_BUILTIN_PF2ID,
  IX86_BUILTIN_PFACC,
  IX86_BUILTIN_PFADD,
  IX86_BUILTIN_PFCMPEQ,
  IX86_BUILTIN_PFCMPGE,
  IX86_BUILTIN_PFCMPGT,
  IX86_BUILTIN_PFMAX,
  IX86_BUILTIN_PFMIN,
  IX86_BUILTIN_PFMUL,
  IX86_BUILTIN_PFRCP,
  IX86_BUILTIN_PFRCPIT1,
  IX86_BUILTIN_PFRCPIT2,
  IX86_BUILTIN_PFRSQIT1,
  IX86_BUILTIN_PFRSQRT,
  IX86_BUILTIN_PFSUB,
  IX86_BUILTIN_PFSUBR,
  IX86_BUILTIN_PI2FD,
  IX86_BUILTIN_PMULHRW,

  /* 3DNow! Athlon Extensions */
  IX86_BUILTIN_PF2IW,
  IX86_BUILTIN_PFNACC,
  IX86_BUILTIN_PFPNACC,
  IX86_BUILTIN_PI2FW,
  IX86_BUILTIN_PSWAPDSI,
  IX86_BUILTIN_PSWAPDSF,

  /* SSE2 */
  IX86_BUILTIN_ADDPD,
  IX86_BUILTIN_ADDSD,
  IX86_BUILTIN_DIVPD,
  IX86_BUILTIN_DIVSD,
  IX86_BUILTIN_MULPD,
  IX86_BUILTIN_MULSD,
  IX86_BUILTIN_SUBPD,
  IX86_BUILTIN_SUBSD,

  IX86_BUILTIN_CMPEQPD,
  IX86_BUILTIN_CMPLTPD,
  IX86_BUILTIN_CMPLEPD,
  IX86_BUILTIN_CMPGTPD,
  IX86_BUILTIN_CMPGEPD,
  IX86_BUILTIN_CMPNEQPD,
  IX86_BUILTIN_CMPNLTPD,
  IX86_BUILTIN_CMPNLEPD,
  IX86_BUILTIN_CMPNGTPD,
  IX86_BUILTIN_CMPNGEPD,
  IX86_BUILTIN_CMPORDPD,
  IX86_BUILTIN_CMPUNORDPD,
  IX86_BUILTIN_CMPNEPD,
  IX86_BUILTIN_CMPEQSD,
  IX86_BUILTIN_CMPLTSD,
  IX86_BUILTIN_CMPLESD,
  IX86_BUILTIN_CMPNEQSD,
  IX86_BUILTIN_CMPNLTSD,
  IX86_BUILTIN_CMPNLESD,
  IX86_BUILTIN_CMPORDSD,
  IX86_BUILTIN_CMPUNORDSD,
  IX86_BUILTIN_CMPNESD,

  IX86_BUILTIN_COMIEQSD,
  IX86_BUILTIN_COMILTSD,
  IX86_BUILTIN_COMILESD,
  IX86_BUILTIN_COMIGTSD,
  IX86_BUILTIN_COMIGESD,
  IX86_BUILTIN_COMINEQSD,
  IX86_BUILTIN_UCOMIEQSD,
  IX86_BUILTIN_UCOMILTSD,
  IX86_BUILTIN_UCOMILESD,
  IX86_BUILTIN_UCOMIGTSD,
  IX86_BUILTIN_UCOMIGESD,
  IX86_BUILTIN_UCOMINEQSD,

  IX86_BUILTIN_MAXPD,
  IX86_BUILTIN_MAXSD,
  IX86_BUILTIN_MINPD,
  IX86_BUILTIN_MINSD,

  IX86_BUILTIN_ANDPD,
  IX86_BUILTIN_ANDNPD,
  IX86_BUILTIN_ORPD,
  IX86_BUILTIN_XORPD,

  IX86_BUILTIN_SQRTPD,
  IX86_BUILTIN_SQRTSD,

  IX86_BUILTIN_UNPCKHPD,
  IX86_BUILTIN_UNPCKLPD,

  IX86_BUILTIN_SHUFPD,

  IX86_BUILTIN_LOADUPD,
  IX86_BUILTIN_STOREUPD,
  IX86_BUILTIN_MOVSD,

  IX86_BUILTIN_LOADHPD,
  IX86_BUILTIN_LOADLPD,

  IX86_BUILTIN_CVTDQ2PD,
  IX86_BUILTIN_CVTDQ2PS,

  IX86_BUILTIN_CVTPD2DQ,
  IX86_BUILTIN_CVTPD2PI,
  IX86_BUILTIN_CVTPD2PS,
  IX86_BUILTIN_CVTTPD2DQ,
  IX86_BUILTIN_CVTTPD2PI,

  IX86_BUILTIN_CVTPI2PD,
  IX86_BUILTIN_CVTSI2SD,
  IX86_BUILTIN_CVTSI642SD,

  IX86_BUILTIN_CVTSD2SI,
  IX86_BUILTIN_CVTSD2SI64,
  IX86_BUILTIN_CVTSD2SS,
  IX86_BUILTIN_CVTSS2SD,
  IX86_BUILTIN_CVTTSD2SI,
  IX86_BUILTIN_CVTTSD2SI64,

  IX86_BUILTIN_CVTPS2DQ,
  IX86_BUILTIN_CVTPS2PD,
  IX86_BUILTIN_CVTTPS2DQ,

  IX86_BUILTIN_MOVNTI,
  IX86_BUILTIN_MOVNTPD,
  IX86_BUILTIN_MOVNTDQ,

  /* SSE2 MMX */
  IX86_BUILTIN_MASKMOVDQU,
  IX86_BUILTIN_MOVMSKPD,
  IX86_BUILTIN_PMOVMSKB128,

  /* APPLE LOCAL begin 4099020 */
  IX86_BUILTIN_MOVQ,
  IX86_BUILTIN_LOADQ,
  IX86_BUILTIN_STOREQ,
  /* APPLE LOCAL end 4099020 */

  IX86_BUILTIN_PACKSSWB128,
  IX86_BUILTIN_PACKSSDW128,
  IX86_BUILTIN_PACKUSWB128,

  IX86_BUILTIN_PADDB128,
  IX86_BUILTIN_PADDW128,
  IX86_BUILTIN_PADDD128,
  IX86_BUILTIN_PADDQ128,
  IX86_BUILTIN_PADDSB128,
  IX86_BUILTIN_PADDSW128,
  IX86_BUILTIN_PADDUSB128,
  IX86_BUILTIN_PADDUSW128,
  IX86_BUILTIN_PSUBB128,
  IX86_BUILTIN_PSUBW128,
  IX86_BUILTIN_PSUBD128,
  IX86_BUILTIN_PSUBQ128,
  IX86_BUILTIN_PSUBSB128,
  IX86_BUILTIN_PSUBSW128,
  IX86_BUILTIN_PSUBUSB128,
  IX86_BUILTIN_PSUBUSW128,

  IX86_BUILTIN_PAND128,
  IX86_BUILTIN_PANDN128,
  IX86_BUILTIN_POR128,
  IX86_BUILTIN_PXOR128,

  IX86_BUILTIN_PAVGB128,
  IX86_BUILTIN_PAVGW128,

  IX86_BUILTIN_PCMPEQB128,
  IX86_BUILTIN_PCMPEQW128,
  IX86_BUILTIN_PCMPEQD128,
  IX86_BUILTIN_PCMPGTB128,
  IX86_BUILTIN_PCMPGTW128,
  IX86_BUILTIN_PCMPGTD128,

  IX86_BUILTIN_PMADDWD128,

  IX86_BUILTIN_PMAXSW128,
  IX86_BUILTIN_PMAXUB128,
  IX86_BUILTIN_PMINSW128,
  IX86_BUILTIN_PMINUB128,

  IX86_BUILTIN_PMULUDQ,
  IX86_BUILTIN_PMULUDQ128,
  IX86_BUILTIN_PMULHUW128,
  IX86_BUILTIN_PMULHW128,
  IX86_BUILTIN_PMULLW128,

  IX86_BUILTIN_PSADBW128,
  IX86_BUILTIN_PSHUFHW,
  IX86_BUILTIN_PSHUFLW,
  IX86_BUILTIN_PSHUFD,

  IX86_BUILTIN_PSLLW128,
  IX86_BUILTIN_PSLLD128,
  IX86_BUILTIN_PSLLQ128,
  IX86_BUILTIN_PSRAW128,
  IX86_BUILTIN_PSRAD128,
  IX86_BUILTIN_PSRLW128,
  IX86_BUILTIN_PSRLD128,
  IX86_BUILTIN_PSRLQ128,
  IX86_BUILTIN_PSLLDQI128,
  /* APPLE LOCAL 591583 */
  IX86_BUILTIN_PSLLDQI128_BYTESHIFT,
  IX86_BUILTIN_PSLLWI128,
  IX86_BUILTIN_PSLLDI128,
  IX86_BUILTIN_PSLLQI128,
  IX86_BUILTIN_PSRAWI128,
  IX86_BUILTIN_PSRADI128,
  IX86_BUILTIN_PSRLDQI128,
  /* APPLE LOCAL 591583 */
  IX86_BUILTIN_PSRLDQI128_BYTESHIFT,
  IX86_BUILTIN_PSRLWI128,
  IX86_BUILTIN_PSRLDI128,
  IX86_BUILTIN_PSRLQI128,

  IX86_BUILTIN_PUNPCKHBW128,
  IX86_BUILTIN_PUNPCKHWD128,
  IX86_BUILTIN_PUNPCKHDQ128,
  IX86_BUILTIN_PUNPCKHQDQ128,
  IX86_BUILTIN_PUNPCKLBW128,
  IX86_BUILTIN_PUNPCKLWD128,
  IX86_BUILTIN_PUNPCKLDQ128,
  IX86_BUILTIN_PUNPCKLQDQ128,

  IX86_BUILTIN_CLFLUSH,
  IX86_BUILTIN_MFENCE,
  IX86_BUILTIN_LFENCE,

  /* Prescott New Instructions.  */
  IX86_BUILTIN_ADDSUBPS,
  IX86_BUILTIN_HADDPS,
  IX86_BUILTIN_HSUBPS,
  IX86_BUILTIN_MOVSHDUP,
  IX86_BUILTIN_MOVSLDUP,
  IX86_BUILTIN_ADDSUBPD,
  IX86_BUILTIN_HADDPD,
  IX86_BUILTIN_HSUBPD,
  IX86_BUILTIN_LDDQU,

  IX86_BUILTIN_MONITOR,
  IX86_BUILTIN_MWAIT,

  /* Merom New Instructions.  */
  IX86_BUILTIN_PHADDW,
  IX86_BUILTIN_PHADDD,
  IX86_BUILTIN_PHADDSW,
  IX86_BUILTIN_PHSUBW,
  IX86_BUILTIN_PHSUBD,
  IX86_BUILTIN_PHSUBSW,
  IX86_BUILTIN_PMADDUBSW,
  IX86_BUILTIN_PMULHRSW,
  IX86_BUILTIN_PSHUFB,
  IX86_BUILTIN_PSIGNB,
  IX86_BUILTIN_PSIGNW,
  IX86_BUILTIN_PSIGND,
  IX86_BUILTIN_PALIGNR,
  IX86_BUILTIN_PABSB,
  IX86_BUILTIN_PABSW,
  IX86_BUILTIN_PABSD,

  IX86_BUILTIN_PHADDW128,
  IX86_BUILTIN_PHADDD128,
  IX86_BUILTIN_PHADDSW128,
  IX86_BUILTIN_PHSUBW128,
  IX86_BUILTIN_PHSUBD128,
  IX86_BUILTIN_PHSUBSW128,
  IX86_BUILTIN_PMADDUBSW128,
  IX86_BUILTIN_PMULHRSW128,
  IX86_BUILTIN_PSHUFB128,
  IX86_BUILTIN_PSIGNB128,
  IX86_BUILTIN_PSIGNW128,
  IX86_BUILTIN_PSIGND128,
  IX86_BUILTIN_PALIGNR128,
  IX86_BUILTIN_PABSB128,
  IX86_BUILTIN_PABSW128,
  IX86_BUILTIN_PABSD128,
  /* APPLE LOCAL begin 5612787 mainline sse4 */
  /* AMDFAM10 - SSE4A New Instructions.  */
  IX86_BUILTIN_MOVNTSD,
  IX86_BUILTIN_MOVNTSS,
  IX86_BUILTIN_EXTRQI,
  IX86_BUILTIN_EXTRQ,
  IX86_BUILTIN_INSERTQI,
  IX86_BUILTIN_INSERTQ,

  /* SSE4.1.  */
  IX86_BUILTIN_BLENDPD,
  IX86_BUILTIN_BLENDPS,
  IX86_BUILTIN_BLENDVPD,
  IX86_BUILTIN_BLENDVPS,
  IX86_BUILTIN_PBLENDVB128,
  IX86_BUILTIN_PBLENDW128,

  IX86_BUILTIN_DPPD,
  IX86_BUILTIN_DPPS,

  IX86_BUILTIN_INSERTPS128,

  IX86_BUILTIN_MOVNTDQA,
  IX86_BUILTIN_MPSADBW128,
  IX86_BUILTIN_PACKUSDW128,
  IX86_BUILTIN_PCMPEQQ,
  IX86_BUILTIN_PHMINPOSUW128,

  IX86_BUILTIN_PMAXSB128,
  IX86_BUILTIN_PMAXSD128,
  IX86_BUILTIN_PMAXUD128,
  IX86_BUILTIN_PMAXUW128,

  IX86_BUILTIN_PMINSB128,
  IX86_BUILTIN_PMINSD128,
  IX86_BUILTIN_PMINUD128,
  IX86_BUILTIN_PMINUW128,

  IX86_BUILTIN_PMOVSXBW128,
  IX86_BUILTIN_PMOVSXBD128,
  IX86_BUILTIN_PMOVSXBQ128,
  IX86_BUILTIN_PMOVSXWD128,
  IX86_BUILTIN_PMOVSXWQ128,
  IX86_BUILTIN_PMOVSXDQ128,

  IX86_BUILTIN_PMOVZXBW128,
  IX86_BUILTIN_PMOVZXBD128,
  IX86_BUILTIN_PMOVZXBQ128,
  IX86_BUILTIN_PMOVZXWD128,
  IX86_BUILTIN_PMOVZXWQ128,
  IX86_BUILTIN_PMOVZXDQ128,

  IX86_BUILTIN_PMULDQ128,
  IX86_BUILTIN_PMULLD128,

  IX86_BUILTIN_ROUNDPD,
  IX86_BUILTIN_ROUNDPS,
  IX86_BUILTIN_ROUNDSD,
  IX86_BUILTIN_ROUNDSS,

  IX86_BUILTIN_PTESTZ,
  IX86_BUILTIN_PTESTC,
  IX86_BUILTIN_PTESTNZC,
  /* APPLE LOCAL end 5612787 mainline sse4 */
  /* APPLE LOCAL end mainline */
  IX86_BUILTIN_VEC_INIT_V2SI,
  IX86_BUILTIN_VEC_INIT_V4HI,
  IX86_BUILTIN_VEC_INIT_V8QI,
  IX86_BUILTIN_VEC_EXT_V2DF,
  IX86_BUILTIN_VEC_EXT_V2DI,
  IX86_BUILTIN_VEC_EXT_V4SF,
  IX86_BUILTIN_VEC_EXT_V4SI,
  IX86_BUILTIN_VEC_EXT_V8HI,
  /* APPLE LOCAL begin 5612787 mainline sse4 */
  /* deletion */
  /* APPLE LOCAL end 5612787 mainline sse4 */
  IX86_BUILTIN_VEC_EXT_V2SI,
  IX86_BUILTIN_VEC_EXT_V4HI,
  /* APPLE LOCAL begin 5612787 mainline sse4 */
  IX86_BUILTIN_VEC_EXT_V16QI,
  IX86_BUILTIN_VEC_SET_V2DI,
  IX86_BUILTIN_VEC_SET_V4SF,
  IX86_BUILTIN_VEC_SET_V4SI,
  /* APPLE LOCAL end 5612787 mainline sse4 */
  IX86_BUILTIN_VEC_SET_V8HI,
  IX86_BUILTIN_VEC_SET_V4HI,
  /* APPLE LOCAL begin 5612787 mainline sse4 */
  IX86_BUILTIN_VEC_SET_V16QI,

  IX86_BUILTIN_VEC_PACK_SFIX,

  /* SSE4.2.  */
  IX86_BUILTIN_CRC32QI,
  IX86_BUILTIN_CRC32HI,
  IX86_BUILTIN_CRC32SI,
  IX86_BUILTIN_CRC32DI,

  IX86_BUILTIN_PCMPESTRI128,
  IX86_BUILTIN_PCMPESTRM128,
  IX86_BUILTIN_PCMPESTRA128,
  IX86_BUILTIN_PCMPESTRC128,
  IX86_BUILTIN_PCMPESTRO128,
  IX86_BUILTIN_PCMPESTRS128,
  IX86_BUILTIN_PCMPESTRZ128,
  IX86_BUILTIN_PCMPISTRI128,
  IX86_BUILTIN_PCMPISTRM128,
  IX86_BUILTIN_PCMPISTRA128,
  IX86_BUILTIN_PCMPISTRC128,
  IX86_BUILTIN_PCMPISTRO128,
  IX86_BUILTIN_PCMPISTRS128,
  IX86_BUILTIN_PCMPISTRZ128,

  IX86_BUILTIN_PCMPGTQ,

  /* TFmode support builtins.  */
  IX86_BUILTIN_INFQ,
  IX86_BUILTIN_FABSQ,
  IX86_BUILTIN_COPYSIGNQ,
  /* APPLE LOCAL end 5612787 mainline sse4 */

  IX86_BUILTIN_MAX
};

#ifdef ENABLE_LLVM

/* LLVM_TARGET_INTRINSIC_PREFIX - Specify what prefix this target uses for its
 * intrinsics.
 */
#define LLVM_TARGET_INTRINSIC_PREFIX "x86"

/* LLVM_TARGET_NAME - This specifies the name of the target, which correlates to
 * the llvm::InitializeXXXTarget() function.
 */
#define LLVM_TARGET_NAME X86

/* Turn -march=xx into a CPU type.
 */
#define LLVM_SET_SUBTARGET_FEATURES(F) \
  { if (TARGET_MACHO && ! strcmp (ix86_arch_string, "apple")) \
      F.setCPU(TARGET_64BIT ? "core2" : "yonah");             \
    else                                                      \
      F.setCPU(ix86_arch_string);                             \
    if (TARGET_64BIT)   F.AddFeature("64bit");                \
    if (TARGET_MMX)     F.AddFeature("mmx");                  \
    else if (target_flags_explicit & MASK_MMX) F.AddFeature("mmx", false); \
    if (TARGET_SSE)     F.AddFeature("sse");                  \
    else if (target_flags_explicit & MASK_SSE) F.AddFeature("sse", false); \
    if (TARGET_SSE2)    F.AddFeature("sse2");                 \
    else if (target_flags_explicit & MASK_SSE2) F.AddFeature("sse2", false); \
    if (TARGET_SSE3)    F.AddFeature("sse3");                 \
    else if (target_flags_explicit & MASK_SSE3) F.AddFeature("sse3", false); \
    if (TARGET_SSSE3)   F.AddFeature("ssse3");                \
    if (TARGET_SSE4_1)  F.AddFeature("sse41");                \
    if (TARGET_SSE4_2)  F.AddFeature("sse42");                \
    if (TARGET_3DNOW)   F.AddFeature("3dnow");                \
    if (TARGET_3DNOW_A) F.AddFeature("3dnowa");               \
  }

#define LLVM_SET_IMPLICIT_FLOAT(flag_no_implicit_float)       \
  if (!TARGET_80387)                                          \
    flag_no_implicit_float = 1;                               \
  else                                                        \
    flag_no_implicit_float = 0;                               
    
/* LLVM ABI definition macros. */

/* When -m64 is specified, set the architecture to x86_64-os-blah even if the
 * compiler was configured for i[3456]86-os-blah.
 */
#define LLVM_OVERRIDE_TARGET_ARCH() \
  (TARGET_64BIT ? "x86_64" : "i386")

/* LLVM_TARGET_INTRINSIC_LOWER - To handle builtins, we want to expand the
 * invocation into normal LLVM code.  If the target can handle the builtin, this
 * macro should call the target TreeToLLVM::TargetIntrinsicLower method and
 *  return true.This macro is invoked from a method in the TreeToLLVM class.
 */
#define LLVM_TARGET_INTRINSIC_LOWER(EXP, BUILTIN_CODE, DESTLOC, RESULT,       \
                                    DESTTY, OPS)                              \
        TargetIntrinsicLower(EXP, BUILTIN_CODE, DESTLOC, RESULT, DESTTY, OPS);

/* LLVM_GET_REG_NAME - When extracting a register name for a constraint, use
   the string extracted from the magic symbol built for that register, rather
   than reg_names.  The latter maps both AH and AL to the same thing, which
   means we can't distinguish them. */
#define LLVM_GET_REG_NAME(REG_NAME, REG_NUM) __extension__ \
  ({ const char *nm = (REG_NAME); \
     if (nm && (*nm == '%' || *nm == '#')) ++nm; \
     ((!nm || ISDIGIT (*nm)) ? reg_names[REG_NUM] : nm); })

/* LLVM_CANONICAL_ADDRESS_CONSTRAINTS - Valid x86 memory addresses include
   symbolic values and immediates.  Canonicalize GCC's "p" constraint for
   memory addresses to allow both memory and immediate operands. */
#define LLVM_CANONICAL_ADDRESS_CONSTRAINTS "im"

/* Propagate code model setting to backend */
#define LLVM_SET_MACHINE_OPTIONS(argvec)                \
  do {                                                  \
    switch (ix86_cmodel) {                              \
    default:                                            \
      sorry ("code model %<%s%> not supported yet",     \
             ix86_cmodel_string);                       \
      break;                                            \
    case CM_SMALL:                                      \
    case CM_SMALL_PIC:                                  \
      argvec.push_back("--code-model=small");           \
      break;                                            \
    case CM_KERNEL:                                     \
      argvec.push_back("--code-model=kernel");          \
      break;                                            \
    case CM_MEDIUM:                                     \
    case CM_MEDIUM_PIC:                                 \
      argvec.push_back("--code-model=medium");          \
      break;                                            \
    case CM_32:                                         \
      argvec.push_back("--code-model=default");         \
      break;                                            \
    }                                                   \
    /* A value of 3 in flag_omit_frame_pointer implies  \
       omitting leaf frame pointers only.  */           \
    if (flag_omit_frame_pointer == 3)                   \
      argvec.push_back("--disable-non-leaf-fp-elim");   \
                                                        \
    if (ix86_force_align_arg_pointer)                   \
      argvec.push_back("-force-align-stack");           \
  } while (0)
#endif /* ENABLE_LLVM */
/* LLVM LOCAL end */

/*
Local variables:
version-control: t
End:
*/
