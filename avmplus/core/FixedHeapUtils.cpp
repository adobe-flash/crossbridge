/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{

    
    FixedHeapRCObject::FixedHeapRCObject()
        : m_rcount(0)
    {
    }
        
    int32_t FixedHeapRCObject::IncrementRef() 
    {
        // worry about overflow?
        return m_rcount.incAndGet();
    }

    int32_t FixedHeapRCObject::DecrementRef() 
    {
        int32_t current = m_rcount.decAndGet();
        AvmAssert(current >= 0);
        if (current == 0)
            destroy();
        return current;
    }

    int32_t FixedHeapRCObject::RefCount()
    {
        return m_rcount.get();
    }
    
    
    FixedHeapRCObject::~FixedHeapRCObject()
    {
    }

}
