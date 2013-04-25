/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VMPI.h"

#include <mach/mach.h>
#include <mach/mach_time.h>

// Call VMPI_getPerformanceFrequency() once to initialize its cache; avoids thread safety issues.
static uint64_t unused_value = VMPI_getPerformanceFrequency();

uint64_t VMPI_getPerformanceFrequency()
{
    // *** NOTE ABOUT THREAD SAFETY ***
    //
    // These statics ought to be safe because they are initialized by a call at startup
    // (see lines above this function), before any AvmCores are created.
    
    static mach_timebase_info_data_t info;
    static uint64_t frequency = 0;
    if ( frequency == 0 ) {
        (void) mach_timebase_info(&info);
        frequency = (uint64_t) ( 1e9 / ((double) info.numer / (double) info.denom) );
    }
    return frequency;
}

uint64_t VMPI_getPerformanceCounter()
{
    return mach_absolute_time();
}

static size_t computePagesize()
{
    long pagesize = sysconf(_SC_PAGESIZE);
    // MacOS X 10.1 needs the extra check
    if (pagesize == -1)
        pagesize = 4096;
    return size_t(pagesize);
}

// Private to VMPI_getVMPageSize; DO NOT REFERENCE THIS VARIABLE ELSEWHERE.
// Always call VMPI_getVMPageSize, even inside this file.

static size_t pagesize = computePagesize();

size_t VMPI_getVMPageSize()
{
    return pagesize;
}
