// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bug 603411 - SECURITY: AvmCore dtor needs to clear CallStackNode chain

%%component mmgc
%%category bugzilla_603411
%%ifdef DEBUGGER

%%prefix

class MyDebugger : public Debugger
{
public:
    MyDebugger(AvmCore *core, avmplus::Debugger::TraceLevel tracelevel) : Debugger(core, tracelevel) {}
    ~MyDebugger() {}

    virtual void enterDebugger() {}
    virtual bool filterException(avmplus::Exception*, bool) { return false; }
    virtual bool hitWatchpoint() {return false; }
};


class MyToplevel: public avmplus::Toplevel 
{
    public:
    MyToplevel(avmplus::AbcEnv* env)
        : Toplevel(env) {}
    virtual GCRef<avmplus::ClassClosure> workerClass() const
    {
        return NULL;
    }
    
    virtual GCRef<avmplus::ClassClosure> workerDomainClass() const
    {
        return NULL;
    }

    virtual GCRef<avmplus::ScriptObject> constructWorkerObject() const
    {
        return NULL;
    }
    
};

class MyAvmCore : public avmplus::AvmCore
{
public:
    MyAvmCore(MMgc::GC* gc) : AvmCore(gc, kApiVersionSeries_FP) { initBuiltinPool(1); }
    ~MyAvmCore() {}

    virtual void interrupt(Toplevel*, InterruptReason) { }
    virtual void stackOverflow(Toplevel*) { }
    virtual avmplus::String* readFileForEval(avmplus::String*, avmplus::String*) { return NULL; }
    virtual avmplus::ApiVersion getDefaultAPI() { return kApiVersion_VM_INTERNAL; }
    virtual avmplus::Toplevel* createTopLevel(avmplus::AbcEnv*)
    {
        return NULL;
    }
    Debugger* createDebugger(int tracelevel) { return new (this->gc) MyDebugger(this, (avmplus::Debugger::_TraceLevel)tracelevel); }

    virtual avmplus::Toplevel* createToplevel(avmplus::AbcEnv* env) { 
        return new (gc) MyToplevel(env);
    }

};


class MyTestClass
{
public:
    MyTestClass(MyAvmCore* core) : m_core(core)
    {
        (void)core;
    }

    void testAvmCoreDelete()
    {
        // create CallStackNode
        CallStackNode csn(m_core, "Date");

        // delete core
        delete m_core;
    }
private:
    MyAvmCore* m_core;
};

%%decls
private:
    MyTestClass* testClass;
    MyAvmCore*   testCore;
%%prologue

%%epilogue
    testClass = NULL;
    testCore = NULL;

%%test test1
    // create our own core
    testCore = new MyAvmCore(core->gc);
    testClass = new MyTestClass(testCore);
    testClass->testAvmCoreDelete();

    %%verify true
