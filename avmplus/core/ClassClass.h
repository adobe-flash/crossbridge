/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ClassClass__
#define __avmplus_ClassClass__


namespace avmplus
{
    /**
     * class Class - the builtin Class that defines the type of each user defined class
     * and all other builtin classes.  (not to be confused with a superclass).
     */
    class GC_AS3_EXACT(ClassClass, ClassClosure)
    {
    protected:
        ClassClass(VTable* cvtable);

    public:
        REALLY_INLINE static ClassClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) ClassClass(cvtable);
        }

    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(ClassClass)

    private:
        DECLARE_SLOTS_ClassClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_ClassClass__ */
