/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* machine generated file via utils/exactgc.as -- do not edit */

namespace avmplus
{

#ifdef DEBUG
MMgc::GCTracerCheckResult ArgumentErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ArgumentErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ArgumentErrorClass
    m_slots_ArgumentErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ArgumentErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ArgumentErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ArgumentErrorObject
    m_slots_ArgumentErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ArrayClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ArrayClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ArrayClass
    m_slots_ArrayClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t ArrayObject::gcTracePointerOffsets[] = {
    offsetof(ArrayObject, m_denseArray),
    0};

MMgc::GCTracerCheckResult ArrayObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = m_denseArray.gcTraceOffsetIsTraced(off - offsetof(ArrayObject,m_denseArray))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ArrayObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ArrayObject
    m_slots_ArrayObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    m_denseArray.gcTrace(gc);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult BooleanClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool BooleanClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_BooleanClass
    m_slots_BooleanClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ByteArrayClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ByteArrayClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ByteArrayClass
    m_slots_ByteArrayClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t ByteArrayObject::gcTracePointerOffsets[] = {
    offsetof(ByteArrayObject, m_byteArray),
    0};

MMgc::GCTracerCheckResult ByteArrayObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = m_byteArray.gcTraceOffsetIsTraced(off - offsetof(ByteArrayObject,m_byteArray))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ByteArrayObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ByteArrayObject
    m_slots_ByteArrayObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    m_byteArray.gcTrace(gc);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ClassClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ClassClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ClassClass
    m_slots_ClassClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t ClassClosure::gcTracePointerOffsets[] = {
    offsetof(ClassClosure, m_prototype),
    0};

MMgc::GCTracerCheckResult ClassClosure::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ClassClosure::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ClassClosure
    m_slots_ClassClosure.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    gc->TraceLocation(&m_prototype);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ConditionClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ConditionClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ConditionClass
    m_slots_ConditionClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t ConditionObject::gcTracePointerOffsets[] = {
    offsetof(ConditionObject, m_mutex),
    0};

MMgc::GCTracerCheckResult ConditionObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ConditionObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ConditionObject
    m_slots_ConditionObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    gc->TraceLocation(&m_mutex);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DateClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DateClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DateClass
    m_slots_DateClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DateObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DateObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DateObject
    m_slots_DateObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DefinitionErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DefinitionErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DefinitionErrorClass
    m_slots_DefinitionErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DefinitionErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DefinitionErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DefinitionErrorObject
    m_slots_DefinitionErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DictionaryClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DictionaryClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DictionaryClass
    m_slots_DictionaryClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DictionaryObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DictionaryObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DictionaryObject
    m_slots_DictionaryObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    gcTraceHook_DictionaryObject(gc);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DoubleVectorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorClass<DoubleVectorObject>::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DoubleVectorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DoubleVectorClass
    m_slots_DoubleVectorClass.gcTracePrivateProperties(gc);
#endif
    TypedVectorClass<DoubleVectorObject>::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorClassXDoubleVectorObjectX_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DoubleVectorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorObject< DataList<double> >::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DoubleVectorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DoubleVectorObject
    m_slots_DoubleVectorObject.gcTracePrivateProperties(gc);
#endif
    TypedVectorObject< DataList<double> >::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorObjectXDataListXdoubleXX_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DynamicPropertyOutputClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DynamicPropertyOutputClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DynamicPropertyOutputClass
    m_slots_DynamicPropertyOutputClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult DynamicPropertyOutputObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DynamicPropertyOutputObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DynamicPropertyOutputObject
    m_slots_DynamicPropertyOutputObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ErrorClass
    m_slots_ErrorClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t ErrorObject::gcTracePointerOffsets[] = {
    offsetof(ErrorObject, stackTrace),
    0};

MMgc::GCTracerCheckResult ErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ErrorObject
    m_slots_ErrorObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    gc->TraceLocation(&stackTrace);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult EvalErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool EvalErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_EvalErrorClass
    m_slots_EvalErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult EvalErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool EvalErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_EvalErrorObject
    m_slots_EvalErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}


#if defined(VMCFG_FLOAT)

#ifdef DEBUG
MMgc::GCTracerCheckResult Float4Class::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool Float4Class::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_Float4Class
    m_slots_Float4Class.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}

#endif // defined(VMCFG_FLOAT)

#if defined(VMCFG_FLOAT)

#ifdef DEBUG
MMgc::GCTracerCheckResult Float4VectorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorClass<Float4VectorObject>::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool Float4VectorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_Float4VectorClass
    m_slots_Float4VectorClass.gcTracePrivateProperties(gc);
#endif
    TypedVectorClass<Float4VectorObject>::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorClassXFloat4VectorObjectX_isExactInterlock != 0);
    return false;
}

#endif // defined(VMCFG_FLOAT)

#if defined(VMCFG_FLOAT)

#ifdef DEBUG
MMgc::GCTracerCheckResult Float4VectorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = Float4VectorObjectBaseClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool Float4VectorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_Float4VectorObject
    m_slots_Float4VectorObject.gcTracePrivateProperties(gc);
#endif
    Float4VectorObjectBaseClass::gcTrace(gc, 0);
    (void)(avmplus_Float4VectorObjectBaseClass_isExactInterlock != 0);
    return false;
}

#endif // defined(VMCFG_FLOAT)

#if defined(VMCFG_FLOAT)

#ifdef DEBUG
MMgc::GCTracerCheckResult FloatClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool FloatClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_FloatClass
    m_slots_FloatClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}

#endif // defined(VMCFG_FLOAT)

#if defined(VMCFG_FLOAT)

#ifdef DEBUG
MMgc::GCTracerCheckResult FloatVectorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorClass<FloatVectorObject>::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool FloatVectorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_FloatVectorClass
    m_slots_FloatVectorClass.gcTracePrivateProperties(gc);
#endif
    TypedVectorClass<FloatVectorObject>::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorClassXFloatVectorObjectX_isExactInterlock != 0);
    return false;
}

#endif // defined(VMCFG_FLOAT)

#if defined(VMCFG_FLOAT)

#ifdef DEBUG
MMgc::GCTracerCheckResult FloatVectorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorObject< DataList<float> >::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool FloatVectorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_FloatVectorObject
    m_slots_FloatVectorObject.gcTracePrivateProperties(gc);
#endif
    TypedVectorObject< DataList<float> >::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorObjectXDataListXfloatXX_isExactInterlock != 0);
    return false;
}

#endif // defined(VMCFG_FLOAT)


#ifdef DEBUG
MMgc::GCTracerCheckResult FunctionClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool FunctionClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_FunctionClass
    m_slots_FunctionClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t FunctionObject::gcTracePointerOffsets[] = {
    offsetof(FunctionObject, m_callEnv),
    0};

MMgc::GCTracerCheckResult FunctionObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool FunctionObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_FunctionObject
    m_slots_FunctionObject.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    gc->TraceLocation(&m_callEnv);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult IntClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool IntClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_IntClass
    m_slots_IntClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult IntVectorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorClass<IntVectorObject>::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool IntVectorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_IntVectorClass
    m_slots_IntVectorClass.gcTracePrivateProperties(gc);
#endif
    TypedVectorClass<IntVectorObject>::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorClassXIntVectorObjectX_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult IntVectorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorObject< DataList<int32_t> >::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool IntVectorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_IntVectorObject
    m_slots_IntVectorObject.gcTracePrivateProperties(gc);
#endif
    TypedVectorObject< DataList<int32_t> >::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorObjectXDataListXint32_tXX_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult JSONClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool JSONClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_JSONClass
    m_slots_JSONClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult MathClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool MathClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_MathClass
    m_slots_MathClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t MethodClosure::gcTracePointerOffsets[] = {
    offsetof(MethodClosure, m_savedThis),
    0};

MMgc::GCTracerCheckResult MethodClosure::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = FunctionObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool MethodClosure::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_MethodClosure
    m_slots_MethodClosure.gcTracePrivateProperties(gc);
#endif
    FunctionObject::gcTrace(gc, 0);
    (void)(avmplus_FunctionObject_isExactInterlock != 0);
    gc->TraceAtom(&m_savedThis);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult MethodClosureClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool MethodClosureClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_MethodClosureClass
    m_slots_MethodClosureClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult MutexClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool MutexClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_MutexClass
    m_slots_MutexClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult MutexObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool MutexObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_MutexObject
    m_slots_MutexObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NamespaceClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NamespaceClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NamespaceClass
    m_slots_NamespaceClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NumberClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NumberClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NumberClass
    m_slots_NumberClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ObjectClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ObjectClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ObjectClass
    m_slots_ObjectClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t ObjectEncodingClass::gcTracePointerOffsets[] = {
    offsetof(ObjectEncodingClass, m_writer),
    0};

MMgc::GCTracerCheckResult ObjectEncodingClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ObjectEncodingClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ObjectEncodingClass
    m_slots_ObjectEncodingClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    gc->TraceLocation(&m_writer);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ObjectInputClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ObjectInputClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ObjectInputClass
    m_slots_ObjectInputClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ObjectInputObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ObjectInputObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ObjectInputObject
    m_slots_ObjectInputObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ObjectOutputClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ObjectOutputClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ObjectOutputClass
    m_slots_ObjectOutputClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ObjectOutputObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ObjectOutputObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ObjectOutputObject
    m_slots_ObjectOutputObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ObjectVectorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorClass<ObjectVectorObject>::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ObjectVectorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ObjectVectorClass
    m_slots_ObjectVectorClass.gcTracePrivateProperties(gc);
#endif
    TypedVectorClass<ObjectVectorObject>::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorClassXObjectVectorObjectX_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ObjectVectorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorObject< AtomList >::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ObjectVectorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ObjectVectorObject
    m_slots_ObjectVectorObject.gcTracePrivateProperties(gc);
#endif
    TypedVectorObject< AtomList >::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorObjectXAtomListX_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ProxyClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ProxyClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ProxyClass
    m_slots_ProxyClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ProxyObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ProxyObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ProxyObject
    m_slots_ProxyObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult QNameClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool QNameClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_QNameClass
    m_slots_QNameClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t QNameObject::gcTracePointerOffsets[] = {
    offsetof(QNameObject, m_mn),
    0};

MMgc::GCTracerCheckResult QNameObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = m_mn.gcTraceOffsetIsTraced(off - offsetof(QNameObject,m_mn))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool QNameObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_QNameObject
    m_slots_QNameObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    m_mn.gcTrace(gc);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult RangeErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool RangeErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_RangeErrorClass
    m_slots_RangeErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult RangeErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool RangeErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_RangeErrorObject
    m_slots_RangeErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ReferenceErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ReferenceErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ReferenceErrorClass
    m_slots_ReferenceErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult ReferenceErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ReferenceErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_ReferenceErrorObject
    m_slots_ReferenceErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult RegExpClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool RegExpClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_RegExpClass
    m_slots_RegExpClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t RegExpObject::gcTracePointerOffsets[] = {
    offsetof(RegExpObject, m_pcreInst),
    offsetof(RegExpObject, m_source),
    0};

MMgc::GCTracerCheckResult RegExpObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool RegExpObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_RegExpObject
    m_slots_RegExpObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    gc->TraceLocation(&m_pcreInst);
    gc->TraceLocation(&m_source);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult SecurityErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool SecurityErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_SecurityErrorClass
    m_slots_SecurityErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult SecurityErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool SecurityErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_SecurityErrorObject
    m_slots_SecurityErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult StringClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool StringClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_StringClass
    m_slots_StringClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult SyntaxErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool SyntaxErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_SyntaxErrorClass
    m_slots_SyntaxErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult SyntaxErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool SyntaxErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_SyntaxErrorObject
    m_slots_SyntaxErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult TypeErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool TypeErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_TypeErrorClass
    m_slots_TypeErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult TypeErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool TypeErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_TypeErrorObject
    m_slots_TypeErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult UIntClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool UIntClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_UIntClass
    m_slots_UIntClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult UIntVectorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorClass<UIntVectorObject>::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool UIntVectorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_UIntVectorClass
    m_slots_UIntVectorClass.gcTracePrivateProperties(gc);
#endif
    TypedVectorClass<UIntVectorObject>::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorClassXUIntVectorObjectX_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult UIntVectorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = TypedVectorObject< DataList<uint32_t> >::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool UIntVectorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_UIntVectorObject
    m_slots_UIntVectorObject.gcTracePrivateProperties(gc);
#endif
    TypedVectorObject< DataList<uint32_t> >::gcTrace(gc, 0);
    (void)(avmplus_TypedVectorObjectXDataListXuint32_tXX_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult URIErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool URIErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_URIErrorClass
    m_slots_URIErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult URIErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool URIErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_URIErrorObject
    m_slots_URIErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult UninitializedErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool UninitializedErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_UninitializedErrorClass
    m_slots_UninitializedErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult UninitializedErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool UninitializedErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_UninitializedErrorObject
    m_slots_UninitializedErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult VectorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool VectorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_VectorClass
    m_slots_VectorClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult VerifyErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = NativeErrorClass::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool VerifyErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_VerifyErrorClass
    m_slots_VerifyErrorClass.gcTracePrivateProperties(gc);
#endif
    NativeErrorClass::gcTrace(gc, 0);
    (void)(avmplus_NativeErrorClass_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult VerifyErrorObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ErrorObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool VerifyErrorObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_VerifyErrorObject
    m_slots_VerifyErrorObject.gcTracePrivateProperties(gc);
#endif
    ErrorObject::gcTrace(gc, 0);
    (void)(avmplus_ErrorObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult XMLClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool XMLClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_XMLClass
    m_slots_XMLClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult XMLListClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool XMLListClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_XMLListClass
    m_slots_XMLListClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t XMLListObject::gcTracePointerOffsets[] = {
    offsetof(XMLListObject, m_children),
    offsetof(XMLListObject, m_targetObject),
    offsetof(XMLListObject, m_targetProperty),
    0};

MMgc::GCTracerCheckResult XMLListObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = m_children.gcTraceOffsetIsTraced(off - offsetof(XMLListObject,m_children))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = m_targetProperty.gcTraceOffsetIsTraced(off - offsetof(XMLListObject,m_targetProperty))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,3);
}
#endif // DEBUG

bool XMLListObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_XMLListObject
    m_slots_XMLListObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    m_children.gcTrace(gc);
    gc->TraceAtom(&m_targetObject);
    m_targetProperty.gcTrace(gc);
    return false;
}



#ifdef DEBUG
const uint32_t XMLObject::gcTracePointerOffsets[] = {
    offsetof(XMLObject, m_node),
    offsetof(XMLObject, publicNS),
    0};

MMgc::GCTracerCheckResult XMLObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool XMLObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_XMLObject
    m_slots_XMLObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    gc->TraceLocation(&m_node);
    gc->TraceLocation(&publicNS);
    return false;
}



#ifdef DEBUG
const uint32_t AbcEnv::gcTracePointerOffsets[] = {
    offsetof(AbcEnv, m_codeContext),
    offsetof(AbcEnv, m_domainEnv),
    offsetof(AbcEnv, m_finddef_table),
#if defined(DEBUGGER)
    offsetof(AbcEnv, m_invocationCounts),
#endif
    offsetof(AbcEnv, m_pool),
    0};

MMgc::GCTracerCheckResult AbcEnv::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,5);
}
#endif // DEBUG

bool AbcEnv::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    if (_xact_cursor == 0) {
        gc->TraceLocation(&m_codeContext);
        gc->TraceLocation(&m_domainEnv);
        gc->TraceLocation(&m_finddef_table);
#if defined(DEBUGGER)
        gc->TraceLocation(&m_invocationCounts);
#endif
        gc->TraceLocation(&m_pool);
    }
    const size_t _xact_work_increment = 2000/sizeof(void*);
    const size_t _xact_work_count = m_pool ? m_pool->methodCount() : 0;
    if (_xact_cursor * _xact_work_increment >= _xact_work_count)
        return false;
    size_t _xact_work = _xact_work_increment;
    bool _xact_more = true;
    if ((_xact_cursor + 1) * _xact_work_increment >= _xact_work_count)
    {
        _xact_work = _xact_work_count - (_xact_cursor * _xact_work_increment);
        _xact_more = false;
    }
    gc->TraceLocations((m_methods+(_xact_cursor * _xact_work_increment)), _xact_work);
    return _xact_more;
}


#if defined(DEBUGGER)

#ifdef DEBUG
const uint32_t AbcFile::gcTracePointerOffsets[] = {
    offsetof(AbcFile, abcname),
    offsetof(AbcFile, source),
    offsetof(AbcFile, sourcemap),
    0};

MMgc::GCTracerCheckResult AbcFile::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = source.gcTraceOffsetIsTraced(off - offsetof(AbcFile,source))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = AbcInfo::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,3);
}
#endif // DEBUG

bool AbcFile::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    AbcInfo::gcTrace(gc, 0);
    (void)(avmplus_AbcInfo_isExactInterlock != 0);
    gc->TraceLocation(&abcname);
    source.gcTrace(gc);
    gc->TraceLocation(&sourcemap);
    return false;
}

#endif // defined(DEBUGGER)

#if defined(DEBUGGER)

#ifdef DEBUG
MMgc::GCTracerCheckResult AbcInfo::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool AbcInfo::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;

    return false;
}

#endif // defined(DEBUGGER)


#ifdef DEBUG
MMgc::GCTracerCheckResult InlineHashtable::AtomContainer::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool InlineHashtable::AtomContainer::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    const size_t _xact_work_increment = 2000/sizeof(void*);
    const size_t _xact_work_count = count();
    if (_xact_cursor * _xact_work_increment >= _xact_work_count)
        return false;
    size_t _xact_work = _xact_work_increment;
    bool _xact_more = true;
    if ((_xact_cursor + 1) * _xact_work_increment >= _xact_work_count)
    {
        _xact_work = _xact_work_count - (_xact_cursor * _xact_work_increment);
        _xact_more = false;
    }
    gc->TraceAtoms((atoms+(_xact_cursor * _xact_work_increment)), _xact_work);
    return _xact_more;
}



#ifdef DEBUG
const uint32_t AttributeE4XNode::gcTracePointerOffsets[] = {
    offsetof(AttributeE4XNode, m_value),
    0};

MMgc::GCTracerCheckResult AttributeE4XNode::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = E4XNode::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool AttributeE4XNode::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    E4XNode::gcTrace(gc, 0);
    (void)(avmplus_E4XNode_isExactInterlock != 0);
    gc->TraceLocation(&m_value);
    return false;
}



#ifdef DEBUG
const uint32_t CDATAE4XNode::gcTracePointerOffsets[] = {
    offsetof(CDATAE4XNode, m_value),
    0};

MMgc::GCTracerCheckResult CDATAE4XNode::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = E4XNode::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool CDATAE4XNode::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    E4XNode::gcTrace(gc, 0);
    (void)(avmplus_E4XNode_isExactInterlock != 0);
    gc->TraceLocation(&m_value);
    return false;
}



#ifdef DEBUG
const uint32_t CommentE4XNode::gcTracePointerOffsets[] = {
    offsetof(CommentE4XNode, m_value),
    0};

MMgc::GCTracerCheckResult CommentE4XNode::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = E4XNode::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool CommentE4XNode::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    E4XNode::gcTrace(gc, 0);
    (void)(avmplus_E4XNode_isExactInterlock != 0);
    gc->TraceLocation(&m_value);
    return false;
}


#if defined(DEBUGGER)

#ifdef DEBUG
const uint32_t Debugger::gcTracePointerOffsets[] = {
    offsetof(Debugger, abcList),
    offsetof(Debugger, trace_callback),
    0};

MMgc::GCTracerCheckResult Debugger::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = abcList.gcTraceOffsetIsTraced(off - offsetof(Debugger,abcList))) != MMgc::kOffsetNotFound) {
        return result;
    }
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool Debugger::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    abcList.gcTrace(gc);
    gc->TraceLocation(&trace_callback);
    return false;
}

#endif // defined(DEBUGGER)

#if defined(DEBUGGER)

#ifdef DEBUG
const uint32_t DebuggerMethodInfo::gcTracePointerOffsets[] = {
    offsetof(DebuggerMethodInfo, file),
    0};

MMgc::GCTracerCheckResult DebuggerMethodInfo::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool DebuggerMethodInfo::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&file);
    gc->TraceLocations((localNames+0), local_count);
    return false;
}

#endif // defined(DEBUGGER)


#ifdef DEBUG
const uint32_t Domain::gcTracePointerOffsets[] = {
    offsetof(Domain, m_cachedScripts),
    offsetof(Domain, m_cachedTraits),
    offsetof(Domain, m_loadedScripts),
    offsetof(Domain, m_loadedTraits),
    offsetof(Domain, m_parameterizedTypes),
    0};

MMgc::GCTracerCheckResult Domain::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,5);
}
#endif // DEBUG

bool Domain::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&m_cachedScripts);
    gc->TraceLocation(&m_cachedTraits);
    gc->TraceLocation(&m_loadedScripts);
    gc->TraceLocation(&m_loadedTraits);
    gc->TraceLocation(&m_parameterizedTypes);
    gc->TraceLocations((m_bases+0), m_baseCount);
    return false;
}



#ifdef DEBUG
const uint32_t DomainEnv::gcTracePointerOffsets[] = {
    offsetof(DomainEnv, m_domain),
    offsetof(DomainEnv, m_globalMemoryProviderObject),
    offsetof(DomainEnv, m_scriptEnvMap),
    offsetof(DomainEnv, m_toplevel),
    0};

MMgc::GCTracerCheckResult DomainEnv::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,4);
}
#endif // DEBUG

bool DomainEnv::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&m_domain);
    gc->TraceLocation(&m_globalMemoryProviderObject);
    gc->TraceLocation(&m_scriptEnvMap);
    gc->TraceLocation(&m_toplevel);
    gc->TraceLocations((m_bases+0), m_baseCount);
    return false;
}



#ifdef DEBUG
const uint32_t E4XNode::gcTracePointerOffsets[] = {
    offsetof(E4XNode, m_nameOrAux),
    offsetof(E4XNode, m_parent),
    0};

MMgc::GCTracerCheckResult E4XNode::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool E4XNode::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&m_nameOrAux);
    gc->TraceLocation(&m_parent);
    return false;
}



#ifdef DEBUG
const uint32_t E4XNodeAux::gcTracePointerOffsets[] = {
    offsetof(E4XNodeAux, m_name),
    offsetof(E4XNodeAux, m_notification),
    offsetof(E4XNodeAux, m_ns),
    0};

MMgc::GCTracerCheckResult E4XNodeAux::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,3);
}
#endif // DEBUG

bool E4XNodeAux::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&m_name);
    gc->TraceLocation(&m_notification);
    gc->TraceLocation(&m_ns);
    return false;
}



#ifdef DEBUG
const uint32_t ElementE4XNode::gcTracePointerOffsets[] = {
    offsetof(ElementE4XNode, m_attributes),
    offsetof(ElementE4XNode, m_children),
    offsetof(ElementE4XNode, m_namespaces),
    0};

MMgc::GCTracerCheckResult ElementE4XNode::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = E4XNode::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,3);
}
#endif // DEBUG

bool ElementE4XNode::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    E4XNode::gcTrace(gc, 0);
    (void)(avmplus_E4XNode_isExactInterlock != 0);
    gc->TraceLocation(&m_attributes);
    gc->TraceLocation(&m_children);
    gc->TraceLocation(&m_namespaces);
    return false;
}


#if defined(DEBUG)

#ifdef DEBUG
const uint32_t ExactGCTest::gcTracePointerOffsets[] = {
#if !(CASE1) && !(CASE2) && (!CASE3NEG)
    offsetof(ExactGCTest, case1_and_3_w),
#endif
#if (CASE1)
    offsetof(ExactGCTest, case1_and_3_w),
#endif
#if (CASE1)
    offsetof(ExactGCTest, case1_w),
#endif
#if (CASE1)
    offsetof(ExactGCTest, case1_x),
#endif
#if !(CASE1) && (CASE2)
    offsetof(ExactGCTest, case2_x),
#endif
#if !(CASE1) && (CASE2)
    offsetof(ExactGCTest, case2_z),
#endif
#if !(CASE1) && !(CASE2) && (!CASE3NEG)
    offsetof(ExactGCTest, case3_z),
#endif
#if !(CASE1) && !(CASE2) && !(!CASE3NEG)
    offsetof(ExactGCTest, else_r),
#endif
    offsetof(ExactGCTest, nestedTemplate),
    offsetof(ExactGCTest, sobject1),
    offsetof(ExactGCTest, sobject2),
    offsetof(ExactGCTest, sobject3),
    offsetof(ExactGCTest, sobject4),
    0};

MMgc::GCTracerCheckResult ExactGCTest::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,13);
}
#endif // DEBUG

bool ExactGCTest::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
#if !(CASE1) && !(CASE2) && (!CASE3NEG)
    gc->TraceLocation(&case1_and_3_w);
#endif
#if (CASE1)
    gc->TraceLocation(&case1_and_3_w);
#endif
#if (CASE1)
    gc->TraceLocation(&case1_w);
#endif
#if (CASE1)
    gc->TraceLocation(&case1_x);
#endif
#if !(CASE1) && (CASE2)
    gc->TraceLocation(&case2_x);
#endif
#if !(CASE1) && (CASE2)
    gc->TraceLocation(&case2_z);
#endif
#if !(CASE1) && !(CASE2) && (!CASE3NEG)
    gc->TraceLocation(&case3_z);
#endif
#if !(CASE1) && !(CASE2) && !(!CASE3NEG)
    gc->TraceLocation(&else_r);
#endif
    gc->TraceLocation(&nestedTemplate);
    gc->TraceLocation(&sobject1);
    gc->TraceLocation(&sobject2);
    gc->TraceLocation(&sobject3);
    gc->TraceLocation(&sobject4);
    return false;
}

#endif // defined(DEBUG)


#ifdef DEBUG
MMgc::GCTracerCheckResult ExceptionHandlerTable::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool ExceptionHandlerTable::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    const size_t _xact_work_increment = 2000/sizeof(void*);
    const size_t _xact_work_count = exception_count;
    if (_xact_cursor * _xact_work_increment >= _xact_work_count)
        return false;
    size_t _xact_work = _xact_work_increment;
    bool _xact_more = true;
    if ((_xact_cursor + 1) * _xact_work_increment >= _xact_work_count)
    {
        _xact_work = _xact_work_count - (_xact_cursor * _xact_work_increment);
        _xact_more = false;
    }
    for ( size_t _xact_iter=0 ; _xact_iter < _xact_work; _xact_iter++ )
        exceptions[+_xact_iter+(_xact_cursor * _xact_work_increment)].gcTrace(gc);
    return _xact_more;
}



#ifdef DEBUG
const uint32_t FunctionEnv::gcTracePointerOffsets[] = {
    offsetof(FunctionEnv, closure),
    0};

MMgc::GCTracerCheckResult FunctionEnv::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = MethodEnv::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool FunctionEnv::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    MethodEnv::gcTrace(gc, 0);
    (void)(avmplus_MethodEnv_isExactInterlock != 0);
    gc->TraceLocation(&closure);
    return false;
}



#ifdef DEBUG
const uint32_t HeapHashtable::gcTracePointerOffsets[] = {
    offsetof(HeapHashtable, ht),
    0};

MMgc::GCTracerCheckResult HeapHashtable::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ht.gcTraceOffsetIsTraced(off - offsetof(HeapHashtable,ht))) != MMgc::kOffsetNotFound) {
        return result;
    }
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool HeapHashtable::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    ht.gcTrace(gc);
    return false;
}



#ifdef DEBUG
const uint32_t HeapHashtableRC::gcTracePointerOffsets[] = {
    offsetof(HeapHashtableRC, ht),
    0};

MMgc::GCTracerCheckResult HeapHashtableRC::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ht.gcTraceOffsetIsTraced(off - offsetof(HeapHashtableRC,ht))) != MMgc::kOffsetNotFound) {
        return result;
    }
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool HeapHashtableRC::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    ht.gcTrace(gc);
    return false;
}



#ifdef DEBUG
const uint32_t InlineHashtable::gcTracePointerOffsets[] = {
    offsetof(InlineHashtable, m_atomsAndFlags),
    0};

MMgc::GCTracerCheckResult InlineHashtable::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool InlineHashtable::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&m_atomsAndFlags);
    return false;
}



#ifdef DEBUG
const uint32_t LivePoolNode::gcTracePointerOffsets[] = {
    offsetof(LivePoolNode, pool),
    0};

MMgc::GCTracerCheckResult LivePoolNode::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool LivePoolNode::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&pool);
    return false;
}



#ifdef DEBUG
const uint32_t MethodEnv::gcTracePointerOffsets[] = {
    offsetof(MethodEnv, _scope),
    offsetof(MethodEnv, activationOrMCTable),
    offsetof(MethodEnv, method),
    0};

MMgc::GCTracerCheckResult MethodEnv::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = MethodEnvProcHolder::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,3);
}
#endif // DEBUG

bool MethodEnv::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    MethodEnvProcHolder::gcTrace(gc, 0);
    (void)(avmplus_MethodEnvProcHolder_isExactInterlock != 0);
    gc->TraceLocation(&_scope);
    gc->TraceConservativeLocation(&activationOrMCTable);
    gc->TraceLocation(&method);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult MethodEnvProcHolder::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool MethodEnvProcHolder::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;

    return false;
}



#ifdef DEBUG
const uint32_t MethodInfo::gcTracePointerOffsets[] = {
    offsetof(MethodInfo, _activation),
    offsetof(MethodInfo, _declarer),
#if defined(AVMPLUS_SAMPLER)
    offsetof(MethodInfo, _methodName),
#endif
    offsetof(MethodInfo, _msref),
    offsetof(MethodInfo, _pool),
    0};

MMgc::GCTracerCheckResult MethodInfo::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = _activation.gcTraceOffsetIsTraced(off - offsetof(MethodInfo,_activation))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = _declarer.gcTraceOffsetIsTraced(off - offsetof(MethodInfo,_declarer))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = MethodInfoProcHolder::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,5);
}
#endif // DEBUG

bool MethodInfo::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    MethodInfoProcHolder::gcTrace(gc, 0);
    (void)(avmplus_MethodInfoProcHolder_isExactInterlock != 0);
    gcTraceHook_MethodInfo(gc);
    _activation.gcTrace(gc);
    _declarer.gcTrace(gc);
#if defined(AVMPLUS_SAMPLER)
    gc->TraceLocation(&_methodName);
#endif
    gc->TraceLocation(&_msref);
    gc->TraceLocation(&_pool);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult MethodInfoProcHolder::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool MethodInfoProcHolder::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;

    return false;
}



#ifdef DEBUG
const uint32_t MethodSignature::gcTracePointerOffsets[] = {
    offsetof(MethodSignature, _returnTraits),
    0};

MMgc::GCTracerCheckResult MethodSignature::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = QCachedItem::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool MethodSignature::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    QCachedItem::gcTrace(gc, 0);
    (void)(avmplus_QCachedItem_isExactInterlock != 0);
    gcTraceHook_MethodSignature(gc);
    gc->TraceLocation(&_returnTraits);
    return false;
}



#ifdef DEBUG
const uint32_t Namespace::gcTracePointerOffsets[] = {
    offsetof(Namespace, m_prefix),
    offsetof(Namespace, m_uriAndType),
    0};

MMgc::GCTracerCheckResult Namespace::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool Namespace::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceAtom(&m_prefix);
    gc->TraceLocation(&m_uriAndType);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NamespaceSet::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NamespaceSet::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocations((_namespaces+0), count());
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeErrorClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeErrorClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t PIE4XNode::gcTracePointerOffsets[] = {
    offsetof(PIE4XNode, m_value),
    0};

MMgc::GCTracerCheckResult PIE4XNode::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = E4XNode::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool PIE4XNode::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    E4XNode::gcTrace(gc, 0);
    (void)(avmplus_E4XNode_isExactInterlock != 0);
    gc->TraceLocation(&m_value);
    return false;
}



#ifdef DEBUG
const uint32_t PoolObject::gcTracePointerOffsets[] = {
    offsetof(PoolObject, _abcStrings),
    offsetof(PoolObject, _classes),
    offsetof(PoolObject, _code),
#if defined(DEBUGGER)
    offsetof(PoolObject, _method_dmi),
#endif
    offsetof(PoolObject, _method_name_indices),
    offsetof(PoolObject, _methods),
    offsetof(PoolObject, _scripts),
#if defined(VMCFG_HALFMOON)
    offsetof(PoolObject, cpool_const_double),
#endif
#if defined(VMCFG_HALFMOON)
    offsetof(PoolObject, cpool_const_string),
#endif
    offsetof(PoolObject, cpool_double),
#if defined(VMCFG_FLOAT)
    offsetof(PoolObject, cpool_float),
#endif
#if defined(VMCFG_FLOAT)
    offsetof(PoolObject, cpool_float4),
#endif
    offsetof(PoolObject, cpool_int),
#if !defined(AVMPLUS_64BIT)
    offsetof(PoolObject, cpool_int_atoms),
#endif
    offsetof(PoolObject, cpool_mn_offsets),
    offsetof(PoolObject, cpool_ns),
    offsetof(PoolObject, cpool_ns_set),
    offsetof(PoolObject, cpool_uint),
#if !defined(AVMPLUS_64BIT)
    offsetof(PoolObject, cpool_uint_atoms),
#endif
    offsetof(PoolObject, domain),
    offsetof(PoolObject, m_cachedScripts),
    offsetof(PoolObject, m_cachedTraits),
    offsetof(PoolObject, m_loadedScripts),
    offsetof(PoolObject, m_loadedTraits),
    offsetof(PoolObject, metadata_infos),
    offsetof(PoolObject, precompNames),
    0};

MMgc::GCTracerCheckResult PoolObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = _classes.gcTraceOffsetIsTraced(off - offsetof(PoolObject,_classes))) != MMgc::kOffsetNotFound) {
        return result;
    }
#if defined(DEBUGGER)
    if((result = _method_dmi.gcTraceOffsetIsTraced(off - offsetof(PoolObject,_method_dmi))) != MMgc::kOffsetNotFound) {
        return result;
    }
#endif
    if((result = _method_name_indices.gcTraceOffsetIsTraced(off - offsetof(PoolObject,_method_name_indices))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = _methods.gcTraceOffsetIsTraced(off - offsetof(PoolObject,_methods))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = _scripts.gcTraceOffsetIsTraced(off - offsetof(PoolObject,_scripts))) != MMgc::kOffsetNotFound) {
        return result;
    }
#if defined(VMCFG_HALFMOON)
    if((result = cpool_const_double.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_const_double))) != MMgc::kOffsetNotFound) {
        return result;
    }
#endif
#if defined(VMCFG_HALFMOON)
    if((result = cpool_const_string.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_const_string))) != MMgc::kOffsetNotFound) {
        return result;
    }
#endif
    if((result = cpool_double.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_double))) != MMgc::kOffsetNotFound) {
        return result;
    }
#if defined(VMCFG_FLOAT)
    if((result = cpool_float.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_float))) != MMgc::kOffsetNotFound) {
        return result;
    }
#endif
#if defined(VMCFG_FLOAT)
    if((result = cpool_float4.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_float4))) != MMgc::kOffsetNotFound) {
        return result;
    }
#endif
    if((result = cpool_int.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_int))) != MMgc::kOffsetNotFound) {
        return result;
    }
#if !defined(AVMPLUS_64BIT)
    if((result = cpool_int_atoms.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_int_atoms))) != MMgc::kOffsetNotFound) {
        return result;
    }
#endif
    if((result = cpool_mn_offsets.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_mn_offsets))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = cpool_ns.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_ns))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = cpool_ns_set.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_ns_set))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = cpool_uint.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_uint))) != MMgc::kOffsetNotFound) {
        return result;
    }
#if !defined(AVMPLUS_64BIT)
    if((result = cpool_uint_atoms.gcTraceOffsetIsTraced(off - offsetof(PoolObject,cpool_uint_atoms))) != MMgc::kOffsetNotFound) {
        return result;
    }
#endif
    if((result = metadata_infos.gcTraceOffsetIsTraced(off - offsetof(PoolObject,metadata_infos))) != MMgc::kOffsetNotFound) {
        return result;
    }
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,26);
}
#endif // DEBUG

bool PoolObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&_abcStrings);
    _classes.gcTrace(gc);
    gc->TraceLocation(&_code);
#if defined(DEBUGGER)
    _method_dmi.gcTrace(gc);
#endif
    _method_name_indices.gcTrace(gc);
    _methods.gcTrace(gc);
    _scripts.gcTrace(gc);
#if defined(VMCFG_HALFMOON)
    cpool_const_double.gcTrace(gc);
#endif
#if defined(VMCFG_HALFMOON)
    cpool_const_string.gcTrace(gc);
#endif
    cpool_double.gcTrace(gc);
#if defined(VMCFG_FLOAT)
    cpool_float.gcTrace(gc);
#endif
#if defined(VMCFG_FLOAT)
    cpool_float4.gcTrace(gc);
#endif
    cpool_int.gcTrace(gc);
#if !defined(AVMPLUS_64BIT)
    cpool_int_atoms.gcTrace(gc);
#endif
    cpool_mn_offsets.gcTrace(gc);
    cpool_ns.gcTrace(gc);
    cpool_ns_set.gcTrace(gc);
    cpool_uint.gcTrace(gc);
#if !defined(AVMPLUS_64BIT)
    cpool_uint_atoms.gcTrace(gc);
#endif
    gc->TraceLocation(&domain);
    gc->TraceLocation(&m_cachedScripts);
    gc->TraceLocation(&m_cachedTraits);
    gc->TraceLocation(&m_loadedScripts);
    gc->TraceLocation(&m_loadedTraits);
    metadata_infos.gcTrace(gc);
    gc->TraceLocation(&precompNames);
    return false;
}



#ifdef DEBUG
const uint32_t QCache::gcTracePointerOffsets[] = {
    offsetof(QCache, m_head),
    0};

MMgc::GCTracerCheckResult QCache::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool QCache::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&m_head);
    return false;
}



#ifdef DEBUG
const uint32_t QCachedItem::gcTracePointerOffsets[] = {
    offsetof(QCachedItem, next),
    0};

MMgc::GCTracerCheckResult QCachedItem::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool QCachedItem::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&next);
    return false;
}



#ifdef DEBUG
const uint32_t ScopeChain::gcTracePointerOffsets[] = {
    offsetof(ScopeChain, _abcEnv),
    offsetof(ScopeChain, _defaultXmlNamespace),
    offsetof(ScopeChain, _scopeTraits),
    offsetof(ScopeChain, _vtable),
    0};

MMgc::GCTracerCheckResult ScopeChain::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,4);
}
#endif // DEBUG

bool ScopeChain::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&_abcEnv);
    gc->TraceLocation(&_defaultXmlNamespace);
    gc->TraceLocation(&_scopeTraits);
    gc->TraceLocation(&_vtable);
    gc->TraceAtoms((_scopes+0), getSize());
    return false;
}



#ifdef DEBUG
const uint32_t ScopeTypeChain::gcTracePointerOffsets[] = {
    offsetof(ScopeTypeChain, _traits),
    0};

MMgc::GCTracerCheckResult ScopeTypeChain::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ScopeTypeChain::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&_traits);
    gc->TraceLocations((_scopes+0), fullsize);
    return false;
}



#ifdef DEBUG
const uint32_t ScriptEnv::gcTracePointerOffsets[] = {
    offsetof(ScriptEnv, global),
    0};

MMgc::GCTracerCheckResult ScriptEnv::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = MethodEnv::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ScriptEnv::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    MethodEnv::gcTrace(gc, 0);
    (void)(avmplus_MethodEnv_isExactInterlock != 0);
    gc->TraceLocation(&global);
    return false;
}



#ifdef DEBUG
const uint32_t ScriptEnvMap::gcTracePointerOffsets[] = {
    offsetof(ScriptEnvMap, ht),
    0};

MMgc::GCTracerCheckResult ScriptEnvMap::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ht.gcTraceOffsetIsTraced(off - offsetof(ScriptEnvMap,ht))) != MMgc::kOffsetNotFound) {
        return result;
    }
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ScriptEnvMap::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    ht.gcTrace(gc);
    return false;
}


#if defined(DEBUGGER)

#ifdef DEBUG
const uint32_t SourceFile::gcTracePointerOffsets[] = {
    offsetof(SourceFile, functions),
    offsetof(SourceFile, named),
    0};

MMgc::GCTracerCheckResult SourceFile::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = functions.gcTraceOffsetIsTraced(off - offsetof(SourceFile,functions))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = SourceInfo::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool SourceFile::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    SourceInfo::gcTrace(gc, 0);
    (void)(avmplus_SourceInfo_isExactInterlock != 0);
    functions.gcTrace(gc);
    gc->TraceLocation(&named);
    return false;
}

#endif // defined(DEBUGGER)

#if defined(DEBUGGER)

#ifdef DEBUG
MMgc::GCTracerCheckResult SourceInfo::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool SourceInfo::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;

    return false;
}

#endif // defined(DEBUGGER)


#ifdef DEBUG
const uint32_t StackTrace::gcTracePointerOffsets[] = {
    offsetof(StackTrace, stringRep),
    0};

MMgc::GCTracerCheckResult StackTrace::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool StackTrace::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    if (_xact_cursor == 0) {
        gc->TraceLocation(&stringRep);
    }
    const size_t _xact_work_increment = 2000/sizeof(void*);
    const size_t _xact_work_count = depth;
    if (_xact_cursor * _xact_work_increment >= _xact_work_count)
        return false;
    size_t _xact_work = _xact_work_increment;
    bool _xact_more = true;
    if ((_xact_cursor + 1) * _xact_work_increment >= _xact_work_count)
    {
        _xact_work = _xact_work_count - (_xact_cursor * _xact_work_increment);
        _xact_more = false;
    }
    for ( size_t _xact_iter=0 ; _xact_iter < _xact_work; _xact_iter++ )
        elements[+_xact_iter+(_xact_cursor * _xact_work_increment)].gcTrace(gc);
    return _xact_more;
}



#ifdef DEBUG
const uint32_t TextE4XNode::gcTracePointerOffsets[] = {
    offsetof(TextE4XNode, m_value),
    0};

MMgc::GCTracerCheckResult TextE4XNode::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = E4XNode::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool TextE4XNode::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    E4XNode::gcTrace(gc, 0);
    (void)(avmplus_E4XNode_isExactInterlock != 0);
    gc->TraceLocation(&m_value);
    return false;
}



#ifdef DEBUG
const uint32_t Toplevel::gcTracePointerOffsets[] = {
    offsetof(Toplevel, _abcEnv),
    offsetof(Toplevel, _aliasToClassClosureMap),
    offsetof(Toplevel, _booleanClass),
    offsetof(Toplevel, _builtinClasses),
    offsetof(Toplevel, _classClass),
#if defined(VMCFG_FLOAT)
    offsetof(Toplevel, _float4Class),
#endif
#if defined(VMCFG_FLOAT)
    offsetof(Toplevel, _floatClass),
#endif
    offsetof(Toplevel, _functionClass),
    offsetof(Toplevel, _intClass),
    offsetof(Toplevel, _isolateInternedObjects),
    offsetof(Toplevel, _mainEntryPoint),
    offsetof(Toplevel, _namespaceClass),
    offsetof(Toplevel, _numberClass),
    offsetof(Toplevel, _scriptEntryPoints),
    offsetof(Toplevel, _stringClass),
    offsetof(Toplevel, _traitsToAliasMap),
    offsetof(Toplevel, _uintClass),
    offsetof(Toplevel, objectClass),
    0};

MMgc::GCTracerCheckResult Toplevel::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = _scriptEntryPoints.gcTraceOffsetIsTraced(off - offsetof(Toplevel,_scriptEntryPoints))) != MMgc::kOffsetNotFound) {
        return result;
    }
    if((result = _traitsToAliasMap.gcTraceOffsetIsTraced(off - offsetof(Toplevel,_traitsToAliasMap))) != MMgc::kOffsetNotFound) {
        return result;
    }
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,18);
}
#endif // DEBUG

bool Toplevel::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    gc->TraceLocation(&_abcEnv);
    gc->TraceLocation(&_aliasToClassClosureMap);
    gc->TraceLocation(&_booleanClass);
    gc->TraceLocation(&_builtinClasses);
    gc->TraceLocation(&_classClass);
#if defined(VMCFG_FLOAT)
    gc->TraceLocation(&_float4Class);
#endif
#if defined(VMCFG_FLOAT)
    gc->TraceLocation(&_floatClass);
#endif
    gc->TraceLocation(&_functionClass);
    gc->TraceLocation(&_intClass);
    gc->TraceLocation(&_isolateInternedObjects);
    gc->TraceLocation(&_mainEntryPoint);
    gc->TraceLocation(&_namespaceClass);
    gc->TraceLocation(&_numberClass);
    _scriptEntryPoints.gcTrace(gc);
    gc->TraceLocation(&_stringClass);
    _traitsToAliasMap.gcTrace(gc);
    gc->TraceLocation(&_uintClass);
    gc->TraceLocation(&objectClass);
    return false;
}



#ifdef DEBUG
const uint32_t Traits::gcTracePointerOffsets[] = {
#if defined(VMCFG_CACHE_GQCN)
    offsetof(Traits, _fullname),
#endif
    offsetof(Traits, _name),
    offsetof(Traits, _ns),
    offsetof(Traits, base),
    offsetof(Traits, init),
    offsetof(Traits, itraits),
    offsetof(Traits, m_declaringScope),
    offsetof(Traits, m_paramTraits),
    offsetof(Traits, m_secondary_supertypes),
    offsetof(Traits, m_slotDestroyInfo),
    offsetof(Traits, m_supertype_cache),
    offsetof(Traits, m_tbref),
    offsetof(Traits, m_tmref),
    offsetof(Traits, pool),
    offsetof(Traits, protectedNamespace),
    0};

MMgc::GCTracerCheckResult Traits::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = m_slotDestroyInfo.gcTraceOffsetIsTraced(off - offsetof(Traits,m_slotDestroyInfo))) != MMgc::kOffsetNotFound) {
        return result;
    }
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,15);
}
#endif // DEBUG

bool Traits::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#if defined(VMCFG_CACHE_GQCN)
    gc->TraceLocation(&_fullname);
#endif
    gc->TraceLocation(&_name);
    gc->TraceLocation(&_ns);
    gc->TraceLocation(&base);
    gc->TraceLocation(&init);
    gc->TraceLocation(&itraits);
    gc->TraceLocation(&m_declaringScope);
    gc->TraceLocation(&m_paramTraits);
    gc->TraceLocation(&m_secondary_supertypes);
    m_slotDestroyInfo.gcTrace(gc);
    gc->TraceLocation(&m_supertype_cache);
    gc->TraceLocation(&m_tbref);
    gc->TraceLocation(&m_tmref);
    gc->TraceLocation(&pool);
    gc->TraceLocation(&protectedNamespace);
    gc->TraceLocations((m_primary_supertypes+0), MAX_PRIMARY_SUPERTYPE);
    return false;
}



#ifdef DEBUG
const uint32_t TraitsBindings::gcTracePointerOffsets[] = {
    offsetof(TraitsBindings, base),
    offsetof(TraitsBindings, m_bindings),
    offsetof(TraitsBindings, owner),
    0};

MMgc::GCTracerCheckResult TraitsBindings::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = QCachedItem::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,3);
}
#endif // DEBUG

bool TraitsBindings::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    QCachedItem::gcTrace(gc, 0);
    (void)(avmplus_QCachedItem_isExactInterlock != 0);
    gcTraceHook_TraitsBindings(gc);
    gc->TraceLocation(&base);
    gc->TraceLocation(&m_bindings);
    gc->TraceLocation(&owner);
    return false;
}



#ifdef DEBUG
const uint32_t TraitsMetadata::gcTracePointerOffsets[] = {
    offsetof(TraitsMetadata, base),
    offsetof(TraitsMetadata, residingPool),
    0};

MMgc::GCTracerCheckResult TraitsMetadata::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = QCachedItem::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool TraitsMetadata::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    QCachedItem::gcTrace(gc, 0);
    (void)(avmplus_QCachedItem_isExactInterlock != 0);
    gc->TraceLocation(&base);
    gc->TraceLocation(&residingPool);
    return false;
}



#ifdef DEBUG
const uint32_t TypedVectorClassBase::gcTracePointerOffsets[] = {
    offsetof(TypedVectorClassBase, m_typeTraits),
    0};

MMgc::GCTracerCheckResult TypedVectorClassBase::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool TypedVectorClassBase::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    gc->TraceLocation(&m_typeTraits);
    return false;
}



#ifdef DEBUG
const uint32_t VTable::gcTracePointerOffsets[] = {
    offsetof(VTable, _toplevel),
    offsetof(VTable, base),
#if defined(VMCFG_NANOJIT)
    offsetof(VTable, imt),
#endif
    offsetof(VTable, init),
    offsetof(VTable, ivtable),
    offsetof(VTable, traits),
    0};

MMgc::GCTracerCheckResult VTable::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
#if defined(VMCFG_NANOJIT)
    if((result = imt.gcTraceOffsetIsTraced(off - offsetof(VTable,imt))) != MMgc::kOffsetNotFound) {
        return result;
    }
#endif
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,6);
}
#endif // DEBUG

bool VTable::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    if (_xact_cursor == 0) {
        gc->TraceLocation(&_toplevel);
        gc->TraceLocation(&base);
#if defined(VMCFG_NANOJIT)
        imt.gcTrace(gc);
#endif
        gc->TraceLocation(&init);
        gc->TraceLocation(&ivtable);
        gc->TraceLocation(&traits);
    }
    const size_t _xact_work_increment = 2000/sizeof(void*);
    const size_t _xact_work_count = (MMgc::GC::Size(this) - offsetof(VTable, methods)) / sizeof(MethodEnv*);
    if (_xact_cursor * _xact_work_increment >= _xact_work_count)
        return false;
    size_t _xact_work = _xact_work_increment;
    bool _xact_more = true;
    if ((_xact_cursor + 1) * _xact_work_increment >= _xact_work_count)
    {
        _xact_work = _xact_work_count - (_xact_cursor * _xact_work_increment);
        _xact_more = false;
    }
    gc->TraceLocations((methods+(_xact_cursor * _xact_work_increment)), _xact_work);
    return _xact_more;
}



#ifdef DEBUG
const uint32_t VectorBaseObject::gcTracePointerOffsets[] = {
    offsetof(VectorBaseObject, m_vecClass),
    0};

MMgc::GCTracerCheckResult VectorBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool VectorBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    gc->TraceLocation(&m_vecClass);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult WeakKeyHashtable::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = HeapHashtable::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool WeakKeyHashtable::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    HeapHashtable::gcTrace(gc, 0);
    (void)(avmplus_HeapHashtable_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t WeakMethodClosure::gcTracePointerOffsets[] = {
    offsetof(WeakMethodClosure, m_weakCallEnv),
    offsetof(WeakMethodClosure, m_weakSavedThis),
    0};

MMgc::GCTracerCheckResult WeakMethodClosure::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = MethodClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool WeakMethodClosure::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    MethodClosure::gcTrace(gc, 0);
    (void)(avmplus_MethodClosure_isExactInterlock != 0);
    gc->TraceLocation(&m_weakCallEnv);
    gc->TraceLocation(&m_weakSavedThis);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult WeakValueHashtable::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = HeapHashtable::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool WeakValueHashtable::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    HeapHashtable::gcTrace(gc, 0);
    (void)(avmplus_HeapHashtable_isExactInterlock != 0);
    return false;
}


}
