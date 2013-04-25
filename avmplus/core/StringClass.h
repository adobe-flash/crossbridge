/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_StringClass__
#define __avmplus_StringClass__


namespace avmplus
{
    /**
     * class StringClass
     */
    class GC_AS3_EXACT(StringClass, ClassClosure)
    {
    protected:
        StringClass(VTable* cvtable);
        
    public:
        REALLY_INLINE static StringClass* create(MMgc::GC *gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) StringClass(cvtable);
        }

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv);

        // native methods.  see String.as
        ArrayObject* _match(Stringp s, Atom pattern);
        Stringp _replace(Stringp in, Atom pattern, Atom replacementAtom);
        int _search(Stringp in, Atom regexpAtom);
        ArrayObject* _split(Stringp in, Atom delimAtom, uint32_t limit);

        // defined via rest args to allow length = 1 and support calling with no args... ES3 spec says length = 1
        Stringp AS3_fromCharCode(Atom *argv, int argc);

        inline Stringp fromCharCode(Atom *argv, int argc) { return AS3_fromCharCode(argv, argc); }

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(StringClass)

        DECLARE_SLOTS_StringClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_StringClass__ */
