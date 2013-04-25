/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_JSONClass__
#define __avmplus_JSONClass__

namespace avmplus
{

    class GC_AS3_EXACT(JSONClass, ClassClosure)
    {
    protected:
        JSONClass(VTable* cvtable);
    public:
        Atom parseCore(String* text);

        String* stringifySpecializedToString(Atom value,
                                             ArrayObject* propertyWhitelist,
                                             FunctionObject* replacerFunction,
                                             String* gap);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(JSONClass)

        DECLARE_SLOTS_JSONClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_JSONClass__ */
