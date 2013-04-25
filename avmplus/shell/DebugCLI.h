/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_AvmDebugCLI__
#define __avmshell_AvmDebugCLI__

#ifdef DEBUGGER
namespace avmshell
{
    /**
     * Represents a single breakpoint in the Debugger.
     */
    class BreakAction : public MMgc::GCObject
    {
    public:
        GCMember<BreakAction> prev;
        GCMember<BreakAction> next;
        GCMember<avmplus::SourceFile> sourceFile;
        int id;
        GCMember<avmplus::String> filename;
        int linenum;

        BreakAction(avmplus::SourceFile *sourceFile,
                    int id,
                    avmplus::Stringp filename,
                    int linenum)
        {
            this->sourceFile = sourceFile;
            this->id = id;
            this->filename = filename;
            this->linenum = linenum;
        }

        void print(avmplus::PrintWriter& out);
    };

    /**
     * This can be either an l-value or an r-value.
     */
    class IValue : public MMgc::GCFinalizedObject
    {
    public:
        virtual ~IValue() {}

        virtual bool isLValue() = 0;
        virtual avmplus::Atom get() = 0;
        virtual void set(avmplus::Atom newValue) = 0;
    };

    class ConstantValue: public IValue
    {
    public:
        ConstantValue(avmplus::Atom value) : value(value) { }

        virtual bool isLValue() { return false; }
        virtual avmplus::Atom get() { return value; }
        virtual void set(avmplus::Atom /*newValue*/) { AvmAssert(false); }

    private:
        avmplus::AtomWB value;
    };

    /**
     * An IValue representing a local variable.
     */
    class LocalValue: public IValue
    {
    public:
        LocalValue(avmplus::DebugFrame* frame, int index) : frame(frame), index(index) { }

        virtual bool isLValue() { return true; }
        virtual avmplus::Atom get()
        {
            avmplus::Atom* locals;
            int countLocals;
            frame->locals(locals, countLocals);
            return locals[index];
        }
        virtual void set(avmplus::Atom newValue)
        {
            frame->setLocal(index, newValue);
        }

    private:
        GCMember<avmplus::DebugFrame> frame;
        int index;
    };

    /**
     * An IValue representing an argument to a function.
     */
    class ArgumentValue: public IValue
    {
    public:
        ArgumentValue(avmplus::DebugFrame* frame, int index) : frame(frame), index(index) { }

        virtual bool isLValue() { return true; }
        virtual avmplus::Atom get()
        {
            avmplus::Atom* arguments;
            int countArguments;
            frame->arguments(arguments, countArguments);
            return arguments[index];
        }
        virtual void set(avmplus::Atom newValue)
        {
            frame->setArgument(index, newValue);
        }

    private:
        GCMember<avmplus::DebugFrame> frame;
        int index;
    };

    /**
     * An IValue representing a property of an object.
     */
    class PropertyValue: public IValue
    {
    public:
        PropertyValue(avmplus::ScriptObject* parent, avmplus::Multiname& propertyname)
            : parent(parent), propertyname(propertyname) { }

        virtual bool isLValue() { return true; }
        virtual avmplus::Atom get()
        {
            return parent->toplevel()->getproperty(parent->atom(), propertyname, parent->vtable);
        }
        virtual void set(avmplus::Atom newValue)
        {
            parent->toplevel()->setproperty(parent->atom(), propertyname, newValue, parent->vtable);
        }

    private:
        GCMember<avmplus::ScriptObject> parent;
        avmplus::HeapMultiname propertyname;
    };

    /**
     * A simple command line interface for the Debugger.
     * Supports a gdb-like command line.
     */
    class GC_CPP_EXACT(DebugCLI, avmplus::Debugger)
    {
    private:
        DebugCLI(avmplus::AvmCore *core, avmplus::Debugger::TraceLevel tracelevel);
    public:
        /** @name command codes */
        /*@{*/
        enum {
            CMD_UNKNOWN = 0,
            CMD_QUIT,
            CMD_CONTINUE,
            CMD_STEP,
            CMD_NEXT,
            CMD_FINISH,
            CMD_BREAK,
            CMD_SET,
            CMD_LIST,
            CMD_PRINT,
            CMD_INFO,
            CMD_DELETE,
            CMD_COMMENT,
            CMD_HELP,
        };
        /*@}*/

        /** @name info sub commands */
        /*@{*/
        enum {
            INFO_UNKNOWN_CMD = 100,
            INFO_ARGS_CMD,
            INFO_BREAK_CMD,
            INFO_FILES_CMD,
            INFO_FUNCTIONS_CMD,
            INFO_LOCALS_CMD,
            INFO_STACK_CMD,
        };
        /*@}*/

        /**
         * StringInt is used to store the command arrays
         * used to translate command strings into command codes
         */
        struct StringInt
        {
            const char *text;
            int id;
        };

        REALLY_INLINE static DebugCLI* create(MMgc::GC* gc, avmplus::AvmCore *core, avmplus::Debugger::TraceLevel tracelevel)
        {
            return new (gc, MMgc::kExact) DebugCLI(core, tracelevel);
        }

        ~DebugCLI();

        void enterDebugger();
        void setCurrentSource(avmplus::Stringp file);
        bool filterException(avmplus::Exception *exception, bool willBeCaught);
        bool hitWatchpoint() { return false; }

        /**
         * @name command implementations
         */
        /*@{*/
        void breakpoint(char *location);
        void deleteBreakpoint(char *idstr);
        void showBreakpoints();
        void help();
        void locals(int frameNumber);
        void arguments(int frameNumber);

        static void bt();
        static bool printFrame(int frameNumber);
        static bool debuggerInterruptOnEnter;


        void info();
        void set();
        void list(const char* line);
        void print(char *name);
        void quit();
        /*@}*/

        void activate() { activeFlag = true; }

        static avmplus::MethodInfo* functionFor(avmplus::SourceInfo* src, int line);

        /**
         * Formats a value for display to the user.  Very similar to
         * AvmCore::format(), but does a few extra things.
         */
        avmplus::Stringp formatValue(avmplus::Atom value);

        GC_DATA_BEGIN(DebugCLI)
        
    private:
        bool activeFlag;
        char *currentSource;
        uint32_t currentSourceLen;
        GCMember<avmplus::String> GC_POINTER(currentFile);
        int breakpointCount;
        bool warnMissingSource;

        GCMember<BreakAction> GC_POINTER(firstBreakAction);
        GCMember<BreakAction> GC_POINTER(lastBreakAction);

        enum { kMaxCommandLine = 1024 };
        char commandLine[kMaxCommandLine];
        char lastCommand[kMaxCommandLine];
        char *currentToken;
        
        GC_DATA_END(DebugCLI)

        char *nextToken();

        void printIP();
        void displayLines(int linenum, int count);

        char* lineStart(int linenum);

        void throwUndefinedVarError(const char* name);

        /**
         * Gets a value.  "name" can be either a constant (string,
         * number, boolean, undefined, null, xml, xmllist), or the
         * name of a local, argument, or member of a variable on
         * the scope chain.
         */
        IValue* getValue(const char *name);

        /**
         * Evaluates an expression, and returns its value.
         * *outSawTrailingDot is set to true if the expression has a trailing
         * dot, e.g. "foo." or "foo.bar."
         */
        IValue* evaluate(char *expr, bool* outSawTrailingDot);

       /**
         * @name command name arrays and support code
         */
        /*@{*/
        int determineCommand(StringInt cmdList[],
                             const char *input,
                             int defCmd);
        const char* commandNumberToCommandName(StringInt cmdList[],
                                               int cmdNumber);
        int commandFor(const char *input) {
            return determineCommand(commandArray, input, CMD_UNKNOWN);
        }
        int infoCommandFor(const char *input) {
            return determineCommand(infoCommandArray, input, INFO_UNKNOWN_CMD);
        }

        static StringInt commandArray[];
        static StringInt infoCommandArray[];
        /*@}*/
    };

    /**
     * Helper class to iterate over all properties of an object (both slots
     * and dynamic properties).
     */
    class PropertyIterator
    {
    public:
        /**
         * atom: the object whose properties we will iterate over.
         */
        PropertyIterator(avmplus::AvmCore* core, avmplus::Atom atom);
        virtual ~PropertyIterator() {}

        /**
         * Iterates over all the object's properties, and calls process()
         * for each one.  If process() ever returns false, iteration halts.
         */
        void iterate();

    protected:
        /**
         * Called once for each property that has been found.  If process()
         * ever returns false, iteration halts.
         */
        virtual bool process(avmplus::Multiname* key, avmplus::BindingKind bk) = 0;

        avmplus::AvmCore* core;
        avmplus::ScriptObject* object;
    };

    /**
     * Helper to find a property with the given name.  Since this is for the
     * debugger, it ignores visibility issues -- e.g. it will match any property
     * with the given name, even if that property is private and therefore not
     * visible from the caller's context.
     *
     * After you call iterate(), 'value' will be the property's value if it was
     * found, or NULL if it was not found.
     *
     * This class can only be used on the stack.  ('value' does not have a write
     * barrier around it.)
     */
    class StPropertyFinder: public PropertyIterator
    {
    public:
        /**
         * atom:         The object in which we want to look for a property.
         * propertyname: The property name to look for.
         */
        StPropertyFinder(avmplus::AvmCore* core, avmplus::Atom atom, const char* propertyname);

        IValue* value;

    protected:
        /**
         * Returns true if the key matches the originally passed-in property name.
         */
        bool matches(const avmplus::Multiname* key) const;

        virtual bool process(avmplus::Multiname* key, avmplus::BindingKind bk);

        const char* propertyname;
    };

    /**
     * Prints all of an object's properties, and their values, to core->console.
     */
    class PropertyPrinter: public PropertyIterator
    {
    public:
        PropertyPrinter(avmplus::AvmCore* core, DebugCLI* debugger, avmplus::Atom atom);

    protected:
        virtual bool process(avmplus::Multiname* key, avmplus::BindingKind bk);

        DebugCLI* debugger;
    };
}
#endif

#endif /* __avmshell_AvmDebugCLI__ */
