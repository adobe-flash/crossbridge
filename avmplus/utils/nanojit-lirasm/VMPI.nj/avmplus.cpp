/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <signal.h>
#include "nanojit.h"

#ifdef SOLARIS
    typedef caddr_t maddr_ptr;
#else
    typedef void *maddr_ptr;
#endif

using namespace avmplus;

void
avmplus::AvmLog(char const *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    VMPI_vfprintf(stderr, msg, ap);
    va_end(ap);
}

#ifdef _DEBUG
namespace avmplus {
    void AvmAssertFail(const char* /* msg */) {
        fflush(stderr);
#if defined(WIN32)
        DebugBreak();
        exit(3);
#elif defined(__APPLE__)
        /*
         * On Mac OS X, Breakpad ignores signals. Only real Mach exceptions are
         * trapped.
         */
        *((int *) NULL) = 0;  /* To continue from here in GDB: "return" then "continue". */
        raise(SIGABRT);  /* In case above statement gets nixed by the optimizer. */
#else
        raise(SIGABRT);  /* To continue from here in GDB: "signal 0". */
#endif
    }
}
#endif

#if defined(WIN32)

void*
nanojit::CodeAlloc::allocCodeChunk(size_t nbytes) {
    return VirtualAlloc(NULL,
                        nbytes,
                        MEM_COMMIT | MEM_RESERVE,
                        PAGE_EXECUTE_READWRITE);
}

void
nanojit::CodeAlloc::freeCodeChunk(void *p, size_t) {
    VirtualFree(p, 0, MEM_RELEASE);
}

#elif defined(AVMPLUS_OS2)

void*
nanojit::CodeAlloc::allocCodeChunk(size_t nbytes) {

    // alloc from high memory, fallback to low memory if that fails
    void * addr;
    if (DosAllocMem(&addr, nbytes, OBJ_ANY |
                    PAG_COMMIT | PAG_READ | PAG_WRITE | PAG_EXECUTE)) {
        if (DosAllocMem(&addr, nbytes,
                        PAG_COMMIT | PAG_READ | PAG_WRITE | PAG_EXECUTE)) {
            return 0;
        }
    }
    return addr;
}

void
nanojit::CodeAlloc::freeCodeChunk(void *p, size_t nbytes) {
    DosFreeMem(p);
}

#elif defined(AVMPLUS_UNIX)

void*
nanojit::CodeAlloc::allocCodeChunk(size_t nbytes) {
    return mmap(NULL,
                nbytes,
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_PRIVATE | MAP_ANON,
                -1,
                0);
}

void
nanojit::CodeAlloc::freeCodeChunk(void *p, size_t nbytes) {
    munmap((maddr_ptr)p, nbytes);
}

#else // !WIN32 && !AVMPLUS_OS2 && !AVMPLUS_UNIX

void*
nanojit::CodeAlloc::allocCodeChunk(size_t nbytes) {
    void* mem = valloc(nbytes);
    VMPI_setPageProtection(mem, nbytes, true /* exec */, true /* write */);
    return mem;
}

void
nanojit::CodeAlloc::freeCodeChunk(void *p, size_t nbytes) {
    VMPI_setPageProtection(p, nbytes, false /* exec */, true /* write */);
    ::free(p);
}

#endif // WIN32

// All of the allocCodeChunk/freeCodeChunk implementations above allocate
// code memory as RWX and then free it, so the explicit page protection api's
// below are no-ops.

void
nanojit::CodeAlloc::markCodeChunkWrite(void*, size_t)
{}

void
nanojit::CodeAlloc::markCodeChunkExec(void*, size_t)
{}

bool
nanojit::CodeAlloc::checkChunkMark(void* /*addr*/, size_t /*nbytes*/, bool /*isExec*/) { 
    return true; // always correct 
}
