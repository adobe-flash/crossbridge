/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ShellWorkerDomainGlue__
#define __avmplus_ShellWorkerDomainGlue__


namespace avmshell {
    class ShellWorkerDomainClass : public avmplus::ClassClosure
    {
        public:
        ShellWorkerDomainClass(avmplus::VTable* cvtable);
        
        private:
        DECLARE_SLOTS_ShellWorkerDomainClass;
    };

    class ShellWorkerDomainObject : public avmplus::ScriptObject, public avmplus::WorkerDomainObjectBase<ShellWorkerDomainObject>
    {
        friend class ShellWorkerDomainClass;
        
        public:
        ShellWorkerDomainObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype); 
        virtual ShellWorkerObject* createWorkerFromByteArrayInternal(avmplus::ByteArrayObject* byteArray);

        avmplus::ObjectVectorObject* listWorkers();

        private:
        DECLARE_SLOTS_ShellWorkerDomainObject;
    };
}
#endif /* __avmplus_ShellWorkerDomainGlue__ */

