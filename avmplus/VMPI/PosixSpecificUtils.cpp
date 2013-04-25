/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "VMPI.h"

#include <sys/mman.h>
#include <sys/time.h>

uint64_t VMPI_getPerformanceFrequency()
{
    return 1000000;
}

uint64_t VMPI_getPerformanceCounter()
{
#ifdef AVMPLUS_UNIX
    struct timeval tv;
    ::gettimeofday(&tv, NULL);

    uint64_t seconds = (uint64_t)tv.tv_sec * 1000000;
    uint64_t microseconds = (uint64_t)tv.tv_usec;
    uint64_t result = seconds + microseconds;

    return result;
#else
    #error "High resolution timer needs to be defined for this platform"
#endif
}

static size_t pagesize = size_t(sysconf(_SC_PAGESIZE));

size_t VMPI_getVMPageSize()
{
    return pagesize;
}
