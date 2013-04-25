/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_NamespaceClass__
#define __avmplus_NamespaceClass__


namespace avmplus
{
    /**
     * class Namespace - the type of namespace objects
     */
    class GC_AS3_EXACT(NamespaceClass, ClassClosure)
    {
    protected:
        NamespaceClass(VTable* cvtable);
        
    public:
        REALLY_INLINE static NamespaceClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) NamespaceClass(cvtable);
        }

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call (int argc, Atom* argv);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(NamespaceClass)

        DECLARE_SLOTS_NamespaceClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_NamespaceClass__ */
