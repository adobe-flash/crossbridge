/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avm {
using avmplus::Atom;
using avmplus::CodeContext;
using avmplus::MethodEnv;
using avmplus::ScriptObject;
using avmplus::Toplevel;

// you probably don't want to open this namespace.
// using namespace avmplus;

bool isFunction(const ScriptObject* so)
{
    return so && so->core()->isFunction(so->atom());
}

CodeContext* getFunctionCodeContext(const ScriptObject* so)
{
    if (!isFunction(so)) {
        AvmAssert(!"Only Function is legal here.");
        return NULL;
    }
//  CodeContext* cc = ((avmplus::FunctionObject*)so)->getFunctionCodeContext();
// getFunctionCodeContext() has been temporarily relocated to ScriptObject, as AIR defines
// some classes that are subclasses of Function (in AS3) but not of FunctionObject (in C++)...
    avmplus::CodeContext* cc = ((avmplus::ScriptObject*)so)->getFunctionCodeContext();
    return cc;
}

CodeContext* getClassCodeContext(const ScriptObject* so)
{
    if (!so)
        return NULL;
    if (so->core()->isFunction(so->atom())) {
        AvmAssert(!"Function or MC is not legal here.");
        return NULL;
    }
    avmplus::TraitsPosType t = so->traits()->posType();
    if (t == avmplus::TRAITSTYPE_CATCH || t == avmplus::TRAITSTYPE_ACTIVATION) {
        AvmAssert(!"Activation and Catch objects are not legal here.");
        return NULL;
    }
    MethodEnv* init = so->vtable->init;
    if (!init) {
        AvmAssert(!"init method is null, should not be possible.");
        return NULL;
    }
    return init->scope()->abcEnv()->codeContext();
}

Atom callFunction(Toplevel* toplevel, Atom f, int argc, Atom* args)
{
    return avmplus::op_call(toplevel, f, argc, args);
}

Atom callFunction(ScriptObject* f, int argc, Atom* args)
{
    AvmAssertMsg(f != NULL, "f must not be null. Use callFunction(toplevel, atom, ...) form");
    return f->call(argc, args);
}

Atom callFunction(ScriptObject* f)
{
    AvmAssertMsg(f != NULL, "f must not be null. Use callFunction(toplevel, atom, ...) form");
    Atom args[] = { nullObjectAtom };
    return f->call(0, args);
}

} // namespace avm

