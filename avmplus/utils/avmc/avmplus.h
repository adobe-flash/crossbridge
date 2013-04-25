/* -*- tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// replacement for avmplus.h for the eval code.
//
// You also need to define a couple of things on your command line or in your project file:
//
//  AVMC_STANDALONE    (to tweak the evaluator in a couple of places)
//  VMCFG_EVAL         (to enable the evaluator)

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

// The following represent dependencies that eval makes on avmplus.
// We also depend on the opcode table in ActionBlockConstants.h

typedef uint16_t wchar;

#ifdef DEBUG
    #define AvmAssert assert
#else
    #define AvmAssert(x) (void)(0)
#endif

#define MMGC_STATIC_ASSERT(x) enum {}

#define VMPI_sprintf sprintf
#define VMPI_snprintf snprintf
#define VMPI_vsnprintf vsnprintf
#define VMPI_strcpy strcpy
#define VMPI_strlen strlen
#define VMPI_memcmp memcmp
#define VMPI_memcpy memcpy

#include "ActionBlockConstants.h"

inline void *operator new(size_t size, void* p) { return p; }

union double_overlay
{
    double_overlay() {}
    double_overlay(double d) { value=d; }
    double_overlay(uint64_t v) { bits64=v; }
    
    double value;
#if defined VMCFG_BIG_ENDIAN || defined VMCFG_DOUBLE_MSW_FIRST
    struct { uint32_t msw, lsw; } words;
#else
    struct { uint32_t lsw, msw; } words;
#endif
    uint32_t bits32[2];
    uint64_t bits64;
};

class MathUtils
{
public:
    static bool isNaN(double x);
    static bool isInfinite(double x);
    static const double kNaN;
};

class MethodInfo
{
public:
	static const int NEED_ARGUMENTS		= 0x00000001;
	static const int NEED_ACTIVATION	= 0x00000002;
	static const int NEED_REST          = 0x00000004;
	static const int HAS_OPTIONAL       = 0x00000008;
	static const int IGNORE_REST        = 0x00000010;
	static const int NATIVE				= 0x00000020;
	static const int SETS_DXNS			= 0x00000040;
	static const int HAS_PARAM_NAMES	= 0x00000080;
};
