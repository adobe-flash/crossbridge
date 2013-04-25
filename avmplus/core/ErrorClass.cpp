/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    ErrorClass::ErrorClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        AvmAssert(traits()->getSizeOfInstance() >= sizeof(ErrorClass));

        // trick: set our prototype to objectClass->prototypePtr for now...
        setPrototypePtr(toplevel()->objectClass->prototypePtr());
        // now we can create a proper proto, using this->ivtable() and object->proto
        ScriptObject* proto = newInstance();
        // and replace the one we had before.
        setPrototypePtr(proto);
    }

    /**
     * ErrorObject
     */
    ErrorObject::ErrorObject(VTable* vtable,
                             ScriptObject *delegate)
        : ScriptObject(vtable, delegate)
    {
        AvmAssert(traits()->getSizeOfInstance() >= sizeof(ErrorObject));

        AvmCore *core = this->core();
        const BugCompatibility *bugCompatibility = core->currentBugCompatibility();
        if (!bugCompatibility->bugzilla619148) { // Stack traces in release builds
#ifdef DEBUGGER
            if (!core->debugger())
                return;
#else
            return;
#endif
        }
        // Copy the stack trace
        stackTrace = core->newStackTrace();
    }

    Stringp ErrorObject::getStackTrace() const
    {
        AvmCore* core = this->core();
        const BugCompatibility *bugCompatibility = core->currentBugCompatibility();
        if (!bugCompatibility->bugzilla619148) {  // Stack traces in release builds
#ifdef DEBUGGER
            if (!core->debugger())
                return NULL;
#else
            return NULL;
#endif
        }

        // getStackTrace returns the concatenation of the
        // error message and the stack trace
        Stringp buffer = core->string(atom());
        buffer = core->concatStrings(buffer, core->newConstantStringLatin1("\n"));

        if (stackTrace) {
            buffer = core->concatStrings(buffer, stackTrace->format(core));
        }

        return buffer;
    }

    Stringp ErrorClass::getErrorMessage(int errorID) const
    {
        return this->core()->getErrorMessage(errorID);
    }

    /**
     * NativeErrorClass
     */

    NativeErrorClass::NativeErrorClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        AvmAssert(traits()->getSizeOfInstance() >= sizeof(ErrorClass));
        createVanillaPrototype();
    }
}
