/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

#include "pcre.h"

namespace avmplus
{
    RegExpClass::RegExpClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        AvmAssert(traits()->getSizeOfInstance() == sizeof(RegExpClass));

        AvmCore* core = this->core();
        ScriptObject* object_prototype = toplevel()->objectClass->prototypePtr();
        String* pattern = core->newConstantStringLatin1("(?:)");
        setPrototypePtr(RegExpObject::create(core->GetGC(), cvtable->ivtable, object_prototype, pattern, core->kEmptyString));
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom RegExpClass::call(int argc, Atom* argv)
    {
        // ECMA-262 15.10.4.1: If pattern is RegExp and flags is undefined,
        // return pattern unchanged.
        if (argc > 0) {
            Atom flagsAtom = (argc>1) ? argv[2] : undefinedAtom;
            if (AvmCore::istype(argv[1], traits()->itraits) && flagsAtom == undefinedAtom) {
                return argv[1];
            }
        }

        // Otherwise, call the RegExp constructor.
        return construct(argc, argv);
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom RegExpClass::construct(int argc, Atom* argv)
    {
        AvmCore* core = this->core();
        Stringp pattern;

        Atom patternAtom = (argc>0) ? argv[1] : undefinedAtom;
        Atom optionsAtom = (argc>1) ? argv[2] : undefinedAtom;

        if (AvmCore::istype(patternAtom, traits()->itraits)) {
            // Pattern is a RegExp object
            if (optionsAtom != undefinedAtom) {
                // ECMA 15.10.4.1 says to throw an error if flags specified
                toplevel()->throwTypeError(kRegExpFlagsArgumentError);
            }
            // Return a clone of the RegExp object
            RegExpObject* regExpObject = (RegExpObject*)AvmCore::atomToScriptObject(patternAtom);
            return RegExpObject::create(core->GetGC(), this, regExpObject)->atom();
        } else {
            if (patternAtom != undefinedAtom) {
                pattern = core->string(argv[1]);
            } else {
                // cn:  disable this, breaking ecma3 tests.   was: todo look into this. it's what SpiderMonkey does.
                pattern = core->kEmptyString; //core->newConstantStringLatin1("(?:)");
            }
        }

        Stringp options = NULL;
        if (optionsAtom != undefinedAtom) {
            options = core->string(optionsAtom);
        }

        RegExpObject* inst = RegExpObject::create(core->GetGC(), this->ivtable(), this->prototypePtr(), pattern, options);
        return inst->atom();
    }
}
