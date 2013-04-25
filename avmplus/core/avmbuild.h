/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmbuild__
#define __avmbuild__

// Note, the goal is to keep this file small and for it to concern itself mostly
// with debugging settings useful for VM development and other ad-hocery.  In
// general all externally visible configuration should be handled by the configuration
// system in core/avmfeatures.as.
//
// For sanity's sake, please note the reason for the presence of each clause in the
// following.

// This is here as a sanity check, nuke after a few more release cycles (now == Apr-2009).
#ifdef AVMPLUS_DISABLE_NJ
    #error "AVMPLUS_DISABLE_NJ is no longer supported"
#endif

// This is here because the configuration system does not deal with DEBUG
#if defined DEBUG && !defined AVMPLUS_VERBOSE && !defined VMCFG_SYMBIAN
    #define AVMPLUS_VERBOSE
#endif

// This is here because the conditional for MacTel can't be expressed by the
// feature system (yet).
// FIXME: extend the feature system to handle this.
//
// all x64, and all MacTel machines, always have sse2
#if defined(AVMPLUS_AMD64) || (defined(AVMPLUS_MAC) && defined(AVMPLUS_IA32))
    #ifndef AVMPLUS_SSE2_ALWAYS
        #define AVMPLUS_SSE2_ALWAYS
    #endif
#endif

// This is here because it's an always-enabled switch to externally sourced code:
// PCRE should always be compiled as a statically linked library, never as a shared
// library.  See pcre/pcre_internal.h, pcre/pcre.h, and pcre/config.h.
#define PCRE_STATIC

// The PERFM macros are here because they are for VM debugging.
//
// Enable performance metrics for NJ
//
// *** NOTE ON THREAD SAFETY ***
// PERFM is not supported in configurations where more than one AvmCore is
// instantiated in the same process, as the monitoring code is not thread
// safe and the results will be unpredictable.

//#define PERFM

#ifdef PERFM
# define PERFM_NVPROF(n,v) _nvprof(n,v)
# define PERFM_NTPROF_BEGIN(n) _ntprof_begin(n)
# define PERFM_NTPROF_END(n) _ntprof_end(n)
#else
# define PERFM_NVPROF(n,v)
# define PERFM_NTPROF_BEGIN(n)
# define PERFM_NTPROF_END(n)
#endif

// This is here for VM performance profiling.
//
// The use of the SUPERWORD_PROFILING switch is described in comments at the head of
// utils/superwordprof.c
//
// The limit is optional and describes a cutoff for sampling; the program continues to
// run after sampling ends but data are no longer gathered or stored.  A limit of 250e6
// produces 1GB of sample data.  There is one sample per VM instruction executed.
//
// *** NOTE ON THREAD SAFETY ***
// SUPERWORD_PROFILING is not supported in configurations where more than one
// AvmCore is instantiated in the same process, as the monitoring code is not
// thread safe and the results will be unpredictable.

//#define SUPERWORD_PROFILING
//#define SUPERWORD_LIMIT 250000000

#ifdef SUPERWORD_PROFILING
#  ifndef VMCFG_WORDCODE
#    error "You must have word code enabled to perform superword profiling"
#  endif
#  ifdef VMCFG_DIRECT_THREADED
#    error "You must disable direct threading to perform superword profiling"
#  endif
#endif

//Enable this if compiling for fuzz testing, where informational asserts are not desired.
//There are only a few informational asserts in the source code.
//#define VMCFG_FUZZTESTING

// Optimize ...rest arguments and the 'arguments' array.
//
// This is here because the conditions under which this optimization is used
// depends on other settings and is not an external-facing concern, nor should
// code implementing the optimization know directly which external settings that
// trigger it.
//
// We could use this for the wordcode interpreter, too; it might be a win.
#ifdef VMCFG_NANOJIT
    #define VMCFG_RESTARG_OPTIMIZATION
#endif

// Enable setting the OSR threshold via an environment variable "OSR".
// This may be useful in certain testing scenarios, but should not be included in production
// releases.  It is a temporary and unsupported facility that may be removed entirely.
//#define VMCFG_OSR_ENV_VAR

// Enable more agressive use of type-specialized helper functions and speculative
// inlining in the JIT.
// It is likely that this will want to become a feature, as the space-for-speed
// tradeoff may not be appropriate for all target platforms and host embeddings.

#define VMCFG_JIT_FASTPATH

// Individual optimizations should be controlled by separate VMCFG variables
// to facilitate performance analsyis experiments.

#ifdef VMCFG_JIT_FASTPATH

    // Generate inline fastpath for converting numeric atom to double.
    // Required opcode LIR_q2d for 64-bit support is not implemented.
    #ifndef AVMPLUS_64BIT
        #define VMCFG_FASTPATH_FROMATOM
    #endif

    // Use type-specialized helper functions for addition.
    #define VMCFG_FASTPATH_ADD

    // Inline fastpaths for additions involving kIntptrType atoms.
    // Inlining requires the addjovi instruction, which is not
    // implemented on PPC or Sparc.
    // PPC:   https://bugzilla.mozilla.org/show_bug.cgi?id=601266
    // Sparc: https://bugzilla.mozilla.org/show_bug.cgi?id=603876
    #if defined(VMCFG_FASTPATH_ADD) && !(defined(AVMPLUS_PPC) || defined(AVMPLUS_SPARC))
        #define VMCFG_FASTPATH_ADD_INLINE
    #endif

#endif

// enable deopt trampolines on halfmon-x86 configurations
#if defined(VMCFG_HALFMOON) && defined(VMCFG_IA32)
#  define NANOJIT_FRAMELIB
#  define NANOJIT_EAGER_REGSAVE
#endif

// Enable stack metrics API (for development purposes only)
//#define VMCFG_STACK_METRICS

#endif /* __avmbuild__ */
