/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include <windows.h>
#include <winbase.h>

// disable warnings about "Inline asm assigning to FS:0 handler not registered as safe handler"
#pragma warning(disable:4733)

// Code from Intel that crashes a Cyrix CPU
#define CPUID   _asm _emit 0x0F _asm _emit 0xA2

#define CPUID_SSE2_FLAG 0x04000000 //; Is IA SSE2 bit (Bit 26 of EDX) in feature flags set
static BOOL gP4OsSupport = FALSE;

#ifndef _WIN64
EXCEPTION_DISPOSITION __cdecl MyExceptionHandlerSSE2(struct _EXCEPTION_RECORD * /*ExceptionRecord*/,
                                                 void * /*EstablisherFrame*/,
                                                 struct _CONTEXT *ContextRecord,
                                                 void * /*DispatcherContext*/)
{
    // Turn off the P4 OS support flag.
    gP4OsSupport = FALSE;

    // The offending P4 instruction is 3 bytes long.  Skip it and continue.
    ContextRecord->Eip += 3;
    return ExceptionContinueExecution;
}
#endif

bool P4Available()
{
#ifdef _M_AMD64
    // we support all this stuff
    return true;
#else
    long featureFlags = 0;
    //arun
    BOOL procType = 0;

// disable warnings about unreferenced _asm labels.
#pragma warning(disable:4102)

    _asm {
        push ecx
        push ebx

;       i386 CPU check
;       The AC bit, bit #18, is a new bit introduced in the EFLAGS
;       register on the i486 DX CPU to generate alignment faults.
;       This bit can not be set on the i386 CPU.
;
check_Intel386:
        pushfd
        pop             eax                     ; get original EFLAGS
        mov             ecx,eax                 ; save original EFLAGS
        xor             eax,40000h              ; flip AC bit in EFLAGS
        push    eax                     ; save for EFLAGS
        popfd                                   ; copy to EFLAGS
        pushfd                                  ; push EFLAGS
        pop             eax                     ; get new EFLAGS value
        xor             eax,ecx                 ; can't toggle AC bit, CPU=Intel386
        je              end_get_cpuid   ; CPU is i386,

;       i486 DX CPU / i487 SX MCP and i486 SX CPU checking
;
;       Checking for ability to set/clear ID flag (Bit 21) in EFLAGS
;       which indicates the presence of a processor
;       with the ability to use the CPUID instruction.
;
check_Intel486:
        pushfd                                  ; push original EFLAGS
        pop             eax                     ; get original EFLAGS in eax
        mov             ecx,eax                 ; save original EFLAGS in ecx
        xor             eax,200000h             ; flip ID bit in EFLAGS
        push    eax                     ; save for EFLAGS
        popfd                                   ; copy to EFLAGS
        pushfd                                  ; push EFLAGS
        pop             eax                     ; get new EFLAGS value
        xor     eax, ecx
        je              end_get_cpuid   ; CPU=486 without CPUID instruction functionality

;       Execute CPUID instruction to determine vendor, family,
;       model and stepping.  The use of the CPUID instruction used
;       in this program can be used for B0 and later steppings
;       of the P5 processor.

cpuid_data:
        // At this point we know we can do our CPUID instruction
        // We need some special code here to handle Cyrix buggy processors

        // Get our Vendors name out first
        mov         eax, 0
        CPUID
        //arun
        cmp ebx, 0x47656e75
        jz cyrix_version
        cmp edx, 0x696e6549
        jz cyrix_version
        cmp ecx, 0x6e74656c
        jz cyrix_version
        mov procType, 1
        jmp non_cyrix_version

cyrix_version:
        // EAX returns the highest value we can use as input into the
        // CPUID instruction.  If for some reason it returns 0, assume
        // no MMX support.  (Since we need to input 1 to query MMX)
        cmp eax, 0
        jz end_get_cpuid

        cmp ebx, 0x69727943 // This is "iryC", "Cyri" in memory
        jne non_cyrix_version

        // EAX now contains the stepping, model and family information
        and eax, 0x0FF0 // isolate model and family info
        cmp eax, 0x0520 // We're a 6x86, so there's no MMX
        je end_get_cpuid

non_cyrix_version:
        // Non Cyrix version
        mov         eax, 1 // Our input flag for our CPUID instruction
        CPUID
        mov featureFlags, edx ; save feature flags

end_get_cpuid:
        pop ecx
        pop ebx
    }

    BOOL bOsSupport = FALSE;
    BOOL bHwSupport = (featureFlags & CPUID_SSE2_FLAG);

    if(bHwSupport) {
        // Execute a KNI instruction and use Structured Exception Handling
        // to catch the exception if the OS does not support KNI.

        DWORD handler = (DWORD)MyExceptionHandlerSSE2;

        gP4OsSupport = TRUE;

        __asm { // Build EXCEPTION_REGISTRATION record:
            push handler    // Address of handler function
            push FS:[0]     // Address of previous handler
            mov FS:[0],ESP  // Install new EXECEPTION_REGISTRATION
        }

        // If so, test a KNI instruction and make sure you don't get
        // an exception (this tests OS support)
        __asm{
            pushad;
            //orpd xmm1,xmm1;   //Below are the op codes for this instruction
                                //emits will compile w/ MSVC 5.0 compiler
                                //You can comment these out and uncomment the
                                //orpd when using the Intel Compiler
            __emit 0x66
            __emit 0x0f
            __emit 0x56
            __emit 0xc9
            popad;
        }

        __asm {                 // Remove our EXECEPTION_REGISTRATION record
            mov eax,[ESP]       // Get pointer to previous record
            mov FS:[0], EAX     // Install previous record
            add esp, 8          // Clean our EXECEPTION_REGISTRATION off stack
        }

        bOsSupport = gP4OsSupport;
    }

    return bHwSupport && bOsSupport && procType;
#endif
}

