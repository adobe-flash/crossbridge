/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* machine generated file via utils/exactgc.as -- do not edit */

namespace avmplus
{

#ifdef DEBUG
MMgc::GCTracerCheckResult DomainClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool DomainClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DomainClass
    m_slots_DomainClass.gcTracePrivateProperties(gc);
#endif
    ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
const uint32_t DomainObject::gcTracePointerOffsets[] = {
    offsetof(DomainObject, domainEnv),
    offsetof(DomainObject, domainToplevel),
    0};

MMgc::GCTracerCheckResult DomainObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,2);
}
#endif // DEBUG

bool DomainObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_DomainObject
    m_slots_DomainObject.gcTracePrivateProperties(gc);
#endif
    ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    gc->TraceLocation(&domainEnv);
    gc->TraceLocation(&domainToplevel);
    return false;
}


}
