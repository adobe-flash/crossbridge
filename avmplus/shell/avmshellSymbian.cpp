/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//unix/solaris

#include "avmshell.h"
#include "PosixPartialPlatform.h"

#include <e32base.h>
#include <e32std.h>

namespace avmshell
{
    class SymbianPlatform : public PosixPartialPlatform
    {
    public:
        SymbianPlatform(void* stackbase) : stackbase(stackbase), f(NULL) {}
        virtual ~SymbianPlatform() { fclose(f); }

        virtual void initializeLogging(const char* filename);
        virtual int logMessage(const char* message);
        
        virtual void exit(int /*code*/) {}

        virtual void setTimer(int seconds, AvmTimerCallback callback, void* callbackData);
        virtual uintptr_t getMainThreadStackLimit();
        
    private:
        void* stackbase;
        FILE *f;
    };
    
    void SymbianPlatform::initializeLogging(const char* filename)
    {
        f = freopen(filename, "w", stdout);
        if (!f)
            AvmLog("freopen %s failed.\n",filename);
    }
    
    int SymbianPlatform::logMessage(const char* message)
    {
        int ret = fprintf(stdout, "%s", message);
        fflush(stdout);
        return ret;
    }
    
    //AvmTimerCallback pCallbackFunc = 0;
    //void* pCallbackData = 0;
    
    uintptr_t SymbianPlatform::getMainThreadStackLimit()
    {
        TThreadStackInfo info;
        RThread currentThread;
        currentThread.StackInfo(info);
        return uintptr_t(info.iLimit) + avmshell::kStackMargin;
    }
    
    void SymbianPlatform::setTimer(int /*seconds*/, AvmTimerCallback /*callback*/, void* /*callbackData*/)
    {
        // TODO. Symbian Posix library does not have signals, timer should be implemented using other Symbian API.
    }
}

avmshell::SymbianPlatform* gPlatformHandle = NULL;

avmshell::Platform* avmshell::Platform::GetInstance()
{
    AvmAssert(gPlatformHandle != NULL);
    return gPlatformHandle;
}

_LIT(kProcessName, "AVMPLUSSHELL");

void RunTestL()
{
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    // Rename the process so TestShell can find it
    RProcess process;
    process.RenameMe(kProcessName);

    char* dummy;
    avmshell::SymbianPlatform platformInstance(&dummy);
    gPlatformHandle = &platformInstance;

    int argc = 3;
    char* argv[3];
    argv[0] = 0;
    argv[1] = "-log";
    argv[2] = "c:/data/avmplus_file.abc\0";

    /*int code = */avmshell::Shell::run(argc, argv);
    /*
    if (code == avmshell::OUT_OF_MEMORY) {
    write(1, "OUT OF MEMORY\n", 14);
    }
    */
    // Delete active scheduler
    CleanupStack::PopAndDestroy(scheduler);
}

GLDEF_C TInt E32Main()
{
//  __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt result = KErrNoMemory;
    if (cleanup)
    {
        TRAP(result, RunTestL());
        delete cleanup;
    }
//  __UHEAP_MARKEND;
    return result;
}
