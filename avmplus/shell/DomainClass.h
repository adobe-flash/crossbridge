/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_DomainClass__
#define __avmshell_DomainClass__

#ifndef AVMSHELL_BUILD
#error "This file is only for use with avmshell"
#endif

namespace avmplus
{
    class GC_AS3_EXACT(DomainObject, ScriptObject)
    {
        friend class DomainClass;
    protected:
        DomainObject(VTable *vtable, ScriptObject *delegate);
    public:

        void init(DomainObject *base);
        Atom loadBytes(ByteArrayObject* bytes, uint32_t swfVersion);
        ClassClosure* getClass(Stringp name);
        // AS3 declaration requires these are ByteArrayObject
        ByteArrayObject* get_domainMemory() const;
        void set_domainMemory(ByteArrayObject* mem);

    private:
        ScriptObject* finddef(const Multiname& multiname, DomainEnv* domainEnv);

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(DomainObject)

    public:
        GCMember<DomainEnv> GC_POINTER(domainEnv);
        GCMember<Toplevel>  GC_POINTER(domainToplevel);
        
        GC_DATA_END(DomainObject)

    private:
        DECLARE_SLOTS_DomainObject;
    // ------------------------ DATA SECTION END
    };

    class GC_AS3_EXACT(DomainClass, ClassClosure)
    {
    protected:
        DomainClass(VTable* cvtable);
        
    public:
        DomainObject* get_currentDomain();
        int get_MIN_DOMAIN_MEMORY_LENGTH();

    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(DomainClass)

    private:
        DECLARE_SLOTS_DomainClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmshell_DomainClass__ */
