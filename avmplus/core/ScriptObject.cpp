/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
//#define DOPROF
//#include "../vprof/vprof.h"

namespace avmplus
{
    ScriptObject::ScriptObject(VTable* _vtable, ScriptObject* _delegate) :
#ifdef DEBUGGER
        AvmPlusScriptableObject(sotObject(_vtable)),
#endif // DEBUGGER
        vtable(_vtable),
        // note that it's substantially more efficient to initialize this in the ctor
        // list vs. a later explicit call to setDelegate, as we don't have to check for marking
        // nor decrement an existing value...
        delegate(_delegate)
    {
        AvmAssert(vtable->traits->isResolved());

        // Ensure that our object is large enough to hold its extra traits data.
        AvmAssert(MMgc::GC::Size(this) >= vtable->traits->getTotalSize());
    }

    ScriptObject::ScriptObject(VTable* _vtable, ScriptObject* _delegate, int capacity) :
#ifdef DEBUGGER
        AvmPlusScriptableObject(sotObject(_vtable)),
#endif // DEBUGGER
        vtable(_vtable),
        // note that it's substantially more efficient to initialize this in the ctor
        // list vs. a later explicit call to setDelegate, as we don't have to check for marking
        // nor decrement an existing value...
        delegate(_delegate)
    {
        AvmAssert(vtable->traits->isResolved());

        // Ensure that our object is large enough to hold its extra traits data.
        AvmAssert(MMgc::GC::Size(this) >= vtable->traits->getTotalSize());

        //if capacity not specified then initialize the hashtable lazily
        if (vtable->traits->needsHashtable() && capacity)
        {
            initHashtable(capacity);
        }
    }

    // Not actually entirely trivial, as it decrements reference counts
    // manually, not just by GCMember smart pointer destructors.
#ifdef DRC_TRIVIAL_DESTRUCTOR
    ScriptObject::~ScriptObject()
    {
        //setDelegate(NULL); -- no longer necessary
        vtable->traits->destroyInstance(this);
    }
#endif

    /* The storage for a ScriptObject or a subclass SO of ScriptObject is
     * laid out as follows.
     *
     *   - first the bits of the C++ class; if there are pointers here
     *     then they must be traced explicitly by the appropriate class's
     *     gcTrace method
     *   - then the bits for the ActionScript slots
     *   - optionally an InlineHashtable for dynamic properties
     *
     * The in-line slots are native and their representation is described
     * by the Traits object (vtable->traits).  They are not named, but named
     * lookup is possible by going to the Traits object, which contains
     * a name->slot map.
     *
     * The InlineHashtable always stores Atoms.
     */
    bool ScriptObject::gcTrace(MMgc::GC* gc, size_t cursor)
    {
        (void)cursor;
        gc->TraceLocation(&vtable);
        gc->TraceLocation(&delegate);
        traits()->traceSlots(gc, this);
        if (traits()->needsHashtable())
        {
            // Avoid initializing the hash table here
            InlineHashtable* iht = getTableNoInit();
            // The iht pointer can be NULL for Dictionary objects that
            // haven't had init called yet.
            if(iht)
                iht->gcTrace(gc);
        }

        return false;
    }

    void ScriptObject::initHashtable(int capacity /*=InlineHashtable::kDefaultCapacity*/)
    {
        AvmAssert(vtable->traits->isDictionary() == 0); //should not be called DictionaryObject uses HeapHashtable

        MMGC_MEM_TYPE(this);
        union {
            uint8_t* p;
            InlineHashtable* iht;
        };
        p = (uint8_t*)this + vtable->traits->getHashtableOffset();
        iht->initializeWithDontEnumSupport(this->gc(), capacity);
    }

    InlineHashtable* ScriptObject::getTable() const
    {
        AvmAssert(vtable->traits->getHashtableOffset() != 0);
        union {
            uint8_t* p;
            InlineHashtable* iht;
            HeapHashtable** hht;
        };
        p = (uint8_t*)this + vtable->traits->getHashtableOffset();
        if(!vtable->traits->isDictionary())
        {
            if (iht->needsInitialize())
                const_cast<ScriptObject*>(this)->initHashtable();
            return iht;
        }
        else
        {
            //DictionaryObjects store pointer to HeapHashtable at
            //the hashtable offset
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=666720
            if (*hht == NULL)
            {
                // ugh... if we are here, it means that a Dictionary subclass didn't
                // call the Dictionary ctor via super(), so our HeapHashtable was never created.
                // This is a horrible band-aid: lazily create it here. (Since the
                // ctor was never called, we'll just assume weakKeys = false.) This is
                // a horrible encapsulation violation, but a proper fix requires a fairly
                // sweeping rethinking of ScriptObject, so this will have to do short-term.
                MMgc::GC* gc = this->gc();
                // if we are lazy-initing here, it means our ctor was never called:
                // so, just assume weakKeys = false.
                HeapHashtable* ht = HeapHashtable::create(gc);
                WB(gc, this, hht, ht);
            }
#endif
            InlineHashtable *ihp = (*hht)->get_ht();
            AvmAssertMsg(ihp != NULL, "Illegal to call getTable before Dictionary init is called");
            return ihp;
        }
    }

    bool ScriptObject::isOwnAtomPropertyHere(Atom name, Atom *recv) const
    {
        AvmAssert(this->vtable->traits->getHashtableOffset() != 0);
        Atom const value = this->getTable()->getNonEmpty(name);
        if (!InlineHashtable::isEmpty(value))
        {
            *recv = value;
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * traverse the delegate chain looking for a value.
     * [ed] it's okay to look only at the HT's in the delegate chain because
     * delegate values may only be instances of Object.  They cannot be objects
     * with slots.  We don't need to look at traits at each step.
     * todo - enforce this rule
     * @param name
     * @return
     */
    Atom ScriptObject::getAtomProperty(Atom name) const
    {
        if (!traits()->needsHashtable())
        {
            return getAtomPropertyFromProtoChain(name, delegate, traits());
        }
        else
        {
            Stringp s = core()->atomToString(name);
            AvmAssert(s->isInterned());
            Atom ival = s->getIntAtom();
            if (ival)
            {
                name = ival;
            }

            // dynamic lookup on this object
            const ScriptObject *o = this;
            do
            {
                // ensure prototype is dynamic
                if (!o->vtable->traits->getHashtableOffset())
                    continue;

                Atom value;
                if (o->isOwnAtomPropertyHere(name, &value))
                    return value;
            }
            while ((o = o->delegate) != NULL);
            return undefinedAtom;
        }
    }

    Atom ScriptObject::getAtomPropertyFromProtoChain(Atom name, ScriptObject* o, Traits *origObjTraits) const
    {
        // todo will delegate always be non-null here?
        if (o != NULL)
        {
            Atom searchname = name;
            Stringp s = core()->atomToString(name);
            AvmAssert(s->isInterned());
            Atom ival = s->getIntAtom();
            if (ival)
            {
                searchname = ival;
            }
            do
            {
                // ensure prototype is dynamic
                if (!o->vtable->traits->getHashtableOffset())
                    continue;
                Atom value;
                if (o->isOwnAtomPropertyHere(searchname, &value))
                    return value;
            }
            while ((o = o->delegate) != NULL);
        }
        // NOTE use default public since name is not used
        Multiname multiname(core()->getAnyPublicNamespace(), AvmCore::atomToString(name));
        toplevel()->throwReferenceError(kReadSealedError, &multiname, origObjTraits);
        // unreached
        return undefinedAtom;
    }

    bool ScriptObject::hasMultinameProperty(const Multiname* multiname) const
    {
        if (traits()->needsHashtable() && multiname->isValidDynamicName())
            {
                return hasAtomProperty(multiname->getName()->atom());
            }
            else
            {
            // ISSUE should this walk the proto chain?
            return false;
        }
    }

    bool ScriptObject::hasAtomProperty(Atom name) const
    {
        if (traits()->needsHashtable())
        {
            Stringp s = core()->atomToString(name);
            AvmAssert(s->isInterned());
            Atom ival = s->getIntAtom();
            if (ival)
            {
                name = ival;
            }

            return getTable()->contains(name);
        }
        else
        {
            // ISSUE should this walk the proto chain?
            return false;
        }
    }

    void ScriptObject::throwWriteSealedError(const Multiname& name)
    {
        toplevel()->throwReferenceError(kWriteSealedError, name, traits());
    }

    void ScriptObject::throwWriteSealedError(Atom name)
    {
        AvmCore* core = this->core();
        throwWriteSealedError(Multiname(core->getAnyPublicNamespace(), core->intern(name)));
    }

    void ScriptObject::throwCantInstantiateError()
    {
        Multiname qname(traits()->ns(), traits()->name());
        toplevel()->argumentErrorClass()->throwError(kCantInstantiateError, core()->toErrorString(&qname));
    }

    void ScriptObject::setAtomProperty(Atom name, Atom value)
    {
        if (traits()->needsHashtable())
        {
            Stringp s = core()->atomToString(name);
            AvmAssert(s->isInterned());
            Atom ival = s->getIntAtom();
            if (ival)
            {
                name = ival;
            }

            MMGC_MEM_TYPE(this);
            getTable()->add (name, value);
            MMGC_MEM_TYPE(NULL);
        }
        else
        {
            throwWriteSealedError(name);
        }
    }

    void ScriptObject::setMultinameProperty(const Multiname* name, Atom value)
    {
        if (traits()->needsHashtable() && name->isValidDynamicName())
        {
            setStringProperty(name->getName(), value);
        }
        else
        {
            throwWriteSealedError(*name);
        }
    }

    bool ScriptObject::getAtomPropertyIsEnumerable(Atom name) const
    {
        if (traits()->needsHashtable())
        {
            Stringp s = core()->atomToString(name);
            AvmAssert(s->isInterned());
            Atom ival = s->getIntAtom();
            if (ival)
            {
                name = ival;
            }

            return getTable()->getAtomPropertyIsEnumerable(name);
        }
        else
        {
            // ISSUE should this walk the proto chain?
            return false;
        }
    }

    void ScriptObject::setAtomPropertyIsEnumerable(Atom name, bool enumerable)
    {
        if (traits()->needsHashtable())
        {
            Stringp s = core()->atomToString(name);
            AvmAssert(s->isInterned());
            Atom ival = s->getIntAtom();
            if (ival)
            {
                name = ival;
            }

            getTable()->setAtomPropertyIsEnumerable(name, enumerable);
        }
        else
        {
            throwWriteSealedError(name);
        }
    }

    bool ScriptObject::deleteAtomProperty(Atom name)
    {
        if (traits()->needsHashtable())
        {
            Stringp s = core()->atomToString(name);
            AvmAssert(s->isInterned());
            Atom ival = s->getIntAtom();
            if (ival)
            {
                name = ival;
            }

            getTable()->remove(name);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ScriptObject::deleteMultinameProperty(const Multiname* name)
    {
        if (traits()->needsHashtable() && name->isValidDynamicName())
        {
            return deleteStringProperty(name->getName());
        }
        else
        {
            return false;
        }
    }

    Atom ScriptObject::getUintProperty(uint32_t i) const
    {
        // N.B.: a key present in ScriptObject must be interned string;
        // thus uninterned implies absent (cf. bugzilla 556023).

        AvmCore* core = this->core();

        if (!(i&MAX_INTEGER_MASK))
        {
            if (!traits()->needsHashtable())
            {
                Stringp interned;
                bool present = core->isInternedUint(i, &interned);
                if (present)
                {
                    Atom name = interned->atom();
                    return getAtomPropertyFromProtoChain(name, delegate,
                                                         traits());
                }
                else
                {
                    return undefinedAtom;
                }
            }
            else
            {
                // dynamic lookup on this object
                Atom name = core->uintToAtom (i);
                const ScriptObject *o = this;
                do
                {
                    // ensure prototype is dynamic
                    if (!o->vtable->traits->getHashtableOffset())
                        continue;

                    Atom value;
                    if (o->isOwnAtomPropertyHere(name, &value))
                        return value;
                }
                while ((o = o->delegate) != NULL);
                return undefinedAtom;
            }
        }
        else
        {
            Stringp interned;
            bool present;
            present = core->isInternedUint(i, &interned);
            if (present)
            {
                return getAtomProperty(interned->atom());
            }
            else
            {
                return undefinedAtom;
            }
        }
    }

    void ScriptObject::setUintProperty(uint32_t i, Atom value)
    {
        AvmCore* core = this->core();
        if (!(i&MAX_INTEGER_MASK))
        {
            Atom name = core->uintToAtom (i);
            if (traits()->needsHashtable())
            {
                MMGC_MEM_TYPE(this);
                getTable()->add(name, value);
                MMGC_MEM_TYPE(NULL);
            }
            else
            {
                throwWriteSealedError(core->internUint32(i)->atom());
            }
        }
        else
        {
            setAtomProperty(core->internUint32(i)->atom(), value);
        }
    }

    bool ScriptObject::delUintProperty(uint32_t i)
    {
        AvmCore* core = this->core();
        if (!(i&MAX_INTEGER_MASK))
        {
            Atom name = core->uintToAtom (i);
            if (traits()->needsHashtable())
            {
                getTable()->remove(name);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return deleteAtomProperty(core->internUint32(i)->atom());
        }
    }

    bool ScriptObject::hasUintProperty(uint32_t i) const
    {
        AvmCore* core = this->core();
        if (!(i&MAX_INTEGER_MASK))
        {
            Atom name = core->uintToAtom (i);
            if (traits()->needsHashtable())
            {
                return getTable()->contains(name);
            }
            else
            {
                // ISSUE should this walk the proto chain?
                return false;
            }
        }
        else
        {
            return hasAtomProperty(core->internUint32(i)->atom());
        }
    }

    Atom ScriptObject::getMultinameProperty(const Multiname* multiname) const
    {
        if (multiname->isValidDynamicName())
        {
            return getStringProperty(multiname->getName());
        }
        else
        {
            Toplevel* toplevel = this->toplevel();

            if (multiname->isNsset())
                toplevel->throwReferenceError(kReadSealedErrorNs, multiname, traits());
            else
                toplevel->throwReferenceError(kReadSealedError, multiname, traits());
            return undefinedAtom;
        }
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom ScriptObject::callProperty(const Multiname* multiname, int argc, Atom* argv)
    {
        Atom method = getMultinameProperty(multiname);
        if (!AvmCore::isObject(method))
            toplevel()->throwTypeError(kCallOfNonFunctionError, core()->toErrorString(multiname));
        argv[0] = atom(); // replace receiver
        return avmplus::op_call(toplevel(), method, argc, argv);
    }

    Atom ScriptObject::getDescendants(const Multiname* /*name*/) const
    {
        toplevel()->throwTypeError(kDescendentsError, core()->toErrorString(traits()));
        return undefinedAtom;// not reached
    }

    bool ScriptObject::isGlobalObject() const
    {
        return traits()->posType() == TRAITSTYPE_SCRIPT;
    }

#ifdef AVMPLUS_VERBOSE
    PrintWriter& ScriptObject::print(PrintWriter& prw) const
    {
        (traits()->name() != NULL) ? prw << traits()
                                   : prw << "{}";
        return prw << "@" << asAtomHex(atom());
    }
#endif

    Atom ScriptObject::defaultValue()
    {
        AvmCore *core = this->core();
        Toplevel* toplevel = this->toplevel();

        Atom atomv_out[1];

        // call this.valueOf()
        // NOTE use callers versioned public to get correct valueOf
        Multiname tempname(core->findPublicNamespace(), core->kvalueOf);
        atomv_out[0] = atom();
        Atom result = toplevel->callproperty(atom(), &tempname, 0, atomv_out, vtable);

        // if result is primitive, return it
        if (atomKind(result) != kObjectType)
            return result;

        // otherwise call this.toString()
        tempname.setName(core->ktoString);
        atomv_out[0] = atom();
        result = toplevel->callproperty(atom(), &tempname, 0, atomv_out, vtable);

        // if result is primitive, return it
        if (atomKind(result) != kObjectType)
            return result;

        // could not convert to primitive.
        toplevel->throwTypeError(kConvertToPrimitiveError, core->toErrorString(traits()));
        return undefinedAtom;
    }

    // Execute the ToString algorithm as described in ECMA-262 Section 9.8.
    // This is ToString(ToPrimitive(input argument, hint String))
    // ToPrimitive(input argument, hint String) calls [[DefaultValue]]
    // described in ECMA-262 8.6.2.6.  The [[DefaultValue]] algorithm
    // with hint String is inlined here.
    Stringp ScriptObject::toString()
    {
        AvmCore *core = this->core();
        Toplevel* toplevel = this->toplevel();

        Atom atomv_out[1];

        // call this.toString()
        // NOTE use callers versioned public to get correct toString
        Multiname tempname(core->findPublicNamespace(), core->ktoString);
        atomv_out[0] = atom();
        Atom result = toplevel->callproperty(atom(), &tempname, 0, atomv_out, vtable);

        // if result is primitive, return its ToString
        if (atomKind(result) != kObjectType)
            return core->string(result);

        // otherwise call this.valueOf()
        tempname.setName(core->kvalueOf);
        atomv_out[0] = atom();
        result = toplevel->callproperty(atom(), &tempname, 0, atomv_out, vtable);

        // if result is primitive, return it
        if (atomKind(result) != kObjectType)
            return core->string(result);

        // could not convert to primitive.
        toplevel->throwTypeError(kConvertToPrimitiveError, core->toErrorString(traits()));
        return NULL; // unreachable
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom ScriptObject::call(int /*argc*/, Atom* /*argv*/)
    {
        // TypeError in ECMA to execute a non-function
        // NOTE use default public since name is not used
        Multiname name(core()->getAnyPublicNamespace(), core()->kvalue);
        toplevel()->throwTypeError(kCallOfNonFunctionError, core()->toErrorString(&name));
        return undefinedAtom;
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom ScriptObject::construct(int /*argc*/, Atom* /*argv*/)
    {
        // TypeError in ECMA to execute a non-function
        toplevel()->throwTypeError(kConstructOfNonFunctionError);
        return undefinedAtom;
    }

    Atom ScriptObject::applyTypeArgs(int /*argc*/, Atom* /*argv*/)
    {
        toplevel()->throwTypeError(kTypeAppOfNonParamType);
        return undefinedAtom;
    }

    Atom ScriptObject::getSlotAtom(uint32_t slot, AvmCore *core)
    {
        Traits* traits = this->traits();
        const TraitsBindingsp td = traits->getTraitsBindings();
        // repeated if-else is actually more performant than a switch statement in this case.
        // SST_atom is most common case, put it first
        void* p;
        const SlotStorageType sst = td->calcSlotAddrAndSST(slot, (void*)this, p);
        if (sst == SST_atom)
        {
            return *((const Atom*)p);
        }
        else if (sst == SST_double)
        {
            return core->doubleToAtom(*((const double*)p));
        }
#ifdef VMCFG_FLOAT
        else if (sst == SST_float)
        {
            return traits->core->floatToAtom(*((const float*)p));
        }
        else if (sst == SST_float4)
        {
            return traits->core->float4ToAtom(AvmThunkUnbox_FLOAT4(float4_t, *((Atom*)p)) );
        }
#endif // VMCFG_FLOAT
        else if (sst == SST_int32)
        {
            return core->intToAtom(*((const int32_t*)p));
        }
        else if (sst == SST_uint32)
        {
            return core->uintToAtom(*((const int32_t*)p));
        }
        else if (sst == SST_bool32)
        {
            return (*((const int32_t*)p)<<3)|kBooleanType;
        }
        else if (sst == SST_string)
        {
            return (*((const Stringp*)p))->atom(); // may be null|kStringType, that's ok
        }
        else if (sst == SST_namespace)
        {
            return (*((const Namespacep*)p))->atom(); // may be null|kNamespaceType, no problemo
        }
        else // if (sst == SST_scriptobject)
        {
            AvmAssert(sst == SST_scriptobject);
            return (*((const ScriptObject**)p))->atom(); // may be null|kObjectType, copacetic
        }
    }

    Atom ScriptObject::getSlotAtom(uint32_t slot)
    {
        return getSlotAtom(slot, this->traits()->core);
    }

    ScriptObject* ScriptObject::getSlotObject(uint32_t slot)
    {
        Traits* traits = this->traits();
        const TraitsBindingsp td = traits->getTraitsBindings();
        void* p;
        const SlotStorageType sst = td->calcSlotAddrAndSST(slot, (void*)this, p);

        // based on profiling of Flex apps, it's *much* more common for the slot in this case
        // to have a type (vs "atom"), so check for that first...
        if (sst == SST_scriptobject)
        {
            return *((ScriptObject**)p);
        }
        else if (sst == SST_atom)
        {
            Atom const a = *((const Atom*)p);

            // don't call AvmCore::isObject(); it checks for null, which we don't care about here
            if (atomKind(a) == kObjectType)
                return (ScriptObject*)atomPtr(a);

            // else fall thru and return null
        }

        return NULL;
    }

    // note: coerceAndSetSlotAtom now includes a simplified and streamlined version
    // of Toplevel::coerce. If you modify that code, you might need to modify this code.
    void ScriptObject::coerceAndSetSlotAtom(uint32_t slot, Atom value)
    {
        Traits* traits = this->traits();
        const TraitsBindingsp td = traits->getTraitsBindings();
        void* p;
        const SlotStorageType sst = td->calcSlotAddrAndSST(slot, (void*)this, p);
        // repeated if-else is actually more performant than a switch statement in this case.
        // SST_atom is most common case, put it first
        if (sst == SST_atom)
        {
            // no call to coerce() needed, since anything will fit here... with one exception:
            // BUILTIN_object needs to convert undefined->null (though BUILTIN_any does not).
            // it's cheaper to do that here than call out to coerce().
            AvmAssert(td->getSlotTraits(slot) == NULL || td->getSlotTraits(slot)->builtinType == BUILTIN_object);
            if (value == undefinedAtom && td->getSlotTraits(slot) != NULL)
                value = nullObjectAtom;
            WBATOM(traits->core->GetGC(), this, (Atom*)p, value);
        }
        else if (sst == SST_double)
        {
            *((double*)p) = AvmCore::number(value);
        }
#ifdef VMCFG_FLOAT
        else if (sst == SST_float)
        {
            *((float*)p) = AvmCore::singlePrecisionFloat(value);
        }
        else if (sst == SST_float4)
        {
            float4_t* ptr = reinterpret_cast<float4_t*>(p);
            AvmCore::float4(ptr, value);
        }
#endif // VMCFG_FLOAT
        else if (sst == SST_int32)
        {
            *((int32_t*)p) = AvmCore::integer(value);
        }
        else if (sst == SST_uint32)
        {
            *((uint32_t*)p) = AvmCore::toUInt32(value);
        }
        else if (sst == SST_bool32)
        {
            *((int32_t*)p) = AvmCore::boolean(value);
        }
        else
        {
            // null/undefined -> NULL for all of these
            if (AvmCore::isNullOrUndefined(value))
            {
                value = (Atom)0; // don't bother setting tag bits
            }
            else if (sst == SST_string)
            {
                value = (Atom)traits->core->string(value); // don't bother setting tag bits
            }
            else if (sst == SST_namespace)
            {
                // Namespace is final, so we don't have to do the hard work
                if (atomKind(value) != kNamespaceType)
                    goto failure;
            }
            else // if (sst == SST_scriptobject)
            {
                AvmAssert(sst == SST_scriptobject);
                if (atomKind(value) != kObjectType || !AvmCore::atomToScriptObject(value)->traits()->subtypeof(td->getSlotTraits(slot)))
                    goto failure;
            }
            WBRC(traits->core->GetGC(), this, p, atomPtr(value));
        }
        return;

    failure:
        toplevel()->throwTypeError(kCheckTypeFailedError, traits->core->atomToErrorString(value), traits->core->toErrorString(td->getSlotTraits(slot)));
        return;
    }

    Atom ScriptObject::nextName(int index)
    {
        if (!traits()->needsHashtable())
            return nullStringAtom;
        
        AvmAssert(index > 0);

        InlineHashtable* ht = getTable();
        Atom m = ht->keyAt(index);
        return AvmCore::isNullOrUndefined(m) ? nullStringAtom : m;
    }

    Atom ScriptObject::nextValue(int index)
    {
        if (!traits()->needsHashtable())
            return undefinedAtom;

        AvmAssert(index > 0);

        InlineHashtable* ht = getTable();
        Atom m = ht->keyAt(index);
        if (AvmCore::isNullOrUndefined(m))
            return nullStringAtom;
        return ht->valueAt(index);
    }

    int ScriptObject::nextNameIndex(int index)
    {
        AvmAssert(index >= 0);

        if (!traits()->needsHashtable())
            return 0;

        return getTable()->next(index);
    }

#ifdef DEBUGGER
    uint64_t ScriptObject::bytesUsed() const
    {
        uint64_t bytesUsed = traits()->getTotalSize();
        if(traits()->needsHashtable())
        {
            if (traits()->isDictionary())
            {
                union {
                    uint8_t* p;
                    HeapHashtable** hht;
                };
                p = (uint8_t*)this + traits()->getHashtableOffset();
                bytesUsed += (*hht)->bytesUsed();
            }
            else
            {
                bytesUsed += getTable()->bytesUsed();
            }
        }
        return bytesUsed;
    }
#endif

    Stringp ScriptObject::implToString() const
    {
        AvmCore* core = this->core();
        Traits* t = this->traits();
        Stringp s = core->concatStrings(core->newConstantStringLatin1("[object "), t->name());
        return core->concatStrings(s, core->newConstantStringLatin1("]"));
    }

    uint32_t ScriptObject::getLengthProperty()
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();
        Multiname mname(core->getAnyPublicNamespace(), core->klength);
        Atom lenAtm = toplevel->getproperty(this->atom(), &mname, this->vtable);
        return AvmCore::toUInt32(lenAtm);
    }

    void ScriptObject::setLengthProperty(uint32_t newLen)
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();
        Multiname mname(core->getAnyPublicNamespace(), core->klength);
        Atom lenAtm = core->uintToAtom(newLen);
        toplevel->setproperty(this->atom(), &mname, lenAtm, this->vtable);
    }
}
