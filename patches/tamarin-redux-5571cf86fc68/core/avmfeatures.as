/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
   vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)

 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.


   This file defines the feature set for avmplus.

   It is also a program to transform the feature definition into three files:

     * a C++ header file, avmfeatures.h
     * a C++ source file, avmfeatures.cpp
     * a Python file, ../build/avmfeatures.py


   The feature set is defined as an XML (really E4X) datum, see below.

   Grammar for feature definitions.

       features           ::= "<features>" feature* at-least-one* at-most-one* exactly-one* "</features>"
       feature            ::= "<feature>" desc name defines+ precludes* requires* build-flags* "</feature>"
                            | "<tweak>" desc name default defines+ precludes* requires* "</tweak>"
       desc               ::= "<desc>" TEXT "</desc>"
       name               ::= "<name>" TEXT "</name>"
       default            ::= "<default> "true" | "false" "</default>"
       defines            ::= "<defines>" TEXT "</defines>"
       precludes          ::= "<precludes>" TEXT "</precludes>"
       requires           ::= "<requires>" TEXT | at-most-one | at-least-one | exactly-one "</requires>"
       build-flags        ::= <build-flags type="boolean"|"onoff"> TEXT </build-flags>
       at-least-one       ::= "<at-least-one>" name+ "</at-least-one>"
       at-most-one        ::= "<at-most-one>" name+ "</at-most-one>"
       exactly-one        ::= "<exactly-one>" name+ "</exactly-one>"
       TEXT               ::= space* [a-zA-Z][a-zA-Z0-9_]* space*

       Clause ordering is not significant anywhere and xml comments are
       allowed everywhere.  Leading and trailing whitespace in each element
       is ignored.

   Semantics:

       Features are compile time constraints that select and deselect
       aspects of the virtual machine code.  At compile time, a value
       (off or on) must be provided for every feature, there are no
       default values.

       A FEATURE clause defines a named feature, named by its NAME clause
       and described in human terms by its DESC clause.

         NOTE: A good description is absolutely essential.  It should
         state what the feature does, but also what the implications of
         selecting or deselecting it might be.

       A TWEAK clause defines a named feature with a default value, which
       provides an opportunity for workarounds for bugs and other very
       minor issues.  TWEAKs should be used sparingly (and if they are
       abused because it's convenient to introduce default values then
       the default value facility will be removed).

       No two FEATUREs or TWEAKSs may name the same feature.

       Feature names named by PRECLUDES, REQUIRES, AT-LEAST-ONE, and
       AT-MOST-ONE clauses must be defined by some FEATURE.

       A PRECLUDES clause in a FEATURE adds a constraint that the
       precluded feature cannot be turned on if the feature defined by
       the present FEATURE is turned on.

       A REQUIRES clause in a FEATURE adds a constraint that the
       required feature must be turned on if the feature defined by
       the present FEATURE is turned on.

       A DEFINES clause in a FEATURE names a preprocessor macro that is
       defined (with a simple "#define X") if the FEATURE is turned on.
       This seems redundant until you consider that a FEATURE can define
       multiple macros; this allows discrimination internally in the
       module to be organized differently than feature definition
       externally.

       A BUILD-FLAGS clause in a FEATURE names a flag that will be passed
       to the script that compiles the builtins. There are two types of
       flags that may be passed - "boolean" and "onoff":
           - A "boolean" flag will always be passed to the compilation script,
           in the form "TEXT=true" (if the feature is enabled) or "TEXT=false"
           (if the feature is disabled)
           - An "onoff" flag will only be passed to the compilation script if
           the feature is enabled (nothing will be passed if it is disabled)

       An AT-LEAST-ONE clause adds a constraint that at least one of
       the features has been turned on.

       An AT-MOST-ONE clause adds a constraint that at most one of the
       features has been turned on.  (The same can be expressed by
       PRECLUDES clauses but that would be more error-prone.)

       EXACTLY-ONE is an abbreviation for the combination of AT-LEAST-ONE
       and AT-MOST-ONE.

   This program processes the feature definition below to check static
   constraints and produce the C++ header file core/avmfeatures.h; that
   header file checks compile-time constraints and defines preprocessor
   macros as appropriate.

   The product's feature settings are imported through VMPI.h; see comments
   in that file for how to provide the settings.

   This program also produces a C++ compilation unit (.cpp file) that
   contains a single string that lists the features that were enabled at
   build time; it is used by the AVM shell for debugging purposes.

   This program also produces a Python file that is used by the cross-platform
   build system to handle configuration switches; see ../configure.py.

*/

/****************************************************************************
 ***** Start of feature definition ******************************************
 ****************************************************************************/

var FEATURES =
<features>

  <!-- Basic system characteristics -->

  <feature>
    <desc> Selects a 32-bit architecture </desc>

    <name>      AVMSYSTEM_32BIT </name>
    <precludes> AVMSYSTEM_64BIT </precludes>
    <defines>   VMCFG_32BIT </defines>
  </feature>

  <feature>
    <desc> Selects a 64-bit architecture </desc>

    <name>      AVMSYSTEM_64BIT </name>
    <precludes> AVMSYSTEM_32BIT </precludes>
    <defines>   VMCFG_64BIT </defines>
    <defines>   MMGC_64BIT </defines>       <!-- FIXME: Bug 536304 legacy name -->
    <defines>   AVMPLUS_64BIT </defines>    <!-- FIXME: Bug 536304 legacy name -->
  </feature>

  <feature>
    <desc> Selects an architecture that allows load/store of unaligned 16- and 32-bit ints.

        While it's OK for an unaligned int load or store to be slower than an aligned load
        or store, we require that:

          - an unaligned load/store MUST NOT generate a run-time fault, and
          - an unaligned load/store MUST be at least as efficient as separate instructions
            to load and assemble the word one byte at a time / disassemble and store one
            byte at a time.

        If you cannot guarantee that the requirements are met then DO NOT enable
        this feature.  (For example, on Palm Pre unaligned loads/stores are allowed but 
        they're reportedly so slow that they're pointless.)

        Code that uses this feature MUST NOT use it as a license to load/store floating-point
        data using integer instructions, since in general that will not work.  In
        particular this classical pattern will not work:

            uint8_t* p = ...;  // possibly-unaligned address we're loading from
            union (
                float f;
                uint32_t i;
            ) u;
        #ifdef VMCFG_UNALIGNED_INT_ACCESS
            u.i = *(uint32_t*)p;
            return u.f;
        #else
            ...

        The reason it won't work is that some compilers (notably on ARM) will generate
        code that uses a floating-point load into an FP register, so the code actually
        needs unaligned floating-point loads to be supported, AVMSYSTEM_UNALIGNED_FP_ACCESS.
        (Whether it is correct for the compiler to generate that code is beside the point.)

        The prohibition applies to 64-bit loads/stores as well (expressed as pairs of
        uint32_t loads/stores): ARM compilers as of October 2011 will rewrite a pair of loads 
        into a uint32_t array in a union with a return of a double from the union as a single
        double load into the return register.  See comments throughout the code as well
        as Bugzilla 569691 and 685441.
    </desc>

    <name>      AVMSYSTEM_UNALIGNED_INT_ACCESS </name>
    <defines>   VMCFG_UNALIGNED_INT_ACCESS </defines>
  </feature>

  <feature>
    <desc> Selects an architecture that allows load/store of unaligned 32- and 64-bit floats.
    
        While it's OK for an unaligned floating-point load/store to be slower than an aligned
        load/store, we require that:

          - an unaligned load/store MUST NOT generate a run-time fault, and
          - an unaligned load/store MUST be at least as efficient as separate instructions
            to load and assemble the datum one byte at a time / disassemble and store one
            byte at a time.

        If you cannot guarantee that the requirements are met then DO NOT enable
        this feature.

        Note that if AVMSYSTEM_UNALIGNED_FP_ACCESS is not set then it is assumed that 64-bit
        floats require 8-byte alignment.
        
        Note that AVMSYSTEM_UNALIGNED_FP_ACCESS does not apply to float4 values.  Some SIMD
        units have different instructions for aligned and unaligned access; on some 
        systems the alignment requirement is 16 bytes, on others it's 8 bytes.  But as of
        November 2011 all C++ compilers we use will assume such alignment when manipulating
        float4 values and will not use the instructions for unaligned access even if
        they are available.  C++ code must never assume that unaligned access is OK is
        appropriate for float4 data.
    </desc>

    <name>      AVMSYSTEM_UNALIGNED_FP_ACCESS </name>
    <defines>   VMCFG_UNALIGNED_FP_ACCESS </defines>
  </feature>

  <feature>
    <desc> Selects a big-endian architecture: the most significant byte of a word
           is stored at the lowest byte address of the word </desc>

    <name>      AVMSYSTEM_BIG_ENDIAN </name>
    <precludes> AVMSYSTEM_LITTLE_ENDIAN </precludes>
    <defines>   VMCFG_BIG_ENDIAN </defines>
  </feature>

  <feature>
    <desc> Selects a little-endian architecture: the least significant byte of a word
           is stored at the lowest byte address of the word </desc>

    <name>      AVMSYSTEM_LITTLE_ENDIAN </name>
    <precludes> AVMSYSTEM_BIG_ENDIAN </precludes>
    <defines>   VMCFG_LITTLE_ENDIAN </defines>
  </feature>

  <feature>
    <desc> Selects a reverse floating-point layout on little-endian systems:
           the most significant word (containing the sign, exponent, and most
           significant bits of the significand) are at the lower word address.
           Each word is stored little-endian, however. </desc>
    <name>     AVMSYSTEM_DOUBLE_MSW_FIRST </name>
    <requires> AVMSYSTEM_LITTLE_ENDIAN </requires>
    <defines>  VMCFG_DOUBLE_MSW_FIRST </defines>
  </feature>

  <!-- CPU architectures, one of these is required for the JIT -->

  <feature>
    <desc> Selects the x86-32 architecture </desc>

    <name>      AVMSYSTEM_IA32 </name>
    <precludes> AVMSYSTEM_64BIT </precludes>
    <defines>   VMCFG_IA32 </defines>
    <defines>   VMCFG_SSE2 </defines>
    <defines>   MMGC_IA32 </defines>     <!-- FIXME: Bug 536304 legacy name -->
    <defines>   AVMPLUS_IA32 </defines>  <!-- FIXME: Bug 536304 legacy name -->
  </feature>

  <feature>
    <desc> Selects the x86-64 architecture </desc>

    <name>     AVMSYSTEM_AMD64 </name>
    <requires> AVMSYSTEM_64BIT </requires>
    <defines>  VMCFG_AMD64 </defines>
    <defines>  VMCFG_SSE2 </defines>
    <defines>  MMGC_AMD64 </defines>     <!-- FIXME: Bug 536304 legacy name -->
    <defines>  AVMPLUS_AMD64 </defines>  <!-- FIXME: Bug 536304 legacy name -->
  </feature>

  <feature>
    <desc> Selects the ARM architecture (version left unspecified for now). </desc>

    <name>      AVMSYSTEM_ARM </name>
    <precludes> AVMSYSTEM_64BIT </precludes>
    <defines>   VMCFG_ARM </defines>
    <defines>   MMGC_ARM </defines>     <!-- FIXME: Bug 536304 legacy name -->
    <defines>   AVMPLUS_ARM </defines>  <!-- FIXME: Bug 536304 legacy name -->
  </feature>

  <feature>
    <desc> Selects the PowerPC / Power architecture.  Whether it's the 32-bit or the
           64-bit version of the architecture is controlled independently. </desc>

    <name>     AVMSYSTEM_PPC </name>
    <defines>  VMCFG_PPC </defines>
    <defines>  MMGC_PPC </defines>     <!-- FIXME: Bug 536304 legacy name -->
    <defines>  AVMPLUS_PPC </defines>  <!-- FIXME: Bug 536304 legacy name -->
  </feature>

  <feature>
    <desc> Selects the 32-bit SPARC architecture. </desc>

    <name>      AVMSYSTEM_SPARC </name>
    <precludes> AVMSYSTEM_64BIT </precludes>
    <defines>   VMCFG_SPARC </defines>
    <defines>   MMGC_SPARC </defines>     <!-- FIXME: Bug 536304 legacy name -->
    <defines>   AVMPLUS_SPARC </defines>  <!-- FIXME: Bug 536304 legacy name -->
  </feature>

  <feature>
    <desc> Selects the MIPS architecture (version left unspecified for now). </desc>

    <name>      AVMSYSTEM_MIPS </name>
    <defines>   VMCFG_MIPS </defines>
    <defines>   AVMPLUS_MIPS </defines>  <!-- FIXME: Bug 536304 legacy name -->
  </feature>

  <feature>
    <desc> Selects the 32-bit SH4 architecture. </desc>

    <name>      AVMSYSTEM_SH4 </name>
    <precludes> AVMSYSTEM_64BIT </precludes>
    <defines>   VMCFG_SH4 </defines>
  </feature>

  <at-most-one>
    <!-- architectures are all mutually exclusive, but there can be "none", I believe (need to verify) -->
    <name> AVMSYSTEM_IA32 </name>
    <name> AVMSYSTEM_AMD64 </name>
    <name> AVMSYSTEM_ARM </name>
    <name> AVMSYSTEM_PPC </name>
    <name> AVMSYSTEM_SPARC </name>
    <name> AVMSYSTEM_MIPS </name>
    <name> AVMSYSTEM_SH4 </name>
  </at-most-one>


  <!-- operating systems.
       We might want more of these, I bet the code has a bunch of implied operating
       systems (eg WIN32 && ARM usually means Windows Mobile...)  -->

  <feature>
    <desc> Selects Unix / Linux (but not MacOS) </desc>
    <name> AVMSYSTEM_UNIX </name>
    <defines> AVMPLUS_UNIX </defines>
  </feature>

  <feature>
    <desc> Selects MacOS X </desc>
    <name> AVMSYSTEM_MAC </name>
    <defines> AVMPLUS_MAC </defines>
    <defines> MMGC_MAC </defines>
  </feature>

  <feature>
    <desc> Selects Win32, Win64, Windows Mobile </desc>
    <name> AVMSYSTEM_WIN32 </name>
    <defines> AVMPLUS_WIN32 </defines>
  </feature>

  <feature>
    <desc> Selects Symbian </desc>
    <name> AVMSYSTEM_SYMBIAN </name>
    <defines> VMCFG_SYMBIAN </defines>
  </feature>

  <exactly-one>
    <name> AVMSYSTEM_UNIX </name>
    <name> AVMSYSTEM_MAC </name>
    <name> AVMSYSTEM_WIN32 </name>
    <name> AVMSYSTEM_SYMBIAN </name>
  </exactly-one>


  <!-- VM facilities: AVMFEATURE_* -->

  <feature>
    <desc>  Selects the AVM debugger API, including retaining debug information at
            run-time and human-readable error messages for run-time errors.

            There is a performance penalty to enabling this; clients that want
            maximal execution performance and don't care about debugging should
            disable it.

            If you enable the debugger you may want to consider enabling support for
            specific language strings for error messages in order to avoid getting
            them all.  See the AVMPLUS_ERROR_LANG_ macros in core/ErrorConstants.h.
            It's easiest to define the ones you want in core/avmbuild.h.
            </desc>

    <name>     AVMFEATURE_DEBUGGER  </name>
    <defines>  VMCFG_DEBUGGER  </defines>
    <defines>  VMCFG_VERIFYALL </defines>
    <defines>  AVMPLUS_VERBOSE </defines>
    <defines>  DEBUGGER </defines>           <!-- FIXME: Bug 536304 legacy name -->
  </feature>

  <feature>
    <desc>  This is used to compile AVM with the debugger API enabled, but
            certain bits of functionality reduced to no-ops. </desc>

    <name>     AVMFEATURE_DEBUGGER_STUB  </name>
    <requires>  AVMFEATURE_DEBUGGER  </requires>
    <defines>  VMCFG_DEBUGGER_STUB  </defines>
  </feature>

  <feature>
    <desc> Enable the sample-based memory profiler.  This makes allocation a
           little more expensive if a sampler callback is not installed, and
           more expensive still if it is installed.

           FIXME: more information needed.

           Note that this is enabled always by AVMFEATURE_DEBUGGER.

           It is known that the Flash Player wants to enable this if SCRIPT_DEBUGGER
           is enabled in the Player code. </desc>

    <name> AVMFEATURE_ALLOCATION_SAMPLER </name>
    <defines> AVMPLUS_SAMPLER </defines>
  </feature>

  <feature>
    <desc> Selects VTune profiling of jit'd code.  Requires Windows x86,
           and could support windows x64 after more testing.
           turns on AVMPLUS_VERBOSE solely to get method/class names for profiling
    </desc>

    <name>     AVMFEATURE_VTUNE  </name>
    <requires> AVMSYSTEM_WIN32 </requires>
    <requires> AVMSYSTEM_IA32 </requires>
    <defines>  VMCFG_VTUNE </defines>

    <defines>  AVMPLUS_VERBOSE </defines>
  </feature>

  <feature>
    <desc> Selects Shark profiling of jit'd code.  MacOS 10.6.  This technique
           should work for oprofile on linux and/or android, with more tweaks.
           See README in utils/sharkprof.
    </desc>
    <name>     AVMFEATURE_SHARK  </name>
    <requires> AVMSYSTEM_MAC </requires>
    <defines>  VMCFG_SHARK </defines>
  </feature>

  <feature>
    <desc> Enables the just-in-time compiler.  This will typically increase performance
           significantly but may result in significantly higher memory consumption. </desc>

    <name> AVMFEATURE_JIT  </name>
    <requires>
     <exactly-one>
      <name> AVMSYSTEM_IA32 </name>
      <name> AVMSYSTEM_AMD64 </name>
      <name> AVMSYSTEM_ARM </name>
      <name> AVMSYSTEM_PPC </name>
      <name> AVMSYSTEM_SPARC </name>
      <name> AVMSYSTEM_MIPS </name>
      <name> AVMSYSTEM_SH4 </name>
     </exactly-one>
    </requires>
    <requires> AVMFEATURE_ABC_INTERP </requires>
    <defines> VMCFG_NANOJIT  </defines>
    <defines> FEATURE_NANOJIT </defines> <!-- referenced by nanojit module only -->
  </feature>

  <feature>
    <desc> Enables the Halfmoon optimizing just-in-time compiler. </desc>
    <name> AVMFEATURE_HALFMOON  </name>
    <requires> AVMFEATURE_JIT </requires>
    <defines> VMCFG_HALFMOON  </defines>
  </feature>

  <feature>
    <desc> Enables the types 'float' and 'float4' in the VM. </desc>
    <name> AVMFEATURE_FLOAT  </name>
    <requires> AVMFEATURE_SWF16 </requires>
    <defines> VMCFG_FLOAT  </defines>
    <defines> VMCFG_GENERIC_FLOAT4 </defines>
    <build-flags type="boolean"> -config CONFIG::VMCFG_FLOAT </build-flags>
    <build-flags type="onoff"> -abcfuture </build-flags>
    <precludes> AVMFEATURE_AOT </precludes> <!-- AOT + float doesn't work yet, byt that will change eventually -->
  </feature>

  <!-- CROSSBRIDGRE PATCH START -->
  <feature>
    <desc> Enables the POSIX builtins used by commandline Alchemy apps. </desc>
    <name> AVMFEATURE_ALCHEMY_POSIX </name>
    <defines> VMCFG_ALCHEMY_POSIX  </defines>
    <build-flags type="boolean"> -config CONFIG::VMCFG_ALCHEMY_POSIX </build-flags>
    <build-flags type="onoff"> </build-flags>
  </feature>
  <!-- CROSSBRIDGRE PATCH END -->
  
  <feature>
    <desc> Enables delayed JIT-compilation with on-stack replacement, by default,
           and supports runtime-disabling of OSR to get the legacy policy (OSR=0).
           Without this feature, legacy policy is the default: the VM
           compiles a method eagerly or interprets it always, and the OSR
           invocation threshold can be enabled at runtime (OSR=K, K>0). </desc>

    <name> AVMFEATURE_OSR  </name>
    <requires> AVMFEATURE_JIT </requires>
    <defines> VMCFG_OSR  </defines>
  </feature>

  <feature>
    <desc> Allows the default JIT compilation policy to be overriden with alternate rules.
           In shell builds, this enables the -policy option which allows one to specify
           which methods should be compiled and which should be interpreted.  There are
           currently three means of identifying a method to be controlled; unique id,
           exact name match, and regular expression name match.
            </desc>

    <name> AVMFEATURE_COMPILEPOLICY  </name>
    <requires> AVMFEATURE_JIT </requires>
    <defines> VMCFG_COMPILEPOLICY  </defines>
  </feature>

  <feature>
    <desc>Enables the ahead-of-time compiler.</desc>

    <name> AVMFEATURE_AOT </name>
    <requires>
     <exactly-one>
      <name> AVMSYSTEM_IA32 </name>
      <name> AVMSYSTEM_ARM </name>
     </exactly-one>
    </requires>
    
    <defines> VMCFG_AOT </defines>
    <defines> VMCFG_AOTSHELL </defines>
    <defines> VMCFG_CDECL </defines>
  </feature>

  <feature>
    <desc>Enables the exception based caching code, right now this is used
           exclusively by AOT.</desc>

    <name> AVMFEATURE_BUFFER_GUARD </name>
    <requires>
     <exactly-one>
      <name> AVMFEATURE_AOT </name>
     </exactly-one>
    </requires>
    <defines> VMCFG_BUFFER_GUARD </defines>
    <defines> VMCFG_MACH_EXCEPTIONS </defines>
  </feature>

  <feature>
    <desc> Selects the ABC interpreter.  Appropriate for platforms that run
           the interpreter only for initialization code and for
           platforms that are exceptionally memory-constrained. </desc>

    <name>       AVMFEATURE_ABC_INTERP  </name>
    <defines>    VMCFG_INTERPRETER  </defines>
  </feature>


  <feature>
    <desc> Selects the wordcode interpreter.  Appropriate for platforms that run the
           interpreter for some or all methods and are not exceptionally memory-constrained. </desc>

    <name>       AVMFEATURE_WORDCODE_INTERP  </name>
    <defines>    VMCFG_INTERPRETER  </defines>
    <defines>    VMCFG_WORDCODE  </defines>
    <defines>    VMCFG_WORDCODE_PEEPHOLE  </defines>
    <precludes>  AVMFEATURE_COMPILEPOLICY </precludes>
  </feature>

  <feature>
    <desc> Selects the faster, direct threaded wordcode interpreter.
           This is appropriate only for C++ compilers that support GCC-style computed
           "goto".  It is believed that RCVT, Intel's C++ compiler, and the Sunpro
           compiler all do.  </desc>

     <name>      AVMFEATURE_THREADED_INTERP </name>
     <requires>  AVMFEATURE_WORDCODE_INTERP </requires>
     <defines>   VMCFG_DIRECT_THREADED </defines>
  </feature>

  <at-most-one>
    <!-- Though it is believed that it is possible to run avmplus with just a JIT,
         doing so is not supported at this time. Also note that some build configurations
         (such as AOT) may wish to have no interpreter or JIT. -->

    <name>  AVMFEATURE_WORDCODE_INTERP  </name>
    <name>  AVMFEATURE_ABC_INTERP  </name>
  </at-most-one>

  <at-most-one>
    <!-- The wordcode interpreter and JIT should work together in principle but we haven't
         tested that configuration, and so prevent it here.  -->
    <name> AVMFEATURE_WORDCODE_INTERP </name>
    <name> AVMFEATURE_JIT </name>
  </at-most-one>

  <at-most-one>
    <!-- AOT currently implies no JIT or Interpreter, but this could be fixed
         in the future if necessary -->
    <name> AVMFEATURE_AOT </name>
    <name> AVMFEATURE_JIT </name>
  </at-most-one>

  <at-most-one>
    <!-- AOT currently implies no JIT or Interpreter, but this could be fixed
         in the future if necessary -->
    <name> AVMFEATURE_AOT </name>
    <name> AVMFEATURE_ABC_INTERP </name>
  </at-most-one>

  <at-most-one>
    <!-- AOT currently implies no JIT or Interpreter, but this could be fixed
         in the future if necessary -->
    <name> AVMFEATURE_AOT </name>
    <name> AVMFEATURE_WORDCODE_INTERP </name>
  </at-most-one>

  <feature>
   <desc> AVMFEATURE_SELFTEST enables the built-in selftests.  These can be run by -Dselftest
          at the shell or by calling the global function avmplus::selftests(), see extensions/Selftest.h.
          Mostly they are useful for AVM development, not for embedders.

          Apart from code size considerations this can be enabled for release builds.  </desc>

   <name>     AVMFEATURE_SELFTEST </name>
   <defines>  VMCFG_SELFTEST </defines>
  </feature>

  <feature>
    <desc> Select support for the AS3 run-time compiler.  NOT RECOMMENDED.  The run-time compiler
           is still undergoing development.  </desc>

    <name> AVMFEATURE_EVAL </name>
    <defines> VMCFG_EVAL </defines>
    <precludes> AVMFEATURE_AOT </precludes>
  </feature>

  <feature>
    <desc> Select support for Telemetry (only interface for now; to be used in player) </desc>
    <name> AVMFEATURE_TELEMETRY </name>
    <defines> VMCFG_TELEMETRY </defines>
  </feature>

  <feature>
    <desc> Select support for Telemetry based sampler, requires a Telemetry implementation
           (to be used in host) </desc>
    <name> AVMFEATURE_TELEMETRY_SAMPLER </name>
    <requires> AVMFEATURE_TELEMETRY </requires>
    <defines> VMCFG_TELEMETRY_SAMPLER </defines>
  </feature>

  <feature>
    <desc> Makes all JIT code buffers read-only whenever JIT code is executing,
           to reduce the probability of heap overflow attacks. </desc>
    <name> AVMFEATURE_PROTECT_JITMEM </name>
    <defines> VMCFG_PROTECT_JITMEM </defines>
  </feature>

  <feature>
    <desc> Selects locking around calls to the memory block manager (GCHeap), allowing multiple
           threads to share the block manager.  Any client with more than one thread that uses
           MMgc either for garbage collected or manually managed memory wants this; the Flash
           Player requires it.  </desc>
    <name> AVMFEATURE_SHARED_GCHEAP </name>
    <defines> MMGC_LOCKING </defines>
  </feature>

  <feature>
    <desc> Make MMgc's overridden global new and delete operators delegate allocation and
           deallocation to VMPI_alloc and VMPI_free instead of going to FixedMalloc.

           Whether you want this or not probably depends on the performance of the
           underlying malloc and might depend on memory consumption patterns.  On desktop
           systems you probably want this to be disabled.  </desc>
    <name> AVMFEATURE_USE_SYSTEM_MALLOC </name>
    <defines> MMGC_USE_SYSTEM_MALLOC </defines>
  </feature>

  <feature>
    <desc> Support C++ exceptions in the MMgc API.  At the time of writing (Apr 2009)
           this means decorating the global new and delete operator with appropriate 'throw'
           clauses.  It is unlikely to mean anything more, as AVM+ and MMgc do not use and
           do not generally support C++ exceptions.

           Note that even if this is enabled, the global new and delete operators may
           not throw exceptions when memory can't be allocated, because the out-of-memory
           handling in MMgc may take precedence.

           FixedMalloc never throws an exception for a failed allocation. </desc>

    <name> AVMFEATURE_CPP_EXCEPTIONS </name>
    <defines> MMGC_ENABLE_CPP_EXCEPTIONS </defines>
  </feature>

  <feature>
    <desc> Recognize a pointer or pointer-like value into anywhere in an object as referencing
           that object during marking in the garbage collector.

           Enabling this tends to be increase GC cost but it can be a useful debugging aid. </desc>
    <name> AVMFEATURE_INTERIOR_POINTERS </name>
    <defines> MMGC_INTERIOR_PTRS </defines>
  </feature>

  <feature>
    <desc> If enabled then always divert avmStackAlloc() to a separately managed stack,
           to avoid blowing the stack on small systems or to support systems that
           don't provide alloca().  If disabled then smallish allocations are handled
           by the built-in alloca() (which must be provided) and larger allocations
           are handled by diverting to a separately managed stack; the latter case is
           mainly a security issue, as alloca() will do strange things if given sufficiently
           large requests.  </desc>
    <name> AVMFEATURE_HEAP_ALLOCA </name>
    <defines> AVMPLUS_HEAP_ALLOCA </defines>
  </feature>

  <feature>
    <desc> Enabling this will cause the mmfx_* memory macros to use global new/delete.
        By default we use specialized new/delete operators and avoid global new/delete.  However
        this requires some tricks to get multiple inheritance and private destructors to work
        so some codebases may want to use the simpler path of overriding global new/delete.
        Note that this feature works independently of AVMFEATURE_USE_SYSTEM_MALLOC.
      </desc>
    <name> AVMFEATURE_OVERRIDE_GLOBAL_NEW </name>
    <defines> MMGC_OVERRIDE_GLOBAL_NEW </defines>
  </feature>

  <feature>
    <desc> Enabling this will compile in code to enable memory profiling. (Must still be
        enabled at runtime.)
      </desc>
    <name> AVMFEATURE_MEMORY_PROFILER </name>
    <defines> MMGC_MEMORY_PROFILER </defines>
  </feature>

  <feature>
    <desc> Enabling this will cache the result of getQualifiedClassName, making it run
        much more quickly, at the expense of more memory usage.
      </desc>
    <name> AVMFEATURE_CACHE_GQCN </name>
    <defines> VMCFG_CACHE_GQCN </defines>
  </feature>

  <feature>
    <desc> Enabling this will compile in code to tell valgrind about how MMgc allocates memory.
      </desc>
    <name> AVMFEATURE_VALGRIND </name>
    <defines> MMGC_VALGRIND </defines>
  </feature>

  <feature>
    <desc> Enables safepoints and the mechanism to dispatch SafepointTasks
      </desc>
    <name> AVMFEATURE_SAFEPOINTS </name>
    <defines> VMCFG_SAFEPOINTS </defines>
  </feature>
  
  <feature>
    <desc> Enables local returns and safepoint polling from interrupt checks </desc>
    <name> AVMFEATURE_INTERRUPT_SAFEPOINT_POLL </name>
    <defines> VMCFG_INTERRUPT_SAFEPOINT_POLL </defines>
    <requires>AVMFEATURE_SAFEPOINTS </requires>
  </feature>

  <feature>
    <desc> Enabling this will support SWF12 / ABC version 47.12 </desc>
    <name> AVMFEATURE_SWF12 </name>
    <defines> VMCFG_SWF12 </defines>
  </feature>

  <feature>
    <desc> Enabling this will support SWF13 / ABC version 47.13 </desc>
    <name> AVMFEATURE_SWF13 </name>
    <defines> VMCFG_SWF13 </defines>
    <requires> AVMFEATURE_SWF12 </requires>
  </feature>

  <feature>
    <desc> Enabling this will support SWF14 / ABC version 47.14 </desc>
    <name> AVMFEATURE_SWF14 </name>
    <defines> VMCFG_SWF14 </defines>
    <requires> AVMFEATURE_SWF13 </requires>
  </feature>

  <feature>
    <desc> Enabling this will support SWF15 / ABC version 47.15 </desc>
    <name> AVMFEATURE_SWF15 </name>
    <defines> VMCFG_SWF15 </defines>
    <requires> AVMFEATURE_SWF14 </requires>
  </feature>

  <feature>
    <desc> Enabling this will support SWF16 / ABC version 47.16 </desc>
    <name> AVMFEATURE_SWF16 </name>
    <defines> VMCFG_SWF16 </defines>
    <requires> AVMFEATURE_SWF15 </requires>
  </feature>

  <feature>
    <desc> Enabling this will support SWF17 / ABC version 47.17 </desc>
    <name> AVMFEATURE_SWF17 </name>
    <defines> VMCFG_SWF17 </defines>
    <requires> AVMFEATURE_SWF16 </requires>
  </feature>

  <feature>
    <desc> Enabling this will support SWF18 / ABC version 47.18 </desc>
    <name> AVMFEATURE_SWF18 </name>
    <defines> VMCFG_SWF18 </defines>
    <requires> AVMFEATURE_SWF17 </requires>
  </feature>

  <feature>
    <desc> Enabling this will support SWF19 / ABC version 47.19 </desc>
    <name> AVMFEATURE_SWF19 </name>
    <defines> VMCFG_SWF19 </defines>
    <requires> AVMFEATURE_SWF18 </requires>
  </feature>

  <feature>
    <desc> Enabling this will support SWF20 / ABC version 47.20 </desc>
    <name> AVMFEATURE_SWF20 </name>
    <defines> VMCFG_SWF20 </defines>
    <requires> AVMFEATURE_SWF19 </requires>
  </feature>

  <!-- VM adjustments for various oddities: AVMTWEAK_* -->

  <tweak>
    <desc> Various iphone SDK versions - at least - botch sin() and cos() around NaN
           and infinity.  See https://bugzilla.mozilla.org/show_bug.cgi?id=556149. </desc>
    <name> AVMTWEAK_SIN_COS_NONFINITE </name>
    <defines> VMCFG_TWEAK_SIN_COS_NONFINITE </defines>
    <default> false </default>
  </tweak>

  <tweak>
    <desc> The current (June 2010) EPOC/Symbian emulator has certain limitations,
           described throughout the code where this tweak is used. </desc>
    <name> AVMTWEAK_EPOC_EMULATOR </name>
    <defines> VMCFG_EPOC_EMULATOR </defines>
    <default> false </default>
  </tweak>

  <tweak>
    <desc> Compile-time selected exact tracing </desc>
    <name> AVMTWEAK_EXACT_TRACING </name>
    <defines> VMCFG_EXACT_TRACING </defines>
    <default> true </default>
  </tweak>

  <tweak>
    <desc> Run-time selectable exact tracing, for experimentation </desc>
    <name> AVMTWEAK_SELECTABLE_EXACT_TRACING </name>
    <defines> VMCFG_SELECTABLE_EXACT_TRACING </defines>
    <default> false </default>
  </tweak>

  <tweak>
    <desc> Heap graph, enables blacklist feature for inspecting object reachability </desc>
    <name> AVMTWEAK_HEAP_GRAPH </name>
    <defines> MMGC_HEAP_GRAPH </defines>
    <default> false </default>
  </tweak>

  <at-most-one>
    <name> AVMTWEAK_EXACT_TRACING </name>
    <name> AVMTWEAK_SELECTABLE_EXACT_TRACING </name>
  </at-most-one>

</features>;

/****************************************************************************
 ***** End of feature definition ********************************************
 ****************************************************************************/

import avmplus.*;
import flash.utils.*;

var allnames = {};

main();

function main() {

    // Generate avmfeatures.h

    var s = newString();
    var feature;
    var x;
    s += "\n";
    for each ( feature in FEATURES.feature )
        s += undefDefines(feature);
    s += "\n";
    for each ( feature in FEATURES.tweak )
        s += undefDefines(feature);
    s += "\n";
    for each ( feature in FEATURES.feature )
        s += testDefined(feature);
    s += "\n";
    for each ( feature in FEATURES.tweak )
        s += testDefined(feature);
    s += "\n";
    for each ( feature in FEATURES.feature )
        s += checkRequirements(feature);
    s += "\n";
    for each ( feature in FEATURES.tweak )
        s += checkRequirements(feature);
    s += "\n";
    for each ( x in FEATURES["at-least-one"] )
        s += atLeastOne(x);
    s += "\n";
    for each ( x in FEATURES["at-most-one"] )
        s += atMostOne(x);
    s += "\n";
    for each ( x in FEATURES["exactly-one"] )
        s += exactlyOne(x);
    s += "\n";
    for each ( feature in FEATURES.feature )
        s += defineResults(feature);
    s += "\n";
    for each ( feature in FEATURES.tweak )
        s += defineResults(feature);
    s += "\n";
    s += "#ifdef AVMSHELL_BUILD\n";
    s += "extern const char * const avmfeatures;\n";
    s += "#endif\n\n";
    print('writing avmfeatures.h');
    File.write("avmfeatures.h", s);

    // Generate avmfeatures.cpp

    s = newString();
    s += "\n\n";
    s += "#include \"avmplus.h\"\n\n";
    s += "#ifdef AVMSHELL_BUILD\n\n";
    s += "// The string avmfeatures contains the names of all features that were enabled\n";
    s += "// when the program was compiled.  Each feature name is terminated by a semicolon.\n";
    s += "const char * const avmfeatures = \"\"\n"
    for each ( feature in FEATURES.feature )
        s += showFeature(feature);
    for each ( feature in FEATURES.tweak )
        s += showFeature(feature);
    s += ";\n\n"
    s += "#endif // AVMSHELL_BUILD\n";
    print('writing avmfeatures.cpp');
    File.write("avmfeatures.cpp", s);

    // Generate ../build/avmfeatures.py

    s = newString("#");
    s += "\n\n";
    s += "def featureSettings(o):\n";
    s += "    args = \"\"\n";
    for each ( feature in FEATURES.feature )
        s += configureFeature(feature);
    for each ( feature in FEATURES.tweak )
        s += configureFeature(feature);
    s += "    return args\n";

    s += "\ndef builtinBuildFlags(o):\n";
    s += "    buildFlags = \"\"\n";
    for each ( feature in FEATURES.feature )
        s += featureFlags(feature);
    for each ( feature in FEATURES.tweak )
        s += featureFlags(feature);
    s += "    return buildFlags\n";

    print('writing ../build/avmfeatures.py');
    File.write("../build/avmfeatures.py", s);
}

function newString(prefix="//") {
    var s = ["                     DO NOT EDIT THIS FILE",
             "",
             "  This file has been generated by the script core/avmfeatures.as,",
             "  from a set of configuration parameters in that file.",
             "",
             "  If you feel you need to make changes below, instead edit the configuration",
             "  file and rerun it to get a new version of this file.",
             "",
             "  This Source Code Form is subject to the terms of the Mozilla Public",
             "  License, v. 2.0. If a copy of the MPL was not distributed with this",
             "  file, You can obtain one at http://mozilla.org/MPL/2.0/."
    ].map(function(x) { return prefix + x }).join("\n");
    return s;
}

// Undefine all names that are going to be defined so that predefined values
// do not pollute the results.

function undefDefines(feature) {
    var s = [];
    var d;
    for each ( d in feature.defines )
        s.push("#undef " + d);
    return s.join("\n") + "\n";
}

// Emit a description and a test of definedness and correct value for every feature;
// the description is located with this test because it will typically be this test
// that fails, so this is where the user will look.

function testDefined(feature) {
    var name = feature.name;
    var s = [];

    if (!name)
        fail("Feature with missing name");

    if (allnames.hasOwnProperty(name))
        fail("Duplicate name: " + name);
    allnames[name] = true;

    s.push("");
    s.push("");
    s.push("/* " + name + "\n *\n" + formatFeature(feature) + "\n */");
    if (feature.name() == "tweak") {
        s.push("#if !defined " + name);
        if (feature.default.length() == 0)
            fail("Tweak \"" + name + "\" is missing a 'default' clause.");
    s.push("#  define " + name + " " + (feature["default"] == "true" ? 1 : 0));
    s.push("#endif");
        s.push("#if " + name + " != 0 && " + name + " != 1");
    s.push("#  error \"" + name + " must be defined and 0 or 1 (only).\"");
    s.push("#endif");
    }
    else {
        if (feature.default.length() > 0)
            fail("Feature \"" + name + "\" must not have a 'default' clause: " + feature.default);
        s.push("#if !defined " + name + " || " + name + " != 0 && " + name + " != 1");
    s.push("#  error \"" + name + " must be defined and 0 or 1 (only).\"");
    s.push("#endif");
    }
    return s.join("\n") + "\n";
}

function formatFeature(feature) {
    // make the beginning of the line be " * " and then trim the extra space for empty lines
    return feature.desc.toString().replace(/^[ \t]*/gm, " * ").replace(/[ ]+\n/g, "\n")
}

function checkRequirements(feature) {
    var name = feature.name;
    var requires = feature.requires;
    var precludes = feature.precludes;
    var defines = feature.defines;
    var s = [];
    var items;

    if (defines.length() == 0)
        fail("Feature has no DEFINES clauses: " + name);

    if (requires.length() + precludes.length() > 0)
        s.push("#if " + name);

    for each ( var req in requires ) {
        if ((items = req["at-most-one"]).length() > 0)
            s.push(atMostOne(items));
        else if ((items = req["at-least-one"]).length() > 0)
            s.push(atLeastOne(items));
        else if ((items = req["exactly-one"]).length() > 0)
            s.push(exactlyOne(items));
        else {
            if (!allnames.hasOwnProperty(req))
                fail("Name used in REQUIRES not defined: " + req);
            s.push("#  if !" + req);
            s.push("#    error \"" + req + " is required for " + name + "\"");
            s.push("#  endif");
        }
    }

    for each ( var pre in precludes ) {
        if (!allnames.hasOwnProperty(pre))
            fail("Name used in PRECLUDES not defined: " + pre);
        s.push("#  if " + pre);
        s.push("#    error \"" + pre + " is precluded for " + name + "\"");
        s.push("#  endif");
    }

    if (requires.length() + precludes.length() > 0)
        s.push("#endif");
    return s.join("\n") + "\n";
}

function defineResults(feature) {
    var name = feature.name;
    var defines = feature.defines;
    var s = [];

    if (!defines)
        fail(name + ": missing 'defines' clause");

    for each ( var def in defines ) {
        s.push("#if " + name);
        s.push("#  define " + def);
        s.push("#endif");
    }
    return s.join("\n") + "\n";
}

function showFeature(feature) {
    return ("  #if " + feature.name + "\n" +
            "    \"" + feature.name + ";\"\n" +
            "  #endif\n");
}

// We ignore AVMSYSTEM_ settings as they're always autodetected

function configureFeature(feature) {
    // Public service: if a feature is exclusive with others, disable the others
    // if the one is set
    function complement(feature_name, candidates) {
        var res = "";
        for each (var x in candidates) {
            var mustDo = false;
            var s = "";
            for each (var n in x.name) {
                if (n == feature_name)
                    mustDo = true;
                else
                    s += "-D" + n + "=0 ";
            }
            if (mustDo)
               res += s;
        }
        return res;
    }

    var len = 0;
    if (feature.name.indexOf("AVMFEATURE_") == 0)
        len = 11;
    else if (feature.name.indexOf("AVMTWEAK_") == 0)
        len = 9;

    if (len > 0) {
        var enable = "-D" + feature.name + "=1 ";
        var disable = "-D" + feature.name + "=0 ";
        var dependent = complement(feature.name, FEATURES["at-most-one"]) +
                        complement(feature.name, FEATURES["exactly-one"]);
        var feature = feature.name.substring(len).toLowerCase().replace(/_/g,"-");

        // features without dependencies can be explicitly enabled or disabled
        if (dependent == "") {
            return ("    arg = o.getBoolArg(\"" + feature + "\")\n" +
                    "    if (arg == True):\n" +
                    "        args += \"" + enable + "\"\n" +
                    "    if (arg == False):\n" +
                    "        args += \"" + disable + "\"\n");
        }
        return ("    if o.getBoolArg(\"" + feature + "\"):\n" +
                "        args += \"" + enable + dependent + "\"\n");
    }
    return "";
}

function featureFlags(feature) {
    var len = 0;
    if (feature.name.indexOf("AVMFEATURE_") == 0)
        len = 11;
    else if (feature.name.indexOf("AVMTWEAK_") == 0)
        len = 9;

    if(len <= 0) return "";
    if(String(feature["build-flags"]).length <= 0)
        return "";

    var featureName = feature.name.substring(len).toLowerCase().replace(/_/g,"-");
    var enable= [];
    var disable=[];
    for each ( d in feature["build-flags"]){
        if(d.@type=="onoff"){
            enable.push(d);
        } else if(d.@type == "boolean"){
            enable.push(d + "=true");
            disable.push(d + "=false");
        } else {
            fail("Unknown build flag type [" + d.@type + "] in feature "+ featureName);
        }
    }

    var retval = "    arg = o.getBoolArg(\"" + featureName + "\", False, False)\n" +
                 "    if (arg == True):\n" +
                 "        buildFlags += \"" + enable.join(" ") + "\"\n" +
                 "    if (arg == False):\n" +
                 "        buildFlags += \"" + disable.join(" ") + "\"\n"

    return retval;
}

function groupConstraint(x, tagname, condition, english_condition) {
    var names = [];
    for each ( var name in x.name ) {
        if (!allnames.hasOwnProperty(name))
            fail("Name used in \"" + tagname + "\" not defined: " + name);
        names.push(name);
    }
    return "#if " + names.join("+") + " " + condition + " 1\n#  error \"" + english_condition + " one of " + names.join(",") + " must be defined.\"\n#endif\n";
}

function atLeastOne(x) {
    return groupConstraint(x, "at-least-one", "<", "At least");
}

function atMostOne(x) {
    return groupConstraint(x, "at-most-one", ">", "At most");
}

function exactlyOne(x) {
    return groupConstraint(x, "exactly-one", "!=", "Exactly");
}

function fail(msg) {
    print("Error: " + msg);
    System.exit(1);
}
