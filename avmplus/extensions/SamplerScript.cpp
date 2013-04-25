/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


// Adobe patent application tracking #P721, entitled Application Profiling, inventors: T. Reilly

#ifdef AVMSHELL_BUILD
#include "avmshell.h"
#else
// player
#include "avmplayer-classes.h"
#include "SamplerScript.h"
#endif

using namespace MMgc;

namespace avmplus
{
    TraceClass::TraceClass(VTable *cvtable)
        : ClassClosure(cvtable)
    {
        createVanillaPrototype();
    }

    int TraceClass::getLevel(int target)
    {
        int lvl = 0 ; /*Debugger::TRACE_OFF;*/
    #ifdef DEBUGGER
        AvmCore *core = this->core();
        if (core->debugger())
        {
            if (target > 1)
                lvl = core->debugger()->astrace_callback;
            else
                lvl = core->debugger()->astrace_console;
        }
    #endif /* DEBUGGER */
        (void)target;
        return lvl;
    }

    Atom TraceClass::setLevel(int lvl, int target)
    {
    #ifdef DEBUGGER
        AvmCore *core = this->core();
        if (core->debugger())
        {
            if (target > 1)
                core->debugger()->astrace_callback = (Debugger::TraceLevel) lvl;
            else
                core->debugger()->astrace_console = (Debugger::TraceLevel) lvl;
        }
    #endif /* DEBUGGER */
        (void)lvl;
        (void)target;
        return undefinedAtom;
    }

    Atom TraceClass::setListener(FunctionObject* f)
    {
    #ifdef DEBUGGER
        AvmCore *core = this->core();
        if (core->debugger())
        {
            // Listeners MUST be functions or null
            if ( core->isNullOrUndefined(f->atom()) )
            {
                f = 0;
            }
            else if (!AvmCore::istype(f->atom(), core->traits.function_itraits))
            {
                toplevel()->argumentErrorClass()->throwError( kInvalidArgumentError, core->toErrorString("Function"));
                return undefinedAtom;
            }

            //MethodClosure* mc = f->toplevel()->methodClosureClass->create(f->getCallMethodEnv(), f->atom());
            core->debugger()->trace_callback = f;
        }
    #endif /* DEBUGGER */
        (void)f;
        return undefinedAtom;
    }

    FunctionObject* TraceClass::getListener()
    {
        FunctionObject* f = 0;
    #ifdef DEBUGGER
        AvmCore *core = this->core();
        if (core->debugger())
            f = core->debugger()->trace_callback;
    #endif /* DEBUGGER */
        return f;
    }

    using namespace MMgc;

#ifdef DEBUGGER

    class SampleIterator : public ScriptObject
    {
    public:
        SampleIterator(ScriptObject* script, VTable *vt) :
            ScriptObject(vt, NULL),
            script(script)
        {
            Sampler* const sampler = script->core()->get_sampler();
            sampleBufferId = sampler->getSampleBufferId();
            cursor = sampler->getSamples(count);
        }

        int nextNameIndex(int index)
        {
            if(count == 0)
            {
                return 0;
            }

            Sampler* const sampler = script->core()->get_sampler();
            if (sampler == NULL || sampleBufferId != sampler->getSampleBufferId())
            {
                // If the sampler is stopped
                // while we are iterating on items
                // the iterator should be invalidated
                // because the Sampler::sampleBufferId is incremented
                // each time the sample buffer is cleared.
                count = 0;
                return 0;
            }

            return index+1;
        }

        Atom nextValue(int i)
        {
            if (count == 0)
            {
                return undefinedAtom;
            }

            Sampler * const sampler = script->core()->get_sampler();
            if (sampler == NULL || sampleBufferId != sampler->getSampleBufferId())
            {
                count = 0;
                return undefinedAtom;
            }

            (void) i;
            Sample s;
            sampler->readSample(cursor, s);
            count--;
            ScriptObject* sam = SamplerScript::makeSample(script, cf, s);
            if(!sam) {
                count = 0;
                return undefinedAtom;
            }
            return sam->atom();
        }


        Atom nextName(int)
        {
            // they are iterating over the names, bail
            count = 0;
            return undefinedAtom;
        }

    private:
        uint64_t sampleBufferId;
        uint32_t count;
        uint8_t *cursor;
        GCMember<ScriptObject> script;
    public:
        GCMember<ClassFactoryClass> cf;
    };

    class SlotIterator : public ScriptObject
    {
    public:
        SlotIterator(Traits *t, VTable *vtable)
            : ScriptObject(vtable, NULL)
            , currTraits(t ? t->getTraitsBindings() : NULL)
        {
        }

        int nextNameIndex(int index)
        {
            while (currTraits != NULL)
            {
                while ((index = currTraits->next(index)) != 0)
                {
                        return index;
                }

                currTraits = currTraits->base;
            }

            return 0;
        }

        Atom nextValue(int index)
        {
            Multiname mn(currTraits->nsAt(index), currTraits->keyAt(index), true);
            QNameObject *qname = QNameObject::create(gc(), toplevel()->qnameClass(), mn);

            return qname->atom();
        }

        Atom nextName(int)
        {
            // they are iterating over the names, bail
            currTraits = NULL;
            return undefinedAtom;
        }

    private:
        GCMember<const TraitsBindings> currTraits;
    };

#endif // DEBUGGER

#ifdef DEBUGGER
    static VTable* _newVT(Toplevel* toplevel, PoolObject* pool, uint16_t sz)
    {
        Traits* t = Traits::newTraits(pool, NULL, sz, 0, 0, TRAITSTYPE_RT);
        t->verifyBindings(toplevel);
        t->resolveSignatures(toplevel);
        return toplevel->core()->newVTable(t, NULL, toplevel);
    }
#endif

    Atom SamplerScript::_getSamples(ScriptObject* self, ClassClosure* cf)
    {
#ifdef DEBUGGER
        AvmCore* core = self->core();
        Sampler *s = core->get_sampler();
        if (!s || !s->sampling() || s->sampleCount() == 0 || !trusted(self))
            return undefinedAtom;

        if (s->sampleIteratorVTable == NULL)
            s->sampleIteratorVTable = _newVT(self->toplevel(), self->traits()->pool, sizeof(SampleIterator));
        SampleIterator* iter = new (self->gc()) SampleIterator(self, s->sampleIteratorVTable);
        iter->cf = (ClassFactoryClass*)cf;
        return iter->atom();
#else
        (void)self; (void)cf;
        return undefinedAtom;
#endif
    }

    double SamplerScript::getSampleCount(ScriptObject* self)
    {
#ifdef DEBUGGER
        Sampler* s = self->core()->get_sampler();
        if (!s)
            return -1;
        uint32_t num;
        s->getSamples(num);
        return (double)num;
#else
        (void)self;
        return -1;
#endif
    }

#ifdef DEBUGGER
    ClassClosure *SamplerScript::getType(Toplevel* ss_toplevel, SamplerObjectType sot, const void *ptr)
    {
        Toplevel* tl;

        switch (sotGetKind(sot))
        {
            case kSOT_String:
            {
                // toplevel can be null here if there was no CodeContext active
                // when the sample was taken (ie, string was allocated from C++ code).
                // in that case, use the TL from the SamplerScript itself... it isn't
                // technically the right one to use, but is adequate for our purposes here
                // (it will return a stringClass or namespaceClass that will be valid
                // for the sampler)
                tl = sotGetToplevel(sot);
                if (!tl) tl = ss_toplevel;
                return tl->stringClass();
            }
            case kSOT_Namespace:
            {
                tl = sotGetToplevel(sot);
                if (!tl) tl = ss_toplevel;
                return tl->namespaceClass();
            }
            default:
                AvmAssert(0);
            case kSOT_Object:
                break;
        }

        VTable* vt = sotGetVTable(sot);
        tl = vt->toplevel();
        AvmCore* core = tl->core();

        ClassClosure *type;
        ScriptObject* obj = (ScriptObject*)ptr;
        if (obj && AvmCore::istype(obj->atom(), core->traits.class_itraits))
        {
            type = tl->classClass();
        }
        else if (obj && AvmCore::istype(obj->atom(), core->traits.function_itraits))
        {
            type = tl->functionClass();
        }
        else if (obj && obj->traits()->isActivationTraits())
        {
            type = tl->objectClass;
        }
        else
        {
            // fallback answer
            type = tl->objectClass;

            // note that note all types will have an init method,
            // so those types may get reported as "objectClass" rather
            // than something more specific. However, it's not clear
            // that the Sampler ever really cared about reporting those
            // objects well in the first place (eg activation or catch objects),
            // so it doesn't seem we're a lot worse off than before.
            ScopeChain* sc = NULL;
            if (vt->init)
                sc = vt->init->scope();

            if (sc && sc->getSize() <= 1)
            {
                if(sc->getSize() == 1)
                    type = tl->classClass();
            }
            else if (sc)
            {
                Atom ccAtom = sc->getScope(sc->getSize()-1);
                if(AvmCore::isObject(ccAtom))
                {
                    type = (ClassClosure*) AvmCore::atomToScriptObject(ccAtom);
                    if(!AvmCore::istype(type->atom(), core->traits.class_itraits))
                    {
                        // obj is a ClassClosure
                        type = tl->classClass();
                    }
                }
            }
        }
        AvmAssert(AvmCore::istype(type->atom(), core->traits.class_itraits));
        return type;
    }
#endif // DEBUGGER

#ifdef DEBUGGER
    bool SamplerScript::set_stack(ScriptObject* self, ClassFactoryClass* cf, const Sample& sample, SampleObject* sam)
    {
        if (sample.stack.depth > 0)
        {
            Toplevel* toplevel = self->toplevel();
            AvmCore* core = toplevel->core();
            Sampler* s = core->get_sampler();

            StackFrameClass* sfcc = (StackFrameClass*)cf->get_StackFrameClass();
            ArrayObject* stack = toplevel->arrayClass()->newArray(sample.stack.depth);
            StackTrace::Element* e = (StackTrace::Element*)sample.stack.trace;
            for(uint32_t i=0; i < sample.stack.depth; i++, e++)
            {
                StackFrameObject* sf = sfcc->constructObject();

                // at every allocation the sample buffer could overflow and the samples could be deleted
                // the StackTrace::Element pointer is a raw pointer into that buffer so we need to check
                // that its still around before dereferencing e
                uint32_t num;
                if (s->getSamples(num) == NULL)
                    return false;

                sf->setconst_name(e->name()); // NOT e->info()->name() because e->name() can be a fake name
                sf->setconst_file(e->filename());
                sf->setconst_line(e->linenum());
                sf->setconst_scriptID(static_cast<double>(e->functionId()));

                stack->setUintProperty(i, sf->atom());
            }
            sam->setconst_stack(stack);
        }
        return true;
    }

    ScriptObject* SamplerScript::makeSample(ScriptObject* self, ClassFactoryClass* cf, const Sample& sample)
    {
        Toplevel* toplevel = self->toplevel();
        AvmCore* core = toplevel->core();
        Sampler* s = core->get_sampler();
        if (!s)
            return NULL;

        switch (sample.sampleType)
        {
            case Sampler::RAW_SAMPLE:
            {
                SampleClass* cls = (SampleClass*)cf->get_SampleClass();
                SampleObject* sam = cls->constructObject();
                sam->setconst_time(static_cast<double>(sample.micros));
                if (!set_stack(self, cf, sample, sam))
                    return NULL;
                return sam;
            }
            case Sampler::DELETED_OBJECT_SAMPLE:
            {
                DeleteObjectSampleClass* cls = (DeleteObjectSampleClass*)cf->get_DeleteObjectSampleClass();
                DeleteObjectSampleObject* dsam = cls->constructObject();
                dsam->setconst_time(static_cast<double>(sample.micros));
                dsam->setconst_id(static_cast<double>(sample.id));
                dsam->setconst_size(static_cast<double>(sample.size));
                return dsam;
            }
            case Sampler::NEW_OBJECT_SAMPLE:
            {
                NewObjectSampleClass* cls = (NewObjectSampleClass*)cf->get_NewObjectSampleClass();
                NewObjectSampleObject* nsam = cls->constructObject();
                nsam->setconst_time(static_cast<double>(sample.micros));
                nsam->setconst_id(static_cast<double>(sample.id));
                if (!set_stack(self, cf, sample, nsam))
                    return NULL;
                if (sample.ptr != NULL )
                    nsam->setRef((AvmPlusScriptableObject*)sample.ptr);
                nsam->setconst_type(getType(toplevel, sample.sot, sample.ptr));
                nsam->setSize(sample.alloc_size);
                return nsam;
            }
            case Sampler::NEW_AUX_SAMPLE:
            {
                NewObjectSampleClass* cls = (NewObjectSampleClass*)cf->get_NewObjectSampleClass();
                NewObjectSampleObject* nsam = cls->constructObject();
                nsam->setconst_time(static_cast<double>(sample.micros));
                nsam->setconst_id(static_cast<double>(sample.id));
                if (!set_stack(self, cf, sample, nsam))
                    return NULL;
                nsam->setSize(sample.alloc_size);
                return nsam;
            }
        }

        AvmAssert(0);
        return NULL;
    }
#endif // DEBUGGER


    Atom SamplerScript::getMemberNames(ScriptObject* self, Atom o, bool instanceNames)
    {
#ifdef DEBUGGER
        AvmCore* core = self->core();
        MMgc::GC* gc = core->GetGC();
        Sampler* s = core->get_sampler();
        if (!s || !trusted(self))
            return undefinedAtom;

        if (AvmCore::isObject(o))
        {
            Traits *t = AvmCore::atomToScriptObject(o)->traits();
            if(AvmCore::istype(o, CLASS_TYPE) && instanceNames && t->itraits)
                t = t->itraits;
            if (s->slotIteratorVTable == NULL)
                s->slotIteratorVTable = _newVT(self->toplevel(), self->traits()->pool, sizeof(SlotIterator));
            return (new (gc) SlotIterator(t, s->slotIteratorVTable))->atom();
        }
#else
        (void)self;
        (void)o; (void)instanceNames;
#endif
        return undefinedAtom;
    }

    static double _get_size(Atom a)
    {
#ifdef DEBUGGER
        switch(a&7)
        {
        case kDoubleType:
            return 8;
        case kObjectType:
        case kStringType:
        case kNamespaceType:
            AvmPlusScriptableObject *o = (AvmPlusScriptableObject*)(a&~7);
            if(o)
                return (double)o->bytesUsed();
        }
        return 4;
#else
        (void)a;
        return 0;
#endif
    }

    double SamplerScript::getSize(ScriptObject* self, Atom a)
    {
#ifdef DEBUGGER
        AvmCore* core = self->core();
        Sampler* s = core->get_sampler();
        if (!s)
            return 0;
        return _get_size(a);
#else
        (void)self;
        (void)a;
        return 0;
#endif
    }

    void SamplerScript::startSampling(ScriptObject* self)
    {
#ifdef DEBUGGER
        Sampler* s = self->core()->get_sampler();
        if (!s || !trusted(self))
            return;
        s->startSampling();
#else
        (void)self;
#endif
    }

    void SamplerScript::stopSampling(ScriptObject* self)
    {
#ifdef DEBUGGER
        Sampler* s = self->core()->get_sampler();
        if (!s || !trusted(self))
            return;
        s->stopSampling();
#else
        (void)self;
#endif
    }

    void SamplerScript::clearSamples(ScriptObject* self)
    {
#ifdef DEBUGGER
        Sampler* s = self->core()->get_sampler();
        if (!s || !trusted(self))
            return;
        s->clearSamples();
#else
        (void)self;
#endif
    }

    void SamplerScript::pauseSampling(ScriptObject* self)
    {
#ifdef DEBUGGER
        Sampler* s = self->core()->get_sampler();
        if (!s || !trusted(self))
            return;
        s->pauseSampling();
#else
        (void)self;
#endif
    }

    void SamplerScript::sampleInternalAllocs(ScriptObject* self, bool b)
    {
#ifdef DEBUGGER
        Sampler* s = self->core()->get_sampler();
        if (!s || !trusted(self))
            return;
        s->sampleInternalAllocs(b);
#else
        (void)self;
        (void)b;
#endif
    }

    void SamplerScript::_setSamplerCallback(ScriptObject* self, ScriptObject *callback)
    {
#ifdef DEBUGGER
        Sampler* s = self->core()->get_sampler();
        if (!s || !trusted(self))
            return;
        s->setCallback(callback);
#else
        (void)self;
        (void)callback;
#endif
    }

    double SamplerScript::_getInvocationCount(ScriptObject* self, Atom a, QNameObject* qname, uint32_t type)
    {
#ifdef DEBUGGER
        AvmCore* core = self->core();
        Sampler* s = core->get_sampler();
        if (!s || !trusted(self))
            return -1;

        Multiname multiname;
        if(qname)
            qname->getMultiname(multiname);

        ScriptObject* object = self->toplevel()->global();
        if(!AvmCore::isObject(a))
        {
            // not sure if this will be true for standalone avmplus
            AvmAssert(core->codeContext() != NULL);
            DomainEnv *domainEnv = core->codeContext()->domainEnv();
            ScriptEnv* script = (ScriptEnv*) core->domainMgr()->findScriptEnvInDomainEnvByMultiname(domainEnv, multiname);
            if (script != (ScriptEnv*)BIND_NONE)
            {
                if (script == (ScriptEnv*)BIND_AMBIGUOUS)
                    self->toplevel()->throwReferenceError(kAmbiguousBindingError, &multiname);

                object = script->global;
                if (object == NULL)
                {
                    object = script->initGlobal();
                    script->coerceEnter(script->global->atom());
                }
            }
        }
        else
        {
            object = AvmCore::atomToScriptObject(a);

            if(AvmCore::istype(a, CLASS_TYPE) && !qname) {
                // return constructor count
                ClassClosure *cc = (ClassClosure*)object;
                if (cc->vtable->init) // Vector related crash here, Tommy says: I didn't think a type could ever not have a constructor but I guess there's no reason it has to.
                    return (double)cc->vtable->init->invocationCount();
            }
        }

        if(!object || !qname)
            return -1;

        VTable *v = object->vtable;

    again:

        MethodEnv *env = NULL;
        Binding b = self->toplevel()->getBinding(v->traits, &multiname);
        switch (AvmCore::bindingKind(b))
        {
        case BKIND_VAR:
        case BKIND_CONST:
        {
            // only look at slots for first pass, otherwise we're applying instance traits to the Class
            if(v == object->vtable) {
                Atom method = object->getSlotAtom(AvmCore::bindingToSlotId(b));
                if(AvmCore::isObject(method))
                {
                    env = AvmCore::atomToScriptObject(method)->getCallMethodEnv();
                }
            }
            break;
        }
        case BKIND_METHOD:
        {
            int m = AvmCore::bindingToMethodId(b);
            env = v->methods[m];
            break;
        }
        case BKIND_GET:
        case BKIND_GETSET:
        case BKIND_SET:
        {
            if(type == GET && AvmCore::hasGetterBinding(b))
                env = v->methods[AvmCore::bindingToGetterId(b)];
            else if(type == SET && AvmCore::hasSetterBinding(b))
                env = v->methods[AvmCore::bindingToSetterId(b)];
            break;
        }
        case BKIND_NONE:
        {
            Atom method = object->getStringProperty(multiname.getName());
            if(AvmCore::isObject(method))
            {
                env = AvmCore::atomToScriptObject(method)->getCallMethodEnv();
            }
            else if(v->ivtable)
            {
                v = v->ivtable;
                goto again;
            }
        }
        default:
            break;
        }

        if(env)
            return (double)env->invocationCount();
#else
        (void)self;
        (void)type;
        (void)qname;
        (void)a;
#endif

        return -1;
    }

    bool SamplerScript::isGetterSetter(ScriptObject* self, Atom a, QNameObject *qname)
    {
#ifdef DEBUGGER
        AvmCore* core = self->core();
        Sampler* s = core->get_sampler();
        if (!s)
            return false;

        if(!AvmCore::isObject(a) || !AvmCore::atomToScriptObject(a))
            self->toplevel()->throwArgumentError(kInvalidArgumentError, "object");

        ScriptObject *object = AvmCore::atomToScriptObject(a);

        if(!object || !qname)
            self->toplevel()->argumentErrorClass()->throwError(kInvalidArgumentError);

        Multiname multiname;
        qname->getMultiname(multiname);

        VTable *v = object->vtable;

    again:

        Binding b = self->toplevel()->getBinding(v->traits, &multiname);

        if(b == BIND_NONE && v->ivtable)
        {
            v = v->ivtable;
            goto again;
        }

        return AvmCore::hasSetterBinding(b) || AvmCore::hasGetterBinding(b);
#else
        (void)self;
        (void)a; (void)qname;
        return false;
#endif
    }


    NewObjectSampleObject::NewObjectSampleObject(VTable *vtable, ScriptObject *delegate)
        : SampleObject(vtable, delegate), size(0)
    {}

    Atom NewObjectSampleObject::get_object()
    {
        if(obj) {
            Atom a = obj->toAtom();
            AvmAssert((a&~7) != 0);
            return a;
        }
        return undefinedAtom;
    }

    double NewObjectSampleObject::get_size()
    {
        double s = (double)size;
        if( !size ) {
            Atom a = get_object();
            s = _get_size(a);
        }
        return s;
    }

    NewObjectSampleClass::NewObjectSampleClass(VTable *vtable)
        : SampleClass(vtable)
    {
    }

    /*static*/
    ArrayObject* SamplerScript::getLexicalScopes(ScriptObject *self, FunctionObject *function)
    {
#ifdef DEBUGGER
        AvmCore* core = self->core();
        Sampler *s = core->get_sampler();
        if (!s || !trusted(self))
            return NULL;

        if(function != NULL && function->getCallMethodEnv())
            return function->getCallMethodEnv()->getLexicalScopes();
        else
            return NULL;
#else
        (void)self;
        (void)function;
        return NULL;
#endif
    }

    /*static*/
    Atom SamplerScript::getSavedThis(ScriptObject *self, FunctionObject *method)
    {
#ifdef DEBUGGER
        AvmCore* core = self->core();
        Sampler *s = core->get_sampler();
        if (!s || !trusted(self))
            return undefinedAtom;

        MethodClosure* mc;
        if (method != NULL && ((mc = method->toMethodClosure()) != NULL))
            return mc->get_savedThis();
        else
            return undefinedAtom;
#else
        (void)self;
        (void)method;
        return undefinedAtom;
#endif
    }

    /*static*/
    Stringp SamplerScript::getMasterString(ScriptObject *self, Stringp str)
    {
#ifdef DEBUGGER
        AvmCore* core = self->core();
        Sampler *s = core->get_sampler();
        if (!s || !trusted(self))
            return NULL;

        if(str != NULL)
            return str->getMasterString();
        else
            return NULL;
#else
        (void)self;
        (void)str;
        return NULL;
#endif
    }
}

