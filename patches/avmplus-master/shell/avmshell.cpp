/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmshell.h"
#ifdef VMCFG_NANOJIT
#include "../nanojit/nanojit.h"
#endif
#include <float.h>

#include "extensions-tracers.hh"
#include "avmshell-tracers.hh"

#ifdef VMCFG_HALFMOON
#include "../halfmoon/hm-main.h"  //  halfmoon::enable_mode
#endif

namespace avmshell
{
#ifdef AVMPLUS_WIN32
    extern bool show_error; // in avmshellWin.cpp
#endif

    ShellSettings::ShellSettings()
        : ShellCoreSettings()
        , programFilename(NULL)
        , filenames(NULL)
        , numfiles(-1)
        , do_selftest(false)
        , do_repl(false)
        , do_log(false)
        , do_projector(false)
        , numthreads(1)
        , numworkers(1)
        , repeats(1)
        , stackSize(0)
    {
    }

    ShellCoreImpl::ShellCoreImpl(MMgc::GC* gc, ShellSettings& settings, bool mainthread)
        : ShellCore(gc, settings.apiVersionSeries)
        , settings(settings)
        , mainthread(mainthread)
    {
    }


    /* virtual */
    void ShellCoreImpl::setStackLimit()
    {
        uintptr_t minstack;
        if (settings.stackSize != 0)
        {
            // Here we really depend on being called fairly high up on
            // the thread's stack, because we don't know where the highest
            // stack address is.
            minstack = uintptr_t(&minstack) - settings.stackSize + avmshell::kStackMargin;
        }
        else
        {
#ifdef VMCFG_WORKERTHREADS
            if (mainthread)
                minstack = Platform::GetInstance()->getMainThreadStackLimit();
            else {
                // Here we really depend on being called fairly high up on
                // the thread's stack, because we don't know where the highest
                // stack address is.
                // In order to be platform-independent, we can no
                // longer rely on getting the default stack height
                // from a pthread call. Instead, we get it from the
                // VMPI_threadAttrDefaultStackSize() call.
                size_t stackheight = VMPI_threadAttrDefaultStackSize();
                minstack = uintptr_t(&stackheight) - stackheight + avmshell::kStackMargin;
            }
#else
            minstack = Platform::GetInstance()->getMainThreadStackLimit();
#endif
        }

        // call the non-virtual setter on AvmCore
        AvmCore::setStackLimit(minstack);
    }

    /* static */
    int Shell::run(int argc, char *argv[])
    {
        MMgc::GCHeap::EnterLockInit();
        MMgc::GCHeapConfig conf;
        //conf.verbose = AvmCore::DEFAULT_VERBOSE_ON;
        MMgc::GCHeap::Init(conf);

        // Note that output from the command line parser (usage messages, error messages,
        // and printed version number / feature list) will not go to the log file.  We
        // could fix this if it's a hardship.

        {
            MMGC_ENTER_RETURN(OUT_OF_MEMORY);

            {
              // code coverage/cheap test
              MMGC_ENTER_SUSPEND;
            }

            avmplus::FixedHeapRef<Shell> instance(mmfx_new(Shell));

            instance->settings.programFilename = argv[0]; // How portable / reliable is this?
#ifdef AVMSHELL_PROJECTOR_SUPPORT
            ShellCore::gatherProjectorSettings(instance->settings);

            if(instance->settings.projectorHasArgs && argc > 0) {
                instance->settings.arguments = &argv[1];
                instance->settings.numargs = argc-1;
            }

            if(!(instance->settings.do_projector && instance->settings.projectorHasArgs))
#endif
            instance->parseCommandLine(argc, argv, instance->settings);

            if (instance->settings.do_log)
              initializeLogging(instance->settings.numfiles > 0 ? instance->settings.filenames[0] : "AVMLOG");

#ifdef VMCFG_WORKERTHREADS
            if (instance->settings.numworkers == 1 && instance->settings.numthreads == 1 && instance->settings.repeats == 1) 
            {
                avmplus::Isolate* isolate = instance->newIsolate(NULL); 
                instance->stateTransition(isolate, avmplus::Isolate::CANSTART);
				isolate->IncrementRef();
                isolate->run();
            }
            else
            {
                instance->multiWorker(instance->settings);
            }
#else
            avmplus::Isolate* isolate = instance->newIsolate(NULL); 
            instance->stateTransition(isolate, avmplus::Isolate::CANSTART);
			isolate->IncrementRef();
			isolate->run();
#endif
            instance->waitUntilNoIsolates();
            // Shell is refcounted now
            //mmfx_delete(instance);
        }

        MMgc::GCHeap::Destroy();
        MMgc::GCHeap::EnterLockDestroy();
        return 0;
    }

    ShellIsolate::ShellIsolate(int32_t desc, int32_t parentDesc, avmplus::Aggregate* aggregate)
        : Isolate(desc, parentDesc, aggregate)
    {}


    void ShellIsolate::copyByteCode(avmplus::ByteArrayObject* ba)
    {
        if (ba != NULL) {
			Isolate::copyByteCode(ba);
        }
		else 
		{
			Shell* shell = static_cast<Shell*> (getAggregate());
			 #ifdef AVMSHELL_PROJECTOR_SUPPORT
        if(shell->settings.do_projector)
        {
            m_code.allocate(1);
            size_t size = shell->settings.projectorScriptBuffer->getSize();
            m_code.values[0].allocate((int)size);
            memcpy(m_code.values[0].values, shell->settings.projectorScriptBuffer->getBuffer(), size);
            return;
        }
#endif
			int numfiles = shell->settings.numfiles;
			char** filenames = shell->settings.filenames;
			
			m_code.allocate(numfiles);
			for (int i = 0; i < numfiles; i++) {
				char* filename = filenames[i];
				FileInputStream f(filename);
				const bool isValid = f.valid() && ((uint64_t)f.length() < UINT32_T_MAX); //currently we cannot read files > 4GB
				if (isValid) {
					// parse new bytecode
					//avmplus::ScriptBuffer code = targetCore()->newScriptBuffer((size_t)f.available());
					//(f.read(buf), (size_t)f.available();
					size_t avail = (size_t)f.available();
					m_code.values[i].allocate((int)avail);
					f.read(m_code.values[i].values, avail);
				}
			}
		}
    }

    /*virtual*/ void ShellIsolate::doRun()
    {
        MMGC_ENTER_VOID;
        
        Shell* aggregate = static_cast<Shell*> (getAggregate());
        ShellSettings& settings = aggregate->settings;

        MMgc::GCConfig gcconfig;
        gcconfig.mode = settings.gcMode();
        MMgc::GC* gc = mmfx_new(MMgc::GC(MMgc::GCHeap::GetGCHeap(),  gcconfig));
        ShellToplevel* toplevel = NULL;
        {
            MMGC_GCENTER(gc);
            
            ShellCoreImpl* core = new ShellCoreImpl(gc, settings, false);

            aggregate->initialize(core, this);

            avmplus::EnterSafepointManager enterSafepointManager(core);
            
            toplevel = core->setup(settings);
            if (toplevel != NULL) { // setup OK
                avmplus::Isolate::State state = aggregate->queryState(this);
                if (state == avmplus::Isolate::RUNNING) { 
                    this->evalCodeBlobs(settings.enter_debugger_on_launch);
                } 
            }
			
            aggregate->beforeCoreDeletion(this);
            gc->Collect();            
            
            delete core;
        }
        mmfx_delete(gc);
        if (toplevel == NULL) {
            aggregate->stateTransition(this, avmplus::Isolate::ABORTED);
            // TODO
            // Harshness: kill the program if setup does not go right.  Are there
            // alternatives?  Do we even want to contemplate isolate creation
            // failing?  Recall that start() actually throws an exception if the
            // process limit is exceeded, but that's a controlled resource exhaustion.
            // In the case of MMGC_ENTER and ShellCore::setup() we should never fail.
            avmshell::Platform::GetInstance()->exit(1);
        }
        aggregate->afterGCDeletion(this);
    }


    avmplus::ScriptObject* ShellIsolate::newWorkerObject(avmplus::Toplevel* toplevel)
    {
        GCRef<ShellWorkerObject> workerObject 
            = toplevel->workerClass().staticCast<ShellWorkerClass>()->constructObject().staticCast<ShellWorkerObject>();
        return workerObject->setIsolate(this);
    }
	
	void ShellIsolate::evalCodeBlobs(bool enter_debugger_on_launch)
    {
        // The isolate is now live and operating independently.
        AvmAssert(avmplus::AvmCore::getActiveCore()->getIsolate() == this);
        AvmAssert(getAggregate()->queryState(this) == RUNNING);
        
        avmplus::AvmCore* core = getAvmCore();
        for (int i = 0; i < m_code.length; i++ ) {
            // execute event loop at the end of the last script
            if (i == (m_code.length - 1) && !isPrimordial())
                core->enterEventLoop = true;
            // parse new bytecode
			avmplus::ReadOnlyScriptBufferImpl* codeimpl = 
			new (core->gc) avmplus::ReadOnlyScriptBufferImpl(m_code.values[i].values, m_code.values[i].length);
			avmplus::ScriptBuffer code(codeimpl);
            core->evaluateScriptBuffer(code, enter_debugger_on_launch);
            // release bytes regardless of success/failure
            m_code.values[i].deallocate(); 
        }
        m_code.deallocate();
    }
	

    class PrimordialShellIsolate : public ShellIsolate
    {

    public:
        PrimordialShellIsolate(int32_t desc, int32_t parentDesc, avmplus::Aggregate* aggregate)
            : ShellIsolate(desc, parentDesc, aggregate) {}

        virtual void doRun()
        {
            Shell* aggregate = static_cast<Shell*>(getAggregate());
            ShellSettings& settings = aggregate->settings;
            MMgc::GCConfig gcconfig;
            if (settings.gcthreshold != 0)
                // (zero means use default value already in gcconfig.)
                gcconfig.collectionThreshold = settings.gcthreshold;
            gcconfig.exactTracing = settings.exactgc;
            gcconfig.markstackAllowance = settings.markstackAllowance;
            gcconfig.drc = settings.drc;
            gcconfig.mode = settings.gcMode();
            gcconfig.validateDRC = settings.drcValidation;
            
            MMgc::GC *gc = mmfx_new( MMgc::GC(MMgc::GCHeap::GetGCHeap(), gcconfig) );
            
            MMGC_GCENTER(gc);

            ShellCore* shell = new ShellCoreImpl(gc, settings, true);
            this->initialize(shell);

            avmplus::EnterSafepointManager enterSafepointManager(shell);

            ShellToplevel* toplevel = shell->setup(settings);
            // inlined singleWorkerHelper
            if (toplevel == NULL) // FIXME abort?
				Platform::GetInstance()->exit(1);

#ifdef VMCFG_SELFTEST
        if (settings.do_selftest) {
            shell->executeSelftest(settings);
                aggregate->beforeCoreDeletion(this);
                delete shell;
                mmfx_delete( gc );
                aggregate->afterGCDeletion(this);
            return;
        }
#endif

#ifdef AVMSHELL_PROJECTOR_SUPPORT
        if (settings.do_projector) {
            AvmAssert(settings.programFilename != NULL);
            int exitCode = shell->executeProjector(settings);
            // if (exitCode != 0) // HACKERY?!
                Platform::GetInstance()->exit(exitCode);
        }
#endif

#ifdef VMCFG_AOT
        int exitCode = shell->evaluateFile(settings, NULL);
        aggregate->beforeCoreDeletion(this);
        delete shell;
        mmfx_delete( gc );
        aggregate->afterGCDeletion(this);
        
        if (exitCode != 0)
            Platform::GetInstance()->exit(exitCode);
        return;
#endif

        // For -testswf we must have exactly one file
        if (settings.do_testSWFHasAS3 && settings.numfiles != 1)
            Platform::GetInstance()->exit(1);

        // execute each abc file
        for (int i=0 ; i < settings.numfiles ; i++ ) {
            int exitCode = shell->evaluateFile(settings, settings.filenames[i]);
            if (exitCode != 0)
                Platform::GetInstance()->exit(exitCode);
        }

#ifdef VMCFG_EVAL
        if (settings.do_repl)
                Shell::repl(shell);
#endif
		aggregate->requestAggregateExit();
        aggregate->beforeCoreDeletion(this);
        delete shell;
        mmfx_delete( gc );
        aggregate->afterGCDeletion(this);
    }
    };



#ifdef VMCFG_WORKERTHREADS

//#define LOGGING(x)    x
#define LOGGING(x)

    // When we have more than one worker then we spawn as many threads as
    // called for and create a number of ShellCores corresponding to
    // the number of workers.  Those cores are scheduled on the threads.
    // The main thread acts as a supervisor, handing out work and scheduling
    // cores on the threads.

    struct MultiworkerState;

    struct CoreNode
    {
        CoreNode(ShellCore* core, int id)
            : core(core)
            , id(id)
            , next(NULL)
        {
        }

        ~CoreNode()
        {
            // Destruction order matters.
            MMgc::GC* gc = core->GetGC();
            {
                MMGC_GCENTER(gc);
#ifdef _DEBUG
                core->codeContextThread = VMPI_currentThread();
#endif
                delete core;
            }


            delete gc;
        }

        ShellCore * const   core;
        const int           id;
        CoreNode *          next;       // For the LRU list of available cores
    };

    struct ThreadNode
    {
        ThreadNode(MultiworkerState& state, int id)
            : state(state)
            , id(id)
            , pendingWork(false)
            , corenode(NULL)
            , filename(NULL)
            , next(NULL)
        {
        }

        // Called from master, which should not be holding
        // thread_monitor but may hold global_monitor
        void startWork(CoreNode* corenode, const char* filename)
        {
            SCOPE_LOCK_NAMED(locker, thread_monitor) {
                this->corenode = corenode;
                this->filename = filename;
                this->pendingWork = true;
                locker.notify();
            }
        }

        vmbase::WaitNotifyMonitor thread_monitor;
        MultiworkerState& state;
        vmbase::VMThread* thread;
        const int id;
        bool pendingWork;
        CoreNode* corenode;         // The core running (or about to run, or just finished running) on this thread
        const char* filename;       // The work given to that core
        ThreadNode * next;          // For the LRU list of available threads
    };

    struct MultiworkerState
    {
        MultiworkerState(ShellSettings& settings)
            : settings(settings)
            , numthreads(settings.numthreads)
            , numcores(settings.numworkers)
            , free_threads(NULL)
            , free_threads_last(NULL)
            , free_cores(NULL)
            , free_cores_last(NULL)
            , num_free_threads(0)
        {
        }


        // Called from the slave threads, which should be holding
        // neither thread_monitor nor global_monitor.
        void freeThread(ThreadNode* t)
        {
            SCOPE_LOCK_NAMED(locker, global_monitor) {

                if (free_threads_last != NULL)
                    free_threads_last->next = t;
                else
                    free_threads = t;
                free_threads_last = t;

                if (t->corenode != NULL) {
                    if (free_cores_last != NULL)
                        free_cores_last->next = t->corenode;
                    else
                        free_cores = t->corenode;
                    free_cores_last = t->corenode;
                }

                num_free_threads++;

                locker.notify();
            }
        }

        // Called from the master thread, which must already hold global_monitor.
        bool getThreadAndCore(ThreadNode** t, CoreNode** c)
        {
            if (free_threads == NULL || free_cores == NULL)
                return false;

            *t = free_threads;
            free_threads = free_threads->next;
            if (free_threads == NULL)
                free_threads_last = NULL;
            (*t)->next = NULL;

            *c = free_cores;
            free_cores = free_cores->next;
            if (free_cores == NULL)
                free_cores_last = NULL;
            (*c)->next = NULL;

            num_free_threads--;

            return true;
        }

        vmbase::WaitNotifyMonitor global_monitor;
        ShellSettings&      settings;
        int                 numthreads;
        int                 numcores;

        // Queues of available threads and cores.  Protected by
        // global_monitor, also used to signal availability
        ThreadNode*         free_threads;
        ThreadNode*         free_threads_last;
        CoreNode*           free_cores;
        CoreNode*           free_cores_last;
        int                 num_free_threads;
    };

    static void masterThread(MultiworkerState& state);

    class SlaveThread : public vmbase::VMThread
    {
    public:
        SlaveThread(ThreadNode* tn) : self(tn) {}

        virtual void run();

    private:
        ThreadNode* self;

    };

    /* static */
    void Shell::multiWorker(ShellSettings& settings)
    {
        AvmAssert(!settings.do_repl && !settings.do_projector && !settings.do_selftest);

        MultiworkerState    state(settings);
        const int           numthreads(state.numthreads);
        const int           numcores(state.numcores);
        ThreadNode** const  threads(new ThreadNode*[numthreads]);
        CoreNode** const    cores(new CoreNode*[numcores]);

        MMgc::GCConfig gcconfig;
        gcconfig.collectionThreshold = settings.gcthreshold;
        gcconfig.exactTracing = settings.exactgc;
        gcconfig.markstackAllowance = settings.markstackAllowance;
        gcconfig.mode = settings.gcMode();

        // Going multi-threaded.

        // Create and start threads.  They add themselves to the free list.
        for ( int i=0 ; i < numthreads ; i++ )  {
            threads[i] = new ThreadNode(state, i);
            threads[i]->thread = new SlaveThread(threads[i]);
            threads[i]->thread->start();
        }

        // Create collectors and cores.
        // Extra credit: perform setup in parallel on the threads.
        for ( int i=0 ; i < numcores ; i++ ) {
            MMgc::GC* gc = new MMgc::GC(MMgc::GCHeap::GetGCHeap(),  gcconfig);
            MMGC_GCENTER(gc);
            cores[i] = new CoreNode(new ShellCoreImpl(gc, settings, false), i);
            if (!cores[i]->core->setup(settings))
                Platform::GetInstance()->exit(1);
        }

        // Add the cores to the free list.
        for ( int i=numcores-1 ; i >= 0 ; i-- ) {
            cores[i]->next = state.free_cores;
            state.free_cores = cores[i];
        }
        state.free_cores_last = cores[numcores-1];

        // No locks are held by the master at this point
        masterThread(state);
        // No locks are held by the master at this point

        // Some threads may still be computing, so just wait for them
        SCOPE_LOCK_NAMED(locker, state.global_monitor) {
            while (state.num_free_threads < numthreads)
                locker.wait();
        }

        // Shutdown: feed NULL to all threads to make them exit.
        for ( int i=0 ; i < numthreads ; i++ )
            threads[i]->startWork(NULL,NULL);

        // Wait for all threads to exit.
        for ( int i=0 ; i < numthreads ; i++ ) {
            threads[i]->thread->join();
            LOGGING( avmplus::AvmLog("T%d: joined the main thread\n", i); )
        }

        // Single threaded again.

        for ( int i=0 ; i < numthreads ; i++ ) {
            delete threads[i]->thread;
            delete threads[i];
        }

        for ( int i=0 ; i < numcores ; i++ )
            delete cores[i];

        delete [] threads;
        delete [] cores;
    }

    static void masterThread(MultiworkerState& state)
    {
        const int numfiles(state.settings.numfiles);
        const int repeats(state.settings.repeats);
        char** const filenames(state.settings.filenames);

        SCOPE_LOCK_NAMED(locker, state.global_monitor) {
            int r=0;
            for (bool finish = false; !finish;) {
                int nextfile = 0;
                for (;;) {
                    ThreadNode* threadnode;
                    CoreNode* corenode;
                    while (state.getThreadAndCore(&threadnode, &corenode) && !finish) {
                        LOGGING( avmplus::AvmLog("Scheduling %s on T%d with C%d\n", filenames[nextfile], threadnode->id, corenode->id); )
                        threadnode->startWork(corenode, filenames[nextfile]);
                        nextfile++;
                        if (nextfile == numfiles) {
                            r++;
                            if (r == repeats)
                                finish = true;
                            else
                                nextfile = 0;
                        }
                    }
                    // The original algorithm was jumping directly to
                    // the end of critical section upon discovering
                    // that it should terminate all the
                    // loops. Consequently, we have to break out of
                    // this for loop to bypass the locker.wait()
                    // statement.
                    if (finish) break;
                    locker.wait();
                }
            }
        }
    }

    void SlaveThread::run()
    {
        MMGC_ENTER_VOID;

        MultiworkerState& state = self->state;

        for (;;) {
            // Signal that we're ready for more work: add self to the list of free threads

            state.freeThread(self);

            // Obtain more work.  We have to hold self->thread_monitor here but the master won't touch corenode and
            // filename until we register for more work, so they don't have to be copied out of
            // the thread structure.

            SCOPE_LOCK_NAMED(locker, self->thread_monitor) {
                // Don't wait when pendingWork == true,
                // slave might have been already signalled but it didn't notice because it wasn't waiting yet.
                while (self->pendingWork == false)
                    locker.wait();
            }
            if (self->corenode == NULL) {
                LOGGING( avmplus::AvmLog("T%d: Exiting\n", self->id); )
                return;
            }

            // Perform work
            LOGGING( avmplus::AvmLog("T%d: Work starting\n", self->id); )
            {
                MMGC_GCENTER(self->corenode->core->GetGC());
#ifdef _DEBUG
                self->corenode->core->codeContextThread = VMPI_currentThread();
#endif
                self->corenode->core->evaluateFile(state.settings, self->filename); // Ignore the exit code for now
            }
            LOGGING( avmplus::AvmLog("T%d: Work completed\n", self->id); )

            SCOPE_LOCK(self->thread_monitor) {
                self->pendingWork = false;
            }


        }
        return;
    }

#endif  // VMCFG_WORKERTHREADS

#ifdef VMCFG_EVAL

    /* static */
    void Shell::repl(ShellCore* shellCore)
    {
        const int kMaxCommandLine = 1024;
        char commandLine[kMaxCommandLine];
        avmplus::String* input;

        avmplus::AvmLog("avmplus interactive shell\n"
               "Type '?' for help\n\n");

        for (;;)
        {
            bool record_time = false;
            avmplus::AvmLog("> ");

            if(Platform::GetInstance()->getUserInput(commandLine, kMaxCommandLine) == NULL)
                return;

            commandLine[kMaxCommandLine-1] = 0;
            if (VMPI_strncmp(commandLine, "?", 1) == 0) {
                avmplus::AvmLog("Text entered at the prompt is compiled and evaluated unless\n"
                       "it is one of these commands:\n\n"
                       "  ?             print help\n"
                       "  .input        collect lines until a line that reads '.end',\n"
                       "                then eval the collected lines\n"
                       "  .load file    load the file (source or compiled)\n"
                       "  .quit         leave the repl\n"
                       "  .time expr    evaluate expr and report the time it took.\n\n");
                continue;
            }

            if (VMPI_strncmp(commandLine, ".load", 5) == 0) {
                const char* s = commandLine+5;
                while (*s == ' ' || *s == '\t')
                    s++;
                // wrong, handles only source code
                //readFileForEval(NULL, newStringLatin1(s));
                // FIXME: implement .load
                // Small amount of generalization of the code currently in the main loop should
                // take care of it.
                avmplus::AvmLog("The .load command is not implemented\n");
                continue;
            }

            if (VMPI_strncmp(commandLine, ".input", 6) == 0) {
                input = shellCore->newStringLatin1("");
                for (;;) {
                    if(Platform::GetInstance()->getUserInput(commandLine, kMaxCommandLine) == NULL)
                        return;
                    commandLine[kMaxCommandLine-1] = 0;
                    if (VMPI_strncmp(commandLine, ".end", 4) == 0)
                        break;
                    input->appendLatin1(commandLine);
                }
                goto compute;
            }

            if (VMPI_strncmp(commandLine, ".quit", 5) == 0) {
                return;
            }

            if (VMPI_strncmp(commandLine, ".time", 5) == 0) {
                record_time = true;
                input = shellCore->newStringLatin1(commandLine+5);
                goto compute;
            }

            input = shellCore->newStringLatin1(commandLine);

        compute:
            shellCore->evaluateString(input, record_time);
        }
    }

#endif // VMCFG_EVAL

    // open logfile based on a filename
    void Shell::initializeLogging(const char* basename)
    {
        const char* lastDot = VMPI_strrchr(basename, '.');
        if(lastDot)
        {
            //filename could contain '/' or '\' as their separator, look for both
            const char* lastPathSep1 = VMPI_strrchr(basename, '/');
            const char* lastPathSep2 = VMPI_strrchr(basename, '\\');
            if(lastPathSep1 < lastPathSep2) //determine the right-most
                lastPathSep1 = lastPathSep2;

                //if dot is before the separator, the filename does not have an extension
                if(lastDot < lastPathSep1)
                    lastDot = NULL;
        }

        //if no extension then take the entire filename or
        size_t logFilenameLen = (lastDot == NULL) ? VMPI_strlen(basename) : (lastDot - basename);

        char* logFilename = new char[logFilenameLen + 5];  // 5 bytes for ".log" + null char
        VMPI_strncpy(logFilename,basename,logFilenameLen);
        VMPI_strcpy(logFilename+logFilenameLen,".log");

        Platform::GetInstance()->initializeLogging(logFilename);

        delete [] logFilename;
    }

    /* static */
    void Shell::parseCommandLine(int argc, char* argv[], ShellSettings &settings)
    {
        bool print_version = false;

        // options filenames -- args
        for (int i=1; i < argc ; i++) {
            const char * const arg = argv[i];
            bool mmgcSaysArgIsWrong = false;

            if (arg[0] == '-')
            {
                if (arg[1] == '-' && arg[2] == 0) {
                    if (settings.filenames == NULL)
                        settings.filenames = &argv[i];
                    settings.numfiles = int(&argv[i] - settings.filenames);
                    i++;
                    settings.arguments = &argv[i];
                    settings.numargs = argc - i;
                    break;
                }

                if (arg[1] == 'D') {
                    if (!VMPI_strcmp(arg+2, "timeout")) {
                        settings.interrupts = true;
                    }
                    else if (!VMPI_strcmp(arg+2, "version")) {
                        print_version = true;
                    }
                    else if (!VMPI_strcmp(arg+2, "nodebugger")) {
                        // allow this option even in non-DEBUGGER builds to make test scripts simpler
                        settings.nodebugger = true;
                    }
#if defined(AVMPLUS_IA32) && defined(VMCFG_NANOJIT)
                    else if (!VMPI_strcmp(arg+2, "nosse")) {
                        settings.njconfig.i386_sse2 = false;
                    }
                    else if (!VMPI_strcmp(arg+2, "fixedesp")) {
                        settings.njconfig.i386_fixed_esp = true;
                    }
#endif /* AVMPLUS_IA32 */
#if defined(AVMPLUS_ARM) && defined(VMCFG_NANOJIT)
                    else if (!VMPI_strcmp(arg+2, "arm_arch") && i+1 < argc ) {
                        settings.njconfig.arm_arch = (uint8_t)VMPI_strtol(argv[++i], 0, 10);
                    }
                    else if (!VMPI_strcmp(arg+2, "arm_vfp")) {
                        settings.njconfig.arm_vfp = true;
                    }
#endif /* AVMPLUS_IA32 */
#ifdef VMCFG_VERIFYALL
                    else if (!VMPI_strcmp(arg+2, "verifyall")) {
                        settings.verifyall = true;
                        settings.verifyquiet = false;
                    }
                    else if (!VMPI_strcmp(arg+2, "verifyonly")) {
                        settings.verifyall = true;
                        settings.verifyonly = true;
                        settings.verifyquiet = false;
                    }
                    else if (!VMPI_strcmp(arg+2, "verifyquiet")) {
                        settings.verifyall = true;
                        settings.verifyonly = true;
                        settings.verifyquiet = true;
                    }
#endif /* VMCFG_VERIFYALL */
                    else if (!VMPI_strcmp(arg+2, "greedy")) {
                        settings.greedy = true;
                    }
                    else if (!VMPI_strcmp(arg+2, "nogc")) {
                        settings.nogc = true;
                    }
                    else if (!VMPI_strcmp(arg+2, "noincgc")) {
                        settings.incremental = false;
                    }
                    else if (!VMPI_strcmp(arg+2, "drcvalidation")) {
                        settings.drcValidation = true;
                    }
#ifdef VMCFG_SELECTABLE_EXACT_TRACING
                    else if (!VMPI_strcmp(arg+2, "noexactgc")) {
                        settings.exactgc = false;
                    }
#endif
                    else if (!VMPI_strcmp(arg+2, "nodrc")) {
                        settings.drc = false;
                    }
                    else if (!VMPI_strcmp(arg+2, "nofixedcheck")) {
                        settings.fixedcheck = false;
                    }
                    else if (!VMPI_strcmp(arg+2, "gcthreshold") && i+1 < argc) {
                        settings.gcthreshold = VMPI_strtol(argv[++i], 0, 10);
                    }
#if defined(DEBUGGER) && !defined(VMCFG_DEBUGGER_STUB)
                    else if (!VMPI_strcmp(arg+2, "astrace") && i+1 < argc ) {
                        settings.astrace_console = VMPI_strtol(argv[++i], 0, 10);
                    }
                    else if (!VMPI_strcmp(arg+2, "language") && i+1 < argc ) {
                        settings.langID=-1;
                        for (int j=0;j<kLanguages;j++) {
                            if (!VMPI_strcmp(argv[i+1],languageNames[j].str)) {
                                settings.langID=j;
                                break;
                            }
                        }
                        if (settings.langID==-1) {
                            settings.langID = VMPI_atoi(argv[i+1]);
                        }
                        i++;
                    }
#endif /* defined(DEBUGGER) && !defined(VMCFG_DEBUGGER_STUB) */
#ifdef VMCFG_SELFTEST
                    else if (!VMPI_strncmp(arg+2, "selftest", 8)) {
                        settings.do_selftest = true;
                        if (arg[10] == '=') {
                            VMPI_strncpy(settings.st_mem, arg+11, sizeof(settings.st_mem));
                            settings.st_mem[sizeof(settings.st_mem)-1] = 0;
                            char *p = settings.st_mem;
                            settings.st_component = p;
                            while (*p && *p != ',')
                                p++;
                            if (*p == ',')
                                *p++ = 0;
                            settings.st_category = p;
                            while (*p && *p != ',')
                                p++;
                            if (*p == ',')
                                *p++ = 0;
                            settings.st_name = p;
                            if (*settings.st_component == 0)
                                settings.st_component = NULL;
                            if (*settings.st_category == 0)
                                settings.st_category = NULL;
                            if (*settings.st_name == 0)
                                settings.st_name = NULL;
                        }
                    }
#endif /* VMCFG_SELFTEST */
#ifdef AVMPLUS_VERBOSE
                    else if (!VMPI_strncmp(arg+2, "verbose-only", 12)) {
                        settings.verboseOnlyArg = argv[++i];  // capture the string process it later
                    }
                    else if (!VMPI_strncmp(arg+2, "verbose", 7)) {
                        settings.do_verbose = avmplus::AvmCore::DEFAULT_VERBOSE_ON; // all 'on' by default
                        if (arg[9] == '=') {
                            char* badFlag;
                            settings.do_verbose = avmplus::AvmCore::parseVerboseFlags(&arg[10], badFlag);
                            if (badFlag) {
                                avmplus::AvmLog("Unknown verbose flag while parsing '%s'\n", badFlag);
                                usage();
                            }
                        }
                    }
#endif /* AVMPLUS_VERBOSE */
#ifdef VMCFG_NANOJIT
                    else if (!VMPI_strcmp(arg+2, "nocse")) {
                        settings.njconfig.cseopt = false;
                    }
                    else if (!VMPI_strcmp(arg+2, "checkjitpageflags")) {
                        settings.njconfig.check_page_flags = true;
                    }
                    else if (!VMPI_strcmp(arg+2, "noinline")) {
                        settings.jitconfig.opt_inline = false;
                    }
                    else if (!VMPI_strcmp(arg+2, "jitordie")) {
                        settings.runmode = avmplus::RM_jit_all;
                        settings.jitordie = true;
                    }
#ifdef VMCFG_HALFMOON
                    else if (!VMPI_strcmp(arg, "-Dhalfmoon")) {
                        // FIXME: This is a dirty hack.  (bug 746736)
                        // Halfmoon is presently configured globally with a bunch
                        // of static variables.  It should be configured for each
                        // AvmCore instance using the JitConfig structure or similar.
                        halfmoon::enable_mode = 4;	// use halfmoon to produce LIR
                    }
#endif /* VMCFG_HALFMOON */
#endif /* VMCFG_NANOJIT */
                    else if (!VMPI_strcmp(arg+2, "interp")) {
                        settings.runmode = avmplus::RM_interp_all;
                    }
                    else {
                        avmplus::AvmLog("Unrecognized option %s\n", arg);
                        usage();
                    }
                }
                else if (!VMPI_strcmp(arg, "-cache_bindings") && i+1 < argc) {
                    settings.cacheSizes.bindings = (uint16_t)VMPI_strtol(argv[++i], 0, 10);
                }
                else if (!VMPI_strcmp(arg, "-cache_metadata") && i+1 < argc) {
                    settings.cacheSizes.metadata = (uint16_t)VMPI_strtol(argv[++i], 0, 10);
                }
                else if (!VMPI_strcmp(arg, "-cache_methods") && i+1 < argc ) {
                    settings.cacheSizes.methods = (uint16_t)VMPI_strtol(argv[++i], 0, 10);
                }
                else if (!VMPI_strcmp(arg, "-swfHasAS3")) {
                    settings.do_testSWFHasAS3 = true;
                }
#ifdef VMCFG_NANOJIT
                else if (!VMPI_strcmp(arg, "-jitharden")) {
                    settings.njconfig.harden_nop_insertion = true;
                    settings.njconfig.harden_function_alignment = true;
                }
                else if (!VMPI_strcmp(arg, "-Ojit")) {
                    settings.runmode = avmplus::RM_jit_all;
                }
#ifdef VMCFG_COMPILEPOLICY
                else if (!VMPI_strcmp(arg, "-policy")) {
                    settings.policyRulesArg = argv[++i];  // capture the string process it later
                }
#endif /* VMCFG_COMPILEPOLICY */
                else if (!VMPI_strncmp(arg, "-osr=", 5)) {
                    // parse the commandline threshold
                    int32_t threshold;
                    if (VMPI_sscanf(arg + 5, "%d", &threshold) != 1 ||
                        threshold < 0) {
                      avmplus::AvmLog("Bad value to -osr: %s\n", arg + 5);
                      usage();
                    }
                    settings.osr_threshold = threshold;
                    if (threshold == 0)
                        settings.osr_enabled = false;
                }
                else if (!VMPI_strncmp(arg, "-prof=", 6)) {
                    // parse jit profiling level
                    int32_t level;
                    if (VMPI_sscanf(arg + 6, "%d", &level) != 1 ||
                        level < 0 || level > 2) {
                        avmplus::AvmLog("Bad value to -prof: %s", arg + 6);
                        usage();
                    }
                    settings.jitprof_level = level;
                }
#endif /* VMCFG_NANOJIT */
                else if (MMgc::GCHeap::GetGCHeap()->Config().IsGCOptionWithParam(arg) && i+1 < argc ) {
                    const char *val = argv[++i];
                    if (MMgc::GCHeap::GetGCHeap()->Config().ParseAndApplyOption(arg, mmgcSaysArgIsWrong, val)) {
                        if (mmgcSaysArgIsWrong) {
                            avmplus::AvmLog("Invalid GC option: %s %s\n", arg, val);
                            usage();
                        }
                        // otherwise, option was implicitly applied during the
                        // ParseAndApplyOption invocation in the test above.
                    } else {
                        AvmAssertMsg(false, "IsGCOptionWithParam inconsistent with ParseAndApplyOption.");
                    }
                }
                else if (MMgc::GCHeap::GetGCHeap()->Config().ParseAndApplyOption(arg, mmgcSaysArgIsWrong)) {
                    if (mmgcSaysArgIsWrong) {
                        avmplus::AvmLog("Invalid GC option: %s\n", arg);
                        usage();
                    }
                }
                else if (!VMPI_strcmp(arg, "-stack") && i+1 < argc ) {
                    unsigned stack;
                    int nchar;
                    const char* val = argv[++i];
                    if (VMPI_sscanf(val, "%u%n", &stack, &nchar) == 1 && size_t(nchar) == VMPI_strlen(val) && stack > avmshell::kStackMargin) {
                        settings.stackSize = uint32_t(stack);
                    }
                    else
                    {
                        avmplus::AvmLog("Bad argument to -stack\n");
                        usage();
                    }
                }
#ifdef MMGC_MARKSTACK_ALLOWANCE
                else if (!VMPI_strcmp(arg, "-gcstack") && i+1 < argc ) {
                    int stack;
                    int nchar;
                    const char* val = argv[++i];
                    if (VMPI_sscanf(val, "%d%n", &stack, &nchar) == 1 && size_t(nchar) == VMPI_strlen(val) && stack >= 0) {
                        settings.markstackAllowance = int32_t(stack);
                    }
                    else
                    {
                        avmplus::AvmLog("Bad argument to -gcstack\n");
                        usage();
                    }
                }
#endif
                else if (!VMPI_strcmp(arg, "-log")) {
                    settings.do_log = true;
                }
#ifdef VMCFG_EVAL
                else if (!VMPI_strcmp(arg, "-repl")) {
                    settings.do_repl = true;
                }
#endif /* VMCFG_EVAL */
#ifdef VMCFG_WORKERTHREADS
                else if (!VMPI_strcmp(arg, "-workers") && i+1 < argc ) {
                    const char *val = argv[++i];
                    int nchar;
                    if (val == NULL)
                        val = "";
                    if (VMPI_sscanf(val, "%d,%d,%d%n", &settings.numworkers, &settings.numthreads, &settings.repeats, &nchar) != 3)
                        if (VMPI_sscanf(val, "%d,%d%n", &settings.numworkers, &settings.numthreads, &nchar) != 2) {
                            avmplus::AvmLog("Bad value to -workers: %s\n", val);
                            usage();
                        }
                    if (settings.numthreads < 1 ||
                        settings.numworkers < settings.numthreads ||
                        settings.repeats < 1 ||
                        size_t(nchar) != VMPI_strlen(val)) {
                        avmplus::AvmLog("Bad value to -workers: %s\n", val);
                        usage();
                    }
                }
#endif // VMCFG_WORKERTHREADS
#ifdef AVMPLUS_WIN32
                else if (!VMPI_strcmp(arg, "-error")) {
                    show_error = true;
#ifndef UNDER_CE
                    SetErrorMode(0);  // set to default
#endif
                }
#endif // AVMPLUS_WIN32
#ifdef DEBUGGER
                else if (!VMPI_strcmp(arg, "-d")) {
                    settings.enter_debugger_on_launch = true;
                }
#endif /* DEBUGGER */
                else if (!VMPI_strcmp(arg, "-api") && i+1 < argc) {
                    if (!avmplus::AvmCore::parseApiVersion(argv[i+1], settings.apiVersion, settings.apiVersionSeries))
                    {
                        avmplus::AvmLog("Unknown api version'%s'\n", argv[i+1]);
                        usage();
                    }
                    i++;
                }
                else if (VMPI_strcmp(arg, "-swfversion") == 0 && i+1 < argc) {
                    int j = avmplus::BugCompatibility::VersionCount;
                    unsigned swfVersion;
                    int nchar;
                    const char* val = argv[++i];
                    if (VMPI_sscanf(val, "%u%n", &swfVersion, &nchar) == 1 && size_t(nchar) == VMPI_strlen(val))
                    {
                        for (j = 0; j < avmplus::BugCompatibility::VersionCount; ++j)
                        {
                            if (avmplus::BugCompatibility::kNames[j] == swfVersion)
                            {
                                settings.swfVersion = (avmplus::BugCompatibility::Version)j;
                                break;
                            }
                        }
                    }
                    if (j == avmplus::BugCompatibility::VersionCount) {
                        avmplus::AvmLog("Unrecognized -swfversion version %s\n", val);
                        usage();
                    }
                }
                else {
                    // Unrecognized command line option
                    avmplus::AvmLog("Unrecognized option %s\n", arg);
                    usage();
                }
            }
            else {
                if (settings.filenames == NULL)
                    settings.filenames = &argv[i];
            }
        }

        if (settings.filenames == NULL)
            settings.filenames = &argv[argc];

        if (settings.numfiles == -1)
            settings.numfiles = int(&argv[argc] - settings.filenames);

        if (settings.arguments == NULL) {
            settings.arguments = &argv[argc];
            settings.numargs = 0;
        }

        AvmAssert(settings.filenames != NULL && settings.numfiles != -1);
        AvmAssert(settings.arguments != NULL && settings.numargs != -1);

        if (print_version)
        {
            avmplus::AvmLog("shell " AVMPLUS_VERSION_USER " " AVMPLUS_BIN_TYPE );
            if (RUNNING_ON_VALGRIND)
                avmplus::AvmLog("-valgrind");
            avmplus::AvmLog(" build " AVMPLUS_BUILD_CODE "\n");
#ifdef AVMPLUS_DESC_STRING
        if (VMPI_strcmp(AVMPLUS_DESC_STRING, ""))
        {
            avmplus::AvmLog("Description: " AVMPLUS_DESC_STRING "\n");
        }
#endif
            avmplus::AvmLog("features %s\n", avmfeatures);
            Platform::GetInstance()->exit(1);
        }

        // Vetting the options

#ifdef AVMSHELL_PROJECTOR_SUPPORT
        if (settings.do_projector) {
            if (settings.do_selftest || settings.do_repl || settings.numfiles > 0) {
                avmplus::AvmLog("Projector files can't be used with -repl, -Dselftest, or program file arguments.\n");
                usage();
            }
            if (settings.numthreads > 1 || settings.numworkers > 1) {
                avmplus::AvmLog("A projector requires exactly one worker on one thread.\n");
                usage();
            }
            return;
        }
#endif

#ifndef VMCFG_AOT
        if (settings.numfiles == 0) {
            // no files, so we need something more
            if (!settings.do_selftest && !settings.do_repl) {
                avmplus::AvmLog("You must provide input files, -repl, or -Dselftest, or the executable must be a projector file.\n");
                usage();
            }
        }
#endif

#ifdef VMCFG_EVAL
        if (settings.do_repl)
        {
            if (settings.numthreads > 1 || settings.numworkers > 1) {
                avmplus::AvmLog("The REPL requires exactly one worker on one thread.\n");
                usage();
            }
        }
#endif

#ifdef VMCFG_SELFTEST
        if (settings.do_selftest)
        {
            // Presumably we'd want to use the selftest harness to test multiple workers eventually.
            if (settings.numthreads > 1 || settings.numworkers > 1 || settings.numfiles > 0) {
                avmplus::AvmLog("The selftest harness requires exactly one worker on one thread, and no input files.\n");
                usage();
            }
        }
#endif
    }

    // Does not return

    /*static*/
    void Shell::usage()
    {
        avmplus::AvmLog("avmplus shell " AVMPLUS_VERSION_USER " " AVMPLUS_BIN_TYPE " build " AVMPLUS_BUILD_CODE "\n\n");
#ifdef AVMPLUS_DESC_STRING
        if (VMPI_strcmp(AVMPLUS_DESC_STRING, ""))
        {
            avmplus::AvmLog("Description: " AVMPLUS_DESC_STRING "\n\n");
        }
#endif
        avmplus::AvmLog("usage: avmplus\n");
#ifdef DEBUGGER
        avmplus::AvmLog("          [-d]          enter debugger on start\n");
#endif
        avmplus::AvmLog("          [-memstats]   generate statistics on memory usage\n");
        avmplus::AvmLog("          [-memstats-verbose]\n"
               "                        generate more statistics on memory usage\n");
        avmplus::AvmLog("          [-memlimit d] limit the heap size to d pages\n");
        avmplus::AvmLog("          [-eagersweep] sweep the heap synchronously at the end of GC;\n"
               "                        improves usage statistics.\n");
#ifdef MMGC_POLICY_PROFILING
        avmplus::AvmLog("          [-gcbehavior] summarize GC behavior and policy, after every gc\n");
        avmplus::AvmLog("          [-gcsummary]  summarize GC behavior and policy, at end only\n");
#endif
        avmplus::AvmLog("          [-load L,B, ...\n"
               "                        GC load factor L up to a post-GC heap size of B megabytes.\n"
               "                        Up to %d pairs can be accommodated, the limit for the last pair\n"
               "                        will be ignored and can be omitted\n", int(MMgc::GCHeapConfig::kNumLoadFactors));
        avmplus::AvmLog("          [-loadCeiling X] GC load multiplier ceiling (default 1.0)\n");
        avmplus::AvmLog("          [-gcwork G]   Max fraction of time (default 0.25) we're willing to spend in GC\n");
        avmplus::AvmLog("          [-stack N]    Stack size in bytes (will be honored approximately).\n"
               "                        Be aware of the stack margin: %u\n", avmshell::kStackMargin);
#ifdef MMGC_MARKSTACK_ALLOWANCE
        avmplus::AvmLog("          [-gcstack N]  Mark stack size allowance (# of segments), for testing.\n");
#endif
        avmplus::AvmLog("          [-cache_bindings N]   size of bindings cache (0 = unlimited)\n");
        avmplus::AvmLog("          [-cache_metadata N]   size of metadata cache (0 = unlimited)\n");
        avmplus::AvmLog("          [-cache_methods  N]   size of method cache (0 = unlimited)\n");
        avmplus::AvmLog("          [-Dgreedy]    collect before every allocation\n");
        avmplus::AvmLog("          [-Dnogc]      don't collect (including DRC)\n");
        avmplus::AvmLog("          [-Dnodrc]     don't use DRC (only use mark/sweep)\n");
        avmplus::AvmLog("          [-Ddrcvalidation]     Perform a mark before each Reap to seek out reachable zero count items\n");
#ifdef VMCFG_SELECTABLE_EXACT_TRACING
        avmplus::AvmLog("          [-Dnoexactgc] disable exact tracing\n");
#endif
        avmplus::AvmLog("          [-Dnoincgc]   don't use incremental collection\n");
        avmplus::AvmLog("          [-Dnodebugger] do not initialize the debugger (in DEBUGGER builds)\n");
        avmplus::AvmLog("          [-Dgcthreshold N] lower bound on allocation budget, in blocks, between collection completions\n");
        avmplus::AvmLog("          [-Dnofixedcheck]  don't check FixedMalloc deallocations for correctness (sometimes expensive)\n");
#ifdef DEBUGGER
        avmplus::AvmLog("          [-Dastrace N] display AS execution information, where N is [1..4]\n");
        avmplus::AvmLog("          [-Dlanguage l] localize runtime errors, languages are:\n");
        avmplus::AvmLog("                        en,de,es,fr,it,ja,ko,zh-CN,zh-TW\n");
#endif
#ifdef AVMPLUS_VERBOSE
        avmplus::AvmLog("          [-Dverbose[=[parse,verify,interp,traits,builtins,memstats,sweep,occupancy,execpolicy"
#  ifdef VMCFG_NANOJIT
               ",jit,opt,regs,raw,bytes,lir,lircfg[-bb|-ins]"
#  endif
               "]]\n");
        avmplus::AvmLog("                        With no options, enables extreme! output mode.  Otherwise the\n");
        avmplus::AvmLog("                        options are mostly self-descriptive except for the following: \n");
        avmplus::AvmLog("                           builtins - includes output from builtin methods\n");
        avmplus::AvmLog("                           memstats - generate statistics on memory usage \n");
        avmplus::AvmLog("                           sweep - [memstats] include detailed sweep information \n");
        avmplus::AvmLog("                           occupancy - [memstats] include occupancy bit graph \n");
        avmplus::AvmLog("                           execpolicy - shows which execution method (interpretation, compilation) was chosen and why \n");
#  ifdef VMCFG_NANOJIT
        avmplus::AvmLog("                           jit - output LIR as it is generated, and final assembly code\n");
        avmplus::AvmLog("                           lir - [jit] only write out the LIR instructions\n");
        avmplus::AvmLog("                           opt - [jit] show details about each optimization pass\n");
        avmplus::AvmLog("                           regs - [jit] show register allocation state after each assembly instruction\n");
        avmplus::AvmLog("                           raw - [jit] assembly code is displayed in raw (i.e unbuffered bottom-up) fashion. \n");
        avmplus::AvmLog("                           bytes - [jit] display the byte values of the assembly code. \n");
        avmplus::AvmLog("                           lircfg - [jit] write a gml representation of the LIR control-flow graph to a file.\n");
        avmplus::AvmLog("                           lircfg-bb - [jit] same as above, but each vertex is a basic blocks rather than an EBBs.\n");
        avmplus::AvmLog("                           lircfg-ins - [jit] same as above, but each vertex is an instruction.\n");
#  endif

        avmplus::AvmLog("                        Note that ordering matters for options with dependencies.  Dependencies \n");
        avmplus::AvmLog("                        are contained in [ ] For example, 'sweep' requires 'memstats' \n");
        avmplus::AvmLog("          [-Dverbose-only [{id|name|%%regex%%}]+ ]\n");
        avmplus::AvmLog("                        enable verbose output only for the methods identified. Valid only for verify,interp and jit\n");
        avmplus::AvmLog("                        options.  The method identification follows the same syntax as -policy (see below)\n");
        avmplus::AvmLog("                        e.g. -Dverbose-only \"Function/prime_val.as\\$1:prime,%%global%%\" \n");
#endif
#ifdef VMCFG_NANOJIT
        avmplus::AvmLog("          [-Dinterp]    do not generate machine code, interpret instead\n");
        avmplus::AvmLog("          [-Ojit]       use jit always, never interp (except when the jit fails)\n");
        avmplus::AvmLog("          [-Dcheckjitpageflags] check page protection flags on JIT memory allocation (sometimes expensive)\n");
#ifdef VMCFG_HALFMOON
        avmplus::AvmLog("          [-Dhalfmoon   use experimental 'halfmoon' jit (development only)\n");
#endif
        avmplus::AvmLog("          [-Djitordie]  use jit always, and abort when the jit fails\n");
        avmplus::AvmLog("          [-Dnocse]     disable CSE optimization\n");
        avmplus::AvmLog("          [-Dnoinline]  disable speculative inlining\n");
        avmplus::AvmLog("          [-jitharden]  enable jit hardening techniques\n");
        avmplus::AvmLog("          [-osr=T]      enable OSR with invocation threshold T; disable with -osr=0; default is -osr=%d\n",
                        avmplus::AvmCore::osr_threshold_default);
        avmplus::AvmLog("          [-prof=L]     enable jit profile level L; default 0=disabled; 1=function ranges, 2=functions+native asm)\n");
    #ifdef AVMPLUS_IA32
        avmplus::AvmLog("          [-Dnosse]     use FPU stack instead of SSE2 instructions\n");
        avmplus::AvmLog("          [-Dfixedesp]  pre-decrement stack for all needed call usage upon method entry\n");
    #endif
    #ifdef AVMPLUS_ARM
        avmplus::AvmLog("          [-Darm_arch N]  nanojit assumes ARMvN architecture (default=5)\n");
        avmplus::AvmLog("          [-Darm_vfp]     nanojit uses VFP rather than SoftFloat\n");
    #endif
    #ifdef VMCFG_COMPILEPOLICY
        avmplus::AvmLog("          [-policy [{interp|jit}={id|name|%%regex%%}]+ ]\n");
        avmplus::AvmLog("                        override the default policy using method identifiers (see -Dverbose=execpolicy,builtins)\n");
        avmplus::AvmLog("                        and/or method names and/or regular expressions. ids can be specified either\n");
        avmplus::AvmLog("                        singly or as a range, while names will attempt to match exactly. regex follows\n");
        avmplus::AvmLog("                        perl regular expression syntax. Note: rules are *not* checked for consistency\n");
        avmplus::AvmLog("                        e.g. -Dverbose=execpolicy,builtins -policy \"jit=%%(avmshell)|(global)%%,interp=Function/prime\",jit=-3,interp=5-9,jit=42\n");
    #endif /* VMCFG_COMPILEPOLICY */
#endif
#ifdef VMCFG_VERIFYALL
        avmplus::AvmLog("          [-Dverifyall] verify greedily instead of lazily\n");
        avmplus::AvmLog("          [-Dverifyonly] verify greedily and don't execute anything\n");
        avmplus::AvmLog("          [-Dverifyquiet] verify greedily and don't execute anything w/o re-queue msgs\n");
#endif
#ifdef VMCFG_SELFTEST
        avmplus::AvmLog("          [-Dselftest[=component,category,test]]\n");
        avmplus::AvmLog("                        run selftests\n");
#endif
        avmplus::AvmLog("          [-Dtimeout]   enforce maximum 15 seconds execution\n");
        avmplus::AvmLog("          [-Dversion]   print the version and the list of compiled-in features and then exit\n");
#ifdef AVMPLUS_WIN32
        avmplus::AvmLog("          [-error]      crash opens debug dialog, instead of dumping\n");
#endif
#ifdef VMCFG_EVAL
        avmplus::AvmLog("          [-repl]       read-eval-print mode\n");
#endif
#ifdef VMCFG_WORKERTHREADS
        avmplus::AvmLog("          [-workers W,T[,R]]\n");
        avmplus::AvmLog("                        Spawn W worker VMs on T threads where W >= T and T >= 1.\n");
        avmplus::AvmLog("                        The files provided are handed off to the workers in the order given,\n");
        avmplus::AvmLog("                        as workers become available, and these workers are scheduled onto threads\n");
        avmplus::AvmLog("                        in a deterministic order that prevents them from having affinity to a thread.\n");
        avmplus::AvmLog("                        To test this functionality you want many more files than workers and many more\n");
        avmplus::AvmLog("                        workers than threads, and at least two threads.\n");
        avmplus::AvmLog("                        If R > 0 is provided then it is the number of times the list of files is repeated.\n");
#endif
        avmplus::AvmLog("          [-swfHasAS3]  Exit with code 0 if the single file argument is a swf that contains a DoABC or DoABC2 tag,\n");
        avmplus::AvmLog("                        otherwise exit with code 1.  Do not execute or verify anything.\n");
        avmplus::AvmLog("          [-swfversion version]\n");
        avmplus::AvmLog("                        Run with a given bug-compatibility version in use by default.\n");
        avmplus::AvmLog("                        (This can be overridden on a per-ABC basis by embedded metadata.)\n");
        avmplus::AvmLog("                        Legal versions are:\n");
        for (int j = 0; j < avmplus::BugCompatibility::VersionCount; ++j)
        {
        avmplus::AvmLog("                            %d\n",avmplus::BugCompatibility::kNames[j]);
        }
        avmplus::AvmLog("          [-log]\n");
        avmplus::AvmLog("          [-api N] execute ABCs as version N (see api-versions.h)\n");
        avmplus::AvmLog("          [filename.{abc,swf} ...\n");
        avmplus::AvmLog("          [-- application argument ...]\n");
        Platform::GetInstance()->exit(1);
    }
}

namespace avmplus 
{
    Isolate* Isolate::newIsolate(int32_t desc, int32_t parentDesc, Aggregate* aggregate)
    {
        if (parentDesc == Isolate::INVALID_DESC) {
            return mmfx_new(avmshell::PrimordialShellIsolate(desc, parentDesc, aggregate));
        } else {
            return mmfx_new(avmshell::ShellIsolate(desc, parentDesc, aggregate));
        }
    }

	/*static*/
	bool MutexClass::getMutexSupported (GCRef<avmplus::Toplevel> toplevel)
	{
		AvmAssert(toplevel != NULL);
		
		return toplevel->core()->getIsolate() != NULL;;
	}
}

