/*
** Copyright (c) 2013 Adobe Systems Inc

** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:

** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
*/

/* LLVM LOCAL begin - AVM2 target */
/*
This is part of minimal implementation for being able to
compile llvm-gcc with avm2 target.

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
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* TODO take a pass over this trying to remove more x86 specific stuff TRAMPOLINE stuff, register related stuff, etc. */

/* Identify our target */
#define TARGET_AVM2 1

/* testing our assembler directly won't work,
   so we have to specify this here */
#define HAVE_GAS_HIDDEN 1

/* This declaration should be present.  */
extern int target_flags;
extern int flag_function_sections;

/* To make llvm-backend.cpp recognize us */
#define LLVM_OVERRIDE_TARGET_ARCH() "avm2"
#define LLVM_TARGET_NAME AVM2Shim

#define TARGET_CPU_CPP_BUILTINS()	   \
  do						           \
    {						           \
      builtin_define ("__AVM2__"); \
      builtin_define ("__AVM2"); \
      builtin_define ("__FLASHPLAYER"); \
      builtin_define ("__FLASHPLAYER__"); \
 \
      if(enable_pthreads) { \
        if(!swf_version_internal || atoi(swf_version_internal) < 18) { \
          warning(0, "SWF version must be at least 18 to use pthreads, your SWF version will be forced to 18."); \
          swf_version = swf_version_internal = xstrdup("18"); \
        } \
      } \
      if (flag_blocks) { \
        builtin_define ("__block=__attribute__((__blocks__(byref)))"); \
      } \
 \
      char swfver[128] = {}; \
      sprintf(&swfver[0], "__SWF_VER=%s", swf_version_internal); \
      cpp_define (pfile, xstrdup(&swfver[0])); \
      sprintf(&swfver[0], "__SWF_VER__=%s", swf_version_internal); \
      cpp_define (pfile, xstrdup(&swfver[0])); \
    } \
   while (0)

/* LLVM specific stuff for supporting calling convention output */
#define TARGET_ADJUST_LLVM_CC(CC, type)                         \
  {                                                             \
    tree type_attributes = TYPE_ATTRIBUTES (type);              \
    if (lookup_attribute ("stdcall", type_attributes)) {        \
      CC = CallingConv::X86_StdCall;                            \
    } else if (lookup_attribute("fastcall", type_attributes)) { \
      CC = CallingConv::X86_FastCall;                           \
    }                                                           \
  }

#define TARGET_ADJUST_LLVM_RETATTR(Rattributes, type)           \
  {                                                             \
    tree type_attributes = TYPE_ATTRIBUTES (type);              \
  }

/* LLVM specific stuff for converting gcc's `regparm` attribute to LLVM's
   `inreg` parameter attribute */
#define LLVM_TARGET_ENABLE_REGPARM

extern int avm2_regparm;

#define LLVM_TARGET_INIT_REGPARM(local_regparm, local_fp_regparm, type) \
  {                                                             \
    tree attr;                                                  \
    local_regparm = avm2_regparm;                               \
    local_fp_regparm = 0;               \
    attr = lookup_attribute ("regparm",                         \
                              TYPE_ATTRIBUTES (type));          \
    if (attr) {                                                 \
      local_regparm = TREE_INT_CST_LOW (TREE_VALUE              \
                                        (TREE_VALUE (attr)));   \
    }                                                           \
  }

#define LLVM_ADJUST_REGPARM_ATTRIBUTE(PAttribute, Type, Size,   \
                                      local_regparm,            \
                                      local_fp_regparm)         \
  {                                                             \
    if (!TARGET_64BIT) {                                        \
      if (TREE_CODE(Type) == REAL_TYPE &&                       \
          (TYPE_PRECISION(Type)==32 ||                          \
           TYPE_PRECISION(Type)==64)) {                         \
          local_fp_regparm -= 1;                                \
          if (local_fp_regparm >= 0)                            \
            PAttribute |= Attribute::InReg;                     \
          else                                                  \
            local_fp_regparm = 0;                               \
      } else if (INTEGRAL_TYPE_P(Type) ||                       \
                 POINTER_TYPE_P(Type)) {                        \
          int words =                                           \
                  (Size + BITS_PER_WORD - 1) / BITS_PER_WORD;   \
          local_regparm -= words;                               \
          if (local_regparm>=0)                                 \
            PAttribute |= Attribute::InReg;                     \
          else                                                  \
            local_regparm = 0;                                  \
      }                                                         \
    }                                                           \
  }

#define EXTRA_SPECS                                             \
  { "cc1_cpu",  "" },                                        	\
  SUBTARGET_EXTRA_SPECS

#define ASM_OUTPUT_IDENT(FILE, NAME)

#define SWFOPTS "--plugin-opt disableprojector --plugin-opt llc-opt --plugin-opt -target-player %{disable-telemetry: --plugin-opt disable-telemetry} %{!nostdlib: ISpecialFile.abc%s IBackingStore.abc%s IVFS.abc%s DefaultVFS.abc%s InMemoryBackingStore.abc%s PlayerKernel.abc%s --plugin-opt appendabc --plugin-opt PlayerCreateWorker.abc%s } "

#undef CC1_SPEC
#define CC1_SPEC "%{pthread: -pthread} -swf-version-internal=%{!swf-version=*:18}%{swf-version=*:%*} %{jvmopt=*:-jvmopt=%*} %{ascopt=*:-ascopt=%*} %{emit-swf|emit-swc=*: -target-player } "

/*
** TODO: re-enable once falcon is thread-safe!
** %{!muse-legacy-asc: %{!disable-falcon-parallel: -plugin-opt llc-opt --plugin-opt  -falcon-parallel} }
*/

#ifndef LINK_COMMAND_SPEC
#define str(s) #s
#define xstr(s) str(s)
#define LINK_COMMAND_SPEC "\
%{!fsyntax-only:%{!c:%{!M:%{!MM:%{!E:%{!S:\
    %(linker) \
     --plugin %smultiplug" xstr(SHARED_LIBRARY_EXTENSION) " \
        %{O4|emit-llvm|flto: ;: %:useabcstdlibs() } \
        %{pthread: --plugin-opt pthread} \
        %{muse-legacy-asc:--plugin-opt llc-opt --plugin-opt -use-legacy-as3-asm } \
        %{!muse-legacy-asc: %{!disable-falcon-parallel: -plugin-opt llc-opt --plugin-opt -falcon-parallel} } \
        %{!emit-swc=*: %{!emit-swf: %{!nostdlib: ISpecialFile.abc%s IBackingStore.abc%s IVFS.abc%s --plugin-opt appendabc --plugin-opt startHack.abc%s --plugin-opt appendabc --plugin-opt ShellCreateWorker.abc%s } } } \
        %{emit-swf: %<emit-swf " SWFOPTS " --plugin-opt symbolclass=%{!symbol-class=*:0:com.adobe.flascc::Console}%{symbol-class=*:%*} --plugin-opt appendabc --plugin-opt %{!symbol-abc=*:Console.abc%s}%{symbol-abc=*:%*} %{!no-swf-preloader: --plugin-opt swfpreloader --plugin-opt %{swf-preloader=*:%*;:DefaultPreloader.swf%s} \ } } \
        %{emit-swc=*: %<emit-swc=* " SWFOPTS " --plugin-opt emit-swc=%* } \
        %{enable-debugger: %<enable-debugger --plugin-opt enabledebugger } \
        %{swf-ns=*:--plugin-opt swf-ns=%*} \
        %{jvmopt=*:--plugin-opt jvmopt=%*} \
        %{g: --plugin-opt llc-opt --plugin-opt -gendbgsymtable --plugin-opt enabledebugger AlcDbgHelper.abc%s ELF.abc%s --plugin-opt codegen-opt --plugin-opt -disable-inlining} \
        %{!g:--plugin-opt codegen-opt --plugin-opt -strip-debug} \
        %{v:--plugin-opt verbose} \
        %{fllvm-llc-opt=*:--plugin-opt llc-opt --plugin-opt %*} \
        %{flto-api=*: --plugin-opt codegen-opt --plugin-opt -internalize-public-api-file=%* --plugin-opt codegen-opt --plugin-opt -internalize --plugin-opt codegen-opt --plugin-opt -globaldce --plugin-opt codegen-opt --plugin-opt -dce } \
        %{fllvm-opt-opt=*:--plugin-opt codegen-opt --plugin-opt %*} \
        %{ascopt=*:--plugin-opt ascopt=%*} \
        --plugin-opt obj-path=%{save-temps:%g.lto.abc;:%d%u.lto.abc} --plugin-opt also-emit-llvm=%{save-temps:%g.lto.bc;:%d%u.lto.bc} \
        --plugin-opt \"sysroot=%k\" \
        --plugin-opt swf-size=%{!swf-size=*:1024x512}%{swf-size=*:%*} \
        --plugin-opt swf-version=%{!swf-version=*:18}%{swf-version=*:%*} \
        --plugin-opt mtriple=avm2-unknown-freebsd \
        --plugin-opt lto-as3-1=%{save-temps:%g.lto.1.as;:%d%u.lto.1.as}  \
        --plugin-opt lto-as3-2=%{save-temps:%g.lto.2.as;:%d%u.lto.2.as} \
    %l " LINK_PIE_SPEC "%X %{o*} %{A} %{d} %<emit-llvm %{e*}\
    %{m} %{N} %{n} %{r}\
    %{s} %{t} %<use-gold-plugin \
    %{u*} %{x} %{z} %{Z} %{!A:%{!nostdlib:%{!nostartfiles:%S}}}\
    %{static:} %{L*} %(mfwrap) %(link_libgcc) %o\
    %{fopenmp:%:include(libgomp.spec)%(link_gomp)} %(mflib)\
    %{fprofile-arcs|fprofile-generate|coverage|fcreate-profile:-lgcov}\
    %{!nostdlib:%{!nodefaultlibs:%(link_ssp) %(link_gcc_c_sequence)}}\
    %{!A:%{!nostdlib:%{!nostartfiles:%E}}} %{T*} }}}}}}"
#endif

/* There is no default multilib. */
#undef MULTILIB_DEFAULTS      

#define OPTIMIZATION_OPTIONS(LEVEL, SIZE)  \
  do						               \
    {						               \
    }						               \
  while (0)

#define TARGET_VERSION fprintf (stderr, " (avm2)");

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

/*{{{  Register Classes.  */ 

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
  AD_REGS,								/* %eax/%edx for DImode */
  Q_REGS,								/* %eax %ebx %ecx %edx */
  NON_Q_REGS,							/* %esi %edi %ebp %esp */
  INDEX_REGS,							/* %eax %ebx %ecx %edx %esi %edi %ebp */
  LEGACY_REGS,							/* %eax %ebx %ecx %edx %esi %edi %ebp %esp */
  GENERAL_REGS,							/* %eax %ebx %ecx %edx %esi %edi %ebp %esp %r8 - %r15*/
  FP_TOP_REG, FP_SECOND_REG,			/* %st(0) %st(1) */
  FLOAT_REGS,
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

extern enum reg_class const regclass_map[FIRST_PSEUDO_REGISTER];

#define N_REG_CLASSES 	((int) LIM_REG_CLASSES)

#define STRICT_ALIGNMENT 0

/* If bit field type is int, don't let it cross an int,
   and give entire struct the alignment of an int.  */
/* Required on the 386 since it doesn't have bit-field insns.  */
#define PCC_BITFIELD_TYPE_MATTERS 1

/*{{{  Layout of Source Language Data Types.  */ 

#define CHAR_TYPE_SIZE			 8
#define LONG_DOUBLE_TYPE_SIZE 	64
#define SHORT_TYPE_SIZE 		16
#define INT_TYPE_SIZE 			32
#define FLOAT_TYPE_SIZE 		32
#define LONG_TYPE_SIZE 			BITS_PER_WORD
#define DOUBLE_TYPE_SIZE 		64
#define LONG_LONG_TYPE_SIZE 	64

#define DEFAULT_SIGNED_CHAR 1

/*{{{  Storage Layout.  */ 
#define BITS_BIG_ENDIAN  0
#define BYTES_BIG_ENDIAN 0
#define WORDS_BIG_ENDIAN 0
#define UNITS_PER_WORD 	 4

#define FUNCTION_BOUNDARY 8
#define BIGGEST_ALIGNMENT 128
#define STACK_BOUNDARY    128

/* TODO:
**#define BIGGEST_FIELD_ALIGNMENT 32
*/

/* A C expression for the size in bytes of the trampoline, as an integer.  */
#define TRAMPOLINE_SIZE 10

/* The register number of the stack pointer register, which must also be a
   fixed register according to `FIXED_REGISTERS'.  On most machines, the
   hardware determines which register this is.  */
#define STACK_POINTER_REGNUM	7

/* The register number of the frame pointer register, which is used to access
   automatic variables in the stack frame.  On some machines, the hardware
   determines which register this is.  On other machines, you can choose any
   register you wish for this purpose.  */
#define FRAME_POINTER_REGNUM	20

/* The register number of the arg pointer register, which is used to access the
   function's argument list.  On some machines, this is the same as the frame
   pointer register.  On some machines, the hardware determines which register
   this is.  On other machines, you can choose any register you wish for this
   purpose.  If this is not the same register as the frame pointer register,
   then you must mark it as a fixed register according to `FIXED_REGISTERS', or
   arrange to be able to eliminate it.  */
#define ARG_POINTER_REGNUM 16

/* The first register that can contain the arguments to a function.  */
#define FIRST_ARG_REGNUM	 2

#define EH_RETURN_DATA_REGNO(N)	((N) < 2 ? (N) : INVALID_REGNUM)

#define DWARF_FRAME_REGNUM(REG) (REG)
#define DWARF_FRAME_RETURN_COLUMN DWARF_FRAME_REGNUM (1)

#define REGPARM_MAX 3

/* A C expression that is nonzero if REGNO is the number of a hard register in
   which function arguments are sometimes passed.  This does *not* include
   implicit arguments such as the static chain and the structure-value address.
   On many machines, no registers can be used for this purpose since all
   function arguments are pushed on the stack.  */
/* #define FUNCTION_ARG_REGNO_P(REGNO) */
#define FUNCTION_ARG_REGNO_P(REGNO) ((REGNO) < REGPARM_MAX)

/* A C expression that is nonzero if it is permissible to store a value of mode
   MODE in hard register number REGNO (or in several registers starting with
   that one).  */
#define HARD_REGNO_MODE_OK(REGNO, MODE) 1

#define PARM_BOUNDARY BITS_PER_WORD

/* A C statement to initialize the variable parts of a trampoline.  ADDR is an
   RTX for the address of the trampoline; FNADDR is an RTX for the address of
   the nested function; STATIC_CHAIN is an RTX for the static chain value that
   should be passed to the function when it is called.  */
#define INITIALIZE_TRAMPOLINE(ADDR, FNADDR, STATIC_CHAIN)			\
do										\
{										\
      rtx disp = expand_binop (SImode, sub_optab, FNADDR,\
                               plus_constant (ADDR, 10),\
                               NULL_RTX, 1, OPTAB_DIRECT);\
      emit_move_insn (gen_rtx_MEM (QImode, ADDR),\
                      gen_int_mode (0xb9, QImode));\
      emit_move_insn (gen_rtx_MEM (SImode, plus_constant (ADDR, 1)), STATIC_CHAIN);\
      emit_move_insn (gen_rtx_MEM (QImode, plus_constant (ADDR, 5)),\
                      gen_int_mode (0xe9, QImode));\
      emit_move_insn (gen_rtx_MEM (SImode, plus_constant (ADDR, 6)), disp);\
} while (0);


/* A macro whose definition is the name of the class to which a valid base
   register must belong.  A base register is one used in an address which is
   the register value plus a displacement.  */
#define BASE_REG_CLASS 	GENERAL_REGS

/*{{{  Describing Relative Costs of Operations */ 

/* Define this macro as a C expression which is nonzero if accessing less than
   a word of memory (i.e. a `char' or a `short') is no faster than accessing a
   word of memory, i.e., if such access require more than one instruction or if
   there is no difference in cost between byte and (aligned) word loads.

   When this macro is not defined, the compiler will access a field by finding
   the smallest containing object; when it is defined, a fullword load will be
   used if alignment permits.  Unless bytes accesses are faster than word
   accesses, using word accesses is preferable since it may eliminate
   subsequent memory access if subsequent accesses occur to other fields in the
   same word of the structure, but to different bytes.  */
#define SLOW_BYTE_ACCESS 1

#define FIRST_SSE_REG (21)
#define LAST_SSE_REG  (FIRST_SSE_REG + 7)
#define FIRST_MMX_REG  (LAST_SSE_REG + 1)
#define LAST_MMX_REG   (FIRST_MMX_REG + 7)
#define FIRST_REX_INT_REG  (LAST_MMX_REG + 1)
#define LAST_REX_INT_REG   (FIRST_REX_INT_REG + 7)
#define FIRST_REX_SSE_REG  (LAST_REX_INT_REG + 1)
#define LAST_REX_SSE_REG   (FIRST_REX_SSE_REG + 7)
#define FIRST_REX_INT_REG  (LAST_MMX_REG + 1)

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


/* A number, the maximum number of registers that can appear in a valid memory
   address.  Note that it is up to you to specify a value equal to the maximum
   number that `GO_IF_LEGITIMATE_ADDRESS' would ever accept.  */
#define MAX_REGS_PER_ADDRESS 2

/* A C expression which is nonzero if a function must have and use a frame
   pointer.  This expression is evaluated in the reload pass.  If its value is
   nonzero the function will have a frame pointer.

   The expression can in principle examine the current function and decide
   according to the facts, but on most machines the constant 0 or the constant
   1 suffices.  Use 0 when the machine allows code to be generated with no
   frame pointer, and doing so saves some time or space.  Use 1 when there is
   no possible advantage to avoiding a frame pointer.

   In certain cases, the compiler does not know how to produce valid code
   without a frame pointer.  The compiler recognizes those cases and
   automatically gives the function a frame pointer regardless of what
   `FRAME_POINTER_REQUIRED' says.  You don't need to worry about them.

   In a function that does not require a frame pointer, the frame pointer
   register can be allocated for ordinary usage, unless you mark it as a fixed
   register.  See `FIXED_REGISTERS' for more information.  */
/* #define FRAME_POINTER_REQUIRED 0 */
#define FRAME_POINTER_REQUIRED \
     (flag_omit_frame_pointer == 0 || current_function_pretend_args_size > 0)

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD

/* Define this to nonzero if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
#define FRAME_GROWS_DOWNWARD 1

/* Offset from the frame pointer to the first local variable slot to be
   allocated.

   If `FRAME_GROWS_DOWNWARD', find the next slot's offset by subtracting the
   first slot's length from `STARTING_FRAME_OFFSET'.  Otherwise, it is found by
   adding the length of the first slot to the value `STARTING_FRAME_OFFSET'.  */
/* #define STARTING_FRAME_OFFSET -4 */
#define STARTING_FRAME_OFFSET 0

/* A macro whose definition is the name of the class to which a valid index
   register must belong.  An index register is one used in an address where its
   value is either multiplied by a scale factor or added to another register
   (as well as added to a displacement).  */
#define INDEX_REG_CLASS INDEX_REGS

/* A C expression whose value is a register class containing hard register
   REGNO.  In general there is more than one such class; choose a class which
   is "minimal", meaning that no smaller class also contains the register.  */
#define REGNO_REG_CLASS(REGNO) (regclass_map[REGNO])

#define SYMBOLIC_CONST(X)	\
  (GET_CODE (X) == SYMBOL_REF						\
   || GET_CODE (X) == LABEL_REF						\
   || (GET_CODE (X) == CONST && symbolic_reference_mentioned_p (X)))

/* A C expression that is nonzero if X is a legitimate constant for an
   immediate operand on the target machine.  You can assume that X satisfies
   `CONSTANT_P', so you need not check this.  In fact, `1' is a suitable
   definition for this macro on machines where anything `CONSTANT_P' is valid.  */
#define LEGITIMATE_CONSTANT_P(X) 1

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

#define HI_REGISTER_NAMES						\
{"ax","dx","cx","bx","si","di","bp","sp",				\
 "st","st(1)","st(2)","st(3)","st(4)","st(5)","st(6)","st(7)",		\
 "argp", "flags", "fpsr", "dirflag", "frame",				\
 "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7",		\
 "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"	,		\
 "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",			\
 "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"}

/* A C initializer containing the assembler's names for the machine registers,
   each one as a C string constant.  This is what translates register numbers
   in the compiler into assembler language.  */
#define REGISTER_NAMES HI_REGISTER_NAMES

/* An initializer containing the names of the register classes as C string
   constants.  These names are used in writing some of the debugging dumps.  */
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

/* Define a data type for recording info about an argument list
   during the scan of that argument list.  This data type should
   hold all necessary information about the function itself
   and about the args processed so far, enough to enable macros
   such as FUNCTION_ARG to determine where the next arg should go.  */

typedef struct avm2_args {
  int words;                    /* # words passed so far */
  int nregs;                    /* # registers available for passing */
  int regno;                    /* next available register number */
  int fastcall;                 /* fastcall calling convention is used */
  int sse_words;                /* # sse words passed so far */
  int sse_nregs;                /* # sse registers available for passing */
  int warn_sse;                 /* True when we want to warn about SSE ABI.  */
  int warn_mmx;                 /* True when we want to warn about MMX ABI.  */
  int sse_regno;                /* next available sse register number */
  int mmx_words;                /* # mmx words passed so far */
  int mmx_nregs;                /* # mmx registers available for passing */
  int mmx_regno;                /* next available mmx register number */
  int maybe_vaarg;              /* true for calls to possibly vardic fncts.  */
  int float_in_sse;             /* 1 if in 32-bit mode SFmode (2 for DFmode) should
                                   be passed in SSE registers.  Otherwise 0.  */
} CUMULATIVE_ARGS;

/* A C expression whose value is a string containing the assembler operation
   that should precede instructions and read-only data.  Normally `".text"' is
   right.  */
#define TEXT_SECTION_ASM_OP "\t.text"

/* A C expression whose value is a string containing the assembler operation to
   identify the following data as writable initialized data.  Normally
   `".data"' is right.  */
#define DATA_SECTION_ASM_OP "\t.data"

/* Globalizing directive for a label.  */
#define GLOBAL_ASM_OP "\t.globl "

/* An alias for the machine mode for pointers.  On most machines, define this
   to be the integer mode corresponding to the width of a hardware pointer;
   `SImode' on 32-bit machine or `DImode' on 64-bit machines.  On some machines
   you must define this to be one of the partial integer modes, such as
   `PSImode'.

   The width of `Pmode' must be at least as large as the value of
   `POINTER_SIZE'.  If it is not equal, you must define the macro
   `POINTERS_EXTEND_UNSIGNED' to specify how pointers are extended to `Pmode'.  */
#define Pmode SImode

/* An alias for the machine mode used for memory references to functions being
   called, in `call' RTL expressions.  On most machines this should be
   `QImode'.  */
#define FUNCTION_MODE QImode

/* The maximum number of bytes that a single instruction can move quickly from
   memory to memory.  */
#define MOVE_MAX 16

/* An alias for a machine mode name.  This is the machine mode that elements of
   a jump-table should have.  */
#define CASE_VECTOR_MODE SImode

#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, SIZE) \
  avm2_return_pops_args ((FUNDECL), (FUNTYPE), (SIZE))

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.  */

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS) \
  init_cumulative_args (&(CUM), (FNTYPE), (LIBNAME), (FNDECL))

/* The number of register assigned to holding function arguments.  */
#define ARG_REGS	 4

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

/* A C statement (sans semicolon) to update the summarizer variable CUM to
   advance past an argument in the argument list.  The values MODE, TYPE and
   NAMED describe that argument.  Once this is done, the variable CUM is
   suitable for analyzing the *following* argument with `FUNCTION_ARG', etc.

   This macro need not do anything if the argument in question was passed on
   the stack.  The compiler knows how to track the amount of stack space used
   for arguments without any special help.  */
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED) 

/* A C expression which is nonzero if on this machine it is safe to "convert"
   an integer of INPREC bits to one of OUTPREC bits (where OUTPREC is smaller
   than INPREC) by merely operating on it as if it had only OUTPREC bits.

   On many machines, this expression can be 1.

   When `TRULY_NOOP_TRUNCATION' returns 1 for a pair of sizes for modes for
   which `MODES_TIEABLE_P' is 0, suboptimal code can result.  If this is the
   case, making `TRULY_NOOP_TRUNCATION' return 0 in such cases may improve
   things.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/*{{{  Assembler Commands for Alignment.  */ 

/* A C statement to output to the stdio stream STREAM an assembler command to
   advance the location counter to a multiple of 2 to the POWER bytes.  POWER
   will be a C expression of type `int'.  */

#define ASM_OUTPUT_ALIGN(FILE,LOG)      \
    if ((LOG)!=0) fprintf ((FILE), "\t.align %d\n", 1<<(LOG))

/* A C expression that is 1 if the RTX X is a constant which is a valid
   address.  On most machines, this can be defined as `CONSTANT_P (X)', but a
   few machines are more restrictive in which constant addresses are supported.

   `CONSTANT_P' accepts integer-values expressions whose values are not
   explicitly known, such as `symbol_ref', `label_ref', and `high' expressions
   and `const' arithmetic expressions, in addition to `const_int' and
   `const_double' expressions.  */
#define CONSTANT_ADDRESS_P(X) CONSTANT_P (X)

#define RETURN_VALUE_REGNUM	 0

#define FUNCTION_VALUE(VALTYPE, FUNC) \
     gen_rtx_REG (TYPE_MODE (VALTYPE), RETURN_VALUE_REGNUM)

/* A C expression to create an RTX representing the place where a library
   function returns a value of mode MODE.  If the precise function being called
   is known, FUNC is a tree node (`FUNCTION_DECL') for it; otherwise, FUNC is a
   null pointer.  This makes it possible to use a different value-returning
   convention for specific functions when all their calls are known.

   Note that "library function" in this context means a compiler support
   routine, used to perform arithmetic, whose name is known specially by the
   compiler and was not mentioned in the C code being compiled.

   The definition of `LIBRARY_VALUE' need not be concerned aggregate data
   types, because none of the library functions returns such types.  */
#define LIBCALL_VALUE(MODE) gen_rtx_REG (MODE, RETURN_VALUE_REGNUM)

/* A C compound statement to output to stdio stream STREAM the assembler syntax
   for an instruction operand X.  X is an RTL expression.

   CODE is a value that can be used to specify one of several ways of printing
   the operand.  It is used when identical operands must be printed differently
   depending on the context.  CODE comes from the `%' specification that was
   used to request printing of the operand.  If the specification was just
   `%DIGIT' then CODE is 0; if the specification was `%LTR DIGIT' then CODE is
   the ASCII code for LTR.

   If X is a register, this macro should print the register's name.  The names
   can be found in an array `reg_names' whose type is `char *[]'.  `reg_names'
   is initialized from `REGISTER_NAMES'.

   When the machine description has a specification `%PUNCT' (a `%' followed by
   a punctuation character), this macro is called with a null pointer for X and
   the punctuation character for CODE.  */
#define PRINT_OPERAND(STREAM, X, CODE)


/* A C compound statement to output to stdio stream STREAM the assembler syntax
   for an instruction operand that is a memory reference whose address is X.  X
   is an RTL expression.  */

#define PRINT_OPERAND_ADDRESS(STREAM, X) 

/* A C statement or compound statement to output to FILE some assembler code to
   call the profiling subroutine `mcount'.  Before calling, the assembler code
   must load the address of a counter variable into a register where `mcount'
   expects to find the address.  The name of this variable is `LP' followed by
   the number LABELNO, so you would generate the name using `LP%d' in a
   `fprintf'.

   The details of how the address should be passed to `mcount' are determined
   by your operating system environment, not by GCC.  To figure them out,
   compile a small program for profiling using the system's installed C
   compiler and look at the assembler code that results.  */
#define FUNCTION_PROFILER(FILE, LABELNO)	\
{						\
  fprintf (FILE, "\t mov rp, r1\n" );		\
  fprintf (FILE, "\t ldi:32 mcount, r0\n" );	\
  fprintf (FILE, "\t call @r0\n" );		\
  fprintf (FILE, ".word\tLP%d\n", LABELNO);	\
}

/* Offset from the argument pointer register to the first argument's address.
   On some machines it may depend on the data type of the function.

   If `ARGS_GROW_DOWNWARD', this is the offset to the location above the first
   argument's address.  */
#define FIRST_PARM_OFFSET(FUNDECL) 0

/* A C expression that is nonzero if it is desirable to choose register
   allocation so as to avoid move instructions between a value of mode MODE1
   and a value of mode MODE2.

   If `HARD_REGNO_MODE_OK (R, MODE1)' and `HARD_REGNO_MODE_OK (R, MODE2)' are
   ever different for any R, then `MODES_TIEABLE_P (MODE1, MODE2)' must be
   zero.  */
#define MODES_TIEABLE_P(MODE1, MODE2) 1


/* A C expression that places additional restrictions on the register class to
   use when it is necessary to copy value X into a register in class CLASS.
   The value is a register class; perhaps CLASS, or perhaps another, smaller
   class.  On many machines, the following definition is safe:

        #define PREFERRED_RELOAD_CLASS(X,CLASS) CLASS

   Sometimes returning a more restrictive class makes better code.  For
   example, on the 68000, when X is an integer constant that is in range for a
   `moveq' instruction, the value of this macro is always `DATA_REGS' as long
   as CLASS includes the data registers.  Requiring a data register guarantees
   that a `moveq' will be used.

   If X is a `const_double', by returning `NO_REGS' you can force X into a
   memory constant.  This is useful on certain machines where immediate
   floating values cannot be loaded into certain kinds of registers.  */
#define PREFERRED_RELOAD_CLASS(X, CLASS) CLASS

/* A C expression for the maximum number of consecutive registers of
   class CLASS needed to hold a value of mode MODE.

   This is closely related to the macro `HARD_REGNO_NREGS'.  In fact, the value
   of the macro `CLASS_MAX_NREGS (CLASS, MODE)' should be the maximum value of
   `HARD_REGNO_NREGS (REGNO, MODE)' for all REGNO values in the class CLASS.

   This macro helps control the handling of multiple-word values in
   the reload pass.  */
#define CLASS_MAX_NREGS(CLASS, MODE) HARD_REGNO_NREGS (0, MODE)

/* A C expression for the number of consecutive hard registers, starting at
   register number REGNO, required to hold a value of mode MODE.  */

#define HARD_REGNO_NREGS(REGNO, MODE) 			\
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* A C expression that is nonzero if REGNO is the number of a hard register in
   which the values of called function may come back.  */
#define FUNCTION_VALUE_REGNO_P(REGNO) ((REGNO) == RETURN_VALUE_REGNUM)

#define INITIAL_FRAME_POINTER_OFFSET(X) 0

#define KEEP_AGGREGATE_RETURN_POINTER 0

/* Disable RTD */
#define TARGET_RTD 0

/* We need to define for libgcc2 */
#define TARGET_64BIT 0

#undef OBJC_WARN_OBJC2_FEATURES
#define OBJC_WARN_OBJC2_FEATURES(MESSAGE) \
do {} while(0)

/* LLVM LOCAL end - TCE target */
