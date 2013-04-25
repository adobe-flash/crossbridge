/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    DomainEnv::DomainEnv(AvmCore* core, Domain* domain, DomainEnv* base, uint32_t baseCount)
        : m_scriptEnvMap(ScriptEnvMap::create(core->GetGC()))
        , m_domain(domain)
        , m_globalMemoryScratch(mmfx_new0(Scratch))
        , m_globalMemoryBase(m_globalMemoryScratch->scratch)
        , m_globalMemorySize(GLOBAL_MEMORY_MIN_SIZE)
        , m_baseCount(baseCount)
    {
        WB(core->GetGC(), this, &this->m_bases[0], this);
        for (uint32_t i = 1; i < baseCount; ++i)
        {
            WB(core->GetGC(), this, &this->m_bases[i], base->m_bases[i-1]);
        }

    #ifdef _DEBUG
        GCList<Domain> dl(core->GetGC(), 0);
        for (Domain* di = m_domain; di != NULL; di = di->base())
            dl.add(di);

        GCList<DomainEnv> el(core->GetGC(), 0);
        for (DomainEnv* di = this; di != NULL; di = di->base())
            el.add(di);

        AvmAssert(dl.length() == el.length());
        for (uint32_t i = 0, n = el.length(); i < n; ++i)
        {
            Domain* d = dl[i];
            DomainEnv* e = el[i];
            AvmAssert(e->domain() == d);
        }
    #endif
    }

    DomainEnv* DomainEnv::newDomainEnv(AvmCore* core, Domain* domain, DomainEnv* base)
    {
        uint32_t baseCount = (base ? base->m_baseCount : 0) + 1;
        // Note that we deliberately overallocate by one here (the proper
        // amount is baseCount-1) so that we always have one, zeroed-out
        // entry at the end. This allows us to always use "m_bases[1]"
        // to get our immediate base, even if our base is NULL, thus
        // avoiding a check in the implementation of base().
        uint32_t extra = baseCount * sizeof(DomainEnv*);
        DomainEnv* domainEnv = new (core->GetGC(), MMgc::kExact, extra) DomainEnv(core, domain, base, baseCount);
        core->m_domainEnvs.add(domainEnv);
        return domainEnv;
    }

    DomainEnv::~DomainEnv()
    {
        mmfx_delete(m_globalMemoryScratch);
    }

    Toplevel* DomainEnv::toplevel() const
    {
        if(m_toplevel) return m_toplevel;
        if(base()) return base()->toplevel();
        AvmAssert(0);
        return NULL;
    }

    bool DomainEnv::set_globalMemory(ByteArrayObject* providerObject)
    {
        if (!providerObject)
        {
            // null obj -- use scratch
            if (m_globalMemoryProviderObject) // unsubscribe from current if any
                globalMemoryUnsubscribe(m_globalMemoryProviderObject);
            // remember NULL obj
            m_globalMemoryProviderObject = NULL;
            // point at scratch mem
            notifyGlobalMemoryChanged(m_globalMemoryScratch->scratch, GLOBAL_MEMORY_MIN_SIZE);
        }
        else if (!globalMemorySubscribe(providerObject))
        {
            // failed... do nothing
            return false;
        }
        else
        {
            // success on globalMemorySubscribe would have led to notifyGlobalMemoryChanged
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=723401
            // success... unsubscribe from original, but only if not the same as new provider
            if (m_globalMemoryProviderObject && m_globalMemoryProviderObject != providerObject)
#else
           // success... unsubscribe from original
           if (m_globalMemoryProviderObject)
#endif
                globalMemoryUnsubscribe(m_globalMemoryProviderObject);
            // remember the new one
            m_globalMemoryProviderObject = providerObject;
        }
        return true;
    }

    // memory changed so go through and update all reference to both the base
    // and the size of the global memory
    void DomainEnv::notifyGlobalMemoryChanged(uint8_t* newBase, uint32_t newSize)
    {
        AvmAssert(newBase != NULL); // real base address
        AvmAssert(newSize >= GLOBAL_MEMORY_MIN_SIZE); // big enough

        m_globalMemoryBase = newBase;

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=723401
        // We require that MOps addresses be non-negative int32 values.
        // Furthermore, the bounds checks may fail to detect an out-of-bounds
        // access if the global memory size is not itself a non-negative int32.
        // See bug 723401.  Since the size of a byte array may expand as
        // side-effect of other operations according to a policy not under direct
        // control of the user, it is not appropriate to signal an error here.
        // We simply silently ignore any excess allocation.
        // TODO: While we cannot do much about automatic resizing, an attempt
        // to explicitly set the size too large should throw an exception before
        // arriving here.

        m_globalMemorySize = (newSize > 0x7fffffff) ? 0x7fffffff : newSize;
#else
        m_globalMemorySize = newSize;
#endif
		TELEMETRY_UINT32(toplevel()->core()->getTelemetry(), ".mem.bytearray.alchemy",m_globalMemorySize/1024);
    }

    bool DomainEnv::globalMemorySubscribe(ByteArrayObject* providerObject)
    {
        return providerObject->GetByteArray().addSubscriber(this);
    }

    bool DomainEnv::globalMemoryUnsubscribe(ByteArrayObject* providerObject)
    {
        return providerObject->GetByteArray().removeSubscriber(this);
    }
}


