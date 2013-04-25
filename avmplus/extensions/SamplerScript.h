/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_SamplerScript__
#define __avmplus_SamplerScript__

#ifdef AVMSHELL_BUILD
#include "shell_toplevel-classes.hh"
#else
#include "platformbuild.h"
#include "avmplayer.h"
#endif

namespace avmplus
{
    struct Sample;

    class TraceClass : public ClassClosure
    {
    public:
        TraceClass(VTable* cvtable);

        int getLevel(int target);
        Atom setLevel(int lvl, int target);
        Atom setListener(FunctionObject* f);
        FunctionObject* getListener();

        DECLARE_SLOTS_TraceClass;
    };

    class SamplerScript
    {
    private:
        static bool trusted(ScriptObject* self) { return self->toplevel()->sampler_trusted(self); }
        explicit SamplerScript(); // unimplemented, not constructable

    public:
        enum { GET = 1, SET = 2 };

        static double getSize(ScriptObject* self, Atom o);
        static Atom getMemberNames(ScriptObject* self, Atom o, bool instanceNames);
        static Atom _getSamples(ScriptObject* self, ClassClosure* cf);
        static void clearSamples(ScriptObject* self);
        static void startSampling(ScriptObject* self);
        static void stopSampling(ScriptObject* self);
        static void pauseSampling(ScriptObject* self);
        static void sampleInternalAllocs(ScriptObject* self, bool b);
        static double getSampleCount(ScriptObject* self);
        static void _setSamplerCallback(ScriptObject* self, ScriptObject* callback);
        static double _getInvocationCount(ScriptObject* self, Atom a, QNameObject* qname, uint32_t type);
        static bool isGetterSetter(ScriptObject* self, Atom a, QNameObject* name);

        static ArrayObject* getLexicalScopes(ScriptObject* self, FunctionObject *function);
        static Atom getSavedThis(ScriptObject* self, FunctionObject *method);

        static Stringp getMasterString(ScriptObject* self, Stringp str);

#ifdef DEBUGGER
    private:
        static ClassClosure* getType(Toplevel* toplevel, SamplerObjectType sot, const void *obj);

        friend class SampleIterator;
        static ScriptObject* makeSample(ScriptObject* self, ClassFactoryClass* cf, const Sample& sample);
        static bool set_stack(ScriptObject* self, ClassFactoryClass* cf, const Sample& sample, SampleObject* sam);
#endif
    };

    class NewObjectSampleObject : public SampleObject
    {
        friend class SamplerScript;
    public:
        NewObjectSampleObject(VTable *vtable, ScriptObject *delegate);
        Atom get_object();
        double get_size();
        void setRef(AvmPlusScriptableObject* o) { obj = o; }
        void setSize(uint64_t s) { size = s; }
    private:
        GCMember<AvmPlusScriptableObject> obj;
        uint64_t size;

        DECLARE_SLOTS_NewObjectSampleObject;
    };

    class NewObjectSampleClass : public SampleClass
    {
    public:
        NewObjectSampleClass(VTable *vtable);

        DECLARE_SLOTS_NewObjectSampleClass;
    };
}
#endif // __avmplus_SamplerScript__
