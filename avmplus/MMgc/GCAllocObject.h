/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCAllocObject__
#define __GCAllocObject__


namespace MMgc
{
    /**
     * GCAllocObject is a base class for allocated objects.  MMgc
     * cannot have any dependency on the C runtime library, because
     * Flash Player does not.  MMgc also cannot rely on global
     * operator new and delete, because it may be used to implement
     * them!  So, GCAllocObject provides a simple operator new and
     * delete implementation for classes used internally by MMgc.
     *
     * The implementation of GCAllocObject is platform-dependent.
     */
    class GCAllocObject
    {
    public:
        void* operator new (size_t size);
        void* operator new [] (size_t size);
        void operator delete (void *ptr);
        void operator delete [] (void *ptr);
    };
}

#endif /* __GCAllocObject__ */
