/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmshell.h"
namespace avmshell {
    ShellWorkerDomainObject::ShellWorkerDomainObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype)
        : avmplus::ScriptObject(vtable, prototype)
    {
    }
    
    ShellWorkerObject* ShellWorkerDomainObject::createWorkerFromByteArrayInternal(avmplus::ByteArrayObject* byteArray)
    {
        // Note that the cast to ShellWorkerClass is needed because constructObject is not virtual!


        ShellWorkerObject* shellWorkerObject = toplevel()->workerClass().staticCast<ShellWorkerClass>()->constructObject().staticCast<ShellWorkerObject>();
        shellWorkerObject->setByteCode(byteArray);
        shellWorkerObject->initialize();
        return shellWorkerObject;
    }
    

    avmplus::ObjectVectorObject* ShellWorkerDomainObject::listWorkers()
    {
        return avmplus::WorkerDomainObjectBase<ShellWorkerDomainObject>::listWorkers();
    }


    ShellWorkerDomainClass::ShellWorkerDomainClass(avmplus::VTable *cvtable)
        : avmplus::ClassClosure(cvtable)
    {
        AvmAssert(avmplus::AvmCore::getActiveCore() == cvtable->core());
        createVanillaPrototype();
                
        ShellWorkerDomainObject* current = (ShellWorkerDomainObject*)newInstance();
        set_m_current(current);
    }        
}
