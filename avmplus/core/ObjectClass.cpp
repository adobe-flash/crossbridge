/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    ObjectClass::ObjectClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        toplevel()->objectClass = this;

        // patch Object's instance vtable scope, since we created it earlier for
        // bootstrapping
        //ivtable()->scope = cvtable->scope;

        AvmAssert(traits()->getSizeOfInstance() == sizeof(ObjectClass));
        // it is correct call construct() when this.prototype == null
        setPrototypePtr(construct());
    }

    void ObjectClass::initPrototype()
    {
        // patch global.__proto__ = Object.prototype
        Toplevel* toplevel = this->toplevel();
        toplevel->global()->setDelegate(prototypePtr());                // global.__proto__ = Object.prototype
        this->setDelegate(toplevel->classClass()->prototypePtr());        // Object.__proto__ = Class.prototype
    }

    ScriptObject* ObjectClass::construct()
    {
        return (ScriptObject*) core()->newObject(ivtable(), prototypePtr());
    }

    // this = argv[0]
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom ObjectClass::call(int argc, Atom* argv)
    {
        // see E-262 15.2.1
        if (argc == 0 || AvmCore::isNullOrUndefined(argv[1]))
            return construct(argc, argv);
        else
            return argv[1];
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom ObjectClass::construct(int argc, Atom* argv)
    {
        switch (argc)
        {
        default:
            // TODO throw exception if argc > 1 (see E4 semantics)
        case 1: {
            Atom o = argv[1];
            if (!AvmCore::isNullOrUndefined(o))
                return o;
            // else fall through
        }
        case 0:
            return construct()->atom();
        }
    }

    /**
        15.2.4.5 Object.prototype.hasOwnProperty (V)
        When the hasOwnProperty method is called with argument V, the following steps are taken:
        1. Let O be this object.
        2. Call ToString(V).
        3. If O doesn't have a property with the name given by Result(2), return false.
        4. Return true.
        NOTE Unlike [[HasProperty]] (section 8.6.2.4), this method does not consider objects in the prototype chain.
     */
    bool ObjectClass::_hasOwnProperty(Atom thisAtom, Stringp name)
    {
        AvmCore* core = this->core();
        name = name ? core->internString(name) : (Stringp)core->knull;

        Traitsp t = NULL;
        switch (atomKind(thisAtom))
        {
            case kObjectType:
            {
                // ISSUE should this look in traits and dynamic vars, or just dynamic vars.
                ScriptObject* obj = AvmCore::atomToScriptObject(thisAtom);
                // TODO
                // The change below is important as otherwise we will throw error in a call to hasAtomProperty for ByteArrayObject.
                // This gets us back to the behaviour which we had in Marlin.
                // A bugzilla bug [ 562224 ] has been created to address this issue more cleanly in near future
                return obj->traits()->getTraitsBindings()->findBinding(name, core->findPublicNamespace()) != BIND_NONE ||
                    obj->hasStringProperty(name);
            }
            case kNamespaceType:
            case kStringType:
            case kBooleanType:
            case kDoubleType:
            case kIntptrType:
                t = toplevel()->toTraits(thisAtom);
                break;
            default:
                return false;
        }
        // NOTE use caller's public namespace
        return t->getTraitsBindings()->findBinding(name, core->findPublicNamespace()) != BIND_NONE;
    }

    bool ObjectClass::_propertyIsEnumerable(Atom thisAtom, Stringp name)
    {
        AvmCore* core = this->core();
        name = name ? core->internString(name) : (Stringp)core->knull;

        if (atomKind(thisAtom) == kObjectType)
        {
            ScriptObject* obj = AvmCore::atomToScriptObject(thisAtom);
            return obj->getStringPropertyIsEnumerable(name);
        }
        else if (atomKind(thisAtom) == kNamespaceType)
        {
            // Special case:
            // E4X 13.2.5.1, 13.2.5.2 specifies that prefix and uri
            // are not DontEnum.
            return name == core->kuri || name == core->kprefix;
        }
        else
        {
            return false;
        }
    }

    void ObjectClass::_setPropertyIsEnumerable(Atom thisAtom, Stringp name, bool enumerable)
    {
        AvmCore* core = this->core();
        name = name ? core->internString(name) : (Stringp)core->knull;

        if (atomKind(thisAtom) == kObjectType)
        {
            ScriptObject* obj = AvmCore::atomToScriptObject(thisAtom);
            obj->setStringPropertyIsEnumerable(name, enumerable);
        }
        else
        {
            // cannot create properties on a sealed object.
            // NOTE just use the unmarked version
            Multiname multiname(core->getAnyPublicNamespace(), name);
            // NOTE use default public
            toplevel()->throwReferenceError(kWriteSealedError, &multiname, traits());
        }
    }

    /**
        15.2.4.6 Object.prototype.isPrototypeOf (V)
        When the isPrototypeOf method is called with argument V, the following steps are taken:
        1. Let O be this object.
        2. If V is not an object, return false.
        3. Let V be the value of the [[Prototype]] property of V.
        4. if V is null, return false
        5. If O and V refer to the same object or if they refer to objects joined to each other (section 13.1.2), return true.
        6. Go to step 3.
    */
    bool ObjectClass::_isPrototypeOf(Atom thisAtom, Atom V)
    {
        // ECMA-262 Section 15.2.4.6
        if (AvmCore::isNullOrUndefined(V))
            return false;

        ScriptObject* o = toplevel()->toPrototype(V);
        for (; o != NULL; o = o->getDelegate())
        {
            if (o->atom() == thisAtom)
                return true;
        }
        return false;
    }

    /**
     * Object.prototype.toString()
     */
    Stringp ObjectClass::_toString(Atom thisAtom)
    {
        AvmCore* core = this->core();

        if (AvmCore::isObject(thisAtom))
        {
            return AvmCore::atomToScriptObject(thisAtom)->implToString();
        }
        else
        {
            Traits*     t = toplevel()->toTraits(thisAtom);
            Stringp s = core->concatStrings(core->newConstantStringLatin1("[object "), t->name());
            return core->concatStrings(s, core->newConstantStringLatin1("]"));
        }
    }
}
