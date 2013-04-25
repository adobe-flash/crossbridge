/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_BooleanClass__
#define __avmplus_BooleanClass__


namespace avmplus
{
    /**
     * class Boolean
     */
    class GC_AS3_EXACT(BooleanClass, ClassClosure)
    {
    protected:
        BooleanClass(VTable* vtable);

    public:
        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv)
        {
            return construct(argc, argv);
        }

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(BooleanClass)

        DECLARE_SLOTS_BooleanClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_BooleanClass__ */
