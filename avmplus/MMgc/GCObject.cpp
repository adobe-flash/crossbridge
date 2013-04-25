/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "MMgc.h"

namespace MMgc
{
    bool GCTraceableBase::gcTrace(GC* gc, size_t cursor)
    {
        // The most-base tracer must do nothing, because it's the tracer that gets called if
        // the object becomes visible very early, or survives after destruction because it
        // was referenced from the mark stack (see GC::AbortFree).
        (void)gc;
        (void)cursor;
        return false;
    }
    
#ifdef MMGC_RC_HISTORY

    void RCObject::DumpHistory()
    {
        GCDebugMsg(false, "Ref count modification history for object %p:\n", this);
        for(uint32_t i=0, n=history.Count(); i<n; i++)
        {
            PrintStackTrace(history.Get(i));
        }
    }

#endif
}
