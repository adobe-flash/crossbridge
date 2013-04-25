/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __GCTypes__
#define __GCTypes__

#include "VMPI.h"

namespace MMgc
{
    typedef void* (*GCMallocFuncPtr)(size_t size);
    typedef void (*GCFreeFuncPtr)(void* mem);

    enum FixedMallocOpts { kNone=0, kZero=1, kCanFail=2, kCanFailAndZero=3 };

    enum GCTracerCheckResult { kPassDueToManualTracer, kOffsetFound, kOffsetNotFound };

    #ifndef NULL
    #define NULL 0
    #endif
}

#endif /* __GCTypes__ */
