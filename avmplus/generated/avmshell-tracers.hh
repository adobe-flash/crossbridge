/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* machine generated file via utils/exactgc.as -- do not edit */

namespace avmshell
{

#ifdef DEBUG
MMgc::GCTracerCheckResult AbstractBaseClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool AbstractBaseClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_AbstractBaseClass
    m_slots_AbstractBaseClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult AbstractBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool AbstractBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_AbstractBaseObject
    m_slots_AbstractBaseObject.gcTracePrivateProperties(gc);
#endif
    avmplus::ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult AbstractRestrictedBaseClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool AbstractRestrictedBaseClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_AbstractRestrictedBaseClass
    m_slots_AbstractRestrictedBaseClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult AbstractRestrictedBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool AbstractRestrictedBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_AbstractRestrictedBaseObject
    m_slots_AbstractRestrictedBaseObject.gcTracePrivateProperties(gc);
#endif
    avmplus::ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult CheckBaseClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool CheckBaseClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_CheckBaseClass
    m_slots_CheckBaseClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult CheckBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool CheckBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_CheckBaseObject
    m_slots_CheckBaseObject.gcTracePrivateProperties(gc);
#endif
    avmplus::ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeBaseClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeBaseClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NativeBaseClass
    m_slots_NativeBaseClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NativeBaseObject
    m_slots_NativeBaseObject.gcTracePrivateProperties(gc);
#endif
    avmplus::ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeSubclassOfAbstractBaseClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeSubclassOfAbstractBaseClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NativeSubclassOfAbstractBaseClass
    m_slots_NativeSubclassOfAbstractBaseClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeSubclassOfAbstractBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = AbstractBaseObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeSubclassOfAbstractBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NativeSubclassOfAbstractBaseObject
    m_slots_NativeSubclassOfAbstractBaseObject.gcTracePrivateProperties(gc);
#endif
    AbstractBaseObject::gcTrace(gc, 0);
    (void)(avmshell_AbstractBaseObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeSubclassOfAbstractRestrictedBaseClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeSubclassOfAbstractRestrictedBaseClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NativeSubclassOfAbstractRestrictedBaseClass
    m_slots_NativeSubclassOfAbstractRestrictedBaseClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeSubclassOfAbstractRestrictedBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = AbstractRestrictedBaseObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeSubclassOfAbstractRestrictedBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NativeSubclassOfAbstractRestrictedBaseObject
    m_slots_NativeSubclassOfAbstractRestrictedBaseObject.gcTracePrivateProperties(gc);
#endif
    AbstractRestrictedBaseObject::gcTrace(gc, 0);
    (void)(avmshell_AbstractRestrictedBaseObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeSubclassOfRestrictedBaseClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeSubclassOfRestrictedBaseClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NativeSubclassOfRestrictedBaseClass
    m_slots_NativeSubclassOfRestrictedBaseClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult NativeSubclassOfRestrictedBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = RestrictedBaseObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool NativeSubclassOfRestrictedBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_NativeSubclassOfRestrictedBaseObject
    m_slots_NativeSubclassOfRestrictedBaseObject.gcTracePrivateProperties(gc);
#endif
    RestrictedBaseObject::gcTrace(gc, 0);
    (void)(avmshell_RestrictedBaseObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult RestrictedBaseClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool RestrictedBaseClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_RestrictedBaseClass
    m_slots_RestrictedBaseClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult RestrictedBaseObject::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ScriptObject::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool RestrictedBaseObject::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_RestrictedBaseObject
    m_slots_RestrictedBaseObject.gcTracePrivateProperties(gc);
#endif
    avmplus::ScriptObject::gcTrace(gc, 0);
    (void)(avmplus_ScriptObject_isExactInterlock != 0);
    return false;
}



#ifdef DEBUG
MMgc::GCTracerCheckResult SystemClass::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::ClassClosure::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::kOffsetNotFound;
}
#endif // DEBUG

bool SystemClass::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
#ifndef GC_TRIVIAL_TRACER_SystemClass
    m_slots_SystemClass.gcTracePrivateProperties(gc);
#endif
    avmplus::ClassClosure::gcTrace(gc, 0);
    (void)(avmplus_ClassClosure_isExactInterlock != 0);
    return false;
}


#if defined(DEBUGGER)

#ifdef DEBUG
const uint32_t DebugCLI::gcTracePointerOffsets[] = {
    offsetof(DebugCLI, currentFile),
    offsetof(DebugCLI, firstBreakAction),
    offsetof(DebugCLI, lastBreakAction),
    0};

MMgc::GCTracerCheckResult DebugCLI::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::Debugger::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,3);
}
#endif // DEBUG

bool DebugCLI::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    avmplus::Debugger::gcTrace(gc, 0);
    (void)(avmplus_Debugger_isExactInterlock != 0);
    gc->TraceLocation(&currentFile);
    gc->TraceLocation(&firstBreakAction);
    gc->TraceLocation(&lastBreakAction);
    return false;
}

#endif // defined(DEBUGGER)


#ifdef DEBUG
const uint32_t ShellToplevel::gcTracePointerOffsets[] = {
    offsetof(ShellToplevel, shellClasses),
    0};

MMgc::GCTracerCheckResult ShellToplevel::gcTraceOffsetIsTraced(uint32_t off) const
{
    MMgc::GCTracerCheckResult result;
    (void)off;
    (void)result;
    if((result = avmplus::Toplevel::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound)
        return result;
    return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,1);
}
#endif // DEBUG

bool ShellToplevel::gcTrace(MMgc::GC* gc, size_t _xact_cursor)
{
    (void)gc;
    (void)_xact_cursor;
    avmplus::Toplevel::gcTrace(gc, 0);
    (void)(avmplus_Toplevel_isExactInterlock != 0);
    gc->TraceLocation(&shellClasses);
    return false;
}


}
