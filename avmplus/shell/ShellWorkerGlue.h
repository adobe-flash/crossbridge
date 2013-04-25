/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ShellWorkerGlue__
#define __avmplus_ShellWorkerGlue__


namespace avmshell {
    class ShellWorkerClass : public avmplus::ClassClosure
    {
        public:
        ShellWorkerClass(avmplus::VTable* cvtable);
        ShellWorkerObject* getCurrentWorker();
        void registerClassAlias(avmplus::String* name, avmplus::ClassClosure* constr);
        void pr(avmplus::Stringp s);
        
        private:
        DECLARE_SLOTS_ShellWorkerClass;
    };
    
    class ShellWorkerObject : public avmplus::ScriptObject, public avmplus::WorkerObjectBase<ShellWorkerObject>
    {
        friend class ShellWorkerClass;
        
        public:
        ShellWorkerObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype); 
        void clearByteCode();
        avmplus::ByteArrayObject* getByteCode();
        void setByteCode(avmplus::ByteArrayObject* byteCode);

        bool terminate();

		bool get_isPrimordial ()	{ return isPrimordial(); }

        private:
        DECLARE_SLOTS_ShellWorkerObject;
    };
}
#endif /* __avmplus_ShellWorkerGlue__ */

