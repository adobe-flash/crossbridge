/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell_core__
#define __avmshell_core__

// Ugly, ugly hack
namespace avmplus
{
    namespace ST_avmplus_vector_accessors
    {
        class ST_avmplus_vector_accessors;
    }
    namespace ST_mmgc_dependent
    {
        class ST_mmgc_dependent;
    }
}

namespace avmshell
{
    /**
     * Settings for ShellCore.  The command line parser uses this, but the initial state
     * is set in ShellCore.cpp, and it's propagated throughout various parts of the
     * system.  Partly duplicates the functionality in AvmCore::config, and some settings
     * are copied from ShellCoreSettings into that structure when a new core is created.
     *
     * We try to avoid conditional compilation here, as there's only ever one of these
     * structures floating around and the savings would be minuscule.
     *
     * FIXME: the naming of many fields could be much better, and they should be documented.
     */
    class ShellCoreSettings
    {
    public:
        ShellCoreSettings();

        char** arguments;               // non-terminated array of argument values, never NULL
        int numargs;                    // number of entries in 'arguments'
        bool nodebugger;
        int  astrace_console;
        uint32_t do_verbose;            // copy to config
        const char* verboseOnlyArg;     // copy to config (raw unprocessed)
        bool enter_debugger_on_launch;
        bool interrupts;                // copy to config
        bool verifyall;                 // copy to config
        bool verifyonly;                // copy to config
        bool verifyquiet;               // copy to config
        bool greedy;                    // copy to each GC
        bool nogc;                      // copy to each GC
        bool incremental;               // copy to each GC
        bool exactgc;                   // copy to each GC
        bool drc;                       // copy to each GC
        bool drcValidation;             // copy to each GC
        int32_t markstackAllowance;     // copy to each GC;
        bool fixedcheck;                // copy to each GC
        int gcthreshold;                // copy to each GC
        int langID;                     // copy to ShellCore?
        bool jitordie;                  // copy to config
        bool do_testSWFHasAS3;
        avmplus::Runmode runmode;       // copy to config
#ifdef VMCFG_NANOJIT
        nanojit::Config njconfig;       // copy to config
        avmplus::JitConfig jitconfig;   // copy to config
        bool osr_enabled;               // copy to config
        uint32_t osr_threshold;         // copy to config
        uint32_t jitprof_level;         // Log level for jit profiling
        const char* policyRulesArg;     // copy to config (raw unprocessed)
#endif
        avmplus::AvmCore::CacheSizes cacheSizes; // Default to unlimited
        const char* st_component;
        const char* st_category;
        const char* st_name;
        avmplus::ApiVersionSeries apiVersionSeries;
        avmplus::ApiVersion apiVersion;
        avmplus::BugCompatibility::Version swfVersion;

        MMgc::GCConfig::GCMode gcMode()
        {
            if (nogc)               return MMgc::GCConfig::kDisableGC;
            else if (greedy)        return MMgc::GCConfig::kGreedyGC;
            else if (incremental)   return MMgc::GCConfig::kIncrementalGC;
            else                    return MMgc::GCConfig::kNonincrementalGC;
        }
    };

    class ShellCodeContext : public avmplus::CodeContext
    {
    public:
        inline ShellCodeContext(avmplus::DomainEnv* env, const avmplus::BugCompatibility* bugCompatibility)
            : avmplus::CodeContext(env, bugCompatibility) { }
    };

    /**
     * ShellCore provides typical housekeeping tasks around an AvmCore, lightweight
     * embedders may prefer to use this rather than to subclass AvmCore themselves
     * and do all the housekeeping that results.  The shell uses this; see the
     * shell code for typical usage patterns.
     */
    class ShellCore : public avmplus::AvmCore
    {
        friend class SystemClass;
        friend class avmplus::DomainObject;
        friend class avmplus::ST_avmplus_vector_accessors::ST_avmplus_vector_accessors;
        friend class avmplus::ST_mmgc_dependent::ST_mmgc_dependent;
        friend class Shell; // access to shell_toplevel
    public:
        /**
         * Create a new core with the given GC (one gc per core).
         *
         * Requires: MMGC_ENTER and MMGC_GCENTER(gc) on the stack.
         */
        ShellCore(MMgc::GC *gc, avmplus::ApiVersionSeries apiVersionSeries);

        /**
         * Initialize the new core from the settings.  This creates a top-level
         * environment and performs other housekeeping.
         *
         * Requires: MMGC_ENTER and MMGC_GCENTER(gc) on the stack.
         */
        ShellToplevel* setup(const ShellCoreSettings& settings);

        /**
         * Load the contents from the file and run them in the context of this core's
         * top level.  The file may contain ABC bytecode, a SWF containing a single DoABC
         * tag, or (if the run-time compiler is present) ActionScript source code.
         *
         * Requires: MMGC_ENTER and MMGC_GCENTER(gc) on the stack.
         */
        int evaluateFile(ShellCoreSettings& settings, const char* filename);

        int evaluateScriptBuffer(avmplus::ScriptBuffer& buffer, bool enter_debugger_on_launch);


#ifdef VMCFG_EVAL
        /**
         * Load ActionScript source code from the string and run it in the
         * context of this core's top level.  Note the string must have been
         * created in the context of the GC in this ShellCore instance.
         *
         * Requires: MMGC_ENTER and MMGC_GCENTER(gc) on the stack.
         */
        void evaluateString(avmplus::String* input, bool record_time=false);
#endif

#ifdef AVMSHELL_PROJECTOR_SUPPORT
        int executeProjector(ShellSettings &settings);
#endif

#ifdef VMCFG_SELFTEST
        void executeSelftest(ShellCoreSettings& settings);
#endif

        avmplus::PoolObject* getShellPool() { return shellPool; }

        avmshell::SystemClass* getSystemClass() { return systemClass; }

#ifdef AVMSHELL_PROJECTOR_SUPPORT
        static bool isValidProjectorFile(const char* filename);
        static void gatherProjectorSettings(ShellSettings &settings);
#endif

        /*virtual*/ inline avmplus::ApiVersion getDefaultAPI() { return this->defaultAPIVersion; }

        inline avmplus::BugCompatibility::Version getDefaultBugCompatibilityVersion() const { return defaultBugCompatibilityVersion; }

    protected:
        virtual void setStackLimit() = 0;

        virtual avmplus::Toplevel* createToplevel(avmplus::AbcEnv* abcEnv);
#ifdef DEBUGGER
        virtual avmplus::Debugger* createDebugger(int tracelevel)
        {
            AvmAssert(allowDebugger >= 0);
            return allowDebugger ? DebugCLI::create(GetGC(), this, (avmplus::Debugger::TraceLevel)tracelevel) : NULL;
        }

#endif
#ifdef VMCFG_EVAL
        virtual avmplus::String* readFileForEval(avmplus::String* referencing_filename, avmplus::String* filename);
#endif

    private:
        static void interruptTimerCallback(void* data);

        ShellToplevel* createShellToplevel();

        void interrupt(avmplus::Toplevel*, InterruptReason);
        void stackOverflow(avmplus::Toplevel *toplevel);
        void setEnv(avmplus::Toplevel *toplevel, int argc, char *argv[]);
        void initShellPool();
        int handleArbitraryExecutableContent(bool do_testSWFHasAS3, avmplus::ScriptBuffer& code, const char * filename);
#ifdef VMCFG_EVAL
        avmplus::String* decodeBytesAsUTF16String(uint8_t* bytes, uint32_t nbytes, bool terminate=false);
#endif // VMCFG_EVAL
#ifdef DEBUGGER
        DebugCLI* debugCLI() { return (DebugCLI*)debugger(); }
#endif

#ifdef AVMPLUS_VERBOSE
        virtual const char* identifyDomain(avmplus::Domain* domain);
#endif
        SystemClass* systemClass;
        avmplus::PoolObject* shellPool;
        avmplus::GCOutputStream *consoleOutputStream;
        bool gracePeriod;
        bool inStackOverflow;
        int allowDebugger;
        ShellToplevel* shell_toplevel;
        avmplus::DomainEnv* shell_domainEnv;
        avmplus::Domain* shell_domain;
        // Note that this has been renamed to emphasize the fact that it is
        // the CodeContext/DomainEnv that user code will run in (as opposed
        // to the Shell's builtin classes, eg System, File, Domain).
        ShellCodeContext* user_codeContext;
        avmplus::ApiVersion defaultAPIVersion;
        avmplus::BugCompatibility::Version defaultBugCompatibilityVersion;
    };

    class GC_CPP_EXACT(ShellToplevel, avmplus::Toplevel)
    {
        friend class ShellCore;

    private:
        ShellToplevel(avmplus::AbcEnv* abcEnv);

    public:
        REALLY_INLINE static ShellToplevel* create(MMgc::GC* gc, avmplus::AbcEnv* abcEnv)
        {
            return new (gc, MMgc::kExact) ShellToplevel(abcEnv);
        }

        ShellCore* core() const {
            return (ShellCore*)Toplevel::core();
        }

        GCRef<avmplus::ClassClosure> workerClass() const 
        {
            return shellClasses->get_WorkerClass();
        }

        GCRef<avmplus::ClassClosure> workerDomainClass() const 
        {
            return shellClasses->get_WorkerDomainClass();
        }

        GCRef<avmplus::ScriptObject> constructWorkerObject() const;
        
    private:
        GC_DATA_BEGIN(ShellToplevel)

        GCMember<avmplus::shell_toplevelClassManifest> GC_POINTER(shellClasses);

        GC_DATA_END(ShellToplevel)
    };
}

#endif /* __avmshell_core__ */
