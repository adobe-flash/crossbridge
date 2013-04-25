/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//mac

#include "avmshell.h"
#include "PosixPartialPlatform.h"

#include <sys/signal.h>
#include <unistd.h>
#include <sys/resource.h>

namespace avmshell
{
    class MacPlatform : public PosixPartialPlatform
    {
    public:
        MacPlatform(void* stackbase) : stackbase(stackbase) {}
        virtual ~MacPlatform() {}

        virtual void setTimer(int seconds, AvmTimerCallback callback, void* callbackData);
        virtual uintptr_t getMainThreadStackLimit();

    private:
        void* stackbase;
    };

    uintptr_t MacPlatform::getMainThreadStackLimit()
    {
        struct rlimit r;
        size_t stackheight = avmshell::kStackSizeFallbackValue;
        // https://bugzilla.mozilla.org/show_bug.cgi?id=504976: setting the height to kStackSizeFallbackValue
        // is not ideal if the stack is meant to be unlimited but is an OK workaround for the time being.
        if (getrlimit(RLIMIT_STACK, &r) == 0 && r.rlim_cur != RLIM_INFINITY)
            stackheight = size_t(r.rlim_cur);
        return uintptr_t(stackbase) - stackheight + avmshell::kStackMargin;
    }

    AvmTimerCallback pCallbackFunc = 0;
    void* pCallbackData = 0;

    void MacPlatform::setTimer(int seconds, AvmTimerCallback callback, void* callbackData)
    {
        extern void alarmProc(int);

        pCallbackFunc = callback;
        pCallbackData = callbackData;

        signal(SIGALRM, alarmProc);
        alarm(seconds);

    }

    void alarmProc(int /*signum*/)
    {
        pCallbackFunc(pCallbackData);
    }
}

avmshell::MacPlatform* gPlatformHandle = NULL;

avmshell::Platform* avmshell::Platform::GetInstance()
{
    AvmAssert(gPlatformHandle != NULL);
    return gPlatformHandle;
}

int main(int argc, char *argv[])
{
#ifdef VMCFG_MACH_EXCEPTIONS
    avmplus::GenericGuard::staticInit();
#endif

    char* dummy;
    avmshell::MacPlatform platformInstance(&dummy);
    gPlatformHandle = &platformInstance;

    int code = avmshell::Shell::run(argc, argv);
    if (code == avmshell::OUT_OF_MEMORY)
        write(1, "OUT OF MEMORY\n", 14);

#ifdef VMCFG_MACH_EXCEPTIONS
    avmplus::GenericGuard::staticDestroy();
#endif

    return code;
}
