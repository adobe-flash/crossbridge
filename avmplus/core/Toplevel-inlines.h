/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE Toplevel::Toplevel(AbcEnv* abcEnv)
    : _traitsToAliasMap(abcEnv->core()->GetGC())
    , _abcEnv(abcEnv)
    , _scriptEntryPoints(abcEnv->core()->gc, 10)
    , _aliasToClassClosureMap(HeapHashtable::create(abcEnv->core()->GetGC()))
    , _isolateInternedObjects(WeakValueHashtable::create(abcEnv->core()->GetGC()))
{
}

REALLY_INLINE GCRef<builtinClassManifest> Toplevel::builtinClasses() const
{
    AvmAssert(_builtinClasses != NULL);
    return GCRef<builtinClassManifest>(_builtinClasses);
}

    
REALLY_INLINE GCRef<ArgumentErrorClass> Toplevel::argumentErrorClass() const { return builtinClasses()->get_ArgumentErrorClass(); }
REALLY_INLINE GCRef<ArrayClass> Toplevel::arrayClass() const { return builtinClasses()->get_ArrayClass(); }
REALLY_INLINE GCRef<BooleanClass> Toplevel::booleanClass() const { return builtinClasses()->get_BooleanClass(); }
REALLY_INLINE GCRef<ByteArrayClass> Toplevel::byteArrayClass() const { return builtinClasses()->get_ByteArrayClass(); }
REALLY_INLINE GCRef<ClassClass> Toplevel::classClass() const { return builtinClasses()->get_ClassClass(); }
REALLY_INLINE GCRef<EOFErrorClass> Toplevel::eofErrorClass() const { return builtinClasses()->get_EOFErrorClass(); }
REALLY_INLINE GCRef<IOErrorClass> Toplevel::ioErrorClass() const { return builtinClasses()->get_IOErrorClass(); }
REALLY_INLINE GCRef<IllegalOperationErrorClass> Toplevel::illegalOperationErrorClass() const { return builtinClasses()->get_IllegalOperationErrorClass(); }
REALLY_INLINE GCRef<MemoryErrorClass> Toplevel::memoryErrorClass() const { return builtinClasses()->get_MemoryErrorClass(); }
REALLY_INLINE GCRef<DateClass> Toplevel::dateClass() const { return builtinClasses()->get_DateClass(); }
REALLY_INLINE GCRef<DoubleVectorClass> Toplevel::doubleVectorClass() const { return builtinClasses()->get_Vector_doubleClass(); }
REALLY_INLINE GCRef<ErrorClass> Toplevel::errorClass() const { return builtinClasses()->get_ErrorClass(); }
REALLY_INLINE GCRef<EvalErrorClass> Toplevel::evalErrorClass() const { return builtinClasses()->get_EvalErrorClass(); }
#ifdef VMCFG_FLOAT
REALLY_INLINE GCRef<FloatClass> Toplevel::floatClass() const { return builtinClasses()->get_floatClass(); }
REALLY_INLINE GCRef<Float4Class> Toplevel::float4Class() const { return builtinClasses()->get_float4Class(); }
REALLY_INLINE GCRef<FloatVectorClass> Toplevel::floatVectorClass() const { return builtinClasses()->get_Vector_floatClass(); }
REALLY_INLINE GCRef<Float4VectorClass> Toplevel::float4VectorClass() const { return builtinClasses()->get_Vector_float4Class(); }
#endif
REALLY_INLINE GCRef<FunctionClass> Toplevel::functionClass() const { return builtinClasses()->get_FunctionClass(); }
REALLY_INLINE GCRef<IntClass> Toplevel::intClass() const { return builtinClasses()->get_intClass(); }
REALLY_INLINE GCRef<IntVectorClass> Toplevel::intVectorClass() const { return builtinClasses()->get_Vector_intClass(); }
REALLY_INLINE GCRef<MethodClosureClass> Toplevel::methodClosureClass() const { return builtinClasses()->get_MethodClosureClass(); }
REALLY_INLINE GCRef<NamespaceClass> Toplevel::namespaceClass() const { return builtinClasses()->get_NamespaceClass(); }
REALLY_INLINE GCRef<NumberClass> Toplevel::numberClass() const { return builtinClasses()->get_NumberClass(); }
REALLY_INLINE GCRef<ObjectVectorClass> Toplevel::objectVectorClass() const { return builtinClasses()->get_Vector_objectClass(); }
REALLY_INLINE GCRef<QNameClass> Toplevel::qnameClass() const { return builtinClasses()->get_QNameClass(); }
REALLY_INLINE GCRef<RangeErrorClass> Toplevel::rangeErrorClass() const { return builtinClasses()->get_RangeErrorClass(); }
REALLY_INLINE GCRef<ReferenceErrorClass> Toplevel::referenceErrorClass() const { return builtinClasses()->get_ReferenceErrorClass(); }
REALLY_INLINE GCRef<RegExpClass> Toplevel::regexpClass() const { return builtinClasses()->get_RegExpClass(); }
REALLY_INLINE GCRef<SecurityErrorClass> Toplevel::securityErrorClass() const { return builtinClasses()->get_SecurityErrorClass(); }
REALLY_INLINE GCRef<StringClass> Toplevel::stringClass() const { return builtinClasses()->get_StringClass(); }
REALLY_INLINE GCRef<SyntaxErrorClass> Toplevel::syntaxErrorClass() const { return builtinClasses()->get_SyntaxErrorClass(); }
REALLY_INLINE GCRef<TypeErrorClass> Toplevel::typeErrorClass() const { return builtinClasses()->get_TypeErrorClass(); }
REALLY_INLINE GCRef<UIntClass> Toplevel::uintClass() const { return builtinClasses()->get_uintClass(); }
REALLY_INLINE GCRef<UIntVectorClass> Toplevel::uintVectorClass() const { return builtinClasses()->get_Vector_uintClass(); }
REALLY_INLINE GCRef<URIErrorClass> Toplevel::uriErrorClass() const { return builtinClasses()->get_URIErrorClass(); }
REALLY_INLINE GCRef<VectorClass> Toplevel::vectorClass() const { return builtinClasses()->get_VectorClass(); }
REALLY_INLINE GCRef<VerifyErrorClass> Toplevel::verifyErrorClass() const { return builtinClasses()->get_VerifyErrorClass(); }
REALLY_INLINE GCRef<XMLClass> Toplevel::xmlClass() const { return builtinClasses()->get_XMLClass(); }
REALLY_INLINE GCRef<XMLListClass> Toplevel::xmlListClass() const { return builtinClasses()->get_XMLListClass(); }

REALLY_INLINE Atom Toplevel::callproperty(Atom base, const Multiname* name, int argc, Atom* atomv, VTable* vtable)
{
    AssertNotNull(base);
    Binding b = avmplus::getBinding(this, vtable, name);
    return avmplus::callprop_b(this, base, name, argc, atomv, vtable, b);
}

REALLY_INLINE VTable* Toplevel::toVTable(Atom atom)
{
    return avmplus::toVTable(this, atom);
}

REALLY_INLINE Atom Toplevel::op_construct(Atom ctor, int argc, Atom* atomv)
{
    return avmplus::op_construct(this, ctor, argc, atomv);
}

REALLY_INLINE Binding Toplevel::getBinding(Traits* traits, const Multiname* ref) const
{
    return avmplus::getBinding(this, traits, ref);
}

REALLY_INLINE Atom Toplevel::constructprop(const Multiname *name, int argc, Atom* atomv, VTable* vtable)
{
    return avmplus::constructprop(this, name, argc, atomv, vtable);
}

REALLY_INLINE Atom Toplevel::op_applytype(Atom obj, int argc, Atom* atomv)
{
    return avmplus::op_applytype(this, obj, argc, atomv);
}

REALLY_INLINE QNameObject* Toplevel::ToAttributeName(const Stringp arg)
{
    return ToAttributeName(arg->atom());
}

REALLY_INLINE Atom Toplevel::coerce(Atom atom, Traits* expected) const
{
    return avmplus::coerce(this, atom, expected);
}

// static
REALLY_INLINE bool Toplevel::contains(const uint32_t *uriSet, uint32_t ch)
{
    return (ch<0x80) && (uriSet[ch>>5]&(1<<(ch&0x1f))) != 0;
}

REALLY_INLINE void Toplevel::throwReferenceError(int id, const Multiname& multiname, const Traits* traits) const
{
    throwReferenceError(id, &multiname, traits);
}

REALLY_INLINE void Toplevel::throwReferenceError(int id, const Multiname& multiname) const
{
    throwReferenceError(id, &multiname);
}

#ifndef DEBUGGER
REALLY_INLINE void Toplevel::throwVerifyError(int id, Stringp) const
{
    throwVerifyError(id);
}

REALLY_INLINE void Toplevel::throwVerifyError(int id, Stringp, Stringp) const
{
    throwVerifyError(id);
}
#endif // !DEBUGGER

REALLY_INLINE ScriptEnv* Toplevel::scriptEnv(uint32_t index) const
{
    return _scriptEntryPoints.get(index);
}

REALLY_INLINE uint32_t Toplevel::scriptEnvCount()
{
    return _scriptEntryPoints.length();
}

REALLY_INLINE AbcEnv* Toplevel::abcEnv() const
{
    return _abcEnv;
}

REALLY_INLINE DomainEnv* Toplevel::domainEnv() const
{
    return _abcEnv->domainEnv();
}

REALLY_INLINE AvmCore* Toplevel::core() const
{
    return _abcEnv->pool()->core;
}

REALLY_INLINE MMgc::GC* Toplevel::gc() const
{
    return core()->GetGC();
}

REALLY_INLINE ScriptObject* Toplevel::global() const
{
    AvmAssert(_mainEntryPoint != NULL);
    AvmAssert(_mainEntryPoint->global != NULL);
    return _mainEntryPoint->global;
}

REALLY_INLINE Atom Toplevel::atom() const
{
    return global()->atom();
}

REALLY_INLINE void Toplevel::init_mainEntryPoint(ScriptEnv* main, builtinClassManifest* builtins)
{
    AvmAssert(_mainEntryPoint == NULL && _builtinClasses == NULL);
    _mainEntryPoint = main;
    _builtinClasses = builtins;
}

} // namespace avmplus
