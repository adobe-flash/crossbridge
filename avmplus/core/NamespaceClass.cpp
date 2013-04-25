/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    NamespaceClass::NamespaceClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        toplevel()->_namespaceClass = this;
        AvmAssert(traits()->getSizeOfInstance() == sizeof(NamespaceClass));
        createVanillaPrototype();
    }


    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom NamespaceClass::construct(int argc, Atom* argv)
    {
        AvmCore* core = this->core();

        // See E4X 13.2.2, pg 65
        if (argc < 1)
            return core->newNamespace(core->kEmptyString->atom())->atom();
        else if (argc == 1)
            return core->newNamespace(argv[1])->atom();
        else
        {
            // Rhino throws an error when prefix is specified and uri is not a valid string
            String *p = core->string (argv[1]);
            String *u = core->string (argv[2]);
            if (p->length() && !u->length())
            {
                toplevel()->throwTypeError(kXMLNamespaceWithPrefixAndNoURI, p);
            }

            return core->newNamespace(argv[1], argv[2])->atom();
        }
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom NamespaceClass::call(int argc, Atom* argv)
    {
        if ((argc == 1) && (core()->isNamespace(argv[1])))
            return argv[1];
        else
            return construct (argc, argv);
    }
}
