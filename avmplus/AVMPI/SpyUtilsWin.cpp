/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

#ifdef MMGC_MEMORY_PROFILER

#include <io.h>

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

static bool spyRunning=false;
static SignalData *sig_data;
static vmpi_spin_lock_t lock;

DWORD WINAPI WaitForMemorySignal(LPVOID)
{
    while(spyRunning) {
        WaitForSingleObject(sig_data->eventHandle, INFINITE);
        if(spyRunning)
            *(sig_data->profilerAddr) = true;
    }
    CloseHandle(sig_data->eventHandle);
    delete sig_data;
    return 0;
}

void WriteOnNamedSignal(const char *name, uint32_t *addr)
{
    HANDLE m_namedSharedObject;

    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;
    sa.lpSecurityDescriptor = &sd;

    m_namedSharedObject = CreateEventA(&sa, FALSE, FALSE, name);
    if(m_namedSharedObject == NULL){
        LPVOID lpMsgBuf;
        FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPSTR) &lpMsgBuf, 0, NULL );
        fputs((const char*)lpMsgBuf, stderr);
        return;
    }
    sig_data = new SignalData(addr, m_namedSharedObject);
    spyRunning = true;
    CreateThread(NULL, 0, WaitForMemorySignal, NULL, 0, NULL);
}

#include "windows.h"

FILE *HandleToStream(void *handle)
{
    return _fdopen(_open_osfhandle((intptr_t)handle, 0), "w");
}

void* OpenAndConnectToNamedPipe(const char *pipeName)
{
    char name[256];
    VMPI_snprintf(name, sizeof(name), "\\\\.\\pipe\\%s", pipeName);

    HANDLE pipe = CreateNamedPipeA((LPCSTR)name, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 4096, 4096, 100, NULL);
    ConnectNamedPipe(pipe, NULL);
    return (void*)pipe;
}


void CloseNamedPipe(void *handle)
{
    FlushFileBuffers(handle);
    CloseHandle((HANDLE)handle);
}

static uint32_t mmgc_spy_signal = 0;

//log redirector function for outputting log messages to the spy
FILE* spyStream = NULL;

void SpyLog(const char* message)
{
    fprintf(spyStream, "%s", message);
}

typedef void (*LoggingFunction)(const char*);
extern LoggingFunction GetCurrentLogFunction();
extern void RedirectLogOutput(void (*)(const char*));

void AVMPI_spyCallback()
{
    if(mmgc_spy_signal)
    {
        VMPI_lockAcquire(&lock);
        if(mmgc_spy_signal)
        {
            mmgc_spy_signal = 0;

            void *pipe = OpenAndConnectToNamedPipe("MMgc_Spy");

            spyStream = HandleToStream(pipe);
            GCAssert(spyStream != NULL);
            LoggingFunction oldLogFunc = GetCurrentLogFunction();
            RedirectLogOutput(SpyLog);

            MMgc::GCHeap::GetGCHeap()->DumpMemoryInfo();

            fflush(spyStream);

            CloseNamedPipe(pipe);
            RedirectLogOutput(oldLogFunc);
            spyStream = NULL;
        }
        VMPI_lockRelease(&lock);
    }
}

bool AVMPI_spySetup()
{
    VMPI_lockInit(&lock);
    WriteOnNamedSignal("MMgc::MemoryProfiler::DumpFatties", &mmgc_spy_signal);
    return true;
}

void AVMPI_spyTeardown()
{
    VMPI_lockDestroy(&lock);
    spyRunning = false;
}

bool AVMPI_hasSymbols()
{
    return true;
}

#endif //MMGC_MEMORY_PROFILER
