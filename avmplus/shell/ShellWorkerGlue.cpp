/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmshell.h"
namespace avmshell {
    using namespace avmplus;
    ShellWorkerObject::ShellWorkerObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype)
        : avmplus::ScriptObject(vtable, prototype)
    {
    }
    
    void ShellWorkerObject::clearByteCode()
    {
        set_m_byteCode(NULL);
    }

    void ShellWorkerObject::setByteCode(avmplus::ByteArrayObject* code)
    {
        set_m_byteCode(code);
    }
    
    avmplus::ByteArrayObject* ShellWorkerObject::getByteCode()
    {
        return get_m_byteCode();
    }

    bool ShellWorkerObject::terminate()
    {
        return internalStop();
    }

    ShellWorkerClass::ShellWorkerClass(avmplus::VTable *cvtable)
        : avmplus::ClassClosure(cvtable)
    {
        AvmAssert(avmplus::AvmCore::getActiveCore() == cvtable->core());
        createVanillaPrototype();
        
        avmplus::Isolate* ent = core()->getIsolate();
        // By convention nobody will mutate the isolate table entry
        // except the current thread, as long as the entry has a nonnegative giid
        // so no need for locking.
        
        ShellWorkerObject* current = (ShellWorkerObject*)constructObject();
        set_m_current(current);
        current->setIsolate(ent);
    }
    
    void ShellWorkerClass::pr(avmplus::Stringp s)
    {
        printf("%s\n", avmplus::StUTF8String(core()->internString(s)).c_str());
        fflush(stdout);
    }
    
    ShellWorkerObject* ShellWorkerClass::getCurrentWorker()
    {
        // Exposing accessor as public.
        return get_m_current();
    }
    
}
