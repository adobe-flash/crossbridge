/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_system_selection__
#define __avmplus_system_selection__

// Standard operating system, CPU, word size, and endianness selection
// for tier-one AVM platforms.  This file defines all the AVMSYSTEM_
// names.  It would normally be included into a feature definition file.

// operating system

#ifdef UNIX
  #define AVMSYSTEM_UNIX 1
#else
  #define AVMSYSTEM_UNIX 0
#endif
#ifdef _MAC
  #define AVMSYSTEM_MAC  1
#else
  #define AVMSYSTEM_MAC  0
#endif
#ifdef WIN32
  #define AVMSYSTEM_WIN32 1
#else
  #define AVMSYSTEM_WIN32 0
#endif
#ifdef __SYMBIAN32__ // defined by Symbian S60 tool chain
  #define AVMSYSTEM_SYMBIAN 1
#else
  #define AVMSYSTEM_SYMBIAN 0
#endif

#ifdef WEBOS
  #define AVMSYSTEM_WEBOS 1
#else
  #define AVMSYSTEM_WEBOS 0
#endif

// cpu, word size

#undef SIXTYFOURBIT

#if AVMSYSTEM_MAC || AVMSYSTEM_UNIX

  #if defined(__i386__) || defined(__i386)
    #define AVMSYSTEM_IA32   1
  #else
    #define AVMSYSTEM_IA32   0
  #endif

  #if defined(__x86_64__)
    #define AVMSYSTEM_AMD64  1
    #define SIXTYFOURBIT
  #else
    #define AVMSYSTEM_AMD64  0
  #endif

  #if defined(__ppc__) || defined(__powerpc__) || (__ppc64__) || (__powerpc64__)
    #define AVMSYSTEM_PPC    1
    #if (__ppc64__) || (__powerpc64__)
      #define SIXTYFOURBIT
    #endif
  #else
    #define AVMSYSTEM_PPC    0
  #endif

  #if defined(__arm__) || defined(__ARM__)
    #define AVMSYSTEM_ARM    1
  #else
    #define AVMSYSTEM_ARM    0
  #endif

  #if defined(__sparc__) || defined(__sparc)
    #define AVMSYSTEM_SPARC  1
  #else
    #define AVMSYSTEM_SPARC  0
  #endif

  #if defined(__mips__) || defined(__MIPS__)
    #define AVMSYSTEM_MIPS   1
  #else
    #define AVMSYSTEM_MIPS   0
  #endif

  #if defined(__SH4__)
    #define AVMSYSTEM_SH4   1
  #else
    #define AVMSYSTEM_SH4   0
  #endif

#endif  // mac || unix

#if AVMSYSTEM_WIN32

  #ifdef _M_X64
    #define AVMSYSTEM_AMD64  1
    #define SIXTYFOURBIT
  #else
    #define AVMSYSTEM_AMD64  0
  #endif

  #if defined(_ARM_)
    #define AVMSYSTEM_ARM    1
  #else
    #define AVMSYSTEM_ARM    0
  #endif

  #if !AVMSYSTEM_AMD64 && !AVMSYSTEM_ARM
    #define AVMSYSTEM_IA32 1
  #else
    #define AVMSYSTEM_IA32 0
  #endif

  #define AVMSYSTEM_PPC    0
  #define AVMSYSTEM_SPARC  0
  #define AVMSYSTEM_SH4	   0
  #define AVMSYSTEM_MIPS   0

#endif  // win32

#if AVMSYSTEM_SYMBIAN

  #define AVMSYSTEM_PPC     0
  #define AVMSYSTEM_AMD64   0
  #define AVMSYSTEM_SPARC   0
  #define AVMSYSTEM_MIPS   0
  #define AVMSYSTEM_SH4    0

  #if defined(__ARMCC__)
    #define AVMSYSTEM_ARM    1
  #else
    #define AVMSYSTEM_ARM    0
  #endif

  #if !AVMSYSTEM_ARM
    #define AVMSYSTEM_IA32 1
  #else
    #define AVMSYSTEM_IA32 0
  #endif

  #ifdef __WINSCW__
    #define AVMTWEAK_EPOC_EMULATOR 1
  #endif

#endif // symbian

#ifdef SIXTYFOURBIT
  #define AVMSYSTEM_32BIT 0
  #define AVMSYSTEM_64BIT 1
#else
  #define AVMSYSTEM_32BIT 1
  #define AVMSYSTEM_64BIT 0
#endif

#undef SIXTYFOURBIT

// endianness

#if AVMSYSTEM_IA32 || AVMSYSTEM_AMD64
  #define AVMSYSTEM_LITTLE_ENDIAN       1
  #define AVMSYSTEM_BIG_ENDIAN          0
#elif AVMSYSTEM_ARM
  #if defined _MSC_VER
    #define AVMSYSTEM_LITTLE_ENDIAN     1
    #define AVMSYSTEM_BIG_ENDIAN        0
  #elif defined __GNUC__
    #if AVMSYSTEM_MAC && AVMSYSTEM_ARM
        #include <machine/endian.h>
        #define __BYTE_ORDER BYTE_ORDER
    #else
        #include <endian.h>
    #endif
    #if __BYTE_ORDER == LITTLE_ENDIAN
      #define AVMSYSTEM_LITTLE_ENDIAN   1
      #define AVMSYSTEM_BIG_ENDIAN      0
      #if __FLOAT_WORD_ORDER == BIG_ENDIAN
        #define AVMSYSTEM_DOUBLE_MSW_FIRST 1
      #endif
    #else
      #define AVMSYSTEM_LITTLE_ENDIAN   0
      #define AVMSYSTEM_BIG_ENDIAN      1
    #endif
  #elif defined __ARMCC__
    #define AVMSYSTEM_LITTLE_ENDIAN   1
    #define AVMSYSTEM_BIG_ENDIAN      0
  #endif
#elif AVMSYSTEM_PPC || AVMSYSTEM_SPARC
  #define AVMSYSTEM_LITTLE_ENDIAN       0
  #define AVMSYSTEM_BIG_ENDIAN          1
#elif AVMSYSTEM_MIPS || AVMSYSTEM_SH4
  #if defined __GNUC__
    #include <endian.h>
    #if __BYTE_ORDER == LITTLE_ENDIAN
      #define AVMSYSTEM_LITTLE_ENDIAN   1
      #define AVMSYSTEM_BIG_ENDIAN      0
    #else
      #define AVMSYSTEM_LITTLE_ENDIAN   0
      #define AVMSYSTEM_BIG_ENDIAN      1
    #endif
  #endif
#else
  #error "Error in test to determine endianness"
#endif

// unaligned access

#if AVMSYSTEM_IA32 || AVMSYSTEM_AMD64
  #define AVMSYSTEM_UNALIGNED_INT_ACCESS 1
  #define AVMSYSTEM_UNALIGNED_FP_ACCESS  1
#elif AVMSYSTEM_ARM
  #if AVMSYSTEM_WEBOS
    // At the time of this writing (Feb 2010), Palm's webOS deliberately enables software interrupts
    // for all unaligned accesses, apparently in the name of promoting "clean code", making unaligned
    // accesses vastly slower than aligned (reportedly on the order of 1000x). Unless they change this
    // policy (or give us a way to change it selectively) we will consider all webOS builds not to
    // support unaligned access, regardless of the processor variant.
    #define AVMSYSTEM_UNALIGNED_INT_ACCESS 0
  #else
    //
    // ARM is a little complicated:
    //
    // ARMv5 (e.g. ARM926): No support for unaligned accesses.
    // ARMv6 (e.g. ARM1176): Optional support that must be enabled by the OS.
    // ARMv7 (e.g. Cortex-A8): Unaligned access support cannot be disabled; you always have it.
    //

    // GCC and RealView usually define __ARM_ARCH__,
    // otherwise try well-known GCC flags ( see http://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html )
    #if __ARM_ARCH__ >= 7 ||  defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) ||  defined(__ARM_ARCH_7M__) || \
        defined(__ARM_ARCH_7R__) || defined(_ARM_ARCH_7) || defined(UNDER_RT)

      #define AVMSYSTEM_UNALIGNED_INT_ACCESS 1
    #else
      #define AVMSYSTEM_UNALIGNED_INT_ACCESS 0
    #endif
  #endif
  // VFP rules are different from int rules on ARM
  #define AVMSYSTEM_UNALIGNED_FP_ACCESS  0
#elif AVMSYSTEM_PPC || AVMSYSTEM_SPARC || AVMSYSTEM_MIPS || AVMSYSTEM_SH4
  #define AVMSYSTEM_UNALIGNED_INT_ACCESS 0
  #define AVMSYSTEM_UNALIGNED_FP_ACCESS  0
#else
  #error "Error in test to determine endianness"
#endif

#ifndef AVMSYSTEM_DOUBLE_MSW_FIRST
  #define AVMSYSTEM_DOUBLE_MSW_FIRST 0
#endif

#endif
