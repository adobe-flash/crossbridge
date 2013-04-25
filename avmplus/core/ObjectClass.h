/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ObjectClass__
#define __avmplus_ObjectClass__


namespace avmplus
{
    /**
     * class Object
     * base class for all objects in ES4
     */
    class GC_AS3_EXACT(ObjectClass, ClassClosure)
    {
    protected:
        ObjectClass(VTable* cvtable);
        
    public:
        REALLY_INLINE static ObjectClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) ObjectClass(cvtable);
        }

        void initPrototype();

        ScriptObject* construct();

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv);

        // native methods
        bool _hasOwnProperty(Atom thisAtom, Stringp name);
        bool _isPrototypeOf(Atom thisAtom, Atom other);
        bool _propertyIsEnumerable(Atom thisAtom, Stringp name);
        void _setPropertyIsEnumerable(Atom thisAtom, Stringp name, bool enumerable);
        Stringp _toString(Atom thisAtom);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(ObjectClass)

        DECLARE_SLOTS_ObjectClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_ObjectClass__ */
