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
        disableAllTracing();
    }

    void Debugger::stepInto()
    {
        stepState.flag = true;
        stepState.depth = -1;

        // Check that core and core->callStack are non-null before dereferencing
        // them, so that it's possible to call stepInto() even before there is
        // a stackframe (so execution will stop at the first executed line of code).
        stepState.startingDepth = (core && core->callStack) ? core->callStack->depth() : 0;
    }

    void Debugger::stepOver()
    {
        stepState.flag = true;
        stepState.depth = core->callStack->depth();
        stepState.startingDepth = core->callStack->depth();
    }

    void Debugger::stepOut()
    {
        stepState.flag = true;
        stepState.depth = core->callStack->depth() - 1;
        stepState.startingDepth = core->callStack->depth();
    }

    void Debugger::stepContinue()
    {
        // Restore the previous stepping state
        stepState = oldStepState;
    }

    /**
     * Set a breakpoint in a particular source flie
     * NOTE: if the 'same' source file appears in
     * multiple abc files, it is up to the caller
     * to ensure that this call is performed for each
     * SourceInfo object.
     */
    bool Debugger::breakpointSet(SourceInfo* source, int linenum)
    {
        return source->setBreakpoint(linenum);
    }

    /**
     * Clear a breakpoint on a particular source file
     */
    bool Debugger::breakpointClear(SourceInfo* source, int linenum)
    {
        return source->clearBreakpoint(linenum);
    }

    void Debugger::debugLine(int linenum)
    {
        AvmAssert( core->callStack !=0 );
        if (!core->callStack)
            return;

        AvmAssert(linenum > 0);

        int prev = core->callStack->linenum();
        core->callStack->set_linenum(linenum);

        int line = linenum;

        // line number has changed
        bool changed = (prev == line) ? false : true;
        bool exited =  (prev == -1) ? true : false; // are we being called as a result of function exit?
        if (!changed && !exited)
            return;  // still on the same line in the same function?

        // tracing information
        if (!exited)
            traceLine(line);

        // check if we should stop due to breakpoint or step
        bool stop = false;
        if (stepState.flag)
        {
            if (stepState.startingDepth != -1 && core->callStack->depth() < stepState.startingDepth)
            {
                // We stepped out of whatever function was executing when the
                // stepInto/stepOver/stepOut command was executed.  We may be
                // in the middle of a line of code, but we still want to stop
                // immediately.  See bug 126633.
                stop = true;
            }
            else if (!exited && (stepState.depth == -1 || core->callStack->depth() <= stepState.depth) )
            {
                // We reached the beginning of a new line of code.
                stop = true;
            }
        }

        // we didn't decide to stop due to a step, but check if we hit a breakpoint
        if (!stop && !exited)
        {
            MethodInfo* f = core->callStack->info();
#ifdef VMCFG_AOT
            if (f && (f->hasMethodBody() || f->isAotCompiled()))
#else
            if (f && f->hasMethodBody())
#endif
            {
                AbcFile* abc = f->file();
                if (abc)
                {
                    SourceFile* source = abc->sourceNamed( core->callStack->filename() );
                    if (source && source->hasBreakpoint(line))
                    {
                        stop = true;
                    }
                }
            }
        }

        // we still haven't decided to stop; check our watchpoints
        if (!stop && !exited)
        {
            if (hitWatchpoint())
                stop = true;
        }

        if (stop)
        {
            // Terminate whatever step operation may have been happening.  But first,
            // save the state of the step, so that if someone calls stepContinue(),
            // then we can restore it.
            StepState oldOldStepState = oldStepState; // save oldStepState in case of reentrancy
            oldStepState = stepState; // save stepState so that stepContinue() can find it
            stepState.clear(); // turn off stepping

            enterDebugger();

            oldStepState = oldOldStepState; // restore oldStepState
        }
    }

    void Debugger::debugFile(Stringp filename)
    {
       AvmAssert( core->callStack != 0 );
       if (!core->callStack)
           return;

       AvmAssert(filename != 0);

       core->callStack->set_filename(filename);
    }

    void Debugger::debugMethod(MethodEnv* /*env*/)
    {
        // nop
    }

    void Debugger::_debugMethod(MethodEnv* env)
    {
        traceMethod(env->method);

        // can't debug native methods
        if (!env->method->isNative()
#ifdef VMCFG_AOT
            || env->method->isAotCompiled()
#endif
            )
            debugMethod(env);
    }

    void Debugger::disableAllTracing()
    {
        in_trace = true;
        astrace_callback = TRACE_OFF;
        astrace_console = TRACE_OFF;
        trace_callback = 0;
        in_trace = false;
    }

    void Debugger::traceMethod(MethodInfo* fnc, bool ignoreArgs)
    {
        if (in_trace) return;
        in_trace = true;

        // callback trace
        if (trace_callback && fnc && astrace_callback > TRACE_OFF)
            traceCallback(0);

        if (astrace_console > TRACE_OFF)
        {
            if (fnc)
            {
                // WARNING: don't change the format of output since outside utils depend on it
                uint64_t delta = VMPI_getTime() - astraceStartTime;
                core->console << (uint32_t)(delta) << " AVMINF: MTHD ";
                Stringp fname = fnc->getMethodName();
                if (fname && (fname->length() > 0) )
                    core->console << fname;
                else
                    core->console << "<unknown>";

                core->console << " (";

                if (!ignoreArgs && core->callStack && (astrace_console == TRACE_METHODS_WITH_ARGS || astrace_console == TRACE_METHODS_AND_LINES_WITH_ARGS))
                    traceArgumentsString(core->console);

                core->console << ")\n";
            }
        }
        in_trace = false;
    }

    void Debugger::traceLine(int line)
    {
        if (in_trace) return;
        in_trace = true;

        // callback trace
        AvmAssert(line != 0);
        if (trace_callback && astrace_callback >= TRACE_METHODS_AND_LINES)
            traceCallback(line);

        // console level trace
        if (astrace_console >= TRACE_METHODS_AND_LINES)
        {
            Stringp file = core->callStack->filename();

            // WARNING: don't change the format of output since outside utils depend on it
            uint64_t delta = VMPI_getTime() - astraceStartTime;
            core->console << (uint32_t)(delta) << " AVMINF: LINE ";
            if (file)
                core->console << "   " << line << "\t\t " << file << "\n";
            else
                core->console << "   " << line << "\t\t ??? \n";
        }
        in_trace = false;
    }

    void Debugger::traceCallback(int line)
    {
        if (!(core->callStack && core->callStack->env()))
            return;

        Stringp file = ( core->callStack->filename() ) ? Stringp(core->callStack->filename()) : Stringp(core->kEmptyString);
        Stringp name = core->kEmptyString;
        Stringp args = core->kEmptyString;

        MethodEnv* env = core->callStack->env();
        if (env->method)
        {
            // normally, getMethodName omits nonpublic namespaces, but for this purpose,
            // we want to include all namespaces.
            const bool includeAllNamespaces = true;
            name = env->method->getMethodName(includeAllNamespaces);
            if (!name)
                name = core->kEmptyString;
            if ((line == 0) && (astrace_callback == TRACE_METHODS_WITH_ARGS || astrace_callback == TRACE_METHODS_AND_LINES_WITH_ARGS)) {
                StringBuffer sb(core);
                traceArgumentsString(sb);
                args = sb.toString();
            }
        }

        Atom argv[5] = { trace_callback->atom(), file->atom(), core->intToAtom(line), name->atom(), args->atom() };
        int argc = 4;

        TRY(core, kCatchAction_ReportAsError)
        {
            trace_callback->call(argc, argv);
        }
        CATCH(Exception *exception)
        {
            (void) exception;
            //core->uncaughtException(exception);
        }
        END_CATCH
        END_TRY
    }

    void Debugger::traceArgumentsString(PrintWriter& prw)
    {
        DebugStackFrame* frame = (DebugStackFrame*)frameAt(0);
        int count;
        Atom* arr;
        if (frame && frame->arguments(arr, count))
        {
            for(int i=0; i<count; i++)
            {
                prw << asAtom(arr[i]);
                if (i+1 < count)
                    prw << ",";
            }
        }
    }

    /**
     * Called when an abc file is first decoded.
     * This method builds a list of source files
     * and methods, etc from the given abc file.
     */
    void Debugger::processAbc(PoolObject* pool, ScriptBuffer code, Stringp abcname)
    {
#ifdef VMCFG_AOT
        if(pool2abcIndex.get(pool) != NULL)
            return;
#endif

        // first off we build an AbcInfo object
        AbcFile* abc = AbcFile::create(core->GetGC(), core, (int)code.getSize());
        abc->setName(abcname);
        
        // now let's scan the abc resources pulling out what we need
        scanResources(abc, pool);

        // build a bridging table from pools to abcs
        uintptr_t index = abcList.length();
        pool2abcIndex.add(pool, (const void*)index);

        // at this point our abc object has been populated with
        // source file objects and should ready to go.
        // so we add it to the list and we are done
        abcList.add(abc);
    }

    /**
     * Scans the pool object and pulls out information about the abc file
     * placing it in the AbcFile
     */
    void Debugger::scanResources(AbcFile* file, PoolObject* pool)
    {
        // walk all methods
        for(uint32_t i=0, n = pool->methodCount(); i<n; i++)
        {
            MethodInfo* f = pool->getMethodInfo(i);
            if (f->hasMethodBody())
            {
                // yes there is code for this method
                if (f->abc_body_pos())
                {
                    // if body_pos is null we havent got the body yet or
                    // this is an interface method
                    scanCode(file, pool, f);
                }
            }
        }
    }

    /**
     * Scans the bytecode and adds source level information to
     * the abc info object
     */
    bool Debugger::scanCode(AbcFile* file, PoolObject* pool, MethodInfo* m)
    {
        const uint8_t *abc_start = &m->pool()->code()[0];

        const uint8_t *pos = m->abc_body_pos();

        m->setFile(file);

        AvmCore::skipU32(pos, 4); // max_stack; local_count; init_stack_depth; max_stack_depth;
        uint32_t code_len = AvmCore::readU32(pos); // checked earlier by AbcParser::parseMethodInfos()

        const uint8_t *start = pos;
        const uint8_t *end = pos + code_len;

        uintptr_t size = 0;
        int op_count;
        SourceFile* active = NULL; // current source file
        for (const uint8_t* pc=start; pc < end; pc += size)
        {
            op_count = opcodeInfo[*pc].operandCount;
            if (op_count == -1 && *pc != OP_lookupswitch)
                return false; // ohh very bad, verifier will catch this

            size = AvmCore::calculateInstructionWidth(pc);

            // if pc+size overflows, we're definitely hosed
            if (uintptr_t(pc) > uintptr_t(-1) - size)
                return false;

            if (pc+size > end)
                return false; // also bad, let the verifier will handle it

            switch (*pc)
            {
                case OP_lookupswitch:
                {
                    // variable length instruction
                    const uint8_t *pc2 = pc+4;
                    // case_count is a U30, so if the high bits are set,
                    // bail and let Verifier throw the error
                    uint32_t const case_count = readU32(pc2);
                    if (case_count & 0xc0000000)
                        return false;
                    // case_count*3 can't overflow a U32...
                    uint32_t const case_skip = (case_count + 1) * 3;
                    // ...but size could, so check to see if it will.
                    if (size > uintptr_t(-1) - case_skip)
                        return false;
                    size += case_skip;
                    break;
                }

                case OP_debug:
                {
                    // form is 8bit type followed by pool entry
                    // then 4Byte extra info
                    int type = (uint8_t)*(pc+1);

                    switch(type)
                    {
                        case DI_LOCAL:
                        {
                            // in this case last word contains
                            // register and line number
                            const uint8_t* pc2 = pc+2;
                            uint32_t index = AvmCore::readU32(pc2);
                            int slot = (uint8_t)*(pc2);
                            //int line = readS24(pc+5);
                            if (index >= pool->constantStringCount)
                                break; // let the verifier handle this


                            //Atom str = pool->cpool[index];
                            Stringp s = pool->getString(index);
                            m->setRegName(slot, s);
                        }
                    }

                    break;
                }

                case OP_debugline:
                {
                    // this means that we have a new source line for the given offset
                    const uint8_t* pc2 = pc+1;
                    int line = readU32(pc2);
                    if (active != NULL)
                        active->addLine(line, m, (int)(pc - abc_start));
                    // else, OP_line occurred before OP_file: fix the compiler!
                    break;
                }

                case OP_debugfile:
                {
                    // new or existing source file
                    const uint8_t* pc2 = pc+1;
                    uint32_t index = AvmCore::readU32(pc2);
                    if (index >= pool->constantStringCount)
                        break; // let the verifier handle this
                    Stringp name = pool->getString(index);
                    active = file->sourceNamed(name);
                    if (active == NULL)
                    {
                        active = SourceFile::create(core->GetGC(), name);
                        file->sourceAdd(active);
                    }
                    break;
                }
            }
        }
        return true;
    }

    /**
     * Returns the call stack depth (i.e. number of frames).
     */
    int Debugger::frameCount()
    {
        const int MAX_FRAMES = 500; // we need a max, for perf. reasons (bug 175526)
        CallStackNode* trace = core->callStack;
        if (trace == NULL)
            return 0;
        int count = 1;
        while( (trace = trace->next()) != 0 && count < MAX_FRAMES )
            count++;
        return count;
    }

    /**
     * Set frame to point to the specified frame number
     * or null if frame number does not exist.
     */
    DebugFrame* Debugger::frameAt(int frameNbr)
    {
        DebugFrame* frame = NULL;

        if (frameNbr >= 0)
        {
            CallStackNode* trace = locateTrace(frameNbr);
            if (trace)
                frame = new (core->GetGC()) DebugStackFrame(frameNbr, trace, this);
        }
        return frame;
    }

    /**
     * Stack frames are labelled 0 to stackTrace->depth
     * With zero being the topmost or most recent frame.
     */
    CallStackNode* Debugger::locateTrace(int frameNbr)
    {
        int count = 0;
        CallStackNode* trace = core->callStack;
        while(count++ < frameNbr && trace != NULL)
            trace = trace->next();

        return trace;
    }

    /**
     * # of abc files available
     */
    int Debugger::abcCount() const
    {
        return abcList.length();
    }


    Atom Debugger::autoAtomAt(DebugFrame* frame, int index, AutoVarKind kind) {
        Atom* arr = NULL;
        Atom dhis;
        int count = 0;
        bool success;
        switch (kind) {
        case AUTO_LOCAL:
            success = frame->locals(arr, count);
            break;

        case AUTO_ARGUMENT:
            success = frame->arguments(arr, count);
            break;
        case AUTO_THIS:
            AvmAssert(index == 0);
            success = frame->dhis(dhis);
            if (success) {
                arr = &dhis;
            }
            break;
        default:
            AvmAssert(false);
            return unreachableAtom;
        }
        if (success) {
            if (index >= 0 && index < count) {
                return arr[index];
            }
        }
        return unreachableAtom;
    }

    // interactive
    Atom Debugger::autoAtomKindAt(DebugFrame* frame, int autoIndex, AutoVarKind kind) {
        if (!frame) return unreachableAtom;
        else return atomKind(autoAtomAt(frame, autoIndex, kind));
    }

    ScriptObject* Debugger::autoVarAsObject(DebugFrame* frame, int index, AutoVarKind kind)
    {
        if (!frame) return NULL; // should have tested for error earlier
        return AvmCore::atomToScriptObject(autoAtomAt(frame, index, kind));
    }

    Stringp Debugger::autoVarAsString(DebugFrame* frame, int index, AutoVarKind kind)
    {
        if (!frame) return NULL; // should have tested for error earlier
        return AvmCore::atomToString(autoAtomAt(frame, index, kind));
    }

    bool Debugger::autoVarAsBoolean(DebugFrame* frame, int index, AutoVarKind kind)
    {
        if (!frame) return false; // should have tested for error earlier
        Atom value = autoAtomAt(frame, index, kind);
        return value == trueAtom ? true : false;
    }

    double Debugger::autoVarAsInteger(DebugFrame* frame, int index, AutoVarKind kind) {
        if (!frame) return MathUtils::kNaN; // should have tested for error earlier
        return AvmCore::number_d(autoAtomAt(frame, index, kind));
    }

    double Debugger::autoVarAsDouble(DebugFrame* frame, int index, AutoVarKind kind) {
        if (!frame) return MathUtils::kNaN; // should have tested for error earlier
        return AvmCore::atomToDouble(autoAtomAt(frame, index, kind));
    }

#ifdef VMCFG_FLOAT
    float Debugger::autoVarAsFloat(DebugFrame* frame, int index, AutoVarKind kind) {
        if (!frame) return (float)MathUtils::kNaN;
        return AvmCore::atomToFloat(autoAtomAt(frame, index, kind));
    }

    float4_t Debugger::autoVarAsFloat4(DebugFrame* frame, int index, AutoVarKind kind) {
        float4_t res;
        if (!frame) {
            res.x = res.y = res.z = res.w = (float)MathUtils::kNaN;
            return res;
        }
        return AvmCore::atomToFloat4(autoAtomAt(frame, index, kind));
    }
#endif

    Stringp Debugger::autoVarName(DebugStackFrame* frame, int index, AutoVarKind kind) {
        if (frame == NULL) return NULL;
        int line;
        SourceInfo* src = NULL;
        // source information
        frame->sourceLocation(src, line);
        MethodInfo* info = functionFor(src, line, frame);
        if (!info) return NULL;
        switch (kind) {
        case AUTO_LOCAL:
            return info->getLocalName(index);
        case AUTO_ARGUMENT:
            return info->getArgName(index);
        case AUTO_THIS:
            return NULL;
        default:
            AvmAssert(false);
        }
        return NULL;
    }

    int Debugger::autoVarCount(DebugStackFrame* frame, AutoVarKind kind) {
        if (frame == NULL) return 0;
        int line, count;
        Atom* ptr;
        SourceInfo* src = NULL;
        // source information
        frame->sourceLocation(src, line);
        MethodInfo* info = functionFor(src, line, frame);
        if (!info) return 0;
        switch (kind) {
        case AUTO_LOCAL:
            return frame->locals(ptr, count) ? count : -1;
        case AUTO_ARGUMENT:
            return frame->arguments(ptr, count) ? count : -1;
        case AUTO_THIS:
            return 1;
        default:
            AvmAssert(false);
        }
        return -1;
    }

     Stringp Debugger::methodNameAt(DebugStackFrame* frame) {
         if (frame == NULL) return NULL;
         int line;
         SourceInfo* src = NULL;
         // source information
         frame->sourceLocation(src, line);
         MethodInfo* info = functionFor(src, line, frame);
         return  info ? info->getMethodName() : NULL;
     }

     void Debugger::printString(Stringp string) {
         StUTF8String buf(string);
         fputs(buf.c_str(), stdout);
         // flush in case debugger output is interleaved with process output
         fflush(stdout);
     }

     void Debugger::printAtom(Atom atom) {
         StringBuffer buf(AvmCore::getActiveCore());
         buf.writeAtom(atom);
         fputs(buf.c_str(), stdout);
         // flush in case debugger output is interleaved with process output
         fflush(stdout);
     }

     void Debugger::printMethod(MethodInfo* info) {
          String* name = info->getMethodName();
          if (!name) return;
          StUTF8String buf(info->getMethodName());
          fputs(buf.c_str(), stdout);
          // flush in case debugger output is interleaved with process output
          fflush(stdout);
     }

     MethodInfo* Debugger::functionFor(SourceInfo* src, int line, DebugStackFrame* frame)
     {
         MethodInfo* info = NULL;
         if (src)
         {
             // find the function at this location
             int size = src->functionCount();
             for(int i=0; i<size; i++)
             {
                 MethodInfo* m = src->functionAt(i);
                 if (line >= m->firstSourceLine() && line <= m->lastSourceLine())
                 {
                      info = m;
                      break;
                 }
             }
         }
         if (!info && frame) {
              // fallback on frame info
              return frame->trace->info();
         }
         return info;
     }

    /**
     * Get information on each of the abc files that
     * have been loaded.
     */
    AbcInfo* Debugger::abcAt(int index) const
    {
        return abcList.get(index);
    }

    /**
     * Contains all known debug information regarding a single
     * abc/swf file
     */
    AbcFile::AbcFile(AvmCore* core, int size)
        : core(core),
          source(core->GetGC(), kListInitialCapacity),
          byteCount(size)
    {
        sourcemap = HeapHashtable::create(core->GetGC());
    }

    int AbcFile::sourceCount() const
    {
        return source.length();
    }

    SourceInfo* AbcFile::sourceAt(int index) const
    {
        return source.get(index);
    }

    int AbcFile::size() const
    {
        return byteCount;
    }

    /**
     * Find a source file with the given name
     */
    SourceFile* AbcFile::sourceNamed(Stringp name)
    {
        Atom atom = sourcemap->get(name->atom());
        if (AvmCore::isUndefined(atom))
            return NULL;
        uint32_t index = AvmCore::integer_u(atom);
        return source.get(index);
    }

    /**
     * Add source file to list; no check for uniqueness
     * of name
     */
    void AbcFile::sourceAdd(SourceFile* s)
    {
        source.add(s);
        sourcemap->add(s->name()->atom(), core->uintToAtom(source.length()-1));
    }

    /**
     * new source info
     */
    SourceFile::SourceFile(MMgc::GC* gc, Stringp name)
        : named(name)
        , functions(gc, kListInitialCapacity)
    {
    }

    Stringp SourceFile::name() const
    {
        return named;
    }

    /**
     * A line - offset pair should be recorded
     */
    void SourceFile::addLine(int linenum, MethodInfo* func, int offset)
    {
        // Add the function to our list if it doesn't exist.  Use lastIndexOf() instead of
        // indexOf(), because this will be faster in the very common case where the function
        // already exists at the end of the list.
        int index = functions.lastIndexOf(func);
        if (index < 0)
        {
            functions.add(func);
            index = functions.length() - 1;
        }

        // line numbers for a given function don't always come in sequential
        // order -- for example, I've seen them come out of order if a function
        // contains an inner anonymous function -- so, update every time we get called
        func->updateSourceLines(linenum, offset);
        sourceLines.set(linenum);
    }

    int SourceFile::functionCount() const
    {
        return functions.length();
    }

    MethodInfo* SourceFile::functionAt(int index) const
    {
        return functions.get(index);
    }

    bool SourceFile::setBreakpoint(int linenum)
    {
        if (!sourceLines.get(linenum))
            return false;
        breakpoints.set(linenum);
        return true;
    }

    bool SourceFile::clearBreakpoint(int linenum)
    {
        if (!breakpoints.get(linenum))
            return false;
        breakpoints.clear(linenum);
        return true;
    }

    bool SourceFile::hasBreakpoint(int linenum)
    {
        return breakpoints.get(linenum);
    }

    bool DebugFrame::methodName(Stringp&) {
        return false; // base implementation
    }

    bool DebugFrame::argumentName(int, Stringp&) {
        return false; // base implementation
    }


    DebugStackFrame::DebugStackFrame(int nbr, CallStackNode* tr, Debugger* debug)
        : trace(tr)
        , debugger(debug)
        , frameNbr(nbr)
    {
        AvmAssert(tr != NULL);
        AvmAssert(debug != NULL);
    }

    /**
     * Identifies the source file and source line number
     * corresponding to this frame
     */
    bool DebugStackFrame::sourceLocation(SourceInfo*& source, int& linenum)
    {
        // use the method info to locate the abcfile / source
        if (trace->info() && trace->filename() && debugger)
        {
            uintptr_t index = (uintptr_t)debugger->pool2abcIndex.get(Atom(trace->info()->pool()));

            AbcFile* abc = (AbcFile*)debugger->abcAt((int)index);
            source = abc->sourceNamed(trace->filename());
        }
        linenum = trace->linenum();

        // valid info?
        return (source != NULL && linenum > 0);
    }

    /**
     * This pointer for the frame
     */
    bool DebugStackFrame::dhis(Atom& a)
    {
        bool worked = false;
        if (trace->framep() && trace->info())
        {
            trace->info()->boxLocals(trace->framep(), 0, trace->types(), &a, 0, 1); // pull framep[0] = [this]
            worked = true;
        }
        else
        {
            a = undefinedAtom;
        }
        return worked;
    }

    bool DebugStackFrame::methodName(Stringp& result) {
        if (trace->info()) {
            //PoolObject* pool = trace->info()->pool();
            //return trace->info()->pool()->getMethodInfoName(trace->info()->method_id());
            result = trace->info()->getMethodName();
            return true;
        } else return false;
    }

    bool DebugStackFrame::argumentName(int which, Stringp& result) {
        if (trace->info()) {
            result = trace->info()->getArgName(which);
            return true;
        } else return false;
    }




    /**
     * @return a pointer to an object Atom whose members are
     * the arguments passed into a function for this frame
     */
    bool DebugStackFrame::arguments(Atom*& ar, int& count)
    {
        bool worked = true;
        if (trace->framep() && trace->info())
        {
            int firstArgument, pastLastArgument;
            argumentBounds(&firstArgument, &pastLastArgument);
            count = pastLastArgument - firstArgument;
            if ((count > 0) && debugger)
            {
                // pull the args into an array -- skip [0] which is [this]
                ar = (Atom*) debugger->core->GetGC()->Calloc(count, sizeof(Atom), GC::kContainsPointers|GC::kZero);
                MethodInfo* info = trace->info();
                info->boxLocals(trace->framep(), firstArgument, trace->types(), ar, 0, count);
            }
        }
        else
        {
            worked = false;
            count = 0;
        }
        return worked;
    }

    bool DebugStackFrame::setArgument(int which, Atom& val)
    {
        bool worked = false;
        if (trace->framep() && trace->info())
        {
            int firstArgument, pastLastArgument;
            argumentBounds(&firstArgument, &pastLastArgument);
            int count = pastLastArgument - firstArgument;
            if (count > 0 && which < count)
            {
                // copy the single arg over
                MethodInfo* info = trace->info();
                info->unboxLocals(&val, 0, trace->types(), trace->framep(), firstArgument+which, 1);
                worked = true;
            }
        }
        return worked;
    }

    /**
     * @return a pointer to an object Atom whose members are
     * the active locals for this frame.
     */
    bool DebugStackFrame::locals(Atom*& ar, int& count)
    {
        bool worked = true;
        if (trace->framep() && trace->info())
        {
            int firstLocal, pastLastLocal;
            localBounds(&firstLocal, &pastLastLocal);
            count = pastLastLocal - firstLocal;
            AvmAssert(count >= 0);
            if ((count > 0) && debugger)
            {
                // frame looks like [this][param0...paramN][local0...localN]
                ar = (Atom*) debugger->core->GetGC()->Calloc(count, sizeof(Atom), GC::kContainsPointers|GC::kZero);
                MethodInfo* info = trace->info();
                info->boxLocals(trace->framep(), firstLocal, trace->types(), ar, 0, count);

                // If abcMethod_NEED_REST or abcMethod_NEED_ARGUMENTS is set, and the jit is being used, then the first
                // local is actually not an atom at all -- it is an ArrayObject*.  So, we need to
                // convert it to an atom.  (If the interpreter is being used instead of the jit, then
                // it is stored as an atom.)
                if (info->needRestOrArguments())
                {
                    int atomType = atomKind(ar[0]);
                    if (atomType == 0) // 0 is not a legal atom type, so ar[0] is not an atom
                    {
                        ScriptObject* obj = (ScriptObject*)ar[0];
                        ar[0] = obj->atom();
                    }
                }
            }
        }
        else
        {
            worked = false;
            count = 0;
        }
        return worked;
    }

    bool DebugStackFrame::setLocal(int which, Atom& val)
    {
        bool worked = false;
        if (trace->framep() && trace->info())
        {
            int firstLocal, pastLastLocal;
            localBounds(&firstLocal, &pastLastLocal);
            int count = pastLastLocal - firstLocal;
            if (count > 0 && which < count)
            {
                MethodInfo* info = trace->info();
                if (which == 0 && info->needRestOrArguments())
                {
                    // They are trying to modify the first local, but that is actually the special
                    // array for "...rest" or for "arguments".  That is too complicated to allow
                    // right now.  We're just going to fail the request.
                }
                else
                {
                    // copy the single arg over
                    info->unboxLocals(&val, 0, trace->types(), trace->framep(), firstLocal+which, 1);
                    worked = true;
                }
            }
        }
        return worked;
    }

    // Returns the indices of the arguments: [firstArgument, pastLastArgument)
    void DebugStackFrame::argumentBounds(int* firstArgument, int* pastLastArgument)
    {
        *firstArgument = 1; // because [0] is 'this'
        *pastLastArgument = indexOfFirstLocal();
    }

    // Returns the indices of the locals: [firstLocal, pastLastLocal)
    void DebugStackFrame::localBounds(int* firstLocal, int* pastLastLocal)
    {
        *firstLocal = indexOfFirstLocal();
        if (trace->framep() && trace->info())
        {
#ifdef VMCFG_AOT
            *pastLastLocal = trace->info()->local_count();
            if (*pastLastLocal < *firstLocal)
                *pastLastLocal = *firstLocal;
#else
            const MethodSignature* ms = trace->info()->getMethodSignature();
            *pastLastLocal = ms->local_count();
#endif
        }
        else
        {
            *pastLastLocal = *firstLocal;
        }
    }

    int DebugStackFrame::indexOfFirstLocal()
    {
        // 'trace->argc' is the number of arguments that were actually passed in
        // to this function, but that is not what we want -- we want
        // 'info->param_count', because that is the number of arguments we were
        // *expecting* to get.  There are two reasons we want 'info->param_count':
        //
        // (1) if the caller passed in too many args, we want to ignore the
        //     trailing ones; and
        // (2) if the caller passed in too few args to a function that has some
        //     default parameters, we want to display the args with their default
        //     values.
        if (!trace->info())
            return 0;
        MethodSignaturep ms = trace->info()->getMethodSignature();
        return 1 + ms->param_count();
    }

}

#endif /* DEBUGGER */
