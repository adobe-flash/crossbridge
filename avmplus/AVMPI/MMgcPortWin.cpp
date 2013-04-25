/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include <windows.h>

#include "MMgc.h"

#ifdef MMGC_MEMORY_PROFILER
    #include <malloc.h>
    #include <strsafe.h>
#ifndef UNDER_CE  // Not available on WinMo builds
    #include <DbgHelp.h>
    #include <io.h>
#endif
#endif

bool AVMPI_canMergeContiguousRegions()
{
    return false;
}

bool AVMPI_canCommitAlreadyCommittedMemory()
{
    return true;
}

bool AVMPI_useVirtualMemory()
{
    return true;
}

bool AVMPI_areNewPagesDirty()
{
    return false;
}

void* AVMPI_reserveMemoryRegion(void* address, size_t size)
{
    return VirtualAlloc(address,
                        size,
                        MEM_RESERVE
#ifdef _WIN64
                        | MEM_TOP_DOWN
#endif
                        ,
                        PAGE_NOACCESS);
}

bool AVMPI_releaseMemoryRegion(void* address, size_t /*size*/)
{
    return VirtualFree(address, 0, MEM_RELEASE) == TRUE;
}

static bool CommitMemory(void* address, size_t size)
{
    address = VirtualAlloc(address,
                           size,
                           MEM_COMMIT
#ifdef _WIN64
                           | MEM_TOP_DOWN
#endif //#ifdef _WIN64
                           ,
                           PAGE_READWRITE);
    return address != NULL;
}

bool AVMPI_commitMemory(void* address, size_t size)
{
    bool success = false;
    MEMORY_BASIC_INFORMATION mbi;
    do {
        VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
        size_t commitSize = size > mbi.RegionSize ? mbi.RegionSize : size;
        success = CommitMemory(address, commitSize);
        address = (char*)address + commitSize;
        size -= commitSize;
    } while(size > 0 && success);

    return success;
}

bool AVMPI_decommitMemory(char *address, size_t size)
{
    bool success = false;
    MEMORY_BASIC_INFORMATION mbi;
    do {
        VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
        size_t commitSize = size > mbi.RegionSize ? mbi.RegionSize : size;
        success = VirtualFree(address, commitSize, MEM_DECOMMIT) == TRUE;
        address += commitSize;
        size -= commitSize;
    } while(size > 0 && success);

    return success;
}

void* AVMPI_allocateAlignedMemory(size_t size)
{
    return VirtualAlloc(NULL, size, MEM_COMMIT
#ifdef _WIN64
                        | MEM_TOP_DOWN
#endif
                        , PAGE_READWRITE);
}

void AVMPI_releaseAlignedMemory(void* address)
{
    AVMPI_releaseMemoryRegion(address, 0);
}

#ifdef UNDER_CE
    typedef DWORD (*pTGetProcessIndexFromID)(HANDLE hProc);
    static pTGetProcessIndexFromID gGetID=NULL;
#if _WIN32_WCE>=0x600
    THIS WILL NOT WORK ON WINCE 6.0 AND ABOVE
#endif
#endif

#ifdef UNDER_CE

// The WinCE version of getPrivateResidentPageCount must do some specific things to get
// an accurate picture of private bytes due to how WinCE lays out memory for the process.
// see http://msdn.microsoft.com/en-us/library/bb331824.aspx for a desccription of how the memory is laid out.
// Note that we are running on Windows Mobile 6.0, but that is based on WinCE 5.0.
// Basically, first we walk the memory for the process slot, from 0x10000 to 0x2000000.  Then we walk the memory
// in the large memory area (0x42000000 - 0x80000000), as this is where gcheap allocates memory from.

size_t AVMPI_getPrivateResidentPageCount()
{
    void  *addr = (void*)(0x00010000);
    void  *endAddr = (void*)(0x02000000);

    size_t bytes=0;
    MEMORY_BASIC_INFORMATION mib;
    while(true)
    {
        size_t ret = VirtualQuery(addr, &mib, sizeof(MEMORY_BASIC_INFORMATION));
        if(ret == 0)
            break;

        if((mib.State & MEM_COMMIT))
            if ((DWORD)mib.BaseAddress + mib.RegionSize > (DWORD)endAddr)
                bytes += (DWORD)endAddr - (DWORD)mib.BaseAddress;
            else

            bytes += mib.RegionSize;

            addr = (void*) ((intptr_t)mib.BaseAddress + mib.RegionSize);
            if (addr>=endAddr)
                break;
    }

    MMgc::GCHeap* heap = MMgc::GCHeap::GetGCHeap();

    // We need to also walk the shared memory regions to make sure we
    // count the blocks we've allocated there
    MMgc::GCHeap::Region* curRegion = heap->lastRegion;
    if (curRegion)
        addr = curRegion->baseAddr;
    else
        addr = NULL;

    while (curRegion!=NULL)
    {
        addr = curRegion->baseAddr;
        if (addr < (void*)0x42000000)
        {
            // Not in the shared regions
            curRegion = curRegion->prev;
            continue;
        }

        while(true)
        {
            size_t ret = VirtualQuery(addr, &mib, sizeof(MEMORY_BASIC_INFORMATION));
            if(ret == 0)
                break;

            if((mib.State & MEM_COMMIT)) // && (mib.Type & MEM_PRIVATE))
            {
                if ((DWORD)mib.BaseAddress + mib.RegionSize > (DWORD)curRegion->reserveTop)
                    bytes += (DWORD)curRegion->reserveTop - (DWORD)mib.BaseAddress;
                else
                    bytes += mib.RegionSize;
            }

            addr = (void*) ((intptr_t)mib.BaseAddress + mib.RegionSize);

            if (addr>=curRegion->reserveTop)
                break;
        }
        curRegion = curRegion->prev;
    }

    return bytes / VMPI_getVMPageSize();
}

#else // UNDER_CE

size_t AVMPI_getPrivateResidentPageCount()
{
    void *addr = 0;
    size_t bytes=0;
    MEMORY_BASIC_INFORMATION mib;
    while(true)
    {
        size_t ret = VirtualQuery(addr, &mib, sizeof(MEMORY_BASIC_INFORMATION));
        if(ret == 0)
            break;

        if((mib.State & MEM_COMMIT) && (mib.Type & MEM_PRIVATE))
            bytes += mib.RegionSize;

            addr = (void*) ((intptr_t)mib.BaseAddress + mib.RegionSize);
    }

    return bytes / VMPI_getVMPageSize();
}
#endif //UNDER_CE

void AVMPI_cleanStack(size_t amt)
{
    void *space = alloca(amt);
    if(space)
    {
        VMPI_memset(space, 0, amt);
    }
}

uintptr_t AVMPI_getThreadStackBase()
{
    MEMORY_BASIC_INFORMATION __mib;
    VirtualQuery(&__mib, &__mib, sizeof(MEMORY_BASIC_INFORMATION));
    return (uintptr_t)__mib.BaseAddress + __mib.RegionSize;
}

#ifdef MMGC_MEMORY_PROFILER

#ifndef UNDER_CE
namespace MMgc
{
    // This relies on headers that don't exist in winmo builds

    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------

    // helper snarfed and simplified from main flash player code.
    // since we only need it here and only for debug, I didn't bother
    // migrating the whole thing.
    class DynamicLoadLibraryHelper
    {
    protected:
        DynamicLoadLibraryHelper(const char* p_dllName, bool p_required = true);
        virtual ~DynamicLoadLibraryHelper();

        FARPROC GetProc(const char* p_funcName, bool p_required = true);

    public:
        // note that this is only if any of the *required* ones failed;
        // some "optional" ones may be missing and still have this return true.
        bool AllRequiredItemsPresent() const { return m_allRequiredItemsPresent; }

    private:
        HMODULE m_lib;
        bool m_allRequiredItemsPresent;
    };

    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------

#define GETPROC(n) do { m_##n = (n##ProcPtr)GetProc(#n); } while (0)
#define GETPROC_OPTIONAL(n) do { m_##n = (n##ProcPtr)GetProc(#n, false); } while (0)

    // --------------------------------------------------------------------------
    DynamicLoadLibraryHelper::DynamicLoadLibraryHelper(const char* p_dllName, bool p_required) :
    m_lib(NULL),
        m_allRequiredItemsPresent(true) // assume the best
    {
        m_lib = ::LoadLibraryA(p_dllName);
        if (p_required && (m_lib == NULL || m_lib == INVALID_HANDLE_VALUE))
        {
            // don't assert here... it will trigger a DebugBreak(), which will crash
            // systems not running a debugger... and QE insists that they be able
            // to run Debug builds on debuggerless Win98 systems... (sigh)
            //GCAssertMsg(0, p_dllName);
            m_allRequiredItemsPresent = false;
        }
    }

    // --------------------------------------------------------------------------
    FARPROC DynamicLoadLibraryHelper::GetProc(const char* p_funcName, bool p_required)
    {
        FARPROC a_proc = NULL;
        if (m_lib != NULL && m_lib != INVALID_HANDLE_VALUE)
        {
            a_proc = ::GetProcAddress(m_lib, p_funcName);
        }
        if (p_required && a_proc == NULL)
        {
            // don't assert here... it will trigger a DebugBreak(), which will crash
            // systems not running a debugger... and QE insists that they be able
            // to run Debug builds on debuggerless Win98 systems... (sigh)
            //GCAssertMsg(0, p_funcName);
            m_allRequiredItemsPresent = false;
        }
        return a_proc;
    }

    // --------------------------------------------------------------------------
    DynamicLoadLibraryHelper::~DynamicLoadLibraryHelper()
    {
        if (m_lib != NULL && m_lib != INVALID_HANDLE_VALUE)
        {
            ::FreeLibrary(m_lib);
        }
    }

    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------

    // --------------------------------------------------------------------------
    class DbgHelpDllHelper : public DynamicLoadLibraryHelper
    {
    public:
        DbgHelpDllHelper();

    public:

        typedef BOOL (__stdcall *StackWalk64ProcPtr)(
            DWORD                             MachineType,
            HANDLE                            hProcess,
            HANDLE                            hThread,
            LPSTACKFRAME64                    StackFrame,
            PVOID                             ContextRecord,
            PREAD_PROCESS_MEMORY_ROUTINE64    ReadMemoryRoutine,
            PFUNCTION_TABLE_ACCESS_ROUTINE64  FunctionTableAccessRoutine,
            PGET_MODULE_BASE_ROUTINE64        GetModuleBaseRoutine,
            PTRANSLATE_ADDRESS_ROUTINE64      TranslateAddress
            );

        typedef PVOID (__stdcall *SymFunctionTableAccess64ProcPtr)(
            HANDLE  hProcess,
            DWORD64 AddrBase
            );

        typedef DWORD64 (__stdcall *SymGetModuleBase64ProcPtr)(
            HANDLE  hProcess,
            DWORD64 qwAddr
            );

        typedef BOOL (__stdcall *SymGetLineFromAddr64ProcPtr)(
            HANDLE                  hProcess,
            DWORD64                 qwAddr,
            PDWORD                  pdwDisplacement,
            PIMAGEHLP_LINE64        Line64
            );

        typedef BOOL (__stdcall *SymGetSymFromAddr64ProcPtr)(
            HANDLE                  hProcess,
            DWORD64                 qwAddr,
            PDWORD64                pdwDisplacement,
            PIMAGEHLP_SYMBOL64      Symbol
            );

        typedef BOOL (__stdcall *SymInitializeProcPtr)(
            HANDLE   hProcess,
            PSTR     UserSearchPath,
            BOOL     fInvadeProcess
            );

    public:
        StackWalk64ProcPtr                  m_StackWalk64;
        SymFunctionTableAccess64ProcPtr     m_SymFunctionTableAccess64;
        SymGetModuleBase64ProcPtr           m_SymGetModuleBase64;
        SymGetLineFromAddr64ProcPtr         m_SymGetLineFromAddr64;
        SymGetSymFromAddr64ProcPtr          m_SymGetSymFromAddr64;
        SymInitializeProcPtr                m_SymInitialize;
    };

    // --------------------------------------------------------------------------
    DbgHelpDllHelper::DbgHelpDllHelper() :
    DynamicLoadLibraryHelper("dbghelp.dll"),
        m_StackWalk64(NULL),
        m_SymFunctionTableAccess64(NULL),
        m_SymGetModuleBase64(NULL),
        m_SymGetLineFromAddr64(NULL),
        m_SymGetSymFromAddr64(NULL),
        m_SymInitialize(NULL)
    {
        GETPROC(StackWalk64);
        GETPROC(SymFunctionTableAccess64);
        GETPROC(SymGetModuleBase64);
        GETPROC(SymGetLineFromAddr64);
        GETPROC(SymGetSymFromAddr64);
        GETPROC(SymInitialize);
    }
}
    // declaring this statically will dynamically load the dll and procs
    // at startup, and never ever release them... if this ever becomes NON-debug
    // code, you might want to have a way to toss all this... but for _DEBUG
    // only, it should be fine
static MMgc::DbgHelpDllHelper g_DbgHelpDll;
#endif

    static const int MaxNameLength = 256;

#ifdef _WIN64
#define MACHINETYPE IMAGE_FILE_MACHINE_AMD64
#else
#define MACHINETYPE IMAGE_FILE_MACHINE_I386
#endif

    bool InitDbgHelp()
    {
        static vmpi_spin_lock_t lock;
        static bool inited = false;

        // We must hold the lock for the entire initialization process:
        //  - if we set inited to true and Release the lock then other
        //    threads may forge ahead without initialization having occured
        //  - if we leave it false and Release then other threads
        //    may try to perform initialization as well.
        MMGC_LOCK(lock);
        if(!inited) {
#ifndef UNDER_CE
            if(!g_DbgHelpDll.m_SymInitialize ||
                !(*g_DbgHelpDll.m_SymInitialize)(GetCurrentProcess(), NULL, true)) {
                    LPVOID lpMsgBuf;
                    if(FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (LPTSTR) &lpMsgBuf, 0, NULL ))
                    {
                        GCAssertMsg(false, "See lpMsgBuf");
                        LocalFree(lpMsgBuf);
                    }
                    return false;
            }
#endif // ifn UNDER_CE
            inited = true;
        }
        return true;
    }

#ifdef UNDER_CE
typedef ULONG NTAPI RtlCaptureStackBackTrace_Function(
    IN HANDLE hThrd,
    IN ULONG dwMaxFrames,
    OUT void* lpFrames,
    IN DWORD dwFlags,
    IN DWORD dwSkip);

static RtlCaptureStackBackTrace_Function* const RtlCaptureStackBackTrace_fn =
    (RtlCaptureStackBackTrace_Function*)
    GetProcAddress(LoadLibrary(_T("coredll.dll")), _T("GetThreadCallStack"));

typedef struct _CallSnapshot {
    DWORD dwReturnAddr;
} CallSnapshot;

#define STACKSNAP_EXTENDED_INFO 2

#else // UNDER_CE

typedef USHORT NTAPI RtlCaptureStackBackTrace_Function(
    IN ULONG frames_to_skip,
    IN ULONG frames_to_capture,
    OUT PVOID *backtrace,
    OUT PULONG backtrace_hash);

static RtlCaptureStackBackTrace_Function* const RtlCaptureStackBackTrace_fn =
    (RtlCaptureStackBackTrace_Function*)
    GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlCaptureStackBackTrace");

#endif // UNDER_CE

#ifdef UNDER_CE
#include <cmnintrin.h>
extern "C" unsigned long* get_frame_pointer();
#endif

    bool AVMPI_captureStackTrace(uintptr_t *buffer, size_t bufferSize, uint32_t framesToSkip)
    {
        if(RtlCaptureStackBackTrace_fn == NULL)
            return false;

#ifdef UNDER_CE

        CallSnapshot lpFrames[32];
        DWORD dwCnt = RtlCaptureStackBackTrace_fn(GetCurrentThread(), bufferSize >= 32 ? 32 : bufferSize, (void*)&lpFrames,0, framesToSkip);

        size_t i = 0;
        for( ; i < dwCnt && i < bufferSize ; ++i )
        {
            buffer[i] = lpFrames[i].dwReturnAddr;
        }

#else

        int num = RtlCaptureStackBackTrace_fn(framesToSkip, (uint32_t)(bufferSize - 1), (PVOID*)buffer, NULL);
        buffer[num] = 0;

#endif //UNDER_CE
        return true;
    }

    bool AVMPI_getFileAndLineInfoFromPC(uintptr_t pc, char *filenameBuffer, size_t bufferSize, uint32_t* lineNumber)
    {
#ifdef UNDER_CE
        (void)pc;
        (void)lineNumber;
        (void)filenameBuffer;
        (void)bufferSize;

        return false;
#else
        if(!InitDbgHelp())
            return false;

        // gleaned from IMAGEHLP_SYMBOL64 docs
        IMAGEHLP_SYMBOL64 *pSym = (IMAGEHLP_SYMBOL64 *) alloca(sizeof(IMAGEHLP_SYMBOL64) + MaxNameLength);
        pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        pSym->MaxNameLength = MaxNameLength;

        DWORD64 offsetFromSymbol;
        if(!g_DbgHelpDll.m_SymGetSymFromAddr64 ||
            !(*g_DbgHelpDll.m_SymGetSymFromAddr64)(GetCurrentProcess(), pc, &offsetFromSymbol, pSym)) {
                return false;
        }

        // get line
        IMAGEHLP_LINE64 line;
        VMPI_memset(&line, 0, sizeof(IMAGEHLP_LINE64));
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD offsetFromLine;
        if(!g_DbgHelpDll.m_SymGetLineFromAddr64 ||
            !(*g_DbgHelpDll.m_SymGetLineFromAddr64)(GetCurrentProcess(), pc, &offsetFromLine, &line)) {
                return false;
        }


        // success!
        char *fileName = line.FileName + VMPI_strlen(line.FileName);

        // skip everything up to last slash
        while(fileName > line.FileName && *fileName != '\\')
            fileName--;
        fileName++;

        StringCchPrintfA(filenameBuffer, bufferSize, "%s", fileName);
        *lineNumber = line.LineNumber;

        return true;
#endif // UNDER_CE
    }

    bool AVMPI_getFunctionNameFromPC(uintptr_t pc, char *buffer, size_t bufferSize)
    {
#ifdef UNDER_CE
        (void)pc;
        (void)buffer;
        (void)bufferSize;
        return false;
#else

        if(!InitDbgHelp())
            return false;

        // gleaned from IMAGEHLP_SYMBOL64 docs
        IMAGEHLP_SYMBOL64 *pSym = (IMAGEHLP_SYMBOL64 *) alloca(sizeof(IMAGEHLP_SYMBOL64) + MaxNameLength);
        pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        pSym->MaxNameLength = MaxNameLength;

        DWORD64 offsetFromSymbol;
        if(!g_DbgHelpDll.m_SymGetSymFromAddr64 ||
            !(*g_DbgHelpDll.m_SymGetSymFromAddr64)(GetCurrentProcess(), pc, &offsetFromSymbol, pSym)) {
                return false;
        }

        StringCchPrintfA(buffer, bufferSize, "%s", pSym->Name);
        //printf("%s\n", pSym->Name);
        return true;

#endif //UNDER_CE
    }

    void AVMPI_setupPCResolution() { }

    void AVMPI_desetupPCResolution() { }

    bool AVMPI_isMemoryProfilingEnabled()
    {
    #if defined (UNDER_CE) && defined(MMGC_MEMORY_PROFILER)
        return true;
    #else
        //read the mmgc profiling option switch
        const char *env = VMPI_getenv("MMGC_PROFILE");
        return (env && (VMPI_strncmp(env, "1", 1) == 0));
    #endif
    }

#endif // MMGC_MEMORY_PROFILER

// Constraint: nbytes must be a multiple of the VM page size.
//
// The returned memory will be aligned on a VM page boundary and cover
// an integral number of VM pages.  This is necessary in order for
// AVMPI_makeCodeMemoryExecutable to work properly - it too operates
// on entire VM pages.
//
// This function should be the same as for generic Posix platforms, if you
// fix a bug here be sure to fix the bug there.

void *AVMPI_allocateCodeMemory(size_t nbytes)
{
    MMgc::GCHeap* heap = MMgc::GCHeap::GetGCHeap();
    size_t pagesize = VMPI_getVMPageSize();
    
    if (nbytes % pagesize != 0) {
#ifdef DEBUG
        char buf[256];
        VMPI_snprintf(buf,
                      sizeof(buf),
                      "AVMPI_allocateCodeMemory invariants violated: request=%lu pagesize=%lu\nAborting.\n",
                      (unsigned long)nbytes,
                      (unsigned long)pagesize);
        VMPI_log(buf);
#endif
        VMPI_abort();
    }
    
    size_t nblocks = nbytes / MMgc::GCHeap::kBlockSize;
    heap->SignalCodeMemoryAllocation(nblocks, true);
    return heap->Alloc(nblocks, MMgc::GCHeap::flags_Alloc, pagesize/MMgc::GCHeap::kBlockSize);
}

// Constraint: address must have been returned from AVMPI_allocateCodeMemory
// and nbytes must be the size of the allocation.  We can't quite check
// this, so we check that the address points to a page boundary and that
// the size is given as an integral number of VM pages and that the size
// corresponds to GCHeap's notion of the size.
//
// Usage note: on Posix, where the memory goes back into the common pool
// and isn't unmapped by the OS, it is very bad form for the client to
// free executable memory, we do not try to detect that (in DEBUG mode)
// but we probably should.
//
// This function should be the same as for generic Posix platforms, if you
// fix a bug here be sure to fix the bug there.

void AVMPI_freeCodeMemory(void* address, size_t nbytes)
{
    MMgc::GCHeap* heap = MMgc::GCHeap::GetGCHeap();
    size_t pagesize = VMPI_getVMPageSize();
    size_t nblocks = heap->Size(address);
    size_t actualBytes = nblocks * MMgc::GCHeap::kBlockSize;
    
    if ((uintptr_t)address % pagesize != 0 || nbytes % pagesize != 0 || nbytes != actualBytes) {
#ifdef DEBUG
        char buf[256];
        VMPI_snprintf(buf,
                      sizeof(buf),
                      "AVMPI_freeCodeMemory invariants violated: address=%llu provided=%llu actual=%llu\nAborting.\n",
                      (unsigned long long)(uintptr_t)address,
                      (unsigned long long)nbytes,
                      (unsigned long long)actualBytes);
        VMPI_log(buf);
#endif
        VMPI_abort();
    }
    
    heap->Free(address);
    heap->SignalCodeMemoryDeallocated(nblocks, true);
}

// Constraint: address must point into a block returned from AVMPI_allocateCodeMemory
// that has not been freed, it must point to a VM page boundary, and the number of
// bytes to protect must be an integral number of VM pages.  We can't check that
// the memory was returned from AVMPI_allocateCodeMemory though and we don't check
// that the memory is currently allocated.

void AVMPI_makeCodeMemoryExecutable(void *address, size_t nbytes, bool makeItSo)
{
    size_t pagesize = VMPI_getVMPageSize();
    
    if ((uintptr_t)address % pagesize != 0 || nbytes % pagesize != 0) {
#ifdef DEBUG
        char buf[256];
        VMPI_snprintf(buf,
                      sizeof(buf),
                      "AVMPI_makeCodeMemoryExecutable invariants violated: address=%llu size=%llu pagesize=%llu\nAborting.\n",
                      (unsigned long long)(uintptr_t)address,
                      (unsigned long long)nbytes,
                      (unsigned long long)pagesize);
        VMPI_log(buf);
#endif
        VMPI_abort();
    }
    
    DWORD oldProtectFlags = 0;
    DWORD newProtectFlags = 0;
    if ( makeItSo )
        newProtectFlags = PAGE_EXECUTE_READ;
    else
        newProtectFlags = PAGE_READWRITE;
    
    BOOL retval;
    do {
        MEMORY_BASIC_INFORMATION mbi;
        VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
        size_t markSize = nbytes > mbi.RegionSize ? mbi.RegionSize : nbytes;  // handle multiple adjoining regions
        
        retval = VirtualProtect(address, markSize, newProtectFlags, &oldProtectFlags);
        AvmAssert(retval != 0);
        
        address = (char*) address + markSize;
        nbytes -= markSize;
    } while(nbytes != 0 && retval != 0);
}
