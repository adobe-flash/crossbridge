/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_DateClass__
#define __avmplus_DateClass__


namespace avmplus
{
    /**
     * class DateClass
     */
    class GC_AS3_EXACT(DateClass, ClassClosure)
    {
    protected:
        DateClass(VTable* cvtable);

    public:

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv);

        /** @name instance methods */
        /*@{*/
        Atom get(Atom thisAtom, Atom *args, int argc, int index);
        Atom set(Atom thisAtom, Atom *args, int argc, int index);
        /*@}*/

        /** @name static methods */
        /*@{*/
        double parse(Atom input);
        double UTC(Atom year, Atom month, Atom date,
                   Atom hours, Atom minutes, Atom seconds, Atom ms,
                   Atom *args, int argc);
        /*@}*/

    private:
        double      stringToDateDouble(Stringp s);

    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(DateClass)

    private:
        DECLARE_SLOTS_DateClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_DateClass__ */
