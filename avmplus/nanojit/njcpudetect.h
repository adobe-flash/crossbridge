/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __njcpudetect__
#define __njcpudetect__

/***
 * Note: this file should not include *any* other files, nor should it wrap
 * itself in ifdef FEATURE_NANOJIT, nor should it do anything other than
 * define preprocessor symbols.
 */

/***
 * NJ_COMPILER_ARM_ARCH attempts to specify the minimum ARM architecture
 * that the C++ compiler has specified. Note that although Config::arm_arch
 * is initialized to this value by default, there is no requirement that they
 * be in sync.
 *
 * Note, this is done via #define so that downstream preprocessor usage can
 * examine it, but please don't attempt to redefine it.
 *
 * Note, this is deliberately not encased in "ifdef NANOJIT_ARM", as this file
 * may be included before that is defined. On non-ARM platforms we will hit the
 * "Unable to determine" case.
 */

// GCC and RealView usually define __ARM_ARCH__
#if defined(__ARM_ARCH__)

    #define NJ_COMPILER_ARM_ARCH __ARM_ARCH__

// ok, try well-known GCC flags ( see http://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html )
#elif     defined(__ARM_ARCH_7__) || \
        defined(__ARM_ARCH_7A__) || \
        defined(__ARM_ARCH_7M__) || \
        defined(__ARM_ARCH_7R__) || \
        defined(_ARM_ARCH_7)

    #define NJ_COMPILER_ARM_ARCH 7

#elif   defined(__ARM_ARCH_6__) || \
        defined(__ARM_ARCH_6J__) || \
        defined(__ARM_ARCH_6K__) || \
        defined(__ARM_ARCH_6T2__) || \
        defined(__ARM_ARCH_6Z__) || \
        defined(__ARM_ARCH_6ZK__) || \
        defined(__ARM_ARCH_6M__) || \
        defined(_ARM_ARCH_6)

    #define NJ_COMPILER_ARM_ARCH 6

#elif   defined(__ARM_ARCH_5__) || \
        defined(__ARM_ARCH_5T__) || \
        defined(__ARM_ARCH_5E__) || \
        defined(__ARM_ARCH_5TE__)

    #define NJ_COMPILER_ARM_ARCH 5

#elif   defined(__ARM_ARCH_4T__)

    #define NJ_COMPILER_ARM_ARCH 4

// Visual C has its own mojo
#elif defined(_MSC_VER) && defined(_M_ARM)

    #define NJ_COMPILER_ARM_ARCH _M_ARM

// RVCT
#elif defined(__TARGET_ARCH_ARM)

    #define NJ_COMPILER_ARM_ARCH __TARGET_ARCH_ARM

#else

    // non-numeric value
    #define NJ_COMPILER_ARM_ARCH "Unable to determine valid NJ_COMPILER_ARM_ARCH (nanojit only supports ARMv4T or later)"

#endif

#endif // __njcpudetect__
