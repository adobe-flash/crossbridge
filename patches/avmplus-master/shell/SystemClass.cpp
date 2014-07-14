/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmshell.h"
#include "Deopt.h"

namespace avmshell
{
    SystemClass::SystemClass(avmplus::VTable *cvtable)
        : avmplus::ClassClosure(cvtable)
    {
        ShellCore* core = (ShellCore*)this->core();
        if (core->systemClass == NULL) {
            core->systemClass = this;
        }

        createVanillaPrototype();

        // initialTime: support for getTimer
        // todo note this is currently routed to the performance counter
        // for benchmark purposes.
        #ifdef PERFORMANCE_GETTIMER
        initialTime = VMPI_getPerformanceCounter();
        #else
        initialTime = VMPI_getTime();
        #endif // PERFORMANCE_GETTIMER

    }

    SystemClass::~SystemClass()
    {
        initialTime = 0;
    }

    void SystemClass::exit(int status)
    {
    #if 1 // HACKERY
        _exit(status);
        #else
        if (core()->getIsolate()->getAggregate()->isPrimordial(core()->getIsolate()->getDesc())) {
            Platform::GetInstance()->exit(status);
        } else {
        GCRef<avmplus::ClassClosure> workerClass = toplevel()->workerClass();
        static_cast<ShellWorkerClass*>((avmplus::ClassClosure*)workerClass)->getCurrentWorker()->terminate();
    }
     #endif
    }

    void SystemClass::sleep(int32_t ms)
    {
        // Or do we try to make the all the isolate threads sleep? In a safepoint? and interruptibly?
        if (ms < 0) 
            ms = 0;
        vmbase::VMThread::sleep(ms);
    }

    int SystemClass::exec(avmplus::Stringp command)
    {
        if (!command) {
            toplevel()->throwArgumentError(kNullArgumentError, "command");
        }
        #ifdef UNDER_CE
        AvmAssert(0);
        return 0;
        #else
        avmplus::StUTF8String commandUTF8(command);
        return system(commandUTF8.c_str());
        #endif
    }

    avmplus::Stringp SystemClass::getAvmplusVersion()
    {
        return core()->newConstantStringLatin1(AVMPLUS_VERSION_USER " " AVMPLUS_BUILD_CODE);
    }

    avmplus::Stringp SystemClass::getFeatures()
    {
        return core()->newConstantStringLatin1(avmfeatures);
    }

    avmplus::Stringp SystemClass::getRunmode()
    {
        ShellCore* core = (ShellCore*)this->core();
        if (core->config.runmode == avmplus::RM_mixed)
            return core->newConstantStringLatin1("mixed");
        if (core->config.runmode == avmplus::RM_jit_all)
        {
            if (core->config.jitordie)
                return core->newConstantStringLatin1("jitordie");
            return core->newConstantStringLatin1("jit");
        }
        if (core->config.runmode == avmplus::RM_interp_all)
            return core->newConstantStringLatin1("interp");
        return core->newConstantStringLatin1("unknown");
    }

    void SystemClass::write(avmplus::Stringp s)
    {
        if (!s)
            toplevel()->throwArgumentError(kNullArgumentError, "string");
        core()->console << s;
    }

    void SystemClass::trace(avmplus::ArrayObject* a)
    {
        if (!a)
            toplevel()->throwArgumentError(kNullArgumentError, "array");
        avmplus::AvmCore* core = this->core();
        avmplus::PrintWriter& console = core->console;
        for (int i=0, n = a->getLength(); i < n; i++)
        {
            if (i > 0)
                console << ' ';
            avmplus::StringIndexer s(core->string(a->getUintProperty(i)));
            for (int j = 0; j < s->length(); j++)
            {
                wchar c = s[j];
                // '\r' gets converted into '\n'
                // '\n' is left alone
                // '\r\n' is left alone
                if (c == '\r')
                {
                    if (((j+1) < s->length()) && s[j+1] == '\n')
                    {
                        console << '\r';
                        j++;
                    }

                    console << '\n';
                }
                else
                {
                    console << c;
                }
            }
        }
        console << '\n';
    }

    void SystemClass::debugger()
    {
        #ifdef DEBUGGER
        if (core()->debugger())
            core()->debugger()->enterDebugger();
        #endif
    }

    bool SystemClass::isDebugger()
    {
        #ifdef DEBUGGER
        return core()->debugger() != NULL;
        #else
        return false;
        #endif
    }

    unsigned SystemClass::getTimer()
    {
#ifdef PERFORMANCE_GETTIMER
        double time = ((double) (VMPI_getPerformanceCounter() - initialTime) * 1000.0 /
                       (double)VMPI_getPerformanceFrequency());
        return (uint32_t)time;
#else
        return (uint32_t)(VMPI_getTime() - initialTime);
#endif /* PERFORMANCE_GETTIMER */

    }

    double SystemClass::getNanosecondTimer()
    {
        return ((VMPI_getPerformanceCounter() - initialTime) * 1e9)
            / VMPI_getPerformanceFrequency();
    }

    int SystemClass::user_argc;
    char **SystemClass::user_argv;

    avmplus::ArrayObject * SystemClass::getArgv()
    {
        // get VTable for avmplus.System
        avmplus::Toplevel *toplevel = this->toplevel();
        avmplus::AvmCore *core = this->core();

        avmplus::ArrayObject *array = toplevel->arrayClass()->newArray();
        for(int i=0; i<user_argc;i++)
            array->setUintProperty(i, core->newStringUTF8(user_argv[i])->atom());

        return array;
    }

    extern "C" char **environ;

    avmplus::ArrayObject * SystemClass::getEnviron()
    {
        // get VTable for avmplus.System
        avmplus::Toplevel *toplevel = this->toplevel();
        avmplus::AvmCore *core = this->core();

        avmplus::ArrayObject *array = toplevel->arrayClass()->newArray();
	char **cur = environ;
	int i = 0;
	while(*cur)
	{
            array->setUintProperty(i, core->newStringUTF8(*cur)->atom());
	    i++;
	    cur++;
	}

        return array;
    }

    avmplus::Stringp SystemClass::readLine()
    {
        avmplus::AvmCore* core = this->core();
        avmplus::Stringp s = core->kEmptyString;
        wchar wc[64];
        int i=0;
        for (int c = getchar(); c != '\n' && c != EOF; c = getchar())
        {
            wc[i++] = (wchar)c;
            if (i == 63) {
                wc[i] = 0;
                s = s->append16(wc);
                i = 0;
            }
        }
        if (i > 0) {
            wc[i] = 0;
            s = s->append16(wc);
        }
        return s;
    }

    double SystemClass::get_totalMemory()
    {
        MMgc::GCHeap* gcheap = MMgc::GCHeap::GetGCHeap();
        // Bugzilla 678975: use GetUsedHeapSize for compatibility with Player
        return double(gcheap->GetUsedHeapSize() * MMgc::GCHeap::kBlockSize);
    }

    double SystemClass::get_freeMemory()
    {
        MMgc::GCHeap* gcheap = MMgc::GCHeap::GetGCHeap();
        return double(gcheap->GetFreeHeapSize() * MMgc::GCHeap::kBlockSize);
    }

    double SystemClass::get_privateMemory()
    {
        return double(AVMPI_getPrivateResidentPageCount() * VMPI_getVMPageSize());
    }

    int32_t SystemClass::get_swfVersion()
    {
        ShellCore* core = (ShellCore*)this->core();
        avmplus::BugCompatibility::Version v = core->getDefaultBugCompatibilityVersion();
        AvmAssert(v >= 0 && v < avmplus::BugCompatibility::VersionCount);
        return avmplus::BugCompatibility::kNames[v];
    }

    int32_t SystemClass::get_apiVersion()
    {
        ShellCore* core = (ShellCore*)this->core();
        return core->defaultAPIVersion;
    }

    void SystemClass::forceFullCollection()
    {
        core()->GetGC()->Collect();
    }

    void SystemClass::queueCollection()
    {
        core()->GetGC()->QueueCollection();
    }

    bool SystemClass::isGlobal(avmplus::Atom o)
    {
        return avmplus::AvmCore::isObject(o) ? avmplus::AvmCore::atomToScriptObject(o)->isGlobalObject() : false;
    }

    void SystemClass::disposeXML(avmplus::XMLObject *xmlObject)
    {
        if(xmlObject)
            xmlObject->dispose();
    }

    void SystemClass::pauseForGCIfCollectionImminent(double imminence)
    {
        if( avmplus::MathUtils::isNaN(imminence) )
            imminence = 0.75;
        core()->GetGC()->Collect(imminence);
    }
    
    void SystemClass::runInSafepoint(avmplus::FunctionObject* code)
    {
        class Task: public vmbase::SafepointTask {
            avmplus::FunctionObject* m_code;
        public:
            Task(avmplus::FunctionObject* code): m_code(code) {}
            void run() {
                avmplus::Atom argv[] = {avmplus::nullObjectAtom};
                m_code->call(0, argv);
            }
        };
        Task task(code);
        core()->getIsolate()->getAggregate()->safepointManager()->requestSafepointTask(task);
    }

    /*static*/ void FASTCALL CheckBaseClass::preCreateInstanceCheck(avmplus::ClassClosure* cls)
    {
        avmplus::Multiname qname(cls->traits()->ns(), cls->traits()->name());
        // Deliberately throw a weird, non-sequitur error here so that we
        // can distinguish this from construct="none" in the acceptance tests.
        cls->toplevel()->argumentErrorClass()->throwError(kNotImplementedError, cls->core()->toErrorString(&qname));
    }

    bool SystemClass::is64bit()
    {
        #ifdef AVMPLUS_64BIT
            return true;
        #else
            return false;
        #endif
    }

    bool SystemClass::isIntptr(avmplus::Atom a)
    {
        return atomKind(a) == avmplus::AtomConstants::kIntptrType;
    }

    avmplus::Atom SystemClass::canonicalizeNumber(avmplus::Atom a)
    {
        if (atomKind(a) == avmplus::AtomConstants::kDoubleType) {
            double val = *((double*)atomPtr(a));
            intptr_t intval = intptr_t(val);
            if (double(intval) == val && !(val == 0 && avmplus::MathUtils::isNegZero(val))) {
                // Atom is double representing an integer value that will fit in intptr_t.
                if (avmplus::atomIsValidIntptrValue(intval)) {
                    // The intptr_t value will also fit in kIntptrType atom, with tag.
                    return avmplus::atomFromIntptrValue(intval);
                }
            }
        }
        return a;
    }

    // Debug scaffolding for deoptimization.
    // Deoptimize method invoked in frame K levels above our caller.
    // Throws if no such frame exists, but is silently ignored if
    // the method is not currently compiled.

    void SystemClass::deopt(int32_t k)
    {
#ifdef VMCFG_HALFMOON
        using avmplus::Deoptimizer;
        if (!Deoptimizer::deoptAncestor(core(), k))
            toplevel()->throwArgumentError(kNullArgumentError, "frame number");
#endif
        (void)k;
    }
}
