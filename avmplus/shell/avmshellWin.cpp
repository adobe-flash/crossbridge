/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//win32

#pragma warning(disable: 4201)

#include "avmshell.h"
#include "WinFile.h"
#include <Mmsystem.h>


namespace avmshell
{
#ifdef AVMPLUS_WIN32
    bool show_error = false;
#endif

    class WinPlatform : public Platform
    {
    public:
        WinPlatform()
        {
            // Increase the accuracy of getTimer and Date to 1 ms from the 16 ms default
            timeBeginPeriod(1);
        }

        virtual ~WinPlatform()
        {
            // Restore default timer accuracy
            timeEndPeriod(1);
        }
        virtual void exit(int code);

        virtual File* createFile();
        virtual void destroyFile(File* file);

        virtual void initializeLogging(const char* filename);

        virtual int logMessage(const char* message);
        virtual char* getUserInput(char* buffer, int bufferSize);

        virtual void setTimer(int seconds, AvmTimerCallback callback, void* callbackData);
        virtual uintptr_t getMainThreadStackLimit();
    };

    struct TimerCallbackInfo
    {
        AvmTimerCallback callbackFunc;
        void* userData;
    };

    void CALLBACK TimeoutProc(UINT /*uTimerID*/,
                              UINT /*uMsg*/,
                              DWORD_PTR dwUser,
                              DWORD_PTR /*dw1*/,
                              DWORD_PTR /*dw2*/)
    {
        TimerCallbackInfo* info = (TimerCallbackInfo*)dwUser;
        info->callbackFunc(info->userData);
        delete info;
    }

    void WinPlatform::exit(int code)
    {
        ::exit(code);
    }

    File* WinPlatform::createFile()
    {
        return new WinFile();
    }

    void WinPlatform::destroyFile(File* file)
    {
        delete file;
    }

    int WinPlatform::logMessage(const char* message)
    {
        return fprintf(stdout, "%s", message);
    }

    char* WinPlatform::getUserInput(char* buffer, int bufferSize)
    {
        fflush(stdout);
        if (fgets(buffer, bufferSize, stdin) == NULL) {
            // EOF or error
            *buffer = 0;
            return NULL;
        }
        return buffer;
    }

    void WinPlatform::setTimer(int seconds, AvmTimerCallback callback, void* callbackData)
    {
        TimerCallbackInfo* info = new TimerCallbackInfo;
        info->callbackFunc = callback;
        info->userData = callbackData;

        timeSetEvent(seconds*1000,
            seconds*1000,
            (LPTIMECALLBACK)TimeoutProc,
            (DWORD_PTR)info,
            TIME_ONESHOT);
    }

    uintptr_t WinPlatform::getMainThreadStackLimit()
    {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);

        int dummy;
        uintptr_t sp = (uintptr_t)(&dummy);
        sp &= ~uintptr_t(sysinfo.dwPageSize-1);

        MEMORY_BASIC_INFORMATION buf;
        if (VirtualQuery((void*)sp, &buf, sizeof(buf)) == sizeof(buf)) {
            return (uintptr_t)buf.AllocationBase + avmshell::kStackMargin;
        }

        return 0;
    }

    #if !defined (AVMPLUS_ARM)

        #include "dbghelp.h"

        unsigned long CrashFilter(LPEXCEPTION_POINTERS pException, int exceptionCode)
        {
            unsigned long result;
            if ((result = UnhandledExceptionFilter(pException)) != EXCEPTION_EXECUTE_HANDLER)
            {
                return result;
            }
            else if (avmshell::show_error)
            {
                // if -error option dont do a dump
                return EXCEPTION_CONTINUE_SEARCH;
            }

            avmplus::AvmLog("avmplus crash: exception 0x%08lX occurred\n", exceptionCode);

            typedef BOOL (WINAPI *MINIDUMP_WRITE_DUMP)(
                HANDLE hProcess,
                DWORD ProcessId,
                HANDLE hFile,
                MINIDUMP_TYPE DumpType,
                PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                );

            HMODULE hDbgHelp = LoadLibraryW(L"dbghelp.dll");
            MINIDUMP_WRITE_DUMP MiniDumpWriteDump_ = NULL;
            if (hDbgHelp)
            {
                MiniDumpWriteDump_ =
                    (MINIDUMP_WRITE_DUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
            }

            if (MiniDumpWriteDump_)
            {
                MINIDUMP_EXCEPTION_INFORMATION  M;
                const WCHAR DumpPath[] = L"avmplusCrash.dmp";

                M.ThreadId = GetCurrentThreadId();
                M.ExceptionPointers = pException;
                M.ClientPointers = 0;

                avmplus::AvmLog("Writing minidump crash log to %ls\n", DumpPath);

                HANDLE hDumpFile = CreateFileW(DumpPath, GENERIC_WRITE, 0,
                    NULL, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, NULL);

                MiniDumpWriteDump_(GetCurrentProcess(),
                    GetCurrentProcessId(),
                    hDumpFile,
                    MiniDumpNormal,
                    (pException) ? &M : NULL, NULL, NULL);

                CloseHandle(hDumpFile);
            }
            else
            {
                avmplus::AvmLog("minidump not available, no crash log written.\n");
            }

            return result;
        }
    #endif //!AVMPLUS_ARM

        void WinPlatform::initializeLogging(const char* filename)
        {
        #if defined (UNDER_CE)
            int filenameLen = VMPI_strlen(filename);

            TCHAR* logname = new TCHAR[filenameLen+1];

            mbstowcs(logname, filename, filenameLen+1);

            _wfreopen(logname, L"w", stdout);

            delete [] logname;
        #else
            FILE *f = freopen(filename, "w", stdout);
            if (!f)
                avmplus::AvmLog("freopen %s failed.\n",filename);
        #endif /* UNDER_CE */
        }

} //namespace avmshell

avmshell::WinPlatform* gPlatformHandle = NULL;

avmshell::Platform* avmshell::Platform::GetInstance()
{
    AvmAssert(gPlatformHandle != NULL);
    return gPlatformHandle;
}

#if !defined (AVMPLUS_ARM)

    static int executeShell(int argc, char *argv[])
    {
        int code = 0;

        __try
        {
            code = avmshell::Shell::run(argc, argv);
            if (code == avmshell::OUT_OF_MEMORY)
                ::OutputDebugStringA("OUT OF MEMORY\n");
        }
        __except(avmshell::CrashFilter(GetExceptionInformation(), GetExceptionCode()))
        {
            code = -1;
        }

        return code;
    }

    int main(int argc, char *argv[])
    {
        SetErrorMode(SEM_NOGPFAULTERRORBOX);

        avmshell::WinPlatform platformInstance;
        gPlatformHandle = &platformInstance;

        int code = executeShell(argc, argv);

        return code;
    }

#endif /* !AVMPLUS_ARM */

#if defined (UNDER_CE) || defined (UNDER_RT)

    // TODO this is a hack until we learn how to determine stack top
    // in ARM
    int StackTop;

    int wmain(int argc, TCHAR *argv[])
    {
        int sp;
        StackTop = (int) &sp;

        avmshell::WinPlatform platformInstance;
        gPlatformHandle = &platformInstance;

        char** argArray = (char**) malloc(argc*sizeof(char*));
        for(int i=0;i<argc;++i)
        {
            int len = (wcslen(argv[i])+1) << 1;
            argArray[i] = (char*) malloc(len);
            wcstombs(argArray[i], argv[i], len);
        }
        int code = avmshell::Shell::run(argc, argArray);
        if (code == avmshell::OUT_OF_MEMORY)
            ::OutputDebugStringW(L"OUT OF MEMORY\n");

        for(int i=0;i<argc;++i)
        {
            free(argArray[i]);
        }
        free(argArray);

        return code;
    }

#endif
