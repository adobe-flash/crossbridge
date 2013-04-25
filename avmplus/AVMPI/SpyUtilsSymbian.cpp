/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

#ifdef MMGC_MEMORY_PROFILER

uint64_t spyLastTime = 0;

void VMPI_spyCallback()
{
    uint64_t time = VMPI_getTime();
    if((time - spyLastTime) > 10000)
    {
        spyLastTime = time;
        MMgc::GCHeap::GetGCHeap()->DumpMemoryInfo();
    }
}

bool VMPI_spySetup()
{
    return true;
}

bool VMPI_hasSymbols()
{
    return true;
}

#endif //MMGC_MEMORY_PROFILER
