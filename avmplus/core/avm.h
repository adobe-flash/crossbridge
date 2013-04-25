/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avm__
#define __avm__

#include "VMPI.h"


namespace avm {
using avmplus::Atom;
using avmplus::CodeContext;
using avmplus::FunctionObject;
using avmplus::ScriptObject;
using avmplus::Toplevel;
using avmplus::VTable;

/**
 * If the given Object is a Function (or subclass thereof), return true. Otherwise, return false.
 */
bool isFunction(const ScriptObject* o);

/**
 * Given an Object that is a Function or MethodClosure, return the CodeContext
 * that is associated with the function's point of definition.
 * It will not return a useful result for anything other than a Function (or MethodClosure
 * or other subclass of Function): passing such an object to this call will return NULL
 * (and assert in debug builds.)
 */
CodeContext* getFunctionCodeContext(const ScriptObject* o);

/**
 * Given an Object, return the CodeContext that is associated with the object's
 * point of definition. Note that this API is deprecated and should really only be
 * used for legacy code that relies on this; do NOT use this for new code.
 * It will not return a useful result for Functions, MethodClosures, activation
 * objects, or catch objects: passing such an object to this call will return NULL
 * (and assert in debug builds.)
 */
CodeContext* getClassCodeContext(const ScriptObject* o);

/**
 * If f is a non-null Function, invoke it.  Otherwise throw an exception.
 */
Atom callFunction(Toplevel*, Atom f, int argc, Atom* args);

/**
 * f must be a non-null ScriptObject.  invoke the [[Call]] internal method to invoke
 * f as a closure.  Exceptions can  be thrown if f is not a Function or Class, or if the
 * target user code throws.
 */
Atom callFunction(ScriptObject*, int argc, Atom* args);
Atom callFunction(ScriptObject*); // call with no args and receiver=null

} // namespace avm

#endif /* __avm__ */
