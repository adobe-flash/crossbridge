/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmshell.h"

#ifdef DEBUGGER
namespace avmshell
{
    /**
     * The array of top level commands that we support.
     * They are placed into a Nx2 array, whereby the first component
     * is a String which is the command and the 2nd component is the
     * integer identifier for the command.
     *
     * NOTE: order matters!  For the case of a single character
     *       match, we let the first hit act like an unambiguous match.
     */
    DebugCLI::StringInt DebugCLI::commandArray[] =
    {
        { "break", CMD_BREAK },
        { "bt", INFO_STACK_CMD },
        { "continue", CMD_CONTINUE },
        { "delete", CMD_DELETE },
        { "finish", CMD_FINISH },
        { "help", CMD_HELP },
        { "?", CMD_HELP },
        { "info", CMD_INFO },
        { "list", CMD_LIST },
        { "next", CMD_NEXT },
        { "print", CMD_PRINT },
        { "quit", CMD_QUIT },
        { "step", CMD_STEP },
        { "set", CMD_SET },
        { "where", INFO_STACK_CMD },
        { NULL, 0 }
    };

    /**
     * Info sub-commands
     */
    DebugCLI::StringInt DebugCLI::infoCommandArray[] =
    {
        { "arguments", INFO_ARGS_CMD },
        { "breakpoints", INFO_BREAK_CMD },
        { "files", INFO_FILES_CMD },
        { "functions", INFO_FUNCTIONS_CMD },
        { "locals", INFO_LOCALS_CMD },
        { "stack", INFO_STACK_CMD },
        { NULL, 0 }
    };

    DebugCLI::DebugCLI(avmplus::AvmCore *core, avmplus::Debugger::TraceLevel tracelevel)
        : Debugger(core, tracelevel)
    {
        currentSourceLen = 0;
        warnMissingSource = true;
    }

    DebugCLI::~DebugCLI()
    {
        if (currentSource) {
            delete [] currentSource;
            currentSource = NULL;
        }
    }

    char* DebugCLI::nextToken()
    {
        char *out = currentToken;
        if (currentToken) {
            while (*currentToken) {
                if (*currentToken == ' ' || *currentToken == '\r' || *currentToken == '\n' || *currentToken == '\t') {
                    *currentToken++ = 0;
                    break;
                }
                currentToken++;
            }
            currentToken = *currentToken ? currentToken : NULL;
        }
        return out;
    }

    /**
     * Attempt to match given the given string against our set of commands
     * @return the command code that was hit.
     */
    int DebugCLI::determineCommand(DebugCLI::StringInt cmdList[],
                                   const char *input,
                                   int defCmd)
    {
        if (!input) return INFO_UNKNOWN_CMD;

        int inputLen = (int)VMPI_strlen(input);

        // first check for a comment
        if (input[0] == '#') {
            return CMD_COMMENT;
        }

        int match = -1;
        bool ambiguous = false;

        for (int i=0; cmdList[i].text; i++) {
            if (!VMPI_strncmp(input, cmdList[i].text, inputLen)) {
                if (match != -1) {
                    ambiguous = true;
                    break;
                }
                match = i;
            }
        }

        /**
         * 3 cases:
         *  - No hits, return unknown and let our caller
         *    dump the error.
         *  - We match unambiguously or we have 1 or more matches
         *    and the input is a single character. We then take the
         *    first hit as our command.
         *  - If we have multiple hits then we dump a 'ambiguous' message
         *    and puke quietly.
         */
        if (match == -1) {
            // no command match return unknown
            return defCmd;
        }
        // only 1 match or our input is 1 character or first match is exact
        else if (!ambiguous || inputLen == 1 || !VMPI_strcmp(cmdList[match].text, input)) {
            return cmdList[match].id;
        }
        else {
            // matches more than one command dump message and go
            core->console << "Ambiguous command '" << input << "': ";
            bool first = true;
            for (int i=0; cmdList[i].text; i++) {
                if (!VMPI_strncmp(cmdList[i].text, input, inputLen)) {
                    if (!first) {
                        core->console << ", ";
                    } else {
                        first = false;
                    }
                    core->console << cmdList[i].text;
                }
            }
            core->console << ".\n";
            return -1;
        }
    }

    const char* DebugCLI::commandNumberToCommandName(DebugCLI::StringInt cmdList[],
                                                     int cmdNumber)
    {
        for (int i = 0; cmdList[i].text; i++)
        {
            if (cmdList[i].id == cmdNumber)
                return cmdList[i].text;
        }

        return "?";
    }

    bool DebugCLI::printFrame(int k)
    {
        avmplus::Atom* ptr;
        int count, line = -1;
        avmplus::SourceInfo* src = NULL;
        avmplus::AvmCore* core = avmplus::AvmCore::getActiveCore();
        avmplus::DebugFrame* frame = core->debugger()->frameAt(k);
        if (frame == NULL) return false;

        // source information
        frame->sourceLocation(src, line);

        core->console << "#" << k << "   ";

        // this
        avmplus::Atom a = avmplus::nullObjectAtom;
        frame->dhis(a);
        core->console << avmplus::asAtom(a) << ".";

        // method
        avmplus::MethodInfo* info = functionFor(src, line);
        if (info) {
            core->console << info->getMethodName();
        } else {
            avmplus::Stringp name = NULL;
            if (frame->methodName(name))
                core->console << name;
            else
                core->console << "???";
        }

        core->console << "(";

        // dump args
        frame->arguments(ptr, count);
        for (int i=0; i<count; i++)
        {
            // write out the name
            if (info)
            {
                avmplus::Stringp nm = info->getArgName(i);
                if (nm != core->kundefined)
                    core->console << nm << "=";
            }

            core->console << avmplus::asAtom(*ptr++);
            if (i<count-1)
                core->console << ",";
        }
        core->console << ") at ";
        if (src)
            core->console << src->name() << ":" << (line) << "\n";
        else
            core->console << "???\n";

        return true;
    }

    void DebugCLI::bt()
    {
        avmplus::AvmCore* core = avmplus::AvmCore::getActiveCore();
        //core->stackTrace->dump(core->console);
        //core->console << '\n';

        // obtain information about each frame
        int frameCount = core->debugger()->frameCount();
        for(int k=0; k<frameCount; k++)
        {
                  printFrame(k);
        }
    }

    void DebugCLI::help()
    {
        StringInt* cmd;

        // "help info" shows sub-commands of the "info" command
        char* next = nextToken();
        if (next && commandFor(next) == CMD_INFO)
            cmd = infoCommandArray;
        else
            cmd = commandArray;

        while (cmd->text)
        {
            core->console << cmd->text;
            if (cmd->id == CMD_INFO)
                core->console << " (see 'help " << cmd->text << "' for sub-commands)";
            core->console << '\n';
            cmd++;
        }
    }

    avmplus::MethodInfo* DebugCLI::functionFor(avmplus::SourceInfo* src, int line)
    {
        avmplus::MethodInfo* info = NULL;
        if (src)
        {
            // find the function at this location
            int size = src->functionCount();
            for(int i=0; i<size; i++)
            {
                avmplus::MethodInfo* m = src->functionAt(i);
                if (line >= m->firstSourceLine() && line <= m->lastSourceLine())
                {
                    info = m;
                    break;
                }
            }
        }
        return info;
    }

    // zero based
    char* DebugCLI::lineStart(int linenum)
    {
        if (!currentSource && currentFile)
            setCurrentSource(currentFile);

        if (!currentSource) {
            return NULL;
        }

        // linenumbers map to zero based array entry
        char *ptr = currentSource;
        for (int i=0; i<linenum; i++) {
            // Scan to end of line
            while (*ptr != '\n') {
                if (!*ptr) {
                    return NULL;
                }
                ptr++;
            }
            // Advance past newline
            ptr++;
        }
        return ptr;
    }

    void DebugCLI::displayLines(int line, int count)
    {
        if (!lineStart(0)) {
            if (currentFile)
                core->console << currentFile;
            else
                core->console << "<unknown>";
            core->console <<":"<<line<<" ";
        } else {
            int lineAt = line;
            while(count-- > 0)
            {
                char* ptr = lineStart(lineAt-1);
                if (!ptr)
                {
                    #if WRAP_AROUND
                    lineAt = 1;
                    count++; // reset line number to beginning skip this iteration
                    #else
                    break;
                    #endif
                }
                else
                {
                    core->console << (lineAt) << ": ";
                    while (*ptr && *ptr != '\n')
                        core->console << *ptr++;
                    core->console << '\n';
                    lineAt++;
                }
            }
        }
    }

    void DebugCLI::list(const char* line)
    {
        int currentLine = (core->callStack) ? core->callStack->linenum() : 0;
        int linenum = (line) ? VMPI_atoi(line) : currentLine;
        displayLines(linenum, 10);
    }

    void DebugCLI::printIP()
    {
        int line = (core->callStack) ? core->callStack->linenum() : 0;
        displayLines(line, 1);
    }

    void DebugCLI::breakpoint(char *location)
    {
        if (!location)
        {
            core->console << "Bad format, should be: 'break [filename:]line'\n";
            return;
        }

        avmplus::Stringp filename = currentFile;
        char *colon = VMPI_strchr(location, ':');

        if (colon) {
            *colon = 0;
            filename = core->internStringLatin1(location);
            location = colon+1;
        }

        if (abcCount() == 0) {
            core->console << "No abc file loaded\n";
            return;
        }

        avmplus::SourceFile* sourceFile = NULL;
        for (int i = 0, n = abcCount(); i < n; ++i)
        {
            avmplus::AbcFile* abcFile = (avmplus::AbcFile*)abcAt(i);
            sourceFile = abcFile->sourceNamed(filename);
            if (sourceFile)
                break;
        }

        if (sourceFile == NULL) {
            core->console << "No source available; can't set breakpoint.\n";
            return;
        }

        int targetLine = VMPI_atoi(location);

        if (breakpointSet(sourceFile, targetLine)) {
            int breakpointId = ++breakpointCount;
            core->console << "Breakpoint " << breakpointId << ": file "
                          << filename
                          << ", " << (targetLine) << ".\n";

            BreakAction *breakAction = new (core->GetGC()) BreakAction(sourceFile,
                                                                  breakpointId,
                                                                  filename,
                                                                  targetLine);
            breakAction->prev = lastBreakAction;
            if (lastBreakAction) {
                lastBreakAction->next = breakAction;
            } else {
                firstBreakAction = breakAction;
            }
            lastBreakAction = breakAction;
        } else {
            core->console << "Could not locate specified line.\n";
        }
    }

    void DebugCLI::showBreakpoints()
    {
        BreakAction *breakAction = firstBreakAction;
        while (breakAction) {
            breakAction->print(core->console);
            breakAction = breakAction->next;
        }
    }

    void DebugCLI::deleteBreakpoint(char *idstr)
    {
        int id = VMPI_atoi(idstr);

        BreakAction *breakAction = firstBreakAction;
        while (breakAction) {
            if (breakAction->id == id) {
                break;
            }
            breakAction = breakAction->next;
        }

        if (breakAction) {
            if (breakAction->prev) {
                breakAction->prev->next = breakAction->next;
            } else {
                firstBreakAction = breakAction->next;
            }
            if (breakAction->next) {
                breakAction->next->prev = breakAction->prev;
            } else {
                lastBreakAction = breakAction->prev;
            }
            if (breakpointClear(breakAction->sourceFile, breakAction->linenum)) {
                core->console << "Breakpoint " << id << " deleted.\n";
            } else {
                core->console << "Internal error; could not delete breakpoint.\n";
            }
        } else {
            core->console << "Could not find breakpoint.\n";
        }
    }

    void DebugCLI::locals(int frameNumber)
    {
        avmplus::Atom* ptr;
        int count, line;
        avmplus::SourceInfo* src = NULL;
        avmplus::AvmCore* core = avmplus::AvmCore::getActiveCore();
        avmplus::DebugFrame* frame = core->debugger()->frameAt(frameNumber);
        if (frame == NULL) return;

        // source information
        frame->sourceLocation(src, line);

        // method
        avmplus::MethodInfo* info = functionFor(src, line);
        if (info) {
            frame->locals(ptr, count);
            for(int i=0; i<count; i++) {
                // write out the name
                avmplus::Stringp nm = info->getLocalName(i);
                core->console << i << ": ";
                if (nm != core->kundefined)
                    core->console << nm;
                else
                    core->console << "<local_" << i << ">";
                core->console << " = " << avmplus::asAtom(*ptr++) << "\n";
            }
        }
    }

    void DebugCLI::arguments(int frameNumber)
    {
         int count;
         avmplus::Atom* arr;
         avmplus::AvmCore* core = avmplus::AvmCore::getActiveCore();
         avmplus::DebugFrame* frame = core->debugger()->frameAt(frameNumber);
         if (frame && frame->arguments(arr, count)) {
             for (int i = 0; i < count; i++) {
                 avmplus::Stringp nm = NULL;
                 frame->argumentName(i, nm);
                 core->console << i << ": " << nm << " = " << avmplus::asAtom(*arr++) << "\n";
             }
         }
    }

    /**
     * Given a pointer, trims leading and trailing whitespace.  Note, this will
     * modify the buffer -- it trims trailing whitespace by writing a null
     * character.
     *
     * Returns the first offset past leading whitespace.
     */
    static char* trim(char* ptr)
    {
        while (*ptr && (*ptr==' ' || *ptr=='\t'))
            ++ptr;
        char* start = ptr;

        while (*ptr)
            ++ptr;

        while (ptr>start && (ptr[-1]==' ' || ptr[-1]=='\t'))
            --ptr;
        *ptr = '\0';

        return start;
    }

    void DebugCLI::set()
    {
        char* line = currentToken;
        char* expr = line;
        char* equ = VMPI_strchr(currentToken, '=');
        char* value = NULL;
        if (equ)
        {
            *equ = '\0';
            value = equ+1;
            expr = trim(expr);
            value = trim(value);
        }

        if (!expr || !*expr || !value || !*value)
        {
            core->console << " Bad format, should be:  'set {variable} = {value}' \n";
        }
        else
        {
            bool sawTrailingDot = false;
            IValue* lvalue = evaluate(expr, &sawTrailingDot);
            IValue* rvalue = evaluate(value, &sawTrailingDot);
            AvmAssert(lvalue != NULL);
            AvmAssert(rvalue != NULL);
            if (!lvalue->isLValue())
                core->console << " Can't assign to " << expr << '\n';
            else
                lvalue->set(rvalue->get());
        }
    }

    class ScopeBuilder : public avmplus::CallStackNode::IScopeChainEnumerator
    {
    public:
        ScopeBuilder(MMgc::GC* gc) : scopeChain(gc, avmplus::kListInitialCapacity) { }
        /*virtual*/ void addScope(avmplus::Atom scope)
        {
            // null/undefined are not legal entries for scope, but
            // enumerateScopeChainAtoms() can hand them to us, for entries
            // within the max_scope range that aren't in use. just ignore 'em.

            if (avmplus::AvmCore::isNullOrUndefined(scope))
                return;

            scopeChain.add(scope);
        }

        avmplus::AtomList scopeChain;
    };

    void DebugCLI::throwUndefinedVarError(const char* name)
    {
        avmplus::Stringp errorMessage = core->newStringLatin1("Error: Variable ");
        errorMessage = errorMessage->appendLatin1(name);
        errorMessage = errorMessage->appendLatin1(" is not defined.");
        core->throwAtom(errorMessage->atom());
    }

    IValue* DebugCLI::getValue(const char *name)
    {
        avmplus::DebugStackFrame* frame = (avmplus::DebugStackFrame*)frameAt(0);
        avmplus::Atom thisAtom;
        frame->dhis(thisAtom);

        avmplus::Stringp namestr = core->internStringLatin1(name, -1);
        if (VMPI_strcmp(name, "this") == 0)
            return new (core->gc) ConstantValue(thisAtom);
        if (VMPI_strcmp(name, "NaN") == 0)
            return new (core->gc) ConstantValue(core->kNaN);
        if (namestr == core->kfalse)
            return new (core->gc) ConstantValue(falseAtom);
        if (namestr == core->ktrue)
            return new (core->gc) ConstantValue(trueAtom);
        if (namestr == core->knull)
            return new (core->gc) ConstantValue(nullObjectAtom);
        if (namestr == core->kundefined)
            return new (core->gc) ConstantValue(undefinedAtom);
        if (name[0] == '-' || VMPI_isdigit(name[0]))
            return new (core->gc) ConstantValue(core->numberAtom(namestr->atom()));
        if (name[0] == '\'' || name[0] == '"') // String literal
        {
            int32_t length = namestr->length();
            if (length >= 2 && namestr->charAt(length-1) == namestr->charAt(0))
            {
                // Note, this doesn't do any escaping
                return new (core->gc) ConstantValue(namestr->substr(1, length-2)->atom());
            }
        }
        if (name[0] == '<') // XML or XMLList literal
        {
            if (avmplus::AvmCore::isObject(thisAtom))
            {
                avmplus::Toplevel* toplevel = avmplus::AvmCore::atomToScriptObject(thisAtom)->toplevel();
                if (name[1] == '>')
                    return new (core->gc) ConstantValue(toplevel->xmlListClass()->ToXMLList(namestr->atom()));
                else
                    return new (core->gc) ConstantValue(toplevel->xmlClass()->ToXML(namestr->atom()));
            }
        }

        avmplus::Atom* ptr;
        int count, line;
        avmplus::SourceInfo* src;

        // See if the name matches a function argument or local
        frame->sourceLocation(src, line);
        if (src)
        {
            avmplus::MethodInfo* info = functionFor(src, line);
            if (info)
            {
                // see if the name matches a function argument
                frame->arguments(ptr, count);
                for(int i=0; i<count; i++)
                {
                    avmplus::Stringp arg = info->getArgName(i);
                    if (arg->equalsLatin1(name))
                    {
                        // match!
                        return new (core->gc) ArgumentValue(frame, i);
                    }
                }

                // see if the name matches a local
                frame->locals(ptr, count);
                for(int i=0; i<count; i++)
                {
                    avmplus::Stringp local = info->getLocalName(i);
                    if ( local->equalsLatin1(name))
                    {
                        // match!
                        return new (core->gc) LocalValue(frame, i);
                    }
                }
            }
        }

        // See if the name matches a property on the scope chain (which includes
        // the 'this' object)
        if (frame->trace)
        {
            avmplus::MethodEnv* env = frame->trace->env();
            if (env)
            {
                ScopeBuilder scopeBuilder(core->GetGC());
                frame->trace->enumerateScopeChainAtoms(scopeBuilder);

                for (uint32_t i=0, n=scopeBuilder.scopeChain.length(); i<n; ++i)
                {
                    StPropertyFinder finder(core, scopeBuilder.scopeChain.get(i), name);
                    finder.iterate();
                    if (finder.value)
                        return finder.value;
                    }
                }
            }

        // Look for globals like 'Number'
        avmplus::MethodEnv* env = frame->trace->env();
        if (env)
        {
            avmplus::Multiname mn(core->getAnyPublicNamespace(),
                         core->internStringLatin1(name));
            avmplus::ScriptEnv* script = core->domainMgr()->findScriptEnvInDomainEnvByMultiname(env->domainEnv(), mn);
            if (script != (avmplus::ScriptEnv*)avmplus::BIND_NONE && script != (avmplus::ScriptEnv*)avmplus::BIND_AMBIGUOUS)
            {
                avmplus::ScriptObject* global = script->global;
                if (global)
                {
                    return new (core->gc) PropertyValue(global, mn);
                }
        }
    }

        throwUndefinedVarError(name);
        return NULL; // unreachable
    }

    /**
     * Accepts expressions of these forms:
     *      foo
     *      foo.bar
     *      foo.bar.baz
     *
     * Also allows literals: boolean, string, numeric, XML/XMLList,
     * null, undefined.
     *
     * If the expression ends with a dot, e.g. "foo.bar.", then
     * *outSawTrailingDot is set to true.  This is used by the
     * "print" command to allow "print foo." to print all members
     * of variable "foo".
     */
    IValue* DebugCLI::evaluate(char *expr, bool* outSawTrailingDot)
    {
        const char *name;
        IValue* value = NULL;
        bool firstPart = true;

        *outSawTrailingDot = false;

        while (expr)
        {
            name = expr;
            char *dot = VMPI_strchr(expr, '.');
            if (dot)
            {
                *dot = '\0';
                name = expr;
                expr = dot+1;
            }
            else
            {
                name = expr;
                expr = NULL;
        }

            if (firstPart)
            {
                value = getValue(name);
                AvmAssert(value != NULL);
            }
            else
            {
                if (*name)
                {
                    // "foo.bar" -- find property "bar" of object "foo"
                    avmplus::Atom parent = value->get();
                    StPropertyFinder finder(core, parent, name);
                    finder.iterate();
                    value = finder.value;
                    if (!value)
                    {
                        if (avmplus::AvmCore::isObject(parent))
                        {
                            // Since we didn't find an existing property, we'll just construct
                            // one.  If the parent object is dynamic, then calling get() on the
                            // PropertyVaulue we create will return undefined, and calling set()
                            // on it will define a new property.  If the parent object is not
                            // dynamic, then get() and set() will throw exceptions.  Either way,
                            // that's the correct behavior.
                            avmplus::Multiname mn(core->getAnyPublicNamespace(),
                                         core->internStringLatin1(name));
                            value = new (core->gc) PropertyValue(avmplus::AvmCore::atomToScriptObject(parent), mn);
                        }
                        else
                        {
                            if (dot) *dot = '.'; // restore for our caller
                            throwUndefinedVarError(name);
                            return NULL; // unreachable
                        }
                    }
                }
                else
                {
                    // "foo."
                    *outSawTrailingDot = true;
                }
            }

            if (dot) *dot = '.'; // restore for our caller
            firstPart = false;
        }

        return value;
    }

    /**
     * Accepts expressions of these forms:
     *      foo
     *      foo.bar
     *      foo.bar.baz
     * Also, allows a trailing dot, e.g. "foo.bar.", in which case all properties
     * of foo.bar will be displayed.
     */
    void DebugCLI::print(char *expr)
    {
        bool sawTrailingDot = false;
        IValue* value = evaluate(expr, &sawTrailingDot);
        AvmAssert(value != NULL);

        if (sawTrailingDot)
        {
            // "foo." -- print all of object foo's properties
            PropertyPrinter printer(core, this, value->get());
            printer.iterate();
        }
        else
        {
            core->console << formatValue(value->get()) << '\n';
        }
    }

    bool DebugCLI::filterException(avmplus::Exception *exception, bool /*willBeCaught*/)
    {
        // Filter exceptions when -d switch specified
        if (activeFlag) {
            core->console << "Exception has been thrown:\n"
                          << core->string(exception->atom)
                          << '\n';
            enterDebugger();
            return true;
        }
        return false;
    }

    void DebugCLI::info()
    {
        char *command = nextToken();
        int cmd = infoCommandFor(command);

        switch (cmd) {
        case -1:
            // ambiguous, we already printed error message
            break;
        case INFO_ARGS_CMD:
            arguments(0);
            break;
        case INFO_LOCALS_CMD:
            locals(0);
            break;
        case INFO_BREAK_CMD:
            showBreakpoints();
            break;
        case INFO_UNKNOWN_CMD:
            core->console << "Unknown command.\n";
            break;
        default:
            core->console << "Command not implemented.\n";
            break;
        }
    }

    bool DebugCLI::debuggerInterruptOnEnter = false;
    void DebugCLI::enterDebugger()
    {
        setCurrentSource( (core->callStack) ? (core->callStack->filename()) : 0 );
        if (debuggerInterruptOnEnter) {
            VMPI_debugBreak();
            return;
        }

        for (;;) {
            printIP();

            core->console << "(asdb) ";
            if (Platform::GetInstance()->getUserInput(commandLine, kMaxCommandLine) == NULL)
                Platform::GetInstance()->exit(0);

            commandLine[VMPI_strlen(commandLine)-1] = 0;

            if (!commandLine[0]) {
                VMPI_strcpy(commandLine, lastCommand);
            } else {
                VMPI_strcpy(lastCommand, commandLine);
            }

            currentToken = commandLine;

            char *command = nextToken();

            TRY(core, avmplus::kCatchAction_Ignore)
            {
            int cmd = commandFor(command);
            switch (cmd) {
            case -1:
                // ambiguous, we already printed error message
                break;
            case CMD_COMMENT:
                break;
            case CMD_INFO:
                info();
                break;
            case CMD_BREAK:
                breakpoint(nextToken());
                break;
            case CMD_DELETE:
                deleteBreakpoint(nextToken());
                break;
            case CMD_LIST:
                list(nextToken());
                break;
            case CMD_UNKNOWN:
                core->console << "Unknown command.\n";
                break;
            case CMD_QUIT:
                Platform::GetInstance()->exit(0);
                break;
            case CMD_CONTINUE:
                return;
            case CMD_PRINT:
                print(nextToken());
                break;
            case CMD_NEXT:
                stepOver();
                return;
            case INFO_STACK_CMD:
                bt();
                break;
            case CMD_FINISH:
                stepOut();
                return;
            case CMD_HELP:
                help();
                break;
            case CMD_STEP:
                stepInto();
                return;
            case CMD_SET:
                set();
                break;
            default:
                core->console << "Command not implemented.\n";
                break;
            }
            }
            CATCH(avmplus::Exception *exception)
            {
                core->console << core->string(exception->atom) << '\n';
            }
            END_CATCH
            END_TRY
        }
    }

    void DebugCLI::setCurrentSource(avmplus::Stringp file)
    {
        if (!file)
            return;

        currentFile = file;

        if (currentSource) {
            delete [] currentSource;
            currentSource = NULL;
            currentSourceLen = 0;
        }

        // Open this file and suck it into memory
        avmplus::StUTF8String currentFileUTF8(currentFile);
        FileInputStream f(currentFileUTF8.c_str());
        if (f.valid() && ((uint64_t)file->length() < UINT32_T_MAX)) { //cannot handle files > 4GB
            currentSourceLen = (uint32_t) f.available();
            currentSource = new char[currentSourceLen+1];
            f.read(currentSource, currentSourceLen);
            currentSource[currentSourceLen] = 0;

            // whip through converting \r\n to space \n
            for(int64_t i=0; i<currentSourceLen-1;i++) {
                if (currentSource[i] == '\r' && currentSource[i+1] == '\n')
                    currentSource[i] = ' ';
            }
        } else if (warnMissingSource) {
            core->console << "Could not find '" << currentFile << "'.  Try running in the same directory as the .as file.\n";
            warnMissingSource = false;
        }
    }

    avmplus::Stringp DebugCLI::formatValue(avmplus::Atom value)
    {
// An experiment: Printing XML and XMLList variables with their full toXMLString() form.
//        if (core->isXMLorXMLList(value))
//        {
//            ScriptObject* object = AvmCore::atomToScriptObject(value);
//            Multiname mn(core->getAnyPublicNamespace(), core->internStringLatin1("toXMLString"));
//            Atom thisAtom = undefinedAtom;
//            return core->string(object->toplevel()->callproperty(value, &mn, 1, &thisAtom, object->vtable));
//        }

        avmplus::StringBuffer sb(core);
        sb << avmplus::asAtom(value);
        return sb.toString();
    }

    //
    // BreakAction
    //

    void BreakAction::print(avmplus::PrintWriter& out)
    {
        out << id << " at "
            << filename
            << ":" << (linenum) << '\n';
    }

    PropertyIterator::PropertyIterator(avmplus::AvmCore* core, avmplus::Atom atom)
    : core(core)
    {
        if (avmplus::AvmCore::isObject(atom))
            object = avmplus::AvmCore::atomToScriptObject(atom);
        else
            object = NULL;
    }

    void PropertyIterator::iterate()
    {
        if (!object)
            return;

        // Iterate over the object's traits
        avmplus::Traits* t = object->traits();
        while (t)
        {
            const avmplus::TraitsBindings* bindings = t->getTraitsBindings();
            avmplus::StTraitsBindingsIterator iter(bindings);
            while (iter.next()) {
                avmplus::Stringp key = iter.key();
                if (!key)
                    continue;
                avmplus::Binding b = iter.value();
                avmplus::Multiname mn(iter.ns(), key);
                if (!process(&mn, avmplus::AvmCore::bindingKind(b)))
                    return;
            }
            t = t->base;
        }

        // Iterate over the object's dynamic properties
        int index = 0;
        while ((index = object->nextNameIndex(index)) != 0)
        {
            avmplus::Multiname mn(core->getAnyPublicNamespace(), core->string(object->nextName(index)));
            if (!process(&mn, avmplus::BKIND_VAR))
                return;
        }
    }

    StPropertyFinder::StPropertyFinder(avmplus::AvmCore* core, avmplus::Atom atom, const char* propertyname)
    : PropertyIterator(core, atom),
      value(NULL),
      propertyname(propertyname)
    {
    }

    bool StPropertyFinder::process(avmplus::Multiname* key, avmplus::BindingKind /*bk*/)
    {
        if (key->getName()->equalsLatin1(propertyname))
        {
            value = new (core->gc) PropertyValue(object, *key);
            return false; // stop iterating
        }

        return true;
    }

    PropertyPrinter::PropertyPrinter(avmplus::AvmCore* core, DebugCLI* debugger, avmplus::Atom atom)
    : PropertyIterator(core, atom),
      debugger(debugger)
    {
    }

    bool PropertyPrinter::process(avmplus::Multiname* key, avmplus::BindingKind bk)
    {
        if (bk == avmplus::BKIND_CONST || bk == avmplus::BKIND_VAR || bk == avmplus::BKIND_GET || bk == avmplus::BKIND_GETSET)
        {
            avmplus::Atom value = object->toplevel()->getproperty(object->atom(), key, object->vtable);
            switch (bk)
            {
            case avmplus::BKIND_CONST:
                core->console << "const ";
                break;
            case avmplus::BKIND_VAR:
                core->console << "var ";
                break;
            case avmplus::BKIND_GET:
            case avmplus::BKIND_GETSET:
                core->console << "function get ";
                break;
            }
            core->console << avmplus::Multiname::FormatNameOnly(key);
            if (bk == avmplus::BKIND_GET || bk == avmplus::BKIND_GETSET)
                core->console << "()";
            core->console << " = " << debugger->formatValue(value) << '\n';
        }

        return true;
    }
}
#endif
