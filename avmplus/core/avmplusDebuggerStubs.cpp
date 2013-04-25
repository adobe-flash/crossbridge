/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

#ifdef DEBUGGER

namespace avmplus
{
    using namespace MMgc;

    Debugger::Debugger(AvmCore *core, TraceLevel tracelevel)
        : astrace_console(tracelevel)
        , astrace_callback(TRACE_OFF)
        , in_trace(false)
        , astraceStartTime(VMPI_getTime())
        , core(core)
        , abcList(core->GetGC(), kListInitialCapacity)
        , pool2abcIndex()
    {
    }

    Debugger::~Debugger()
    {
    }

    void Debugger::stepInto()
    {
    }

    void Debugger::stepOver()
    {
    }

    void Debugger::stepOut()
    {
    }

    void Debugger::stepContinue()
    {
    }

    bool Debugger::breakpointSet(SourceInfo* source, int linenum)
    {
        return false;
    }

    bool Debugger::breakpointClear(SourceInfo* source, int linenum)
    {
        return false;
    }

    void Debugger::debugLine(int linenum)
    {
    }

    void Debugger::debugFile(Stringp filename)
    {
    }

    void Debugger::debugMethod(MethodEnv* /*env*/)
    {
    }

    void Debugger::_debugMethod(MethodEnv* env)
    {
    }

    void Debugger::disableAllTracing()
    {
    }

    void Debugger::traceMethod(MethodInfo* fnc, bool ignoreArgs)
    {
    }

    void Debugger::traceLine(int line)
    {
    }

    void Debugger::processAbc(PoolObject* pool, ScriptBuffer code, Stringp abcname)
    {
    }

    int Debugger::frameCount()
    {
        return 0;
    }

    DebugFrame* Debugger::frameAt(int frameNbr)
    {
        return NULL;
    }

    int Debugger::abcCount() const
    {
        return 0;
    }

    AbcInfo* Debugger::abcAt(int index) const
    {
        return NULL;
    }

    AbcFile::AbcFile(AvmCore* core, int size)
        : core(core),
          source(core->GetGC(), kListInitialCapacity),
          byteCount(size)
    {
        sourcemap = HeapHashtable::create(core->GetGC());
    }

    int AbcFile::sourceCount() const
    {
        return 0;
    }

    SourceInfo* AbcFile::sourceAt(int index) const
    {
        return NULL;
    }

    int AbcFile::size() const
    {
        return 0;
    }

    SourceFile* AbcFile::sourceNamed(Stringp name)
    {
        return NULL;
    }

    void AbcFile::sourceAdd(SourceFile* s)
    {
    }

    SourceFile::SourceFile(MMgc::GC* gc, Stringp name)
        : named(name)
        , functions(gc, kListInitialCapacity)
    {
    }

    Stringp SourceFile::name() const
    {
        return NULL;
    }

    void SourceFile::addLine(int linenum, MethodInfo* func, int offset)
    {
    }

    int SourceFile::functionCount() const
    {
        return 0;
    }

    MethodInfo* SourceFile::functionAt(int index) const
    {
        return NULL;
    }

    bool SourceFile::setBreakpoint(int linenum)
    {
        return false;
    }

    bool SourceFile::clearBreakpoint(int linenum)
    {
        return false;
    }

    bool SourceFile::hasBreakpoint(int linenum)
    {
        return false;
    }

    bool DebugFrame::methodName(Stringp&)
    {
        return false;
    }

    bool DebugFrame::argumentName(int, Stringp&)
    {
        return false;
    }

    DebugStackFrame::DebugStackFrame(int nbr, CallStackNode* tr, Debugger* debug) : trace(NULL)
    {
    }

    bool DebugStackFrame::sourceLocation(SourceInfo*& source, int& linenum)
    {
        return false;
    }

    bool DebugStackFrame::dhis(Atom& a)
    {
        return false;
    }

    bool DebugStackFrame::methodName(Stringp& result) {
        return false;
    }

    bool DebugStackFrame::argumentName(int which, Stringp& result) {
        return false;
    }

    bool DebugStackFrame::arguments(Atom*& ar, int& count)
    {
        return false;
    }

    bool DebugStackFrame::setArgument(int which, Atom& val)
    {
        return false;
    }

    bool DebugStackFrame::locals(Atom*& ar, int& count)
    {
        return false;
    }

    bool DebugStackFrame::setLocal(int which, Atom& val)
    {
       return false;
    }
}

#endif /* DEBUGGER */
