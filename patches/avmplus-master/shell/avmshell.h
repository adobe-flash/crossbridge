/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmshell__
#define __avmshell__

#include "avmplus.h"

#include "extensions-tracers.h"
#include "avmshell-tracers.h"

#ifdef VMCFG_SELFTEST
// Allow selftests coded directly to nanojit APIs.
#include "../nanojit/nanojit.h"
#endif

#if defined AVMPLUS_MAC || defined AVMPLUS_UNIX
  // Support for the -workers switch and distributing files across
  // multiple AvmCores on multiple threads.  Only the code in the
  // shell depends on this setting; the AVM core code works out of
  // the box.
  //
  // Requires pthreads.
  #define VMCFG_WORKERTHREADS
#endif

#if defined VMCFG_TESTING
  // We simulate Flash's use of API versioning for testing only.
  #define VMCFG_TEST_VERSIONING
#endif

#if !defined(UNDER_CE) && !defined(AVM_SHELL_NO_PROJECTOR)
#  define AVMSHELL_PROJECTOR_SUPPORT
#endif

#ifndef INT32_T_MAX
    #define INT32_T_MAX     (int32_t(0x7FFFFFFFL))  //max value for a 32-bit integer
#endif
#ifndef UINT32_T_MAX
    #define UINT32_T_MAX    (uint32_t(0xFFFFFFFFUL))  //max value for a 32-bit unsigned integer
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)       // 'scanf' was declared deprecated
#endif

// forward declarations for shell_toplevel.h
namespace avmshell
{
    class SystemClass;
}
namespace avmplus
{
    class SampleObject;
    class StackFrameObject;
    class NewObjectSampleObject;
    class TraceClass;
    class Isolate;

}

#include "shell_toplevel.h"

namespace avmshell
{
    class Shell;
    class ShellIsolate;
    class ShellCodeContext;
    class ShellCore;
    class ShellCoreImpl;
    class ShellSettings;
    class ShellToplevel;
    class EnvelopeClass;
    class EnvelopeObject;

}

#include "Selftest.h"
#include "Platform.h"
#include "File.h"

#include "FileInputStream.h"
#include "ConsoleOutputStream.h"
#include "SystemClass.h"
#include "FileClass.h"
#include "DomainClass.h"
#include "DebugCLI.h"
#include "DataIO.h"
#include "DictionaryGlue.h"
#include "SamplerScript.h"
#include "ShellCore.h"
// AVM2 PATCH START
#include "ShellPosixGlue.h"
// AVM2 PATCH END
#include "ShellWorkerGlue.h"
#include "ShellWorkerDomainGlue.h"

#include "shell_toplevel-classes.hh"

namespace avmshell
{
    // The stack margin is the amount of stack that should be available to native
    // code that does not itself check for stack overflow.  The execution engines
    // in the VM will reserve this amount of stack.

#ifdef AVMPLUS_64BIT
    const size_t kStackMargin = 262144;
#elif (defined AVMPLUS_WIN32 && (defined UNDER_CE || defined UNDER_RT) ) || defined VMCFG_SYMBIAN
    // FIXME: all embedded platforms, but we have no way of expressing that now
    const size_t kStackMargin = 32768;
#else
    const size_t kStackMargin = 131072;
#endif

    // The fallback stack size is probably not safe but is used by the shell code
    // if it fails to obtain the stack size from the operating system.
    // 512KB is the traditional value.

    const size_t kStackSizeFallbackValue = 512*1024;

    // exit codes
    enum {
        OUT_OF_MEMORY = 128
    };

    // swf support, impl code in swf.cpp
    bool isSwf(avmplus::ScriptBuffer);
    bool handleSwf(const char *, avmplus::ScriptBuffer, avmplus::Toplevel*, avmplus::CodeContext*, bool test_only);

#ifdef VMCFG_AOT
    // AOT support, impl code in aot.cpp
    void handleAOT(avmplus::AvmCore*, avmplus::Domain*, avmplus::DomainEnv*, avmplus::Toplevel*, avmplus::CodeContext*);
#endif

    class ShellSettings : public ShellCoreSettings
    {
    public:
        ShellSettings();

        char* programFilename;          // name of the executable, or NULL
        char** filenames;               // non-terminated array of file names, never NULL
        int numfiles;                   // number of entries in 'filenames'
        bool do_selftest;
        bool do_repl;
        bool do_log;
        bool do_projector;
        bool projectorHasArgs;
        int projectorContentLength;
        int projectorArgsLength;
        avmplus::ScriptBuffer *projectorScriptBuffer;
        int numthreads;
        int numworkers;
        int repeats;
        uint32_t stackSize;
        char st_mem[200];               // Selftest scratch memory.  200 chars ought to be enough for anyone
    };

    class ShellIsolate : public avmplus::Isolate
    {

    public:
        ShellIsolate(int32_t desc, int32_t parentDesc, avmplus::Aggregate* aggregate);
        virtual void doRun();
        virtual void copyByteCode(avmplus::ByteArrayObject* ba);
        virtual avmplus::ScriptObject* newWorkerObject(avmplus::Toplevel* toplevel);
		
		void evalCodeBlobs(bool enter_debugger_on_launch);
    };

    /**
     * Shell driver and command line parser.
     */
    class Shell : public avmplus::Aggregate {
        friend class PrimordialShellIsolate;
        friend class ShellIsolate;
    public:
        static int run(int argc, char *argv[]);
        static void parseCommandLine(int argc, char* argv[], ShellSettings& settings);

    private:
#ifdef VMCFG_WORKERTHREADS
        static void multiWorker(ShellSettings& settings);
#endif
        static void repl(ShellCore* shellCore);
        static void initializeLogging(const char* basename);
        void parseCommandLine(int argc, char* argv[]);
        static void usage();
        ShellSettings settings;
    };


    /**
     * A subclass of ShellCore that provides an implementation of setStackLimit().
     */
    class ShellCoreImpl : public ShellCore {
    public:
        /**
         * @param gc          The garbage collector for this core
         * @param mainthread  True if this core is being used on the main thread and not on a spawned
         *                    thread.  It must be one or the other; a core created on a spawned thread
         *                    cannot be used on the main thread or vice versa.
         */
        ShellCoreImpl(MMgc::GC* gc, ShellSettings& settings, bool mainthread);

        /**
         * Set AvmCore::minstack appropriately for the current thread.
         */
        virtual void setStackLimit();

    private:
        ShellSettings& settings;
        bool mainthread;
    };
}

#endif /* __avmshell__ */
