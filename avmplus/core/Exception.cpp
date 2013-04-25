/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

#if defined(AVMPLUS_AMD64) && defined(_WIN64)
    extern "C"
    {
        _int64 __cdecl longjmp64(jmp_buf jmpbuf, _int64 arg);
    }
#endif

#if (defined(AVMPLUS_MAC) || defined(linux))  && defined(DEBUG)
// definition in PosixPortUtils.cpp
void assertSignalMask(uint32_t expected);
uint32_t querySignalMask();
#endif


namespace avmplus
{
    //
    // Exception
    //

    Exception::Exception(AvmCore* core, Atom atom)
    {
        this->atom = atom;
        this->flags = 0;

        #ifdef DEBUGGER
        // If the exception atom is an Error object, copy its stack trace.
        // Otherwise, generate a new stack trace.
        if (AvmCore::istype(atom, core->traits.error_itraits))
        {
            stackTrace = ((ErrorObject*)AvmCore::atomToScriptObject(atom))->getStackTraceObject();
        }
        else
        {
            stackTrace = core->newStackTrace();
        }
        #else
        (void)core;
        #endif
    }

    bool Exception::isValid()
    {
        return atomKind(atom) == kObjectType;
    }

    //
    // ExceptionHandlerTable
    //

    ExceptionHandlerTable::ExceptionHandlerTable(int exception_count)
        : exception_count(exception_count)
    {
    }

    //
    // ExceptionFrame
    //

    void ExceptionFrame::beginTry(AvmCore* core)
    {
#ifdef _DEBUG
        core->tryHook();
#endif

        this->core = core;

        prevFrame = core->exceptionFrame;

        if (!prevFrame) {
            // Do special setup for first frame
            core->setStackBase();
        }

        core->exceptionFrame = this;

#ifdef DEBUGGER
        callStack = core->callStack;
#endif /* DEBUGGER */

        // beginTry() is called from both the TRY macro and from JIT'd code.  The TRY
        // macro will immediately change the value of catchAction right after the
        // call to beginTry(); but the JIT'd code does not change catchAction.  So,
        // we initialize catchAction to the value that it needs when we're called
        // from JIT'd code, that is, kCatchAction_SearchForActionScriptExceptionHandler.
        catchAction = kCatchAction_SearchForActionScriptExceptionHandler;

        this->stacktop = core->gc->allocaTop();

        savedMethodFrame = core->currentMethodFrame;
#ifdef VMCFG_AOT
        this->llvmUnwindStyle = 0;
#endif

#if (defined(AVMPLUS_MAC) || defined(linux))  && defined(DEBUG)
        this->contextExtra = querySignalMask();
#endif
    }

#ifdef VMCFG_AOT
    void ExceptionFrame::beginLlvmUnwindTry(AvmCore *core)
    {
        beginTry(core);
        this->llvmUnwindStyle = 1;
    }
#endif



    void ExceptionFrame::endTry()
    {
        if (core) {
            // ISSUE do we need to check core if it is set in constructor?
            core->exceptionFrame = prevFrame;

            core->gc->allocaPopTo(this->stacktop);

            core->currentMethodFrame = savedMethodFrame;
        }
    }

    void ExceptionFrame::throwException(Exception *exception)
    {
        core->exceptionAddr = exception;
#ifdef VMCFG_AOT
        if (this->llvmUnwindStyle) {
            llvm_unwind();
            return; // Not reached.
        }
#endif

        VMPI_longjmpNoUnwind(jmpbuf, 1);
        // Not reached.
    }

    void ExceptionFrame::beginCatch()
    {
        core->exceptionFrame = prevFrame;

#if (defined(AVMPLUS_MAC) || defined(linux))  && defined(DEBUG)
        assertSignalMask(this->contextExtra);
#endif


#ifdef DEBUGGER
        core->callStack = callStack;
#endif // DEBUGGER

        core->currentMethodFrame = savedMethodFrame;

        core->gc->allocaPopTo(this->stacktop);

        // Extension hook for host.  For example, it can query the
        // stack depth to determine when an attempt to recover from
        // a stack overflow has succeeded.
        core->catchHook(catchAction);
    }
}
