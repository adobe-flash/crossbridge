/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_CdeclThunk__
#define __avmplus_CdeclThunk__

namespace avmplus
{
    // used to call a MethodEnv with a C cdecl implementation

    // TODO for x86-64, we probably want to split this into real 32-bit integer returning variant
    Atom coerce32CdeclArgDescEnter(Traits* retTraits, uintptr_t argDesc, MethodEnv* env, va_list ap);
    Atom coerce32CdeclArgDescEnter(Traits* retTraits, char* argDesc, MethodEnv* env, va_list ap);
    Atom coerce32CdeclArgDescEnter(Traits* retTraits, MethodEnv* env, int argc, Atom* argv);
    Atom coerce32CdeclArgDescEnter(Traits* retTraits, MethodEnv* env, int argc, uint32_t* ap);
    double coerceNCdeclArgDescEnter(uintptr_t argDesc, MethodEnv* env, va_list ap);
    double coerceNCdeclArgDescEnter(char* argDesc, MethodEnv* env, va_list ap);
    double coerceNCdeclArgDescEnter(MethodEnv* env, int argc, Atom* argv);
    double coerceNCdeclArgDescEnter(MethodEnv* env, int argc, uint32_t* ap);

    // used to convert C parameters to impl32 style (argc, ap)

    // calculate size needed for ap style argument block
    int32_t argDescApSize(uintptr_t argDesc, MethodEnv* env);
    int32_t argDescApSize(char* argDesc, MethodEnv* env);
    // convert arguments to ap style argument block, returning "argc"
    int32_t argDescArgsToAp(void* args, uintptr_t argDesc, MethodEnv* env, va_list ap);
    int32_t argDescArgsToAp(void* args, char* argDesc, MethodEnv* env, va_list ap);

    // used to convert C parameters to Atom* style (argc, atomv)

    // return number of arguments in description
    int32_t argDescArgCount(uintptr_t argDesc);
    int32_t argDescArgCount(char* argDesc);
    // convert arguments to Atoms
    void argDescArgsToAtomv(Atom* args, uintptr_t argDesc, MethodEnv* env, va_list ap);
    void argDescArgsToAtomv(Atom* args, char* argDesc, MethodEnv* env, va_list ap);
    // convert arguments to AtomList
    void argDescArgsToAtomList(AtomList& args, uintptr_t argDesc, MethodEnv* env, va_list ap);
    void argDescArgsToAtomList(AtomList& args, char* argDesc, MethodEnv* env, va_list ap);

#ifdef VMCFG_AOT
    uintptr_t aotThunker(MethodEnv* env, int32_t argc, uint32_t* argv);
    double aotThunkerN(MethodEnv* env, int32_t argc, uint32_t* argv);
#endif
}

#endif // __avmplus_CdeclThunk__

