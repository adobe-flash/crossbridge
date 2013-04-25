/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nanojit.h"

#ifdef FEATURE_NANOJIT

namespace nanojit
{
#ifdef NANOJIT_IA32
    static void setCpuFeatures(Config* config)
    {
        int ecx_flags = 0;
        int edx_flags = 0;
    #if defined _MSC_VER
        __asm
        {
            pushad
            mov eax, 1
            cpuid
            mov edx_flags, edx
            mov ecx_flags, ecx
            popad
        }
    #elif defined __GNUC__
        asm("xchg %%esi, %%ebx\n" /* we can't clobber ebx on gcc (PIC register) */
            "mov $0x01, %%eax\n"
            "cpuid\n"
            "mov %%edx, %0\n"
            "mov %%ecx, %1\n"
            "xchg %%esi, %%ebx\n"
            : "=m" (edx_flags), "=m" (ecx_flags)
            : /* We have no inputs */
            : "%eax", "%esi", "%ecx", "%edx"
           );
    #elif defined __SUNPRO_C || defined __SUNPRO_CC
        asm("push %%ebx\n"
            "mov $0x01, %%eax\n"
            "cpuid\n"
            "pop %%ebx\n"
            : "=d" (edx_flags), "=c", (ecx_flags)
            : /* We have no inputs */
            : "%eax", "%ecx"
           );
    #endif

        config->i386_sse2 = (edx_flags & (1 << 26)) != 0;
        config->i386_sse3 = (ecx_flags & (1 << 0)) != 0;
        config->i386_sse41 = (ecx_flags & (1 << 19)) != 0;
        config->i386_use_cmov = (edx_flags & (1<<15)) != 0;
        config->i386_fixed_esp = false;
    }
#endif

    Config::Config()
    {
        VMPI_memset(this, 0, sizeof(*this));

        cseopt = true;
        harden_function_alignment = false;
        harden_nop_insertion = false;
        check_page_flags = false;

#ifdef NANOJIT_IA32
        setCpuFeatures(this);
#endif

//### FIXME: Thumb2 requires V7+.
#if defined(NANOJIT_ARM) || defined(NANOJIT_THUMB2)
        NanoStaticAssert(NJ_COMPILER_ARM_ARCH >= 4 && NJ_COMPILER_ARM_ARCH <= 7);
        arm_arch = NJ_COMPILER_ARM_ARCH;
        arm_vfp = (arm_arch >= 7);

    #if defined(DEBUG) || defined(_DEBUG)
        arm_show_stats = true;
    #else
        arm_show_stats = false;
    #endif

        soft_float = !arm_vfp;

#endif // NANOJIT_ARM
    }
}
#endif /* FEATURE_NANOJIT */
