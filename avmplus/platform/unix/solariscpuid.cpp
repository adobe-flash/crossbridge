/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdio.h>
#include <sys/types.h>
#include <sys/elf.h>
#include <sys/fcntl.h>
struct cpuid_get_hwcap {
    char *cgh_archname;
    uint_t cgh_hwcap;
};
#ifndef AV_SPARC_V8PLUS // For OpenSolaris, these were defined
#define AV_SPARC_V8PLUS     0x0008
#define AV_SPARC_VIS        0x0020
#define AV_SPARC_VIS2       0x0040
#endif

#ifndef AV_386_MMX // For OpenSolaris, these were defined
#define AV_386_MMX          0x0040
#define AV_386_AMD_MMX      0x0080
#define AV_386_AMD_3DNow    0x0100
#define AV_386_AMD_3DNowx   0x0200
#define AV_386_SSE          0x0800
#define AV_386_SSE2         0x1000
#define AV_386_SSE3         0x4000
#endif
extern "C" int open(const char *, int, ...);
extern "C" int ioctl(int, int, ...);
extern "C" int close(int);

static unsigned int GetFeatureFlags()
{
    static bool checked = false;
    static unsigned int flag = 0;
    if (checked) return flag;

    checked = true;
    static const char dev_cpu_self_cpuid[] = "/dev/cpu/self/cpuid";
#ifdef AVMPLUS_SPARC
    static const char isa[] = "sparc";
#else
    static const char isa[] = "i386";
#endif
    int d = -1;
    d = open(dev_cpu_self_cpuid, O_RDONLY);
    if (d == -1)
      return 0;

    struct cpuid_get_hwcap __cgh, *cgh = &__cgh;

    cgh->cgh_archname = (char *)isa;
    if (ioctl(d, (('c' << 24) | ('i' << 16) | ('d' << 8) | 0) /* CPUID_GET_HWCAP */,
        cgh) == 0)
      flag = cgh->cgh_hwcap;

    close(d);

    return flag;
}

bool P4Available()
{
#ifdef AVMPLUS_SPARC
    return false;
#else
    static int checked = 0;
    if ( !checked ) {
        unsigned int dwFeatures = GetFeatureFlags();
        if (dwFeatures & AV_386_SSE2) {
            checked = 2;
            return true;
        }
        checked = 1;
    }
    if ( checked == 2 ) {
        return true;
    }
    return false;
#endif
}
