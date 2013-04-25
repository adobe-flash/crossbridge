/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_AvmPlusScriptableObject__
#define __avmplus_AvmPlusScriptableObject__


namespace avmplus
{

#ifdef DEBUGGER
    // This really ought to be defined in Sampler.h, but is here to simplify header-file inclusion
    // order issues.
    typedef struct SamplerObjectType_* SamplerObjectType;
#endif

    class AvmPlusScriptableObject : public MMgc::RCObject
    {
    public:
        // used by WeakValueHashtable to correctly atom'ize a pointer to one of these
        virtual Atom toAtom() const = 0;

#ifdef DEBUGGER
        AvmPlusScriptableObject(SamplerObjectType sot);

        /**
         * Returns the number of bytes of memory taken by this object, that are not
         * also shared by any other objects.  For example, a dependent String would
         * include sizeof(String) but not the size of the buffer of the master string;
         * but a dynamic String would include both sizeof(String) and the size of
         * the buffer.
         *
         * Used by the profiler to tell the user the shallow size of the object.
         */
        virtual uint64_t bytesUsed() const = 0;
#endif

        inline AvmCore* core() const
        {
            return (AvmCore*)MMgc::GC::GetGC(this)->core();
        }
    };
}

#endif //__avmplus_AvmPlusScriptableObject__
