/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_OutputStream__
#define __avmplus_OutputStream__


namespace avmplus
{
    // The OutputStream classes are abstract base classes for output streams.

    /**
     * GCOutputStream is a base class for output streams that are allocated on
     * the managed heap.
     *
     * GCOutputStream instances *must always* be allocated on the GC'd heap.
     */
    class GCOutputStream : public MMgc::GCFinalizedObject
    {
    public:
        GCOutputStream() {}
        virtual void write(const char* utf8) = 0;  // null terminated-utf8 data
        virtual void writeN(const char* utf8, size_t charCount) = 0;  // fixed amount of utf8 data
    };

    /**
     * NonGCOutputStream is a base class for output streams that are allocated on
     * on the stack, inside GCRoots, or in unmanaged (malloc'd) memory.
     *
     * NonGCOutputStream instances *must never* be allocated on the GC'd heap.
     */
    class NonGCOutputStream
    {
    private:
        // This operator is private and not implemented, in order to catch errors.
        static void *operator new(size_t size, MMgc::GC *gc);
        
    public:
        NonGCOutputStream() {}
        virtual ~NonGCOutputStream() {}
        virtual void write(const char* utf8) = 0;  // null terminated-utf8 data
        virtual void writeN(const char* utf8, size_t charCount) = 0;  // fixed amount of utf8 data
    };
}

#endif /* __avmplus_OutputStream__ */
