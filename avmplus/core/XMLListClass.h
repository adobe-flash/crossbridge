/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_XMLListClass__
#define __avmplus_XMLListClass__


namespace avmplus
{
    /**
     * class XMLListClass
     */
    class GC_AS3_EXACT(XMLListClass, ClassClosure)
    {
    protected:
        XMLListClass(VTable* cvtable);
    public:
        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv);

        Atom ToXMLList(Atom arg);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(XMLListClass)

        DECLARE_SLOTS_XMLListClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_XMLListClass__ */
