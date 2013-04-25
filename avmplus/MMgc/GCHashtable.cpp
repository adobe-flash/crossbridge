/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

namespace MMgc
{
    void* GCHashtableAllocHandler_VMPI::alloc(size_t size, bool canFail)
    {
        void* p = VMPI_alloc(size);
        if (!p && !canFail)
        {
            // ran out of memory...
            GCHeap::GetGCHeap()->Abort();
        }
        return p;
    }

    void GCHashtableAllocHandler_VMPI::free(void* ptr)
    {
        VMPI_free(ptr);
    }

    void* GCHashtableAllocHandler_new::alloc(size_t size, bool canFail)
    {
        return mmfx_alloc_opt(size, canFail ? kCanFail : kNone);
    }

    void GCHashtableAllocHandler_new::free(void* ptr)
    {
        mmfx_free(ptr);
    }
}

