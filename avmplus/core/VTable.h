/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_VTable__
#define __avmplus_VTable__

namespace avmplus
{
    typedef ScriptObject* (FASTCALL *CreateInstanceProc)(ClassClosure* cls);

    class GC_CPP_EXACT(VTable, MMgc::GCTraceableObject)
    {
        friend class CodegenLIR;
        friend class ImtThunkEnv;
        friend class BaseExecMgr;
        friend class halfmoon::JitFriend;

    private:
        MethodEnv* makeMethodEnv(MethodInfo* method, ScopeChain* scope);

    public:
        VTable(Traits* traits, VTable* base, Toplevel* toplevel);
        void resolveSignatures(ScopeChain* scope);

        VTable* newParameterizedVTable(Traits* param_traits, Stringp fullname);

        size_t getExtraSize() const;
        MMgc::GC* gc() const;
        AvmCore* core() const;
        Toplevel* toplevel() const;

#ifdef AVMPLUS_VERBOSE
        PrintWriter& print(PrintWriter& prw) const;
#endif

#ifdef DEBUGGER
        /**
         * Basically the same as AvmPlusScriptableObject::bytesUsed().
         */
        uint64_t bytesUsed() const;
#endif

    // ------------------------ DATA SECTION BEGIN
    GC_DATA_BEGIN(VTable)

    private:
        Toplevel* const GC_POINTER(_toplevel);

    public:
        GCMember<MethodEnv> GC_POINTER(init);
        GCMember<VTable>    GC_POINTER(base);
        GCMember<VTable>    GC_POINTER(ivtable);
        Traits* const       GC_POINTER(traits);
        CreateInstanceProc  createInstanceProc;
        bool linked;    // @todo -- surely there's a spare bit we can use for this.
        bool pad[2];

#ifdef VMCFG_NANOJIT
    private:
        ImtHolder       GC_STRUCTURE(imt);
#endif

#ifdef VMCFG_AOT
    public:
        int (*interfaceBindingFunction)(int);
#endif

    public:
        // Virtual method table.
        //
        // The tracer needs the table size.  It's probably wrong to
        // try to get the number of methods from the traits, because
        // it looks like that could cause thrashing in the traits
        // cache.  It is also not ideal to use the object's allocated
        // size, but it /is/ reasonably correct to do so because the
        // object is zeroed on allocation, and that saves one word of
        // storage.  However, there's some space in VTable and we
        // could keep a 16-bit count, probably, if we're really worried.
        MethodEnv*      GC_POINTERS(methods[1], "(MMgc::GC::Size(this) - offsetof(VTable, methods)) / sizeof(MethodEnv*)");

    GC_DATA_END(VTable)
    // ------------------------ DATA SECTION END
    };
}

#endif // __avmplus_VTable__

