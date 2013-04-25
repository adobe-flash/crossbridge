/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_IntClass__
#define __avmplus_IntClass__


namespace avmplus
{
    /**
     * class int
     */
    class GC_AS3_EXACT(IntClass, ClassClosure)
    {
    protected:
        IntClass(VTable* cvtable);
    public:
        REALLY_INLINE static IntClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) IntClass(cvtable);
        }

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom *argv)
        {
            return construct(argc, argv);
        }

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(IntClass)

        DECLARE_SLOTS_IntClass;
    // ------------------------ DATA SECTION END
    };

    /**
     * class uint
     */
    class GC_AS3_EXACT(UIntClass, ClassClosure)
    {
    protected:
        UIntClass(VTable* cvtable);
    public:
        REALLY_INLINE static UIntClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) UIntClass(cvtable);
        }
        
        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv)
        {
            return construct(argc,argv);
        }

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(UIntClass)

        DECLARE_SLOTS_UIntClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_IntClass__ */
