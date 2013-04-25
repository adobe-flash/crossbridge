/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

#ifdef MMGC_MEMORY_PROFILER

#include <Msgqueue.h>

/**
* compiled with the /W4 warning level
* which is quite picky.  Disable warnings we don't care about.
*/
#ifdef _MSC_VER
#pragma warning(disable:4127) //conditional expression is constant
#endif

class SignalData : public MMgc::GCAllocObject
{
public:
    SignalData(uint32_t *addr, HANDLE handle)
        : profilerAddr(addr), eventHandle(handle) {}
    uint32_t *profilerAddr;
    HANDLE eventHandle;
};

void WriteOnNamedSignal(const char* name, uint32_t *addr);

static SignalData *sig_data=NULL;
static bool spyRunning=false;
static vmpi_spin_lock_t lock;

DWORD WINAPI WaitForMemorySignal(LPVOID)
{
    SignalData *sd = sig_data;
    while(spyRunning) {
        WaitForSingleObject(sig_data->eventHandle, INFINITE);
        // For some reason ReadMsgQueue does not clear the signal on the handle (even though it should), and we
        // end up setting the profilerAddr constantly, which makes the VM hang, since all it is doing is writing out profile data.
        // so we have to call CloseMsgQueue, and then open a new queue, and wait on that.
        //char buff[256];
        //DWORD bytesread;
        //ReadMsgQueue(sig_data->eventHandle, buff, 256, &bytesread, INFINITE, 0);
        CloseMsgQueue(sd->eventHandle);
        if(spyRunning) {
            VMPI_lockAcquire(&lock);
            *(sig_data->profilerAddr) = true;
            VMPI_lockRelease(&lock);
            WriteOnNamedSignal("MMgc::MemoryProfiler::DumpFatties", sd->profilerAddr);
            break;
        }
    }
    delete sd;
    return 0;
}

void WriteOnNamedSignal(const char *name, uint32_t *addr)
{
    HANDLE m_namedSharedObject;

    MSGQUEUEOPTIONS msgopts;

    msgopts.dwFlags = MSGQUEUE_NOPRECOMMIT;
    msgopts.dwMaxMessages = 1;
    msgopts.cbMaxMessage = 256;
    msgopts.bReadAccess = TRUE;
    msgopts.dwSize = sizeof(MSGQUEUEOPTIONS);

    WCHAR wName[256];

    MultiByteToWideChar(CP_ACP, 0, name, -1, wName, 256);

    m_namedSharedObject = CreateMsgQueue(wName, &msgopts);

    sig_data = new SignalData(addr, m_namedSharedObject);
    spyRunning = true;
    CreateThread(NULL, 0, WaitForMemorySignal, NULL, 0, NULL);
}

#include "windows.h"

static uint32_t mmgc_spy_signal = 0;

FILE* spyStream = NULL;

void SpyLog(const char* message)
{
    fprintf(spyStream, "%s", message);
}

typedef void (*LoggingFunction)(const char*);
extern LoggingFunction GetCurrentLogFunction();
extern void RedirectLogOutput(void (*)(const char*));

void VMPI_spyCallback()
{
    if(mmgc_spy_signal)
    {
        VMPI_lockAcquire(&lock);
        if(mmgc_spy_signal)
        {
            mmgc_spy_signal = 0;

            spyStream = fopen("Temp\\gcstats.txt", "w");

            GCAssert(spyStream != NULL);
            LoggingFunction oldLogFunc = GetCurrentLogFunction();
            RedirectLogOutput(SpyLog);

            MMgc::GCHeap::GetGCHeap()->DumpMemoryInfo();

            fflush(spyStream);

            fclose(spyStream);
            RedirectLogOutput(oldLogFunc);
            spyStream = NULL;
        }
        VMPI_lockRelase(&lock);
    }
}

bool VMPI_spySetup()
{
    VMPI_lockInit(&lock);
    WriteOnNamedSignal("MMgc::MemoryProfiler::DumpFatties", &mmgc_spy_signal);
    return true;
}

void VMPI_spyTeardown()
{
    VMPI_lockDestroy(&lock);
    spyRunning = false;
}

bool VMPI_hasSymbols()
{
    return false;
}

#endif //MMGC_MEMORY_PROFILER
